/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Window Clicking

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

#include "model.h"
#include "window.h"
#include "tab.h"
#include "menu.h"

extern int						cur_mesh,cur_pose,cur_bone,draw_type,shift_x,shift_y,magnify_z,drag_bone_mode,
								gl_view_x_sz,gl_view_y_sz;
extern float					ang_y,ang_x;
extern bool						shift_on,rotate_on,size_on,play_animate,drag_sel_on,model_bone_drag_on;
extern Rect						drag_sel_box;
extern CCrsrHandle				shift_cursor,rotate_cursor,size_cursor,add_cursor,sub_cursor,bone_drag_cursor;

extern model_type				model;
extern model_draw_setup			draw_setup;

extern void draw_model_bones_get_handle_rot(model_type *model,model_draw_setup *draw_setup,int bone_idx,d3ang *rot);
extern float draw_model_bones_drag_handle_offset(model_type *model);
extern void draw_model_bones_drag_handle_calc(float x,float y,float z,d3vct *vct,d3ang *ang,float *hx,float *hy,float *hz);
extern void redraw_pose_row(void);

/* =======================================================

      Model Vertex Selection
      
======================================================= */

void model_sel_vertex(int lx,int ty,int rx,int by,bool chg_sel,double *mod_matrix,double *proj_matrix,double *vport)
{
	int					i,x,y,nt;
	float				*pv;
	double				sx,sy,sz,dx,dy,dz;
	model_vertex_type	*vertex;
	model_mesh_type		*mesh;

		// find selection
		
	mesh=&model.meshes[cur_mesh];
		
	nt=mesh->nvertex;
	vertex=mesh->vertexes;
	
	pv=mesh->draw.gl_vertex_array;

	for (i=0;i!=nt;i++) {
		sx=(int)*pv++;
		sy=(int)*pv++;
		sz=(int)*pv++;
		gluProject(sx,sy,sz,mod_matrix,proj_matrix,(GLint*)vport,&dx,&dy,&dz);
		x=(int)dx;
		y=(int)((gl_view_y_sz+texture_palette_height)-dy);
		
		if ((x>=lx) && (x<=rx) && (y>=ty) && (y<=by)) {
			if (!vertex_check_hide_mask(cur_mesh,i)) vertex_set_sel_mask(cur_mesh,i,chg_sel);
		}
		
		vertex++;
	}
}

void select_model_wind_save_sel_state(char *vertex_sel)
{
	int					i,nt;
	model_vertex_type	*vertex;

	nt=model.meshes[cur_mesh].nvertex;
	vertex=model.meshes[cur_mesh].vertexes;

	for (i=0;i!=nt;i++) {
		vertex_sel[i]=(char)vertex_check_sel_mask(cur_mesh,i);
		vertex++;
	}
}

void select_model_wind_restore_sel_state(char *vertex_sel)
{
	int					i,nt;
	model_vertex_type	*vertex;

	nt=model.meshes[cur_mesh].nvertex;
	vertex=model.meshes[cur_mesh].vertexes;

	for (i=0;i!=nt;i++) {
		vertex_set_sel_mask(cur_mesh,i,(vertex_sel[i]!=0));
		vertex++;
	}
}

void select_model_wind(Point start_pt,unsigned long modifiers)
{
	int						lx,rx,ty,by;
	double					mod_matrix[16],proj_matrix[16],vport[4];
	char					org_vertex_sel[max_model_vertex];
	bool					chg_sel;
	Point					pt,last_pt;
	MouseTrackingResult		track;
	
	model_wind_play(FALSE,FALSE);
	
		// setup the draw pose
		
	draw_model_setup_pose(&model,&draw_setup,cur_pose);
	
	model_create_draw_bones(&model,&draw_setup);
	model_create_draw_vertexes(&model,cur_mesh,&draw_setup);
	
		// setup transforms
		
	draw_model_gl_setup(&model);
	glGetDoublev(GL_MODELVIEW_MATRIX,mod_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,proj_matrix);
	glGetIntegerv(GL_VIEWPORT,(GLint*)vport);
	
		// turn on or off?
		
	if ((modifiers&shiftKey)!=0) {
		select_model_wind_save_sel_state(org_vertex_sel);
		SetCCursor(add_cursor);
		chg_sel=TRUE;
	}
	else {
		if ((modifiers&controlKey)!=0) {
			select_model_wind_save_sel_state(org_vertex_sel);
			SetCCursor(sub_cursor);
			chg_sel=FALSE;
		}
		else {
			memset(org_vertex_sel,0x0,max_model_vertex);
			InitCursor();
			chg_sel=TRUE;
		}
	}
	
		// drag the selection

	last_pt.h=last_pt.v=-1;
	
	drag_sel_on=TRUE;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);
		model_wind_offset_click(&pt);
		
		if (memcmp(&last_pt,&pt,sizeof(Point))==0) continue;
		
		memmove(&last_pt,&pt,sizeof(Point));
		
		if (start_pt.h<last_pt.h) {
			lx=start_pt.h;
			rx=last_pt.h;
		}
		else {
			rx=start_pt.h;
			lx=last_pt.h;
		}
		if (start_pt.v<last_pt.v) {
			ty=start_pt.v;
			by=last_pt.v;
		}
		else {
			by=start_pt.v;
			ty=last_pt.v;
		}
		
		select_model_wind_restore_sel_state(org_vertex_sel);
		model_sel_vertex(lx,ty,rx,by,chg_sel,mod_matrix,proj_matrix,vport);
		
		SetRect(&drag_sel_box,lx,ty,rx,by);
		draw_model_wind_pose(&model,cur_mesh,cur_pose);
	
	} while (track!=kMouseTrackingMouseReleased);
	
	drag_sel_on=FALSE;
	
	draw_model_wind_pose(&model,cur_mesh,cur_pose);
		
		// reset the data browser
		
	hilite_vertex_rows();

	InitCursor();
}

/* =======================================================

      Model Movement
      
======================================================= */

void change_model_wind(Point start_pt)
{
	int						old_shift_x,old_shift_y,old_magnify_z;
	float					old_ang_y,old_ang_x;
	Point					pt,last_pt;
	MouseTrackingResult		track;
	
		// rotate the model

	last_pt.h=last_pt.v=-1;
	
	old_shift_x=shift_x;
	old_shift_y=shift_y;
	old_ang_y=ang_y;
	old_ang_x=ang_x;
	old_magnify_z=magnify_z;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);
		model_wind_offset_click(&pt);
		
		if (memcmp(&last_pt,&pt,sizeof(Point))==0) continue;
		
		memmove(&last_pt,&pt,sizeof(Point));
		
		if (shift_on) {
			shift_x=old_shift_x+((last_pt.h-start_pt.h)*4);
			shift_y=old_shift_y-((last_pt.v-start_pt.v)*4);
		}
		if (rotate_on) {
			ang_y=old_ang_y+(float)((last_pt.h-start_pt.h)/5);
			ang_x=old_ang_x-(float)((last_pt.v-start_pt.v)/5);
		}
		if (size_on) {
			magnify_z=old_magnify_z+((last_pt.v-start_pt.v)*2);
		}
		
		if (!play_animate) draw_model_wind_pose(&model,cur_mesh,cur_pose);
	
	} while (track!=kMouseTrackingMouseReleased);
	
	if (!play_animate) draw_model_wind_pose(&model,cur_mesh,cur_pose);
}

/* =======================================================

      Model Window  Bone Clicks
      
======================================================= */

bool draw_bone_model_wind_click_box(Point start_pt,float x,float y,float z)
{
	int				ix,iy;
	double			dx,dy,dz,mod_matrix[16],proj_matrix[16],vport[4];
	Rect			box;
	
		// reverse the projection
		
	glGetDoublev(GL_MODELVIEW_MATRIX,mod_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,proj_matrix);
	glGetIntegerv(GL_VIEWPORT,(GLint*)vport);
	
	gluProject(x,y,z,mod_matrix,proj_matrix,(GLint*)vport,&dx,&dy,&dz);
	ix=(int)dx;
	iy=(int)((gl_view_y_sz+texture_palette_height)-dy);

		// check box
		
	SetRect(&box,(ix-6),(iy-6),(ix+6),(iy+6));
	return(PtInRect(start_pt,&box));
}

bool drag_bone_model_wind(Point start_pt)
{
	int						n,k,drag_handle;
	float					x,y,z,hx,hy,hz,org_ang,org_mov,
							bone_drag_handle_offset;
	float					*ang,*mov;
	Point					pt,last_pt;
	d3vct					vct;
	d3ang					hang,rot;
	model_draw_bone_type	*draw_bone;
	MouseTrackingResult		track;
	
	model_wind_play(FALSE,FALSE);
	
		// setup the draw pose
		
	draw_model_setup_pose(&model,&draw_setup,cur_pose);
	model_create_draw_bones(&model,&draw_setup);
	
		// setup transforms
		
	draw_model_gl_setup(&model);
	
		// click on any drag handles?
		
	drag_handle=drag_handle_none;
	
	if (cur_bone!=-1) {
	
		draw_bone=&draw_setup.bones[cur_bone];
		
		x=draw_bone->fpnt.x+draw_setup.move.x;
		y=draw_bone->fpnt.y+draw_setup.move.y;
		z=draw_bone->fpnt.z+draw_setup.move.z;
		
		bone_drag_handle_offset=draw_model_bones_drag_handle_offset(&model);

		draw_model_bones_get_handle_rot(&model,&draw_setup,cur_bone,&rot);
		
			// x drag bone

		vct.x=bone_drag_handle_offset;
		vct.y=0;
		vct.z=0;
		hang.x=0;
		hang.y=rot.y;
		hang.z=rot.z;
		draw_model_bones_drag_handle_calc(x,y,z,&vct,&hang,&hx,&hy,&hz);
		if (draw_bone_model_wind_click_box(start_pt,hx,hy,hz)) drag_handle=drag_handle_x;
		
			// y drag bone
			
		vct.x=0;
		vct.y=bone_drag_handle_offset;
		vct.z=0;
		hang.x=rot.x;
		hang.y=0;
		hang.z=rot.z;
		draw_model_bones_drag_handle_calc(x,y,z,&vct,&hang,&hx,&hy,&hz);
		if (draw_bone_model_wind_click_box(start_pt,hx,hy,hz)) drag_handle=drag_handle_y;
		
			// z drag bone
			
		vct.x=0;
		vct.y=0;
		vct.z=bone_drag_handle_offset;
		hang.x=rot.x;
		hang.y=rot.y;
		hang.z=0;
		draw_model_bones_drag_handle_calc(x,y,z,&vct,&hang,&hx,&hy,&hz);
		if (draw_bone_model_wind_click_box(start_pt,hx,hy,hz)) drag_handle=drag_handle_z;
	}
	
		// click on any bones?
		
	if (drag_handle==drag_handle_none) {
	
		k=-1;
		draw_bone=draw_setup.bones;
			
		for (n=0;n!=model.nbone;n++) {
			x=draw_bone->fpnt.x+draw_setup.move.x;
			y=draw_bone->fpnt.y+draw_setup.move.y;
			z=draw_bone->fpnt.z+draw_setup.move.z;
			
			if (draw_bone_model_wind_click_box(start_pt,x,y,z)) {
				k=n;
				break;
			}

			draw_bone++;
		}
		
		if (k==-1) return(FALSE);
	
			// select as current bone
		
		cur_bone=k;
		reset_bone_tab(cur_bone);
		
			// switch to pose and redraw model
			
		switch_tab_pose();
		reset_pose_tab(cur_pose,cur_bone);
		draw_model_wind_pose(&model,cur_mesh,cur_pose);

		return(TRUE);
	}
	
		// if no pose, then no handle dragging
		
	if (cur_pose==-1) {
		StandardAlert(0,"\pCan't Drag Bones","\pYou need to select a pose before you can drag any bones.",NULL,NULL);
		return(TRUE);
	}
	
		// get drag angle
		
	switch (drag_handle) {
	
		case drag_handle_x:
			ang=&model.poses[cur_pose].bone_moves[cur_bone].rot.x;
			mov=&model.poses[cur_pose].bone_moves[cur_bone].mov.x;
			break;
		
		case drag_handle_y:
			ang=&model.poses[cur_pose].bone_moves[cur_bone].rot.y;
			mov=&model.poses[cur_pose].bone_moves[cur_bone].mov.y;
			break;
			
		case drag_handle_z:
			ang=&model.poses[cur_pose].bone_moves[cur_bone].rot.z;
			mov=&model.poses[cur_pose].bone_moves[cur_bone].mov.z;
			break;
		
		default:
			return(TRUE);
			
	}

		// switch to pose tab
	
	switch_tab_pose();
	reset_pose_tab(cur_pose,cur_bone);
	
		// drag bone

	org_ang=*ang;
	org_mov=*mov;
	last_pt.h=last_pt.v=-1;
	
	undo_set_bone_move(cur_pose,cur_bone);
	
	SetCCursor(bone_drag_cursor);
		
	do {
		TrackMouseLocation(NULL,&pt,&track);
		model_wind_offset_click(&pt);
		
		if (memcmp(&last_pt,&pt,sizeof(Point))==0) continue;
	
		memmove(&last_pt,&pt,sizeof(Point));
		
		x=pt.h-start_pt.h;
		if (x<-180) x=-180;
		if (x>180) x=180;
		
		if (drag_bone_mode==drag_bone_mode_rotate) {
			*ang=org_ang+(((float)x)/2.0f);
		}
		else {
			*mov=org_mov+(((float)x)/20.0f);
		}
		
			// draw the model
			
		model_bone_drag_on=TRUE;
		draw_model_wind_pose(&model,cur_mesh,cur_pose);
		model_bone_drag_on=FALSE;
		
		redraw_pose_row();

	} while (track!=kMouseTrackingMouseReleased);

	InitCursor();

		// redraw model
		
	draw_model_wind_pose(&model,cur_mesh,cur_pose);

	return(TRUE);
}
	
/* =======================================================

      Model Window Clicks
      
======================================================= */

void click_model_wind(Point pt,unsigned long modifiers)
{
	model_wind_reset_modifiers();
	
	if ((shift_on) || (rotate_on) || (size_on)) {
		change_model_wind(pt);
		return;
	}
	
	if ((draw_type==dt_bones) || (draw_type==dt_model_bones) || (draw_type==dt_mesh_bones)) {
		if (drag_bone_model_wind(pt)) return;
	}
	
	select_model_wind(pt,modifiers);
}

