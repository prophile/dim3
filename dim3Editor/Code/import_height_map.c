/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Height Map Import Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "interface.h"
#include "dialog.h"
#include "import.h"
#include "common_view.h"
#include "portal_view.h"
#include "top_view.h"
#include "walk_view.h"

extern int					cr;
extern map_type				map;

/* =======================================================

      Import Height Map Utility
      
======================================================= */

float import_height_map_get_height(bitmap_type *bitmap,int portal_x_start,int portal_z_start,int portal_total_x_sz,int portal_total_z_sz,portal_type *portal,int seg_x,int seg_z)
{
	int			x,z,y;
	ptr			p;
	
		// find offset in portal
		
	x=(portal->x-portal_x_start)+seg_x;
	z=(portal->z-portal_z_start)+seg_z;
	
	x=(int)(((float)x/(float)portal_total_x_sz)*(float)bitmap->wid);
	y=(int)(((float)z/(float)portal_total_z_sz)*(float)bitmap->high);
	
	if (x<0) x=0;
	if (x>=bitmap->wid) x=bitmap->wid-1;
	if (y<0) y=0;
	if (y>=bitmap->high) y=bitmap->high-1;
	
	p=bitmap->data+((y*(bitmap->wid<<2))+(x<<2));
	
	return(((float)*p)/255.0f);
}

/* =======================================================

      Add Walls and Floors
      
======================================================= */

void import_height_map_walls_add_segment(int rn,int lx,int lz,int rx,int rz,int ty,int by)
{
	segment_type	*seg;
	
	if (map.nsegment==max_segment) return;
	
	seg=&map.segments[map.nsegment];
	
	seg->type=sg_wall;
	seg->rn=rn;
	seg->fill=0;
    seg->clip=wc_none;
    seg->curve=cv_none;
	seg->x_txtoff=seg->y_txtoff=0;
	seg->x_txtfact=seg->y_txtfact=1;
    seg->x_shift=seg->y_shift=0;
	seg->txt_ang=0;
	seg->dark_factor=1.0f;
    seg->alpha=1.0f;
	seg->group_idx=-1;
	seg->primitive_uid[0]=-1;
    seg->pass_through=FALSE;
	seg->moveable=FALSE;
	seg->lock=FALSE;
	seg->on=TRUE;
	
	seg->data.wall.lx=lx;
	seg->data.wall.rx=rx;
	seg->data.wall.lz=lz;
	seg->data.wall.rz=rz;
	seg->data.wall.ty=ty;
	seg->data.wall.by=by;

	map.nsegment++;
}

/* =======================================================

      Import Height Map Segment Fills
      
======================================================= */

void import_height_map_fix_segments_fill(import_height_map_settings_type *hmi_settings)
{
	int					i,x,y,z,high;
	segment_type		*seg;
	
		// get floor separations
		
	high=(hmi_settings->portal_by-hmi_settings->portal_ty)/3;
	
		// set the textures
	
	seg=map.segments;
	
	for (i=0;i!=map.nsegment;i++) {
	
		switch (seg->type) {
		
			case sg_wall:
				seg->fill=hmi_settings->texture_portal_wall;
				break;
				
			case sg_floor:
				map_segment_calculate_center(&map,i,&x,&y,&z);
				
				switch (abs(y-hmi_settings->portal_ty)/high) {
					case 0:
						seg->fill=hmi_settings->texture_portal_high_floor;
						break;
					case 1:
						seg->fill=hmi_settings->texture_portal_mid_floor;
						break;
					default:
						seg->fill=hmi_settings->texture_portal_low_floor;
						break;
				}
				break;
		}
		
		seg++;
	}
}

/* =======================================================

      Import Height Map
      
======================================================= */

void map_import_height_map(import_height_map_settings_type *hmi_settings)
{
	int					rn,x,z,sx,sz,px,pz,pex,pez,
						lx,rx,tz,bz,y[4],ty,by,
						portal_x_start,portal_z_start,
						portal_total_x_sz,portal_total_z_sz;
	float				f_portal_y_sz;
	bitmap_type			bitmap;
	portal_type			*portal;
	
		// get the png
		
	if (!bitmap_open(&bitmap,hmi_settings->path,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE)) return;
	
		// clear the map
		
	map_auto_generate_clear(&map);
	
		// get the portal sizes
		
	portal_total_x_sz=hmi_settings->portal_x_sz*hmi_settings->portal_x_cnt;
	portal_total_z_sz=hmi_settings->portal_z_sz*hmi_settings->portal_z_cnt;
	
	f_portal_y_sz=(float)(hmi_settings->portal_by-hmi_settings->portal_ty);
	
	portal_x_start=(map_x_size/2)-((map_x_size-portal_total_x_sz)/2);
	portal_z_start=(map_z_size/2)-((map_z_size-portal_total_z_sz)/2);
	
		// create portals
	
	for (z=0;z!=hmi_settings->portal_z_cnt;z++) {
		for (x=0;x!=hmi_settings->portal_x_cnt;x++) {
			
				// portal
				
			px=portal_x_start+(x*hmi_settings->portal_x_sz);
			pz=portal_z_start+(z*hmi_settings->portal_z_sz);
			pex=px+hmi_settings->portal_x_sz;
			pez=pz+hmi_settings->portal_z_sz;
			
			rn=map_portal_create(&map,px,pz,pex,pez);
			if (rn==-1) break;
			
			portal=&map.portals[rn];
			
				// floors
				
			for (sz=0;sz!=10;sz++) {
			
				tz=((hmi_settings->portal_z_sz)/10)*sz;
				bz=tz+(hmi_settings->portal_z_sz/10);
				
				for (sx=0;sx!=10;sx++) {
				
					lx=((hmi_settings->portal_x_sz)/10)*sx;
					rx=lx+(hmi_settings->portal_x_sz/10);
					
					y[0]=hmi_settings->portal_by-(int)(f_portal_y_sz*import_height_map_get_height(&bitmap,portal_x_start,portal_z_start,portal_total_x_sz,portal_total_z_sz,portal,lx,tz));
					y[1]=hmi_settings->portal_by-(int)(f_portal_y_sz*import_height_map_get_height(&bitmap,portal_x_start,portal_z_start,portal_total_x_sz,portal_total_z_sz,portal,rx,tz));
					y[2]=hmi_settings->portal_by-(int)(f_portal_y_sz*import_height_map_get_height(&bitmap,portal_x_start,portal_z_start,portal_total_x_sz,portal_total_z_sz,portal,rx,bz));
					y[3]=hmi_settings->portal_by-(int)(f_portal_y_sz*import_height_map_get_height(&bitmap,portal_x_start,portal_z_start,portal_total_x_sz,portal_total_z_sz,portal,lx,bz));
					
					cr=rn;
					
					if (hmi_settings->import_type==hm_import_quads) {
						segment_add_floor_quad(lx,tz,y[0],rx,tz,y[1],rx,bz,y[2],lx,bz,y[3],-1,FALSE);
					}
					else {
						if (((sx+sz)&0x1)!=0) {
							segment_add_floor_trig(lx,tz,y[0],rx,tz,y[1],rx,bz,y[2],-1,FALSE);
							segment_add_floor_trig(lx,tz,y[0],rx,bz,y[2],lx,bz,y[3],-1,FALSE);
						}
						else {
							segment_add_floor_trig(lx,tz,y[0],rx,tz,y[1],lx,bz,y[3],-1,FALSE);
							segment_add_floor_trig(rx,tz,y[1],rx,bz,y[2],lx,bz,y[3],-1,FALSE);
						}
					}
				}
			}
			
				// walls
				
			if (hmi_settings->wall) {
			
				ty=hmi_settings->portal_ty-hmi_settings->wall_high;
				by=hmi_settings->portal_ty-1;
					
				if (portal->x==portal_x_start) {
					for (sz=0;sz!=10;sz++) {
						tz=((hmi_settings->portal_z_sz)/10)*sz;
						bz=tz+(hmi_settings->portal_z_sz/10);
						import_height_map_walls_add_segment(rn,0,tz,0,bz,ty,by);
					}
				}
				
				if (portal->ex==(portal_x_start+portal_total_x_sz)) {
					for (sz=0;sz!=10;sz++) {
						tz=((hmi_settings->portal_z_sz)/10)*sz;
						bz=tz+(hmi_settings->portal_z_sz/10);
						import_height_map_walls_add_segment(rn,hmi_settings->portal_x_sz,tz,hmi_settings->portal_x_sz,bz,ty,by);
					}
				}
				
				if (portal->z==portal_z_start) {
					for (sx=0;sx!=10;sx++) {
						lx=((hmi_settings->portal_x_sz)/10)*sx;
						rx=lx+(hmi_settings->portal_x_sz/10);
						import_height_map_walls_add_segment(rn,lx,0,rx,0,ty,by);
					}
				}
				
				if (portal->ez==(portal_z_start+portal_total_z_sz)) {
					for (sx=0;sx!=10;sx++) {
						lx=((hmi_settings->portal_x_sz)/10)*sx;
						rx=lx+(hmi_settings->portal_x_sz/10);
						import_height_map_walls_add_segment(rn,lx,hmi_settings->portal_z_sz,rx,hmi_settings->portal_z_sz,ty,by);
					}
				}
				
			}
		}
	}
		
	bitmap_close(&bitmap);
	
		// fix segments
		
	map_auto_generate_fix_segments_uv(&map);
	import_height_map_fix_segments_fill(hmi_settings);
	
		// lights and sight paths
		
	if (hmi_settings->lights) map_auto_generate_add_simple_lights(&map);
	if (hmi_settings->sight_path) map_portal_sight_generate_paths(&map,hmi_settings->sight_path_all);
	
		// create player spot
		
	map_auto_generate_add_player_spot(&map);
}


