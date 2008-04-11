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

extern bool map_auto_generate_portal_collision(map_type *map,int x,int z,int ex,int ez,int skip_idx);
extern int map_auto_generate_portal_find_to_left(map_type *map,portal_type *org_portal);
extern int map_auto_generate_portal_find_to_right(map_type *map,portal_type *org_portal);
extern int map_auto_generate_portal_find_to_top(map_type *map,portal_type *org_portal);
extern int map_auto_generate_portal_find_to_bottom(map_type *map,portal_type *org_portal);

extern int map_auto_generate_get_corridor_type(auto_generate_settings_type *ags);
extern int map_auto_generate_get_ceiling_type(auto_generate_settings_type *ags);

extern void map_auto_generate_block_preset(auto_generate_settings_type *ag_settings,int block);
extern bool map_auto_generate_block_collision(auto_generate_settings_type *ags,int x,int z,int ex,int ez);

extern void map_auto_generate_poly_from_square_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor(int lx,int lz,int rx,int rz,int fy,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor_slant(int lx,int lz,int rx,int rz,int fy,int yadd,int lower_mode,bool reverse_slant,int *x,int *y,int *z,float *gx,float *gy);

extern bool map_auto_generate_mesh_start(map_type *map,int portal_idx,int group_idx,int txt_idx,bool moveable,bool new_mesh);
extern bool map_auto_generate_mesh_add_poly(map_type *map,int ptsz,int *x,int *y,int *z,float *gx,float *gy);

extern void map_auto_generate_segment_start(int group_idx,int primitive_uid,int fill,bool moveable);
extern int map_auto_generate_segment_wall(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty,int by,int clip);
extern void map_auto_generate_segment_fc(map_type *map,int rn,int type,int lx,int lz,int rx,int rz,int y,int yadd,int lower_mode);

unsigned char						corridor_flags[max_portal],
									corridor_types[max_portal];

auto_generate_settings_type			ag_settings;

/* =======================================================

      Move Segment Between a Portal
      
======================================================= */

void map_auto_generate_move_wall_segment_to_portal(map_type *map,int rn,int seg_idx)
{
	int				lx,lz,rx,rz;
	portal_type		*portal;
	segment_type	*seg;
	
	seg=&map->segments[seg_idx];
	portal=&map->portals[seg->rn];
	
	lx=seg->data.wall.lx+portal->x;
	rx=seg->data.wall.rx+portal->x;
	lz=seg->data.wall.lz+portal->z;
	rz=seg->data.wall.rz+portal->z;

	portal=&map->portals[rn];
	
	seg->data.wall.lx=lx-portal->x;
	seg->data.wall.rx=rx-portal->x;
	seg->data.wall.lz=lz-portal->z;
	seg->data.wall.rz=rz-portal->z;

	seg->rn=rn;
}

/* =======================================================

      Generate Initial Unconnected Portals
      
======================================================= */

void map_auto_generate_initial_portals(map_type *map)
{
	int			i,rn,x,z,ex,ez,initial_count,try_count,split_factor,
				map_x_sz,map_z_sz,portal_rand_sz,portal_min_sz;

		// sizes

	map_x_sz=ag_settings.map.right-ag_settings.map.left;
	map_z_sz=ag_settings.map.bottom-ag_settings.map.top;

	portal_rand_sz=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_random_percent);
	portal_min_sz=ag_settings.portal.sz-portal_rand_sz;
	
	split_factor=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_split_factor_percent);
	
		// initial count of portals
	
	initial_count=(ag_settings.map.sz/ag_settings.portal.sz)*4;

		// create portals

	map->nportal=0;

	for (i=0;i!=initial_count;i++) {
		
		try_count=0;

		while (TRUE) {

				// create portal start

			x=ag_settings.map.left+(rand()%map_x_sz);
			z=ag_settings.map.top+(rand()%map_z_sz);
			
				// create portal size
				
			ex=x+(portal_min_sz+(rand()%portal_rand_sz));
			ez=z+(portal_min_sz+(rand()%portal_rand_sz));

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
				
			if ((!map_auto_generate_block_collision(&ag_settings,x,z,ex,ez)) && (!map_auto_generate_portal_collision(map,x,z,ex,ez,-1))) break;
			
				// try to much?
				
			try_count++;
			if (try_count==100) return;
		}

			// create portal
			
		rn=map_portal_create(map,x,z,ex,ez);
		if (rn==-1) break;

			// create initial mesh

		if (map_portal_mesh_add(map,rn)==-1) break;
	}
}

/* =======================================================

      Merge Close Portals
      
======================================================= */

void map_auto_generate_merge_portals(map_type *map)
{
	int			i,n,k,dist,portal_merge_distance,
				split_factor,merge_try_count;
	bool		moved;
	portal_type	*chk_portal,*merge_portal;

	portal_merge_distance=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_merge_percent);

		// attempt to merge portals within a certain distance
		// together.  All portals are grided on the split
		// factor, so we merge on that

		// only keep moving for a limited time
		// so portals that bounce between two don't cause
		// an infinite loop
		
	split_factor=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_split_factor_percent);
	merge_try_count=(portal_merge_distance/split_factor)*2;

	for (i=0;i!=merge_try_count;i++) {

		moved=FALSE;		// keep looping until no more merging

		for (n=0;n!=map->nportal;n++) {
			chk_portal=&map->portals[n];

				// only move portals in one direction

				// slowly move portals towards merges
				// so they can merge on multiple sides and
				// not interfere with other merges

			for (k=0;k!=map->nportal;k++) {
				if (k==n) break;
				
				merge_portal=&map->portals[k];

					// merge left

				dist=merge_portal->x-chk_portal->ex;
				if ((dist>0) && (dist<=portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(map,(merge_portal->x-split_factor),merge_portal->z,merge_portal->ex,merge_portal->ez,k)) {
						merge_portal->x-=split_factor;
						moved=TRUE;
						break;
					}
				}

					// merge right

				dist=chk_portal->x-merge_portal->ex;
				if ((dist>0) && (dist<=portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(map,merge_portal->x,merge_portal->z,(merge_portal->ex+split_factor),merge_portal->ez,k)) {
						merge_portal->ex+=split_factor;
						moved=TRUE;
						break;
					}
				}

					// merge top

				dist=merge_portal->z-chk_portal->ez;
				if ((dist>0) && (dist<=portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(map,merge_portal->x,(merge_portal->z-split_factor),merge_portal->ex,merge_portal->ez,k)) {
						merge_portal->z-=split_factor;
						moved=TRUE;
						break;
					}
				}

					// merge bottom

				dist=chk_portal->z-merge_portal->ez;
				if ((dist>0) && (dist<=portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(map,merge_portal->x,merge_portal->z,merge_portal->ex,(merge_portal->ez+split_factor),k)) {
						merge_portal->ez+=split_factor;
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
	int				n,k,rn,corridor_sz,corridor_rand_sz,corridor_min_sz,
					portal_merge_distance,portal_connect_distance,connect_sz,
					x,z,ex,ez,x2,z2,ex2,ez2,dist,nportal,cnt,split_factor;
	portal_type		*chk_portal,*cnt_portal;

		// get sizes

	corridor_sz=(int)(((float)ag_settings.portal.sz)*ag_constant_corridor_size_percent);
	corridor_rand_sz=(int)(((float)corridor_sz)*ag_constant_corridor_random_percent);
	corridor_min_sz=corridor_sz-corridor_rand_sz;

	portal_merge_distance=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_merge_percent);
	portal_connect_distance=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_connect_percent);
	
	split_factor=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_split_factor_percent);

		// get original portal count

	nportal=map->nportal;

		// check against all portals

	for (n=0;n!=nportal;n++) {
		chk_portal=&map->portals[n];

			// check for portals that fit horizontal connections

		cnt=0;

		for (k=0;k!=nportal;k++) {
			cnt_portal=&map->portals[k];

				// any more portals?

			if (map->nportal>=max_portal) return;

				// portals miss each other?

			if (chk_portal->z>=cnt_portal->ez) continue;
			if (chk_portal->ez<=cnt_portal->z) continue;

				// too close or too far away?

			dist=cnt_portal->x-chk_portal->ex;
			if (dist<=0) continue;
			if (dist<portal_merge_distance) continue;
			if (dist>portal_connect_distance) continue;

				// get portal size

			x=chk_portal->ex;
			ex=cnt_portal->x;

			connect_sz=corridor_min_sz+(rand()%corridor_rand_sz);
			z=(((chk_portal->z+chk_portal->ez)/2)+((cnt_portal->z+cnt_portal->ez)/2))/2;
			z-=(connect_sz/2);
			ez=z+connect_sz;

			if (z<chk_portal->z) z=chk_portal->z;
			if (z<cnt_portal->z) z=cnt_portal->z;
			if (ez>chk_portal->ez) ez=chk_portal->ez;
			if (ez>cnt_portal->ez) ez=cnt_portal->ez;
			
				// fix vertical

			z/=split_factor;
			z*=split_factor;
			
			ez/=split_factor;
			ez*=split_factor;

				// make sure corridor isn't in corner

			if ((z==chk_portal->z) || (z==cnt_portal->z)) {
				z+=split_factor;
			}

			if ((ez==chk_portal->ez) || (ez==cnt_portal->ez)) {
				ez-=split_factor;
			}

			if (z>=ez) continue;

				// not enough connection size for portals?

			if ((ez-z)<corridor_min_sz) continue;
			
				// check for collisions
				
			z2=z-split_factor;
			ez2=ez+split_factor;			// don't create if z's will touch other portals, then it's no longer a corridor

			if ((map_auto_generate_block_collision(&ag_settings,x,z2,ex,ez2)) || (map_auto_generate_portal_collision(map,x,z2,ex,ez2,-1))) continue;

				// create portal

			corridor_flags[map->nportal]=ag_corridor_flag_horizontal;
			corridor_types[map->nportal]=map_auto_generate_get_corridor_type(&ag_settings);

			rn=map_portal_create(map,x,z,ex,ez);
			if (rn==-1) break;

				// only allow up to 2 connections in a direction

			cnt++;
			if (cnt==2) break;
		}

			// check for portals that fit vertical connections

		cnt=0;

		for (k=0;k!=nportal;k++) {
			cnt_portal=&map->portals[k];

				// any more portals?

			if (map->nportal>=max_portal) return;

				// portals miss each other?

			if (chk_portal->x>=cnt_portal->ex) continue;
			if (chk_portal->ex<=cnt_portal->x) continue;

				// too close or too far away?

			dist=cnt_portal->z-chk_portal->ez;
			if (dist<=0) continue;
			if (dist<portal_merge_distance) continue;
			if (dist>portal_connect_distance) continue;

				// get portal size

			z=chk_portal->ez;
			ez=cnt_portal->z;

			connect_sz=corridor_min_sz+(rand()%corridor_rand_sz);
			x=(((chk_portal->x+chk_portal->ex)/2)+((cnt_portal->x+cnt_portal->ex)/2))/2;
			x-=(connect_sz/2);
			ex=x+connect_sz;

			if (x<chk_portal->x) x=chk_portal->x;
			if (x<cnt_portal->x) x=cnt_portal->x;
			if (ex>chk_portal->ex) ex=chk_portal->ex;
			if (ex>cnt_portal->ex) ex=cnt_portal->ex;
			
				// fix horizontal

			x/=split_factor;
			x*=split_factor;
			
			ex/=split_factor;
			ex*=split_factor;

				// make sure corridor isn't in corner

			if ((x==chk_portal->x) || (x==cnt_portal->x)) {
				x+=split_factor;
			}

			if ((ex==chk_portal->ex) || (ex==cnt_portal->ex)) {
				ex-=split_factor;
			}

			if (x>=ex) continue;

				// not enough connection size for portals?

			if ((ex-x)<corridor_min_sz) continue;
			
				// check for collisions
				
			x2=x-split_factor;
			ex2=ex+split_factor;			// don't create if x's will touch other portals, then it's no longer a corridor

			if ((map_auto_generate_block_collision(&ag_settings,x2,z,ex2,ez)) || (map_auto_generate_portal_collision(map,x2,z,ex2,ez,-1))) continue;

				// create portal
				
			corridor_flags[map->nportal]=ag_corridor_flag_vertical;
			corridor_types[map->nportal]=map_auto_generate_get_corridor_type(&ag_settings);

			rn=map_portal_create(map,x,z,ex,ez);
			if (rn==-1) break;

				// only allow up to 2 connections in a direction

			cnt++;
			if (cnt==2) break;
		}
	}
}

/* =======================================================

      Setup Random Y for Portals
      
======================================================= */

void map_auto_generate_portal_y(map_type *map)
{
	int				n,corridor_slop_y,by_add,ty,by,extra_ty,extra_by;
	portal_type		*portal;
	
		// portal sizes
		
	ty=(map_max_size/2)-(ag_settings.portal.high/2);
	by=(map_max_size/2)+(ag_settings.portal.high/2);
	
	extra_ty=(int)(((float)ag_settings.portal.high)*ag_constant_portal_high_extra_top);
	extra_by=(int)(((float)ag_settings.portal.high)*ag_constant_portal_high_extra_bottom);

		// corridor top slop (so not at very top of portal)

	corridor_slop_y=(by-ty)/5;

		// create portal y

	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
	
			// rooms have variable Ys
			
		if (corridor_flags[n]==ag_corridor_flag_portal) {
			portal->ty=ty-(rand()%extra_ty);

			by_add=(rand()%extra_by);
			if (by_add<=(ag_settings.steps.high+1)) {
				by_add=0;
			}
			else {
				by_add/=ag_settings.steps.high;
				if (by_add!=0) by_add=((by_add-1)*ag_settings.steps.high);
			}

			portal->by=by+by_add;
		}
		
			// corridors have static Ys
			
		else {
			portal->ty=(ty+corridor_slop_y);
			portal->by=by;
		}
		
		portal++;
	}
}

/* =======================================================

      Create Lights
      
======================================================= */

void map_auto_generate_lights(map_type *map)
{
	int					n,k,x,z,y,intensity,lt_type;
	float				r,g,b;
	double				dx,dz;
	portal_type			*portal;
	map_light_type		*lit;
	
	if (!ag_settings.lights) return;

	portal=map->portals;
	
	for (n=0;n!=map->nportal;n++) {

		if (map->nlight>=max_map_light) break;
	
			// find light position
			
		x=(portal->ex-portal->x)/2;
		z=(portal->ez-portal->z)/2;
		y=portal->ty+((portal->by-portal->ty)/2);
		
			// add in the fudge
			
		if (ag_settings.light_fudge_factor>=2) {
			x+=((rand()%(ag_settings.light_fudge_factor/2))-ag_settings.light_fudge_factor);
			z+=((rand()%(ag_settings.light_fudge_factor/2))-ag_settings.light_fudge_factor);
		}
		
			// get intensity
			
		dx=(portal->ex-portal->x);
		dz=(portal->ez-portal->z);
		intensity=(int)sqrt((dx*dx)+(dz*dz));
		
		intensity=(intensity*ag_settings.light_fill_percentage)/200;			// radius, so use half
		if (intensity<100) intensity=100;
		
			// get the color
			
		r=g=b=1.0f;
		
		if (ag_settings.light_color_percentage!=100) {
			
			k=ag_settings.light_color_percentage+(rand()%(100-ag_settings.light_color_percentage));
			
			switch (rand()%3) {
				case 0:
					g=(g*((float)k))/100.0f;
					b=(b*((float)k))/100.0f;
					break;
				case 1:
					r=(r*((float)k))/100.0f;
					b=(b*((float)k))/100.0f;
					break;
				case 2:
					r=(r*((float)k))/100.0f;
					g=(g*((float)k))/100.0f;
					break;
			}
		}
		
			// get the type
			
		lt_type=lt_normal;
		
		if (ag_settings.light_flicker_percentage>(rand()%100)) {
			switch (rand()%3) {
				case 0:
					lt_type=lt_glow;
					break;
				case 1:
					lt_type=lt_pulse;
					break;
				case 2:
					lt_type=lt_flicker;
					break;
			}
		}
		
			// create the light
	
		lit=&map->lights[map->nlight];
		map->nlight++;
		
		lit->pos.rn=n;
		lit->pos.x=x;
		lit->pos.z=z;
		lit->pos.y=y;
		lit->type=lt_type;
		lit->col.r=r;
		lit->col.g=g;
		lit->col.b=b;
		lit->intensity=intensity;
		lit->confine_to_portal=(corridor_flags[n]!=ag_corridor_flag_portal);
		lit->on=TRUE;

		portal++;
	}
}

/* =======================================================

      Auto-Generate Walls
      
======================================================= */

void map_auto_generate_walls_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty,int by)
{
	int				lx2,rx2,lz2,rz2,px[8],py[8],pz[8],
					xadd,zadd,split_factor;
	float			gx[8],gy[8];
	
		// get sizes
		
	split_factor=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_split_factor_percent);

	   // horizontal walls

	if (lz==rz) {

		xadd=split_factor-(map->portals[rn].x%split_factor);

		rx2=lx;

		while (rx2!=rx) {
			lx2=rx2;
			rx2=lx2+xadd;
			if (rx2>=rx) rx2=rx;
			xadd=split_factor;

			map_auto_generate_poly_from_square_wall(lx2,lz,rx2,lz,ty,by,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
		}

		return;
	}

	   // vertical walls

	if (lx==rx) {

		zadd=split_factor-(map->portals[rn].z%split_factor);

		rz2=lz;

		while (rz2!=rz) {
			lz2=rz2;
			rz2=lz2+zadd;
			if (rz2>=rz) rz2=rz;
			zadd=split_factor;
			
			map_auto_generate_poly_from_square_wall(lx,lz2,lx,rz2,ty,by,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
		}

		return;
	}

	   // regular wall

	map_auto_generate_poly_from_square_wall(lx,lz,rx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
}

void map_auto_generate_walls(map_type *map)
{
	int				n,x,z,ex,ez,xsz,zsz,lx,rx,x2,lz,rz,z2,ty,by,txt_idx;
	char			*wall_left_run,*wall_right_run,*wall_top_run,*wall_bottom_run;
	bool			in_left_wall,in_right_wall,in_top_wall,in_bottom_wall;
	portal_type		*portal;

		// array for wall runs

	wall_left_run=(char*)malloc(map_max_size);
	wall_right_run=(char*)malloc(map_max_size);
	wall_top_run=(char*)malloc(map_max_size);
	wall_bottom_run=(char*)malloc(map_max_size);

		// create surrounding walls for portals

	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
		
			// start mesh
			
		txt_idx=(corridor_flags[n]==ag_corridor_flag_portal)?ag_settings.texture.portal_wall:ag_settings.texture.corridor_wall;
		if (!map_auto_generate_mesh_start(map,n,-1,txt_idx,FALSE,FALSE)) return;

			// portal height

		ty=portal->ty;
		by=portal->by-1;

			// get left/right run

		x=portal->x-1;
		ex=portal->ex+1;

		for (z=portal->z;z!=portal->ez;z++) {
			wall_left_run[z-portal->z]=map_auto_generate_portal_collision(map,x,z,x,z,n)?1:0;
			wall_right_run[z-portal->z]=map_auto_generate_portal_collision(map,ex,z,ex,z,n)?1:0;
		}

			// get top/bottom run

		z=portal->z-1;
		ez=portal->ez+1;

		for (x=portal->x;x!=portal->ex;x++) {
			wall_top_run[x-portal->x]=map_auto_generate_portal_collision(map,x,z,x,z,n)?1:0;
			wall_bottom_run[x-portal->x]=map_auto_generate_portal_collision(map,x,ez,x,ez,n)?1:0;
		}

			// portal size

		xsz=portal->ex-portal->x;
		zsz=portal->ez-portal->z;

			// build left/right walls

		lz=rz=0;
		in_left_wall=in_right_wall=TRUE;

		for (z=0;z!=zsz;z++) {

				// left

			if (wall_left_run[z]==1) {
				z2=z-1;
				if ((in_left_wall) && (lz<z2)) map_auto_generate_walls_add(map,n,0,lz,0,z2,ty,by);
				in_left_wall=FALSE;
			}
			else {
				if (!in_left_wall) {
					lz=z;
					in_left_wall=TRUE;
				}
			}

				// right

			if (wall_right_run[z]==1) {
				z2=z-1;
				if ((in_right_wall) && (rz<z2)) map_auto_generate_walls_add(map,n,xsz,rz,xsz,z2,ty,by);
				in_right_wall=FALSE;
			}
			else {
				if (!in_right_wall) {
					rz=z;
					in_right_wall=TRUE;
				}
			}
		}

		if ((in_left_wall) && (lz!=zsz)) map_auto_generate_walls_add(map,n,0,lz,0,zsz,ty,by);
		if ((in_right_wall) && (rz!=zsz)) map_auto_generate_walls_add(map,n,xsz,rz,xsz,zsz,ty,by);

			// build top/bottom walls

		lx=rx=0;
		in_top_wall=in_bottom_wall=TRUE;

		for (x=0;x!=xsz;x++) {

				// top

			if (wall_top_run[x]==1) {
				x2=x-1;
				if ((in_top_wall) && (lx<x2)) map_auto_generate_walls_add(map,n,lx,0,x2,0,ty,by);
				in_top_wall=FALSE;
			}
			else {
				if (!in_top_wall) {
					lx=x;
					in_top_wall=TRUE;
				}
			}

				// bottom

			if (wall_bottom_run[x]==1) {
				x2=x-1;
				if ((in_bottom_wall) && (rx<x2)) map_auto_generate_walls_add(map,n,rx,zsz,x2,zsz,ty,by);
				in_bottom_wall=FALSE;
			}
			else {
				if (!in_bottom_wall) {
					rx=x;
					in_bottom_wall=TRUE;
				}
			}
		}

		if ((in_top_wall) && (lx!=xsz)) map_auto_generate_walls_add(map,n,lx,0,xsz,0,ty,by);
		if ((in_bottom_wall) && (rx!=xsz)) map_auto_generate_walls_add(map,n,rx,zsz,xsz,zsz,ty,by);

		portal++;
	}

		// free wall runs

	free(wall_left_run);
	free(wall_right_run);
	free(wall_top_run);
	free(wall_bottom_run);
}

/* =======================================================

      Auto-Generate Height Connecting Walls
      
======================================================= */

void map_auto_generate_height_walls(map_type *map)
{
	int				n,k,x,ex,z,ez,xsz,zsz,fill;
	portal_type		*portal,*chk_portal;

	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
		
			// portal size

		xsz=portal->ex-portal->x;
		zsz=portal->ez-portal->z;
		
			// check for touching portals
	
		for (k=0;k!=map->nportal;k++) {
			if (k==n) continue;
			
				// only put height connecting walls in rooms that are higher
				
			chk_portal=&map->portals[k];
			if (portal->ty>=chk_portal->ty) continue;
			
				// fill
			
			fill=(corridor_flags[n]==ag_corridor_flag_portal)?ag_settings.texture.portal_wall:ag_settings.texture.corridor_wall;
			
			map_auto_generate_segment_start(-1,-1,fill,FALSE);
			
				// portals touching top
				
			if (portal->z==chk_portal->ez) {
				x=portal->x;
				if (chk_portal->x>x) x=chk_portal->x;
				ex=portal->ex;
				if (chk_portal->ex<ex) ex=chk_portal->ex;
				if (x>ex) continue;
				
				x-=portal->x;
				ex-=portal->x;
				map_auto_generate_walls_add(map,n,x,0,ex,0,portal->ty,(chk_portal->ty-1));
				continue;
			}
			
				// portals touching bottom
				
			if (portal->ez==chk_portal->z) {
				x=portal->x;
				if (chk_portal->x>x) x=chk_portal->x;
				ex=portal->ex;
				if (chk_portal->ex<ex) ex=chk_portal->ex;
				if (x>ex) continue;
				
				x-=portal->x;
				ex-=portal->x;
				map_auto_generate_walls_add(map,n,x,zsz,ex,zsz,portal->ty,(chk_portal->ty-1));
				continue;
			}
			
				// portals touching left
				
			if (portal->x==chk_portal->ex) {
				z=portal->z;
				if (chk_portal->z>z) z=chk_portal->z;
				ez=portal->ez;
				if (chk_portal->ez<ez) ez=chk_portal->ez;
				if (z>ez) continue;
				
				z-=portal->z;
				ez-=portal->z;
				map_auto_generate_walls_add(map,n,0,z,0,ez,portal->ty,(chk_portal->ty-1));
				continue;
			}
			
				// portals touching right
				
			if (portal->ex==chk_portal->x) {
				z=portal->z;
				if (chk_portal->z>z) z=chk_portal->z;
				ez=portal->ez;
				if (chk_portal->ez<ez) ez=chk_portal->ez;
				if (z>ez) continue;
				
				z-=portal->z;
				ez-=portal->z;
				map_auto_generate_walls_add(map,n,xsz,z,xsz,ez,portal->ty,(chk_portal->ty-1));
				continue;
			}
			
		}
	
		portal++;
	}
}

/* =======================================================

      Auto-Generate Portal Ramps
      
======================================================= */

void map_auto_generate_ramps(map_type *map)
{
	int				n,k,x,ex,z,ez,xsz,zsz,high,primitive_uid;
	portal_type		*portal,*chk_portal;
	
	for (n=0;n!=map->nportal;n++) {
		if (corridor_flags[n]!=ag_corridor_flag_portal) continue;
		
			// portal size

		portal=&map->portals[n];

		xsz=portal->ex-portal->x;
		zsz=portal->ez-portal->z;
		
			// check for touching portals
	
		for (k=0;k!=map->nportal;k++) {
			if (k==n) continue;
			if (corridor_flags[k]!=ag_corridor_flag_portal) continue;
			
				// only put ramps in rooms that are lower
				
			chk_portal=&map->portals[k];
			if (portal->by<=chk_portal->by) continue;

			high=portal->by-chk_portal->by;

				// make ramps a primitive

			primitive_uid=map_primitive_create_uid(map);
			
				// portals touching top
				
			if (portal->z==chk_portal->ez) {
				x=portal->x;
				if (chk_portal->x>x) x=chk_portal->x;
				ex=portal->ex;
				if (chk_portal->ex<ex) ex=chk_portal->ex;
				if (x>ex) continue;

				x-=portal->x;
				ex-=portal->x;

				z=0;
				ez=z+ag_settings.ramp.sz;
				
				if (high<4) {
					map_auto_generate_segment_start(-1,-1,ag_settings.texture.portal_wall,FALSE);
					map_auto_generate_segment_wall(map,n,x,z,ex,z,chk_portal->by,(portal->by-1),wc_none);
				}
				else {
					map_auto_generate_segment_start(-1,primitive_uid,ag_settings.texture.ramp,FALSE);
					map_auto_generate_segment_fc(map,n,sg_floor,x,z,ex,ez,portal->by,high,ag_ceiling_lower_neg_z);
					map_auto_generate_segment_wall(map,n,x,z,x,ez,chk_portal->by,(portal->by-1),wc_top);
					map_auto_generate_segment_wall(map,n,ex,z,ex,ez,chk_portal->by,(portal->by-1),wc_top);
				}
				
				continue;
			}
			
				// portals touching bottom
				
			if (portal->ez==chk_portal->z) {
				x=portal->x;
				if (chk_portal->x>x) x=chk_portal->x;
				ex=portal->ex;
				if (chk_portal->ex<ex) ex=chk_portal->ex;
				if (x>ex) continue;
				
				x-=portal->x;
				ex-=portal->x;

				ez=portal->ez-portal->z;
				z=ez-ag_settings.ramp.sz;

				if (high<4) {
					map_auto_generate_segment_start(-1,-1,ag_settings.texture.portal_wall,FALSE);
					map_auto_generate_segment_wall(map,n,x,ez,ex,ez,chk_portal->by,(portal->by-1),wc_none);
				}
				else {
					map_auto_generate_segment_start(-1,primitive_uid,ag_settings.texture.ramp,FALSE);
					map_auto_generate_segment_fc(map,n,sg_floor,x,z,ex,ez,portal->by,high,ag_ceiling_lower_pos_z);
					map_auto_generate_segment_wall(map,n,x,ez,x,z,chk_portal->by,(portal->by-1),wc_top);
					map_auto_generate_segment_wall(map,n,ex,ez,ex,z,chk_portal->by,(portal->by-1),wc_top);
				}
				
				continue;
			}
			
				// portals touching left
				
			if (portal->x==chk_portal->ex) {
				z=portal->z;
				if (chk_portal->z>z) z=chk_portal->z;
				ez=portal->ez;
				if (chk_portal->ez<ez) ez=chk_portal->ez;
				if (z>ez) continue;

				z-=portal->z;
				ez-=portal->z;

				x=0;
				ex=x+ag_settings.ramp.sz;

				if (high<4) {
					map_auto_generate_segment_start(-1,-1,ag_settings.texture.portal_wall,FALSE);
					map_auto_generate_segment_wall(map,n,x,z,x,ez,chk_portal->by,(portal->by-1),wc_none);
				}
				else {
					map_auto_generate_segment_start(-1,primitive_uid,ag_settings.texture.ramp,FALSE);
					map_auto_generate_segment_fc(map,n,sg_floor,x,z,ex,ez,portal->by,high,ag_ceiling_lower_neg_x);
					map_auto_generate_segment_wall(map,n,x,z,ex,z,chk_portal->by,(portal->by-1),wc_top);
					map_auto_generate_segment_wall(map,n,x,ez,ex,ez,chk_portal->by,(portal->by-1),wc_top);
				}
				
				continue;
			}
			
				// portals touching right
				
			if (portal->ex==chk_portal->x) {
				z=portal->z;
				if (chk_portal->z>z) z=chk_portal->z;
				ez=portal->ez;
				if (chk_portal->ez<ez) ez=chk_portal->ez;
				if (z>ez) continue;
				
				z-=portal->z;
				ez-=portal->z;

				ex=portal->ex-portal->x;
				x=ex-ag_settings.ramp.sz;

				if (high<4) {
					map_auto_generate_segment_start(-1,-1,ag_settings.texture.portal_wall,FALSE);
					map_auto_generate_segment_wall(map,n,ex,z,ex,ez,chk_portal->by,(portal->by-1),wc_none);
				}
				else {
					map_auto_generate_segment_start(-1,primitive_uid,ag_settings.texture.ramp,FALSE);
					map_auto_generate_segment_fc(map,n,sg_floor,x,z,ex,ez,portal->by,high,ag_ceiling_lower_pos_x);
					map_auto_generate_segment_wall(map,n,ex,z,x,z,chk_portal->by,(portal->by-1),wc_top);
					map_auto_generate_segment_wall(map,n,ex,ez,x,ez,chk_portal->by,(portal->by-1),wc_top);
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

void map_auto_generate_corridor_clip_horizontal_left_walls(map_type *map,int rn,int x,int lz,int rz,int ty,int by,int clip_sz,bool swap)
{
	int				rn2,seg_idx;
	
	seg_idx=map_auto_generate_segment_wall(map,rn,x,(lz+clip_sz),x,lz,ty,by,wc_bottom);
	if (seg_idx!=-1) {
		if (!swap) {
			rn2=map_auto_generate_portal_find_to_left(map,&map->portals[rn]);
		}
		else {
			rn2=map_auto_generate_portal_find_to_right(map,&map->portals[rn]);
		}
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
	
	seg_idx=map_auto_generate_segment_wall(map,rn,x,(rz-clip_sz),x,rz,ty,by,wc_bottom);
	if (seg_idx!=-1) {
		if (!swap) {
			rn2=map_auto_generate_portal_find_to_left(map,&map->portals[rn]);
		}
		else {
			rn2=map_auto_generate_portal_find_to_right(map,&map->portals[rn]);
		}
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
}

void map_auto_generate_corridor_clip_horizontal_right_walls(map_type *map,int rn,int x,int lz,int rz,int ty,int by,int clip_sz,bool swap)
{
	int				rn2,seg_idx;
	
	seg_idx=map_auto_generate_segment_wall(map,rn,x,lz,x,(lz+clip_sz),ty,by,wc_top);
	if (seg_idx!=-1) {
		if (!swap) {
			rn2=map_auto_generate_portal_find_to_right(map,&map->portals[rn]);
		}
		else {
			rn2=map_auto_generate_portal_find_to_left(map,&map->portals[rn]);
		}
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}

	seg_idx=map_auto_generate_segment_wall(map,rn,x,rz,x,(rz-clip_sz),ty,by,wc_top);
	if (seg_idx!=-1) {
		if (!swap) {
			rn2=map_auto_generate_portal_find_to_right(map,&map->portals[rn]);
		}
		else {
			rn2=map_auto_generate_portal_find_to_left(map,&map->portals[rn]);
		}
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
}

void map_auto_generate_corridor_clip_vertical_top_walls(map_type *map,int rn,int lx,int rx,int z,int ty,int by,int clip_sz,bool swap)
{
	int				rn2,seg_idx;
	
	seg_idx=map_auto_generate_segment_wall(map,rn,(lx+clip_sz),z,lx,z,ty,by,wc_bottom);
	if (seg_idx!=-1) {
		if (!swap) {
			rn2=map_auto_generate_portal_find_to_top(map,&map->portals[rn]);
		}
		else {
			rn2=map_auto_generate_portal_find_to_bottom(map,&map->portals[rn]);
		}
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}

	seg_idx=map_auto_generate_segment_wall(map,rn,(rx-clip_sz),z,rx,z,ty,by,wc_bottom);
	if (seg_idx!=-1) {
		if (!swap) {
			rn2=map_auto_generate_portal_find_to_top(map,&map->portals[rn]);
		}
		else {
			rn2=map_auto_generate_portal_find_to_bottom(map,&map->portals[rn]);
		}
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
}

void map_auto_generate_corridor_clip_vertical_bottom_walls(map_type *map,int rn,int lx,int rx,int z,int ty,int by,int clip_sz,bool swap)
{
	int				rn2,seg_idx;
	
	seg_idx=map_auto_generate_segment_wall(map,rn,lx,z,(lx+clip_sz),z,ty,by,wc_top);
	if (seg_idx!=-1) {
		if (!swap) {
			rn2=map_auto_generate_portal_find_to_bottom(map,&map->portals[rn]);
		}
		else {
			rn2=map_auto_generate_portal_find_to_top(map,&map->portals[rn]);
		}
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}

	seg_idx=map_auto_generate_segment_wall(map,rn,rx,z,(rx-clip_sz),z,ty,by,wc_top);
	if (seg_idx!=-1) {
		if (!swap) {
			rn2=map_auto_generate_portal_find_to_bottom(map,&map->portals[rn]);
		}
		else {
			rn2=map_auto_generate_portal_find_to_top(map,&map->portals[rn]);
		}
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
}

void map_auto_generate_corridor_clip_walls(map_type *map)
{
	int				n,lx,rx,lz,rz,ty,by,clip_sz;
	portal_type		*portal;
	
	map_auto_generate_segment_start(-1,-1,ag_settings.texture.portal_wall,FALSE);
	
		// add in the clip walls
		
	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {

			// portal size

		lx=lz=0;
		rx=portal->ex-portal->x;
		rz=portal->ez-portal->z;
		
		clip_sz=(portal->by-portal->ty)>>2;

			// top walls
			
		if ((corridor_types[n]==ag_corridor_type_slanted_ceiling) || (corridor_types[n]==ag_corridor_type_octagon)) {
			
			ty=portal->ty;
			by=(ty+clip_sz)-1;
		
			if (corridor_flags[n]==ag_corridor_flag_horizontal) {
				map_auto_generate_corridor_clip_horizontal_left_walls(map,n,lx,lz,rz,ty,by,clip_sz,FALSE);
				map_auto_generate_corridor_clip_horizontal_left_walls(map,n,rx,lz,rz,ty,by,clip_sz,TRUE);
			}
			if (corridor_flags[n]==ag_corridor_flag_vertical) {
				map_auto_generate_corridor_clip_vertical_top_walls(map,n,lx,rx,lz,ty,by,clip_sz,FALSE);
				map_auto_generate_corridor_clip_vertical_top_walls(map,n,lx,rx,rz,ty,by,clip_sz,TRUE);
			}

		}
		
			// bottom walls
			
		if (corridor_types[n]==ag_corridor_type_octagon) {
		
			ty=portal->by-clip_sz;
			by=portal->by-1;
		
			if (corridor_flags[n]==ag_corridor_flag_horizontal) {
				map_auto_generate_corridor_clip_horizontal_right_walls(map,n,lx,lz,rz,ty,by,clip_sz,TRUE);
				map_auto_generate_corridor_clip_horizontal_right_walls(map,n,rx,lz,rz,ty,by,clip_sz,FALSE);
			}
			if (corridor_flags[n]==ag_corridor_flag_vertical) {
				map_auto_generate_corridor_clip_vertical_bottom_walls(map,n,lx,rx,lz,ty,by,clip_sz,TRUE);
				map_auto_generate_corridor_clip_vertical_bottom_walls(map,n,lx,rx,rz,ty,by,clip_sz,FALSE);
			}

		}
			
		portal++;
	}
}

/* =======================================================

      Auto-Generate Floors and Ceilings
      
======================================================= */

bool map_auto_generate_portal_ceiling_ok(int ceiling_type,int lx,int lz,int rx,int rz,int lx2,int lz2,int rx2,int rz2)
{
	int				mx,mz;
	
	switch (ceiling_type) {
	
		case ag_ceiling_type_closed:
			return(TRUE);
			
		case ag_ceiling_type_open:
			return((lx2==lx) || (rx2==rx) || (lz2==lz) || (rz2==rz));

		case ag_ceiling_type_cross:
			if ((lx2==lx) || (rx2==rx) || (lz2==lz) || (rz2==rz)) return(TRUE);
			mx=(lx+rx)>>1;
			if ((mx>=lx2) && (mx<=rx2)) return(TRUE);
			mz=(lz+rz)>>1;
			return((mz>=lz2) && (mz<=rz2));

	}
	
	return(TRUE);
}

void map_auto_generate_portal_fc_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty,int by)
{
	int				xoff,zoff,ceiling_type,split_factor,
					xadd,zadd,lx2,rx2,lz2,rz2,slant_sz,
					px[8],py[8],pz[8];
	float			gx[8],gy[8];
	portal_type		*portal;
		
		// get sizes
		
	split_factor=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_split_factor_percent);

		// get split points

	portal=&map->portals[rn];
	
	xoff=split_factor-(portal->x%split_factor);
	zoff=split_factor-(portal->z%split_factor);
	
	slant_sz=(portal->by-portal->ty)>>2;
	
	ceiling_type=map_auto_generate_get_ceiling_type(&ag_settings);
	
		// create floor/ceiling polys

	rz2=lz;
	zadd=zoff;

	while (rz2!=rz) {

		lz2=rz2;
		rz2=lz2+zadd;
		if (rz2>=rz) rz2=rz;
		zadd=split_factor;

		rx2=lx;
		xadd=xoff;

		while (rx2!=rx) {
			lx2=rx2;
			rx2=lx2+xadd;
			if (rx2>=rx) rx2=rx;
			xadd=split_factor;

				// floors

			if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.portal_floor,FALSE,FALSE)) return;
			map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,by,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

				// ceilings
				
			if (map_auto_generate_portal_ceiling_ok(ceiling_type,lx,lz,rx,rz,lx2,lz2,rx2,rz2)) {
				if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.portal_ceiling,FALSE,FALSE)) return;
				map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,ty,px,py,pz,gx,gy);
				if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
			}
		}
	}	
}

void map_auto_generate_corridor_fc_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty,int by)
{
	int				xoff,zoff,split_factor,
					xadd,zadd,lx2,rx2,lz2,rz2,slant_sz,
					px[8],py[8],pz[8];
	float			gx[8],gy[8];
	portal_type		*portal;
	
		// get sizes
		
	split_factor=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_split_factor_percent);
		
	   // get split points

	portal=&map->portals[rn];
	
	xoff=split_factor-(portal->x%split_factor);
	zoff=split_factor-(portal->z%split_factor);
	
	slant_sz=(portal->by-portal->ty)>>2;
	
		// create regular floor/ceiling segments

	rz2=lz;
	zadd=zoff;

	while (rz2!=rz) {

		lz2=rz2;
		rz2=lz2+zadd;
		if (rz2>=rz) rz2=rz;
		zadd=split_factor;

		rx2=lx;
		xadd=xoff;

		while (rx2!=rx) {
			lx2=rx2;
			rx2=lx2+xadd;
			if (rx2>=rx) rx2=rx;
			xadd=split_factor;

				// floors

			if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor_floor,FALSE,FALSE)) return;
			map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,by,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
			
				// ceilings
				
			if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor_ceiling,FALSE,FALSE)) return;
			map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,ty,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
		}
	}
	
		// additional slanted floors for corridors
		
	if (corridor_types[rn]==ag_corridor_type_octagon) {
		
		if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor_floor,FALSE,FALSE)) return;

		if (corridor_flags[rn]==ag_corridor_flag_horizontal) {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,rx,(lz+slant_sz),by,slant_sz,ag_ceiling_lower_neg_z,FALSE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

			map_auto_generate_poly_from_square_floor_slant(lx,(rz-slant_sz),rx,rz,by,slant_sz,ag_ceiling_lower_pos_z,FALSE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
		}
		else {
			map_auto_generate_poly_from_square_floor_slant(lx,lz,(lx+slant_sz),rz,by,slant_sz,ag_ceiling_lower_neg_x,FALSE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

			map_auto_generate_poly_from_square_floor_slant((rx-slant_sz),lz,rx,rz,by,slant_sz,ag_ceiling_lower_pos_x,FALSE,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
		}
	}
	
		// additional slanted ceilings for corridors
		
	if ((corridor_types[rn]==ag_corridor_type_slanted_ceiling) || (corridor_types[rn]==ag_corridor_type_octagon)) {

		if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor_ceiling,FALSE,FALSE)) return;

		if (corridor_flags[rn]==ag_corridor_flag_horizontal) {
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

void map_auto_generate_fcs(map_type *map)
{
	int				n,xsz,zsz;
	portal_type		*portal;

	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {

			// portal size

		xsz=portal->ex-portal->x;
		zsz=portal->ez-portal->z;

			// floors and ceilings
			
		if (corridor_flags[n]==ag_corridor_flag_portal) {
			map_auto_generate_portal_fc_add(map,n,0,0,xsz,zsz,portal->ty,portal->by);
		}
		else {
			map_auto_generate_corridor_fc_add(map,n,0,0,xsz,zsz,portal->ty,portal->by);
		}
		
		portal++;
	}
}

/* =======================================================

      Auto-Generate Ceiling Walls
      
======================================================= */

void map_auto_generate_ceiling_wall_single(map_type *map,int portal_idx,int seg_idx,int lx,int lz,int rx,int rz,int ty,int by)
{
	int				n,k,k2;
	segment_type	*seg;
	
		// is this part of the segment open?
		
	for (n=0;n!=map->nsegment;n++) {
		if (n==seg_idx) continue;
		
		seg=&map->segments[n];
		if (seg->rn!=portal_idx) continue;
		if (seg->type!=sg_ceiling) continue;
		
		for (k=0;k!=seg->data.fc.ptsz;k++) {
			k2=k+1;
			if (k2==seg->data.fc.ptsz) k2=0;
			if ((seg->data.fc.x[k]==lx) && (seg->data.fc.z[k]==lz) && (seg->data.fc.x[k2]==rx) && (seg->data.fc.z[k2]==rz)) return;
			if ((seg->data.fc.x[k2]==lx) && (seg->data.fc.z[k2]==lz) && (seg->data.fc.x[k]==rx) && (seg->data.fc.z[k]==rz)) return;
		}
	}
	
		// create wall

	map_auto_generate_segment_wall(map,portal_idx,lx,lz,rx,rz,ty,by,wc_none);
}

void map_auto_generate_ceiling_walls(map_type *map)
{
	int				n,i,k,k2,ex,ez,ty,by;
	portal_type		*portal;
	segment_type	*seg;
		
			// fill
			
	map_auto_generate_segment_start(-1,-1,ag_settings.texture.portal_ceiling,FALSE);

		// run through the portals
		
	for (n=0;n!=map->nportal;n++) {
		if (corridor_flags[n]!=ag_corridor_flag_portal) continue;
		
			// wall heights
			
		portal=&map->portals[n];
		
		ty=portal->ty-((portal->by-portal->ty)/4);
		by=portal->ty-1;
		
		if ((by-ty)<4) ty=by-4;
		
			// portal edges
			
		ex=portal->ex-portal->x;
		ez=portal->ez-portal->z;
		
			// find all ceiling pieces for this portal
			
		for (i=0;i!=map->nsegment;i++) {
			seg=&map->segments[i];
			if (seg->rn!=n) continue;
			if (seg->type!=sg_ceiling) continue;
			
			for (k=0;k!=seg->data.fc.ptsz;k++) {
				k2=k+1;
				if (k2==seg->data.fc.ptsz) k2=0;
				
					// skip segments on edges
					
				if ((seg->data.fc.x[k]==0) || (seg->data.fc.z[k]==0) || (seg->data.fc.x[k]==ex) || (seg->data.fc.z[k]==ez)) continue;
				
					// make walls for segments with no connecting ceiling
					
				map_auto_generate_ceiling_wall_single(map,n,i,seg->data.fc.x[k],seg->data.fc.z[k],seg->data.fc.x[k2],seg->data.fc.z[k2],ty,by);
			}
		}
	}
}

/* =======================================================

      Auto-Generate Rough Floors
      
======================================================= */

bool map_auto_generate_rough_floor_seg_ok(segment_type *seg)
{
	if (seg->type!=sg_floor) return(FALSE);
	return(seg->primitive_uid[0]==-1);
}
	
bool map_auto_generate_rough_floor_vertex_ok(map_type *map,segment_type *seg,int idx)
{
	portal_type		*portal;
	
	portal=&map->portals[seg->rn];
	
	if ((seg->data.fc.x[idx]==0) || (seg->data.fc.x[idx]==(portal->ex-portal->x))) return(FALSE);
	if ((seg->data.fc.z[idx]==0) || (seg->data.fc.z[idx]==(portal->ez-portal->z))) return(FALSE);
	
	return(TRUE);
}

int map_auto_generate_rough_floor_find_floor(map_type *map,bool portal_only)
{
	int				idx;
	
	while (TRUE) {
		idx=rand()%map->nsegment;
		if (map_auto_generate_rough_floor_seg_ok(&map->segments[idx])) {
			if (portal_only) {
				if (corridor_flags[map->segments[idx].rn]!=ag_corridor_flag_portal) continue;
			}
			return(idx);
		}
	}
}

int map_auto_generate_rough_floor_find_vertex(map_type *map,segment_type *seg)
{
	int			vertex_idx,idx;

	vertex_idx=idx=rand()%seg->data.fc.ptsz;
	
	while (TRUE) {
		if (map_auto_generate_rough_floor_vertex_ok(map,seg,idx)) return(idx);
		idx=(idx+1)%seg->data.fc.ptsz;
		if (idx==vertex_idx) return(-1);
	}
	
	return(-1);
}

void map_auto_generate_rough_floor(map_type *map)
{
	int				n,k,i,nvertexshift,
					x,z,x2,z2,seg_idx,vertex_idx,chng;
	bool			portal_only,skip;
	portal_type		*portal;
	segment_type	*seg;
	
	if (!ag_settings.rough_portal_floors) return;
	
	portal_only=!ag_settings.rough_corridor_floors;
	
		// calculate number of vertexes to move
		
	nvertexshift=map->nsegment;
	
		// shift segments
		
	for (n=0;n!=nvertexshift;n++) {
		seg_idx=map_auto_generate_rough_floor_find_floor(map,portal_only);
		seg=&map->segments[seg_idx];

		vertex_idx=map_auto_generate_rough_floor_find_vertex(map,seg);
		if (vertex_idx==-1) continue;
		
		chng=1+(rand()%ag_settings.rough_floor_factor);
		
			// find absolute position of vertex
			
		portal=&map->portals[seg->rn];
		
		x=seg->data.fc.x[vertex_idx]+portal->x;
		z=seg->data.fc.z[vertex_idx]+portal->z;
		
			// if portal only, don't move vertexes that are attached to corridors
			// and don't move floors that are in primitives (because they will be stairs, ramps, etc)
			
		if (portal_only) {
		
			skip=FALSE;
			seg=map->segments;
			
			for (k=0;k!=map->nsegment;k++) {
			
				if (map_auto_generate_rough_floor_seg_ok(seg)) {
					portal=&map->portals[seg->rn];
					
					for (i=0;i!=seg->data.fc.ptsz;i++) {
						x2=seg->data.fc.x[i]+portal->x;
						z2=seg->data.fc.z[i]+portal->z;
						
						if ((x2==x) && (z2==z)) {
							if (corridor_flags[seg->rn]!=ag_corridor_flag_portal) {
								skip=TRUE;
								break;
							}
						}
					}
				}
				
				if (skip) break;
				
				seg++;
			}
			
			if (skip) continue;
		}
		
			// move all connected vertexes
			
		seg=map->segments;
		
		for (k=0;k!=map->nsegment;k++) {
		
			if (map_auto_generate_rough_floor_seg_ok(seg)) {
				portal=&map->portals[seg->rn];
				
				for (i=0;i!=seg->data.fc.ptsz;i++) {
					x2=seg->data.fc.x[i]+portal->x;
					z2=seg->data.fc.z[i]+portal->z;
					
					if ((x2==x) && (z2==z)) seg->data.fc.y[i]-=chng;
				}
			}
			
			seg++;
		}
	}
}

/* =======================================================

      Corridor to Portal Steps
      
======================================================= */

void map_auto_generate_corridor_to_portal_steps_single(map_type *map,int rn,int ty,int by,int stair_mode,int lx,int rx,int lz,int rz)
{
	int				y,y2,step_sz,primitive_uid;

		// make steps a primitive

	primitive_uid=map_primitive_create_uid(map);
	map_auto_generate_segment_start(-1,primitive_uid,ag_settings.texture.steps,FALSE);

		// create steps

	y=ty;
	step_sz=ag_settings.steps.sz;

	while (TRUE) {
		y2=y+ag_settings.steps.high;

			// step wall

		map_auto_generate_segment_wall(map,rn,lx,lz,rx,rz,y,(y2-1),wc_none);

		if (y2>=(by-1)) break;

			// step floor and side

		switch (stair_mode) {

			case ag_stair_neg_z:
				map_auto_generate_segment_fc(map,rn,sg_floor,lx,(lz-step_sz),rx,rz,y2,0,ag_ceiling_lower_none);
				map_auto_generate_segment_wall(map,rn,lx,(lz-step_sz),lx,rz,y2,(by-1),wc_none);
				map_auto_generate_segment_wall(map,rn,rx,(lz-step_sz),rx,rz,y2,(by-1),wc_none);
				lz-=step_sz;
				rz-=step_sz;
				break;

			case ag_stair_pos_z:
				map_auto_generate_segment_fc(map,rn,sg_floor,lx,lz,rx,(rz+step_sz),y2,0,ag_ceiling_lower_none);
				map_auto_generate_segment_wall(map,rn,lx,lz,lx,(rz+step_sz),y2,(by-1),wc_none);
				map_auto_generate_segment_wall(map,rn,rx,lz,rx,(rz+step_sz),y2,(by-1),wc_none);
				lz+=step_sz;
				rz+=step_sz;
				break;

			case ag_stair_neg_x:
				map_auto_generate_segment_fc(map,rn,sg_floor,(lx-step_sz),lz,rx,rz,y2,0,ag_ceiling_lower_none);
				map_auto_generate_segment_wall(map,rn,(lx-step_sz),lz,rx,lz,y2,(by-1),wc_none);
				map_auto_generate_segment_wall(map,rn,(lx-step_sz),rz,rx,rz,y2,(by-1),wc_none);
				lx-=step_sz;
				rx-=step_sz;
				break;

			case ag_stair_pos_x:
				map_auto_generate_segment_fc(map,rn,sg_floor,lx,lz,(rx+step_sz),rz,y2,0,ag_ceiling_lower_none);
				map_auto_generate_segment_wall(map,rn,lx,lz,(rx+step_sz),lz,y2,(by-1),wc_none);
				map_auto_generate_segment_wall(map,rn,lx,rz,(rx+step_sz),rz,y2,(by-1),wc_none);
				lx+=step_sz;
				rx+=step_sz;
				break;
		}

		y=y2;
	}
}

void map_auto_generate_corridor_to_portal_steps(map_type *map)
{
	int				n,k;
	portal_type		*portal,*chk_portal;
	
	for (n=0;n!=map->nportal;n++) {
		if (corridor_flags[n]!=ag_corridor_flag_portal) continue;

		portal=&map->portals[n];

			// find connecting corridors

		for (k=0;k!=map->nportal;k++) {
			if (n==k) continue;
			if (corridor_flags[k]==ag_corridor_flag_portal) continue;
			
			chk_portal=&map->portals[k];
			if (chk_portal->by>=portal->by) continue;

			if ((chk_portal->x>=portal->x) && (chk_portal->x<=portal->ex)) {
				if (chk_portal->ez==portal->z) map_auto_generate_corridor_to_portal_steps_single(map,n,chk_portal->by,portal->by,ag_stair_pos_z,(chk_portal->x-portal->x),(chk_portal->ex-portal->x),0,0);
				if (chk_portal->z==portal->ez) map_auto_generate_corridor_to_portal_steps_single(map,n,chk_portal->by,portal->by,ag_stair_neg_z,(chk_portal->x-portal->x),(chk_portal->ex-portal->x),(portal->ez-portal->z),(portal->ez-portal->z));
			}
			
			if ((chk_portal->z>=portal->z) && (chk_portal->z<=portal->ez)) {
				if (chk_portal->ex==portal->x) map_auto_generate_corridor_to_portal_steps_single(map,n,chk_portal->by,portal->by,ag_stair_pos_x,0,0,(chk_portal->z-portal->z),(chk_portal->ez-portal->z));
				if (chk_portal->x==portal->ex) map_auto_generate_corridor_to_portal_steps_single(map,n,chk_portal->by,portal->by,ag_stair_neg_x,(portal->ex-portal->x),(portal->ex-portal->x),(chk_portal->z-portal->z),(chk_portal->ez-portal->z));
			}
		}
	}
}

/* =======================================================

      Doors
      
======================================================= */

void map_auto_generate_doors(map_type *map)
{
	int						n,k,x,z,xsz,zsz,ty,by,
							door_cnt,group_idx,primitive_uid,
							movement_idx,move_idx;
	char					name[name_str_len];
	portal_type				*portal;
	movement_type			*movement;
	movement_move_type		*move;
	
	if (!ag_settings.doors) return;
	
		// run through corridors
		
	door_cnt=0;
		
	for (n=0;n!=map->nportal;n++) {
	
		portal=&map->portals[n];
	
			// is this a corridor?
			
		if (corridor_flags[n]==ag_corridor_flag_portal) continue;
			
			// percentage correct?
				
		if ((rand()%100)>ag_settings.door_percentage) continue;
		
			// which type of door?
			
		k=rand()%3;
			
			// create a new group and primitive
			
		if (map->ngroup>=max_group) return;
		if (map->nmovement>=max_movement) return;
		
		sprintf(name,"Door%d",(door_cnt+1));
		
		group_idx=map->ngroup;
		map->ngroup++;
		
		strcpy(map->groups[group_idx].name,name);
		
			// create new primitive
		
		primitive_uid=map_primitive_create_uid(map);
		
			// start segments
			
		map_auto_generate_segment_start(group_idx,primitive_uid,ag_settings.texture.door,TRUE);
		
			// create the movement
			
		movement_idx=map_movement_add(map);
		movement=&map->movements[movement_idx];
		
		strcpy(movement->name,name);
		movement->group_idx=group_idx;
		movement->auto_open=TRUE;
		movement->auto_open_distance=20000;
			
		move_idx=map_movement_move_add(map,movement_idx);
		move=&movement->moves[move_idx];
		
		move->mov.y=-(((portal->by-portal->ty)-1)*map_enlarge);
		move->msec=1500;
		strcpy(move->sound_name,ag_settings.door_sound);
		
			// size of door
			
		xsz=portal->ex-portal->x;
		zsz=portal->ez-portal->z;
		ty=portal->ty;
		by=portal->by-1;
		
			// create door segments
			
		if (corridor_flags[n]==ag_corridor_flag_horizontal) {
		
			switch (k) {
				case 0:
					x=4;
					break;
				case 1:
					x=xsz-6;
					break;
				default:
					x=(xsz/2)-1;
					break;
			}

			map_auto_generate_segment_wall(map,n,x,0,x,zsz,ty,by,wc_none);
			map_auto_generate_segment_wall(map,n,(x+2),0,(x+2),zsz,ty,by,wc_none);
			map_auto_generate_segment_fc(map,n,sg_ceiling,x,0,(x+2),zsz,ty,0,ag_ceiling_lower_none);
			map_auto_generate_segment_fc(map,n,sg_floor,x,0,(x+2),zsz,(by+1),0,ag_ceiling_lower_none);
		}
		else {
		
			switch (k) {
				case 0:
					z=4;
					break;
				case 1:
					z=zsz-6;
					break;
				default:
					z=(zsz/2)-1;
					break;
			}

			map_auto_generate_segment_wall(map,n,0,z,xsz,z,ty,by,wc_none);
			map_auto_generate_segment_wall(map,n,0,(z+2),xsz,(z+2),ty,by,wc_none);
			map_auto_generate_segment_fc(map,n,sg_ceiling,0,z,xsz,(z+2),ty,0,ag_ceiling_lower_none);
			map_auto_generate_segment_fc(map,n,sg_floor,0,z,xsz,(z+2),(by+1),0,ag_ceiling_lower_none);
		}
		
			// next door
			
		door_cnt++;
	}
}

/* =======================================================

      Add Spots
      
======================================================= */

void map_auto_generate_spots(map_type *map)
{
	int				n,k,x,y,z,idx;
	segment_type	*seg;
	spot_type		*spot;

	return;	// supergumba -- not working right now -- no segments
	
	if (!ag_settings.spots) return;
	
	for (n=0;n!=ag_settings.spot_count;n++) {
		idx=map_auto_generate_rough_floor_find_floor(map,TRUE);
		seg=&map->segments[idx];
		
			// get middle
			
		x=y=z=0;
		
		for (k=0;k!=seg->data.fc.ptsz;k++) {
			x+=seg->data.fc.x[k];
			y+=seg->data.fc.y[k];
			z+=seg->data.fc.z[k];
		}
		
		x/=seg->data.fc.ptsz;
		y/=seg->data.fc.ptsz;
		z/=seg->data.fc.ptsz;
		
			// create spot
			
		if (map->nspot>max_spot) return;
		
		spot=&map->spots[map->nspot];
		
		spot->pos.rn=seg->rn;
		spot->pos.x=x;
		spot->pos.z=z;
		spot->pos.y=y;
		spot->ang.y=0;
		sprintf(spot->name,"Spot %d",map->nspot);
		strcpy(spot->type,"Spot");
		spot->script[0]=0x0;
		spot->display_model[0]=0x0;
		spot->params[0]=0x0;
		
		map->nspot++;
	}
}

/* =======================================================

      Initialize Flags
      
======================================================= */

void map_auto_generate_clear_flags(void)
{
	int				n;
	
	for (n=0;n!=max_portal;n++) {
		corridor_flags[n]=ag_corridor_flag_portal;
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

	srand(ag_settings.seed);
	
	sz=ag_settings.map.sz/2;

	ag_settings.map.left=(map_max_size/2)-sz;
	ag_settings.map.right=(map_max_size/2)+sz;
	ag_settings.map.top=(map_max_size/2)-sz;
	ag_settings.map.bottom=(map_max_size/2)+sz;
	
	map_auto_generate_clear_flags();

		// create portals

	map_auto_generate_initial_portals(map);
	map_auto_generate_merge_portals(map);
	map_auto_generate_connect_portals(map);
	map_auto_generate_portal_y(map);
	
		// create lights
		
	map_auto_generate_lights(map);
	
		// create segments
/*
	map_auto_generate_walls(map);
	map_auto_generate_height_walls(map);
	map_auto_generate_ramps(map);
	map_auto_generate_corridor_clip_walls(map);
	map_auto_generate_fcs(map);
	map_auto_generate_ceiling_walls(map);
	map_auto_generate_rough_floor(map);
	map_auto_generate_corridor_to_portal_steps(map);
	
		// create doors
		
	map_auto_generate_doors(map);
*/	
		// fix segments and sight paths
		
//	map_auto_generate_fix_segments_uv(map);
	if (ag_settings.sight_path) map_portal_sight_generate_paths(map,FALSE);
	
		// create player spot
		
	map_auto_generate_add_player_spot(map);
	
		// create spots
		
	map_auto_generate_spots(map);
}

/* =======================================================

      Auto-Generate Test
      
======================================================= */

bool map_auto_generate_test(map_type *map,bool load_shaders)
{
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

	ags.map.sz=2000*map_enlarge;

	ags.portal.sz=500*map_enlarge;
	ags.portal.high=40*map_enlarge;
	
	ags.ceiling.type_on[ag_ceiling_type_closed]=TRUE;
	ags.ceiling.type_on[ag_ceiling_type_open]=TRUE;
	ags.ceiling.type_on[ag_ceiling_type_cross]=TRUE;
	
	ags.corridor.type_on[ag_corridor_type_normal]=TRUE;
	ags.corridor.type_on[ag_corridor_type_slanted_ceiling]=TRUE;
	ags.corridor.type_on[ag_corridor_type_octagon]=TRUE;

	ags.steps.sz=6;
	ags.steps.high=2;

	ags.ramp.sz=36;
	
	ags.texture.portal_wall=0;
	ags.texture.portal_floor=2;
	ags.texture.portal_ceiling=1;
	ags.texture.steps=5;
	ags.texture.ramp=5;
	ags.texture.corridor_wall=4;
	ags.texture.corridor_floor=4;
	ags.texture.corridor_ceiling=4;
	ags.texture.door=3;
	
	ags.rough_portal_floors=FALSE;
	ags.rough_corridor_floors=FALSE;
	ags.rough_floor_factor=1;
	ags.doors=TRUE;
	ags.door_percentage=50;
	strcpy(ags.door_sound,"Door");

	ags.lights=TRUE;
	ags.light_fudge_factor=10;
	ags.light_fill_percentage=90;
	ags.light_color_percentage=90;
	ags.light_flicker_percentage=10;
	ags.sight_path=TRUE;
	ags.spots=TRUE;
	ags.spot_count=20;

	map_auto_generate_block_preset(&ags,ag_block_preset_empty);

		// create the map

	map_auto_generate(map,&ags);

		// clear all save touches

	map_segments_clear_touch(map);

	return(TRUE);
}
