/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Auto-Generation (and Import) Utility Routines

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

int					map_ag_portal_idx,map_ag_mesh_idx,map_ag_poly_txt_idx;

int					map_ag_seg_group_idx,map_ag_seg_primitive_uid,map_ag_seg_fill;
bool				map_ag_seg_moveable;

/* =======================================================

      Import Map Clear
      
======================================================= */

void map_auto_generate_clear(map_type *map)
{
	int				n;
	portal_type		*portal;
	
		// if map already has portals, clear any memory
	
	portal=map->portals;
	
	for (n=0;n!=map->nportal;n++) {
		if (portal->mesh.meshes!=NULL) free(portal->mesh.meshes);
		if (portal->liquid.liquids!=NULL) free(portal->liquid.liquids);
		portal++;
	}
	
		// clear all other settings
		
	map->nportal=0;
	map->nspot=0;
	map->nnode=0;
	map->nscenery=0;
	map->nlight=0;
	map->nsound=0;
	map->nparticle=0;
	map->nmovement=0;
	map->ngroup=0;
}

/* =======================================================

      Random Number Generator
      
======================================================= */

void map_auto_generate_random_seed(int seed)
{
	srandom(seed);
}

int map_auto_generate_random_int(int max)
{
	float			f;

	f=(float)random()/(float)RAND_MAX;
	return((int)(f*(float)max));
}

/* =======================================================

      Portal Collisions and Connections
      
======================================================= */

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

bool map_auto_generate_portal_horz_edge_block(map_type *map,int skip_portal_idx,int z,int ez,int x)
{
	int			n;
	portal_type	*portal;

	for (n=0;n!=map->nportal;n++) {
		if (skip_portal_idx==n) continue;

		portal=&map->portals[n];

		if ((portal->x!=x) && (portal->ex!=x)) continue;
		if ((z>=portal->z) && (z<portal->ez)) return(TRUE);
		if ((ez>portal->z) && (ez<=portal->ez)) return(TRUE);
	}

	return(FALSE);
}

bool map_auto_generate_portal_vert_edge_block(map_type *map,int skip_portal_idx,int x,int ex,int z)
{
	int			n;
	portal_type	*portal;

	for (n=0;n!=map->nportal;n++) {
		if (skip_portal_idx==n) continue;

		portal=&map->portals[n];

		if ((portal->z!=z) && (portal->ez!=z)) continue;
		if ((x>=portal->x) && (x<portal->ex)) return(TRUE);
		if ((ex>portal->x) && (ex<=portal->ex)) return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Edge Touching
      
======================================================= */

bool map_auto_generate_portal_touching_left(map_type *map,int portal_idx,unsigned char *corridor_flags)
{
	int				n;
	portal_type		*portal,*chk_portal;

	portal=&map->portals[portal_idx];
	
	for (n=0;n!=map->nportal;n++) {
		if (portal_idx==n) continue;
		if (corridor_flags[n]!=ag_corridor_flag_portal) continue;		// don't check corridors

		chk_portal=&map->portals[n];
		
		if (((chk_portal->z>=portal->z) && (chk_portal->z<portal->ez)) || ((chk_portal->ez>portal->z) && (chk_portal->ez<=portal->ez))) {
			if (portal->x==chk_portal->ex) return(TRUE);
		}
	}

	return(FALSE);
}

bool map_auto_generate_portal_touching_right(map_type *map,int portal_idx,unsigned char *corridor_flags)
{
	int				n;
	portal_type		*portal,*chk_portal;

	portal=&map->portals[portal_idx];
	
	for (n=0;n!=map->nportal;n++) {
		if (portal_idx==n) continue;
		if (corridor_flags[n]!=ag_corridor_flag_portal) continue;		// don't check corridors

		chk_portal=&map->portals[n];
		
		if (((chk_portal->z>=portal->z) && (chk_portal->z<portal->ez)) || ((chk_portal->ez>portal->z) && (chk_portal->ez<=portal->ez))) {
			if (portal->ex==chk_portal->x) return(TRUE);
		}
	}

	return(FALSE);
}

bool map_auto_generate_portal_touching_top(map_type *map,int portal_idx,unsigned char *corridor_flags)
{
	int				n;
	portal_type		*portal,*chk_portal;

	portal=&map->portals[portal_idx];
	
	for (n=0;n!=map->nportal;n++) {
		if (portal_idx==n) continue;
		if (corridor_flags[n]!=ag_corridor_flag_portal) continue;		// don't check corridors

		chk_portal=&map->portals[n];
		
		if (((chk_portal->x>=portal->x) && (chk_portal->x<portal->ex)) || ((chk_portal->ex>portal->x) && (chk_portal->ex<=portal->ex))) {
			if (portal->z==chk_portal->ez) return(TRUE);
		}
	}

	return(FALSE);
}

bool map_auto_generate_portal_touching_bottom(map_type *map,int portal_idx,unsigned char *corridor_flags)
{
	int				n;
	portal_type		*portal,*chk_portal;

	portal=&map->portals[portal_idx];
	
	for (n=0;n!=map->nportal;n++) {
		if (portal_idx==n) continue;
		if (corridor_flags[n]!=ag_corridor_flag_portal) continue;		// don't check corridors

		chk_portal=&map->portals[n];
		
		if (((chk_portal->x>=portal->x) && (chk_portal->x<portal->ex)) || ((chk_portal->ex>portal->x) && (chk_portal->ex<=portal->ex))) {
			if (portal->ez==chk_portal->z) return(TRUE);
		}
	}

	return(FALSE);
}

bool map_auto_generate_portal_touching_any(map_type *map,int portal_idx,unsigned char *corridor_flags)
{
	if (map_auto_generate_portal_touching_left(map,portal_idx,corridor_flags)) return(TRUE);
	if (map_auto_generate_portal_touching_right(map,portal_idx,corridor_flags)) return(TRUE);
	if (map_auto_generate_portal_touching_top(map,portal_idx,corridor_flags)) return(TRUE);
	if (map_auto_generate_portal_touching_bottom(map,portal_idx,corridor_flags)) return(TRUE);

	return(FALSE);
}

/* =======================================================

      Random Types
      
======================================================= */

int map_auto_generate_get_floor_type(auto_generate_settings_type *ags)
{
	int			n,idx,count;

		// count types

	count=0;

	for (n=0;n!=ag_floor_type_count;n++) {
		if (ags->floor_type_on[n]) count++;
	}

	if (count==0) return(ag_floor_type_flat);

		// get random type

	idx=map_auto_generate_random_int(count);

	for (n=0;n!=ag_floor_type_count;n++) {
		if (ags->floor_type_on[n]) {
			idx--;
			if (idx<0) return(n);
		}
	}

	return(ag_floor_type_flat);
}

int map_auto_generate_get_ceiling_type(auto_generate_settings_type *ags)
{
	int			n,idx,count;

		// count types

	count=0;

	for (n=0;n!=ag_ceiling_type_count;n++) {
		if (ags->ceiling_type_on[n]) count++;
	}

	if (count==0) return(ag_ceiling_type_closed);

		// get random type

	idx=map_auto_generate_random_int(count);

	for (n=0;n!=ag_ceiling_type_count;n++) {
		if (ags->ceiling_type_on[n]) {
			idx--;
			if (idx<0) return(n);
		}
	}

	return(ag_ceiling_type_closed);
}

int map_auto_generate_get_corridor_type(auto_generate_settings_type *ags)
{
	int			n,idx,count;

		// count types

	count=0;

	for (n=0;n!=ag_corridor_type_count;n++) {
		if (ags->corridor_type_on[n]) count++;
	}

	if (count==0) return(ag_corridor_type_normal);

		// get random type

	idx=map_auto_generate_random_int(count);

	for (n=0;n!=ag_corridor_type_count;n++) {
		if (ags->corridor_type_on[n]) {
			idx--;
			if (idx<0) return(n);
		}
	}

	return(ag_corridor_type_normal);
}

/* =======================================================

      Polygon Utilities
      
======================================================= */

void map_auto_generate_poly_from_square_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy)
{
	x[0]=x[3]=lx;
	x[1]=x[2]=rx;
	z[0]=z[3]=lz;
	z[1]=z[2]=rz;

	y[0]=y[1]=ty;
	y[2]=y[3]=by;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;
}

void map_auto_generate_poly_from_top_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy)
{
	x[0]=x[2]=lx;
	x[1]=rx;
	z[0]=z[2]=lz;
	z[1]=rz;

	y[0]=ty;
	y[1]=y[2]=by;

	gx[0]=gx[2]=0.0f;
	gx[1]=1.0f;
	gy[0]=0.0f;
	gy[1]=gy[2]=1.0f;
}

void map_auto_generate_poly_from_bot_trig_wall(int lx,int lz,int rx,int rz,int ty,int by,int *x,int *y,int *z,float *gx,float *gy)
{
	x[0]=x[2]=lx;
	x[1]=rx;
	z[0]=z[2]=lz;
	z[1]=rz;

	y[0]=by;
	y[1]=y[2]=ty;

	gx[0]=gx[2]=0.0f;
	gx[1]=1.0f;
	gy[0]=0.0f;
	gy[1]=gy[2]=1.0f;
}

void map_auto_generate_poly_from_square_floor(int lx,int lz,int rx,int rz,int fy,int *x,int *y,int *z,float *gx,float *gy)
{
	x[0]=x[3]=lx;
	x[1]=x[2]=rx;
	z[0]=z[1]=lz;
	z[2]=z[3]=rz;
		
	y[0]=y[1]=y[2]=y[3]=fy;

	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;
}

void map_auto_generate_poly_from_square_floor_slant(int lx,int lz,int rx,int rz,int fy,int yadd,int lower_mode,bool reverse_slant,int *x,int *y,int *z,float *gx,float *gy)
{
	map_auto_generate_poly_from_square_floor(lx,lz,rx,rz,fy,x,y,z,gx,gy);
	
	if (reverse_slant) {
	
		switch (lower_mode) {
			case ag_ceiling_lower_neg_x:
				y[0]+=yadd;
				y[3]+=yadd;
				break;
			case ag_ceiling_lower_pos_x:
				y[1]+=yadd;
				y[2]+=yadd;
				break;
			case ag_ceiling_lower_neg_z:
				y[0]+=yadd;
				y[1]+=yadd;
				break;
			case ag_ceiling_lower_pos_z:
				y[2]+=yadd;
				y[3]+=yadd;
				break;
		}
	}
	
	else {
	
		switch (lower_mode) {
			case ag_ceiling_lower_neg_x:
				y[0]-=yadd;
				y[3]-=yadd;
				break;
			case ag_ceiling_lower_pos_x:
				y[1]-=yadd;
				y[2]-=yadd;
				break;
			case ag_ceiling_lower_neg_z:
				y[0]-=yadd;
				y[1]-=yadd;
				break;
			case ag_ceiling_lower_pos_z:
				y[2]-=yadd;
				y[3]-=yadd;
				break;
		}
	
	}
}

/* =======================================================

      Mesh/Poly Adding
      
======================================================= */

bool map_auto_generate_mesh_start(map_type *map,int portal_idx,int group_idx,int txt_idx,bool moveable,bool new_mesh)
{
	map_mesh_type			*mesh;

	map_ag_portal_idx=portal_idx;
	map_ag_mesh_idx=0;

	map_ag_poly_txt_idx=txt_idx;

		// need a new mesh?

	if ((new_mesh) || (map->portals[portal_idx].mesh.nmesh==0)) {
		map_ag_mesh_idx=map_portal_mesh_add(map,portal_idx);
		if (map_ag_mesh_idx==-1) return(FALSE);

		mesh=&map->portals[portal_idx].mesh.meshes[map_ag_mesh_idx];
		mesh->group_idx=group_idx;
		mesh->flag.moveable=moveable;
	}

	return(TRUE);
}

bool map_auto_generate_mesh_add_poly(map_type *map,int ptsz,int *x,int *y,int *z,float *gx,float *gy)
{
	return(map_portal_mesh_add_poly(map,map_ag_portal_idx,map_ag_mesh_idx,ptsz,x,y,z,gx,gy,map_ag_poly_txt_idx)!=-1);
}

/* =======================================================

      Create Simple Lights
      
======================================================= */

void map_auto_generate_add_simple_lights(map_type *map)
{
	int					n,x,z,y,intensity;
	double				dx,dz;
	portal_type			*portal;
	map_light_type		*lit;

	portal=map->portals;
	
	for (n=0;n!=map->nportal;n++) {

		if (map->nlight>=max_map_light) break;
	
			// find light position
			
		map_portal_calculate_center(map,n,&x,&y,&z);
		
			// get intensity
			
		dx=(portal->ex-portal->x);
		dz=(portal->ez-portal->z);
		intensity=(int)sqrt((dx*dx)+(dz*dz));
		
		intensity/=2;								// radius, so use half
		if (intensity<100) intensity=100;
		
			// create the light
	
		lit=&map->lights[map->nlight];
		map->nlight++;
		
		lit->pos.rn=n;
		lit->pos.x=x;
		lit->pos.z=z;
		lit->pos.y=y;
		lit->type=lt_normal;
		lit->col.r=lit->col.g=lit->col.b=1.0f;
		lit->intensity=intensity;
		lit->on=TRUE;

		portal++;
	}
}

/* =======================================================

      Add Player Start
      
======================================================= */

void map_auto_generate_add_player_spot(map_type *map)
{
	int					x,y,z,ty;
	spot_type			*spot;
	
	if (map->nportal==0) return;
		
		// spot place
	
	map_portal_calculate_center(map,0,&x,&y,&z);
	map_portal_calculate_y_extent(map,0,&ty,&y);
	
		// supergumba -- temp fix for rough floors
		
	y-=(map_enlarge*5);
	
		// add spot
		
	spot=&map->spots[0];
	map->nspot=1;
	
	spot->pos.rn=0;
	spot->pos.x=x;
	spot->pos.z=z;
	spot->pos.y=y;
	spot->ang.y=135;
	strcpy(spot->name,"Start");
	strcpy(spot->type,"Player");
	strcpy(spot->script,"Player");
	spot->display_model[0]=0x0;
	spot->params[0]=0x0;
}

