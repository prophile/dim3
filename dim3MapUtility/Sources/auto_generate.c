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

extern bool map_auto_generate_portal_collision(map_type *map,int x,int z,int ex,int ez,int skip_idx);
extern bool map_auto_generate_portal_horz_edge_block(map_type *map,int skip_portal_idx,int z,int ez,int x);
extern bool map_auto_generate_portal_vert_edge_block(map_type *map,int skip_portal_idx,int x,int ex,int z);

extern int map_auto_generate_portal_find_to_left(map_type *map,portal_type *org_portal);
extern int map_auto_generate_portal_find_to_right(map_type *map,portal_type *org_portal);
extern int map_auto_generate_portal_find_to_top(map_type *map,portal_type *org_portal);
extern int map_auto_generate_portal_find_to_bottom(map_type *map,portal_type *org_portal);

extern int map_auto_generate_get_corridor_type(auto_generate_settings_type *ags);
extern int map_auto_generate_get_ceiling_type(auto_generate_settings_type *ags);

extern void map_auto_generate_block_preset(auto_generate_settings_type *ag_settings,int block);
extern bool map_auto_generate_block_collision(auto_generate_settings_type *ags,int x,int z,int ex,int ez);

extern void map_auto_generate_poly_from_square_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_top_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_bot_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor(int lx,int lz,int rx,int rz,int fy,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor_slant(int lx,int lz,int rx,int rz,int fy,int yadd,int lower_mode,bool reverse_slant,int *x,int *y,int *z,float *gx,float *gy);

extern bool map_auto_generate_mesh_start(map_type *map,int portal_idx,int group_idx,int txt_idx,bool moveable,bool new_mesh);
extern bool map_auto_generate_mesh_add_poly(map_type *map,int ptsz,int *x,int *y,int *z,float *gx,float *gy);

unsigned char						ag_ceiling_data[ag_ceiling_type_count][12]=ag_ceiling_data_bytes,
									ag_ceiling_wall_data[ag_ceiling_type_count][7]=ag_ceiling_wall_data_bytes;

unsigned char						corridor_flags[max_portal],
									corridor_types[max_portal],
									ceiling_types[max_portal];

auto_generate_settings_type			ag_settings;

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

			if ((!map_auto_generate_block_collision(&ag_settings,x,z,ex,ez)) && (!map_auto_generate_portal_collision(map,x,z,ex,ez,-1))) break;
			
				// try to much?
				
			try_count++;
			if (try_count==100) return;
		}

			// create portal
			
		rn=map_portal_create(map,x,z,ex,ez);
		if (rn==-1) break;
		
		corridor_flags[rn]=ag_corridor_flag_portal;

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

			connect_sz=corridor_min_sz+map_auto_generate_random_int(corridor_rand_sz);
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

			if ((z==chk_portal->z) || (z==cnt_portal->z)) z+=split_factor;
			if ((ez==chk_portal->ez) || (ez==cnt_portal->ez)) ez-=split_factor;

			if (z>=ez) continue;

				// not enough connection size for portals?

			if ((ez-z)<corridor_min_sz) continue;
			
				// check for collisions
				
			z2=z-split_factor;
			ez2=ez+split_factor;			// don't create if z's will touch other portals, then it's no longer a corridor

			if ((map_auto_generate_block_collision(&ag_settings,x,z2,ex,ez2)) || (map_auto_generate_portal_collision(map,x,z2,ex,ez2,-1))) continue;

				// create portal

			rn=map_portal_create(map,x,z,ex,ez);
			if (rn==-1) break;
			
			corridor_flags[rn]=ag_corridor_flag_horizontal;
			corridor_types[rn]=map_auto_generate_get_corridor_type(&ag_settings);

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

			connect_sz=corridor_min_sz+map_auto_generate_random_int(corridor_rand_sz);
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

			if ((x==chk_portal->x) || (x==cnt_portal->x)) x+=split_factor;
			if ((ex==chk_portal->ex) || (ex==cnt_portal->ex)) ex-=split_factor;

			if (x>=ex) continue;

				// not enough connection size for portals?

			if ((ex-x)<corridor_min_sz) continue;
			
				// check for collisions
				
			x2=x-split_factor;
			ex2=ex+split_factor;			// don't create if x's will touch other portals, then it's no longer a corridor

			if ((map_auto_generate_block_collision(&ag_settings,x2,z,ex2,ez)) || (map_auto_generate_portal_collision(map,x2,z,ex2,ez,-1))) continue;

				// create portal
				
			rn=map_portal_create(map,x,z,ex,ez);
			if (rn==-1) break;
			
			corridor_flags[rn]=ag_corridor_flag_vertical;
			corridor_types[rn]=map_auto_generate_get_corridor_type(&ag_settings);

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
	int				n,corridor_slop_y,by_add,portal_high,
					ty,by,extra_ty,extra_by;
	portal_type		*portal;
	
		// portal sizes

	portal_high=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_high_percent);
		
	ty=(map_max_size/2)-(portal_high/2);
	by=(map_max_size/2)+(portal_high/2);
	
	extra_ty=(int)(((float)portal_high)*ag_constant_portal_high_extra_top);
	extra_by=(int)(((float)portal_high)*ag_constant_portal_high_extra_bottom);

		// corridor top slop (so not at very top of portal)

	corridor_slop_y=(int)((float)(by-ty)*ag_constant_portal_high_slop_y);

		// create portal y

	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
	
			// rooms have variable Ys
			
		if (corridor_flags[n]==ag_corridor_flag_portal) {
			portal->ty=ty-map_auto_generate_random_int(extra_ty);

			by_add=map_auto_generate_random_int(extra_by);
			if (by_add<=ag_constant_step_high) {
				by_add=0;
			}
			else {
				by_add/=ag_constant_step_high;
				if (by_add!=0) by_add=(by_add*ag_constant_step_high);
			}

			portal->by=by+by_add;
		}
		
			// corridors have static Ys
			
		else {
			portal->ty=(ty+corridor_slop_y);
			portal->by=by;
		}

			// fit within map_enlarge

		portal->ty/=map_enlarge;
		portal->ty*=map_enlarge;

		portal->by/=map_enlarge;
		portal->by*=map_enlarge;
		
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
			x+=(map_auto_generate_random_int(ag_settings.light_fudge_factor/2)-ag_settings.light_fudge_factor);
			z+=(map_auto_generate_random_int(ag_settings.light_fudge_factor/2)-ag_settings.light_fudge_factor);
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
			
			k=ag_settings.light_color_percentage+map_auto_generate_random_int(100-ag_settings.light_color_percentage);
			
			switch (map_auto_generate_random_int(3)) {
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
		
		if (ag_settings.light_flicker_percentage>map_auto_generate_random_int(100)) {
			switch (map_auto_generate_random_int(3)) {
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

void map_auto_generate_walls(map_type *map)
{
	int				n,x,z,xsz,zsz,ty,by,split_factor,txt_idx,
					px[8],py[8],pz[8];
	float			gx[8],gy[8];
	portal_type		*portal;

		// how we split the walls into a mesh

	split_factor=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_split_factor_percent);

		// create surrounding walls for portals

	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
		
			// start mesh
			
		txt_idx=(corridor_flags[n]==ag_corridor_flag_portal)?ag_settings.texture.portal_wall:ag_settings.texture.corridor_wall;
		if (!map_auto_generate_mesh_start(map,n,-1,txt_idx,FALSE,FALSE)) return;


			// portal height

		ty=portal->ty;
		by=portal->by;

			// portal size

		xsz=portal->ex-portal->x;
		zsz=portal->ez-portal->z;

			// build left/right walls

		for (z=0;z<zsz;z+=split_factor) {

				// left

			if (!map_auto_generate_portal_horz_edge_block(map,n,(z+portal->z),((z+portal->z)+split_factor),portal->x)) {
				map_auto_generate_poly_from_square_wall(0,z,0,(z+split_factor),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			}

				// right

			if (!map_auto_generate_portal_horz_edge_block(map,n,(z+portal->z),((z+portal->z)+split_factor),portal->ex)) {
				map_auto_generate_poly_from_square_wall(xsz,z,xsz,(z+split_factor),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			}
		}

			// build top/bottom walls

		for (x=0;x<xsz;x+=split_factor) {

				// top

			if (!map_auto_generate_portal_vert_edge_block(map,n,(x+portal->x),((x+portal->x)+split_factor),portal->z)) {
				map_auto_generate_poly_from_square_wall(x,0,(x+split_factor),0,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
			}

				// bottom

			if (!map_auto_generate_portal_vert_edge_block(map,n,(x+portal->x),((x+portal->x)+split_factor),portal->ez)) {
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
	int				n,k,x,ex,kx,z,ez,kz,xsz,zsz,split_factor,txt_idx,
					px[8],py[8],pz[8];
	float			gx[8],gy[8];
	portal_type		*portal,*chk_portal;

		// how we split the walls into a mesh

	split_factor=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_split_factor_percent);

		// create walls for any portals with different heights

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
			
				// texture
			
			txt_idx=(corridor_flags[n]==ag_corridor_flag_portal)?ag_settings.texture.portal_wall:ag_settings.texture.corridor_wall;
			if (!map_auto_generate_mesh_start(map,n,-1,txt_idx,FALSE,FALSE)) return;
			
				// portals touching top
				
			if (portal->z==chk_portal->ez) {
				x=portal->x;
				if (chk_portal->x>x) x=chk_portal->x;
				ex=portal->ex;
				if (chk_portal->ex<ex) ex=chk_portal->ex;
				if (x>ex) continue;
				
				x-=portal->x;
				ex-=portal->x;

				for (kx=x;kx<ex;kx+=split_factor) {
					map_auto_generate_poly_from_square_wall(kx,0,(kx+split_factor),0,portal->ty,chk_portal->ty,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
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

				for (kx=x;kx<ex;kx+=split_factor) {
					map_auto_generate_poly_from_square_wall(kx,zsz,(kx+split_factor),zsz,portal->ty,chk_portal->ty,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
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

				for (kz=z;kz<ez;kz+=split_factor) {
					map_auto_generate_poly_from_square_wall(0,kz,0,(kz+split_factor),portal->ty,chk_portal->ty,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
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

				for (kz=z;kz<ez;kz+=split_factor) {
					map_auto_generate_poly_from_square_wall(xsz,kz,xsz,(kz+split_factor),portal->ty,chk_portal->ty,px,py,pz,gx,gy);
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
	int				n,xsz,zsz,ty,by,
					px[8],py[8],pz[8],clip_sz;
	float			gx[8],gy[8];
	portal_type		*portal;
	
		// add in the clip walls
		
	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
		
		if (corridor_flags[n]==ag_corridor_flag_portal) {
			portal++;
			continue;
		}
		
			// portal size

		xsz=portal->ex-portal->x;
		zsz=portal->ez-portal->z;
		
		clip_sz=(portal->by-portal->ty)>>2;

			// top walls
			
		if ((corridor_types[n]==ag_corridor_type_slanted_ceiling) || (corridor_types[n]==ag_corridor_type_octagon)) {

			if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;

			ty=portal->ty;
			by=ty+clip_sz;
		
			if (corridor_flags[n]==ag_corridor_flag_horizontal) {
				map_auto_generate_poly_from_bot_trig_wall(0,0,0,clip_sz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_bot_trig_wall(0,zsz,0,(zsz-clip_sz),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_bot_trig_wall(xsz,0,xsz,clip_sz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_bot_trig_wall(xsz,zsz,xsz,(zsz-clip_sz),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
			}

			if (corridor_flags[n]==ag_corridor_flag_vertical) {
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
		
			ty=portal->by-clip_sz;
			by=portal->by;
		
			if (corridor_flags[n]==ag_corridor_flag_horizontal) {
				map_auto_generate_poly_from_top_trig_wall(0,0,0,clip_sz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_top_trig_wall(0,zsz,0,(zsz-clip_sz),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_top_trig_wall(xsz,0,xsz,clip_sz,ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

				map_auto_generate_poly_from_top_trig_wall(xsz,zsz,xsz,(zsz-clip_sz),ty,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
			}

			if (corridor_flags[n]==ag_corridor_flag_vertical) {
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
	int				ceiling_type,split_factor,lx2,rx2,lz2,rz2,slant_sz,
					px[8],py[8],pz[8];
	float			gx[8],gy[8];
	portal_type		*portal;
		
		// get sizes
		
	split_factor=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_split_factor_percent);

		// get slant size and ceiling type

	portal=&map->portals[rn];
	
	slant_sz=(portal->by-portal->ty)>>2;
	
	ceiling_type=map_auto_generate_get_ceiling_type(&ag_settings);
	
	ceiling_types[rn]=ceiling_types;
	
		// create floor/ceiling polys

	lz2=lz;

	while (lz2<rz) {

		rz2=lz2+split_factor;

		lx2=lx;

		while (lx2<rx) {

			rx2=lx2+split_factor;

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
			
			lx2=rx2;
		}
		
		lz2=rz2;
	}	
}

void map_auto_generate_corridor_fc_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty,int by)
{
	int				split_factor,lx2,rx2,lz2,rz2,slant_sz,
					px[8],py[8],pz[8];
	float			gx[8],gy[8];
	portal_type		*portal;
	
		// get sizes
		
	split_factor=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_split_factor_percent);
		
	   // get portal slants

	portal=&map->portals[rn];
	
	slant_sz=(portal->by-portal->ty)>>2;
	
		// create regular floor/ceiling segments

	lz2=lz;

	while (lz2<rz) {

		rz2=lz2+split_factor;

		lx2=lx;

		while (lx2<rx) {

			rx2=lx2+split_factor;

				// floors

			if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor_floor,FALSE,FALSE)) return;
			map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,by,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
			
				// ceilings
				
			if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.corridor_ceiling,FALSE,FALSE)) return;
			map_auto_generate_poly_from_square_floor(lx2,lz2,rx2,rz2,ty,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
			
			lx2=rx2;
		}
		
		lz2=rz2;
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
/* supergumba -- all needs to be fixed

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

	map_auto_generate_poly_from_square_wall(lx,lz,rx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_segment_wall(map,portal_idx,lx,lz,rx,rz,ty,by,wc_none);
	*/
}

void map_auto_generate_ceiling_walls(map_type *map)
{
/* supergumba -- fix this
	int				n,i,k,k2,ex,ez,ty,by;
	portal_type		*portal;
	segment_type	*seg;
		
		// run through the portals
		
	for (n=0;n!=map->nportal;n++) {
		if (corridor_flags[n]!=ag_corridor_flag_portal) continue;
		
			// fill

		if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.portal_ceiling,FALSE,FALSE)) return;
		
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
	*/
}

/* =======================================================

      Auto-Generate Rough Floors
      
======================================================= */

/* supergumba -- fix this
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
		idx=map_auto_generate_random_int(map->nsegment);
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

	vertex_idx=idx=map_auto_generate_random_int(seg->data.fc.ptsz);
	
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
		
		chng=1+map_auto_generate_random_int(ag_settings.rough_floor_factor);
		
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
*/

/* =======================================================

      Auto-Generate Portal Ramps
      
======================================================= */

void map_auto_generate_ramps_position(int *x,int *ex)
{
	int			mx,wid,split_factor,sz;

	wid=(*ex)-(*x);
	sz=wid>>1;

		// make sure ramp is at least a certain width
		
	split_factor=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_split_factor_percent);

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
	int				n,k,x,ex,z,ez,xsz,zsz,kx,kz,
					split_factor,high,ramp_sz,
					px[8],py[8],pz[8];
	float			gx[8],gy[8];
	portal_type		*portal,*chk_portal;

		// how we split the walls into a mesh

	split_factor=(int)(((float)ag_settings.portal.sz)*ag_constant_portal_split_factor_percent);
	
		// create ramps

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
			
				// get ramp size
				
			ramp_sz=(int)(((float)high)*ag_constant_ramp_size_percent);

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
				ez=z+ramp_sz;

					// portal wall

				if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;

				for (kx=x;kx<ex;kx+=split_factor) {
					map_auto_generate_poly_from_square_wall(kx,z,(kx+split_factor),z,chk_portal->by,portal->by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}

					// ramp

				if (high>=ag_constant_ramp_min_high) {
					if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.ramp,FALSE,TRUE)) return;

					map_auto_generate_ramps_position(&x,&ex);
				
					map_auto_generate_poly_from_top_trig_wall(x,z,x,ez,chk_portal->by,portal->by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
					map_auto_generate_poly_from_top_trig_wall(ex,z,ex,ez,chk_portal->by,portal->by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
					
					map_auto_generate_poly_from_square_floor_slant(x,z,ex,ez,portal->by,high,ag_ceiling_lower_neg_z,FALSE,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
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
				z=ez-ramp_sz;

					// portal wall

				if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;
			
				for (kx=x;kx<ex;kx+=split_factor) {
					map_auto_generate_poly_from_square_wall(kx,ez,(kx+split_factor),ez,chk_portal->by,portal->by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}

					// ramp

				if (high>=ag_constant_ramp_min_high) {
					if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.ramp,FALSE,TRUE)) return;

					map_auto_generate_ramps_position(&x,&ex);

					map_auto_generate_poly_from_top_trig_wall(x,ez,x,z,chk_portal->by,portal->by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
					map_auto_generate_poly_from_top_trig_wall(ex,ez,ex,z,chk_portal->by,portal->by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

					map_auto_generate_poly_from_square_floor_slant(x,z,ex,ez,portal->by,high,ag_ceiling_lower_pos_z,FALSE,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
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
				ex=x+ramp_sz;

					// portal wall

				if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;

				for (kz=z;kz<ez;kz+=split_factor) {
					map_auto_generate_poly_from_square_wall(x,kz,x,(kz+split_factor),chk_portal->by,portal->by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}

					// ramp

				if (high>=ag_constant_ramp_min_high) {
					if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.ramp,FALSE,TRUE)) return;

					map_auto_generate_ramps_position(&z,&ez);

					map_auto_generate_poly_from_top_trig_wall(x,z,ex,z,chk_portal->by,portal->by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
					map_auto_generate_poly_from_top_trig_wall(x,ez,ex,ez,chk_portal->by,portal->by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

					map_auto_generate_poly_from_square_floor_slant(x,z,ex,ez,portal->by,high,ag_ceiling_lower_neg_x,FALSE,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
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
				x=ex-ramp_sz;

					// portal wall

				if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.portal_wall,FALSE,FALSE)) return;
				
				for (kz=z;kz<ez;kz+=split_factor) {
					map_auto_generate_poly_from_square_wall(ex,kz,ex,(kz+split_factor),chk_portal->by,portal->by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}

					// ramp

				if (high>=ag_constant_ramp_min_high) {
					if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.ramp,FALSE,TRUE)) return;

					map_auto_generate_ramps_position(&z,&ez);

					map_auto_generate_poly_from_top_trig_wall(ex,z,x,z,chk_portal->by,portal->by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);
					map_auto_generate_poly_from_top_trig_wall(ex,ez,x,ez,chk_portal->by,portal->by,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,3,px,py,pz,gx,gy);

					map_auto_generate_poly_from_square_floor_slant(x,z,ex,ez,portal->by,high,ag_ceiling_lower_pos_x,FALSE,px,py,pz,gx,gy);
					map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
				}
				
				continue;
			}
			
		}
	
		portal++;
	}
}

/* =======================================================

      Corridor to Portal Steps
      
======================================================= */

void map_auto_generate_corridor_to_portal_steps_single(map_type *map,int rn,int ty,int by,int stair_mode,int lx,int rx,int lz,int rz)
{
	int				y,y2,step_cnt,step_sz,px[8],py[8],pz[8];
	float			gx[8],gy[8];

		// create new mesh for steps

	if (!map_auto_generate_mesh_start(map,rn,-1,ag_settings.texture.steps,FALSE,TRUE)) return;

		// get step size

	step_cnt=(by-ty)/ag_constant_step_high;
	if (step_cnt<=4) {
		step_sz=ag_constant_step_max_size;
	}
	else {
		if (step_cnt>=10) {
			step_sz=ag_constant_step_min_size;
		}
		else {
			step_sz=ag_constant_step_min_size+(((step_cnt-4)*(ag_constant_step_max_size-ag_constant_step_min_size))/6);
		}
	}

		// create steps

	y=ty;

	while (TRUE) {
		y2=y+ag_constant_step_high;

			// step wall

		map_auto_generate_poly_from_square_wall(lx,lz,rx,rz,y,y2,px,py,pz,gx,gy);
		map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

		if (y2>=by) break;

			// step floor and side

		switch (stair_mode) {

			case ag_stair_neg_z:
				map_auto_generate_poly_from_square_floor(lx,(lz-step_sz),rx,rz,y2,px,py,pz,gx,gy);
				if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

				map_auto_generate_poly_from_square_wall(lx,(lz-step_sz),lx,rz,y2,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_wall(rx,(lz-step_sz),rx,rz,y2,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				lz-=step_sz;
				rz-=step_sz;
				break;

			case ag_stair_pos_z:
				map_auto_generate_poly_from_square_floor(lx,lz,rx,(rz+step_sz),y2,px,py,pz,gx,gy);
				if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

				map_auto_generate_poly_from_square_wall(lx,lz,lx,(rz+step_sz),y2,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_wall(rx,lz,rx,(rz+step_sz),y2,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				lz+=step_sz;
				rz+=step_sz;
				break;

			case ag_stair_neg_x:
				map_auto_generate_poly_from_square_floor((lx-step_sz),lz,rx,rz,y2,px,py,pz,gx,gy);
				if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

				map_auto_generate_poly_from_square_wall((lx-step_sz),lz,rx,lz,y2,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_wall((lx-step_sz),rz,rx,rz,y2,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				lx-=step_sz;
				rx-=step_sz;
				break;

			case ag_stair_pos_x:
				map_auto_generate_poly_from_square_floor(lx,lz,(rx+step_sz),rz,y2,px,py,pz,gx,gy);
				if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

				map_auto_generate_poly_from_square_wall(lx,lz,(rx+step_sz),lz,y2,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

				map_auto_generate_poly_from_square_wall(lx,rz,(rx+step_sz),rz,y2,by,px,py,pz,gx,gy);
				map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

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
							px[8],py[8],pz[8],door_cnt,group_idx,
							movement_idx,move_idx;
	char					name[name_str_len];
	float					gx[8],gy[8];
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
				
		if (map_auto_generate_random_int(100)>ag_settings.door_percentage) continue;
		
			// which type of door?
			
		k=map_auto_generate_random_int(3);
			
			// create a new group and primitive
			
		if (map->ngroup>=max_group) return;
		if (map->nmovement>=max_movement) return;
		
		sprintf(name,"Door%d",(door_cnt+1));
		
		group_idx=map->ngroup;
		map->ngroup++;
		
		strcpy(map->groups[group_idx].name,name);
		
			// start meshes
			
		if (!map_auto_generate_mesh_start(map,n,group_idx,ag_settings.texture.door,FALSE,TRUE)) return;
		
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
		by=portal->by;
		
			// create door segments
			
		if (corridor_flags[n]==ag_corridor_flag_horizontal) {
		
			switch (k) {
				case 0:
					x=ag_constant_door_width*2;
					break;
				case 1:
					x=xsz-(ag_constant_door_width*3);
					break;
				default:
					x=xsz/2;
					break;
			}
			
			map_auto_generate_poly_from_square_wall(x,0,x,zsz,ty,by,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

			map_auto_generate_poly_from_square_wall((x+ag_constant_door_width),0,(x+ag_constant_door_width),zsz,ty,by,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
			
			map_auto_generate_poly_from_square_floor(x,0,(x+ag_constant_door_width),zsz,ty,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

			map_auto_generate_poly_from_square_floor(x,0,(x+ag_constant_door_width),zsz,by,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
		}
		else {
		
			switch (k) {
				case 0:
					z=ag_constant_door_width*2;
					break;
				case 1:
					z=zsz-(ag_constant_door_width*3);
					break;
				default:
					z=zsz/2;
					break;
			}

			map_auto_generate_poly_from_square_wall(0,z,xsz,z,ty,by,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

			map_auto_generate_poly_from_square_wall(0,(z+ag_constant_door_width),xsz,(z+ag_constant_door_width),ty,by,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

			map_auto_generate_poly_from_square_floor(0,z,xsz,(z+ag_constant_door_width),ty,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;

			map_auto_generate_poly_from_square_floor(0,z,xsz,(z+ag_constant_door_width),by,px,py,pz,gx,gy);
			if (!map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy)) return;
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
/* supergumba -- fix this!

	int				n,k,x,y,z,idx;
	segment_type	*seg;
	spot_type		*spot;
	
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
	*/
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

	map_auto_generate_random_seed(ag_settings.seed);
	
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
	
		// create meshes

	map_auto_generate_walls(map);
	map_auto_generate_height_walls(map);
	map_auto_generate_corridor_clip_walls(map);
	map_auto_generate_fcs(map);
//	map_auto_generate_ceiling_walls(map);
//	map_auto_generate_rough_floor(map);
	
		// create ramps, steps, and doors
		
	map_auto_generate_ramps(map);
	map_auto_generate_corridor_to_portal_steps(map);
	map_auto_generate_doors(map);
	
		// fix segments and sight paths
		
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
	
	ags.ceiling.type_on[ag_ceiling_type_closed]=TRUE;
	ags.ceiling.type_on[ag_ceiling_type_open]=TRUE;
	ags.ceiling.type_on[ag_ceiling_type_cross]=TRUE;
	
	ags.corridor.type_on[ag_corridor_type_normal]=TRUE;
	ags.corridor.type_on[ag_corridor_type_slanted_ceiling]=TRUE;
	ags.corridor.type_on[ag_corridor_type_octagon]=TRUE;
	
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

	return(TRUE);
}
