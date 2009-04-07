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
extern bool map_auto_generate_portal_on_map_left_edge(int portal_idx);
extern bool map_auto_generate_portal_on_map_right_edge(int portal_idx);
extern bool map_auto_generate_portal_on_map_top_edge(int portal_idx);
extern bool map_auto_generate_portal_on_map_bottom_edge(int portal_idx);

extern int map_auto_generate_get_ceiling_type(auto_generate_settings_type *ags);
extern int map_auto_generate_get_corridor_type(auto_generate_settings_type *ags);
extern int map_auto_generate_get_stair_type(auto_generate_settings_type *ags);

extern bool map_auto_generate_block_collision(auto_generate_settings_type *ags,int x,int z,int ex,int ez);

extern void map_auto_generate_story_extra_floor(bool *lft,bool *rgt,bool *top,bool *bot,bool *horz,bool *vert);

extern void map_auto_generate_poly_from_square_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_top_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_bot_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor(int lx,int lz,int rx,int rz,int fy,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor_slant(int lx,int lz,int rx,int rz,int fy,int yadd,int lower_mode,bool reverse_slant,int *x,int *y,int *z,float *gx,float *gy);

extern bool map_auto_generate_mesh_start(map_type *map,int box_idx,int group_idx,int txt_idx,bool moveable,bool new_mesh);
extern void map_auto_generate_mesh_set_lock(map_type *map);
extern int map_auto_generate_mesh_change_texture(int txt_idx);
extern bool map_auto_generate_mesh_add_poly(map_type *map,int ptsz,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_mesh_punch_hole_last_poly(map_type *map,int x,int z);
extern void map_auto_generate_mesh_effect_uv_last_poly(map_type *map,float mult_x,float mult_y,bool rot);

extern void map_auto_generate_vert_frame_mesh(map_type *map,int rn,int ty,int by,int x,int z,int frame_sz,bool reverse,bool full_frame);
extern void map_auto_generate_horz_frame_mesh(map_type *map,int rn,int ty,int by,int x,int z,int frame_sz,bool reverse,bool full_frame);

extern int map_auto_generate_steps_get_length(int ty,int by,int step_size,int step_high);
extern void map_auto_generate_steps_mesh(map_type *map,int rn,int step_type,int step_sz,int step_high,int ty,int by,int kx,int kz,float ang_y);
extern void map_auto_generate_lift(map_type *map,int rn,int step_high,int ty,int by,int lx,int rx,int lz,int rz);

extern void map_auto_generate_lights(map_type *map);
extern void map_auto_generate_nodes(map_type *map);
extern void map_auto_generate_spots(map_type *map);
extern void map_auto_generate_ramps(map_type *map);
extern void map_auto_generate_corridor_to_portal_connections(map_type *map);
extern void map_auto_generate_doors(map_type *map);

int									ag_box_count;

unsigned char						ag_ceiling_data[ag_ceiling_type_count][11]=ag_ceiling_data_bytes,
									ag_ceiling_wall_data[ag_ceiling_type_count][6]=ag_ceiling_wall_data_bytes;

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
				map_x_sz,map_z_sz,portal_sz,portal_rand_sz,portal_min_sz;

		// sizes

	map_x_sz=ag_settings.map.right-ag_settings.map.left;
	map_z_sz=ag_settings.map.bottom-ag_settings.map.top;

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	portal_rand_sz=(int)(((float)portal_sz)*ag_constant_portal_random_percent);
	portal_min_sz=portal_sz-portal_rand_sz;
	
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	
		// initial count of portals
	
	initial_count=(ag_settings.map.map_sz/portal_sz)*4;

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
							split_factor,merge_try_count,portal_sz;
	bool					moved;
	auto_generate_box_type	*chk_portal,*merge_portal;

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	portal_merge_distance=(int)(((float)portal_sz)*ag_constant_portal_merge_percent);

		// attempt to merge portals within a certain distance
		// together.  All portals are grided on the split
		// factor, so we merge on that

		// only keep moving for a limited time
		// so portals that bounce between two don't cause
		// an infinite loop
		
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
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

void map_auto_generate_merge_portals_center(void)
{
	int						n,x,z,cx,cz,move_cnt,portal_sz,split_factor,
							xadd[max_portal],zadd[max_portal];
	bool					moved;
	auto_generate_box_type	*portal;

		// movement variables

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

		// find center

	cx=cz=0;

	for (n=0;n!=ag_box_count;n++) {
		portal=&ag_boxes[n];

		cx+=((portal->min.x+portal->max.x)>>1);
		cz+=((portal->min.z+portal->max.z)>>1);
	}

	cx/=ag_box_count;
	cz/=ag_box_count;

		// get movement directions

	for (n=0;n!=ag_box_count;n++) {
		portal=&ag_boxes[n];

		if (((portal->min.x+portal->max.x)>>1)<cx) {
			xadd[n]=split_factor;
		}
		else {
			xadd[n]=-split_factor;
		}

		if (((portal->min.z+portal->max.z)>>1)<cz) {
			zadd[n]=split_factor;
		}
		else {
			zadd[n]=-split_factor;
		}
	}

		// move all portals towards center
		// until no more movement or over a move count

	move_cnt=0;

	while (move_cnt<100) {

		moved=FALSE;

		for (n=0;n!=ag_box_count;n++) {
			portal=&ag_boxes[n];

			x=(portal->min.x+portal->max.x)>>1;
			z=(portal->min.z+portal->max.z)>>1;

			if (((xadd[n]>0) && (x<cx)) || ((xadd[n]<0) && (x>cx))) {
				if (!map_auto_generate_portal_collision((portal->min.x+xadd[n]),portal->min.z,(portal->max.x+xadd[n]),portal->max.z,n)) {
					portal->min.x+=xadd[n];
					portal->max.x+=xadd[n];
					moved=TRUE;
				}
			}

			if (((zadd[n]>0) && (z<cz)) || ((zadd[n]<0) && (z>cz))) {
				if (!map_auto_generate_portal_collision(portal->min.x,(portal->min.z+zadd[n]),portal->max.x,(portal->max.z+zadd[n]),n)) {
					portal->min.z+=zadd[n];
					portal->max.z+=zadd[n];
					moved=TRUE;
				}
			}

		}

		if (!moved) break;

		move_cnt++;
	}
}

/* =======================================================

      Connect Portals
      
======================================================= */

void map_auto_generate_connect_portals(map_type *map)
{
	int							n,k,corridor_sz,corridor_rand_sz,corridor_min_sz,portal_sz,
								portal_merge_distance,portal_connect_distance,connect_sz,
								x,z,ex,ez,x2,z2,ex2,ez2,dist,nportal,cnt,split_factor;
	auto_generate_box_type		*chk_portal,*cnt_portal;

		// get sizes

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	corridor_sz=(int)(((float)portal_sz)*ag_constant_corridor_size_percent);
	corridor_rand_sz=(int)(((float)corridor_sz)*ag_constant_corridor_random_percent);
	corridor_min_sz=corridor_sz-corridor_rand_sz;

	portal_merge_distance=(int)(((float)portal_sz)*ag_constant_portal_merge_percent);
	portal_connect_distance=(int)(((float)portal_sz)*ag_constant_portal_connect_percent);
	
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

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

			ag_boxes[ag_box_count].corridor_connect_box_idx[0]=n;
			ag_boxes[ag_box_count].corridor_connect_box_idx[1]=k;

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

			ag_boxes[ag_box_count].corridor_connect_box_idx[0]=n;
			ag_boxes[ag_box_count].corridor_connect_box_idx[1]=k;

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
	int							n,by_add,portal_high,portal_high_story_add,
								corridor_high,y,ty,by,extra_ty,extra_by,portal_sz;
	auto_generate_box_type		*portal;
	
		// portal sizes

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	portal_high=(int)(((float)portal_sz)*ag_constant_portal_high_percent);
	portal_high_story_add=(int)(((float)portal_sz)*ag_constant_portal_story_high_add_percent);

	corridor_high=(int)(((float)portal_sz)*ag_constant_corridor_high_percent);

	ty=(map_max_size/2)-(portal_high/2);
	by=(map_max_size/2)+(portal_high/2);
	
	extra_ty=(int)(((float)portal_high)*ag_constant_portal_high_extra_top);
	extra_by=(int)(((float)portal_high)*ag_constant_portal_high_extra_bottom);

		// create room portals

	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {
	
		if (portal->corridor_flag==ag_corridor_flag_portal) {
		
			portal->min.y=ty-map_auto_generate_random_int(extra_ty);

				// touching rooms get second stories
				
			if ((map_auto_generate_portal_touching_any(n)) && (ag_settings.second_story)) {
				portal->min.y-=portal_high_story_add;
			}
			
			by_add=map_auto_generate_random_int(extra_by);
			if (by_add<=ag_constant_step_corridor_high) {
				by_add=0;
			}
			else {
				by_add/=ag_constant_step_corridor_high;
				if (by_add!=0) by_add=(by_add*ag_constant_step_corridor_high);
			}

			portal->max.y=by+by_add;

				// fit within map_enlarge

			portal->min.y/=map_enlarge;
			portal->min.y*=map_enlarge;

			portal->max.y/=map_enlarge;
			portal->max.y*=map_enlarge;
		}

		portal++;
	}

		// create corridor portals

	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {
	
		if (portal->corridor_flag!=ag_corridor_flag_portal) {

				// corridor stick to highest
				// room on one of their sides

			y=ag_boxes[portal->corridor_connect_box_idx[0]].max.y;
			if (ag_boxes[portal->corridor_connect_box_idx[1]].max.y<y) y=ag_boxes[portal->corridor_connect_box_idx[1]].max.y;

			portal->min.y=y-corridor_high;
			portal->max.y=y;

				// fit within map_enlarge

			portal->min.y/=map_enlarge;
			portal->min.y*=map_enlarge;

			portal->max.y/=map_enlarge;
			portal->max.y*=map_enlarge;
		}

		portal++;
	}
}

/* =======================================================

      Auto-Generate Walls
      
======================================================= */

bool map_auto_generate_mesh_window_last_poly(map_type *map,int x,int z,int *px,int *py,int *pz)
{
	int				n,mx,my,mz,old_txt_idx,
					kx[4],ky[4],kz[4];
	float			gx[8],gy[8];
	
		// time for a window?
		
	if (!ag_settings.window) return(FALSE);
	if (map_auto_generate_random_int(100)>(int)(100.0f*ag_constant_window_percent)) return(FALSE);

		// create window hole
		
	map_auto_generate_mesh_punch_hole_last_poly(map,x,z);
	
		// window vertexes

	mx=my=mz=0;

	for (n=0;n!=4;n++) {
		mx+=px[n];
		my+=py[n];
		mz+=pz[n];
	}

	mx/=4;
	my/=4;
	mz/=4;

	for (n=0;n!=4;n++) {
		kx[n]=((px[n]-mx)/2)+mx;
		ky[n]=((py[n]-my)/2)+my;
		kz[n]=((pz[n]-mz)/2)+mz;
		
		kx[n]+=(x>>1);
		kz[n]+=(z>>1);
	}
	
	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;
	
	old_txt_idx=map_auto_generate_mesh_change_texture(ag_settings.texture.window);
	map_auto_generate_mesh_add_poly(map,4,kx,ky,kz,gx,gy);
	map_auto_generate_mesh_change_texture(old_txt_idx);

	return(TRUE);
}

void map_auto_generate_mesh_window_vert_frame_mesh(map_type *map,int rn,int ty,int by,int x,int z,int wid,bool reverse)
{
	int			high;

	x+=wid>>2;
	wid=wid>>1;

	x-=ag_constant_door_frame_depth;
	wid+=(ag_constant_door_frame_depth<<1);

	high=(by-ty)>>2;
	ty+=high;
	by-=high;

	map_auto_generate_vert_frame_mesh(map,rn,ty,by,x,z,wid,reverse,TRUE);
}

void map_auto_generate_mesh_window_horz_frame_mesh(map_type *map,int rn,int ty,int by,int x,int z,int wid,bool reverse)
{
	int			high;

	z+=wid>>2;
	wid=wid>>1;

	z-=ag_constant_door_frame_depth;
	wid+=(ag_constant_door_frame_depth<<1);

	high=(by-ty)>>2;
	ty+=high;
	by-=high;

	map_auto_generate_horz_frame_mesh(map,rn,ty,by,x,z,wid,reverse,TRUE);
}

void map_auto_generate_portal_walls_add(map_type *map,int rn,auto_generate_box_type *portal)
{
	int				x,z,xsz,zsz,ty,by,split_factor,portal_sz,
					px[8],py[8],pz[8];
	float			gx[8],gy[8];
	bool			lft_edge,rgt_edge,top_edge,bot_edge;

		// how we split the walls into a mesh

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

		// start mesh
		
	if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;
	
		// detect if we are on an outside edge of the map
	
	lft_edge=map_auto_generate_portal_on_map_left_edge(rn);
	rgt_edge=map_auto_generate_portal_on_map_right_edge(rn);
	top_edge=map_auto_generate_portal_on_map_top_edge(rn);
	bot_edge=map_auto_generate_portal_on_map_bottom_edge(rn);
	
		// portal height

	ty=portal->min.y;
	by=portal->max.y;
	
		// portal size

	xsz=portal->max.x-portal->min.x;
	zsz=portal->max.z-portal->min.z;

		// build left/right walls

	for (z=0;z<zsz;z+=split_factor) {

			// left

		if (!map_auto_generate_portal_horz_edge_block(rn,(z+portal->min.z),((z+portal->min.z)+split_factor),portal->min.x)) {
			map_auto_generate_poly_from_square_wall(0,z,0,(z+split_factor),ty,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			if ((lft_edge) && (z!=0) && (z!=(zsz-split_factor))) {
				if (map_auto_generate_mesh_window_last_poly(map,-ag_constant_window_depth,0,px,py,pz)) {
					map_auto_generate_mesh_window_horz_frame_mesh(map,rn,ty,by,0,z,split_factor,FALSE);
				}
			}
		}

			// right

		if (!map_auto_generate_portal_horz_edge_block(rn,(z+portal->min.z),((z+portal->min.z)+split_factor),portal->max.x)) {
			map_auto_generate_poly_from_square_wall(xsz,z,xsz,(z+split_factor),ty,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			if ((rgt_edge) && (z!=0) && (z!=(zsz-split_factor))) {
				if (map_auto_generate_mesh_window_last_poly(map,ag_constant_window_depth,0,px,py,pz)) {
					map_auto_generate_mesh_window_horz_frame_mesh(map,rn,ty,by,(portal->max.x-portal->min.x),z,split_factor,TRUE);
				}
			}
		}
	}

		// build top/bottom walls

	for (x=0;x<xsz;x+=split_factor) {

			// top

		if (!map_auto_generate_portal_vert_edge_block(rn,(x+portal->min.x),((x+portal->min.x)+split_factor),portal->min.z)) {
			map_auto_generate_poly_from_square_wall(x,0,(x+split_factor),0,ty,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			if ((top_edge) && (x!=0) && (x!=(xsz-split_factor))) {
				if (map_auto_generate_mesh_window_last_poly(map,0,-ag_constant_window_depth,px,py,pz)) {
					map_auto_generate_mesh_window_vert_frame_mesh(map,rn,ty,by,x,0,split_factor,FALSE);
				}
			}
		}

			// bottom

		if (!map_auto_generate_portal_vert_edge_block(rn,(x+portal->min.x),((x+portal->min.x)+split_factor),portal->max.z)) {
			map_auto_generate_poly_from_square_wall(x,zsz,(x+split_factor),zsz,ty,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			if ((bot_edge) && (x!=0) && (x!=(xsz-split_factor))) {
				if (map_auto_generate_mesh_window_last_poly(map,0,ag_constant_window_depth,px,py,pz)) {
					map_auto_generate_mesh_window_vert_frame_mesh(map,rn,ty,by,x,(portal->max.z-portal->min.z),split_factor,TRUE);
				}
			}
		}
	}
}


void map_auto_generate_corridor_walls_add(map_type *map,int rn,auto_generate_box_type *portal)
{
	int				xsz,zsz,ty,by,split_factor,slant_sz,portal_sz,
					px[8],py[8],pz[8];
	float			gx[8],gy[8];

		// how we split the walls into a mesh

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

		// start mesh
		
	if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor,FALSE,FALSE)) return;
	
		// portal height

	ty=portal->min.y;
	by=portal->max.y;
	
		// fix cooridors with slants in them
		
	slant_sz=(portal->max.y-portal->min.y)>>2;
	if ((corridor_types[rn]==ag_corridor_type_slanted_ceiling) || (corridor_types[rn]==ag_corridor_type_octagon)) ty+=slant_sz;
	if (corridor_types[rn]==ag_corridor_type_octagon) by-=slant_sz;
	
		// portal size

	xsz=portal->max.x-portal->min.x;
	zsz=portal->max.z-portal->min.z;

		// build left/right walls

	if (portal->corridor_flag==ag_corridor_flag_vertical) {
		map_auto_generate_poly_from_square_wall(0,0,0,zsz,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		map_auto_generate_mesh_effect_uv_last_poly(map,(float)(zsz/split_factor),1.0f,FALSE);

		map_auto_generate_poly_from_square_wall(xsz,0,xsz,zsz,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		map_auto_generate_mesh_effect_uv_last_poly(map,(float)(zsz/split_factor),1.0f,FALSE);
	}

		// build top/bottom walls

	if (portal->corridor_flag==ag_corridor_flag_horizontal) {
		map_auto_generate_poly_from_square_wall(0,0,xsz,0,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		map_auto_generate_mesh_effect_uv_last_poly(map,(float)(xsz/split_factor),1.0f,FALSE);

		map_auto_generate_poly_from_square_wall(0,zsz,xsz,zsz,ty,by,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		map_auto_generate_mesh_effect_uv_last_poly(map,(float)(xsz/split_factor),1.0f,FALSE);
	}
	
		// if a corridor, we need to lock uvs
		
	map_auto_generate_mesh_set_lock(map);
}

void map_auto_generate_walls(map_type *map)
{
	int							n;
	auto_generate_box_type		*portal;

		// create surrounding walls for portals

	portal=ag_boxes;

	for (n=0;n!=ag_box_count;n++) {

		if (portal->corridor_flag==ag_corridor_flag_portal) {
			map_auto_generate_portal_walls_add(map,n,portal);
		}
		else {
			map_auto_generate_corridor_walls_add(map,n,portal);
		}

		portal++;
	}
}

/* =======================================================

      Auto-Generate Height Connecting Walls
      
======================================================= */

void map_auto_generate_height_walls(map_type *map)
{
	int							n,k,x,ex,kx,z,ez,kz,xsz,zsz,split_factor,txt_idx,portal_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	auto_generate_box_type		*portal,*chk_portal;

		// how we split the walls into a mesh

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

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
			
			txt_idx=(portal->corridor_flag==ag_corridor_flag_portal)?ag_settings.texture.portal_wall:ag_settings.texture.corridor;
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

			if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.portal_wall,FALSE,TRUE)) return;

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

      Auto-Generate Ceiling Pillars
      
======================================================= */

void map_auto_generate_portal_single_ceilling_pillar(map_type *map,int rn,int lx2,int lz2,int rx2,int rz2,int ty,int by)
{
	int							k,split_factor,wall_sz,p_ty,p_by,portal_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	auto_generate_box_type		*portal;

	portal=&ag_boxes[rn];

		// get sizes
		
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	wall_sz=(int)((float)(portal->max.y-portal->min.y)*ag_constant_portal_ceiling_slant_percent);

		// create pillar

	k=(rx2-lx2)>>4;
	lx2+=k;
	rx2-=k;

	k=(rz2-lz2)>>4;
	lz2+=k;
	rz2-=k;

	k=wall_sz>>2;
	p_ty=ty+k;
	p_by=by-k;

	if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.column,FALSE,TRUE)) return;
	map_auto_generate_mesh_set_lock(map);

		// top of pillar
		
	map_auto_generate_mesh_change_texture(ag_settings.texture.column_base);

	map_auto_generate_poly_from_square_wall(lx2,lz2,rx2,lz2,ty,p_ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_wall(lx2,rz2,rx2,rz2,ty,p_ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_wall(lx2,lz2,lx2,rz2,ty,p_ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_wall(rx2,lz2,rx2,rz2,ty,p_ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,p_ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,2,FALSE);

		// bottom of pillar

	map_auto_generate_poly_from_square_wall(lx2,lz2,rx2,lz2,p_by,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_wall(lx2,rz2,rx2,rz2,p_by,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_wall(lx2,lz2,lx2,rz2,p_by,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_wall(rx2,lz2,rx2,rz2,p_by,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,1,FALSE);

	map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,p_by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,2,2,FALSE);

		// pillar

	map_auto_generate_mesh_change_texture(ag_settings.texture.column);
	
	k=(rx2-lx2)>>3;
	lx2+=k;
	rx2-=k;

	k=(rz2-lz2)>>3;
	lz2+=k;
	rz2-=k;

	map_auto_generate_poly_from_square_wall(lx2,lz2,rx2,lz2,p_ty,p_by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx2,rz2,rx2,rz2,p_ty,p_by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx2,lz2,lx2,rz2,p_ty,p_by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(rx2,lz2,rx2,rz2,p_ty,p_by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
}

void map_auto_generate_portal_ceilling_pillar(map_type *map,int rn,int lx2,int lz2,int rx2,int rz2,int ty,bool second_story)
{
	int							portal_sz,portal_high,cy;
	auto_generate_box_type		*portal;
		
	portal=&ag_boxes[rn];
	
		// single pillar
		
	if (!second_story) {
		map_auto_generate_portal_single_ceilling_pillar(map,rn,lx2,lz2,rx2,rz2,ty,portal->max.y);
		return;
	}

		// two pillars broken by a second story
		
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	portal_high=(int)(((float)portal_sz)*ag_constant_portal_high_percent);
	cy=(map_max_size>>1)-((portal_high>>1)+ag_constant_step_story_size);

	map_auto_generate_portal_single_ceilling_pillar(map,rn,lx2,lz2,rx2,rz2,ty,cy);
	map_auto_generate_portal_single_ceilling_pillar(map,rn,lx2,lz2,rx2,rz2,(cy+ag_constant_story_floor_high),portal->max.y);
}

/* =======================================================

      Auto-Generate Ceilings
      
======================================================= */

bool map_auto_generate_portal_ceiling_ok(unsigned char *data,int lx,int lz,int rx,int rz,int lx2,int lz2,int rx2,int rz2)
{
	int			k,portal_sz,split_factor,mx,mz;

		// in outer ring?

	if ((lx2==lx) || (rx2==rx) || (lz2==lz) || (rz2==rz)) return(data[ag_ceiling_outer_ring]!=0);

		// inside sections

	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

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
	int							portal_sz,ceiling_type,split_factor,lx2,rx2,lz2,rz2,
								k,kx,kz,mx,wall_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	unsigned char				*data;
	auto_generate_box_type		*portal;
		
	portal=&ag_boxes[rn];
	
		// get sizes
		
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
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
		
		if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor,FALSE,FALSE)) return;
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

		k=((rz-lz)>>1)/split_factor;
		lz2=k*split_factor;
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

		k=((rx-lx)>>1)/split_factor;
		lx2=k*split_factor;
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

		if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.portal_ceiling,FALSE,FALSE)) return;

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

	if (data[ag_ceiling_central_pillar]!=0) {
		k=((rx-lx)>>1)/split_factor;
		lx2=k*split_factor;
		rx2=lx2+split_factor;

		k=((rz-lz)>>1)/split_factor;
		lz2=k*split_factor;
		rz2=lz2+split_factor;

		map_auto_generate_portal_ceilling_pillar(map,rn,lx2,lz2,rx2,rz2,ty,portal->story_middle);
	}

	if (data[ag_ceiling_corner_pillars]!=0) {
		map_auto_generate_portal_ceilling_pillar(map,rn,lx,lz,(lx+split_factor),(lz+split_factor),ty,portal->story_top_left);
		map_auto_generate_portal_ceilling_pillar(map,rn,(rx-split_factor),lz,rx,(lz+split_factor),ty,portal->story_top_right);
		map_auto_generate_portal_ceilling_pillar(map,rn,lx,(rz-split_factor),(lx+split_factor),rz,ty,portal->story_bottom_left);
		map_auto_generate_portal_ceilling_pillar(map,rn,(rx-split_factor),(rz-split_factor),rx,rz,ty,portal->story_bottom_right);
	}
}

void map_auto_generate_corridor_ceiling_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty)
{
	int							split_factor,lx2,rx2,lz2,rz2,slant_sz,portal_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	bool						slant_piece;
	auto_generate_box_type		*portal;
	
		// get sizes
		
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	
	slant_piece=(corridor_types[rn]==ag_corridor_type_slanted_ceiling) || (corridor_types[rn]==ag_corridor_type_octagon);
		
	   // get portal slants

	portal=&ag_boxes[rn];
	
	slant_sz=(portal->max.y-portal->min.y)>>2;
	
		// create regular ceiling polygon
	
	lz2=lz;
	rz2=rz;
		
	if ((slant_piece) && (portal->corridor_flag==ag_corridor_flag_horizontal)) {
		lz2+=slant_sz;
		rz2-=slant_sz;
	}
	
	lx2=lx;
	rx2=rx;
		
	if ((slant_piece) && (portal->corridor_flag==ag_corridor_flag_vertical)) {
		lx2+=slant_sz;
		rx2-=slant_sz;
	}
	
	if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor,FALSE,FALSE)) return;
	map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,ty,px,py,pz,gx,gy);
	if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
	map_auto_generate_mesh_effect_uv_last_poly(map,(float)((rx-lx)/split_factor),(float)((rz-lz)/split_factor),(portal->corridor_flag==ag_corridor_flag_vertical));
	
		// additional slanted ceilings for corridors
		
	if (slant_piece) {

		if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor,FALSE,FALSE)) return;

		if (portal->corridor_flag==ag_corridor_flag_horizontal) {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,rx,(lz+slant_sz),ty,slant_sz,ag_ceiling_lower_neg_z,TRUE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
			map_auto_generate_mesh_effect_uv_last_poly(map,(float)((rx-lx)/split_factor),1.0f,FALSE);

			map_auto_generate_poly_from_square_floor_slant(lx,(rz-slant_sz),rx,rz,ty,slant_sz,ag_ceiling_lower_pos_z,TRUE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
			map_auto_generate_mesh_effect_uv_last_poly(map,(float)((rx-lx)/split_factor),1.0f,FALSE);
		}
		else {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,(lx+slant_sz),rz,ty,slant_sz,ag_ceiling_lower_neg_x,TRUE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
			map_auto_generate_mesh_effect_uv_last_poly(map,1.0f,(float)((rz-lz)/split_factor),TRUE);

			map_auto_generate_poly_from_square_floor_slant((rx-slant_sz),lz,rx,rz,ty,slant_sz,ag_ceiling_lower_pos_x,TRUE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
			map_auto_generate_mesh_effect_uv_last_poly(map,1.0f,(float)((rz-lz)/split_factor),TRUE);
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
								xadd,zadd,portal_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	auto_generate_box_type		*portal;
		
	portal=&ag_boxes[rn];
	
		// get sizes
		
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	
		// create floor polys

	zadd=0;
	lz2=lz;

	while (lz2<rz) {

		rz2=lz2+split_factor;

		xadd=0;
		lx2=lx;

		while (lx2<rx) {

			rx2=lx2+split_factor;

			if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.portal_floor,FALSE,FALSE)) return;

				// create flat polygon

			map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,by,px,py,pz,gx,gy);

				// rebuild the Ys

			py[0]=py[1]=py[2]=py[3]=by;

				// generate polygon

			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

			lx2=rx2;
			xadd++;
		}
		
		lz2=rz2;
		zadd++;
	}
}

void map_auto_generate_corridor_floor_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int by)
{
	int							split_factor,lx2,rx2,lz2,rz2,slant_sz,portal_sz,
								px[8],py[8],pz[8];
	float						gx[8],gy[8];
	bool						slant_piece;
	auto_generate_box_type		*portal;
	
		// get sizes
		
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	
	slant_piece=(corridor_types[rn]==ag_corridor_type_octagon);
		
	   // get portal slants

	portal=&ag_boxes[rn];
	
	slant_sz=(portal->max.y-portal->min.y)>>2;
	
		// create regular floor polygon

	lz2=lz;
	rz2=rz;
	
	if ((slant_piece) && (portal->corridor_flag==ag_corridor_flag_horizontal)) {
		lz2+=slant_sz;
		rz2-=slant_sz;
	}
	
	lx2=lx;
	rx2=rx;

	if ((slant_piece) && (portal->corridor_flag==ag_corridor_flag_vertical)) {
		lx2+=slant_sz;
		rx2-=slant_sz;
	}

	if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor_floor,FALSE,FALSE)) return;
	map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
	map_auto_generate_mesh_effect_uv_last_poly(map,(float)((rx-lx)/split_factor),(float)((rz-lz)/split_factor),(portal->corridor_flag==ag_corridor_flag_vertical));
	
		// additional slanted floors for corridors
		
	if (slant_piece) {
		
		if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor,FALSE,FALSE)) return;

		if (portal->corridor_flag==ag_corridor_flag_horizontal) {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,rx,(lz+slant_sz),by,slant_sz,ag_ceiling_lower_neg_z,FALSE,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			map_auto_generate_mesh_effect_uv_last_poly(map,(float)((rx-lx)/split_factor),1.0f,FALSE);

			map_auto_generate_poly_from_square_floor_slant(lx,(rz-slant_sz),rx,rz,by,slant_sz,ag_ceiling_lower_pos_z,FALSE,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			map_auto_generate_mesh_effect_uv_last_poly(map,(float)((rx-lx)/split_factor),1.0f,FALSE);
		}
		else {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,(lx+slant_sz),rz,by,slant_sz,ag_ceiling_lower_neg_x,FALSE,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			map_auto_generate_mesh_effect_uv_last_poly(map,1.0f,(float)((rz-lz)/split_factor),TRUE);

			map_auto_generate_poly_from_square_floor_slant((rx-slant_sz),lz,rx,rz,by,slant_sz,ag_ceiling_lower_pos_x,FALSE,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			map_auto_generate_mesh_effect_uv_last_poly(map,1.0f,(float)((rz-lz)/split_factor),TRUE);
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

	by=y+ag_constant_story_floor_high;

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
	int							portal_sz,split_factor;
	auto_generate_box_type		*portal;
	
	portal=&ag_boxes[portal_idx];
	
	z+=portal->min.z;
	ez+=portal->min.z;
	
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	
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
	int							portal_sz,split_factor;
	auto_generate_box_type		*portal;
	
	portal=&ag_boxes[portal_idx];
	
	x+=portal->min.x;
	ex+=portal->min.x;
	
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);
	
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
								x,y,z,by,mx,mz,xsz,zsz,stair_type,portal_sz;
	bool						lft,rgt,top,bot,horz,vert,
								old_lft,old_rgt,old_top,old_bot;
	unsigned char				*poly_map;
	auto_generate_box_type		*portal;
	
	if (!ag_settings.second_story) return;

		// get sizes
		
	portal_sz=(int)(((float)ag_settings.map.map_sz)*ag_constant_portal_percent);
	portal_high=(int)(((float)portal_sz)*ag_constant_portal_high_percent);
	extra_ty=(int)(((float)portal_high)*ag_constant_portal_high_extra_top);

	split_factor=(int)(((float)portal_sz)*ag_constant_portal_split_factor_percent);

		// create second story in touching portals

	for (n=0;n!=ag_box_count;n++) {

		portal=&ag_boxes[n];
		if (portal->corridor_flag!=ag_corridor_flag_portal) continue;
		
			// initially no second story
			
		portal->story_top_left=FALSE;
		portal->story_top_right=FALSE;
		portal->story_bottom_left=FALSE;
		portal->story_bottom_right=FALSE;
		portal->story_middle=FALSE;
		
		if (!ag_settings.second_story) continue;

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
		
			// remember where second story is
			
		portal->story_top_left=top||lft;
		portal->story_top_right=top||rgt;
		portal->story_bottom_left=bot||lft;
		portal->story_bottom_right=bot||rgt;
		portal->story_middle=horz||vert;

			// create polygon map

		xsz=(portal->max.x-portal->min.x)/split_factor;
		zsz=(portal->max.z-portal->min.z)/split_factor;

		sz=(xsz+1)*(zsz+1);
		poly_map=(unsigned char*)malloc(sz);
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

		y=(map_max_size>>1)-((portal_high>>1)+ag_constant_step_story_size);

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
		
		step_len=map_auto_generate_steps_get_length(y,by,ag_constant_step_story_size,ag_constant_step_story_high);
		stair_type=map_auto_generate_get_stair_type(&ag_settings);

		if ((lft) && (!old_lft)) {
			x=split_factor*2;
			z=map_auto_generate_second_story_steps_get_z(n,x,(x+step_len),top,zsz,step_wid);
			
			switch (stair_type) {
				case ag_stair_type_stair:
					map_auto_generate_steps_mesh(map,n,ag_step_second_story,step_wid,ag_constant_step_story_high,(y+ag_constant_story_floor_high),by,x,z,270.0f);
					break;
				case ag_stair_type_lift:
					map_auto_generate_lift(map,n,ag_constant_step_story_high,y,by,x,(x+step_wid),z,(z+step_wid));
					break;
			}

		}
		else {
			if ((rgt) && (!old_rgt)) {
				x=xsz-(split_factor*2);
				z=map_auto_generate_second_story_steps_get_z(n,(x-step_len),x,top,zsz,step_wid);

				switch (stair_type) {
					case ag_stair_type_stair:
						map_auto_generate_steps_mesh(map,n,ag_step_second_story,step_wid,ag_constant_step_story_high,(y+ag_constant_story_floor_high),by,x,z,90.0f);
						break;
					case ag_stair_type_lift:
						map_auto_generate_lift(map,n,ag_constant_step_story_high,y,by,(x-step_wid),x,z,(z+step_wid));
						break;
				}

			}
		}

		if ((top) && (!old_top)) {
			z=split_factor*2;
			x=map_auto_generate_second_story_steps_get_x(n,z,(z+step_len),lft,xsz,step_wid);

			switch (stair_type) {
				case ag_stair_type_stair:
					map_auto_generate_steps_mesh(map,n,ag_step_second_story,step_wid,ag_constant_step_story_high,(y+ag_constant_story_floor_high),by,x,z,0.0f);
					break;
				case ag_stair_type_lift:
					map_auto_generate_lift(map,n,ag_constant_step_story_high,y,by,x,(x+step_wid),z,(z+step_wid));
					break;
			}

		}
		else {
			if ((bot) && (!old_bot)) {
				z=zsz-(split_factor*2);
				x=map_auto_generate_second_story_steps_get_x(n,(z-step_len),z,lft,xsz,step_wid);

				switch (stair_type) {
					case ag_stair_type_stair:
						map_auto_generate_steps_mesh(map,n,ag_step_second_story,step_wid,ag_constant_step_story_high,(y+ag_constant_story_floor_high),by,x,z,180.0f);
						break;
					case ag_stair_type_lift:
						map_auto_generate_lift(map,n,ag_constant_step_story_high,y,by,x,(x+step_wid),(z-step_wid),z);
						break;
				}

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

	if (ag_settings.type==ag_type_room_and_corridor) {
		map_auto_generate_merge_portals();
		map_auto_generate_connect_portals(map);
	}
	else {
		map_auto_generate_merge_portals_center();
	}

	map_auto_generate_portal_y();
	
		// walls and floors

	map_auto_generate_walls(map);
	map_auto_generate_height_walls(map);
	map_auto_generate_corridor_clip_walls(map);
	map_auto_generate_floors(map);
	
		// second stories and ceilings
		
	map_auto_generate_second_story(map);
	map_auto_generate_ceilings(map);
	
		// create ramps, steps, doors and frames
		
	map_auto_generate_ramps(map);
	map_auto_generate_corridor_to_portal_connections(map);
	map_auto_generate_doors(map);
		
		// rebuild the UVs
		
	map_auto_generate_reset_UVs(map);
	
		// create lights, nodes, and
		// player spot
		
	map_auto_generate_lights(map);
	map_auto_generate_nodes(map);
	map_auto_generate_add_player_spot(map);
	
		// center map
		
	map_center(map);
}

/* =======================================================

      Auto-Generate Test
      
======================================================= */

bool map_auto_generate_test(map_type *map)
{
	int								n;
	auto_generate_settings_type		ags;

		// start new map

	if (!map_new(map,"DM Map")) return(FALSE);

		// setup textures

	strcpy(map->textures[0].frames[0].name,"demo_main_wall");
	strcpy(map->textures[1].frames[0].name,"demo_main_floor");
	strcpy(map->textures[2].frames[0].name,"demo_ceiling");
	strcpy(map->textures[3].frames[0].name,"demo_corridor_wall");
	strcpy(map->textures[4].frames[0].name,"demo_second_story");
	strcpy(map->textures[5].frames[0].name,"demo_chain_link");
	strcpy(map->textures[6].frames[0].name,"demo_door");
	strcpy(map->textures[7].frames[0].name,"demo_steps");
	strcpy(map->textures[8].frames[0].name,"demo_ramp");
	strcpy(map->textures[9].frames[0].name,"demo_column");
	strcpy(map->textures[10].frames[0].name,"demo_column_base");
	strcpy(map->textures[11].frames[0].name,"demo_frame");
	strcpy(map->textures[12].frames[0].name,"demo_corridor_floor");
	strcpy(map->textures[13].frames[0].name,"demo_water");
	strcpy(map->textures[14].frames[0].name,"demo_sky");

		// setup auto generation values

#ifndef D3_OS_WINDOWS
	ags.seed=TickCount();
#else
	ags.seed=GetTickCount();
#endif

	ags.type=ag_type_room_and_corridor;
	ags.flow=ag_flow_none;

	ags.second_story=TRUE;
	
	ags.map.map_sz=250000;

	for (n=0;n!=ag_ceiling_type_count;n++) {
		ags.ceiling_type_on[n]=TRUE;
	}
	
	for (n=0;n!=ag_corridor_type_count;n++) {
		ags.corridor_type_on[n]=TRUE;
	}

	for (n=0;n!=ag_stair_type_count;n++) {
		ags.stair_type_on[n]=TRUE;
	}

	for (n=0;n!=ag_door_type_count;n++) {
		ags.door_type_on[n]=TRUE;
	}
	
	for (n=0;n!=ag_light_type_count;n++) {
		ags.light_type_on[n]=TRUE;
	}
	
	ags.texture.portal_wall=0;
	ags.texture.portal_floor=1;
	ags.texture.portal_ceiling=2;
	ags.texture.steps=7;
	ags.texture.ramp=8;
	ags.texture.column=9;
	ags.texture.column_base=10;
	ags.texture.corridor=3;
	ags.texture.corridor_floor=12;
	ags.texture.window=5;
	ags.texture.door=6;
	ags.texture.second_story=4;
	ags.texture.frame=11;
	
	ags.window=TRUE;
	ags.frame=TRUE;
	
	strcpy(ags.sound.door,"Door");
	strcpy(ags.sound.lift,"Lift");

		// create the map

	map_auto_generate(map,&ags);

		// load textures

	if (!map_textures_read(map,TRUE)) return(FALSE);

	return(TRUE);
}
