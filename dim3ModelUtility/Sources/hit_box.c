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
	int					hit_box_idx;
	model_hit_box_type	*hit_box,*ptr;

		// only allow a maximum number of hit boxes

	if (model->nhit_box>=max_model_hit_box) return(-1);

		// create memory for new hit box

	ptr=(model_hit_box_type*)malloc(sizeof(model_hit_box_type)*(model->nhit_box+1));
	if (ptr==NULL) return(-1);

	if (model->hit_boxes!=NULL) {
		memmove(ptr,model->hit_boxes,(sizeof(model_hit_box_type)*model->nhit_box));
		free(model->hit_boxes);
	}

	model->hit_boxes=ptr;

	hit_box_idx=model->nhit_box;
	model->nhit_box++;

		// initialize the hit box
	
	hit_box=&model->hit_boxes[hit_box_idx];
	bzero(hit_box,sizeof(model_hit_box_type));
	
	strcpy(hit_box->name,"New Hit Box");
	hit_box->box.offset.x=0;
	hit_box->box.offset.z=0;
	hit_box->box.offset.y=0;
	hit_box->box.size.x=200;
	hit_box->box.size.z=200;
	hit_box->box.size.y=200;
	
	return(hit_box_idx);
}

void model_hit_box_delete(model_type *model,int hit_box_idx)
{
	model_hit_box_type	*ptr;

		// is the list completely empty?

	if (model->nhit_box==1) {
		free(model->hit_boxes);
		model->hit_boxes=NULL;
		model->nhit_box=0;
		return;
	}

		// if for some reason we can't create new
		// memory, just shuffle the list and wait
		// until next time

	ptr=(model_hit_box_type*)malloc(sizeof(model_hit_box_type)*(model->nhit_box-1));

	if (ptr==NULL) {
		if (hit_box_idx<(model->nhit_box-1)) {
			memmove(&model->hit_boxes[hit_box_idx],&model->hit_boxes[hit_box_idx+1],(sizeof(model_hit_box_type)*((model->nhit_box-hit_box_idx)-1)));
		}
	}
	else {

		if (hit_box_idx>0) {
			memmove(ptr,model->hit_boxes,(sizeof(model_hit_box_type)*hit_box_idx));
		}
		if (hit_box_idx<(model->nhit_box-1)) {
			memmove(&ptr[hit_box_idx],&model->hit_boxes[hit_box_idx+1],(sizeof(model_hit_box_type)*((model->nhit_box-hit_box_idx)-1)));
		}

		free(model->hit_boxes);
		model->hit_boxes=ptr;
	}
	
	model->nhit_box--;
}

