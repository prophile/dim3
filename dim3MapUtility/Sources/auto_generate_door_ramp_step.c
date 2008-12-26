/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto-Generate Doors, Steps, and Ramps

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

extern int map_auto_generate_random_int(int max);

extern bool map_auto_generate_mesh_start(map_type *map,int box_idx,int group_idx,int txt_idx,bool moveable,bool new_mesh);
extern void map_auto_generate_mesh_set_rot_offset(map_type *map,int x,int y,int z);
extern bool map_auto_generate_mesh_add_poly(map_type *map,int ptsz,int *x,int *y,int *z,float *gx,float *gy);

extern void map_auto_generate_poly_from_square_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_top_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor(int lx,int lz,int rx,int rz,int fy,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor_slant(int lx,int lz,int rx,int rz,int fy,int yadd,int lower_mode,bool reverse_slant,int *x,int *y,int *z,float *gx,float *gy);

extern int map_auto_generate_get_door_type(auto_generate_settings_type *ags);
extern bool map_auto_generate_has_door_type(auto_generate_settings_type *ags);

extern int								ag_box_count;

extern auto_generate_settings_type		ag_settings;
extern auto_generate_box_type			ag_boxes[max_ag_box];

/* =======================================================

      Portal to Poral Ramps
      
======================================================= */

void map_auto_generate_ramps_position(int *x,int *ex)
{
	int			mx,wid,split_factor,sz;

	wid=(*ex)-(*x);
	sz=wid>>1;

		// make sure ramp is at least a certain width
		
	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);

	if (sz<split_factor) {
		sz=split_factor;
		if (sz>wid) sz=wid;
	}

		// shift ramp to a side or middle

	switch (map_auto_generate_random_int(3)) {

			// ramp to left

		case 0:
			*ex=(*x)+sz;
			break;

			// ramp to right

		case 1:
			*x=(*ex)-sz;
			break;

			// ramp in middle

		default:
			mx=((*x)+(*ex))/2;
			sz=sz>>1;
			*x=mx-sz;
			*ex=mx+sz;
			break;

	}
}

void map_auto_generate_ramps(map_type *map)
{
	int							n,k,x,ex,z,ez,xsz,zsz,kx,kz,
								split_factor,high,ramp_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	auto_generate_box_type		*portal,*chk_portal;

		// how we split the walls into a mesh

	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);
	
		// create ramps

	for (n=0;n!=ag_box_count;n++) {

		portal=&ag_boxes[n];
		if (portal->corridor_flag!=ag_corridor_flag_portal) continue;
		
			// portal size

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;
		
			// check for touching portals
	
		for (k=0;k!=ag_box_count;k++) {
			if (k==n) continue;
			
			chk_portal=&ag_boxes[k];
			if (chk_portal->corridor_flag!=ag_corridor_flag_portal) continue;
			
				// only put ramps in rooms that are lower
				
			if (portal->max.y<=chk_portal->max.y) continue;

			high=portal->max.y-chk_portal->max.y;
			
				// get ramp size
				
			ramp_sz=(int)(((float)high)*ag_constant_ramp_size_percent);

				// portals touching top
				
			if (portal->min.z==chk_portal->max.z) {
				x=portal->min.x;
				if (chk_portal->min.x>x) x=chk_portal->min.x;
				ex=portal->max.x;
				if (chk_portal->max.x<ex) ex=chk_portal->max.x;
				if (x>ex) continue;

				x-=portal->min.x;
				ex-=portal->min.x;

				z=0;
				ez=z+ramp_sz;

					// portal wall

				if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;

				for (kx=x;kx<ex;kx+=split_factor) {
					map_auto_generate_poly_from_square_wall(kx,z,(kx+split_factor),z,chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}

					// ramp

				if (high>=ag_constant_ramp_min_high) {
					if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.ramp,FALSE,TRUE)) return;

					map_auto_generate_ramps_position(&x,&ex);
				
					map_auto_generate_poly_from_top_trig_wall(x,z,x,ez,chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
					map_auto_generate_poly_from_top_trig_wall(ex,z,ex,ez,chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
					
					map_auto_generate_poly_from_square_floor_slant(x,z,ex,ez,portal->max.y,high,ag_ceiling_lower_neg_z,FALSE,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}
				
				continue;
			}
			
				// portals touching bottom
				
			if (portal->max.z==chk_portal->min.z) {
				x=portal->min.x;
				if (chk_portal->min.x>x) x=chk_portal->min.x;
				ex=portal->max.x;
				if (chk_portal->max.x<ex) ex=chk_portal->max.x;
				if (x>ex) continue;
				
				x-=portal->min.x;
				ex-=portal->min.x;

				ez=portal->max.z-portal->min.z;
				z=ez-ramp_sz;

					// portal wall

				if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;
			
				for (kx=x;kx<ex;kx+=split_factor) {
					map_auto_generate_poly_from_square_wall(kx,ez,(kx+split_factor),ez,chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}

					// ramp

				if (high>=ag_constant_ramp_min_high) {
					if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.ramp,FALSE,TRUE)) return;

					map_auto_generate_ramps_position(&x,&ex);

					map_auto_generate_poly_from_top_trig_wall(x,ez,x,z,chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
					map_auto_generate_poly_from_top_trig_wall(ex,ez,ex,z,chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

					map_auto_generate_poly_from_square_floor_slant(x,z,ex,ez,portal->max.y,high,ag_ceiling_lower_pos_z,FALSE,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}
				
				continue;
			}
			
				// portals touching left
				
			if (portal->min.x==chk_portal->max.x) {
				z=portal->min.z;
				if (chk_portal->min.z>z) z=chk_portal->min.z;
				ez=portal->max.z;
				if (chk_portal->max.z<ez) ez=chk_portal->max.z;
				if (z>ez) continue;

				z-=portal->min.z;
				ez-=portal->min.z;

				x=0;
				ex=x+ramp_sz;

					// portal wall

				if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;

				for (kz=z;kz<ez;kz+=split_factor) {
					map_auto_generate_poly_from_square_wall(x,kz,x,(kz+split_factor),chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}

					// ramp

				if (high>=ag_constant_ramp_min_high) {
					if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.ramp,FALSE,TRUE)) return;

					map_auto_generate_ramps_position(&z,&ez);

					map_auto_generate_poly_from_top_trig_wall(x,z,ex,z,chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
					map_auto_generate_poly_from_top_trig_wall(x,ez,ex,ez,chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

					map_auto_generate_poly_from_square_floor_slant(x,z,ex,ez,portal->max.y,high,ag_ceiling_lower_neg_x,FALSE,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}
				
				continue;
			}
			
				// portals touching right
				
			if (portal->max.x==chk_portal->min.x) {
				z=portal->min.z;
				if (chk_portal->min.z>z) z=chk_portal->min.z;
				ez=portal->max.z;
				if (chk_portal->max.z<ez) ez=chk_portal->max.z;
				if (z>ez) continue;
				
				z-=portal->min.z;
				ez-=portal->min.z;

				ex=portal->max.x-portal->min.x;
				x=ex-ramp_sz;

					// portal wall

				if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;
				
				for (kz=z;kz<ez;kz+=split_factor) {
					map_auto_generate_poly_from_square_wall(ex,kz,ex,(kz+split_factor),chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}

					// ramp

				if (high>=ag_constant_ramp_min_high) {
					if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.ramp,FALSE,TRUE)) return;

					map_auto_generate_ramps_position(&z,&ez);

					map_auto_generate_poly_from_top_trig_wall(ex,z,x,z,chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
					map_auto_generate_poly_from_top_trig_wall(ex,ez,x,ez,chk_portal->max.y,portal->max.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

					map_auto_generate_poly_from_square_floor_slant(x,z,ex,ez,portal->max.y,high,ag_ceiling_lower_pos_x,FALSE,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}
				
				continue;
			}
			
		}
	
		portal++;
	}
}

/* =======================================================

      Steps
      
======================================================= */

int map_auto_generate_steps_get_length(int ty,int by,int step_size)
{
	int				step_cnt;
	
	step_cnt=(by-ty)/ag_constant_step_high;
	return(step_cnt*step_size);
}

void map_auto_generate_steps(map_type *map,int rn,int ty,int by,int stair_mode,int step_sz,bool back_wall,bool side_blocks,int lx,int rx,int lz,int rz)
{
	int				y,y2,step_cnt,px[8],py[8],pz[8];
	float			gx[8],gy[8];

		// create new mesh for steps

	if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.steps,FALSE,TRUE)) return;

		// get step size

	step_cnt=(by-ty)/ag_constant_step_high;

		// step back wall

	if (back_wall) {
		map_auto_generate_poly_from_square_wall(lx,lz,rx,rz,(ty+ag_constant_step_high),by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	}

		// create steps

	y=ty;

	while (TRUE) {
		y2=y+ag_constant_step_high;

		switch (stair_mode) {

			case ag_stair_neg_z:
				map_auto_generate_poly_from_square_wall(lx,(lz-step_sz),rx,(lz-step_sz),y,y2,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_floor(lx,(lz-step_sz),rx,lz,y,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				if (!side_blocks) {
					map_auto_generate_poly_from_square_wall(lx,(lz-step_sz),lx,lz,y,by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

					map_auto_generate_poly_from_square_wall(rx,(lz-step_sz),rx,lz,y,by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}

				lz-=step_sz;
				break;

			case ag_stair_pos_z:
				map_auto_generate_poly_from_square_wall(lx,(lz+step_sz),rx,(lz+step_sz),y,y2,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_floor(lx,lz,rx,(lz+step_sz),y,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				if (!side_blocks) {
					map_auto_generate_poly_from_square_wall(lx,lz,lx,(lz+step_sz),y,by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

					map_auto_generate_poly_from_square_wall(rx,lz,rx,(lz+step_sz),y,by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}

				lz+=step_sz;
				break;

			case ag_stair_neg_x:
				map_auto_generate_poly_from_square_wall((lx-step_sz),lz,(lx-step_sz),rz,y,y2,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_floor((lx-step_sz),lz,lx,rz,y,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				if (!side_blocks) {
					map_auto_generate_poly_from_square_wall((lx-step_sz),lz,lx,lz,y,by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

					map_auto_generate_poly_from_square_wall((lx-step_sz),rz,lx,rz,y,by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}

				lx-=step_sz;
				break;

			case ag_stair_pos_x:
				map_auto_generate_poly_from_square_wall((lx+step_sz),lz,(lx+step_sz),rz,y,y2,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_floor(lx,lz,(lx+step_sz),rz,y,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				if (!side_blocks) {
					map_auto_generate_poly_from_square_wall(lx,lz,(lx+step_sz),lz,y,by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

					map_auto_generate_poly_from_square_wall(lx,rz,(lx+step_sz),rz,y,by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}

				lx+=step_sz;
				break;
		}

		if (y2>=by) break;

		y=y2;
	}

		// step sides

	if (side_blocks) {

		y=ty-ag_constant_step_side_width;

		switch (stair_mode) {

			case ag_stair_neg_z:
			case ag_stair_pos_z:

				map_auto_generate_poly_from_square_wall(lx,lz,lx,rz,y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				map_auto_generate_poly_from_square_wall((lx-ag_constant_step_side_width),lz,(lx-ag_constant_step_side_width),rz,y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_wall((lx-ag_constant_step_side_width),lz,lx,lz,y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				map_auto_generate_poly_from_square_wall((lx-ag_constant_step_side_width),rz,lx,rz,y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				
				map_auto_generate_poly_from_square_floor((lx-ag_constant_step_side_width),lz,lx,rz,y,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_wall(rx,lz,rx,rz,y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				map_auto_generate_poly_from_square_wall((rx+ag_constant_step_side_width),lz,(rx+ag_constant_step_side_width),rz,y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_wall(rx,lz,(rx+ag_constant_step_side_width),lz,y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				map_auto_generate_poly_from_square_wall(rx,rz,(rx+ag_constant_step_side_width),rz,y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_floor(rx,lz,(rx+ag_constant_step_side_width),rz,y,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				break;

			case ag_stair_neg_x:
			case ag_stair_pos_x:
				map_auto_generate_poly_from_square_wall(lx,lz,rx,lz,y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				map_auto_generate_poly_from_square_wall(lx,(lz-ag_constant_step_side_width),rx,(lz-ag_constant_step_side_width),y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_wall(lx,(lz-ag_constant_step_side_width),lx,lz,y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				map_auto_generate_poly_from_square_wall(rx,(lz-ag_constant_step_side_width),rx,lz,y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_floor(lx,(lz-ag_constant_step_side_width),rx,lz,y,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_wall(lx,rz,rx,rz,y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				map_auto_generate_poly_from_square_wall(lx,(rz+ag_constant_step_side_width),rx,(rz+ag_constant_step_side_width),y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_wall(lx,rz,lx,(rz+ag_constant_step_side_width),y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				map_auto_generate_poly_from_square_wall(rx,rz,rx,(rz+ag_constant_step_side_width),y,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_floor(lx,rz,rx,(rz+ag_constant_step_side_width),y,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				break;

		}
	}
}

void map_auto_generate_corridor_to_portal_steps(map_type *map)
{
	int							n,k;
	auto_generate_box_type		*portal,*chk_portal;
	
	for (n=0;n!=ag_box_count;n++) {
	
		portal=&ag_boxes[n];
		if (portal->corridor_flag!=ag_corridor_flag_portal) continue;

			// find connecting corridors

		for (k=0;k!=ag_box_count;k++) {
			if (n==k) continue;
			
			chk_portal=&ag_boxes[k];
			if (chk_portal->corridor_flag==ag_corridor_flag_portal) continue;
			
			if (chk_portal->max.y>=portal->max.y) continue;

			if ((chk_portal->min.x>=portal->min.x) && (chk_portal->min.x<=portal->max.x)) {
				if (chk_portal->max.z==portal->min.z) map_auto_generate_steps(map,n,chk_portal->max.y,portal->max.y,ag_stair_pos_z,ag_constant_step_corridor_size,FALSE,TRUE,(chk_portal->min.x-portal->min.x),(chk_portal->max.x-portal->min.x),0,0);
				if (chk_portal->min.z==portal->max.z) map_auto_generate_steps(map,n,chk_portal->max.y,portal->max.y,ag_stair_neg_z,ag_constant_step_corridor_size,FALSE,TRUE,(chk_portal->min.x-portal->min.x),(chk_portal->max.x-portal->min.x),(portal->max.z-portal->min.z),(portal->max.z-portal->min.z));
			}
			
			if ((chk_portal->min.z>=portal->min.z) && (chk_portal->min.z<=portal->max.z)) {
				if (chk_portal->max.x==portal->min.x) map_auto_generate_steps(map,n,chk_portal->max.y,portal->max.y,ag_stair_pos_x,ag_constant_step_corridor_size,FALSE,TRUE,0,0,(chk_portal->min.z-portal->min.z),(chk_portal->max.z-portal->min.z));
				if (chk_portal->min.x==portal->max.x) map_auto_generate_steps(map,n,chk_portal->max.y,portal->max.y,ag_stair_neg_x,ag_constant_step_corridor_size,FALSE,TRUE,(portal->max.x-portal->min.x),(portal->max.x-portal->min.x),(chk_portal->min.z-portal->min.z),(chk_portal->max.z-portal->min.z));
			}
		}
	}
}

/* =======================================================

      Lifts
      
======================================================= */

void map_auto_generate_lift(map_type *map,int rn,int ty,int by,int lx,int rx,int lz,int rz)
{
	int						group_idx,movement_idx,move_idx,high,
							px[8],py[8],pz[8];
	float					gx[8],gy[8];
	char					name[256];
	movement_type			*movement;
	movement_move_type		*move;

		// name

	sprintf(name,"Lift_%d",rn);

		// group

	group_idx=map->ngroup;
	map->ngroup++;
		
	strcpy(map->groups[group_idx].name,name);

		// create new mesh for elevator

	high=(by-ty)-ag_constant_step_high;
	ty+=high;
	by+=high;

	if (!map_auto_generate_mesh_start(map,rn,group_idx,ag_settings.texture.steps,TRUE,TRUE)) return;

	map_auto_generate_poly_from_square_wall(lx,lz,rx,lz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,rz,rx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,lz,lx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(rx,lz,rx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(lx,lz,rx,rz,ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

		// create the movement

	movement_idx=map_movement_add(map);
	movement=&map->movements[movement_idx];
	
	strcpy(movement->name,name);
	movement->group_idx=group_idx;
	movement->auto_open_stand=TRUE;
		
	move_idx=map_movement_move_add(map,movement_idx);

	move=&movement->moves[move_idx];
	move->mov.y=-high;
	move->msec=ag_constant_lift_move_millisec;
	strcpy(move->sound_name,ag_settings.door_sound);
}

/* =======================================================

      Doors
      
======================================================= */

int map_auto_generate_doors_get_position(int sz)
{
	switch (map_auto_generate_random_int(3)) {

		case 0:
			return(ag_constant_door_width*2);

		case 1:
			return(sz-(ag_constant_door_width*3));

	}

	return(sz/2);
}

int map_auto_generate_doors_horizontal(map_type *map,int rn,int x,int lz,int rz,int rot_off_z,int ty,int by,char *name,char *ext_name)
{
	int			group_idx,px[8],py[8],pz[8];
	float		gx[8],gy[8];

		// group

	group_idx=map->ngroup;
	map->ngroup++;
		
	strcpy(map->groups[group_idx].name,name);
	if (ext_name!=NULL) strcat(map->groups[group_idx].name,ext_name);

		// mesh

	if (!map_auto_generate_mesh_start(map,rn,group_idx,ag_settings.texture.door,TRUE,TRUE)) return(-1);
	map_auto_generate_mesh_set_rot_offset(map,0,0,rot_off_z);

		// walls

	map_auto_generate_poly_from_square_wall(x,lz,x,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall((x+ag_constant_door_width),lz,(x+ag_constant_door_width),rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	
	map_auto_generate_poly_from_square_wall(x,lz,(x+ag_constant_door_width),lz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(x,rz,(x+ag_constant_door_width),rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

		// top and bottom

	map_auto_generate_poly_from_square_floor(x,lz,(x+ag_constant_door_width),rz,ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(x,lz,(x+ag_constant_door_width),rz,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	return(group_idx);
}

int map_auto_generate_doors_vertical(map_type *map,int rn,int z,int lx,int rx,int rot_off_x,int ty,int by,char *name,char *ext_name)
{
	int			group_idx,px[8],py[8],pz[8];
	float		gx[8],gy[8];

		// group

	group_idx=map->ngroup;
	map->ngroup++;
		
	strcpy(map->groups[group_idx].name,name);
	if (ext_name!=NULL) strcat(map->groups[group_idx].name,ext_name);

		// mesh

	if (!map_auto_generate_mesh_start(map,rn,group_idx,ag_settings.texture.door,TRUE,TRUE)) return(-1);
	map_auto_generate_mesh_set_rot_offset(map,rot_off_x,0,0);

		// walls

	map_auto_generate_poly_from_square_wall(lx,z,rx,z,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,(z+ag_constant_door_width),rx,(z+ag_constant_door_width),ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,z,lx,(z+ag_constant_door_width),ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	
	map_auto_generate_poly_from_square_wall(rx,z,rx,(z+ag_constant_door_width),ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	
		// top and bottom

	map_auto_generate_poly_from_square_floor(lx,z,rx,(z+ag_constant_door_width),ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(lx,z,rx,(z+ag_constant_door_width),by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	return(group_idx);
}

void map_auto_generate_doors(map_type *map)
{
	int						n,x,z,xsz,zsz,ty,by,x_rot_off,z_rot_off,
							door_cnt,group_idx,reverse_group_idx,
							door_type,movement_idx,move_idx;
	char					name[name_str_len];
	auto_generate_box_type	*portal;
	movement_type			*movement;
	movement_move_type		*move;
	
	if (!map_auto_generate_has_door_type(&ag_settings)) return;
	
		// run through corridors
		
	door_cnt=0;
		
	for (n=0;n!=ag_box_count;n++) {
	
		portal=&ag_boxes[n];
	
			// is this a corridor?
			
		if (portal->corridor_flag==ag_corridor_flag_portal) continue;
			
			// percentage correct?
				
		if (map_auto_generate_random_int(100)>(int)(ag_constant_door_percentage*100.0f)) continue;
		
			// door type

		door_type=map_auto_generate_get_door_type(&ag_settings);
			
			// enough groups and movements?
			
		if (map->ngroup>=max_group) return;
		if (map->nmovement>=max_movement) return;
		
			// group and movement name

		sprintf(name,"Door%.2d",(door_cnt+1));
		
			// door size
			
		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;

		ty=portal->min.y;
		by=portal->max.y;
		
			// create door mesh

		group_idx=reverse_group_idx=-1;
			
		if (portal->corridor_flag==ag_corridor_flag_horizontal) {
			x=map_auto_generate_doors_get_position(xsz);
			
			switch (door_type) {
				case ag_door_type_slide:
					group_idx=map_auto_generate_doors_horizontal(map,n,x,0,zsz,0,ty,by,name,NULL);
					break;
				case ag_door_type_split:
				case ag_door_type_swing:
					z_rot_off=zsz/6;
					group_idx=map_auto_generate_doors_horizontal(map,n,x,0,(zsz>>1),-z_rot_off,ty,by,name,"_1");
					reverse_group_idx=map_auto_generate_doors_horizontal(map,n,x,(zsz>>1),zsz,z_rot_off,ty,by,name,"_2");
					break;
			}

		}
		else {
			z=map_auto_generate_doors_get_position(zsz);

			switch (door_type) {
				case ag_door_type_slide:
					group_idx=map_auto_generate_doors_vertical(map,n,z,0,xsz,0,ty,by,name,NULL);
					break;
				case ag_door_type_split:
				case ag_door_type_swing:
					x_rot_off=xsz/6;
					group_idx=map_auto_generate_doors_vertical(map,n,z,0,(xsz>>1),-x_rot_off,ty,by,name,"_1");
					reverse_group_idx=map_auto_generate_doors_vertical(map,n,z,(xsz>>1),xsz,x_rot_off,ty,by,name,"_2");
					break;
			}
		}

		if (group_idx==-1) return;
		
			// create the movement
			
		movement_idx=map_movement_add(map);
		movement=&map->movements[movement_idx];
		
		strcpy(movement->name,name);
		movement->group_idx=group_idx;
		if (reverse_group_idx!=-1) movement->reverse_group_idx=reverse_group_idx;
		movement->auto_open=TRUE;
		movement->auto_open_distance=20000;
			
		move_idx=map_movement_move_add(map,movement_idx);
		move=&movement->moves[move_idx];
		
		switch (door_type) {

			case ag_door_type_slide:
				move->mov.y=-(portal->max.y-portal->min.y)-(map_enlarge>>1);
				break;

			case ag_door_type_split:
				if (portal->corridor_flag==ag_corridor_flag_horizontal) {
					move->mov.z=-((portal->max.z-portal->min.z)>>1)-(map_enlarge>>1);
				}
				else {
					move->mov.x=-((portal->max.x-portal->min.x)>>1)-(map_enlarge>>1);
				}
				break;

			case ag_door_type_swing:
				move->rot.y=90.0f;
				break;

		}

		move->msec=ag_constant_door_open_millisec;
		strcpy(move->sound_name,ag_settings.door_sound);
		
			// next door
			
		door_cnt++;
	}
}

