/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Effect Functions Header

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

extern void effect_start(void);
extern effect_type* effect_spawn(int effecttype,d3pnt *pt,int life_tick);
extern void effect_dispose(void);
extern bool effect_spawn_flash(d3pnt *pt,d3col *col,int intensity,int flash_msec,int fade_msec);
extern bool effect_spawn_lightning(int tick,d3pnt *start_pt,d3pnt *end_pt,int wid,float varient,d3col *col,int life_msec);
extern bool effect_spawn_ray(d3pnt *start_pt,d3pnt *end_pt,int wid,d3col *col,int life_msec);
extern bool effect_spawn_globe(d3pnt *pt,int start_sz,int end_sz,float start_alpha,float end_alpha,d3col *col,int life_msec);
extern bool effect_spawn_shake(d3pnt *pt,int distance,int size,int life_msec);
extern void effect_add_single_light(int tick,effect_type *effect);
extern void effect_add_lights(int tick);
extern void effect_draw_lightning_lines(int nline,float varient,int k,int sx,int sz,int sy,int ex,int ez,int ey,int xadd,int zadd,int yadd);
extern void effect_draw_lightning(int tick,effect_type *effect);
extern void effect_draw_ray(effect_type *effect,int count);
extern void effect_draw_globe(effect_type *effect,int count);
extern void effect_image_animate_get_uv(int tick,image_animation_type *animate,float *gx,float *gy,float *g_size);
extern void effect_draw(int tick);

extern void particle_initialize(void);
extern void particle_precalculate(particle_type *particle);
extern void particle_precalculate_all(void);
extern particle_type* particle_find(char *name);
extern int particle_find_index(char *name);
extern bool particle_spawn(int particle_idx,d3pnt *pt,particle_rotate *rot,particle_motion *motion);
extern bool particle_line_spawn(int particle_idx,d3pnt *start_pt,d3pnt *end_pt,int count);
extern float particle_get_gravity(particle_type *particle,int count);
extern void particle_map_initialize(void);
extern void particle_map_run(void);
extern void particle_draw(effect_type *effect,int count);

extern void ring_initialize(void);
extern ring_type* ring_find(char *name);
extern int ring_find_index(char *name);
extern bool ring_spawn(int ring_idx,d3pnt *pt,d3ang *ang);
extern bool ring_line_spawn(int ring_idx,d3pnt *start_pt,d3pnt *end_pt,int count);
extern void ring_draw(effect_type *effect,int count);

extern void mark_initialize(void);
extern int mark_find(char *name);

extern void decal_clear(void);
extern decal_type* decal_find_free(void);
extern void decal_move_with_mesh(int portal_idx,int mesh_idx,int xmove,int ymove,int zmove);
extern void decal_rotate_with_mesh(int portal_idx,int mesh_idx,float y);
extern void decal_add(d3pos *pos,poly_pointer_type *poly_ptr,int mark_idx,int sz,float alpha);
extern void decal_dispose(void);
