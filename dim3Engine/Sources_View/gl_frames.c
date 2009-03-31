/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Routines

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

extern hud_type				hud;
extern setup_type			setup;
extern render_info_type		render_info;
extern server_type			server;
extern map_type				map;
extern view_type			view;

int							vport[4];
double						mod_matrix[16],proj_matrix[16],
							fix_rot_camera_x,fix_rot_camera_y,fix_rot_camera_z;

/* =======================================================

      Start and End a Drawing Session
      
======================================================= */

void gl_frame_start(d3col *col)
{
	if (col==NULL) {
		glClearColor(0.0f,0.0f,0.0f,0.0f);
	}
	else {
		glClearColor(col->r,col->g,col->b,0.0f);
	}

	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}

void gl_frame_end(void)
{
	SDL_GL_SwapBuffers();
}

/* =======================================================

      3D Projections
      
======================================================= */

void gl_3D_view(view_camera_type *camera)
{
	float			ratio;

		// setup projections

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	switch (camera->projection_type) {

		case cp_fov:
			ratio=(((float)setup.screen.x_sz)/((float)setup.screen.y_sz))*camera->aspect_ratio;
			gluPerspective(camera->fov,ratio,camera->near_z,camera->far_z);
			glScalef(-1.0f,-1.0f,-1.0f);
			break;

		case cp_frustum:
			glFrustum(camera->lft,camera->rgt,camera->top,camera->bot,camera->near_z,camera->far_z);
			glScalef(-1.0f,-1.0f,-1.0f);
			break;
	}

		// default rotations

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(view.render->camera.pnt.x,view.render->camera.pnt.y,(view.render->camera.pnt.z+view.camera.near_z),view.render->camera.pnt.x,view.render->camera.pnt.y,view.render->camera.pnt.z,0.0f,1.0f,0.0f);

	glTranslatef(0.0f,0.0f,(float)view.camera.near_z_offset);
}

void gl_3D_rotate(d3pnt *pnt,d3ang *ang)
{
	float		fx,fz,fy,f_temp;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	angle_get_movement_float(ang->y,(float)view.camera.near_z,&fx,&fz);
	angle_get_movement_float(-ang->x,(float)view.camera.near_z,&fy,&f_temp);

	if (pnt==NULL) {
		gluLookAt(fx,fy,fz,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f);
	}
	else {
		gluLookAt((pnt->x+fx),(pnt->y+fy),(pnt->z+fz),pnt->x,pnt->y,pnt->z,0.0f,1.0f,0.0f);
	}

	glTranslatef(0.0f,0.0f,(float)view.camera.near_z_offset);
}

/* =======================================================

      2D Projections
      
======================================================= */

void gl_2D_view_screen(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,(GLdouble)setup.screen.x_sz,(GLdouble)setup.screen.y_sz,0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void gl_2D_view_interface(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,(GLdouble)hud.scale_x,(GLdouble)hud.scale_y,0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/* =======================================================

      Setup ViewPort
      
======================================================= */

void gl_setup_viewport(int y_off)
{
	glViewport(render_info.view_x,(render_info.view_y+y_off),setup.screen.x_sz,(setup.screen.y_sz-y_off));
}

/* =======================================================

      Projection Points
      
======================================================= */

inline void gl_setup_project(void)
{
	glGetDoublev(GL_MODELVIEW_MATRIX,mod_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,proj_matrix);
	glGetIntegerv(GL_VIEWPORT,(GLint*)vport);
}

inline bool gl_project_in_view_z(int x,int y,int z)
{
	return(((((double)x)*mod_matrix[2])+(((double)y)*mod_matrix[6])+(((double)z)*mod_matrix[10])+mod_matrix[14])>0.0);
}

inline void gl_project_point(int *x,int *y,int *z)
{
	double		dx,dy,dz;

	gluProject(*x,*y,*z,mod_matrix,proj_matrix,(GLint*)vport,&dx,&dy,&dz);
	*x=((int)dx)-render_info.view_x;
	*y=((int)dy)-render_info.view_y;
	*z=(int)dz;
}

inline void gl_project_poly(int ptsz,int *x,int *y,int *z)
{
	int			i;

	for (i=0;i<ptsz;i++) {
		gl_project_point(&x[i],&y[i],&z[i]);
	}
}

inline float gl_project_get_depth(int x,int y,int z)
{
	double		dx,dy,dz;

	gluProject(x,y,z,mod_matrix,proj_matrix,(GLint*)vport,&dx,&dy,&dz);
	return((float)dz);
}

inline void gl_unproject_point(float fx,float fy,float fz,int *x,int *y,int *z)
{
	double		dx,dy,dz;
	
	gluUnProject(fx,fy,fz,mod_matrix,proj_matrix,(GLint*)vport,&dx,&dy,&dz);
	*x=((int)dx)+render_info.view_x;
	*y=((int)dy)+render_info.view_y;
	*z=(int)dz;
}

void gl_project_fix_rotation(view_camera_type *camera,int y_off,int *x,int *y,int *z)
{
	double			dx,dy,dz,dx2,dy2,dz2;

	gl_setup_viewport(y_off);

		// remember current settings

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

		// translate from non-rotated 3D space
		// to rotated 3D space
		
	dx=(double)(*x);
	dy=(double)(*y);
	dz=(double)(*z);

	gl_3D_view(&view.camera);
	gl_setup_project();
	gluProject(dx,dy,dz,mod_matrix,proj_matrix,(GLint*)vport,&dx2,&dy2,&dz2);

	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
	gluUnProject(dx2,dy2,dz2,mod_matrix,proj_matrix,(GLint*)vport,&dx,&dy,&dz);

	*x=((int)dx);
	*y=((int)dy);
	*z=((int)dz);

		// restore settings

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	gl_setup_project();
}

