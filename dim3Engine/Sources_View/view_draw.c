/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Drawing

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "lights.h"
#include "effects.h"
#include "cameras.h"
#include "consoles.h"
#include "interfaces.h"
#include "video.h"

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;
extern hud_type				hud;
extern render_info_type		render_info;

extern bool					dim3_debug;

float						shake_ang_x[16]={-1,0,1,2,1,0,-1,-2,-4,-2,0,4,8,12,8,4};
float						team_color_tint[net_team_count][3]=net_team_color_tint_def;

view_render_type			view_camera_render,view_node_render;

extern void draw_weapon_hand(int tick,obj_type *obj,weapon_type *weap);
extern void draw_background(void);
extern void draw_sky(int tick);
extern void model_calc_pose_bones(model_draw *draw);
extern void render_map_setup(void);
extern void render_map_mesh_opaque(void);
extern void render_map_mesh_transparent(void);
extern void rain_draw(int tick);
extern bool fog_solid_on(void);
extern void fog_draw_textured(int tick);
extern void fog_solid_start(void);
extern void fog_solid_end(void);
extern void polygon_segment_start(void);
extern void polygon_segment_end(void);
extern void render_model_setup(int tick,model_draw *draw);
extern void render_model_opaque(model_draw *draw);
extern void render_model_transparent(model_draw *draw);
extern void render_model_target(model_draw *draw,d3col *col);
extern void view_draw_liquid_tint(int liquid_idx);
extern void view_draw_effect_tint(int tick,obj_type *obj);
extern void fade_screen_draw(int tick);
extern void fade_object_draw(int tick,obj_type *obj);
extern void render_map_liquid_opaque(int tick);
extern void render_map_liquid_transparent(int tick);
extern void decal_render(void);
extern void view_create_area_mask(void);
extern void view_start_draw_list(void);
extern void view_add_mesh_draw_list(void);
extern void view_add_liquid_draw_list(void);
extern void view_setup_objects(int tick);
extern void view_setup_projectiles(int tick);
extern void view_add_effect_draw_list(int tick);
extern void view_add_halos(void);
extern bool view_compile_mesh_gl_lists(int tick);
extern void view_calculate_scope(int tick,obj_type *obj,obj_type *camera_obj);
extern void view_calculate_recoil(obj_type *obj);
extern void view_calculate_shakes(int tick,obj_type *obj);
extern void view_calculate_sways(int tick,obj_type *obj);
extern void view_calculate_bump(obj_type *obj);
extern void shadow_render(model_draw *draw);

/* =======================================================

      Team Colors
      
======================================================= */

void view_team_get_tint(int team_idx,d3col *tint)
{
	if ((team_idx<0) || (team_idx>=net_team_count)) team_idx=0;
	
	tint->r=team_color_tint[team_idx][0];
	tint->g=team_color_tint[team_idx][1];
	tint->b=team_color_tint[team_idx][2];
}

void view_object_get_tint(obj_type *obj,d3col *tint)
{
	int			team_idx;

	team_idx=obj->team_idx;
	if ((team_idx<0) || (team_idx>=net_team_count)) team_idx=0;

	tint->r=team_color_tint[team_idx][0];
	tint->g=team_color_tint[team_idx][1];
	tint->b=team_color_tint[team_idx][2];
}

void view_object_get_ui_color(obj_type *obj,bool no_team_to_default,d3col *col)
{
	int			team_idx;
	
	team_idx=obj->team_idx;
	if ((team_idx<0) || (team_idx>=net_team_count)) team_idx=0;
	
		// if player and no teams, then
		// use the default color so UI
		// won't look gross
				
	if ((obj->player) && (team_idx==net_team_none) && (no_team_to_default)) {
		memmove(col,&hud.color.default_tint,sizeof(d3col));
		return;
	}

		// team colors
		
	col->r=team_color_tint[team_idx][0];
	col->g=team_color_tint[team_idx][1];
	col->b=team_color_tint[team_idx][2];
}

/* =======================================================

      Draw Debug Object
      
======================================================= */

void view_draw_debug_bounding_box(obj_type *obj)
{
	int				n,xsz,ysz,zsz,px[8],py[8],pz[8];
	float			fx,fy,fz;
	matrix_type		rot_x_mat,rot_y_mat,rot_z_mat;
	model_type		*mdl;
	
	mdl=model_find_uid(obj->draw.uid);
	if (mdl==NULL) return;

		// bounding box

	xsz=obj->size.x>>1;
	zsz=obj->size.z>>1;
	
	ysz=obj->size.y;
	if (obj->duck.mode!=dm_stand) ysz-=obj->duck.y_move;

	px[0]=px[1]=px[4]=px[5]=-xsz;
	px[2]=px[3]=px[6]=px[7]=xsz;

	py[0]=py[1]=py[2]=py[3]=-ysz;
	py[4]=py[5]=py[6]=py[7]=0;

	pz[1]=pz[2]=pz[5]=pz[6]=-zsz;
	pz[0]=pz[3]=pz[4]=pz[7]=zsz;

	matrix_rotate_x(&rot_x_mat,obj->draw.setup.ang.x);
	matrix_rotate_z(&rot_z_mat,obj->draw.setup.ang.z);
	matrix_rotate_y(&rot_y_mat,obj->draw.setup.ang.y);

	for (n=0;n!=8;n++) {
		fx=(float)(px[n]-mdl->center.x);
		fy=(float)(py[n]-mdl->center.y);
		fz=(float)(pz[n]-mdl->center.z);
		
		matrix_vertex_multiply(&rot_x_mat,&fx,&fy,&fz);
		matrix_vertex_multiply(&rot_z_mat,&fx,&fy,&fz);
		matrix_vertex_multiply(&rot_y_mat,&fx,&fy,&fz);
		
		px[n]=((int)fx)+mdl->center.x;
		py[n]=((int)fy)+mdl->center.y;
		pz[n]=((int)fz)+mdl->center.z;
	}
	
	for (n=0;n!=8;n++) {
		px[n]=px[n]+obj->pnt.x;
		py[n]=py[n]+obj->pnt.y;
		pz[n]=pz[n]+obj->pnt.z;
	}

		// draw box

	glColor4f(0.2f,0.2f,1.0f,1.0f);
	glLineWidth(2.0f);

	glBegin(GL_LINE_LOOP);
	glVertex3i(px[0],py[0],pz[0]);
	glVertex3i(px[1],py[1],pz[1]);
	glVertex3i(px[2],py[2],pz[2]);
	glVertex3i(px[3],py[3],pz[3]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3i(px[4],py[4],pz[4]);
	glVertex3i(px[5],py[5],pz[5]);
	glVertex3i(px[6],py[6],pz[6]);
	glVertex3i(px[7],py[7],pz[7]);
	glEnd();

	glBegin(GL_LINES);
	glVertex3i(px[0],py[0],pz[0]);
	glVertex3i(px[4],py[4],pz[4]);
	glVertex3i(px[1],py[1],pz[1]);
	glVertex3i(px[5],py[5],pz[5]);
	glVertex3i(px[2],py[2],pz[2]);
	glVertex3i(px[6],py[6],pz[6]);
	glVertex3i(px[3],py[3],pz[3]);
	glVertex3i(px[7],py[7],pz[7]);
	glEnd();

	glLineWidth(1.0f);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
}

void view_draw_object_path(obj_type *obj)
{
	int				yadd;
	d3pnt			pnt;

	if (!object_auto_walk_get_seek_position(obj,&pnt)) return;

	glLineWidth(2.0f);

	glColor4f(0.0f,1.0f,0.0f,1.0f);
	
	yadd=obj->size.y>>1;

	glBegin(GL_LINES);
	glVertex3i(obj->pnt.x,(obj->pnt.y-yadd),obj->pnt.z);
	glVertex3i(pnt.x,(pnt.y-yadd),pnt.z);
	glEnd();

	glLineWidth(1.0f);
}

/* =======================================================

      Draw Models in Scene
      
======================================================= */

void view_draw_model_opaque(int tick)
{
	int					n;
	obj_type			*obj;
	proj_type			*proj;

		// setup draw
		
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

		// render the models

	for (n=0;n!=view.render->draw_list.count;n++) {

		switch (view.render->draw_list.items[n].type) {

			case view_render_type_object:
				obj=&server.objs[view.render->draw_list.items[n].idx];
				render_model_setup(tick,&obj->draw);
				if ((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) render_model_opaque(&obj->draw);
				break;

			case view_render_type_projectile:
				proj=&server.projs[view.render->draw_list.items[n].idx];
				render_model_setup(tick,&proj->draw);
				render_model_opaque(&proj->draw);
				break;

		}
	}
}

void view_draw_model_transparent(int tick)
{
	int					n;
	obj_type			*obj;
	proj_type			*proj;

		// setup draw
		
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

		// render the models
		// draw backwards to sort back to front

	for (n=(view.render->draw_list.count-1);n>=0;n--) {

		switch (view.render->draw_list.items[n].type) {

			case view_render_type_object:
				obj=&server.objs[view.render->draw_list.items[n].idx];
				if ((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) render_model_transparent(&obj->draw);
				break;

			case view_render_type_projectile:
				proj=&server.projs[view.render->draw_list.items[n].idx];
				render_model_transparent(&proj->draw);
				break;

		}
	}
}

void view_draw_models_final(void)
{
	int					n;
	d3col				col;
	obj_type			*obj;
	proj_type			*proj;

		// setup draw
		
	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

		// render the shadows, remote names,
		// and any debugging information

	for (n=0;n!=view.render->draw_list.count;n++) {

		switch (view.render->draw_list.items[n].type) {

			case view_render_type_object:
				obj=&server.objs[view.render->draw_list.items[n].idx];
				
				if (!view.render->no_shadow) {
					if ((view.render->draw_list.items[n].flag&view_list_item_flag_shadow_in_view)!=0x0) shadow_render(&obj->draw);
				}
				
				if ((view.render->draw_list.items[n].flag&view_list_item_flag_model_in_view)!=0x0) {
					if (obj->remote.on) remote_draw_status(obj);
					if (object_is_targetted(obj,&col)) render_model_target(&obj->draw,&col);
					if (dim3_debug) {
						view_draw_debug_bounding_box(obj);
						view_draw_object_path(obj);
					}
				}
				break;

			case view_render_type_projectile:
				proj=&server.projs[view.render->draw_list.items[n].idx];
				shadow_render(&proj->draw);
				break;

		}
	}
}

/* =======================================================

      Drawing Mainline
      
======================================================= */

void view_draw_scene_build(int tick)
{
		// setup projection

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
	
		// compile all lights in map
		
	gl_lights_compile(tick);

		// setup draw lists

	view_create_area_mask();
	view_start_draw_list();

		// add mesh and liquids to draw list

	view_add_mesh_draw_list();
	view_add_liquid_draw_list();
	
		// setup objects and projectiles
		// and add to draw list
		
	view_setup_objects(tick);
	view_setup_projectiles(tick);

		// add scene effects

	view_add_effect_draw_list(tick);

		// add scene halos
		
	halo_draw_clear();
	view_add_halos();
}

void view_draw_scene_render(int tick,obj_type *obj,weapon_type *weap)
{
		// setup projection

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();

		// draw background and sky
		// unless obscured by fog
	
	if (!fog_solid_on()) {
		draw_background();
		draw_sky(tick);
	}
	else {
		fog_solid_start();
	}

		// setup per-scene shader variables

	gl_shader_draw_scene_initialize();

		// compile meshes for drawing
	
	if (!view_compile_mesh_gl_lists(tick)) return;

		// setup some map polygon drawing flags

	render_map_setup();

		// draw opaque scene items

	render_map_mesh_opaque();
	render_map_liquid_opaque(tick);
	view_draw_model_opaque(tick);
	
		// draw transparent scene items

	render_map_mesh_transparent();
	render_map_liquid_transparent(tick);
	view_draw_model_transparent(tick);

		// additional model drawing
		// shadows, remote names, etc

	view_draw_models_final();

		// draw decals

	decal_render();

		// effects

	effect_draw(tick);
	
		// draw rain
		
	rain_draw(tick);

		// draw fog

	fog_draw_textured(tick);
	
	if (fog_solid_on()) fog_solid_end();
	
		// setup halos, crosshairs, zoom masks
		
	remote_draw_names_setup();
	halo_draw_setup();
	
	if ((obj!=NULL) && (weap!=NULL)) {
		crosshair_setup(tick,obj,weap);
		zoom_setup(tick,obj,weap);
	
			// draw the weapons in hand

		if (camera.mode==cv_fpp) draw_weapon_hand(tick,obj,weap);
	}

		// draw the remote names, halos, crosshairs, and zoom masks
	
	remote_draw_names_render();
	halo_draw_render();
	
	if ((obj!=NULL) && (weap!=NULL)) {
		crosshair_draw(obj,weap);
		zoom_draw(obj,weap);
	}
}

void view_draw(int tick)
{
	obj_type		*obj,*camera_obj;
	weapon_type		*weap;

		// get player object and held weapon
		
	obj=object_find_uid(server.player_obj_uid);
	weap=weapon_find_current(obj);

		// camera render

	view.render=&view_camera_render;
	
		// set view camera
	
	camera_obj=object_find_uid(camera.obj_uid);
	camera_get_position(&view.render->camera.pnt,&view.render->camera.ang,camera_obj->size.eye_offset);

	view.render->camera.fov=camera.plane.fov;
	view.render->camera.flip=FALSE;
	view.render->camera.under_liquid_idx=camera_check_liquid(&view.render->camera.pnt);
	
	view.render->no_shadow=FALSE;

		// camera adjustments
	
	if (camera.mode==cv_fpp) {
		view_calculate_scope(tick,obj,camera_obj);
		view_calculate_recoil(obj);
	}
	
	view_calculate_shakes(tick,obj);
	view_calculate_sways(tick,obj);
	view_calculate_bump(obj);
	
		// build the scene
		
	view_draw_scene_build(tick);
	
		// do any back frame rendering
		
	gl_back_render_frame_start(tick);

		// render the scene

	view_draw_scene_render(tick,obj,weap);

		// draw tints and fades
		
	view_draw_liquid_tint(view.render->camera.under_liquid_idx);
	view_draw_effect_tint(tick,obj);

	fade_screen_draw(tick);
	fade_object_draw(tick,obj);
}

bool view_draw_node(int tick,node_type *node,int pixel_size)
{
		// switch out to node render
		
	glViewport(0,0,pixel_size,pixel_size);
	view.render=&view_node_render;
	
		// camera position
		
	memmove(&view.render->camera.pnt,&node->pnt,sizeof(d3pnt));
	memmove(&view.render->camera.ang,&node->ang,sizeof(d3ang));
	
	view.render->camera.fov=camera.plane.fov;
	view.render->camera.flip=TRUE;
	view.render->camera.under_liquid_idx=-1;
	
	view.render->no_shadow=TRUE;

		// draw the scene
	
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		// build the scene
		
	view_draw_scene_build(tick);

		// render the scene

	view_draw_scene_render(tick,NULL,NULL);

		// restore the normal rendering
		
	glViewport(render_info.view_x,render_info.view_y,setup.screen.x_sz,setup.screen.y_sz);
	view.render=&view_camera_render;
	
	return(TRUE);
}

