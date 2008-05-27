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
#include "walk_view.h"

#define import_height_map_start_y			50000

extern int					cr;
extern map_type				map;

/* =======================================================

      Import Height Map Utility
      
======================================================= */

float import_height_map_get_height(bitmap_type *bitmap,int portal_start,int portal_total_sz,portal_type *portal,int seg_x,int seg_z)
{
	int			x,z,y;
	ptr			p;
	
		// find offset in portal
		
	x=(portal->x-portal_start)+seg_x;
	z=(portal->z-portal_start)+seg_z;
	
	x=(int)(((float)x/(float)portal_total_sz)*(float)bitmap->wid);
	y=(int)(((float)z/(float)portal_total_sz)*(float)bitmap->high);
	
	if (x<0) x=0;
	if (x>=bitmap->wid) x=bitmap->wid-1;
	if (y<0) y=0;
	if (y>=bitmap->high) y=bitmap->high-1;
	
	p=bitmap->data+((y*(bitmap->wid<<2))+(x<<2));
	
	return(((float)*p)/255.0f);

}

/* =======================================================

      Walls and Floors
      
======================================================= */

void import_height_map_add_wall(import_height_map_settings_type *hmi_settings,int rn,int mesh_idx,int lx,int lz,int rx,int rz,int ty,int by)
{
	int				kx[4],ky[4],kz[4];
	float			gx[4],gy[4];

	kx[0]=kx[3]=lx;
	kx[1]=kx[2]=rx;
	kz[0]=kz[3]=lz;
	kz[1]=kz[2]=rz;
	ky[0]=ky[1]=ty;
	ky[2]=ky[3]=by;
	
	gx[0]=gx[1]=gx[2]=gx[3]=0.0f;
	gy[0]=gy[1]=gy[2]=gy[3]=0.0f;

	map_portal_mesh_add_poly(&map,rn,mesh_idx,4,kx,ky,kz,gx,gy,hmi_settings->texture_portal_wall);
}

int import_height_map_get_texture_idx(import_height_map_settings_type *hmi_settings,int *ky)
{
	int					y,high;
	
		// calculate center
		
	y=(ky[0]+ky[1]+ky[2])/3;
	
		// get floor separations
		
	high=(hmi_settings->portal_high)/3;
	
		// get texture
				
	switch (abs(y-import_height_map_start_y)/high) {
	
		case 0:
			return(hmi_settings->texture_portal_low_floor);

		case 1:
			return(hmi_settings->texture_portal_mid_floor);

	}
	
	return(hmi_settings->texture_portal_high_floor);
}


/* =======================================================

      Import Height Map
      
======================================================= */

void map_import_height_map(import_height_map_settings_type *hmi_settings)
{
	int					rn,x,z,sx,sz,px,pz,pex,pez,txt_idx,
						lx,rx,tz,bz,kx[4],ky[4],kz[4],y[4],ty,by,
						portal_cnt,portal_start,portal_total_sz,
						mesh_idx;
	float				f_portal_y_sz,gx[4],gy[4];
	bitmap_type			bitmap;
	portal_type			*portal;
	
		// get the png
		
	if (!bitmap_open(&bitmap,hmi_settings->path,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE)) return;
	
		// clear the map
		
	map_auto_generate_clear(&map);
	
		// portal counts need to be sqrts
		
	portal_cnt=(int)sqrt(hmi_settings->portal_cnt);
	if (portal_cnt<1) portal_cnt=1;
	
		// get the portal sizes
		
	portal_total_sz=hmi_settings->portal_sz*portal_cnt;
	
	f_portal_y_sz=(float)hmi_settings->portal_high;
	
	portal_start=(map_max_size/2)-((map_max_size-portal_total_sz)/2);
	
		// create portals
	
	for (z=0;z!=hmi_settings->portal_cnt;z++) {
		for (x=0;x!=hmi_settings->portal_cnt;x++) {
			
				// portal
				
			px=portal_start+(x*hmi_settings->portal_sz);
			pz=portal_start+(z*hmi_settings->portal_sz);
			pex=px+hmi_settings->portal_sz;
			pez=pz+hmi_settings->portal_sz;
			
			rn=map_portal_create(&map,px,pz,pex,pez);
			if (rn==-1) break;
			
			portal=&map.portals[rn];
			
				// mesh for this portal
				
			mesh_idx=map_portal_mesh_add(&map,rn);
			if (mesh_idx==-1) break;
			
				// floors
				
			for (sz=0;sz!=10;sz++) {
			
				tz=((hmi_settings->portal_sz)/10)*sz;
				bz=tz+(hmi_settings->portal_sz/10);
				
				for (sx=0;sx!=10;sx++) {
				
					lx=((hmi_settings->portal_sz)/10)*sx;
					rx=lx+(hmi_settings->portal_sz/10);
					
					y[0]=import_height_map_start_y-(int)(f_portal_y_sz*import_height_map_get_height(&bitmap,portal_start,portal_total_sz,portal,lx,tz));
					y[1]=import_height_map_start_y-(int)(f_portal_y_sz*import_height_map_get_height(&bitmap,portal_start,portal_total_sz,portal,rx,tz));
					y[2]=import_height_map_start_y-(int)(f_portal_y_sz*import_height_map_get_height(&bitmap,portal_start,portal_total_sz,portal,rx,bz));
					y[3]=import_height_map_start_y-(int)(f_portal_y_sz*import_height_map_get_height(&bitmap,portal_start,portal_total_sz,portal,lx,bz));
					
					cr=rn;
					
					gx[0]=gx[1]=gx[2]=0.0f;
					gy[0]=gy[1]=gy[2]=0.0f;
					
					if (((sx+sz)&0x1)!=0) {
						kx[0]=lx;
						kx[1]=kx[2]=rx;
						kz[0]=kz[1]=tz;
						kz[2]=bz;
						ky[0]=y[0];
						ky[1]=y[1];
						ky[2]=y[2];
						
						txt_idx=import_height_map_get_texture_idx(hmi_settings,ky);
						map_portal_mesh_add_poly(&map,rn,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
					
						kx[0]=kx[2]=lx;
						kx[1]=rx;
						kz[0]=tz;
						kz[1]=kz[2]=bz;
						ky[0]=y[0];
						ky[1]=y[2];
						ky[2]=y[3];
						
						txt_idx=import_height_map_get_texture_idx(hmi_settings,ky);
						map_portal_mesh_add_poly(&map,rn,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
					}
					else {
						kx[0]=kx[2]=lx;
						kx[1]=rx;
						kz[0]=kz[1]=tz;
						kz[2]=bz;
						ky[0]=y[0];
						ky[1]=y[1];
						ky[2]=y[3];
						
						txt_idx=import_height_map_get_texture_idx(hmi_settings,ky);
						map_portal_mesh_add_poly(&map,rn,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
					
						kx[0]=kx[1]=rx;
						kx[2]=lx;
						kz[0]=tz;
						kz[1]=kz[2]=bz;
						ky[0]=y[1];
						ky[1]=y[2];
						ky[2]=y[3];
						
						txt_idx=import_height_map_get_texture_idx(hmi_settings,ky);
						map_portal_mesh_add_poly(&map,rn,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
					}
				}
			}
			
				// walls

			if (hmi_settings->wall) {

				by=import_height_map_start_y;
				ty=by-hmi_settings->portal_high;
					
				if (portal->x==portal_start) {
					for (sz=0;sz!=10;sz++) {
						tz=((hmi_settings->portal_sz)/10)*sz;
						bz=tz+(hmi_settings->portal_sz/10);
						import_height_map_add_wall(hmi_settings,rn,mesh_idx,0,tz,0,bz,ty,by);
					}
				}
				
				if (portal->ex==(portal_start+portal_total_sz)) {
					for (sz=0;sz!=10;sz++) {
						tz=((hmi_settings->portal_sz)/10)*sz;
						bz=tz+(hmi_settings->portal_sz/10);
						import_height_map_add_wall(hmi_settings,rn,mesh_idx,hmi_settings->portal_sz,tz,hmi_settings->portal_sz,bz,ty,by);
					}
				}
				
				if (portal->z==portal_start) {
					for (sx=0;sx!=10;sx++) {
						lx=((hmi_settings->portal_sz)/10)*sx;
						rx=lx+(hmi_settings->portal_sz/10);
						import_height_map_add_wall(hmi_settings,rn,mesh_idx,lx,0,rx,0,ty,by);
					}
				}
				
				if (portal->ez==(portal_start+portal_total_sz)) {
					for (sx=0;sx!=10;sx++) {
						lx=((hmi_settings->portal_sz)/10)*sx;
						rx=lx+(hmi_settings->portal_sz/10);
						import_height_map_add_wall(hmi_settings,rn,mesh_idx,lx,hmi_settings->portal_sz,rx,hmi_settings->portal_sz,ty,by);
					}
				}

			}
			
				// reset UVs
				
			map_portal_mesh_reset_uv(&map,rn,mesh_idx);
		}
	}
		
	bitmap_close(&bitmap);
	
		// lights and sight paths
		
	if (hmi_settings->lights) map_auto_generate_add_simple_lights(&map);
	map_portal_sight_generate_paths(&map,TRUE);
	
		// create player spot
		
	map_auto_generate_add_player_spot(&map);
}


