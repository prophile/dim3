/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Map Undo

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

#include "common_view.h"
#include "interface.h"

extern int							cr;
extern map_type						map;

int									undo_type,undo_nmesh;
map_mesh_type						*undo_meshes=NULL;

/* =======================================================

      Map Undo
      
======================================================= */

void undo_clear(void)
{
	DisableMenuItem(GetMenuRef(app_menu_edit),1);
	
	if (undo_meshes!=NULL) {
		free(undo_meshes);
		undo_meshes=NULL;
	}
}

void undo_save(void)
{
	int				n,nmesh;
	portal_type		*portal;
	map_mesh_type	*org_mesh,*mesh;
	
	undo_clear();
	
		// save all meshes in portal
		
	portal=&map.portals[cr];
		
	nmesh=portal->mesh.nmesh;
	if (nmesh==0) return;
	
	undo_meshes=(map_mesh_type*)valloc(nmesh*sizeof(map_mesh_type));
	if (undo_meshes==NULL) return;
	
	undo_nmesh=nmesh;
		
	org_mesh=portal->mesh.meshes;
	mesh=undo_meshes;
	
	for (n=0;n!=nmesh;n++) {
	
		memmove(mesh,org_mesh,sizeof(map_mesh_type));
		
			// need to dup the vertexes and polygons
			
		mesh->vertexes=(d3pnt*)valloc(org_mesh->nvertex*sizeof(d3pnt));
		if (mesh->vertexes==NULL) {
			free(undo_meshes);
			undo_meshes=NULL;
		}
		
		memmove(mesh->vertexes,org_mesh->vertexes,(org_mesh->nvertex*sizeof(d3pnt)));
		
		mesh->polys=(map_mesh_poly_type*)valloc(org_mesh->npoly*sizeof(map_mesh_poly_type));
		if (mesh->polys==NULL) {
			free(undo_meshes);
			undo_meshes=NULL;
		}
		
		memmove(mesh->polys,org_mesh->polys,(org_mesh->npoly*sizeof(map_mesh_poly_type)));
					
		org_mesh++;
		mesh++;
	}

		// enable undo
		
	EnableMenuItem(GetMenuRef(app_menu_edit),1);
}

void undo_restore(void)
{
/* supergumba
	int				n,k,primitive_uid,
					sel_count,type,index;
	segment_type	*seg;
	
		// move in the segments
		
	seg=undo_segments;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);

		switch (type) {
			case segment_piece:
				memmove(&map.segments[index],seg,sizeof(segment_type));
				seg++;
				break;
			case primitive_piece:
				primitive_uid=map.segments[index].primitive_uid[0];
				for (k=0;k!=map.nsegment;k++) {
					if (map.segments[k].primitive_uid[0]==primitive_uid) {
						memmove(&map.segments[k],seg,sizeof(segment_type));
						seg++;
					}
				}
				break;
		}
	}
	*/
}

void undo_run(void)
{
	undo_restore();
	undo_clear();

	main_wind_draw();
}

