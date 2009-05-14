/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Group Changes

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
	#include "dim3engine.h"
#endif

#include "scripts.h"
#include "objects.h"
#include "effects.h"

extern map_type			map;

/* =======================================================

      Find Groups
      
======================================================= */

int group_find_by_index(char *name)
{
	int				n;
	group_type		*group;
	
	group=map.groups;
	
	for (n=0;n!=map.ngroup;n++) {
		if (strcasecmp(group->name,name)==0) return(n);
		group++;
	}
	
	return(-1);
}

/* =======================================================

      Show or Set Passthrough
      
======================================================= */

void group_show(int group_idx,bool show)
{
	int					n,unit_cnt;
	group_type			*group;
	group_unit_type		*unit_list;
	map_mesh_type		*mesh;

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {
			unit_list++;
			continue;
		}

		mesh=&map.mesh.meshes[unit_list->idx];
		mesh->flag.on=show;

		unit_list++;
	}
}

void group_solid(int group_idx,bool solid)
{
	int					n,unit_cnt;
	group_type			*group;
	group_unit_type		*unit_list;
	map_mesh_type		*mesh;

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {
			unit_list++;
			continue;
		}

		mesh=&map.mesh.meshes[unit_list->idx];
		mesh->flag.pass_through=!solid;

		unit_list++;
	}
}

/* =======================================================

      Texture Changes
      
======================================================= */

void group_texture(int group_idx,int index)
{
	int					n,k,unit_cnt;
	group_type			*group;
	group_unit_type		*unit_list;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {
			unit_list++;
			continue;
		}

		mesh=&map.mesh.meshes[unit_list->idx];

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {
			poly->draw.txt_frame_offset=index;
			poly++;
		}

		unit_list++;
	}
}

void group_texture_shift(int group_idx,float x_shift,float y_shift)
{
	int					n,k,unit_cnt;
	group_type			*group;
	group_unit_type		*unit_list;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {
			unit_list++;
			continue;
		}

		mesh=&map.mesh.meshes[unit_list->idx];

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {
			poly->uv[0].x_shift=x_shift;
			poly->uv[0].y_shift=y_shift;
			poly++;
		}

		unit_list++;
	}
}

void group_texture_alpha(int group_idx,float alpha)
{
	int					n,k,unit_cnt;
	group_type			*group;
	group_unit_type		*unit_list;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	group=&map.groups[group_idx];
	
	unit_cnt=group->unit_count;
	unit_list=group->unit_list;
	
	for (n=0;n!=unit_cnt;n++) {

		if (unit_list->type==group_type_mesh) {
			unit_list++;
			continue;
		}

		mesh=&map.mesh.meshes[unit_list->idx];

		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {
			poly->alpha=alpha;
			poly++;
		}

		unit_list++;
	}
}

