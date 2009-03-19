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

view_render_type			view_render;		// supergumba -- temporary

float						shake_ang_x[16]={-1,0,1,2,1,0,-1,-2,-4,-2,0,4,8,12,8,4};
float						team_color_tint[net_team_count][3]=net_team_color_tint_def;

extern void draw_weapon_hand(int tick,obj_type *obj,weapon_type *weap);
extern void draw_background(void);
extern void draw_sky(int tick);
extern bool model_inview(model_draw *draw);
extern void model_calc_pose_bones(model_draw *draw);
extern void render_map_setup(view_render_type *view_render);
extern void render_map_opaque(view_render_type *view_render);
extern void render_map_transparent(view_render_type *view_render);
extern void rain_draw(int tick);
extern bool fog_solid_on(void);
extern void fog_draw_textured(int tick);
extern void fog_solid_start(void);
extern void fog_solid_end(void);
extern void polygon_segment_start(void);
extern void polygon_segment_end(void);
extern bool model_inview(model_draw *draw);
extern void model_render(int tick,model_draw *draw);
extern void model_render_target(model_draw *draw,d3col *col);
extern void view_draw_liquid_tint(int liquid_idx);
extern void view_draw_effect_tint(int tick,obj_type *obj);
extern void fade_screen_draw(int tick);
extern void fade_object_draw(int tick,obj_type *obj);
extern void render_map_liquid(int tick);
extern void decal_render(view_render_type *view_render);
extern bool view_compile_mesh_gl_lists(int tick,view_render_type *view_render);

extern bool			dim3_debug;

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

void view_draw_models_setup(view_render_type *view_render)
{
	int							n,k,t,dist,sz;
	obj_type					*obj;
	proj_type					*proj;
	view_render_sort_model_type	*sort;

	sort=&view_render->sort_model;

	sort->count=0;
	
		// sort objects
	
	for (n=0;n!=server.count.obj;n++) {
		obj=&server.objs[n];
		if ((obj->hidden) || (obj->draw.uid==-1) || (!obj->draw.on)) continue;
		if (!((obj->draw.in_view) || (obj->draw.shadow.in_view))) continue;
		
			// get object distance
			
		dist=obj->draw.lod_dist;
		
			// sort into list
		
		t=sort->count;
		
		for (k=0;k!=sort->count;k++) {
			if (dist>sort->items[k].dist) {
				t=k;
				break;
			}
		}
		
			// add to list

		if (t>=sort->count) {
			t=sort->count;
		}
		else {
			sz=(sort->count-t)*sizeof(view_render_sort_model_item_type);
			memmove(&sort->items[t+1],&sort->items[t],sz);
		}
		
		sort->items[t].type=view_sort_object;
		sort->items[t].idx=(short)n;
		sort->items[t].dist=dist;
		
		sort->count++;
	}

		// sort projectiles


	for (n=0;n!=server.count.proj;n++) {
		proj=&server.projs[n];
		if ((proj->draw.uid==-1) || (!proj->draw.on)) continue;
		if (!((proj->draw.in_view) || (proj->draw.shadow.in_view))) continue;
		
			// get object distance
			
		dist=proj->draw.lod_dist;
		
			// sort into list
		
		t=sort->count;
		
		for (k=0;k!=sort->count;k++) {
			if (dist>sort->items[k].dist) {
				t=k;
				break;
			}
		}
		
			// add to list

		if (t>=sort->count) {
			t=sort->count;
		}
		else {
			sz=(sort->count-t)*sizeof(view_render_sort_model_item_type);
			memmove(&sort->items[t+1],&sort->items[t],sz);
		}
		
		sort->items[t].type=view_sort_projectile;
		sort->items[t].idx=(short)n;
		sort->items[t].dist=dist;
		
		sort->count++;
	}
}

void view_draw_models(int tick,view_render_type *view_render)
{
	int							n;
	d3col						col;
	obj_type					*obj;
	proj_type					*proj;
	view_render_sort_model_type	*sort;

		// setup draw
		
	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.pnt,&view.camera.ang);
	gl_setup_project();

		// draw models
		
	sort=&view_render->sort_model;
	
	for (n=0;n!=sort->count;n++) {

		switch (sort->items[n].type) {

			case view_sort_object:
				obj=&server.objs[sort->items[n].idx];
				if (!obj->draw.in_view) break;
				
				model_render(tick,&obj->draw);
				if (obj->remote.on) remote_draw_status(obj);
				if (object_is_targetted(obj,&col)) model_render_target(&obj->draw,&col);
				if (dim3_debug) {
					view_draw_debug_bounding_box(obj);
					view_draw_object_path(obj);
				}
				break;

			case view_sort_projectile:
				proj=&server.projs[sort->items[n].idx];
				if (proj->draw.in_view) model_render(tick,&proj->draw);
				break;

		}
	}
}

/* =======================================================

      Model Shadows in Scene
      
======================================================= */

void view_create_models_shadow(view_render_type *view_render)
{
	int							n;
	obj_type					*obj;
	proj_type					*proj;
	view_render_sort_model_type	*sort;

	sort=&view_render->sort_model;

	if (sort->count==0) return;

		// create shadows backwards to make sure nearest
		// objects get shadow preferences
	
	for (n=(sort->count-1);n>=0;n--) {

		switch (sort->items[n].type) {

			case view_sort_object:
				obj=&server.objs[sort->items[n].idx];
				if (obj->draw.shadow.in_view) {
					if (!shadow_texture_create(&obj->draw)) obj->draw.shadow.in_view=FALSE;
				}
				break;

			case view_sort_projectile:
				proj=&server.projs[sort->items[n].idx];
				if (proj->draw.shadow.in_view) {
					if (!shadow_texture_create(&proj->draw)) proj->draw.shadow.in_view=FALSE;
				}
				break;

		}
	}
}

void view_draw_models_shadow(view_render_type *view_render)
{
	int							n;
	obj_type					*obj;
	proj_type					*proj;
	view_render_sort_model_type	*sort;
	
	shadow_render_init();

	sort=&view_render->sort_model;
	
	for (n=0;n!=sort->count;n++) {

		switch (sort->items[n].type) {

			case view_sort_object:
				obj=&server.objs[sort->items[n].idx];
				if (obj->draw.shadow.in_view) shadow_render(view_render,&obj->draw);
				break;

			case view_sort_projectile:
				proj=&server.projs[sort->items[n].idx];
				if (proj->draw.shadow.in_view) shadow_render(view_render,&proj->draw);
				break;

		}
	}
}

/* =======================================================

      Frame Drawing Mainline
      
======================================================= */

void view_draw(int tick)
{
	obj_type		*obj;
	weapon_type		*weap;

		// get player object and held weapon
		
	obj=object_find_uid(server.player_obj_uid);
	weap=weapon_find_current(obj);

		// setup viewport
	
	gl_setup_viewport(console_y_offset());

		// setup the models

	view_draw_models_setup(&view_render);
	
		// render the shadows onto the shadow back buffer
		
	if (shadow_texture_init()) {
		view_create_models_shadow(&view_render);
		shadow_texture_finish();
	}
	
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
	
	if (!view_compile_mesh_gl_lists(tick,&view_render)) return;

		// setup some map polygon drawing flags

	render_map_setup(&view_render);

		// draw opaque map polygons

	render_map_opaque(&view_render);

		// draw model shadows

	view_draw_models(tick,&view_render);
	view_draw_models_shadow(&view_render);
	
		// draw tranparent map polygons

	render_map_transparent(&view_render);

		// draw map liquids

	render_map_liquid(tick);

		// draw decals

	decal_render(&view_render);

		// effects

	effect_draw(tick,&view_render);
	
		// draw rain
		
	rain_draw(tick);

		// draw fog

	fog_draw_textured(tick);
	
	if (fog_solid_on()) fog_solid_end();
	
		// setup halos, crosshairs, zoom masks
		
	remote_draw_names_setup();
	halo_draw_setup(&view_render);
	
	if (weap!=NULL) {
		crosshair_setup(tick,obj,weap);
		zoom_setup(tick,obj,weap);
	}
	
		// draw the weapons in hand

	if ((camera.mode==cv_fpp) && (weap!=NULL)) draw_weapon_hand(tick,obj,weap);

		// draw the remote names, halos, crosshairs, and zoom masks
	
	remote_draw_names_render();
	halo_draw_render(&view_render);
	
	if (weap!=NULL) {
		crosshair_draw(obj,weap);
		zoom_draw(obj,weap);
	}
	
		// draw tints and fades
		
	view_draw_liquid_tint(view.camera.under_liquid_idx);
	view_draw_effect_tint(tick,obj);

	fade_screen_draw(tick);
	fade_object_draw(tick,obj);
}

