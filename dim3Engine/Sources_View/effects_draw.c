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
        
	sx=lightning->start_pnt.x;
	sy=lightning->start_pnt.y;
	sz=lightning->start_pnt.z;

	ex=lightning->end_pnt.x;
	ey=lightning->end_pnt.y;
	ez=lightning->end_pnt.z;
	
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
        
	sx=ray->start_pnt.x;
	sy=ray->start_pnt.y;
	sz=ray->start_pnt.z;

	ex=ray->end_pnt.x;
	ey=ray->end_pnt.y;
	ez=ray->end_pnt.z;
	
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
	int						n,k,t,dist,sz,count;
	effect_type				*effect;
	
		// sort effects
		
	view.render->effect_draw.count=0;
	
	for (n=0;n!=server.count.effect;n++) {
		effect=&server.effects[n];

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
		
		t=view.render->effect_draw.count;
		
		for (k=0;k!=view.render->effect_draw.count;k++) {
			if (dist>view.render->effect_draw.items[k].dist) {
				t=k;
				break;
			}
		}
		
			// add to list

		if (t>=view.render->effect_draw.count) {
			t=view.render->effect_draw.count;
		}
		else {
			sz=(view.render->effect_draw.count-t)*sizeof(view_render_draw_list_item_type);
			memmove(&view.render->effect_draw.items[t+1],&view.render->effect_draw.items[t],sz);
		}
		
		view.render->effect_draw.items[t].idx=(short)n;
		view.render->effect_draw.items[t].dist=dist;
		
		view.render->effect_draw.count++;
		if (view.render->effect_draw.count==max_view_render_item) break;
	}

	if (view.render->effect_draw.count==0) return;

		// setup view

	gl_3D_view();
	gl_3D_rotate(&view.render->camera.pnt,&view.render->camera.ang);
	gl_setup_project();
		
		// draw effects
		
	for (n=0;n!=view.render->effect_draw.count;n++) {
		effect=&server.effects[view.render->effect_draw.items[n].idx];
		
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
				
		}
	}
}


