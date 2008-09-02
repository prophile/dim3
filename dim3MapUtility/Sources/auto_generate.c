/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto-Generate Map Routines

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

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

extern void map_auto_generate_random_seed(int seed);
extern int map_auto_generate_random_int(int max);

extern bool map_auto_generate_portal_collision(int x,int z,int ex,int ez,int skip_idx);
extern bool map_auto_generate_portal_horz_edge_block(int skip_portal_idx,int z,int ez,int x);
extern bool map_auto_generate_portal_vert_edge_block(int skip_portal_idx,int x,int ex,int z);
extern bool map_auto_generate_portal_horz_edge_touch(int skip_portal_idx,int z,int ez,int x);
extern bool map_auto_generate_portal_vert_edge_touch(int skip_portal_idx,int x,int ex,int z);
extern bool map_auto_generate_portal_touching_left(int portal_idx);
extern bool map_auto_generate_portal_touching_right(int portal_idx);
extern bool map_auto_generate_portal_touching_top(int portal_idx);
extern bool map_auto_generate_portal_touching_bottom(int portal_idx);
extern bool map_auto_generate_portal_touching_any(int portal_idx);

extern int map_auto_generate_get_floor_type(auto_generate_settings_type *ags);
extern int map_auto_generate_get_ceiling_type(auto_generate_settings_type *ags);
extern int map_auto_generate_get_corridor_type(auto_generate_settings_type *ags);

extern bool map_auto_generate_block_collision(auto_generate_settings_type *ags,int x,int z,int ex,int ez);

extern void map_auto_generate_story_extra_floor(bool *lft,bool *rgt,bool *top,bool *bot,bool *horz,bool *vert);

extern void map_auto_generate_poly_from_square_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_top_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_bot_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor(int lx,int lz,int rx,int rz,int fy,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor_slant(int lx,int lz,int rx,int rz,int fy,int yadd,int lower_mode,bool reverse_slant,int *x,int *y,int *z,float *gx,float *gy);

extern bool map_auto_generate_mesh_start(map_type *map,int box_idx,int group_idx,int txt_idx,bool moveable,bool new_mesh);
extern void map_auto_generate_mesh_change_texture(int txt_idx);
extern bool map_auto_generate_mesh_add_poly(map_type *map,int ptsz,int *x,int *y,int *z,float *gx,float *gy);

extern int map_auto_generate_steps_get_length(int ty,int by,int step_size);
extern void map_auto_generate_steps(map_type *map,int rn,int ty,int by,int stair_mode,int step_sz,bool top_step_wall,bool back_wall,int lx,int rx,int lz,int rz);

extern void map_auto_generate_lights(map_type *map);
extern void map_auto_generate_spots(map_type *map);
extern void map_auto_generate_ramps(map_type *map);
extern void map_auto_generate_corridor_to_portal_steps(map_type *map);
extern void map_auto_generate_doors(map_type *map);

int									ag_box_count;

unsigned char						ag_ceiling_data[ag_ceiling_type_count][11]=ag_ceiling_data_bytes,
									ag_ceiling_wall_data[ag_ceiling_type_count][4]=ag_ceiling_wall_data_bytes;

unsigned char						corridor_types[max_portal],
									ceiling_types[max_portal];

auto_generate_settings_type			ag_settings;
auto_generate_box_type				ag_boxes[max_ag_box];

/* =======================================================

      Generate Initial Unconnected Portals
      
======================================================= */

void map_auto_generate_initial_portals(map_type *map)
{
	int			i,x,z,ex,ez,initial_count,try_count,split_factor,
				map_x_sz,map_z_sz,portal_rand_sz,portal_min_sz;

		// sizes

	map_x_sz=ag_settings.map.right-ag_settings.map.left;
	map_z_sz=ag_settings.map.bottom-ag_settings.map.top;

	portal_rand_sz=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_random_percent);
	portal_min_sz=ag_settings.map.portal_sz-portal_rand_sz;
	
	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);
	
		// initial count of portals
	
	initial_count=(ag_settings.map.map_sz/ag_settings.map.portal_sz)*4;

		// create portals

	ag_box_count=0;

	for (i=0;i!=initial_count;i++) {
		
		try_count=0;

		while (TRUE) {

				// create portal start

			x=ag_settings.map.left+map_auto_generate_random_int(map_x_sz);
			z=ag_settings.map.top+map_auto_generate_random_int(map_z_sz);
			
				// create portal size
				
			ex=x+(portal_min_sz+map_auto_generate_random_int(portal_rand_sz));
			ez=z+(portal_min_sz+map_auto_generate_random_int(portal_rand_sz));

				// fix right/bottom overflows

			if (ex>ag_settings.map.right) {
				x=ag_settings.map.right-(ex-x);
				ex=ag_settings.map.right;
			}
			if (ez>ag_settings.map.bottom) {
				z=ag_settings.map.bottom-(ez-z);
				ez=ag_settings.map.bottom;
			}
	
				// use splits as grid
				
			x/=split_factor;
			x*=split_factor;
			
			z/=split_factor;
			z*=split_factor;
			
			ex/=split_factor;
			ex*=split_factor;
			
			ez/=split_factor;
			ez*=split_factor;

				// check for collisions

			if ((!map_auto_generate_block_collision(&ag_settings,x,z,ex,ez)) && (!map_auto_generate_portal_collision(x,z,ex,ez,-1))) break;
			
				// try to much?
				
			try_count++;
			if (try_count==100) return;
		}

			// create portal

		ag_boxes[ag_box_count].min.x=x;
		ag_boxes[ag_box_count].max.x=ex;
		ag_boxes[ag_box_count].min.z=z;
		ag_boxes[ag_box_count].max.z=ez;
		
		ag_boxes[ag_box_count].corridor_flag=ag_corridor_flag_portal;
		ag_boxes[ag_box_count].mesh_idx=map_mesh_add(map);

		ag_box_count++;
	}
}

/* =======================================================

      Merge Close Portals
      
======================================================= */

void map_auto_generate_merge_portals(void)
{
	int						i,n,k,dist,portal_merge_distance,
							split_factor,merge_try_count;
	bool					moved;
	auto_generate_box_type	*chk_portal,*merge_portal;

	portal_merge_distance=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_merge_percent);

		// attempt to merge portals within a certain distance
		// together.  All portals are grided on the split
		// factor, so we merge on that

		// only keep moving for a limited time
		// so portals that bounce between two don't cause
		// an infinite loop
		
	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);
	merge_try_count=(portal_merge_distance/split_factor)*2;

	for (i=0;i!=merge_try_count;i++) {

		moved=FALSE;		// keep looping until no more merging

		for (n=0;n!=ag_box_count;n++) {
			chk_portal=&ag_boxes[n];

				// only move portals in one direction

				// slowly move portals towards merges
				// so they can merge on multiple sides and
				// not interfere with other merges

			for (k=0;k!=ag_box_count;k++) {
				if (k==n) break;
				
				merge_portal=&ag_boxes[k];

					// merge left

				dist=merge_portal->min.x-chk_portal->max.x;
				if ((dist>0) && (dist<=portal_merge_distance)) {
					if (!map_auto_generate_portal_collision((merge_portal->min.x-split_factor),merge_portal->min.z,merge_portal->max.x,merge_portal->max.z,k)) {
						merge_portal->min.x-=split_factor;
						moved=TRUE;
						break;
					}
				}

					// merge right

				dist=chk_portal->min.x-merge_portal->max.x;
				if ((dist>0) && (dist<=portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(merge_portal->min.x,merge_portal->min.z,(merge_portal->max.x+split_factor),merge_portal->max.z,k)) {
						merge_portal->max.x+=split_factor;
						moved=TRUE;
						break;
					}
				}

					// merge top

				dist=merge_portal->min.z-chk_portal->max.z;
				if ((dist>0) && (dist<=portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(merge_portal->min.x,(merge_portal->min.z-split_factor),merge_portal->max.x,merge_portal->max.z,k)) {
						merge_portal->min.z-=split_factor;
						moved=TRUE;
						break;
					}
				}

					// merge bottom

				dist=chk_portal->min.z-merge_portal->max.z;
				if ((dist>0) && (dist<=portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(merge_portal->min.x,merge_portal->min.z,merge_portal->max.x,(merge_portal->max.z+split_factor),k)) {
						merge_portal->max.z+=split_factor;
						moved=TRUE;
						break;
					}
				}
			}
		}

		if (!moved) break;
	}
}

/* =======================================================

      Connect Portals
      
======================================================= */

void map_auto_generate_connect_portals(map_type *map)
{
	int							n,k,corridor_sz,corridor_rand_sz,corridor_min_sz,
								portal_merge_distance,portal_connect_distance,connect_sz,
								x,z,ex,ez,x2,z2,ex2,ez2,dist,nportal,cnt,split_factor;
	auto_generate_box_type		*chk_portal,*cnt_portal;

		// get sizes

	corridor_sz=(int)(((float)ag_settings.map.portal_sz)*ag_constant_corridor_size_percent);
	corridor_rand_sz=(int)(((float)corridor_sz)*ag_constant_corridor_random_percent);
	corridor_min_sz=corridor_sz-corridor_rand_sz;

	portal_merge_distance=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_merge_percent);
	portal_connect_distance=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_connect_percent);
	
	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);

		// get original portal count

	nportal=ag_box_count;

		// check against all portals

	for (n=0;n!=nportal;n++) {
		chk_portal=&ag_boxes[n];

			// check for portals that fit horizontal connections

		cnt=0;

		for (k=0;k!=nportal;k++) {
			cnt_portal=&ag_boxes[k];

				// any more portals?

			if (ag_box_count>=max_ag_box) return;

				// portals miss each other?

			if (chk_portal->min.z>=cnt_portal->max.z) continue;
			if (chk_portal->max.z<=cnt_portal->min.z) continue;

				// too close or too far away?

			dist=cnt_portal->min.x-chk_portal->max.x;
			if (dist<=0) continue;
			if (dist<portal_merge_distance) continue;
			if (dist>portal_connect_distance) continue;

				// get portal size

			x=chk_portal->max.x;
			ex=cnt_portal->min.x;

			connect_sz=corridor_min_sz+map_auto_generate_random_int(corridor_rand_sz);
			z=(((chk_portal->min.z+chk_portal->max.z)/2)+((cnt_portal->min.z+cnt_portal->max.z)/2))/2;
			z-=(connect_sz/2);
			ez=z+connect_sz;

			if (z<chk_portal->min.z) z=chk_portal->min.z;
			if (z<cnt_portal->min.z) z=cnt_portal->min.z;
			if (ez>chk_portal->max.z) ez=chk_portal->max.z;
			if (ez>cnt_portal->max.z) ez=cnt_portal->max.z;
			
				// fix vertical

			z/=split_factor;
			z*=split_factor;
			
			ez/=split_factor;
			ez*=split_factor;

				// make sure corridor isn't in corner

			if ((z==chk_portal->min.z) || (z==cnt_portal->min.z)) z+=split_factor;
			if ((ez==chk_portal->max.z) || (ez==cnt_portal->max.z)) ez-=split_factor;

			if (z>=ez) continue;

				// not enough connection size for portals?

			if ((ez-z)<corridor_min_sz) continue;
			
				// check for collisions
				
			z2=z-split_factor;
			ez2=ez+split_factor;			// don't create if z's will touch other portals, then it's no longer a corridor

			if ((map_auto_generate_block_collision(&ag_settings,x,z2,ex,ez2)) || (map_auto_generate_portal_collision(x,z2,ex,ez2,-1))) continue;

				// create portal

			ag_boxes[ag_box_count].min.x=x;
			ag_boxes[ag_box_count].max.x=ex;
			ag_boxes[ag_box_count].min.z=z;
			ag_boxes[ag_box_count].max.z=ez;
			
			ag_boxes[ag_box_count].corridor_flag=ag_corridor_flag_horizontal;
			corridor_types[ag_box_count]=map_auto_generate_get_corridor_type(&ag_settings);
			
			ag_boxes[ag_box_count].mesh_idx=map_mesh_add(map);

			ag_box_count++;

				// only allow up to 2 connections in a direction

			cnt++;
			if (cnt==2) break;
		}

			// check for portals that fit vertical connections

		cnt=0;

		for (k=0;k!=nportal;k++) {
			cnt_portal=&ag_boxes[k];

				// any more portals?

			if (ag_box_count>=max_ag_box) return;

				// portals miss each other?

			if (chk_portal->min.x>=cnt_portal->max.x) continue;
			if (chk_portal->max.x<=cnt_portal->min.x) continue;

				// too close or too far away?

			dist=cnt_portal->min.z-chk_portal->max.z;
			if (dist<=0) continue;
			if (dist<portal_merge_distance) continue;
			if (dist>portal_connect_distance) continue;

				// get portal size

			z=chk_portal->max.z;
			ez=cnt_portal->min.z;

			connect_sz=corridor_min_sz+map_auto_generate_random_int(corridor_rand_sz);
			x=(((chk_portal->min.x+chk_portal->max.x)/2)+((cnt_portal->min.x+cnt_portal->max.x)/2))/2;
			x-=(connect_sz/2);
			ex=x+connect_sz;

			if (x<chk_portal->min.x) x=chk_portal->min.x;
			if (x<cnt_portal->min.x) x=cnt_portal->min.x;
			if (ex>chk_portal->max.x) ex=chk_portal->max.x;
			if (ex>cnt_portal->max.x) ex=cnt_portal->max.x;
			
				// fix horizontal

			x/=split_factor;
			x*=split_factor;
			
			ex/=split_factor;
			ex*=split_factor;

				// make sure corridor isn't in corner

			if ((x==chk_portal->min.x) || (x==cnt_portal->min.x)) x+=split_factor;
			if ((ex==chk_portal->max.x) || (ex==cnt_portal->max.x)) ex-=split_factor;

			if (x>=ex) continue;

				// not enough connection size for portals?

			if ((ex-x)<corridor_min_sz) continue;
			
				// check for collisions
				
			x2=x-split_factor;
			ex2=ex+split_factor;			// don't create if x's will touch other portals, then it's no longer a corridor

			if ((map_auto_generate_block_collision(&ag_settings,x2,z,ex2,ez)) || (map_auto_generate_portal_collision(x2,z,ex2,ez,-1))) continue;

				// create portal
				
			ag_boxes[ag_box_count].min.x=x;
			ag_boxes[ag_box_count].max.x=ex;
			ag_boxes[ag_box_count].min.z=z;
			ag_boxes[ag_box_count].max.z=ez;
			
			ag_boxes[ag_box_count].corridor_flag=ag_corridor_flag_vertical;
			corridor_types[ag_box_count]=map_auto_generate_get_corridor_type(&ag_settings);
			ag_boxes[ag_box_count].mesh_idx=map_mesh_add(map);

			ag_box_count++;

				// only allow up to 2 connections in a direction

			cnt++;
			if (cnt==2) break;
		}
	}
}

/* =======================================================

      Setup Random Y for Portals
      
======================================================= */

void map_auto_generate_portal_y(void)
{
	int							n,corridor_slop_y,by_add,portal_high,portal_high_story_add,
								ty,by,extra_ty,extra_by;
	auto_generate_box_type		*portal;
	
		// portal sizes

	portal_high=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_high_percent);
	portal_high_story_add=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_story_high_add_percent);
		
	ty=(map_max_size/2)-(portal_high/2);
	by=(map_max_size/2)+(portal_high/2);
	
	extra_ty=(int)(((float)portal_high)*ag_constant_portal_high_extra_top);
	extra_by=(int)(((float)portal_high)*ag_constant_portal_high_extra_bottom);

		// corridor top slop (so not at very top of portal)

	corridor_slop_y=(int)((float)(by-ty)*ag_constant_portal_high_slop_y);

		// create portal y

	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {
	
			// rooms have variable Ys
			
		if (portal->corridor_flag==ag_corridor_flag_portal) {
		
			portal->min.y=ty-map_auto_generate_random_int(extra_ty);

				// touching rooms get second stories
				
			if (map_auto_generate_portal_touching_any(n)) {
				portal->min.y-=portal_high_story_add;
			}
			
			by_add=map_auto_generate_random_int(extra_by);
			if (by_add<=ag_constant_step_high) {
				by_add=0;
			}
			else {
				by_add/=ag_constant_step_high;
				if (by_add!=0) by_add=(by_add*ag_constant_step_high);
			}

			portal->max.y=by+by_add;
		}
		
			// corridors have static Ys
			
		else {
			portal->min.y=(ty+corridor_slop_y);
			portal->max.y=by;
		}

			// fit within map_enlarge

		portal->min.y/=map_enlarge;
		portal->min.y*=map_enlarge;

		portal->max.y/=map_enlarge;
		portal->max.y*=map_enlarge;
		
		portal++;
	}
}

/* =======================================================

      Auto-Generate Walls
      
======================================================= */

void map_auto_generate_walls(map_type *map)
{
	int							n,x,z,xsz,zsz,ty,by,split_factor,txt_idx,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	auto_generate_box_type		*portal;

		// how we split the walls into a mesh

	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);

		// create surrounding walls for portals

	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {
		
			// start mesh
			
		txt_idx=(portal->corridor_flag==ag_corridor_flag_portal)?ag_settings.texture.portal_wall:ag_settings.texture.corridor_wall;
		if (!map_auto_generate_mesh_start(map,n,-1,txt_idx,FALSE,FALSE)) return;


			// portal height

		ty=portal->min.y;
		by=portal->max.y;

			// portal size

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;

			// build left/right walls

		for (z=0;z<zsz;z+=split_factor) {

				// left

			if (!map_auto_generate_portal_horz_edge_block(n,(z+portal->min.z),((z+portal->min.z)+split_factor),portal->min.x)) {
				map_auto_generate_poly_from_square_wall(0,z,0,(z+split_factor),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			}

				// right

			if (!map_auto_generate_portal_horz_edge_block(n,(z+portal->min.z),((z+portal->min.z)+split_factor),portal->max.x)) {
				map_auto_generate_poly_from_square_wall(xsz,z,xsz,(z+split_factor),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			}
		}

			// build top/bottom walls

		for (x=0;x<xsz;x+=split_factor) {

				// top

			if (!map_auto_generate_portal_vert_edge_block(n,(x+portal->min.x),((x+portal->min.x)+split_factor),portal->min.z)) {
				map_auto_generate_poly_from_square_wall(x,0,(x+split_factor),0,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			}

				// bottom

			if (!map_auto_generate_portal_vert_edge_block(n,(x+portal->min.x),((x+portal->min.x)+split_factor),portal->max.z)) {
				map_auto_generate_poly_from_square_wall(x,zsz,(x+split_factor),zsz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			}
		}

		portal++;
	}
}

/* =======================================================

      Auto-Generate Height Connecting Walls
      
======================================================= */

void map_auto_generate_height_walls(map_type *map)
{
	int							n,k,x,ex,kx,z,ez,kz,xsz,zsz,split_factor,txt_idx,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	auto_generate_box_type		*portal,*chk_portal;

		// how we split the walls into a mesh

	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);

		// create walls for any portals with different heights

	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {
		
			// portal size

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;
		
			// check for touching portals
	
		for (k=0;k!=ag_box_count;k++) {
			if (k==n) continue;
			
				// only put height connecting walls in rooms that are higher
				
			chk_portal=&ag_boxes[k];
			if (portal->min.y>=chk_portal->min.y) continue;
			
				// texture
			
			txt_idx=(portal->corridor_flag==ag_corridor_flag_portal)?ag_settings.texture.portal_wall:ag_settings.texture.corridor_wall;
			if (!map_auto_generate_mesh_start(map,n,-1,txt_idx,FALSE,FALSE)) return;
			
				// portals touching top
				
			if (portal->min.z==chk_portal->max.z) {
				x=portal->min.x;
				if (chk_portal->min.x>x) x=chk_portal->min.x;
				ex=portal->max.x;
				if (chk_portal->max.x<ex) ex=chk_portal->max.x;
				if (x>ex) continue;
				
				x-=portal->min.x;
				ex-=portal->min.x;

				for (kx=x;kx<ex;kx+=split_factor) {
					map_auto_generate_poly_from_square_wall(kx,0,(kx+split_factor),0,portal->min.y,chk_portal->min.y,px,py,pz,gx,gy);
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

				for (kx=x;kx<ex;kx+=split_factor) {
					map_auto_generate_poly_from_square_wall(kx,zsz,(kx+split_factor),zsz,portal->min.y,chk_portal->min.y,px,py,pz,gx,gy);
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

				for (kz=z;kz<ez;kz+=split_factor) {
					map_auto_generate_poly_from_square_wall(0,kz,0,(kz+split_factor),portal->min.y,chk_portal->min.y,px,py,pz,gx,gy);
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

				for (kz=z;kz<ez;kz+=split_factor) {
					map_auto_generate_poly_from_square_wall(xsz,kz,xsz,(kz+split_factor),portal->min.y,chk_portal->min.y,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}
				continue;
			}
			
		}
	
		portal++;
	}
}

/* =======================================================

      Auto-Generate Clipping Corridor Walls
      
======================================================= */

void map_auto_generate_corridor_clip_walls(map_type *map)
{
	int							n,xsz,zsz,ty,by,
								px[8],py[8],pz[8],clip_sz;
	float						gx[8],gy[8];
	auto_generate_box_type		*portal;
	
		// add in the clip walls
		
	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {
		
		if (portal->corridor_flag==ag_corridor_flag_portal) {
			portal++;
			continue;
		}
		
			// portal size

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;
		
		clip_sz=(portal->max.y-portal->min.y)>>2;

			// top walls
			
		if ((corridor_types[n]==ag_corridor_type_slanted_ceiling) || (corridor_types[n]==ag_corridor_type_octagon)) {

			if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;

			ty=portal->min.y;
			by=ty+clip_sz;
		
			if (portal->corridor_flag==ag_corridor_flag_horizontal) {
				map_auto_generate_poly_from_bot_trig_wall(0,0,0,clip_sz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_bot_trig_wall(0,zsz,0,(zsz-clip_sz),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_bot_trig_wall(xsz,0,xsz,clip_sz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_bot_trig_wall(xsz,zsz,xsz,(zsz-clip_sz),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
			}

			if (portal->corridor_flag==ag_corridor_flag_vertical) {
				map_auto_generate_poly_from_bot_trig_wall(0,0,clip_sz,0,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_bot_trig_wall(xsz,0,(xsz-clip_sz),0,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
				
				map_auto_generate_poly_from_bot_trig_wall(0,zsz,clip_sz,zsz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_bot_trig_wall(xsz,zsz,(xsz-clip_sz),zsz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
			}

		}
		
			// bottom walls
			
		if (corridor_types[n]==ag_corridor_type_octagon) {

			if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;
		
			ty=portal->max.y-clip_sz;
			by=portal->max.y;
		
			if (portal->corridor_flag==ag_corridor_flag_horizontal) {
				map_auto_generate_poly_from_top_trig_wall(0,0,0,clip_sz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_top_trig_wall(0,zsz,0,(zsz-clip_sz),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_top_trig_wall(xsz,0,xsz,clip_sz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_top_trig_wall(xsz,zsz,xsz,(zsz-clip_sz),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
			}

			if (portal->corridor_flag==ag_corridor_flag_vertical) {
				map_auto_generate_poly_from_top_trig_wall(0,0,clip_sz,0,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_top_trig_wall(xsz,0,(xsz-clip_sz),0,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
				
				map_auto_generate_poly_from_top_trig_wall(0,zsz,clip_sz,zsz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_top_trig_wall(xsz,zsz,(xsz-clip_sz),zsz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
			}

		}
			
		portal++;
	}
}

/* =======================================================

      Auto-Generate Ceilings
      
======================================================= */

bool map_auto_generate_portal_ceiling_ok(unsigned char *data,int lx,int lz,int rx,int rz,int lx2,int lz2,int rx2,int rz2)
{
	int			k,split_factor,mx,mz;

		// in outer ring?

	if ((lx2==lx) || (rx2==rx) || (lz2==lz) || (rz2==rz)) return(data[ag_ceiling_outer_ring]!=0);

		// inside sections

	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);

	k=((rx-lx)/split_factor)>>1;
	mx=lx+(k*split_factor);

	k=((rz-lz)/split_factor)>>1;
	mz=lz+(k*split_factor);

	if ((rx2<=mx) && (rz2<=mz)) return(data[ag_ceiling_top_left]!=0);
	if ((lx2==mx) && (rz2<=mz)) return(data[ag_ceiling_top_middle]!=0);
	if ((lx2>mx) && (rz2<=mz)) return(data[ag_ceiling_top_right]!=0);

	if ((rx2<=mx) && (lz2==mz)) return(data[ag_ceiling_middle_left]!=0);
	if ((lx2==mx) && (lz2==mz)) return(data[ag_ceiling_middle_middle]!=0);
	if ((lx2>mx) && (lz2==mz)) return(data[ag_ceiling_middle_right]!=0);

	if ((rx2<=mx) && (lz2>mz)) return(data[ag_ceiling_bottom_left]!=0);
	if ((lx2==mx) && (lz2>mz)) return(data[ag_ceiling_bottom_middle]!=0);
	if ((lx2>mx) && (lz2>mz)) return(data[ag_ceiling_bottom_right]!=0);

	return(FALSE);
}

void map_auto_generate_portal_ceiling_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty)
{
	int							ceiling_type,split_factor,lx2,rx2,lz2,rz2,
								k,kx,kz,mx,wall_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	unsigned char				*data;
	auto_generate_box_type		*portal;
		
	portal=&ag_boxes[rn];
	
		// get sizes
		
	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);
	wall_sz=(int)((float)(portal->max.y-portal->min.y)*ag_constant_portal_ceiling_slant_percent);
	
		// get type and data

	ceiling_type=map_auto_generate_get_ceiling_type(&ag_settings);
	
	ceiling_types[rn]=(unsigned char)ceiling_type;
	
		// create ceiling polys

	data=(unsigned char*)ag_ceiling_data[ceiling_type];

	lz2=lz;

	while (lz2<rz) {

		rz2=lz2+split_factor;

		lx2=lx;

		while (lx2<rx) {

			rx2=lx2+split_factor;
				
			if (map_auto_generate_portal_ceiling_ok(data,lx,lz,rx,rz,lx2,lz2,rx2,rz2)) {
				if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.portal_ceiling,FALSE,FALSE)) return;
				map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,ty,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			}
			
			lx2=rx2;
		}
		
		lz2=rz2;
	}	

		// slanted ceilings

	if ((ceiling_type==ag_ceiling_type_a_frame_horz_bar) || (ceiling_type==ag_ceiling_type_a_frame_vert_bar)) {
		lx2=lx+split_factor;
		lz2=lz+split_factor;
		rx2=rx-split_factor;
		rz2=rz-split_factor;
		mx=(lx2+rx2)>>1;
		
		if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;
		map_auto_generate_poly_from_square_floor_slant(lx2,lz2,mx,rz2,ty,wall_sz,ag_ceiling_lower_pos_x,FALSE,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		map_auto_generate_poly_from_square_floor_slant(mx,lz2,rx2,rz2,ty,wall_sz,ag_ceiling_lower_neg_x,FALSE,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	}

		// walls

	data=(unsigned char*)ag_ceiling_wall_data[ceiling_type];

	if (data[ag_ceiling_wall_outer_ring]!=0) {

		if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.portal_ceiling,FALSE,FALSE)) return;

		lx2=lx+split_factor;
		rx2=rx-split_factor;

		for (kx=lx2;kx<rx2;kx+=split_factor) {
			map_auto_generate_poly_from_square_wall(kx,(lz+split_factor),(kx+split_factor),(lz+split_factor),(portal->min.y-wall_sz),portal->min.y,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			map_auto_generate_poly_from_square_wall(kx,(rz-split_factor),(kx+split_factor),(rz-split_factor),(portal->min.y-wall_sz),portal->min.y,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}

		lz2=lz+split_factor;
		rz2=rz-split_factor;

		for (kz=lz2;kz<rz2;kz+=split_factor) {
			map_auto_generate_poly_from_square_wall((lx+split_factor),kz,(lx+split_factor),(kz+split_factor),(portal->min.y-wall_sz),portal->min.y,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			map_auto_generate_poly_from_square_wall((rx-split_factor),kz,(rx-split_factor),(kz+split_factor),(portal->min.y-wall_sz),portal->min.y,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}

	}

	if (data[ag_ceiling_wall_horizontal]!=0) {

		if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.portal_ceiling,FALSE,FALSE)) return;

		k=(rz-lz)/split_factor;
		lz2=lz+((k>>1)*split_factor);
		rz2=lz2+split_factor;

		lx2=lx+split_factor;
		rx2=rx-split_factor;

		for (kx=lx2;kx<rx2;kx+=split_factor) {
			map_auto_generate_poly_from_square_wall(kx,lz2,(kx+split_factor),lz2,(portal->min.y-wall_sz),portal->min.y,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			map_auto_generate_poly_from_square_wall(kx,rz2,(kx+split_factor),rz2,(portal->min.y-wall_sz),portal->min.y,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}

	}

	if (data[ag_ceiling_wall_vertical]!=0) {

		if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.portal_ceiling,FALSE,FALSE)) return;

		k=(rx-lx)/split_factor;
		lx2=lx+((k>>1)*split_factor);
		rx2=lx2+split_factor;

		lz2=lz+split_factor;
		rz2=rz-split_factor;

		for (kz=lz2;kz<rz2;kz+=split_factor) {
			map_auto_generate_poly_from_square_wall(lx2,kz,lx2,(kz+split_factor),(portal->min.y-wall_sz),portal->min.y,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			map_auto_generate_poly_from_square_wall(rx2,kz,rx2,(kz+split_factor),(portal->min.y-wall_sz),portal->min.y,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}
	}

	if (data[ag_ceiling_wall_slant]!=0) {

		if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;

		kx=(rx-lx)>>1;

		map_auto_generate_poly_from_top_trig_wall(mx,(lz+split_factor),(lx+split_factor),(lz+split_factor),(ty-wall_sz),ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

		map_auto_generate_poly_from_top_trig_wall(mx,(lz+split_factor),(rx-split_factor),(lz+split_factor),(ty-wall_sz),ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

		map_auto_generate_poly_from_top_trig_wall(mx,(rz-split_factor),(lx+split_factor),(rz-split_factor),(ty-wall_sz),ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

		map_auto_generate_poly_from_top_trig_wall(mx,(rz-split_factor),(rx-split_factor),(rz-split_factor),(ty-wall_sz),ty,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
	}
}

void map_auto_generate_corridor_ceiling_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty)
{
	int							split_factor,lx2,rx2,lz2,rz2,slant_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	auto_generate_box_type		*portal;
	
		// get sizes
		
	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);
		
	   // get portal slants

	portal=&ag_boxes[rn];
	
	slant_sz=(portal->max.y-portal->min.y)>>2;
	
		// create regular floor/ceiling segments

	lz2=lz;

	while (lz2<rz) {

		rz2=lz2+split_factor;

		lx2=lx;

		while (lx2<rx) {

			rx2=lx2+split_factor;
				
			if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor_ceiling,FALSE,FALSE)) return;
			map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,ty,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
			
			lx2=rx2;
		}
		
		lz2=rz2;
	}
	
		// additional slanted ceilings for corridors
		
	if ((corridor_types[rn]==ag_corridor_type_slanted_ceiling) || (corridor_types[rn]==ag_corridor_type_octagon)) {

		if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor_ceiling,FALSE,FALSE)) return;

		if (portal->corridor_flag==ag_corridor_flag_horizontal) {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,rx,(lz+slant_sz),ty,slant_sz,ag_ceiling_lower_neg_z,TRUE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

			map_auto_generate_poly_from_square_floor_slant(lx,(rz-slant_sz),rx,rz,ty,slant_sz,ag_ceiling_lower_pos_z,TRUE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
		}
		else {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,(lx+slant_sz),rz,ty,slant_sz,ag_ceiling_lower_neg_x,TRUE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

			map_auto_generate_poly_from_square_floor_slant((rx-slant_sz),lz,rx,rz,ty,slant_sz,ag_ceiling_lower_pos_x,TRUE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
		}
	}
}

void map_auto_generate_ceilings(map_type *map)
{
	int							n,xsz,zsz;
	auto_generate_box_type		*portal;

	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {

			// portal size

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;

			// ceilings
			
		if (portal->corridor_flag==ag_corridor_flag_portal) {
			map_auto_generate_portal_ceiling_add(map,n,0,0,xsz,zsz,portal->min.y);
		}
		else {
			map_auto_generate_corridor_ceiling_add(map,n,0,0,xsz,zsz,portal->min.y);
		}
		
		portal++;
	}
}

/* =======================================================

      Auto-Generate Floors 
      
======================================================= */

void map_auto_generate_portal_floor_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int by)
{
	int							split_factor,lx2,rx2,lz2,rz2,
								floor_type,rough_max,row_add,col_add,xadd,zadd,
								px[8],py[8],pz[8];
	int							*fys,*fy;
	float						gx[8],gy[8];
	auto_generate_box_type		*portal;
		
	portal=&ag_boxes[rn];
	
		// get sizes
		
	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);
	rough_max=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_rough_floor_percent);
	
		// get floor type
		
	floor_type=map_auto_generate_get_floor_type(&ag_settings);
	
		// create floor Ys

	col_add=((rx-lx)/split_factor)+1;
	row_add=((rz-lz)/split_factor)+1;
	
	fys=(int*)valloc(sizeof(int)*(col_add*row_add));
	if (fys==NULL) return;

	fy=fys;

	for (lz=0;lz<row_add;lz++) {
		for (lx=0;lx<col_add;lx++) {
		
			if ((lz==0) || (lz==(row_add-1)) || (lx==0) || (lx==(col_add-1)) || (floor_type!=ag_floor_type_rough)) {
				*fy=by;
			}
			else {
				*fy=by-map_auto_generate_random_int(rough_max);
			}
			
			fy++;
		}
	}
	
		// create floor polys

	zadd=0;
	lz2=lz;

	while (lz2<rz) {

		rz2=lz2+split_factor;

		xadd=0;
		lx2=lx;

		while (lx2<rx) {

			rx2=lx2+split_factor;

			if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.portal_floor,FALSE,FALSE)) {
				free(fys);
				return;
			}

				// create flat polygon

			map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,by,px,py,pz,gx,gy);

				// rebuild the Ys

			py[0]=fys[(col_add*zadd)+xadd];
			py[1]=fys[(col_add*zadd)+(xadd+1)];
			py[2]=fys[(col_add*(zadd+1))+(xadd+1)];
			py[3]=fys[(col_add*(zadd+1))+xadd];

				// generate polygon

			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

			lx2=rx2;
			xadd++;
		}
		
		lz2=rz2;
		zadd++;
	}

	free(fys);
}

void map_auto_generate_corridor_floor_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int by)
{
	int							split_factor,lx2,rx2,lz2,rz2,slant_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	auto_generate_box_type		*portal;
	
		// get sizes
		
	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);
		
	   // get portal slants

	portal=&ag_boxes[rn];
	
	slant_sz=(portal->max.y-portal->min.y)>>2;
	
		// create regular floor/ceiling segments

	lz2=lz;

	while (lz2<rz) {

		rz2=lz2+split_factor;

		lx2=lx;

		while (lx2<rx) {

			rx2=lx2+split_factor;

			if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor_floor,FALSE,FALSE)) return;
			map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			
			lx2=rx2;
		}
		
		lz2=rz2;
	}
	
		// additional slanted floors for corridors
		
	if (corridor_types[rn]==ag_corridor_type_octagon) {
		
		if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor_floor,FALSE,FALSE)) return;

		if (portal->corridor_flag==ag_corridor_flag_horizontal) {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,rx,(lz+slant_sz),by,slant_sz,ag_ceiling_lower_neg_z,FALSE,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

			map_auto_generate_poly_from_square_floor_slant(lx,(rz-slant_sz),rx,rz,by,slant_sz,ag_ceiling_lower_pos_z,FALSE,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}
		else {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,(lx+slant_sz),rz,by,slant_sz,ag_ceiling_lower_neg_x,FALSE,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

			map_auto_generate_poly_from_square_floor_slant((rx-slant_sz),lz,rx,rz,by,slant_sz,ag_ceiling_lower_pos_x,FALSE,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}
	}
}

void map_auto_generate_floors(map_type *map)
{
	int							n,xsz,zsz;
	auto_generate_box_type		*portal;

	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {

			// portal size

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;

			// floors and ceilings
			
		if (portal->corridor_flag==ag_corridor_flag_portal) {
			map_auto_generate_portal_floor_add(map,n,0,0,xsz,zsz,portal->max.y);
		}
		else {
			map_auto_generate_corridor_floor_add(map,n,0,0,xsz,zsz,portal->max.y);
		}
		
		portal++;
	}
}

/* =======================================================

      Second Stories
      
======================================================= */

void map_auto_generate_second_story_block(map_type *map,unsigned char *poly_map,int x,int z,int xsz,int zsz,int split_factor,int y)
{
	int				lx,rx,lz,rz,by,px[8],py[8],pz[8];
	float			gx[8],gy[8];

		// floor here?

	if (poly_map[(z*xsz)+x]==0x0) return;

		// story size

	lx=x*split_factor;
	rx=lx+split_factor;
	lz=z*split_factor;
	rz=lz+split_factor;

	by=y+ag_constant_portal_story_high;

		// create floor and ceiling

	map_auto_generate_poly_from_square_floor(lx,lz,rx,rz,y,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(lx,lz,rx,rz,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

		// create walls

	if (x!=0) {
		if (poly_map[(z*xsz)+(x-1)]==0x0) {
			map_auto_generate_poly_from_square_wall(lx,lz,lx,rz,y,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}
	}

	if (x!=(xsz-1)) {
		if (poly_map[(z*xsz)+(x+1)]==0x0) {
			map_auto_generate_poly_from_square_wall(rx,lz,rx,rz,y,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}
	}

	if (z!=0) {
		if (poly_map[((z-1)*xsz)+x]==0x0) {
			map_auto_generate_poly_from_square_wall(lx,lz,rx,lz,y,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}
	}

	if (z!=(zsz-1)) {
		if (poly_map[((z+1)*xsz)+x]==0x0) {
			map_auto_generate_poly_from_square_wall(lx,rz,rx,rz,y,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}
	}
}

int map_auto_generate_second_story_steps_get_x(int portal_idx,int z,int ez,bool lft,int xsz,int step_wid)
{
	int							split_factor;
	auto_generate_box_type		*portal;
	
	portal=&ag_boxes[portal_idx];
	
	z+=portal->min.z;
	ez+=portal->min.z;
	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);
	
	if (lft) {
		if (!map_auto_generate_portal_horz_edge_touch(portal_idx,z,ez,portal->max.x)) return(xsz-step_wid);
		return(split_factor*2);
	}
	else {
		if (!map_auto_generate_portal_horz_edge_touch(portal_idx,z,ez,portal->min.x)) return(0);
		return(xsz-((split_factor*2)+step_wid));
	}

	return(0);
}

int map_auto_generate_second_story_steps_get_z(int portal_idx,int x,int ex,bool top,int zsz,int step_wid)
{
	int							split_factor;
	auto_generate_box_type		*portal;
	
	portal=&ag_boxes[portal_idx];
	
	x+=portal->min.x;
	ex+=portal->min.x;
	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);
	
	if (top) {
		if (!map_auto_generate_portal_vert_edge_touch(portal_idx,x,ex,portal->max.z)) return(zsz-step_wid);
		return(split_factor*2);
	}
	else {
		if (!map_auto_generate_portal_vert_edge_touch(portal_idx,x,ex,portal->min.z)) return(0);
		return(zsz-((split_factor*2)+step_wid));
	}

	return(0);
}

void map_auto_generate_second_story(map_type *map)
{
	int							n,portal_high,extra_ty,split_factor,step_wid,step_len,sz,
								x,y,z,by,mx,mz,xsz,zsz;
	bool						lft,rgt,top,bot,horz,vert,
								old_lft,old_rgt,old_top,old_bot;
	unsigned char				*poly_map;
	auto_generate_box_type		*portal;

		// get sizes
		
	portal_high=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_high_percent);
	extra_ty=(int)(((float)portal_high)*ag_constant_portal_high_extra_top);

	split_factor=(int)(((float)ag_settings.map.portal_sz)*ag_constant_portal_split_factor_percent);

		// create second story in touching portals

	for (n=0;n!=ag_box_count;n++) {

		portal=&ag_boxes[n];
		if (portal->corridor_flag!=ag_corridor_flag_portal) continue;

			// find touching edges

		
		lft=map_auto_generate_portal_touching_left(n);
		rgt=map_auto_generate_portal_touching_right(n);
		top=map_auto_generate_portal_touching_top(n);
		bot=map_auto_generate_portal_touching_bottom(n);

		if (!((lft) || (rgt) || (top) || (bot))) continue;

			// extra floor area

		old_lft=lft;
		old_rgt=rgt;
		old_top=top;
		old_bot=bot;

		map_auto_generate_story_extra_floor(&lft,&rgt,&top,&bot,&horz,&vert);

			// create polygon map

		xsz=(portal->max.x-portal->min.x)/split_factor;
		zsz=(portal->max.z-portal->min.z)/split_factor;

		sz=(xsz+1)*(zsz+1);
		poly_map=(unsigned char*)valloc(sz);
		if (poly_map==NULL) return;

		bzero(poly_map,sz);

		if ((lft) || (rgt) || (vert)) {

			mx=xsz>>1;
		
			for (z=0;z<zsz;z++) {
				if (lft) poly_map[(z*xsz)]=poly_map[(z*xsz)+1]=0x1;
				if (rgt) poly_map[(z*xsz)+(xsz-1)]=poly_map[(z*xsz)+(xsz-2)]=0x1;
				if (vert) poly_map[(z*xsz)+mx]=0x1;
			}
		}

		if ((top) || (bot) || (horz)) {

			mz=zsz>>1;

			for (x=0;x<xsz;x++) {
				if (top) poly_map[x]=poly_map[xsz+x]=0x1;
				if (bot) poly_map[((zsz-1)*xsz)+x]=poly_map[((zsz-2)*xsz)+x]=0x1;
				if (horz) poly_map[(mz*xsz)+x]=0x1;
			}
		}

			// create polygons

		if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.second_story,FALSE,TRUE)) return;

		y=(map_max_size>>1)-((portal_high+extra_ty)>>1);

		for (z=0;z<zsz;z++) {
			for (x=0;x<xsz;x++) {
				map_auto_generate_second_story_block(map,poly_map,x,z,xsz,zsz,split_factor,y);
			}
		}

			// is there a place for steps?

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;

		step_wid=split_factor*ag_constant_story_steps_split_factor;
		by=portal->max.y;
		
		step_len=map_auto_generate_steps_get_length(y,by,ag_constant_step_story_size);

		if ((lft) && (!old_lft)) {
			x=split_factor*2;
			z=map_auto_generate_second_story_steps_get_z(n,x,(x+step_len),top,zsz,step_wid);
			map_auto_generate_steps(map,n,y,by,ag_stair_pos_x,ag_constant_step_story_size,FALSE,TRUE,x,x,z,(z+step_wid));
		}
		else {
			if ((rgt) && (!old_rgt)) {
				x=xsz-(split_factor*2);
				z=map_auto_generate_second_story_steps_get_z(n,(x-step_len),x,top,zsz,step_wid);
				map_auto_generate_steps(map,n,y,by,ag_stair_neg_x,ag_constant_step_story_size,FALSE,TRUE,x,x,z,(z+step_wid));
			}
		}

		if ((top) && (!old_top)) {
			z=split_factor*2;
			x=map_auto_generate_second_story_steps_get_x(n,z,(z+step_len),lft,xsz,step_wid);
			map_auto_generate_steps(map,n,y,by,ag_stair_pos_z,ag_constant_step_story_size,FALSE,TRUE,x,(x+step_wid),z,z);
		}
		else {
			if ((bot) && (!old_bot)) {
				z=zsz-(split_factor*2);
				x=map_auto_generate_second_story_steps_get_x(n,(z-step_len),z,lft,xsz,step_wid);
				map_auto_generate_steps(map,n,y,by,ag_stair_neg_z,ag_constant_step_story_size,FALSE,TRUE,x,(x+step_wid),z,z);
			}
		}
	}
}

/* =======================================================

      Initialize Flags
      
======================================================= */

void map_auto_generate_clear_flags(void)
{
	int				n;
	
	for (n=0;n!=max_portal;n++) {
		ag_boxes[n].corridor_flag=ag_corridor_flag_portal;
		corridor_types[n]=ag_corridor_type_normal;
	}
}

/* =======================================================

      Auto-Generate MainLine
      
======================================================= */

void map_auto_generate(map_type *map,auto_generate_settings_type *ags)
{
	int			sz;
	
		// setup global

	memmove(&ag_settings,ags,sizeof(auto_generate_settings_type));

		// clear the map
		
	map_auto_generate_clear(map);

		// some additional setup

	map_auto_generate_random_seed(ag_settings.seed);
	
	sz=ag_settings.map.map_sz/2;

	ag_settings.map.left=(map_max_size/2)-sz;
	ag_settings.map.right=(map_max_size/2)+sz;
	ag_settings.map.top=(map_max_size/2)-sz;
	ag_settings.map.bottom=(map_max_size/2)+sz;
	
	map_auto_generate_clear_flags();

		// create portals

	map_auto_generate_initial_portals(map);
	map_auto_generate_merge_portals();
	map_auto_generate_connect_portals(map);
	map_auto_generate_portal_y();
	
		// create meshes

	map_auto_generate_walls(map);
	map_auto_generate_height_walls(map);
	map_auto_generate_corridor_clip_walls(map);
	map_auto_generate_floors(map);
	map_auto_generate_ceilings(map);
	
		// second stories
		
	map_auto_generate_second_story(map);
	
		// create ramps, steps, and doors
		
	map_auto_generate_ramps(map);
	map_auto_generate_corridor_to_portal_steps(map);
	map_auto_generate_doors(map);
		
		// rebuild the UVs
		
	map_auto_generate_reset_UVs(map);
	
		// create lights
		
	map_auto_generate_lights(map);
	
		// create player spot
		
	map_auto_generate_add_player_spot(map);
	
		// create spots
		
	map_auto_generate_spots(map);
	
		// center map
		
	map_center(map);
}

/* =======================================================

      Auto-Generate Test
      
======================================================= */

bool map_auto_generate_test(map_type *map,bool load_shaders)
{
	int								n;
	auto_generate_settings_type		ags;

		// start new map

	if (!map_new(map,"Demo")) return(FALSE);

		// setup textures

	strcpy(map->textures[0].bitmaps[0].name,"Castle Wall");
	strcpy(map->textures[1].bitmaps[0].name,"Brick Brown");
	strcpy(map->textures[2].bitmaps[0].name,"Rock");
	strcpy(map->textures[3].bitmaps[0].name,"Rusty Metal");
	strcpy(map->textures[4].bitmaps[0].name,"Brick Red");
	strcpy(map->textures[5].bitmaps[0].name,"Stone Wall");
	strcpy(map->textures[6].bitmaps[0].name,"Planks Old");

		// load textures

	if (!map_textures_read(map)) return(FALSE);
	if (!map_textures_setup_glowmaps(map)) return(FALSE);
	if (load_shaders) if (!map_shaders_read(map)) return(FALSE);

		// setup auto generation values

#ifndef D3_OS_WINDOWS
	ags.seed=TickCount();
#else
	ags.seed=GetTickCount();
#endif

	ags.seed=5;	// supergumba -- testing

	ags.block=ag_block_none;
	
	ags.map.map_sz=2000*map_enlarge;
	ags.map.portal_sz=500*map_enlarge;
	
	for (n=0;n!=ag_floor_type_count;n++) {
		ags.floor_type_on[n]=TRUE;
	}

	for (n=0;n!=ag_ceiling_type_count;n++) {
		ags.ceiling_type_on[n]=TRUE;
	}
	
	for (n=0;n!=ag_corridor_type_count;n++) {
		ags.corridor_type_on[n]=TRUE;
	}

	for (n=0;n!=ag_door_type_count;n++) {
		ags.door_type_on[n]=TRUE;
	}
	
	for (n=0;n!=ag_light_type_count;n++) {
		ags.light_type_on[n]=TRUE;
	}
	
	ags.texture.portal_wall=0;
	ags.texture.portal_floor=2;
	ags.texture.portal_ceiling=1;
	ags.texture.steps=5;
	ags.texture.ramp=5;
	ags.texture.corridor_wall=4;
	ags.texture.corridor_floor=4;
	ags.texture.corridor_ceiling=4;
	ags.texture.door=3;
	ags.texture.second_story=6;
	
	strcpy(ags.door_sound,"Door");

		// create the map

	map_auto_generate(map,&ags);

	return(TRUE);
}
