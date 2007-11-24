/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Lighting Functions Header

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

extern void light_clear(void);
extern double light_get_intensity(int light_type,int intensity);
extern void light_add(d3pos *pos,int light_type,int intensity,bool confine_to_portal,d3col *col);
extern int light_create_glsl_array(float *light_pos,float *light_col);

extern void halo_initialize(void);
extern void halo_load_bitmaps(void);
extern void halo_free_bitmaps(void);
extern int halo_find(char *name);

extern void halo_draw_clear(void);
extern void halo_draw_add(int x,int z,int y,int obj_uid,model_draw_halo *mdl_halo);
extern void halo_draw_setup(void);
extern void halo_draw_render(void);

extern void shadow_render_init(void);
extern void shadow_render(model_draw *draw);

extern bool shadow_get_volume_ground(model_draw *draw);
extern bool shadow_get_volume_air(model_draw *draw);
extern bool shadow_get_volume(model_draw *draw,bool in_air);
extern bool shadow_inview(model_draw *draw);
extern bool shadow_texture_init(void);
extern void shadow_texture_finish(void);
extern void shadow_texture_create(model_draw *draw);

extern void light_trace_calculate_light_color(portal_type *portal,float x,float y,float z,float *cf);

