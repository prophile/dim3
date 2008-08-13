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
extern void view_draw_liquid_tint(int liquid_idx);
extern void view_draw_effect_tint(int tick,obj_type *obj);
extern void fade_screen_draw(int tick);
extern void fade_object_draw(int tick,obj_type *obj);
extern void liquid_render(int tick);
extern void decal_render(void);
extern void view_compile_mesh_gl_lists(int tick,int mesh_cnt,int *mesh_list);




int			mesh_draw_count,mesh_draw_list[max_mesh],mesh_draw_dist[max_mesh];		// supergumba -- make part of map



bool		mesh_view_done=FALSE;



#define mesh_view_mesh_wid				640
#define mesh_view_mesh_high				480
#define mesh_view_mesh_aspect_ratio		1.0f
#define mesh_view_mesh_fov				60
#define mesh_view_mesh_near_z			(6*map_enlarge)
#define mesh_view_mesh_far_z			(2000*map_enlarge)
#define mesh_view_mesh_near_z_offset	-(3*map_enlarge)


extern bool boundbox_inview(int x,int z,int ex,int ez,int ty,int by);

int test_me_sort(d3pnt *pt,int *mesh_sort_list)
{
	int					n,t,sz,d,cnt,idx;
	int					*dist;
	map_mesh_type		*mesh;


	dist=valloc(max_mesh*sizeof(int));

	cnt=0;

	for (n=0;n!=map.mesh.nmesh;n++) {

		mesh=&map.mesh.meshes[n];

			// ignore meshes outside of draw distance

		d=distance_get(mesh->box.mid.x,mesh->box.mid.y,mesh->box.mid.z,pt->x,pt->y,pt->z);
		if (d>(mesh_view_mesh_far_z-mesh_view_mesh_near_z)) continue;

			// sort mesh

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




bool temp_mesh_visibility_clip_check(d3pnt *min,d3pnt *max,d3pnt *view_pt,int *vport,double *mod_matrix,double *proj_matrix)
{
	int			n,px[8],py[8],pz[8];
	double		dx,dy,dz;
	bool		hit,lft,rgt,top,bot;

	px[0]=px[3]=px[4]=px[7]=min->x;
	px[1]=px[2]=px[5]=px[6]=max->x;

	py[0]=py[1]=py[2]=py[3]=min->y;
	py[4]=py[5]=py[6]=py[7]=max->y;

	pz[0]=pz[1]=pz[4]=pz[5]=min->z;
	pz[2]=pz[3]=pz[6]=pz[7]=max->z;

		// transform points
		
	for (n=0;n!=8;n++) {
		px[n]-=view_pt->x;
		py[n]-=view_pt->y;
		pz[n]=view_pt->z-pz[n];
	}
	
		// check if points are behind z
		
	hit=FALSE;
	
	for (n=0;n!=8;n++) {
		if (((((double)px[n])*mod_matrix[2])+(((double)py[n])*mod_matrix[6])+(((double)pz[n])*mod_matrix[10])+mod_matrix[14])<0.0) {
			hit=TRUE;
			break;
		}
	}
	
	if (!hit) return(FALSE);

		// check if poly is projected offscreen

	lft=rgt=top=bot=TRUE;

	for (n=0;n!=8;n++) {
		gluProject(px[n],py[n],pz[n],mod_matrix,proj_matrix,(GLint*)vport,&dx,&dy,&dz);

		lft=lft&&(dx<0);
		rgt=rgt&&(dx>=mesh_view_mesh_wid);
		top=top&&(dy<0);
		bot=bot&&(dy>=mesh_view_mesh_high);
	}

	return(!(lft||rgt||top||bot));
}





void temp_mesh_visibility_mesh_setup(int mesh_idx,unsigned char *stencil_pixels,int *mesh_sort_list,float x_rot,float y_rot)
{
	int						n,k,t,mesh_cnt,idx,
							view_mesh_idx;
	bool					transparent;
	unsigned char			*sp;
	d3pnt					*pt;
	map_mesh_type			*mesh,*view_mesh;
	map_mesh_poly_type		*poly;
	texture_type			*texture;
	int						vport[4];
	double					mod_matrix[16],proj_matrix[16];
	
	mesh=&map.mesh.meshes[mesh_idx];
	
		// drawing rotation setup
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(x_rot,1,0,0);						// x pan
	glRotatef(0.0f,0,0,1);						// z pan
	glRotatef(((360.0f-y_rot)+180.0f),0,1,0);	// y rotate -- need to reverse the winding

	glGetIntegerv(GL_VIEWPORT,(GLint*)vport);
	glGetDoublev(GL_PROJECTION_MATRIX,proj_matrix);
	glGetDoublev(GL_MODELVIEW_MATRIX,mod_matrix);

	gl_texture_bind_start();

		// supergumba -- hard code this later as it won't be in game
		
	gl_texture_opaque_start();
	
		// get sort list
		
	mesh_cnt=test_me_sort(&mesh->box.mid,mesh_sort_list);		

		// run through all other meshes to test
		// their visibility against this mesh

		// as we are using the stencil buffer, we can only do up to 255
		// meshes at a time

		// supergumba -- need to do in batches

	glClear(GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	for (n=0;n!=mesh_cnt;n++) {
		
		view_mesh_idx=mesh_sort_list[n];
		view_mesh=&map.mesh.meshes[view_mesh_idx];

			// if this mesh, then auto select it

		if (view_mesh_idx==mesh_idx) {
			mesh->mesh_visibility_flag[view_mesh_idx]=0x1;
			continue;
		}
	
			// ignore all moving meshes as they
			// won't always obscure

		if (view_mesh->flag.moveable) {
			mesh->mesh_visibility_flag[view_mesh_idx]=0x1;
			continue;
		}

			// don't draw meshes clipped from view

		if (!temp_mesh_visibility_clip_check(&view_mesh->box.min,&view_mesh->box.max,&mesh->box.mid,vport,mod_matrix,proj_matrix)) continue;

			// stencil in the mesh we are looking at

		glStencilFunc(GL_ALWAYS,(view_mesh_idx+1),0xFF);

		for (t=0;t!=view_mesh->npoly;t++) {

			poly=&view_mesh->polys[t];

				// transparent polygons don't write into the z
				// but are compared to see if they are obscured

			texture=&map.textures[poly->txt_idx];
			if ((texture->bitmaps[0].alpha_mode==alpha_mode_transparent) || (poly->alpha!=1.0f)) {
				transparent=TRUE;
				glDepthMask(GL_FALSE);
			}

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

			if (transparent) glDepthMask(GL_TRUE);
		}
	}

	gl_texture_opaque_end();

		// read the stencil to look for hits

	glReadPixels(0,0,mesh_view_mesh_wid,mesh_view_mesh_high,GL_STENCIL_INDEX,GL_UNSIGNED_BYTE,stencil_pixels);

	sp=stencil_pixels;

	for (k=0;k!=(mesh_view_mesh_wid*mesh_view_mesh_high);k++) {
		idx=(int)*sp++;
		if (idx!=0) mesh->mesh_visibility_flag[idx-1]=0x1;
	}
}





bool temp_mesh_view_setup(void)
{
	int				n,t,cnt,tick,start_tick;
	int				*mesh_sort_list;
	float			ratio;
	unsigned char	*stencil_pixels;
	
	if (mesh_view_done) return(TRUE);
	
	mesh_view_done=TRUE;

	start_tick=time_get();
	
		// pixels for reading stencil buffer
		
	stencil_pixels=valloc(mesh_view_mesh_wid*mesh_view_mesh_high);
	if (stencil_pixels==NULL) return(FALSE);
	
		// memory for sorted mesh list
		
	mesh_sort_list=valloc(sizeof(int)*max_mesh);
	if (mesh_sort_list==NULL) {
		free(stencil_pixels);
		return(FALSE);
	}

		// global rendering setup

	glViewport(0,0,mesh_view_mesh_wid,mesh_view_mesh_high);
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	ratio=(((float)mesh_view_mesh_wid)/((float)mesh_view_mesh_high))*mesh_view_mesh_aspect_ratio;
	gluPerspective(mesh_view_mesh_fov,ratio,mesh_view_mesh_near_z,mesh_view_mesh_far_z);
	glScalef(-1.0f,-1.0f,1.0f);						// x and y are reversed
	glTranslatef(0.0f,0.0f,(float)mesh_view_mesh_near_z_offset);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_BLEND);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

		// check visibility for all meshes
	
//	for (n=0;n!=map.mesh.nmesh;n++) {
	for (n=0;n!=4;n++) {

		tick=time_get();
	
			// clear visibility bits
		
		bzero(map.mesh.meshes[n].mesh_visibility_flag,max_mesh);
		
			// build mesh list from different angles

		temp_mesh_visibility_mesh_setup(n,stencil_pixels,mesh_sort_list,0.0f,0.0f);
		temp_mesh_visibility_mesh_setup(n,stencil_pixels,mesh_sort_list,0.0f,90.0f);
		temp_mesh_visibility_mesh_setup(n,stencil_pixels,mesh_sort_list,0.0f,180.0f);
		temp_mesh_visibility_mesh_setup(n,stencil_pixels,mesh_sort_list,0.0f,270.0f);
		temp_mesh_visibility_mesh_setup(n,stencil_pixels,mesh_sort_list,90.0f,0.0f);
		temp_mesh_visibility_mesh_setup(n,stencil_pixels,mesh_sort_list,-90.0f,0.0f);

			// supergumba -- mesh count

		cnt=0;
		for (t=0;t!=map.mesh.nmesh;t++) {
			if (map.mesh.meshes[n].mesh_visibility_flag[t]!=0x0) cnt++;
		}

		fprintf(stdout,"mesh %d; time = %d; count = %d/%d\n",n,time_get()-tick,cnt,map.mesh.nmesh);
	}
	
	free(mesh_sort_list);
	free(stencil_pixels);

		// render defaults

	glDisable(GL_STENCIL_TEST);

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

	fprintf(stdout,"total = %d\n",time_get()-start_tick);
	
	return(TRUE);
}







int map_find_mesh(int x,int y,int z)
{
	int				n,d,dist,mesh_idx;
	map_mesh_type	*mesh;

	mesh_idx=-1;

		// look for meshes we are inside of

	dist=map_max_size;

	for (n=0;n!=map.mesh.nmesh;n++) {

			// are we within this mesh?

		mesh=&map.mesh.meshes[n];
		if ((x<mesh->box.min.x) || (x>mesh->box.max.x) || (y<mesh->box.min.y) || (y>mesh->box.max.y) || (z<mesh->box.min.z) || (z>mesh->box.max.z)) continue;

			// might be within multiple meshes, check distances to find best mesh

		d=distance_get(mesh->box.mid.x,mesh->box.mid.y,mesh->box.mid.z,x,y,z);
		if ((mesh_idx==-1) || (d<dist)) {
			dist=d;
			mesh_idx=n;
		}
	}

	if (mesh_idx!=-1) return(mesh_idx);

		// if inside no meshes, then closest mesh

	mesh=map.mesh.meshes;
	dist=map_max_size;

	for (n=0;n!=map.mesh.nmesh;n++) {

		d=distance_get(mesh->box.mid.x,mesh->box.mid.y,mesh->box.mid.z,x,y,z);
		if (d<dist) {
			dist=d;
			mesh_idx=n;
		}

		mesh++;
	}

	if (mesh_idx==-1) return(0);

	return(mesh_idx);
}







void temp_get_mesh_draw_list(void)
{
	int					n,t,sz,d,start_mesh_idx,idx,obscure_dist;
	map_mesh_type		*start_mesh,*mesh;

		// get mesh camera is in

	start_mesh_idx=map_find_mesh(view.camera.pnt.x,view.camera.pnt.y,view.camera.pnt.z);
	start_mesh=&map.mesh.meshes[start_mesh_idx];

		// obscure distance -- normally is the opengl projection
		// distance but can be the fog distance if fog is on


	if (!fog_solid_on()) {
		obscure_dist=view.camera.far_z-view.camera.near_z;
	}
	else {
		obscure_dist=(map.fog.outer_radius>>1)*3;
	}

		// check all visibile meshes from the start mesh
	
	mesh_draw_count=0;
	
	for (n=0;n!=map.mesh.nmesh;n++) {

			// is this mesh visible?

		if (n!=start_mesh_idx) {
			if (start_mesh->mesh_visibility_flag[n]==0x0) continue;
		}

			// auto-eliminate meshes outside of view

		mesh=&map.mesh.meshes[n];

		d=distance_get(mesh->box.mid.x,mesh->box.mid.y,mesh->box.mid.z,view.camera.pnt.x,view.camera.pnt.y,view.camera.pnt.z);
		if (d>obscure_dist) continue;

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
			mesh_draw_list[mesh_draw_count]=n;
			mesh_draw_count++;
			continue;
		}
		
			// insert in list
			
		sz=sizeof(int)*(mesh_draw_count-idx);
		memmove(&mesh_draw_dist[idx+1],&mesh_draw_dist[idx],sz);
		memmove(&mesh_draw_list[idx+1],&mesh_draw_list[idx],sz);
		
		mesh_draw_dist[idx]=d;
		mesh_draw_list[idx]=n;
		
		mesh_draw_count++;
	}
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
		draw_background(view.camera.pnt.x,view.camera.pnt.y,view.camera.pnt.z);
		draw_sky(tick,view.camera.pnt.y);
	}
	else {
		fog_solid_start();
	}
	
		// setup portals for drawing
		
	temp_mesh_view_setup();

	temp_get_mesh_draw_list();
	
	view_compile_mesh_gl_lists(tick,mesh_draw_count,mesh_draw_list);
	
		// draw opaque polygons

	render_opaque_map(mesh_draw_count,mesh_draw_list);

		// draw model shadows

	view_draw_models(tick);
	view_draw_models_shadow();
	
		// draw tranparent polygons
// supergumba -- testing		
//	render_transparent_map(mesh_draw_count,mesh_draw_list);

		// draw liquids

	liquid_render(tick);

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
		
	view_draw_liquid_tint(view.camera.under_liquid_idx);
	view_draw_effect_tint(tick,obj);

	fade_screen_draw(tick);
	fade_object_draw(tick,obj);

		// shadow testing code

//	shadow_render_test();
}

