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
	map->nlight=0;
	map->nsound=0;
	map->nparticle=0;
	map->nspot=0;
	map->nscenery=0;
	map->nnode=0;
	map->nsegment=0;
	map->ngroup=0;
	map->nmovement=0;
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

      Random Types
      
======================================================= */

int map_auto_generate_get_corridor_type(auto_generate_settings_type *ags)
{
	int			n,idx,count;

		// count types

	count=0;

	for (n=0;n!=ag_corridor_type_count;n++) {
		if (ags->corridor.type_on[n]) count++;
	}

	if (count==0) return(ag_corridor_type_normal);

		// get random type

	idx=rand()%count;

	for (n=0;n!=ag_corridor_type_count;n++) {
		if (ags->corridor.type_on[n]) {
			idx--;
			if (idx<0) return(n);
		}
	}

	return(ag_corridor_type_normal);
}

int map_auto_generate_get_ceiling_type(auto_generate_settings_type *ags)
{
	int			n,idx,count;

		// count types

	count=0;

	for (n=0;n!=ag_ceiling_type_count;n++) {
		if (ags->ceiling.type_on[n]) count++;
	}

	if (count==0) return(ag_ceiling_type_closed);

		// get random type

	idx=rand()%count;

	for (n=0;n!=ag_ceiling_type_count;n++) {
		if (ags->ceiling.type_on[n]) {
			idx--;
			if (idx<0) return(n);
		}
	}

	return(ag_ceiling_type_closed);
}

/* =======================================================

      Fix Segments UVs
      
======================================================= */

void map_auto_generate_fix_segments_uv(map_type *map)
{
	register int		i;
	segment_type		*seg;
	
	seg=map->segments;
	
	for (i=0;i!=map->nsegment;i++) {
		map_segment_reset_texture_uvs(map,seg);
		seg++;
	}
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
	
	if (!reverse_slant) {
	
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

	if (new_mesh) {
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

      Segment Adding
      
======================================================= */

void map_auto_generate_segment_start(int group_idx,int primitive_uid,int fill,bool moveable)
{
	map_ag_seg_group_idx=group_idx;
	map_ag_seg_primitive_uid=primitive_uid;
	map_ag_seg_fill=fill;
	map_ag_seg_moveable=moveable;
}

int map_auto_generate_segment_wall(map_type *map,int rn,int lx,int lz,int rx,int rz,int ty,int by,int clip)
{
	segment_type	*seg;

	return(-1);
	
	if (map->nsegment==max_segment) return(-1);
	
	seg=&map->segments[map->nsegment];
	
	seg->type=sg_wall;
	seg->rn=rn;
	seg->fill=map_ag_seg_fill;
    seg->clip=clip;
    seg->curve=cv_none;
	seg->x_txtoff=seg->y_txtoff=0;
	seg->x_txtfact=seg->y_txtfact=1;
    seg->x_shift=seg->y_shift=0;
	seg->txt_ang=0;
	seg->dark_factor=1.0f;
    seg->alpha=1.0f;
	seg->group_idx=map_ag_seg_group_idx;
	seg->primitive_uid[0]=map_ag_seg_primitive_uid;
    seg->pass_through=FALSE;
	seg->moveable=map_ag_seg_moveable;
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

void map_auto_generate_segment_fc(map_type *map,int rn,int type,int lx,int lz,int rx,int rz,int y,int yadd,int lower_mode)
{
	segment_type	*seg;

	return;
	
	if (map->nsegment==max_segment) return;
	
		// create segment
		
	seg=&map->segments[map->nsegment];
	
	seg->type=type;
	seg->rn=rn;
	seg->fill=map_ag_seg_fill;
    seg->clip=wc_none;
    seg->curve=cv_none;
	seg->x_txtoff=seg->y_txtoff=0;
	seg->x_txtfact=seg->y_txtfact=1;
    seg->x_shift=seg->y_shift=0;
	seg->txt_ang=0;
	seg->dark_factor=1.0f;
    seg->alpha=1.0f;
	seg->group_idx=map_ag_seg_group_idx;
	seg->primitive_uid[0]=map_ag_seg_primitive_uid;
    seg->pass_through=FALSE;
	seg->moveable=map_ag_seg_moveable;
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
				seg->data.fc.y[0]+=yadd;
				seg->data.fc.y[3]+=yadd;
				break;
			case ag_ceiling_lower_pos_x:
				seg->data.fc.y[1]+=yadd;
				seg->data.fc.y[2]+=yadd;
				break;
			case ag_ceiling_lower_neg_z:
				seg->data.fc.y[0]+=yadd;
				seg->data.fc.y[1]+=yadd;
				break;
			case ag_ceiling_lower_pos_z:
				seg->data.fc.y[2]+=yadd;
				seg->data.fc.y[3]+=yadd;
				break;
		}
	}
	
	else {
	
		switch (lower_mode) {
			case ag_ceiling_lower_neg_x:
				seg->data.fc.y[0]-=yadd;
				seg->data.fc.y[3]-=yadd;
				break;
			case ag_ceiling_lower_pos_x:
				seg->data.fc.y[1]-=yadd;
				seg->data.fc.y[2]-=yadd;
				break;
			case ag_ceiling_lower_neg_z:
				seg->data.fc.y[0]-=yadd;
				seg->data.fc.y[1]-=yadd;
				break;
			case ag_ceiling_lower_pos_z:
				seg->data.fc.y[2]-=yadd;
				seg->data.fc.y[3]-=yadd;
				break;
		}
	
	}
	
	map->nsegment++;
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
	register int			n;
	register segment_type	*seg;
	int						x,y,z,seg_idx;
	spot_type				*spot;
	
	if (map->nportal==0) return;
	
		// find first floor in first portal
		
	seg_idx=-1;
	
	seg=map->segments;
	
	for (n=0;n!=map->nsegment;n++) {
		if ((seg->rn==0) && (seg->type==sg_floor)) {
			seg_idx=n;
			break;
		}
		seg++;
	}
	
		// spot place

	if (seg_idx!=-1) {
		map_segment_calculate_center(map,seg_idx,&x,&y,&z);
	}
	else {
		map_portal_calculate_center(map,0,&x,&y,&z);
	}
	
	if (x<20) x=20;
	if (z<20) z=20;			// sanity check for getting caught in walls
	
	y--;
	
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

