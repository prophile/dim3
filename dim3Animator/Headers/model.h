/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Edit, Draw, Click Functions

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

extern void draw_model(model_type *model,int mesh_idx,model_draw_setup *draw_setup);
extern void draw_model_faded(model_type *model,int mesh_idx,model_draw_setup *draw_setup);

extern void draw_model_mesh(model_type *model,int mesh_idx);
extern void draw_model_bones(model_type *model,model_draw_setup *draw_setup,int sel_bone_idx);

extern void draw_model_selected_vertexes(model_type *model,int mesh_idx);
extern void draw_model_boxes(model_type *model,model_draw_setup *draw_setup,int sel_hit_box_idx);
extern void draw_model_normals(model_type *model,int mesh_idx);
extern void draw_model_axis(model_type *model);

extern void draw_model_gl_setup(model_type *model);
extern void draw_model_wind(model_type *model,int mesh_idx,model_draw_setup *draw_setup);
extern void draw_model_setup_pose(model_type *model,model_draw_setup *draw_setup,int wpose);
extern void draw_model_wind_pose(model_type *model,int wmesh,int wpose);
