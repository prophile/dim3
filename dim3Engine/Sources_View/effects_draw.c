/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Effect Drawing

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

#include "effects.h"
#include "consoles.h"
#include "video.h"

float					line_zag[16]={1.0f,0.5f,0.8f,-0.2f,0.3f,-1.0f,-0.7f,0.1f,-0.4f,0.6f,-0.4f,0.5f,-0.6f,0.2f,-0.8f,-0.4f};

extern map_type			map;
extern server_type		server;
extern view_type		view;

extern bool effect_inview(effect_type *effect,int count);
extern int distance_to_view_center(int x,int y,int z);
extern bool view_mesh_in_draw_list(int mesh_idx);

/* =======================================================

      Draw Lightning
      
======================================================= */

void effect_draw_lightning_lines(int nline,float varient,int k,int sx,int sz,int sy,int ex,int ez,int ey,int xadd,int zadd,int yadd)
{
    int			i,x,z,y;
		
	glBegin(GL_LINES);
	
	for (i=1;i<(nline-1);i++) {
	
		x=(sx+xadd)+(int)(line_zag[k]*varient);
		y=(sy+yadd)+(int)(line_zag[(k+4)&0xF]*varient);
		z=(sz+zadd)+(int)(line_zag[(k+8)&0xF]*varient);
		
		glVertex3i(sx,sy,sz);
		glVertex3i(x,y,z);
		
		sx=x;
		sz=z;
		sy=y;
        
		k=(k+1)&0xF;
	}
	
	glVertex3i(sx,sy,sz);
	glVertex3i(ex,ey,ez);
	
	glEnd();
}

void effect_draw_lightning(int tick,effect_type *effect)
{
	int						k,nline,wid,x,z,y,
                            sx,sz,sy,ex,ez,ey,xadd,zadd,yadd;
	float					varient,alpha;
	d3col					*col;
	lightning_effect_data	*lightning;
	
	lightning=&effect->data.lightning;
	
        // get line start and end
        
	sx=lightning->start_pnt.x-view.camera.pnt.x;
	sy=lightning->start_pnt.y-view.camera.pnt.y;
	sz=view.camera.pnt.z-lightning->start_pnt.z;

	ex=lightning->end_pnt.x-view.camera.pnt.x;
	ey=lightning->end_pnt.y-view.camera.pnt.y;
	ez=view.camera.pnt.z-lightning->end_pnt.z;
	
        // find number of lines

    x=abs(ex-sx);
    z=abs(ez-sz);
    y=abs(ey-sy);
	nline=distance_get(x,z,y,0,0,0)/(map_enlarge<<1);
    
    if (nline<5) nline=4;
	if (nline>150) nline=150;
	
        // get line steps
        
	xadd=(ex-sx)/nline;
	zadd=(ez-sz)/nline;
	yadd=(ey-sy)/nline;
	
        // line varients
        
	k=((tick>>5)+lightning->line_offset)&0xF;
	varient=lightning->varient;
	
		// line colors
		
	col=&lightning->col;
    
        // draw lines
		
	wid=lightning->wid;
	alpha=0.6f;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
		
	glDisable(GL_LINE_SMOOTH);
	
	if (wid>2) {
		glLineWidth((float)wid);
		glColor4f(col->r,col->g,col->b,alpha);
		effect_draw_lightning_lines(nline,varient,k,sx,sz,sy,ex,ez,ey,xadd,zadd,yadd);
		alpha-=0.1f;
	}
	
	if (wid>1) {
		glLineWidth((float)((wid-1)/2));
		glColor4f(((col->r+1.0f)/2.0f),((col->g+1.0f)/2.0f),((col->b+1.0f)/2.0f),alpha);
		effect_draw_lightning_lines(nline,varient,k,sx,sz,sy,ex,ez,ey,xadd,zadd,yadd);
		alpha-=0.1f;
	}
	
	glLineWidth(1.0f);
	if (wid==1) {
		glColor4f(col->r,col->g,col->b,alpha);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,alpha);
	}
	effect_draw_lightning_lines(nline,varient,k,sx,sz,sy,ex,ez,ey,xadd,zadd,yadd);
    
	glEnable(GL_LINE_SMOOTH);
}

/* =======================================================

      Draw Ray
      
======================================================= */

void effect_draw_ray(effect_type *effect,int count)
{
	int						wid,sx,sz,sy,ex,ez,ey,life_tick;
	float					alpha;
	d3col					*col;
	ray_effect_data			*ray;
	
	ray=&effect->data.ray;
	
        // find line
        
	sx=ray->start_pnt.x-view.camera.pnt.x;
	sy=ray->start_pnt.y-view.camera.pnt.y;
	sz=view.camera.pnt.z-ray->start_pnt.z;

	ex=ray->end_pnt.x-view.camera.pnt.x;
	ey=ray->end_pnt.y-view.camera.pnt.y;
	ez=view.camera.pnt.z-ray->end_pnt.z;
	
		// build line for count
	
	life_tick=effect->life_tick>>1;

	if (count<life_tick) {
		ex=sx+(((ex-sx)*count)/life_tick);
		ey=sy+(((ey-sy)*count)/life_tick);
		ez=sz+(((ez-sz)*count)/life_tick);
	}
	else {
		count=effect->life_tick-count;
		sx=ex+(((sx-ex)*count)/life_tick);
		sy=ey+(((sy-ey)*count)/life_tick);
		sz=ez+(((sz-ez)*count)/life_tick);
	}

		// line colors
		
	col=&ray->col;
    
        // draw lines
		
	wid=ray->wid;
	alpha=0.6f;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
		
	glDisable(GL_LINE_SMOOTH);
        
	if (wid>2) {
		glLineWidth((float)wid);
		glColor4f(col->r,col->g,col->b,0.6f);
		glBegin(GL_LINES);
		glVertex3i(sx,sy,sz);
		glVertex3i(ex,ey,ez);
		glEnd();
		alpha-=0.1f;
	}
	
	if (wid>1) {
		glLineWidth((float)((wid-1)/2));
		glColor4f(((col->r+1.0f)/2.0f),((col->g+1.0f)/2.0f),((col->b+1.0f)/2.0f),0.5f);
		glBegin(GL_LINES);
		glVertex3i(sx,sy,sz);
		glVertex3i(ex,ey,ez);
		glEnd();
		alpha-=0.1f;
	}
	
	glLineWidth(1.0f);
	if (wid==1) {
		glColor4f(col->r,col->g,col->b,alpha);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,alpha);
	}
	glBegin(GL_LINES);
	glVertex3i(sx,sy,sz);
	glVertex3i(ex,ey,ez);
	glEnd();
	
	glEnable(GL_LINE_SMOOTH);
}

/* =======================================================

      Draw Globe
      
======================================================= */

void effect_draw_globe(effect_type *effect,int count)
{
	int						x,z,y,sz,life_tick;
	float					alpha;
	globe_effect_data		*globe;
	GLUquadricObj			*globe_quadric;
	
	globe=&effect->data.globe;
	
		// get size and alpha
		
	life_tick=effect->life_tick;
	
	sz=globe->end_size-globe->start_size;
	sz=((sz*count)/life_tick)+globe->start_size;

	alpha=globe->end_alpha-globe->start_alpha;
	alpha=((alpha*(float)count)/(float)life_tick)+globe->start_alpha;

	x=effect->pnt.x-view.camera.pnt.x;
	y=effect->pnt.y-view.camera.pnt.y;
	z=view.camera.pnt.z-effect->pnt.z;

		// draw globe

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
		
	globe_quadric=gluNewQuadric();
	gluQuadricNormals(globe_quadric,GLU_NONE);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef((float)x,(float)y,(float)z);
	
	glColor4f(globe->col.r,globe->col.g,globe->col.b,alpha);
	gluSphere(globe_quadric,sz,24,24);
	
	glPopMatrix();

	gluDeleteQuadric(globe_quadric);
}

/* =======================================================

      Get UV for animated Images
      
======================================================= */

void effect_image_animate_get_uv(int tick,image_animation_type *animate,float *gx,float *gy,float *g_size)
{
	int				k,nimage,image_per_row;
	float			gsz;
	
	nimage=animate->image_count;
	
	*gx=*gy=0.0f;
	*g_size=1.0f;
	
	if (nimage>1) {
		if (animate->msec==0) {
			k=0;
		}
		else {
			k=tick/animate->msec;
		}

		if ((!animate->loop) && (k>=nimage)) {
			k=nimage-1;
		}
		else {
			if (!animate->loop_back) {
				k=k%nimage;
			}
			else {
				k=k%((nimage<<1)-2);
				if (k>=nimage) k=(nimage+2)-k;
			}
		}

		image_per_row=animate->image_per_row;
		gsz=1.0f/(float)image_per_row;
		
		*gx=gsz*(float)(k%image_per_row);
		*gy=gsz*(float)(k/image_per_row);
		
		*g_size=gsz;
	}
}

/* =======================================================

      Effects Draw Mainline
      
======================================================= */

void effect_draw(int tick)
{
	int						i,k,t,dist,
							drawcnt,sz,count;
	effect_type				*effect;
	view_sort_effect_type	items[max_effect];
	
		// sort effects
		
	drawcnt=0;
	
	for (i=0;i!=server.count.effect;i++) {
		effect=&server.effects[i];

			// effect inside a mesh that's hidden?

		if (effect->mesh_idx!=-1) {
			if (!view_mesh_in_draw_list(effect->mesh_idx)) continue;
		}

			// effect in view
			
		count=tick-effect->start_tick;
		if (!effect_inview(effect,count)) continue;
		
			// get effect distance
			
		dist=distance_to_view_center(effect->pnt.x,effect->pnt.y,effect->pnt.z);
		
			// sort into list
		
		t=drawcnt;
		
		for (k=0;k!=drawcnt;k++) {
			if (dist>items[k].dist) {
				t=k;
				break;
			}
		}
		
			// add to list

		if (t>=drawcnt) {
			t=drawcnt;
		}
		else {
			sz=(drawcnt-t)*sizeof(view_sort_effect_type);
			memmove(&items[t+1],&items[t],sz);
		}
		
		items[t].idx=i;
		items[t].dist=dist;
		
		drawcnt++;
	}

	if (drawcnt==0) return;

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();
		
		// draw effects
		
	for (i=0;i!=drawcnt;i++) {
		effect=&server.effects[items[i].idx];
		
		count=tick-effect->start_tick;

		switch (effect->effecttype) {

			case ef_particle:
				particle_draw(effect,count);
				break;
				
			case ef_ring:
				ring_draw(effect,count);
				break;
				
			case ef_lightning:
				effect_draw_lightning(tick,effect);
				break;
				
			case ef_ray:
				effect_draw_ray(effect,count);
				break;
				
			case ef_globe:
				effect_draw_globe(effect,count);
				break;
				
		}
	}
}


