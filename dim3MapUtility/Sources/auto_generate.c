/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto-Generate Map Routines

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

extern void map_auto_generate_block_preset(auto_generate_settings_type *ag_settings,int block);

auto_generate_settings_type			ag_settings;

/* =======================================================

      Check Colliding Portals
      
======================================================= */

bool map_auto_generate_block_collision(map_type *map,int x,int z,int ex,int ez)
{
	int				blck_x,blck_z,lx,rx,tz,bz,map_x_factor,map_z_factor;

	map_x_factor=(ag_settings.map_right-ag_settings.map_left)/max_ag_block_sz;
	map_z_factor=(ag_settings.map_bottom-ag_settings.map_top)/max_ag_block_sz;

	bz=ag_settings.map_top;

	for (blck_z=0;blck_z!=max_ag_block_sz;blck_z++) {

		tz=bz;
		bz+=map_z_factor;

		rx=ag_settings.map_left;

		for (blck_x=0;blck_x!=max_ag_block_sz;blck_x++) {

			lx=rx;
			rx+=map_x_factor;

			if (ag_settings.block[blck_z][blck_x]!=0x0) {
				if (ez<=tz) continue;
				if (ex<=lx) continue;
				if (x>=rx) continue;
				if (z>=bz) continue;

				return(TRUE);
			}
		}
	}

	return(FALSE);
}

bool map_auto_generate_portal_collision(map_type *map,int x,int z,int ex,int ez,int skip_idx)
{
	int			n;
	portal_type	*portal;

	for (n=0;n!=map->nportal;n++) {
		if (skip_idx==n) continue;

		portal=&map->portals[n];

		if (ez<=portal->z) continue;
		if (ex<=portal->x) continue;
		if (x>=portal->ex) continue;
		if (z>=portal->ez) continue;

		return(TRUE);
	}

	return(FALSE);
}

int map_auto_generate_portal_find_to_left(map_type *map,portal_type *org_portal)
{
	int			n;
	portal_type	*portal;

	portal=map->portals;
	
	for (n=0;n!=map->nportal;n++) {
		if ((portal->ex==org_portal->x) && (portal->z<=org_portal->z) && (portal->ez>=org_portal->ez)) return(n);
		portal++;
	}
	
	return(0);
}

int map_auto_generate_portal_find_to_right(map_type *map,portal_type *org_portal)
{
	int			n;
	portal_type	*portal;

	portal=map->portals;
	
	for (n=0;n!=map->nportal;n++) {
		if ((portal->x==org_portal->ex) && (portal->z<=org_portal->z) && (portal->ez>=org_portal->ez)) return(n);
		portal++;
	}
	
	return(0);
}

int map_auto_generate_portal_find_to_top(map_type *map,portal_type *org_portal)
{
	int			n;
	portal_type	*portal;

	portal=map->portals;
	
	for (n=0;n!=map->nportal;n++) {
		if ((portal->ez==org_portal->z) && (portal->x<=org_portal->x) && (portal->ex>=org_portal->ex)) return(n);
		portal++;
	}
	
	return(0);
}

int map_auto_generate_portal_find_to_bottom(map_type *map,portal_type *org_portal)
{
	int			n;
	portal_type	*portal;

	portal=map->portals;
	
	for (n=0;n!=map->nportal;n++) {
		if ((portal->z==org_portal->ez) && (portal->x<=org_portal->x) && (portal->ex>=org_portal->ex)) return(n);
		portal++;
	}
	
	return(0);
}

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
	int			i,rn,x,z,ex,ez,try_count,
				map_x_sz,map_z_sz,portal_sz;

		// sizes

	map_x_sz=ag_settings.map_right-ag_settings.map_left;
	map_z_sz=ag_settings.map_bottom-ag_settings.map_top;
	portal_sz=ag_settings.max_portal_sz-ag_settings.min_portal_sz;

		// create portals

	map->nportal=0;

	for (i=0;i!=ag_settings.initial_portal_count;i++) {
		
		try_count=0;

		while (TRUE) {

				// create portal start

			x=ag_settings.map_left+(rand()%map_x_sz);
			z=ag_settings.map_top+(rand()%map_z_sz);
			
				// create portal size
				
			ex=x+(ag_settings.min_portal_sz+(rand()%portal_sz));
			ez=z+(ag_settings.min_portal_sz+(rand()%portal_sz));

				// fix right/bottom overflows

			if (ex>ag_settings.map_right) {
				x=ag_settings.map_right-(ex-x);
				ex=ag_settings.map_right;
			}
			if (ez>ag_settings.map_bottom) {
				z=ag_settings.map_bottom-(ez-z);
				ez=ag_settings.map_bottom;
			}
			
				// use splits as grid
				
			x/=ag_settings.split_factor;
			x*=ag_settings.split_factor;
			
			z/=ag_settings.split_factor;
			z*=ag_settings.split_factor;
			
			ex/=ag_settings.split_factor;
			ex*=ag_settings.split_factor;
			
			ez/=ag_settings.split_factor;
			ez*=ag_settings.split_factor;

				// check for collisions
				
			if ((!map_auto_generate_block_collision(map,x,z,ex,ez)) && (!map_auto_generate_portal_collision(map,x,z,ex,ez,-1))) break;
			
				// try to much?
				
			try_count++;
			if (try_count==100) return;
		}

			// create portal
			
		rn=map_portal_create(map,x,z,ex,ez);
		if (rn==-1) break;
	}
}

/* =======================================================

      Merge Close Portals
      
======================================================= */

void map_auto_generate_merge_portals(map_type *map)
{
	int			i,n,k,dist,merge_add,merge_try_count;
	bool		moved;
	portal_type	*chk_portal,*merge_portal;

		// only keep moving for a limited time
		// so portals that bounce between two don't cause
		// an infinite loop
		
	merge_try_count=ag_settings.max_portal_merge_distance*2;
	
	merge_add=ag_settings.split_factor;

	for (i=0;i!=merge_try_count;i++) {

		moved=FALSE;		// keep looping until no more merging

		for (n=0;n!=map->nportal;n++) {
			chk_portal=&map->portals[n];

				// only move portals in one direction

				// slowly move portals towards merges
				// so they can merge on multiple sides and
				// not interfere with other merges

			for (k=0;k!=map->nportal;k++) {
				merge_portal=&map->portals[k];

					// merge left

				dist=merge_portal->x-chk_portal->ex;
				if ((dist>0) && (dist<ag_settings.max_portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(map,(merge_portal->x-merge_add),merge_portal->z,merge_portal->ex,merge_portal->ez,k)) {
						merge_portal->x-=merge_add;
						moved=TRUE;
						break;
					}
				}

					// merge right

				dist=chk_portal->x-merge_portal->ex;
				if ((dist>0) && (dist<ag_settings.max_portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(map,merge_portal->x,merge_portal->z,(merge_portal->ex+merge_add),merge_portal->ez,k)) {
						merge_portal->ex+=merge_add;
						moved=TRUE;
						break;
					}
				}

					// merge top

				dist=merge_portal->z-chk_portal->ez;
				if ((dist>0) && (dist<ag_settings.max_portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(map,merge_portal->x,(merge_portal->z-merge_add),merge_portal->ex,merge_portal->ez,k)) {
						merge_portal->z-=merge_add;
						moved=TRUE;
						break;
					}
				}

					// merge bottom

				dist=chk_portal->ez-merge_portal->z;
				if ((dist>0) && (dist<ag_settings.max_portal_merge_distance)) {
					if (!map_auto_generate_portal_collision(map,merge_portal->x,merge_portal->z,merge_portal->ex,(merge_portal->ez+merge_add),k)) {
						merge_portal->ez+=merge_add;
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
	int				n,k,rn,
					x,z,ex,ez,x2,z2,ex2,ez2,connect_sz,dist,nportal,cnt;
	portal_type		*chk_portal,*cnt_portal;

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
			if (dist<ag_settings.min_portal_connect_distance) continue;
			if (dist>ag_settings.max_portal_connect_distance) continue;

				// get portal size

			x=chk_portal->ex;
			ex=cnt_portal->x;

			connect_sz=ag_settings.min_portal_connect_sz+(rand()%(ag_settings.max_portal_connect_sz-ag_settings.min_portal_connect_sz));
			z=(((chk_portal->z+chk_portal->ez)/2)+((cnt_portal->z+cnt_portal->ez)/2))/2;
			z-=(connect_sz/2);
			ez=z+connect_sz;

			if (z<chk_portal->z) z=chk_portal->z;
			if (z<cnt_portal->z) z=cnt_portal->z;
			if (ez>chk_portal->ez) ez=chk_portal->ez;
			if (ez>cnt_portal->ez) ez=cnt_portal->ez;

				// not enough connection size for portals?

			if ((ez-z)<ag_settings.min_portal_connect_sz) continue;
			
				// fix vertical

			z/=ag_settings.split_factor;
			z*=ag_settings.split_factor;
			
			ez/=ag_settings.split_factor;
			ez*=ag_settings.split_factor;
			
				// check for collisions
				
			z2=z-ag_settings.split_factor;
			ez2=ez+ag_settings.split_factor;			// don't create if z's will touch other portals, then it's no longer a corridor

			if ((map_auto_generate_block_collision(map,x,z2,ex,ez2)) || (map_auto_generate_portal_collision(map,x,z2,ex,ez2,-1))) continue;

				// create portal

			ag_settings.corridor_flags[map->nportal]=ag_corridor_flag_horizontal;

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
			if (dist<ag_settings.min_portal_connect_distance) continue;
			if (dist>ag_settings.max_portal_connect_distance) continue;

				// get portal size

			z=chk_portal->ez;
			ez=cnt_portal->z;

			connect_sz=ag_settings.min_portal_connect_sz+(rand()%(ag_settings.max_portal_connect_sz-ag_settings.min_portal_connect_sz));
			x=(((chk_portal->x+chk_portal->ex)/2)+((cnt_portal->x+cnt_portal->ex)/2))/2;
			x-=(connect_sz/2);
			ex=x+connect_sz;

			if (x<chk_portal->x) x=chk_portal->x;
			if (x<cnt_portal->x) x=cnt_portal->x;
			if (ex>chk_portal->ex) ex=chk_portal->ex;
			if (ex>cnt_portal->ex) ex=cnt_portal->ex;
			
				// fix horizontal

			x/=ag_settings.split_factor;
			x*=ag_settings.split_factor;
			
			ex/=ag_settings.split_factor;
			ex*=ag_settings.split_factor;

				// not enough connection size for portals?

			if ((ex-x)<ag_settings.min_portal_connect_sz) continue;
			
				// check for collisions
				
			x2=x-ag_settings.split_factor;
			ex2=ex+ag_settings.split_factor;			// don't create if x's will touch other portals, then it's no longer a corridor

			if ((map_auto_generate_block_collision(map,x2,z,ex2,ez)) || (map_auto_generate_portal_collision(map,x2,z,ex2,ez,-1))) continue;

				// create portal
				
			ag_settings.corridor_flags[map->nportal]=ag_corridor_flag_vertical;

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
	int				n;
	portal_type		*portal;

	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
	
			// rooms have variable Ys
			
		if (ag_settings.corridor_flags[n]==ag_corridor_flag_portal) {

				// tops

			portal->ty=ag_settings.portal_ty;
			if (ag_settings.max_portal_extra_y>0) {
				portal->ty-=(ag_settings.min_portal_extra_y+(rand()%(ag_settings.max_portal_extra_y-ag_settings.min_portal_extra_y)));
			}

				// bottoms

			portal->by=ag_settings.portal_by;
			if ((rand()%100)>50) portal->by+=ag_settings.min_portal_extra_y;
		}
		
			// corridors have static Ys
			
		else {
			portal->ty=ag_settings.portal_ty;
			portal->by=ag_settings.portal_by;
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
		lit->confine_to_portal=(ag_settings.corridor_flags[n]!=ag_corridor_flag_portal);
		lit->on=TRUE;

		portal++;
	}
}

/* =======================================================

      Auto-Generate Walls
      
======================================================= */

int map_auto_generate_walls_add_segment(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty,int by,int clip,int group_idx,int primitive_uid,int fill,bool movable)
{
	segment_type	*seg;
	
	if (map->nsegment==max_segment) return(-1);
	
	seg=&map->segments[map->nsegment];
	
	seg->type=sg_wall;
	seg->rn=rn;
	seg->fill=fill;
    seg->clip=clip;
    seg->curve=cv_none;
	seg->x_txtoff=seg->y_txtoff=0;
	seg->x_txtfact=seg->y_txtfact=1;
    seg->x_shift=seg->y_shift=0;
	seg->txt_ang=0;
	seg->dark_factor=1.0f;
    seg->alpha=1.0f;
	seg->group_idx=group_idx;
	seg->primitive_uid[0]=primitive_uid;
    seg->pass_through=FALSE;
	seg->moveable=movable;
	seg->lock=FALSE;
	seg->on=TRUE;
	
	seg->data.wall.lx=lx;
	seg->data.wall.rx=rx;
	seg->data.wall.lz=lz;
	seg->data.wall.rz=rz;
	seg->data.wall.ty=ty;
	seg->data.wall.by=by;

	map->nsegment++;
	
	return(map->nsegment-1);
}

void map_auto_generate_walls_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty,int by,int clip,int group_idx,int primitive_uid,int fill)
{
	int					lx2,rx2,lz2,rz2,xadd,zadd;

	   // horizontal walls

	if (lz==rz) {

		xadd=ag_settings.split_factor-(map->portals[rn].x%ag_settings.split_factor);

		rx2=lx;

		while (rx2!=rx) {
			lx2=rx2;
			rx2=lx2+xadd;
			if (rx2>=rx) rx2=rx;
			xadd=ag_settings.split_factor;

			if (map->nsegment<max_segment) map_auto_generate_walls_add_segment(map,rn,lx2,lz,rx2,lz,ty,by,clip,group_idx,primitive_uid,fill,FALSE);
		}

		return;
	}

	   // vertical walls

	if (lx==rx) {

		zadd=ag_settings.split_factor-(map->portals[rn].z%ag_settings.split_factor);

		rz2=lz;

		while (rz2!=rz) {
			lz2=rz2;
			rz2=lz2+zadd;
			if (rz2>=rz) rz2=rz;
			zadd=ag_settings.split_factor;
			
			if (map->nsegment<max_segment) map_auto_generate_walls_add_segment(map,rn,lx,lz2,lx,rz2,ty,by,clip,group_idx,primitive_uid,fill,FALSE);
		}

		return;
	}

	   // regular wall

	if (map->nsegment<max_segment) map_auto_generate_walls_add_segment(map,rn,lx,lz,rx,rz,ty,by,clip,group_idx,primitive_uid,fill,FALSE);
}

void map_auto_generate_walls(map_type *map)
{
	int				n,x,z,ex,ez,xsz,zsz,lx,rx,x2,lz,rz,z2,ty,by,fill;
	char			*wall_left_run,*wall_right_run,*wall_top_run,*wall_bottom_run;
	bool			in_left_wall,in_right_wall,in_top_wall,in_bottom_wall;
	portal_type		*portal;

	if (!ag_settings.walls) return;

		// array for wall runs

	wall_left_run=(char*)malloc(map_x_size);
	wall_right_run=(char*)malloc(map_x_size);
	wall_top_run=(char*)malloc(map_z_size);
	wall_bottom_run=(char*)malloc(map_z_size);

		// create surrounding walls for portals

	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
		
			// fill
			
		fill=(ag_settings.corridor_flags[n]==ag_corridor_flag_portal)?ag_settings.texture.portal_wall:ag_settings.texture.corridor_wall;

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
				if ((in_left_wall) && (lz<z2)) map_auto_generate_walls_add(map,n,0,lz,0,z2,ty,by,wc_none,-1,-1,fill);
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
				if ((in_right_wall) && (rz<z2)) map_auto_generate_walls_add(map,n,xsz,rz,xsz,z2,ty,by,wc_none,-1,-1,fill);
				in_right_wall=FALSE;
			}
			else {
				if (!in_right_wall) {
					rz=z;
					in_right_wall=TRUE;
				}
			}
		}

		if ((in_left_wall) && (lz!=zsz)) map_auto_generate_walls_add(map,n,0,lz,0,zsz,ty,by,wc_none,-1,-1,fill);
		if ((in_right_wall) && (rz!=zsz)) map_auto_generate_walls_add(map,n,xsz,rz,xsz,zsz,ty,by,wc_none,-1,-1,fill);

			// build top/bottom walls

		lx=rx=0;
		in_top_wall=in_bottom_wall=TRUE;

		for (x=0;x!=xsz;x++) {

				// top

			if (wall_top_run[x]==1) {
				x2=x-1;
				if ((in_top_wall) && (lx<x2)) map_auto_generate_walls_add(map,n,lx,0,x2,0,ty,by,wc_none,-1,-1,fill);
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
				if ((in_bottom_wall) && (rx<x2)) map_auto_generate_walls_add(map,n,rx,zsz,x2,zsz,ty,by,wc_none,-1,-1,fill);
				in_bottom_wall=FALSE;
			}
			else {
				if (!in_bottom_wall) {
					rx=x;
					in_bottom_wall=TRUE;
				}
			}
		}

		if ((in_top_wall) && (lx!=xsz)) map_auto_generate_walls_add(map,n,lx,0,xsz,0,ty,by,wc_none,-1,-1,fill);
		if ((in_bottom_wall) && (rx!=xsz)) map_auto_generate_walls_add(map,n,rx,zsz,xsz,zsz,ty,by,wc_none,-1,-1,fill);

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
	
	if (!ag_settings.walls) return;

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
			
			fill=(ag_settings.corridor_flags[n]==ag_corridor_flag_portal)?ag_settings.texture.portal_wall:ag_settings.texture.corridor_wall;
			
				// portals touching top
				
			if (portal->z==chk_portal->ez) {
				x=portal->x;
				if (chk_portal->x>x) x=chk_portal->x;
				ex=portal->ex;
				if (chk_portal->ex<ex) ex=chk_portal->ex;
				if (x>ex) continue;
				
				x-=portal->x;
				ex-=portal->x;
				map_auto_generate_walls_add(map,n,x,0,ex,0,portal->ty,(chk_portal->ty-1),wc_none,-1,-1,fill);
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
				map_auto_generate_walls_add(map,n,x,zsz,ex,zsz,portal->ty,(chk_portal->ty-1),wc_none,-1,-1,fill);
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
				map_auto_generate_walls_add(map,n,0,z,0,ez,portal->ty,(chk_portal->ty-1),wc_none,-1,-1,fill);
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
				map_auto_generate_walls_add(map,n,xsz,z,xsz,ez,portal->ty,(chk_portal->ty-1),wc_none,-1,-1,fill);
				continue;
			}
			
		}
	
		portal++;
	}
}

/* =======================================================

      Auto-Generate Clipping Corridor Walls
      
======================================================= */

void map_auto_generate_corridor_clip_horizontal_left_above_walls(map_type *map,int rn,int x,int lz,int rz,int ty,int by,int split_factor,int fill_wall)
{
	int				rn2,seg_idx;
	
	seg_idx=map_auto_generate_walls_add_segment(map,rn,x,(lz+split_factor),x,lz,ty,by,wc_bottom,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_left(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
	
	seg_idx=map_auto_generate_walls_add_segment(map,rn,x,(rz-split_factor),x,rz,ty,by,wc_bottom,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_left(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
}

void map_auto_generate_corridor_clip_horizontal_right_above_walls(map_type *map,int rn,int x,int lz,int rz,int ty,int by,int split_factor,int fill_wall)
{
	int				rn2,seg_idx;
	
	seg_idx=map_auto_generate_walls_add_segment(map,rn,x,(lz+split_factor),x,lz,ty,by,wc_bottom,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_right(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}

	seg_idx=map_auto_generate_walls_add_segment(map,rn,x,(rz-split_factor),x,rz,ty,by,wc_bottom,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_right(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
}

void map_auto_generate_corridor_clip_vertical_top_above_walls(map_type *map,int rn,int lx,int rx,int z,int ty,int by,int split_factor,int fill_wall)
{
	int				rn2,seg_idx;
	
	seg_idx=map_auto_generate_walls_add_segment(map,rn,(lx+split_factor),z,lx,z,ty,by,wc_bottom,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_top(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}

	seg_idx=map_auto_generate_walls_add_segment(map,rn,(rx-split_factor),z,rx,z,ty,by,wc_bottom,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_top(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
}

void map_auto_generate_corridor_clip_vertical_bottom_above_walls(map_type *map,int rn,int lx,int rx,int z,int ty,int by,int split_factor,int fill_wall)
{
	int				rn2,seg_idx;
	
	seg_idx=map_auto_generate_walls_add_segment(map,rn,(lx+split_factor),z,lx,z,ty,by,wc_bottom,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_bottom(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}

	seg_idx=map_auto_generate_walls_add_segment(map,rn,(rx-split_factor),z,rx,z,ty,by,wc_bottom,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_bottom(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
}

void map_auto_generate_corridor_clip_horizontal_left_below_walls(map_type *map,int rn,int x,int lz,int rz,int ty,int by,int split_factor,int fill_wall)
{
	int				rn2,seg_idx;
	
	seg_idx=map_auto_generate_walls_add_segment(map,rn,x,lz,x,(lz+split_factor),ty,by,wc_top,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_left(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
	
	seg_idx=map_auto_generate_walls_add_segment(map,rn,x,rz,x,(rz-split_factor),ty,by,wc_top,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_left(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
}

void map_auto_generate_corridor_clip_horizontal_right_below_walls(map_type *map,int rn,int x,int lz,int rz,int ty,int by,int split_factor,int fill_wall)
{
	int				rn2,seg_idx;
	
	seg_idx=map_auto_generate_walls_add_segment(map,rn,x,lz,x,(lz+split_factor),ty,by,wc_top,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_right(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}

	seg_idx=map_auto_generate_walls_add_segment(map,rn,x,rz,x,(rz-split_factor),ty,by,wc_top,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_right(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
}

void map_auto_generate_corridor_clip_vertical_top_below_walls(map_type *map,int rn,int lx,int rx,int z,int ty,int by,int split_factor,int fill_wall)
{
	int				rn2,seg_idx;
	
	seg_idx=map_auto_generate_walls_add_segment(map,rn,lx,z,(lx+split_factor),z,ty,by,wc_top,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_top(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}

	seg_idx=map_auto_generate_walls_add_segment(map,rn,rx,z,(rx-split_factor),z,ty,by,wc_top,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_top(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
}

void map_auto_generate_corridor_clip_vertical_bottom_below_walls(map_type *map,int rn,int lx,int rx,int z,int ty,int by,int split_factor,int fill_wall)
{
	int				rn2,seg_idx;
	
	seg_idx=map_auto_generate_walls_add_segment(map,rn,lx,z,(lx+split_factor),z,ty,by,wc_top,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_bottom(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}

	seg_idx=map_auto_generate_walls_add_segment(map,rn,rx,z,(rx-split_factor),z,ty,by,wc_top,-1,-1,fill_wall,FALSE);
	if (seg_idx!=-1) {
		rn2=map_auto_generate_portal_find_to_bottom(map,&map->portals[rn]);
		map_auto_generate_move_wall_segment_to_portal(map,rn2,seg_idx);
	}
}

void map_auto_generate_corridor_clip_walls(map_type *map)
{
	int				n,lx,rx,lz,rz,ty,by;
	portal_type		*portal;
	
		// any special corridors?
		
	if (ag_settings.corridor_type==ag_corridor_type_normal) return;

		// add in the clip walls
		
	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {

			// portal size

		lx=lz=0;
		rx=portal->ex-portal->x;
		rz=portal->ez-portal->z;

			// top walls
			
		if ((ag_settings.corridor_type==ag_corridor_type_slanted_ceiling) || (ag_settings.corridor_type==ag_corridor_type_octagon)) {
			
			ty=portal->ty;
			by=(ty+ag_settings.ceiling_slant_extra_y)-1;
		
			if (ag_settings.corridor_flags[n]==ag_corridor_flag_horizontal) {
				map_auto_generate_corridor_clip_horizontal_left_above_walls(map,n,lx,lz,rz,ty,by,ag_settings.split_factor,ag_settings.texture.portal_wall);
				map_auto_generate_corridor_clip_horizontal_right_above_walls(map,n,rx,lz,rz,ty,by,ag_settings.split_factor,ag_settings.texture.portal_wall);
			}
			if (ag_settings.corridor_flags[n]==ag_corridor_flag_vertical) {
				map_auto_generate_corridor_clip_vertical_top_above_walls(map,n,lx,rx,lz,ty,by,ag_settings.split_factor,ag_settings.texture.portal_wall);
				map_auto_generate_corridor_clip_vertical_bottom_above_walls(map,n,lx,rx,rz,ty,by,ag_settings.split_factor,ag_settings.texture.portal_wall);
			}

		}
		
			// bottom walls
			
		if (ag_settings.corridor_type==ag_corridor_type_octagon) {
		
			ty=portal->by-ag_settings.ceiling_slant_extra_y;
			by=portal->by-1;
		
			if (ag_settings.corridor_flags[n]==ag_corridor_flag_horizontal) {
				map_auto_generate_corridor_clip_horizontal_left_below_walls(map,n,lx,lz,rz,ty,by,ag_settings.split_factor,ag_settings.texture.portal_wall);
				map_auto_generate_corridor_clip_horizontal_right_below_walls(map,n,rx,lz,rz,ty,by,ag_settings.split_factor,ag_settings.texture.portal_wall);
			}
			if (ag_settings.corridor_flags[n]==ag_corridor_flag_vertical) {
				map_auto_generate_corridor_clip_vertical_top_below_walls(map,n,lx,rx,lz,ty,by,ag_settings.split_factor,ag_settings.texture.portal_wall);
				map_auto_generate_corridor_clip_vertical_bottom_below_walls(map,n,lx,rx,rz,ty,by,ag_settings.split_factor,ag_settings.texture.portal_wall);
			}

		}
			
		portal++;
	}
}

/* =======================================================

      Auto-Generate Floors and Ceilings
      
======================================================= */

void map_auto_generate_fc_add_segment(map_type *map,int rn,int type,int lx,int lz,int rx,int rz,int y,int group_idx,int primitive_uid,int fill,bool movable,int lower_mode)
{
	segment_type	*seg;
	
	if (map->nsegment==max_segment) return;
	
		// create segment
		
	seg=&map->segments[map->nsegment];
	
	seg->type=type;
	seg->rn=rn;
	seg->fill=fill;
    seg->clip=wc_none;
    seg->curve=cv_none;
	seg->x_txtoff=seg->y_txtoff=0;
	seg->x_txtfact=seg->y_txtfact=1;
    seg->x_shift=seg->y_shift=0;
	seg->txt_ang=0;
	seg->dark_factor=1.0f;
    seg->alpha=1.0f;
	seg->group_idx=group_idx;
	seg->primitive_uid[0]=primitive_uid;
    seg->pass_through=FALSE;
	seg->moveable=movable;
	seg->lock=FALSE;
	seg->on=TRUE;
	
	seg->data.fc.ptsz=4;
	
	seg->data.fc.x[0]=seg->data.fc.x[3]=lx;
	seg->data.fc.x[1]=seg->data.fc.x[2]=rx;
	seg->data.fc.z[0]=seg->data.fc.z[1]=lz;
	seg->data.fc.z[2]=seg->data.fc.z[3]=rz;
		
	seg->data.fc.y[0]=seg->data.fc.y[1]=seg->data.fc.y[2]=seg->data.fc.y[3]=y;
	
	if (type==sg_ceiling) {
	
		switch (lower_mode) {
			case ag_ceiling_lower_neg_x:
				seg->data.fc.y[0]+=ag_settings.ceiling_slant_extra_y;
				seg->data.fc.y[3]+=ag_settings.ceiling_slant_extra_y;
				break;
			case ag_ceiling_lower_pos_x:
				seg->data.fc.y[1]+=ag_settings.ceiling_slant_extra_y;
				seg->data.fc.y[2]+=ag_settings.ceiling_slant_extra_y;
				break;
			case ag_ceiling_lower_neg_z:
				seg->data.fc.y[0]+=ag_settings.ceiling_slant_extra_y;
				seg->data.fc.y[1]+=ag_settings.ceiling_slant_extra_y;
				break;
			case ag_ceiling_lower_pos_z:
				seg->data.fc.y[2]+=ag_settings.ceiling_slant_extra_y;
				seg->data.fc.y[3]+=ag_settings.ceiling_slant_extra_y;
				break;
		}
	}
	
	else {
	
		switch (lower_mode) {
			case ag_ceiling_lower_neg_x:
				seg->data.fc.y[0]-=ag_settings.ceiling_slant_extra_y;
				seg->data.fc.y[3]-=ag_settings.ceiling_slant_extra_y;
				break;
			case ag_ceiling_lower_pos_x:
				seg->data.fc.y[1]-=ag_settings.ceiling_slant_extra_y;
				seg->data.fc.y[2]-=ag_settings.ceiling_slant_extra_y;
				break;
			case ag_ceiling_lower_neg_z:
				seg->data.fc.y[0]-=ag_settings.ceiling_slant_extra_y;
				seg->data.fc.y[1]-=ag_settings.ceiling_slant_extra_y;
				break;
			case ag_ceiling_lower_pos_z:
				seg->data.fc.y[2]-=ag_settings.ceiling_slant_extra_y;
				seg->data.fc.y[3]-=ag_settings.ceiling_slant_extra_y;
				break;
		}
	
	}
	
	map->nsegment++;
}

void map_auto_generate_fc_add(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty,int by)
{
	int				xoff,zoff,
					xadd,zadd,lx2,rx2,lz2,rz2,
					fill_floor,fill_ceiling,fill_wall,
					lower_mode;
	bool			is_portal;
		
	is_portal=(ag_settings.corridor_flags[rn]==ag_corridor_flag_portal);
					
		// fill
			
	fill_floor=is_portal?ag_settings.texture.portal_floor:ag_settings.texture.corridor_floor;
	fill_ceiling=is_portal?ag_settings.texture.portal_ceiling:ag_settings.texture.corridor_ceiling;

	fill_wall=ag_settings.texture.portal_wall;
	
	   // get split points

	xoff=ag_settings.split_factor-(map->portals[rn].x%ag_settings.split_factor);
	zoff=ag_settings.split_factor-(map->portals[rn].z%ag_settings.split_factor);
	
		// create floor/ceiling segments

	rz2=lz;
	zadd=zoff;

	while (rz2!=rz) {

		lz2=rz2;
		rz2=lz2+zadd;
		if (rz2>=rz) rz2=rz;
		zadd=ag_settings.split_factor;

		rx2=lx;
		xadd=xoff;

		while (rx2!=rx) {
			lx2=rx2;
			rx2=lx2+xadd;
			if (rx2>=rx) rx2=rx;
			xadd=ag_settings.split_factor;

				// floors
				
			if (ag_settings.floors) {
			
					// portal floors
					
				if (is_portal) {
					map_auto_generate_fc_add_segment(map,rn,sg_floor,lx2,lz2,rx2,rz2,by,-1,-1,fill_floor,FALSE,ag_ceiling_lower_none);
				}
				
					// corridor floors
					
				else {
						
					lower_mode=ag_ceiling_lower_none;
						
					if (ag_settings.corridor_type==ag_corridor_type_octagon) {
						if (ag_settings.corridor_flags[rn]==ag_corridor_flag_horizontal) {
							if (((rz-lz)/ag_settings.split_factor)>=2) {
								if (lz2==lz) lower_mode=ag_ceiling_lower_neg_z;
								if (rz2==rz) lower_mode=ag_ceiling_lower_pos_z;
							}
						}
						else {
							if (((rx-lx)/ag_settings.split_factor)>=2) {
								if (lx2==lx) lower_mode=ag_ceiling_lower_neg_x;
								if (rx2==rx) lower_mode=ag_ceiling_lower_pos_x;
							}
						}
					}
				
					map_auto_generate_fc_add_segment(map,rn,sg_floor,lx2,lz2,rx2,rz2,by,-1,-1,fill_floor,FALSE,lower_mode);
				
				}
			}
			
				// ceilings
				
			if (ag_settings.ceilings) {
			
				if (is_portal) {
				
						// portal ceilings
						
					if ((ag_settings.open_hole_ceilings) && (ag_settings.open_hole_flags[rn])) {
						if ((lx2!=lx) && (rx2!=rx) && (lz2!=lz) && (rz2!=rz)) {
							ag_settings.open_hole_flags[rn]=1;
							continue;
						}
					}
								
					map_auto_generate_fc_add_segment(map,rn,sg_ceiling,lx2,lz2,rx2,rz2,ty,-1,-1,fill_ceiling,FALSE,ag_ceiling_lower_none);
				}
				else {
				
						// corridor ceilings
						
					lower_mode=ag_ceiling_lower_none;
						
					if ((ag_settings.corridor_type==ag_corridor_type_slanted_ceiling) || (ag_settings.corridor_type==ag_corridor_type_octagon)) {
						if (ag_settings.corridor_flags[rn]==ag_corridor_flag_horizontal) {
							if (((rz-lz)/ag_settings.split_factor)>=2) {
								if (lz2==lz) lower_mode=ag_ceiling_lower_neg_z;
								if (rz2==rz) lower_mode=ag_ceiling_lower_pos_z;
							}
						}
						else {
							if (((rx-lx)/ag_settings.split_factor)>=2) {
								if (lx2==lx) lower_mode=ag_ceiling_lower_neg_x;
								if (rx2==rx) lower_mode=ag_ceiling_lower_pos_x;
							}
						}
					}
					
					map_auto_generate_fc_add_segment(map,rn,sg_ceiling,lx2,lz2,rx2,rz2,ty,-1,-1,fill_ceiling,FALSE,lower_mode);
				}
			}
			
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
			
		map_auto_generate_fc_add(map,n,0,0,xsz,zsz,portal->ty,portal->by);

		portal++;
	}
}

/* =======================================================

      Auto-Generate Open Ceiling Walls
      
======================================================= */

void map_auto_generate_open_ceiling_walls(map_type *map)
{
	int				n,lx,rx,lz,rz,ty,by,fill;
	portal_type		*portal;
	
		// open ceilings on?

	if (!ag_settings.ceilings) return;
	if (!ag_settings.open_hole_ceilings) return;

		// create surrounding walls for portals

	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
	
		if (ag_settings.corridor_flags[n]!=ag_corridor_flag_portal) {
			portal++;
			continue;
		}
		
		if (!ag_settings.open_hole_flags[n]) {
			portal++;
			continue;
		}
		
			// fill
			
		fill=ag_settings.texture.portal_ceiling;

			// wall sizes

		ty=portal->ty-ag_settings.open_ceiling_extra_y;
		by=portal->ty-1;
		
		lx=ag_settings.split_factor;
		rx=(portal->ex-portal->x)-ag_settings.split_factor;
		lz=ag_settings.split_factor;
		rz=(portal->ez-portal->z)-ag_settings.split_factor;

			// walls
			
		map_auto_generate_walls_add(map,n,lx,lz,rx,lz,ty,by,wc_none,-1,-1,fill);
		map_auto_generate_walls_add(map,n,lx,rz,rx,rz,ty,by,wc_none,-1,-1,fill);
		map_auto_generate_walls_add(map,n,lx,lz,lx,rz,ty,by,wc_none,-1,-1,fill);
		map_auto_generate_walls_add(map,n,rx,lz,rx,rz,ty,by,wc_none,-1,-1,fill);

		portal++;
	}
}

/* =======================================================

      Auto-Generate Rough Floors
      
======================================================= */

int map_auto_generate_rough_floor_find_floor(map_type *map,bool portal_only)
{
	int				idx;
	
	while (TRUE) {
		idx=rand()%map->nsegment;
		if (map->segments[idx].type==sg_floor) {
			if (portal_only) {
				if (ag_settings.corridor_flags[map->segments[idx].rn]!=ag_corridor_flag_portal) continue;
			}
			return(idx);
		}
	}
}

void map_auto_generate_rough_floor(map_type *map)
{
	int				n,k,i,nvertexshift,
					x,z,x2,z2,seg_idx,vertex_idx,chng;
	bool			portal_only,skip;
	portal_type		*portal;
	segment_type	*seg;
	
	if ((!ag_settings.floors) || (!ag_settings.rough_portal_floors)) return;
	
	portal_only=!ag_settings.rough_corridor_floors;
	
		// calculate number of vertexes to move
		
	nvertexshift=map->nsegment;
	
		// shift segments
		
	for (n=0;n!=nvertexshift;n++) {
		seg_idx=map_auto_generate_rough_floor_find_floor(map,portal_only);
		vertex_idx=rand()%4;
		chng=1+(rand()%ag_settings.rough_floor_factor);
		
			// find absolute position of vertex
			
		seg=&map->segments[seg_idx];
		portal=&map->portals[seg->rn];
		
		x=seg->data.fc.x[vertex_idx]+portal->x;
		z=seg->data.fc.z[vertex_idx]+portal->z;
		
			// if portal only, don't move vertexes that are attached to corridors
			
		if (portal_only) {
		
			skip=FALSE;
			seg=map->segments;
			
			for (k=0;k!=map->nsegment;k++) {
			
				if (seg->type==sg_floor) {
					portal=&map->portals[seg->rn];
					
					for (i=0;i!=seg->data.fc.ptsz;i++) {
						x2=seg->data.fc.x[i]+portal->x;
						z2=seg->data.fc.z[i]+portal->z;
						
						if ((x2==x) && (z2==z)) {
							if (ag_settings.corridor_flags[seg->rn]!=ag_corridor_flag_portal) {
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
		
			if (seg->type==sg_floor) {
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
	int				y,y2,step_sz;

	y=ty;
	step_sz=ag_settings.steps.sz;

	while (TRUE) {
		y2=y+ag_settings.steps.high;
		if (y2>=by) y2=by-1;

		map_auto_generate_walls_add_segment(map,rn,lx,lz,rx,rz,y,y2,wc_none,-1,-1,ag_settings.texture.steps,FALSE);

		if (y2==(by-1)) break;

		switch (stair_mode) {

			case ag_stair_neg_z:
				map_auto_generate_fc_add_segment(map,rn,sg_floor,lx,(lz-step_sz),rx,rz,y2,-1,-1,ag_settings.texture.steps,FALSE,ag_ceiling_lower_none);
				lz-=step_sz;
				rz-=step_sz;
				break;

			case ag_stair_pos_z:
				map_auto_generate_fc_add_segment(map,rn,sg_floor,lx,lz,rx,(rz+step_sz),y2,-1,-1,ag_settings.texture.steps,FALSE,ag_ceiling_lower_none);
				lz+=step_sz;
				rz+=step_sz;
				break;

			case ag_stair_neg_x:
				map_auto_generate_fc_add_segment(map,rn,sg_floor,(lx-step_sz),lz,rx,rz,y2,-1,-1,ag_settings.texture.steps,FALSE,ag_ceiling_lower_none);
				lx-=step_sz;
				rx-=step_sz;
				break;

			case ag_stair_pos_x:
				map_auto_generate_fc_add_segment(map,rn,sg_floor,lx,lz,(rx+step_sz),rz,y2,-1,-1,ag_settings.texture.steps,FALSE,ag_ceiling_lower_none);
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
		portal=&map->portals[n];
		if (ag_settings.corridor_flags[n]!=ag_corridor_flag_portal) continue;

			// find connecting corridors

		for (k=0;k!=map->nportal;k++) {
			chk_portal=&map->portals[k];
			if (ag_settings.corridor_flags[k]==ag_corridor_flag_portal) continue;
			if (chk_portal->by>=portal->by) continue;

			if (chk_portal->ez==portal->z) map_auto_generate_corridor_to_portal_steps_single(map,n,chk_portal->by,portal->by,ag_stair_pos_z,(chk_portal->x-portal->x),(chk_portal->ex-portal->x),0,0);
			if (chk_portal->z==portal->ez) map_auto_generate_corridor_to_portal_steps_single(map,n,chk_portal->by,portal->by,ag_stair_neg_z,(chk_portal->x-portal->x),(chk_portal->ex-portal->x),(portal->ez-portal->z),(portal->ez-portal->z));
			if (chk_portal->ex==portal->x) map_auto_generate_corridor_to_portal_steps_single(map,n,chk_portal->by,portal->by,ag_stair_pos_x,0,0,(chk_portal->z-portal->z),(chk_portal->ez-portal->z));
			if (chk_portal->x==portal->ex) map_auto_generate_corridor_to_portal_steps_single(map,n,chk_portal->by,portal->by,ag_stair_neg_x,(portal->ex-portal->x),(portal->ex-portal->x),(chk_portal->z-portal->z),(chk_portal->ez-portal->z));
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
			
		if (ag_settings.corridor_flags[n]==ag_corridor_flag_portal) continue;
			
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
			
		if (ag_settings.corridor_flags[n]==ag_corridor_flag_horizontal) {
		
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

			map_auto_generate_walls_add_segment(map,n,x,0,x,zsz,ty,by,wc_none,group_idx,primitive_uid,ag_settings.texture.door,TRUE);
			map_auto_generate_walls_add_segment(map,n,(x+2),0,(x+2),zsz,ty,by,wc_none,group_idx,primitive_uid,ag_settings.texture.door,TRUE);
			map_auto_generate_fc_add_segment(map,n,sg_ceiling,x,0,(x+2),zsz,ty,group_idx,primitive_uid,ag_settings.texture.door,TRUE,ag_ceiling_lower_none);
			map_auto_generate_fc_add_segment(map,n,sg_floor,x,0,(x+2),zsz,(by+1),group_idx,primitive_uid,ag_settings.texture.door,TRUE,ag_ceiling_lower_none);
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

			map_auto_generate_walls_add_segment(map,n,0,z,xsz,z,ty,by,wc_none,group_idx,primitive_uid,ag_settings.texture.door,TRUE);
			map_auto_generate_walls_add_segment(map,n,0,(z+2),xsz,(z+2),ty,by,wc_none,group_idx,primitive_uid,ag_settings.texture.door,TRUE);
			map_auto_generate_fc_add_segment(map,n,sg_ceiling,0,z,xsz,(z+2),ty,group_idx,primitive_uid,ag_settings.texture.door,TRUE,ag_ceiling_lower_none);
			map_auto_generate_fc_add_segment(map,n,sg_floor,0,z,xsz,(z+2),(by+1),group_idx,primitive_uid,ag_settings.texture.door,TRUE,ag_ceiling_lower_none);
		}
		
			// next door
			
		door_cnt++;
	}
}

/* =======================================================

      Auto-Generate Spots
      
======================================================= */

void map_auto_generate_spots(map_type *map)
{
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
}

/* =======================================================

      Initialize Flags
      
======================================================= */

void map_auto_generate_clear_flags(void)
{
	int				n;
	
	for (n=0;n!=max_portal;n++) {
		ag_settings.corridor_flags[n]=ag_corridor_flag_portal;
		ag_settings.open_hole_flags[n]=(ag_settings.open_hole_percentage>(rand()%100));
	}
}

/* =======================================================

      Auto-Generate MainLine
      
======================================================= */

// supergumba -- void* is a workaround (for now) to avoid defining auto_generate_settings_type in headers

void map_auto_generate(map_type *map,void *ags)
{
		// setup global

	memmove(&ag_settings,ags,sizeof(auto_generate_settings_type));

		// clear the map
		
	map_auto_generate_clear(map);

		// some additional setup

	srand(ag_settings.seed);
	
	ag_settings.map_left=(map_x_size/2)-(ag_settings.max_map_x_size/2);
	ag_settings.map_right=(map_x_size/2)+(ag_settings.max_map_x_size/2);
	ag_settings.map_top=(map_z_size/2)-(ag_settings.max_map_z_size/2);
	ag_settings.map_bottom=(map_z_size/2)+(ag_settings.max_map_z_size/2);
	
	map_auto_generate_clear_flags();

		// create portals

	map_auto_generate_initial_portals(map);
	map_auto_generate_merge_portals(map);
	map_auto_generate_connect_portals(map);
	map_auto_generate_portal_y(map);
	
		// create lights
		
	map_auto_generate_lights(map);
	
		// create segments

	map_auto_generate_walls(map);
	map_auto_generate_height_walls(map);
	map_auto_generate_corridor_clip_walls(map);
	map_auto_generate_fcs(map);
	map_auto_generate_open_ceiling_walls(map);
	map_auto_generate_rough_floor(map);
	map_auto_generate_corridor_to_portal_steps(map);
	
		// create doors
		
	map_auto_generate_doors(map);
	
		// fix segments and sight paths
		
	map_auto_generate_fix_segments_uv(map);
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

	ags.max_map_x_size=1000;
	ags.max_map_z_size=1000;
	ags.split_factor=20;

	ags.initial_portal_count=10;
	ags.min_portal_sz=150;
	ags.max_portal_sz=250;
	ags.portal_by=200;
	ags.portal_ty=170;
	ags.min_portal_extra_y=10;
	ags.max_portal_extra_y=30;
	ags.open_ceiling_extra_y=5;
	
	ags.max_portal_merge_distance=20;
	ags.min_portal_connect_distance=10;
	ags.max_portal_connect_distance=100;
	ags.min_portal_connect_sz=30;
	ags.max_portal_connect_sz=50;
	ags.ceiling_slant_extra_y=2;
	ags.corridor_type=ag_corridor_type_slanted_ceiling;

	ags.steps.sz=4;
	ags.steps.high=2;
	
	ags.texture.portal_wall=0;
	ags.texture.portal_floor=2;
	ags.texture.portal_ceiling=1;
	ags.texture.steps=5;
	ags.texture.corridor_wall=4;
	ags.texture.corridor_floor=4;
	ags.texture.corridor_ceiling=4;
	ags.texture.door=3;
	
	ags.walls=TRUE;
	ags.floors=TRUE;
	ags.rough_portal_floors=FALSE;
	ags.rough_corridor_floors=FALSE;
	ags.rough_floor_factor=1;
	ags.ceilings=TRUE;
	ags.open_hole_ceilings=TRUE;
	ags.open_hole_percentage=50;
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

	map_auto_generate(map,(void*)&ags);

		// clear all save touches

	map_segments_clear_touch(map);

	return(TRUE);
}
