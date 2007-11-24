/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Physics Functions Header

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

extern void box_create_from_point(d3box *box,int x,int y,int z,int wid,int high);
extern void box_create_from_object(d3box *box,obj_type *obj);
extern void box_create_from_projectile(d3box *box,proj_type *proj);
extern void box_add_movement(d3box *box,int mx,int mz);
extern bool box_in_map(d3box *box);
extern bool box_in_portal(d3box *box,portal_type *portal);
extern bool box_collide_2D_line(d3box *box,int lx,int rx,int lz,int rz);

extern void collide_object_polygon(obj_type *obj,int x_add,int z_add,int *px,int *pz);
extern void collide_object_hit_box_polygon(obj_type *obj,model_hit_box_type *hit_box,int *px,int *pz);
extern bool collide_object_to_object(obj_type *obj1,int x_add,int z_add,obj_type *obj2,bool include_y,bool include_stand);
extern bool collide_object_to_object_hit_box(obj_type *obj1,int x_add,int z_add,obj_type *obj2,model_hit_box_type *hit_box);
extern bool collide_object_to_object_slide(obj_type *obj1,int x_add,int z_add,obj_type *obj2,int *lx,int *rx,int *lz,int *rz);
extern bool collide_object_to_object_hit_box_slide(obj_type *obj1,int x_add,int z_add,obj_type *obj2,model_hit_box_type *hit_box,int *lx,int *rx,int *lz,int *rz);
extern void collide_projectile_polygon(proj_type *proj,int *px,int *pz);
extern bool collide_projectile_to_object(proj_type *proj,obj_type *obj);
extern bool collide_projectile_to_object_hit_box(proj_type *proj,obj_type *obj,model_hit_box_type *hit_box);
extern bool collide_projectile_to_projectile(proj_type *proj1,proj_type *proj2);
extern bool collide_sphere_to_object(int sx,int sy,int sz,int radius,obj_type *obj);
extern bool collide_sphere_to_object_hit_box(int sx,int sy,int sz,int radius,obj_type *obj,model_hit_box_type *hit_box);
extern bool collide_sphere_to_projectile(int sx,int sy,int sz,int radius,proj_type *proj);

extern int collide_find_object_for_object_move(obj_type *obj,int x,int z);
extern int collide_find_object_for_object(obj_type *obj);
extern int collide_find_object_for_standing_object(obj_type *obj);
extern int collide_find_object_for_projectile_hit(proj_type *proj,int ignore_obj_uid);
extern int collide_find_object_for_projectile(proj_type *proj,int ignore_obj_uid);
extern int collide_find_projectile_for_projectile(proj_type *proj);
extern bool collide_set_object_hit_box_for_sphere_hit(int sx,int sy,int sz,int radius,obj_type *obj);
extern int collide_find_object_for_sphere_hit(int sx,int sy,int sz,int radius,int ignore_obj_uid);
extern int collide_find_object_for_sphere(int sx,int sy,int sz,int radius,int ignore_obj_uid);
extern void collide_push_objects(int sx,int sz,int sy,int radius,int force);

extern bool map_obj_move_xz(obj_type *obj,int x,int z);
extern bool move_obj_check_xz_map_slide(obj_type *obj,int x,int z);
extern bool move_obj_check_bump(obj_type *obj,int x,int z,int y);
extern bool map_proj_move_xz(proj_type *proj,int ignore_obj_uid,int x,int z);
extern bool move_proj_check_xz_map(proj_type *proj,int x,int z);
extern bool map_spot_empty_sphere(int sx,int sy,int sz,int radius,int ignore_obj_uid,int *hit_obj_uid);
extern bool map_spot_empty_object(obj_type *obj);
extern bool map_spot_empty_projectile(proj_type *proj);

extern int find_fc_for_upward_point(int rn,int x,int y,int z,int ydist,int *hit);
extern int find_fc_for_downward_point(int rn,int x,int y,int z,int ydist,int *hit);
extern int fc_nearest_y(int rn,int x,int y,int z,int ydist,bool ignore_higher);
extern int pin_downward_movement_obj(obj_type *obj,int my);
extern int pin_upward_movement_obj(obj_type *obj,int my);
extern int pin_downward_movement_proj(proj_type *proj,int my);
extern int pin_upward_movement_proj(proj_type *proj,int my);
extern int pin_downward_movement_point(int rn,int x,int y,int z,int my,int *seg_idx);
extern int pin_upward_movement_point(int rn,int x,int y,int z,int my,int *seg_idx);
extern bool map_crush_object(obj_type *obj);
extern bool map_stand_check_object(obj_type *obj);

extern void ray_push(d3pnt *pt,d3ang *ang,int dist);
extern void ray_push_to_end(d3pnt *pt,d3pnt *ept,int dist);
extern bool ray_trace_map_by_angle(d3pnt *spt,d3ang *ang,int dist,d3pnt *hpt,ray_trace_contact *contact);
extern bool ray_trace_map_by_point(d3pnt *spt,d3pnt *ept,d3pnt *hpt,ray_trace_contact *contact);

