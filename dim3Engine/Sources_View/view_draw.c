/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Drawing

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "projectiles.h"
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

float						shake_ang_x[16]={-1,0,1,2,1,0,-1,-2,-4,-2,0,4,8,12,8,4};

extern void draw_weapon_hand(int tick,obj_type *obj,weapon_type *weap);
extern void draw_background(int cx,int cy,int cz);
extern void draw_sky(int tick,int y);
extern bool model_inview(model_draw *draw);
extern void model_calc_pose_bones(model_draw *draw);
extern void render_opaque_map(int mesh_cnt,int *mesh_list);
extern void render_transparent_map(int mesh_cnt,int *mesh_list);
extern void rain_draw(int tick);
extern bool fog_solid_on(void);
extern void fog_draw_textured(int tick);
extern void fog_solid_start(void);
extern void fog_solid_end(void);
extern void polygon_segment_start(void);
extern void polygon_segment_end(void);
extern bool model_inview(model_draw *draw);
extern void model_render(int tick,model_draw *draw);
extern void view_draw_liquid_tint(liquid_pointer_type *liq_ptr);
extern void view_draw_effect_tint(int tick,obj_type *obj);
extern void fade_screen_draw(int tick);
extern void fade_object_draw(int tick,obj_type *obj);
extern void liquid_render(int tick,int mesh_cnt,int *mesh_list);
extern void decal_render(void);
extern void portal_compile_gl_lists(int tick,int rn);




unsigned char *pixels=NULL;

int			mesh_draw_count,mesh_draw_list[max_mesh],mesh_draw_dist[max_mesh];		// supergumba -- make part of map



bool		mesh_view_done=FALSE;



#define mesh_view_mesh_wid				640
#define mesh_view_mesh_high				480
#define mesh_view_mesh_aspect_ratio		1.0f
#define mesh_view_mesh_fov				60
#define mesh_view_mesh_near_z			(6*map_enlarge)
#define mesh_view_mesh_far_z			(2000*map_enlarge)
#define mesh_view_mesh_near_z_offset	-(3*map_enlarge)



int test_me_sort(d3pnt *pt,int *mesh_sort_list)
{
	int					n,t,sz,d,cnt,idx;
	int					*dist;
	map_mesh_type		*mesh;


	dist=valloc(max_mesh*sizeof(int));

	cnt=0;

	for (n=0;n!=map.mesh.nmesh;n++) {

		mesh=&map.mesh.meshes[n];

		d=distance_get(mesh->box.mid.x,mesh->box.mid.y,mesh->box.mid.z,pt->x,pt->y,pt->z);
	
		idx=-1;
	
		for (t=0;t!=cnt;t++) {
			if (dist[t]>d) {
				idx=t;
				break;
			}
		}
	
			// insert at end of list
			
		if (idx==-1) {
			dist[cnt]=d;
			mesh_sort_list[cnt]=n;
			cnt++;
			continue;
		}
		
			// insert in list
			
		sz=sizeof(int)*(cnt-idx);
		memmove(&dist[idx+1],&dist[idx],sz);
		memmove(&mesh_sort_list[idx+1],&mesh_sort_list[idx],sz);
		
		dist[idx]=d;
		mesh_sort_list[idx]=n;
		
		cnt++;
	}

	free(dist);

	return(cnt);
}





void temp_mesh_visibility_mesh_setup(int mesh_idx,unsigned char *stencil_pixels,int *mesh_sort_list)
{
	int						n,k,t,idx,mesh_cnt,byte_idx,shift,view_mesh_idx,stencil_idx,stencil_mesh_start_idx;
	float					ratio;
	unsigned char			*sp;
	d3pnt					*pt;
	map_mesh_type			*mesh,*view_mesh;
	map_mesh_poly_type		*poly;
	texture_type			*texture;
	
	mesh=&map.mesh.meshes[mesh_idx];
	
		// clear visibility bits
		
	bzero(mesh->mesh_visibility_flag,max_mesh_visibility_flag_sz);
	
		// drawing setup
		
	glViewport(0,0,mesh_view_mesh_wid,mesh_view_mesh_high);
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	ratio=(((float)mesh_view_mesh_wid)/((float)mesh_view_mesh_high))*mesh_view_mesh_aspect_ratio;
	gluPerspective(mesh_view_mesh_fov,ratio,mesh_view_mesh_near_z,mesh_view_mesh_far_z);
	glScalef(-1.0f,-1.0f,1.0f);						// x and y are reversed
	glTranslatef(0.0f,0.0f,(float)mesh_view_mesh_near_z_offset);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(0.0f,1,0,0);				// x pan
	glRotatef(0.0f,0,0,1);				// z pan
	glRotatef(((360.0f-0.0f)+180.0f),0,1,0);	// y rotate -- need to reverse the winding

	glClear(GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	gl_texture_bind_start();

	glDisable(GL_BLEND);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
	
		// supergumba -- hard code this later as it won't be in game
		
	gl_texture_opaque_start();
	
		// get sort list
		
	mesh_cnt=test_me_sort(&mesh->box.mid,mesh_sort_list);		
	
		// use multiple stencils so
		// we don't have to constantly read from stencil buffer

	stencil_idx=1;
	stencil_mesh_start_idx=0;

		// run through meshes

	for (n=0;n!=mesh_cnt;n++) {
	
		view_mesh_idx=mesh_sort_list[n];
		view_mesh=&map.mesh.meshes[view_mesh_idx];

			// ignore all moving meshes as they
			// won't always obscure

		if (view_mesh->flag.moveable) {
			byte_idx=view_mesh_idx/8;
			shift=view_mesh_idx%8;
			mesh->mesh_visibility_flag[byte_idx]=mesh->mesh_visibility_flag[byte_idx]|(0x1>>shift);
			continue;
		}

			// draw polygons and use the stencil
			// buffer to detect z-buffer changes

		glStencilFunc(GL_ALWAYS,stencil_idx,0xFF);

		for (t=0;t!=view_mesh->npoly;t++) {

			poly=&view_mesh->polys[t];

				// ignore all transparent polygons

			texture=&map.textures[poly->txt_idx];
			if ((texture->bitmaps[0].alpha_mode==alpha_mode_transparent) || (poly->alpha!=1.0f)) continue;

				// write to the stencil buffer if any part
				// of the polygon can be seen

			gl_texture_opaque_set(texture->bitmaps[0].gl_id);

			glBegin(GL_POLYGON);

			for (k=0;k!=poly->ptsz;k++) {
				pt=&view_mesh->vertexes[poly->v[k]];
				glTexCoord2f(poly->gx[k],poly->gy[k]);
				glVertex3i((pt->x-mesh->box.mid.x),(pt->y-mesh->box.mid.y),(mesh->box.mid.z-pt->z));
			}

			glEnd();
		}

			// read the stencil to look for hits

		if ((stencil_idx==255) || ((n+1)==mesh_cnt)) {

			glReadPixels(0,0,mesh_view_mesh_wid,mesh_view_mesh_high,GL_STENCIL_INDEX,GL_UNSIGNED_BYTE,pixels);

			sp=pixels;

			for (k=0;k!=(mesh_view_mesh_wid*mesh_view_mesh_high);k++) {
				
				idx=(int)*sp++;

				if (idx!=0) {
					idx+=stencil_mesh_start_idx;
					byte_idx=idx/8;
					shift=idx%8;
					mesh->mesh_visibility_flag[byte_idx]=mesh->mesh_visibility_flag[byte_idx]|(0x1>>shift);
				}
			}
			
			stencil_idx=1;
			stencil_mesh_start_idx=n+1;

			if ((n+1)!=mesh_cnt) glClear(GL_STENCIL_BUFFER_BIT);
		}
		else {
			stencil_idx++;
		} 
	}

	gl_texture_opaque_end();

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glClear(GL_STENCIL_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glDisable(GL_STENCIL_TEST);
}

bool temp_mesh_view_setup(void)
{
	int				n;
	int				*mesh_sort_list;
	unsigned char	*stencil_pixels;
	
	if (mesh_view_done) return(TRUE);
	
	mesh_view_done=TRUE;
	
		// pixels for reading stencil buffer
		
	stencil_pixels=valloc(mesh_view_mesh_wid*mesh_view_mesh_high);
	if (stencil_pixels==NULL) return(FALSE);
	
		// memory for sorted mesh list
		
	mesh_sort_list=valloc(sizeof(int)*max_mesh);
	if (mesh_sort_list==NULL) {
		free(stencil_pixels);
		return(FALSE);
	}
	
		// check visibility for all meshes
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		temp_mesh_visibility_mesh_setup(n,stencil_pixels,mesh_sort_list);
	}
	
	free(mesh_sort_list);
	free(stencil_pixels);
	
	return(TRUE);
}
















void temp_get_mesh_draw_list(int nportal,int *portal_list)
{
	int					k,t,sz,d,idx;
	map_mesh_type		*mesh;
	
	mesh_draw_count=0;
	
	for (k=0;k!=map.mesh.nmesh;k++) {

		mesh=&map.mesh.meshes[k];

		d=distance_get(mesh->box.mid.x,mesh->box.mid.y,mesh->box.mid.z,view.camera.pos.x,view.camera.pos.y,view.camera.pos.z);
		
			// auto-eliminate meshes outside of view
			
		if (d>(view.camera.far_z-view.camera.near_z)) continue;
		if (!boundbox_inview(mesh->box.min.x,mesh->box.min.z,mesh->box.max.x,mesh->box.max.z,mesh->box.min.y,mesh->box.max.y)) continue;
	
			// top of list is closest items

		idx=-1;
	
		for (t=0;t!=mesh_draw_count;t++) {
			if (mesh_draw_dist[t]>d) {
				idx=t;
				break;
			}
		}
	
			// insert at end of list
			
		if (idx==-1) {
			mesh_draw_dist[mesh_draw_count]=d;
			mesh_draw_list[mesh_draw_count]=k;
			mesh_draw_count++;
			continue;
		}
		
			// insert in list
			
		sz=sizeof(int)*(mesh_draw_count-idx);
		memmove(&mesh_draw_dist[idx+1],&mesh_draw_dist[idx],sz);
		memmove(&mesh_draw_list[idx+1],&mesh_draw_list[idx],sz);
		
		mesh_draw_dist[idx]=d;
		mesh_draw_list[idx]=k;
		
		mesh_draw_count++;
	}
}










// supergumba -- testing out mesh elimination algorithms
void test_me(int nportal,int *portal_list)
{
/*
	int					n,k,t,stencil_idx,stencil_mesh_start_idx,idx,mesh_cnt,portal_idx,mesh_idx,
						portal_cnt,hide_cnt,depth_cnt,dist;
	int					*mesh_sort_list,*mesh_hit_list;
	bool				hit;
	unsigned char		*sp;
	d3pnt				*pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	texture_type		*texture;

	return;
	
	if (pixels==NULL) pixels=valloc(800*600);

		// get sort array for meshes

	mesh_sort_list=valloc(sizeof(int)*max_mesh);

	mesh_cnt=test_me_sort(mesh_sort_list);

	mesh_hit_list=valloc(sizeof(int)*mesh_cnt);
	bzero(mesh_hit_list,(sizeof(int)*mesh_cnt));

		// attempt to check out meshes that draw

	depth_cnt=0;

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();

	gl_texture_bind_start();

	glDisable(GL_BLEND);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

	glClear(GL_STENCIL_BUFFER_BIT);

	glDisable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

		// use multiple stencils so
		// we don't have to constantly read from stencil buffer

	stencil_idx=1;
	stencil_mesh_start_idx=0;

		// run through meshes

	gl_texture_opaque_start();

	for (n=0;n!=mesh_cnt;n++) {

		portal_idx=mesh_sort_list[n]/1000;
		mesh_idx=mesh_sort_list[n]%1000;

		mesh=&map.portals[portal_idx].mesh.meshes[mesh_idx];
		
			// ignore all moving meshes as they
			// won't always obscure

		if (mesh->flag.moveable) continue;

			// draw polygons and use the stencil
			// buffer to detect z-buffer changes

		glStencilFunc(GL_ALWAYS,stencil_idx,0xFF);

		hit=FALSE;

		for (t=0;t!=mesh->npoly;t++) {

			poly=&mesh->polys[t];

				// ignore all transparent polygons

			texture=&map.textures[poly->txt_idx];
			if ((texture->bitmaps[0].alpha_mode==alpha_mode_transparent) || (poly->alpha!=1.0f)) continue;

				// write to the stencil buffer if any part
				// of the polygon can be seen

			gl_texture_opaque_set(texture->bitmaps[0].gl_id);

			glBegin(GL_POLYGON);

			for (k=0;k!=poly->ptsz;k++) {
				pt=&mesh->vertexes[poly->v[k]];
				glTexCoord2f(poly->gx[k],poly->gy[k]);
				glVertex3i((pt->x-view.camera.pos.x),(pt->y-view.camera.pos.y),(view.camera.pos.z-pt->z));
			}

			glEnd();
		}

			// read the stencil to look for hits

		if ((stencil_idx==255) || ((n+1)==mesh_cnt)) {

			glReadPixels(0,0,setup.screen.x_sz,setup.screen.y_sz,GL_STENCIL_INDEX,GL_UNSIGNED_BYTE,pixels);

			sp=pixels;

			for (k=0;k!=(800*600);k++) {
				
				idx=*sp++;

				if (idx!=0) {
					idx+=stencil_mesh_start_idx;
					if (mesh_hit_list[idx]==0x0) {
						depth_cnt++;
						mesh_hit_list[idx]=0x1;
					}
				}
			}
			
			stencil_idx=1;
			stencil_mesh_start_idx=n+1;

			if ((n+1)!=mesh_cnt) glClear(GL_STENCIL_BUFFER_BIT);
		}
		else {
			stencil_idx++;
		} 
	}

	gl_texture_opaque_end();

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	glClear(GL_STENCIL_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glDisable(GL_STENCIL_TEST);

	free(mesh_sort_list);

	portal_cnt=0;

	for (n=0;n!=nportal;n++) {
		portal_cnt+=map.portals[portal_list[n]].mesh.nmesh;
	}

	hide_cnt=0;

	for (n=0;n!=map.nportal;n++) {

		mesh=map.portals[n].mesh.meshes;

		for (k=0;k!=map.portals[n].mesh.nmesh;k++) {


			dist=distance_get(mesh->box.mid.x,mesh->box.mid.y,mesh->box.mid.z,view.camera.pos.x,view.camera.pos.y,view.camera.pos.z);
			if (dist<(view.camera.far_z-view.camera.near_z)) {

				if (boundbox_inview(mesh->box.min.x,mesh->box.min.z,mesh->box.max.x,mesh->box.max.z,mesh->box.min.y,mesh->box.max.y)) hide_cnt++;
			}

			mesh++;
		}
	}

	fprintf(stdout,"portal/zbuffer %d/%d\n",portal_cnt,depth_cnt);
	*/
}


/* =======================================================

      Draw Models in Scene
      
======================================================= */

void view_draw_models_setup(void)
{
	int					i,k,t,dist,sz;
	obj_type			*obj;
	proj_type			*proj;
	view_sort_type		*sort;

	sort=&view.sort;

	sort->count=0;
	
		// sort objects
	
	for (i=0;i!=server.count.obj;i++) {
		obj=&server.objs[i];
		if (!map.portals[obj->pos.rn].in_path) continue;
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
			sz=(sort->count-t)*sizeof(view_sort_item_type);
			memmove(&sort->items[t+1],&sort->items[t],sz);
		}
		
		sort->items[t].type=view_sort_object;
		sort->items[t].idx=(short)i;
		sort->items[t].dist=dist;
		
		sort->count++;
	}

		// sort projectiles


	for (i=0;i!=server.count.proj;i++) {
		proj=&server.projs[i];
		if (!map.portals[proj->pos.rn].in_path) continue;
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
			sz=(sort->count-t)*sizeof(view_sort_item_type);
			memmove(&sort->items[t+1],&sort->items[t],sz);
		}
		
		sort->items[t].type=view_sort_projectile;
		sort->items[t].idx=(short)i;
		sort->items[t].dist=dist;
		
		sort->count++;
	}
}

void view_draw_models(int tick)
{
	int					i;
	obj_type			*obj;
	proj_type			*proj;
	view_sort_type		*sort;

		// setup draw
		
	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();

		// draw models
		
	sort=&view.sort;
	
	for (i=0;i!=sort->count;i++) {

		switch (sort->items[i].type) {

			case view_sort_object:
				obj=&server.objs[sort->items[i].idx];
				if (obj->draw.in_view) {
					model_render(tick,&obj->draw);
					if (obj->remote.on) remote_draw_status(obj);
				}
				break;

			case view_sort_projectile:
				proj=&server.projs[sort->items[i].idx];
				if (proj->draw.in_view) model_render(tick,&proj->draw);
				break;

		}
	}
}

/* =======================================================

      Model Shadows in Scene
      
======================================================= */

void view_create_models_shadow(void)
{
	int					i;
	obj_type			*obj;
	proj_type			*proj;
	view_sort_type		*sort;

	sort=&view.sort;

	if (sort->count==0) return;

		// create shadows backwards to make sure nearest
		// objects get shadow preferences
	
	for (i=(sort->count-1);i>=0;i--) {

		switch (sort->items[i].type) {

			case view_sort_object:
				obj=&server.objs[sort->items[i].idx];
				if (obj->draw.shadow.in_view) shadow_texture_create(&obj->draw);
				break;

			case view_sort_projectile:
				proj=&server.projs[sort->items[i].idx];
				if (proj->draw.shadow.in_view) shadow_texture_create(&proj->draw);
				break;

		}
	}
}

void view_draw_models_shadow(void)
{
	int					i;
	obj_type			*obj;
	proj_type			*proj;
	view_sort_type		*sort;
	
	shadow_render_init();

	sort=&view.sort;
	
	for (i=0;i!=sort->count;i++) {

		switch (sort->items[i].type) {

			case view_sort_object:
				obj=&server.objs[sort->items[i].idx];
				if (obj->draw.shadow.in_view) shadow_render(&obj->draw);
				break;

			case view_sort_projectile:
				proj=&server.projs[sort->items[i].idx];
				if (proj->draw.shadow.in_view) shadow_render(&proj->draw);
				break;

		}
	}
}

/* =======================================================

      Frame Drawing Mainline
      
======================================================= */

void view_draw(int tick)
{
	int				draw_portal_cnt,draw_portal_list[max_portal];
	obj_type		*obj;
	weapon_type		*weap;
	
		// get player object and held weapon
		
	obj=object_find_uid(server.player_obj_uid);
	weap=weapon_find_current(obj);

		// setup viewport
	
	gl_setup_viewport(console_y_offset());

		// setup the models

	view_draw_models_setup();
	
		// render the shadows onto the shadow back buffer
		
	if (shadow_texture_init()) {
		view_create_models_shadow();
		shadow_texture_finish();
	}
	
		// draw background and sky
		// unless obscured by fog
	
	if (!fog_solid_on()) {
		draw_background(view.camera.pos.x,view.camera.pos.y,view.camera.pos.z);
		draw_sky(tick,view.camera.pos.y);
	}
	else {
		fog_solid_start();
	}
	
		// setup portals for drawing
		
	draw_portal_cnt=map_portal_draw_sort(&map,obj->pos.rn,obj->pos.x,obj->pos.y,obj->pos.z,draw_portal_list);

//	for (n=(draw_portal_cnt-1);n>=0;n--) {
//		portal_compile_gl_lists(tick,draw_portal_list[n]);		// supergumba -- run once
//	}
	portal_compile_gl_lists(tick,0);		// supergumba -- run once

//	test_me(draw_portal_cnt,draw_portal_list);	// supergumba -- all temporary

	temp_mesh_view_setup();

	temp_get_mesh_draw_list(draw_portal_cnt,draw_portal_list);
	
		// draw opaque polygons

	render_opaque_map(mesh_draw_count,mesh_draw_list);

		// draw model shadows

	view_draw_models(tick);
	view_draw_models_shadow();
	
		// draw tranparent polygons
		
	render_transparent_map(mesh_draw_count,mesh_draw_list);

		// draw liquids

	liquid_render(tick,mesh_draw_count,mesh_draw_list);

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
	
	if (weap!=NULL) {
		crosshair_setup(tick,obj,weap);
		zoom_setup(tick,obj,weap);
	}
	
		// draw the weapons in hand

	if ((camera.mode==cv_fpp) && (weap!=NULL)) draw_weapon_hand(tick,obj,weap);

		// draw the remote names, halos, crosshairs, and zoom masks
	
	remote_draw_names_render();
	halo_draw_render();
	
	if (weap!=NULL) {
		crosshair_draw(obj,weap);
		zoom_draw(obj,weap);
	}
	
		// draw tints and fades
		
	view_draw_liquid_tint(&view.camera.under_liquid);
	view_draw_effect_tint(tick,obj);

	fade_screen_draw(tick);
	fade_object_draw(tick,obj);

		// shadow testing code

//	shadow_render_test();
}

