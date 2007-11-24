/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model PreCalculation Routines

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

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Calculate Model Distances and Widths
      
======================================================= */

void model_calculate_parents(model_type *model)
{
	int					i,n,k,nvertex,nbone;
	model_vertex_type	*vertex;
	model_bone_type		*bone;
	
		// set vertex major/minor bone differences
    
	for (i=0;i!=max_model_mesh;i++) {
	
		nvertex=model->meshes[i].nvertex;
		vertex=model->meshes[i].vertexes;
	
		for (n=0;n!=nvertex;n++) {
		
			k=vertex->major_bone_idx;
			if (k!=-1) {
				vertex->major_dist.x=(float)(vertex->pnt.x-model->bones[k].pnt.x);
				vertex->major_dist.z=(float)(vertex->pnt.z-model->bones[k].pnt.z);
				vertex->major_dist.y=(float)(vertex->pnt.y-model->bones[k].pnt.y);
			}
			else {
				vertex->major_dist.x=0;
				vertex->major_dist.z=0;
				vertex->major_dist.y=0;
			}
			
			k=vertex->minor_bone_idx;
			if (k!=-1) {
				vertex->minor_dist.x=(float)(vertex->pnt.x-model->bones[k].pnt.x);
				vertex->minor_dist.z=(float)(vertex->pnt.z-model->bones[k].pnt.z);
				vertex->minor_dist.y=(float)(vertex->pnt.y-model->bones[k].pnt.y);
			}
			else {
				vertex->minor_dist.x=0;
				vertex->minor_dist.z=0;
				vertex->minor_dist.y=0;
			}
			
			vertex++;
		}
	}
	
		// find the bone to parent bone differences
	
	nbone=model->nbone;
	bone=model->bones;
	
	for (n=0;n!=nbone;n++) {
	
		k=bone->parent_idx;
		if (k!=-1) {
			bone->parent_dist.x=(float)(bone->pnt.x-model->bones[k].pnt.x);
			bone->parent_dist.z=(float)(bone->pnt.z-model->bones[k].pnt.z);
			bone->parent_dist.y=(float)(bone->pnt.y-model->bones[k].pnt.y);
		}
		else {
			bone->parent_dist.x=0;
			bone->parent_dist.z=0;
			bone->parent_dist.y=0;
		}
		
		bone++;
	}
}

