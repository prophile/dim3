/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Hit Boxes

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

      Add and Delete Hit Boxes
      
======================================================= */

int model_hit_box_add(model_type *model)
{
	int					nhit_box;
	model_hit_box_type	*hit_box;
	
	nhit_box=model->nhit_box;
	if (nhit_box>=max_model_hit_box) return(-1);
	
		// create the new hit box
	
	hit_box=&model->hit_boxes[nhit_box];
	
	strcpy(hit_box->name,"New Hit Box");
	hit_box->box.offset.x=0;
	hit_box->box.offset.z=0;
	hit_box->box.offset.y=0;
	hit_box->box.size.x=200;
	hit_box->box.size.z=200;
	hit_box->box.size.y=200;
	
	model->nhit_box++;
	
	return(nhit_box);
}

void model_hit_box_delete(model_type *model,int hit_box_idx)
{
	int					nhit_box,sz;
    
        // delete hit box
    
    nhit_box=model->nhit_box;
    
    if (hit_box_idx<(nhit_box-1)) {
        sz=((nhit_box-1)-hit_box_idx)*sizeof(model_hit_box_type);
        memmove(&model->hit_boxes[hit_box_idx],&model->hit_boxes[hit_box_idx+1],sz);
    }
	
	model->nhit_box--;
}

