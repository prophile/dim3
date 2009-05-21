/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto-Generate Second Story Routines

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

extern int map_auto_generate_random_int(int max);

extern bool map_auto_generate_portal_horz_edge_touch(int skip_portal_idx,int z,int ez,int x);
extern bool map_auto_generate_portal_vert_edge_touch(int skip_portal_idx,int x,int ex,int z);
extern bool map_auto_generate_portal_horz_edge_block(int skip_portal_idx,int z,int ez,int x);
extern bool map_auto_generate_portal_vert_edge_block(int skip_portal_idx,int x,int ex,int z);
extern bool map_auto_generate_portal_touching_left(int portal_idx);
extern bool map_auto_generate_portal_touching_right(int portal_idx);
extern bool map_auto_generate_portal_touching_top(int portal_idx);
extern bool map_auto_generate_portal_touching_bottom(int portal_idx);

extern void map_auto_generate_poly_from_square_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy);
extern void map_auto_generate_poly_from_square_floor(int lx,int lz,int rx,int rz,int fy,int *x,int *y,int *z,float *gx,float *gy);

extern bool map_auto_generate_mesh_start(map_type *map,int box_idx,int group_idx,int txt_idx,bool moveable,bool new_mesh);
extern bool map_auto_generate_mesh_add_poly(map_type *map,int ptsz,int *x,int *y,int *z,float *gx,float *gy);

extern int map_auto_generate_get_stair_type(auto_generate_settings_type *ags);
extern int map_auto_generate_steps_get_length(int ty,int by,int step_size,int step_high);
extern void map_auto_generate_steps_mesh(map_type *map,int rn,int step_type,int step_sz,int step_high,int ty,int by,int kx,int kz,float ang_y);
extern void map_auto_generate_lift(map_type *map,int rn,int step_high,int ty,int by,int lx,int rx,int lz,int rz);

extern int							ag_box_count;

extern auto_generate_settings_type	ag_settings;
extern auto_generate_box_type		ag_boxes[max_ag_box];

/* =======================================================

      Second Story Utilities
      
======================================================= */

void map_auto_generate_story_extra_floor(bool *lft,bool *rgt,bool *top,bool *bot,bool *horz,bool *vert)
{
	*horz=FALSE;
	*vert=FALSE;

		// if only one side is filled, create an L from the other

	if ((*lft) && (!(*rgt)) && (!(*top)) && (!(*bot))) {
		if (map_auto_generate_random_int(100)>50) {
			*top=TRUE;
		}
		else {
			*bot=TRUE;
		}
		return;
	}

	if ((*rgt) && (!(*lft)) && (!(*top)) && (!(*bot))) {
		if (map_auto_generate_random_int(100)>50) {
			*top=TRUE;
		}
		else {
			*bot=TRUE;
		}
		return;
	}

	if ((*top) && (!(*bot)) && (!(*lft)) && (!(*rgt))) {
		if (map_auto_generate_random_int(100)>50) {
			*lft=TRUE;
		}
		else {
			*rgt=TRUE;
		}
		return;
	}

	if ((*bot) && (!(*top)) && (!(*lft)) && (!(*rgt))) {
		if (map_auto_generate_random_int(100)>50) {
			*lft=TRUE;
		}
		else {
			*rgt=TRUE;
		}
		return;
	}

		// if opposite sides, connect

	*horz=((*lft) && (*rgt));
	*vert=((*top) && (*bot));
}

void map_auto_generate_second_story_block(map_type *map,unsigned char *poly_map,int x,int z,int xsz,int zsz,int split_factor,int ty,int by)
{
	int				lx,rx,lz,rz,px[8],py[8],pz[8];
	float			gx[8],gy[8];

		// floor here?

	if (poly_map[(z*xsz)+x]==0x0) return;

		// story size

	lx=x*split_factor;
	rx=lx+split_factor;
	lz=z*split_factor;
	rz=lz+split_factor;

		// create floor and ceiling

	map_auto_generate_poly_from_square_floor(lx,lz,rx,rz,ty,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_floor(lx,lz,rx,rz,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

		// create walls

	if (x!=0) {
		if (poly_map[(z*xsz)+(x-1)]==0x0) {
			map_auto_generate_poly_from_square_wall(lx,lz,lx,rz,ty,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}
	}

	if (x!=(xsz-1)) {
		if (poly_map[(z*xsz)+(x+1)]==0x0) {
			map_auto_generate_poly_from_square_wall(rx,lz,rx,rz,ty,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}
	}

	if (z!=0) {
		if (poly_map[((z-1)*xsz)+x]==0x0) {
			map_auto_generate_poly_from_square_wall(lx,lz,rx,lz,ty,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}
	}

	if (z!=(zsz-1)) {
		if (poly_map[((z+1)*xsz)+x]==0x0) {
			map_auto_generate_poly_from_square_wall(lx,rz,rx,rz,ty,by,px,py,pz,gx,gy);
			map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
		}
	}
}

void map_auto_generate_second_story_stand(map_type *map,int lx,int rx,int lz,int rz,int ty,int by)
{
	int							px[8],py[8],pz[8];
	float						gx[8],gy[8];
	
	map_auto_generate_poly_from_square_wall(lx,lz,lx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(rx,lz,rx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,lz,rx,lz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);

	map_auto_generate_poly_from_square_wall(lx,rz,rx,rz,ty,by,px,py,pz,gx,gy);
	map_auto_generate_mesh_add_poly(map,4,px,py,pz,gx,gy);
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

/* =======================================================

      Second Story Setup
      
======================================================= */

void map_auto_generate_second_story_setup(map_type *map)
{
	int							n;
	bool						lft,rgt,top,bot,horz,vert;
	auto_generate_box_type		*portal;

	if (!ag_settings.second_story) return;

	for (n=0;n!=ag_box_count;n++) {

		portal=&ag_boxes[n];
		if (portal->corridor_flag!=ag_corridor_flag_portal) continue;

			// initially no second story

		portal->story.on=FALSE;

			// find touching edges
		
		lft=map_auto_generate_portal_touching_left(n);
		rgt=map_auto_generate_portal_touching_right(n);
		top=map_auto_generate_portal_touching_top(n);
		bot=map_auto_generate_portal_touching_bottom(n);

		if (!((lft) || (rgt) || (top) || (bot))) continue;

			// remember original settings

		portal->story.org_top=top;
		portal->story.org_bottom=bot;
		portal->story.org_left=lft;
		portal->story.org_right=rgt;

			// add extra floors to make better second
			// stories (like L or middle sections) and

		map_auto_generate_story_extra_floor(&lft,&rgt,&top,&bot,&horz,&vert);

			// remember where second stories are

		portal->story.top=top;
		portal->story.bottom=bot;
		portal->story.left=lft;
		portal->story.right=rgt;
		portal->story.middle_horz=horz;
		portal->story.middle_vert=vert;

		portal->story.on=TRUE;
	}
}

/* =======================================================

      Second Story Generate
      
======================================================= */

void map_auto_generate_second_story(map_type *map)
{
	int							n,portal_high,extra_ty,split_factor,step_wid,step_len,sz,
								x,y,z,by,mx,mz,xsz,zsz,stair_type,portal_sz;
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

			// was there a second story?

		if (!portal->story.on) continue;
		
			// create polygon map

		xsz=(portal->max.x-portal->min.x)/split_factor;
		zsz=(portal->max.z-portal->min.z)/split_factor;

		sz=(xsz+1)*(zsz+1);
		poly_map=(unsigned char*)malloc(sz);
		if (poly_map==NULL) return;

		bzero(poly_map,sz);

		if ((portal->story.left) || (portal->story.right) || (portal->story.middle_vert)) {

			mx=xsz>>1;
		
			for (z=0;z<zsz;z++) {
				if (portal->story.left) poly_map[(z*xsz)]=poly_map[(z*xsz)+1]=0x1;
				if (portal->story.right) poly_map[(z*xsz)+(xsz-1)]=poly_map[(z*xsz)+(xsz-2)]=0x1;
				if (portal->story.middle_vert) poly_map[(z*xsz)+mx]=0x1;
			}
		}

		if ((portal->story.top) || (portal->story.bottom) || (portal->story.middle_horz)) {

			mz=zsz>>1;

			for (x=0;x<xsz;x++) {
				if (portal->story.top) poly_map[x]=poly_map[xsz+x]=0x1;
				if (portal->story.bottom) poly_map[((zsz-1)*xsz)+x]=poly_map[((zsz-2)*xsz)+x]=0x1;
				if (portal->story.middle_horz) poly_map[(mz*xsz)+x]=0x1;
			}
		}

			// create polygons

		if (!map_auto_generate_mesh_start(map,n,-1,ag_settings.texture.second_story,FALSE,TRUE)) return;

		y=(map_max_size>>1)-((portal_high>>1)+ag_constant_step_story_size);

		for (z=0;z<zsz;z++) {
			for (x=0;x<xsz;x++) {
				map_auto_generate_second_story_block(map,poly_map,x,z,xsz,zsz,split_factor,y,(y+ag_constant_story_floor_high));
			}
		}

			// create second story stands

		if (portal->story.left) {
			if (!map_auto_generate_portal_vert_edge_block(n,portal->min.x,(portal->min.x+split_factor),portal->min.z)) {
				map_auto_generate_second_story_stand(map,0,split_factor,0,(split_factor/2),(y+ag_constant_story_floor_high),portal->max.y);
			}

			if (!map_auto_generate_portal_vert_edge_block(n,portal->min.x,(portal->min.x+split_factor),portal->max.z)) {
				z=zsz*split_factor;
				map_auto_generate_second_story_stand(map,0,split_factor,(z-(split_factor/2)),z,(y+ag_constant_story_floor_high),portal->max.y);
			}
		}

		if (portal->story.right) {
			x=xsz*split_factor;

			if (!map_auto_generate_portal_vert_edge_block(n,(portal->min.x+(x-split_factor)),(portal->min.x+x),portal->min.z)) {
				map_auto_generate_second_story_stand(map,(x-split_factor),x,0,(split_factor/2),(y+ag_constant_story_floor_high),portal->max.y);
			}

			if (!map_auto_generate_portal_vert_edge_block(n,(portal->min.x+(x-split_factor)),(portal->min.x+x),portal->max.z)) {
				z=zsz*split_factor;
				map_auto_generate_second_story_stand(map,(x-split_factor),x,(z-(split_factor/2)),z,(y+ag_constant_story_floor_high),portal->max.y);
			}
		}

		if (portal->story.top) {
			if (!map_auto_generate_portal_horz_edge_block(n,portal->min.z,(portal->min.z+split_factor),portal->min.x)) {
				map_auto_generate_second_story_stand(map,0,(split_factor/2),0,split_factor,(y+ag_constant_story_floor_high),portal->max.y);
			}

			if (!map_auto_generate_portal_horz_edge_block(n,portal->min.z,(portal->min.z+split_factor),portal->max.x)) {
				x=xsz*split_factor;
				map_auto_generate_second_story_stand(map,(x-(split_factor/2)),x,0,split_factor,(y+ag_constant_story_floor_high),portal->max.y);
			}
		}

		if (portal->story.bottom) {
			z=zsz*split_factor;

			if (!map_auto_generate_portal_horz_edge_block(n,(portal->min.z+(z-split_factor)),(portal->min.z+z),portal->min.x)) {
				map_auto_generate_second_story_stand(map,0,(split_factor/2),(z-split_factor),z,(y+ag_constant_story_floor_high),portal->max.y);
			}

			if (!map_auto_generate_portal_horz_edge_block(n,(portal->min.z+(z-split_factor)),(portal->min.z+z),portal->max.x)) {
				x=xsz*split_factor;
				map_auto_generate_second_story_stand(map,(x-(split_factor/2)),x,(z-split_factor),z,(y+ag_constant_story_floor_high),portal->max.y);
			}
		}

			// is there a place for steps?

		xsz=portal->max.x-portal->min.x;
		zsz=portal->max.z-portal->min.z;

		step_wid=split_factor*ag_constant_story_steps_split_factor;

		by=portal->max.y;
		
		step_len=map_auto_generate_steps_get_length(y,by,ag_constant_step_story_size,ag_constant_step_story_high);
		stair_type=map_auto_generate_get_stair_type(&ag_settings);

		if ((portal->story.left) && (!portal->story.org_left)) {
			x=split_factor*2;
			z=map_auto_generate_second_story_steps_get_z(n,x,(x+step_len),portal->story.top,zsz,step_wid);
			
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
			if ((portal->story.right) && (!portal->story.org_right)) {
				x=xsz-(split_factor*2);
				z=map_auto_generate_second_story_steps_get_z(n,(x-step_len),x,portal->story.top,zsz,step_wid);

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

		if ((portal->story.top) && (!portal->story.org_top)) {
			z=split_factor*2;
			x=map_auto_generate_second_story_steps_get_x(n,z,(z+step_len),portal->story.left,xsz,step_wid);

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
			if ((portal->story.bottom) && (!portal->story.org_bottom)) {
				z=zsz-(split_factor*2);
				x=map_auto_generate_second_story_steps_get_x(n,(z-step_len),z,portal->story.left,xsz,step_wid);

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


