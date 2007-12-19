/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Video Functions Header

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
	
	// init and shutdown
	
extern bool gl_in_window_mode(void);
extern bool gl_initialize(int screen_wid,int screen_high,bool lock_fps_refresh,int fsaa_mode,bool reset,char *err_str);
extern void gl_shutdown(void);

	// drawables
	
extern bool gl_screen_shot(int lft_x,int top_y,int wid,int high,bool thumbnail,char *path);

	// settings
	
extern bool gl_check_bump_ok(void);
extern bool gl_check_specular_ok(void);
extern bool gl_check_glow_ok(void);
extern bool gl_check_shadow_frame_buffer_ok(void);
extern bool gl_check_shadow_pbuffer_ok(void);
extern bool gl_check_shadow_ok(void);
extern bool gl_check_fsaa_ok(void);
extern bool gl_check_texture_compress_ok(void);
extern bool gl_check_texture_anisotropic_filter_ok(void);
extern bool gl_check_texture_generate_mipmaps_ok(void);
extern bool gl_check_point_sprite_ok(void);
extern bool gl_check_shader_ok(void);

	// rendering arrays
	
extern bool gl_render_arrays_initialize(char *err_str);
extern void gl_render_arrays_shutdown(void);
extern void gl_render_arrays_frame_start(void);
extern void gl_render_arrays_frame_finish(void);
extern void gl_render_array_start(void);
extern float* gl_render_array_get_current_vertex(void);
extern float* gl_render_array_get_current_coord(void);
extern float* gl_render_array_get_current_color(void);
extern float* gl_render_array_get_current_normal(void);
extern void gl_render_array_stop(void);

	// shaders
	
extern void gl_shader_start(void);
extern void gl_shader_end(void);
extern void gl_shader_set_program(GLhandleARB shader_prog_obj);
extern void gl_shader_set_variables(GLhandleARB shader_prog_obj,d3pnt *pnt,texture_type *texture,light_spot_type *lspot);

	// shadows
	
extern bool gl_shadow_initialize(int shadow_mode,char *err_str);
extern void gl_shadow_shutdown(void);
extern bool gl_shadow_texture_render_start(void);
extern void gl_shadow_texture_render_end(void);
extern bool gl_shadow_texture_bind_start(float alpha);
extern void gl_shadow_texture_bind_end(void);

	// pbuffer shadows
	
extern bool gl_shadow_initialize_pbuffer(int shadow_mode,char *err_str);
extern void gl_shadow_shutdown_pbuffer(void);
extern bool gl_shadow_texture_render_start_pbuffer(void);
extern void gl_shadow_texture_render_end_pbuffer(void);
extern bool gl_shadow_texture_bind_start_pbuffer(float alpha);
extern void gl_shadow_texture_bind_end_pbuffer(void);

	// frame buffer shadows
	
extern bool gl_shadow_initialize_frame_buffer(int shadow_mode,char *err_str);
extern void gl_shadow_shutdown_frame_buffer(void);
extern bool gl_shadow_texture_render_start_frame_buffer(void);
extern void gl_shadow_texture_render_end_frame_buffer(void);
extern bool gl_shadow_texture_bind_start_frame_buffer(float alpha);
extern void gl_shadow_texture_bind_end_frame_buffer(void);

	// view
	
extern void gl_frame_start(d3col *col);
extern void gl_frame_end(void);
extern void gl_3D_view(view_camera_type *camera);
extern void gl_3D_clear_rotate(void);
extern void gl_3D_rotate(d3ang *ang);
extern void gl_2D_view(void);
extern void gl_2D_depth_view(int nearz,int farz);
extern void gl_setup_viewport(int y_off);
extern void gl_scale_2D_point(int *x,int *y);
extern void gl_scale_2D_x_coordinate(int *x);
extern void gl_scale_2D_y_coordinate(int *y);
extern void gl_unscale_2D_point(int *x,int *y);
extern void gl_unscale_2D_x_coordinate(int *x);
extern void gl_unscale_2D_y_coordinate(int *y);
extern void gl_setup_project(void);
extern void gl_rotate_point(int *x,int *y,int *z);
extern bool gl_rotate_point_on_screen(int x,int y,int z);
extern void gl_project_point(int *x,int *y,int *z);
extern float gl_project_point_z(int x,int y,int z);
extern void gl_unproject_point(float fx,float fy,float fz,int *x,int *y,int *z);
extern void gl_project_poly(int ptsz,int *x,int *y,int *z);
extern void gl_project_fix_rotation(view_camera_type *camera,int y_off,int *x,int *y,int *z);

	// textures
	
extern void gl_texture_initialize(void);
extern void gl_texture_shutdown(void);
extern void gl_texture_opaque_start(void);
extern void gl_texture_opaque_end(void);
extern void gl_texture_opaque_set(int txt_id);
extern void gl_texture_opaque_lighting_start(void);
extern void gl_texture_opaque_lighting_end(void);
extern void gl_texture_opaque_lighting_set(int txt_id);
extern void gl_texture_opaque_lighting_factor(float dark_factor);
extern void gl_texture_opaque_bump_start(void);
extern void gl_texture_opaque_bump_end(void);
extern void gl_texture_opaque_bump_set(int txt_id,int bump_id);
extern void gl_texture_opaque_bump_factor(float *normal);
extern void gl_texture_opaque_bump_lighting_start(void);
extern void gl_texture_opaque_bump_lighting_end(void);
extern void gl_texture_opaque_bump_lighting_set(int txt_id,int bump_id);
extern void gl_texture_opaque_bump_lighting_factor(float *normal);
extern void gl_texture_tesseled_lighting_start(void);
extern void gl_texture_tesseled_lighting_end(void);
extern void gl_texture_tesseled_lighting_factor(float dark_factor);
extern void gl_texture_opaque_specular_start(void);
extern void gl_texture_opaque_specular_end(void);
extern void gl_texture_opaque_specular_set(int specular_id);
extern void gl_texture_opaque_specular_factor(float specular_factor);
extern void gl_texture_opaque_glow_start(void);
extern void gl_texture_opaque_glow_end(void);
extern void gl_texture_opaque_glow_set(int txt_id,int glow_id);
extern void gl_texture_opaque_glow_color(float glow_color);
extern void gl_texture_decal_start(void);
extern void gl_texture_decal_end(void);
extern void gl_texture_decal_set(int txt_id,float alpha);
extern void gl_texture_transparent_start(void);
extern void gl_texture_transparent_end(void);
extern void gl_texture_transparent_set(int txt_id,float alpha);
extern void gl_texture_transparent_specular_start(void);
extern void gl_texture_transparent_specular_end(void);
extern void gl_texture_transparent_specular_set(int specular_id,float alpha);
extern void gl_texture_transparent_specular_factor(float specular_factor);
extern void gl_texture_transparent_glow_start(void);
extern void gl_texture_transparent_glow_end(void);
extern void gl_texture_transparent_glow_set(int txt_id,int glow_id,float alpha);
extern void gl_texture_transparent_glow_color(float glow_color);
extern void gl_texture_shader_start(void);
extern void gl_texture_shader_end(void);
extern void gl_texture_shader_set(int txt_id,int bump_id,int specular_id,int glow_id);
extern void gl_texture_simple_start(void);
extern void gl_texture_simple_end(void);
extern void gl_texture_simple_set(int txt_id,bool clamp,float r,float g,float b,float alpha);
extern void gl_texture_movie_start(int txt_id);
extern void gl_texture_movie_end(void);

	// text

extern void gl_text_initialize(void);
extern void gl_text_shutdown(void);
extern int gl_text_get_char_width(bool small_text);
extern int gl_text_get_char_height(bool small_text);
extern int gl_text_get_string_width(char *str,bool small_text);
extern void gl_text_start(bool small_text);
extern void gl_text_end(void);
extern void gl_text_draw(int x,int y,char *txt,int just,d3col *col,float alpha);

	// images

extern void view_images_load(void);
extern void view_images_free(void);
extern bitmap_type* view_images_get_bitmap(int idx);
extern unsigned long view_images_get_gl_id(int idx);
