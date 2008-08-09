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
#include "walk_view.h"

extern map_type				map;

/* =======================================================

      Import Height Map Utility
      
======================================================= */

float import_height_map_get_height(bitmap_type *bitmap,int x,int z,int div_cnt)
{
	int			y;
	ptr			p;
	
		// find offset in portal
		
	x=(int)(((float)x/(float)div_cnt)*(float)bitmap->wid);
	y=(int)(((float)z/(float)div_cnt)*(float)bitmap->high);
	
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

void import_height_map_add_wall(import_height_map_settings_type *hmi_settings,int mesh_idx,int lx,int lz,int rx,int rz,int ty,int by)
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

	map_mesh_add_poly(&map,mesh_idx,4,kx,ky,kz,gx,gy,hmi_settings->texture_portal_wall);
}

int import_height_map_get_texture_idx(import_height_map_settings_type *hmi_settings,int *ky)
{
	int					y,high;
	
		// calculate center
		
	y=(ky[0]+ky[1]+ky[2])/3;
	
		// get floor separations
		
	high=(hmi_settings->high)/3;
	
		// get texture
				
	switch (abs(y-(map_max_size>>1))/high) {
	
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
	int					x,z,px,pz,txt_idx,div_cnt,div_sz,total_sz,
						lx,rx,tz,bz,kx[4],ky[4],kz[4],y[4],ty,by,
						mesh_idx;
	float				f_portal_y_sz,gx[4],gy[4];
	bitmap_type			bitmap;
	
		// get the png
		
	if (!bitmap_open(&bitmap,hmi_settings->path,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE)) return;
	
		// clear the map
		
	map_auto_generate_clear(&map);
	
		// division and sizes
		
	total_sz=hmi_settings->total_sz;
	div_cnt=hmi_settings->div_cnt;
	div_sz=total_sz/div_cnt;
	
	f_portal_y_sz=(float)hmi_settings->high;
	
		// floor polygons
		
	mesh_idx=map_mesh_add(&map);
	if (mesh_idx==-1) {
		bitmap_close(&bitmap);
		return;
	}
	
	for (z=0;z!=div_cnt;z++) {
		for (x=0;x!=div_cnt;x++) {
			
			px=((map_max_size/2)-(total_sz/2))+(x*div_sz);
			pz=((map_max_size/2)-(total_sz/2))+(z*div_sz);
			
				// floors
				
			y[0]=(map_max_size>>1)-(int)(f_portal_y_sz*import_height_map_get_height(&bitmap,x,z,div_cnt));
			y[1]=(map_max_size>>1)-(int)(f_portal_y_sz*import_height_map_get_height(&bitmap,(x+1),z,div_cnt));
			y[2]=(map_max_size>>1)-(int)(f_portal_y_sz*import_height_map_get_height(&bitmap,(x+1),(z+1),div_cnt));
			y[3]=(map_max_size>>1)-(int)(f_portal_y_sz*import_height_map_get_height(&bitmap,x,(z+1),div_cnt));
										
			gx[0]=gx[1]=gx[2]=0.0f;
			gy[0]=gy[1]=gy[2]=0.0f;
					
			if (((x+z)&0x1)!=0) {
				kx[0]=px;
				kx[1]=kx[2]=px+div_sz;
				kz[0]=kz[1]=pz;
				kz[2]=pz+div_sz;
				ky[0]=y[0];
				ky[1]=y[1];
				ky[2]=y[2];
				
				txt_idx=import_height_map_get_texture_idx(hmi_settings,ky);
				map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
			
				kx[0]=kx[2]=px;
				kx[1]=px+div_sz;
				kz[0]=pz;
				kz[1]=kz[2]=pz+div_sz;
				ky[0]=y[0];
				ky[1]=y[2];
				ky[2]=y[3];
				
				txt_idx=import_height_map_get_texture_idx(hmi_settings,ky);
				map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
			}
			else {
				kx[0]=kx[2]=px;
				kx[1]=px+div_sz;
				kz[0]=kz[1]=pz;
				kz[2]=pz+div_sz;
				ky[0]=y[0];
				ky[1]=y[1];
				ky[2]=y[3];
				
				txt_idx=import_height_map_get_texture_idx(hmi_settings,ky);
				map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
			
				kx[0]=kx[1]=px+div_sz;
				kx[2]=px;
				kz[0]=pz;
				kz[1]=kz[2]=pz+div_sz;
				ky[0]=y[1];
				ky[1]=y[2];
				ky[2]=y[3];
				
				txt_idx=import_height_map_get_texture_idx(hmi_settings,ky);
				map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
			}
		}
	}
	
		// walls
		
	if (hmi_settings->wall) {
	
		mesh_idx=map_mesh_add(&map);
		if (mesh_idx==-1) {
			bitmap_close(&bitmap);
			return;
		}
		
		by=(map_max_size>>1);
		ty=by-hmi_settings->high;
		
		lx=((map_max_size/2)-(total_sz/2));
		rx=lx+total_sz;

		for (z=0;z!=div_cnt;z++) {
			pz=((map_max_size/2)-(total_sz/2))+(z*div_sz);
			import_height_map_add_wall(hmi_settings,mesh_idx,lx,pz,lx,(pz+div_sz),ty,by);
			import_height_map_add_wall(hmi_settings,mesh_idx,rx,pz,rx,(pz+div_sz),ty,by);
		}
		
		tz=((map_max_size/2)-(total_sz/2));
		bz=tz+total_sz;

		for (x=0;x!=div_cnt;x++) {
			px=((map_max_size/2)-(total_sz/2))+(x*div_sz);
			import_height_map_add_wall(hmi_settings,mesh_idx,px,tz,(px+div_sz),tz,ty,by);
			import_height_map_add_wall(hmi_settings,mesh_idx,px,bz,(px+div_sz),bz,ty,by);
		}
	}
	
		// reset UVs
				
	map_mesh_reset_uv(&map,mesh_idx);
		
	bitmap_close(&bitmap);
	
		// lights
		
	if (hmi_settings->lights) map_auto_generate_add_simple_lights(&map);
	
		// create player spot
		
	map_auto_generate_add_player_spot(&map);
}


