/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Point Routines

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

      Transform a Point Inside Model
	        
======================================================= */

void model_get_point_position(model_draw_setup *draw_setup,int *x,int *y,int *z)
{
	int					cx,cy,cz;
	float				fx,fy,fz;
	matrix_type			rot_x_mat,rot_z_mat,rot_y_mat,sway_mat;
	
	fx=(float)*x;
	fz=(float)*z;
	fy=(float)*y;
	
		// sway
		
	matrix_rotate_zyx(&sway_mat,draw_setup->sway.x,draw_setup->sway.y,draw_setup->sway.z);
	matrix_vertex_multiply(&sway_mat,&fx,&fy,&fz);
		
		// rotate
		
	cx=draw_setup->center.x;
	cy=draw_setup->center.y;
	cz=draw_setup->center.z;
		
	fx-=(float)cx;
	fy-=(float)cy;
	fz-=(float)cz;
	
	matrix_rotate_x(&rot_x_mat,draw_setup->ang.x);
	matrix_rotate_z(&rot_z_mat,draw_setup->ang.z);
	matrix_rotate_y(&rot_y_mat,draw_setup->ang.y);
		
	matrix_vertex_multiply(&rot_x_mat,&fx,&fy,&fz);
	matrix_vertex_multiply(&rot_z_mat,&fx,&fy,&fz);
	matrix_vertex_multiply(&rot_y_mat,&fx,&fy,&fz);

	*x=(int)(fx+draw_setup->move.x)+cx;
	*y=(int)(fy+draw_setup->move.y)+cy;
	*z=(int)(fz+draw_setup->move.z)+cz;
}

/* =======================================================

      Find Position of Single Bone
      
======================================================= */

void model_get_draw_bone_position(model_draw_setup *draw_setup,int bone_idx,int *x,int *y,int *z)
{
	model_draw_bone_type		*draw_bone;
	
	draw_bone=&draw_setup->bones[bone_idx];
	*x=(int)draw_bone->fpnt.x;
	*y=(int)draw_bone->fpnt.y;
	*z=(int)draw_bone->fpnt.z;
	
	model_get_point_position(draw_setup,x,y,z);	
}

void model_calc_draw_bone_position(model_type *model,model_draw_setup *draw_setup,int pose_idx,int bone_idx,int *x,int *y,int *z)
{
	draw_setup->poses[0].idx_1=pose_idx;
	draw_setup->poses[0].idx_2=-1;
    draw_setup->poses[0].factor=0.0f;
	draw_setup->poses[0].acceleration=0.0f;
	
	model_create_draw_bones(model,draw_setup);
	
	model_get_draw_bone_position(draw_setup,bone_idx,x,y,z);
}

/* =======================================================

      Light, Halo, and Name Positions
      
======================================================= */

bool model_get_light_position(model_type *model,model_draw_setup *draw_setup,int idx,int *x,int *y,int *z)
{
	int			bone_idx,px,py,pz;
	
		// get bone position
	
	bone_idx=model->tags.light_bone_idx[idx];
	if (bone_idx==-1) return(FALSE);
	
	model_get_draw_bone_position(draw_setup,bone_idx,&px,&py,&pz);
	
		// add in model position
		
	*x+=px;
	*y+=py;
	*z+=pz;
	
	return(TRUE);
}

bool model_get_halo_position(model_type *model,model_draw_setup *draw_setup,int idx,int *x,int *y,int *z)
{
	int			bone_idx,px,py,pz;
	
		// get bone position
	
	bone_idx=model->tags.halo_bone_idx[idx];
	if (bone_idx==-1) return(FALSE);
		
	model_get_draw_bone_position(draw_setup,bone_idx,&px,&py,&pz);
	
		// add in model position
		
	*x+=px;
	*y+=py;
	*z+=pz;
	
	return(TRUE);
}

bool model_get_name_position(model_type *model,model_draw_setup *draw_setup,int *x,int *y,int *z)
{
	int			bone_idx,px,py,pz;
	
		// get bone position
	
	bone_idx=model->tags.name_bone_idx;
	if (bone_idx==-1) return(FALSE);
		
	model_get_draw_bone_position(draw_setup,bone_idx,&px,&py,&pz);
	
		// add in model position
		
	*x+=px;
	*y+=py;
	*z+=pz;
	
	return(TRUE);
}

