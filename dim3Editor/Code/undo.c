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

extern map_type						map;

int									undo_type,undo_nmesh;
map_mesh_type						*undo_meshes=NULL;

/* =======================================================

      Map Undo
      
======================================================= */

void undo_clear(void)
{
	int				n;
	map_mesh_type	*mesh;
	
		// disable menu item
		
	DisableMenuItem(GetMenuRef(app_menu_edit),1);
	
		// clear old backup
	
	if (undo_meshes==NULL) return;
	
	mesh=undo_meshes;
	
	for (n=0;n!=undo_nmesh;n++) {
		if (mesh->vertexes!=NULL) free(mesh->vertexes);
		if (mesh->polys!=NULL) free(mesh->polys);
		
		mesh++;
	}
	
	free(undo_meshes);
	undo_meshes=NULL;
}

void undo_save(void)
{
	int				n,nmesh;
	map_mesh_type	*org_mesh,*mesh;
	
	undo_clear();
	
		// save all meshes in portal
		
	nmesh=map.mesh.nmesh;
	if (nmesh==0) return;
	
	undo_meshes=(map_mesh_type*)malloc(nmesh*sizeof(map_mesh_type));
	if (undo_meshes==NULL) return;
	
	org_mesh=map.mesh.meshes;
	mesh=undo_meshes;
	
	for (n=0;n!=nmesh;n++) {
	
		memmove(mesh,org_mesh,sizeof(map_mesh_type));
		
			// need to dup the vertexes and polygons
			
		mesh->vertexes=(d3pnt*)malloc(org_mesh->nvertex*sizeof(d3pnt));
		if (mesh->vertexes==NULL) {
			free(undo_meshes);
			undo_meshes=NULL;
		}
		
		memmove(mesh->vertexes,org_mesh->vertexes,(org_mesh->nvertex*sizeof(d3pnt)));
		
		mesh->polys=(map_mesh_poly_type*)malloc(org_mesh->npoly*sizeof(map_mesh_poly_type));
		if (mesh->polys==NULL) {
			free(undo_meshes);
			undo_meshes=NULL;
		}
		
		memmove(mesh->polys,org_mesh->polys,(org_mesh->npoly*sizeof(map_mesh_poly_type)));
					
		org_mesh++;
		mesh++;
	}
	
	undo_nmesh=nmesh;

		// enable undo
		
	EnableMenuItem(GetMenuRef(app_menu_edit),1);
}

void undo_restore(void)
{
	int				n;
	map_mesh_type	*org_mesh,*mesh;
	
		// restore all meshes in portal
		
	if (undo_nmesh==0) return;
	
	mesh=map.mesh.meshes;
	org_mesh=undo_meshes;
	
	for (n=0;n!=undo_nmesh;n++) {
	
			// just copy over from backup mesh
			// and then null out the pointers
			
		memmove(mesh,org_mesh,sizeof(map_mesh_type));

		org_mesh->vertexes=NULL;
		org_mesh->polys=NULL;
					
		org_mesh++;
		mesh++;
	}
	
	map.mesh.nmesh=undo_nmesh;

		// clear undo
		
	undo_clear();
}

void undo_run(void)
{
	undo_restore();
	palette_reset();
	main_wind_draw();
}

