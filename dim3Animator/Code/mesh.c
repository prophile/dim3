/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Variants

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "tab.h"
#include "dialog.h"

extern int					cur_mesh,cur_variant;
extern model_type			model;

/* =======================================================

      Add Mesh
      
======================================================= */

void new_mesh(void)
{
	int					nmesh,i;
	model_mesh_type		*mesh;
	
	nmesh=model.nmesh;
	if (nmesh>=max_model_mesh) return;
		
		// create the new mesh
	
	mesh=&model.meshes[nmesh];
	
	mesh->nvertex=0;
	mesh->ntrig=0;
	
	for (i=0;i!=max_model_texture;i++) {
		mesh->materials[i].trig_start=mesh->materials[i].trig_count=0;
	}
	
	strcpy(mesh->name,"New Mesh");
	
	if (!dialog_mesh_info_run(mesh)) return;
	
	model.nmesh++;
	
	reset_mesh_tab(nmesh,cur_variant);
}

/* =======================================================

      Delete Mesh
      
======================================================= */

void delete_mesh(void)
{
/* supergumba
need to delete and rebuild pointers here
	int					sz;
	
	if (model.nmesh<=1) return;
	
		// delete current mesh
				
	sz=(model.nmesh-cur_mesh)-1;
	if (sz>0) memmove(&model.meshes[cur_mesh],&model.meshes[cur_mesh+1],(sz*sizeof(model_mesh_type)));
		
		// reset model
		
	cur_variant=-1;
	model.meshes[cur_mesh].nvariant--;
	
	reset_mesh_tab(cur_mesh,-1);
	*/
}


