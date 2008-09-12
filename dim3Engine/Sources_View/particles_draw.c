/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Particle Drawing Routines

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
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "lights.h"
#include "effects.h"
#include "video.h"

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;

extern void map_calculate_light_color_normal(double x,double y,double z,float *cf,float *nf);
extern void view_next_vertex_object(void);
extern void view_bind_current_vertex_object(void);
extern void view_unbind_current_vertex_object(void);

/* =======================================================

      Get Particle Connected Model Draw
      
======================================================= */

model_draw* particle_draw_get_model(particle_motion *motion)
{
	obj_type				*obj;
	weapon_type				*weap;
	proj_type				*proj;

		// is it a projectile?

	if (motion->proj_uid!=-1) {
		proj=projectile_find_uid(motion->proj_uid);
		if (proj==NULL) return(NULL);
		return(&proj->draw);
	}

		// is it a weapon?

	if (motion->weap_uid!=-1) {
		weap=weapon_find_uid(motion->weap_uid);
		if (weap==NULL) return(NULL);
		return(&weap->draw);
	}

		// is object

	obj=object_find_uid(motion->obj_uid);
	if (obj==NULL) return(NULL);
	return(&obj->draw);
}

/* =======================================================

      Get Particle Position
      
======================================================= */

void particle_draw_position(effect_type *effect,int count,int *x,int *y,int *z)
{
	int						mx,my,mz;
	float					f_tick;
	model_draw				*draw;
	particle_effect_data	*eff_particle;

		// position
	
	mx=effect->pnt.x;
	my=effect->pnt.y;
	mz=effect->pnt.z;

	eff_particle=&effect->data.particle;

		// sticking to bone?

	if (eff_particle->motion.bone_idx!=-1) {
		draw=particle_draw_get_model(&eff_particle->motion);
		if (draw!=NULL) model_find_bone_position_for_current_animation(draw,eff_particle->motion.bone_idx,&mx,&my,&mz);
	}

		// motion

	else {
		f_tick=((float)count)/10.0f;
		mx+=(int)(eff_particle->motion.vct.x*f_tick);
		my+=(int)(eff_particle->motion.vct.y*f_tick);
		mz+=(int)(eff_particle->motion.vct.z*f_tick);
	}

	*x=mx;
	*y=my;
	*z=mz;
}

/* =======================================================

      Fill Particle Quad Array
      
======================================================= */

int particle_fill_array_quad_single(float *vertex_ptr,int idx,int nvertex,int mx,int my,int mz,d3ang *rot_ang,float pixel_size,matrix_type *mat_x,matrix_type *mat_y,float gravity,int count,int particle_count,particle_piece_type *pps,float gx,float gy,float g_size)
{
	int					n,k;
	float				fx,fy,fz,f_count,px[4],py[4],pz[4];
	float				*pv,*pt;
	matrix_type			rot_x_mat,rot_z_mat,rot_y_mat;
	
		// if move angle, setup matrixes
		
	if (rot_ang!=NULL) {
		matrix_rotate_x(&rot_x_mat,rot_ang->x);
		matrix_rotate_z(&rot_z_mat,rot_ang->z);
		matrix_rotate_y(&rot_y_mat,rot_ang->y);
	}
	
		// get particle box
		
	px[0]=px[3]=-pixel_size;
	px[1]=px[2]=pixel_size;
	py[0]=py[1]=-pixel_size;
	py[2]=py[3]=pixel_size;
	pz[0]=pz[1]=pz[2]=pz[3]=0.0f;
	
	for (k=0;k!=4;k++) {
		matrix_vertex_multiply(mat_x,&px[k],&py[k],&pz[k]);
		matrix_vertex_multiply(mat_y,&px[k],&py[k],&pz[k]);
	}
	
		// fill particle arrays
	
	pv=vertex_ptr+(idx*3);
	pt=vertex_ptr+(nvertex*3)+(idx*2);
	
	f_count=(float)count;
	
	for (n=0;n!=particle_count;n++) {
	
			// particle quad
			
		if (rot_ang!=NULL) {			// rotated particles
			fx=pps->vct.x;
			fy=pps->vct.y;
			fz=pps->vct.z;
			
			matrix_vertex_multiply(&rot_x_mat,&fx,&fy,&fz);
			matrix_vertex_multiply(&rot_z_mat,&fx,&fy,&fz);
			matrix_vertex_multiply(&rot_y_mat,&fx,&fy,&fz);
		
			fx=(float)(mx+pps->pt.x)+(fx*f_count);
			fy=(float)(my+pps->pt.y)+((fy*f_count)+gravity);
			fz=(float)(mz+pps->pt.z)+(fz*f_count);
		}
		else {							// normal particles
			fx=(float)(mx+pps->pt.x)+((pps->vct.x)*f_count);
			fy=(float)(my+pps->pt.y)+(((pps->vct.y)*f_count)+gravity);
			fz=(float)(mz+pps->pt.z)+((pps->vct.z)*f_count);
		}
		
		pps++;

		*pv++=(px[0]+fx);
		*pv++=(py[0]+fy);
		*pv++=-(fz+pz[0]);
		
		*pt++=gx;
		*pt++=gy;

		*pv++=(px[1]+fx);
		*pv++=(py[1]+fy);
		*pv++=-(fz+pz[1]);
		
		*pt++=gx+g_size;
		*pt++=gy;

		*pv++=(px[2]+fx);
		*pv++=(py[2]+fy);
		*pv++=-(fz+pz[2]);
		
		*pt++=gx+g_size;
		*pt++=gy+g_size;

		*pv++=(px[3]+fx);
		*pv++=(py[3]+fy);
		*pv++=-(fz+pz[3]);
		
		*pt++=gx;
		*pt++=gy+g_size;

			// change particle image
		
		if (g_size==1) continue;
		
		if (gx<1) {
			gx+=g_size;
			continue;
		}

		gx=0;
		gy+=g_size;
		if (gy>=1) gy=0;
	}
	
	return(idx+(particle_count*4));
}

/* =======================================================

      Draw Particles
      
======================================================= */

void particle_draw(effect_type *effect,int count)
{
	int						i,idx,particle_count,nvertex,
							ntrail,trail_step,mx,mz,my,
							pixel_dif,sz;
	float					gravity,gx,gy,g_size,pixel_sz,f,pc[3],pn[3],
							alpha,alpha_dif,r,g,b,color_dif,f_count,f_tick;
	float					*vertex_ptr;
	d3ang					*rot_ang,rang;
	d3col					ambient_col;
	particle_type			*particle;
	particle_effect_data	*eff_particle;
	matrix_type				pixel_mat_x,pixel_mat_y;

	eff_particle=&effect->data.particle;
	particle=&server.particles[eff_particle->particle_idx];

	f_count=(float)count;

		// position
	
	particle_draw_position(effect,count,&mx,&my,&mz);

	if (particle->ambient_factor!=1.0f) {		// get ambient before position change
		map_calculate_light_reduce_effect(effect);
		map_calculate_light_color_normal((double)mx,(double)my,(double)mz,pc,pn);
		ambient_col.r=pc[0];
		ambient_col.g=pc[1];
		ambient_col.b=pc[2];
	}

	mx-=view.camera.pnt.x;
	my-=view.camera.pnt.y;
	mz-=view.camera.pnt.z;
	
		// particle move rotation
		// we can have rotations from being attached to a bone of a model
		// or the timed rotations from the particle setup
		// we only want to set these up if the rotation is necessary as it
		// saves time over thousands of particles
		
	rot_ang=NULL;

	if (eff_particle->rot.on) {
		rang.x=eff_particle->rot.ang.x;
		rang.y=eff_particle->rot.ang.y;
		rang.z=eff_particle->rot.ang.z;
		rot_ang=&rang;
	}

	if ((particle->rot.x!=0.0f) || (particle->rot.y!=0.0f) || (particle->rot.z!=0.0f)) {

		if (rot_ang==NULL) {
			rang.x=rang.y=rang.z=0.0f;
			rot_ang=&rang;
		}

		f=f_count*particle->rot.x;
		f+=(f*particle->rot_accel.x);
		rot_ang->x=angle_add(rot_ang->x,f);

		f=f_count*particle->rot.y;
		f+=(f*particle->rot_accel.y);
		rot_ang->y=angle_add(rot_ang->y,f);

		f=f_count*particle->rot.z;
		f+=(f*particle->rot_accel.z);
		rot_ang->z=angle_add(rot_ang->z,f);
	}

		// particle sprite rotation

	matrix_rotate_x(&pixel_mat_x,view.camera.ang.x);
	matrix_rotate_y(&pixel_mat_y,view.camera.ang.y);
	
		// reverse
		
	if (particle->reverse) {
		count=(effect->life_tick-count);
	}

		// setup alpha
		
	f_tick=(float)effect->life_tick;
	
	alpha_dif=particle->end_alpha-particle->start_alpha;
    alpha=particle->start_alpha+((alpha_dif*f_count)/f_tick);
	
		// setup size
		
	pixel_dif=particle->end_pixel_size-particle->start_pixel_size;
    pixel_sz=(float)(particle->start_pixel_size+((pixel_dif*count)/effect->life_tick));
	
		// setup color
		
	color_dif=particle->end_color.r-particle->start_color.r;
    r=particle->start_color.r+((color_dif*f_count)/f_tick);
	color_dif=particle->end_color.g-particle->start_color.g;
    g=particle->start_color.g+((color_dif*f_count)/f_tick);
	color_dif=particle->end_color.b-particle->start_color.b;
    b=particle->start_color.b+((color_dif*f_count)/f_tick);

	if (particle->ambient_factor!=1.0f) {
		r=(r*particle->ambient_factor)+((1.0f-particle->ambient_factor)*ambient_col.r);
		if (r>1.0f) r=1.0f;

		g=(g*particle->ambient_factor)+((1.0f-particle->ambient_factor)*ambient_col.g);
		if (g>1.0f) g=1.0f;

		b=(b*particle->ambient_factor)+((1.0f-particle->ambient_factor)*ambient_col.b);
		if (b>1.0f) b=1.0f;
	}
	
		// setup images
		
	effect_image_animate_get_uv(count,&particle->animate,&gx,&gy,&g_size);

		// reduce x/z/y movement and add in offset
		
	count=count/10;

		// get next VBO to use

	view_next_vertex_object();

		// map VBO to memory

	view_bind_current_vertex_object();

	nvertex=(particle->count*(particle->trail_count+1))*4;

	sz=(nvertex*(3+2))*sizeof(float);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,NULL,GL_STREAM_DRAW_ARB);

	vertex_ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	if (vertex_ptr==NULL) {
		view_unbind_current_vertex_object();
		return;
	}
	
		// setup the arrays

	particle_count=particle->count;
	ntrail=particle->trail_count+1;
	trail_step=particle->trail_step;
		
	idx=0;
			
	for (i=0;i!=ntrail;i++) {

			// get gravity

		gravity=particle_get_gravity(particle,count);

			// draw pixels

		idx=particle_fill_array_quad_single(vertex_ptr,idx,nvertex,mx,my,mz,rot_ang,pixel_sz,&pixel_mat_x,&pixel_mat_y,gravity,count,particle_count,particle->pieces[eff_particle->variation_idx],gx,gy,g_size);

			// reduce pixel sizes and counts for trails
			
		pixel_sz=pixel_sz*particle->reduce_pixel_fact;
		count-=trail_step;
		if ((count<0) || (pixel_sz<=0)) break;
	}

		// unmap vertex object

	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
	
		// draw arrays
		
	gl_texture_simple_start();
	gl_texture_simple_set(view_images_get_gl_id(particle->image_idx),FALSE,r,g,b,alpha);

	glEnable(GL_BLEND);

	if (particle->blend_add) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);			// don't let alpha z's interfere with each other

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)((nvertex*3)*sizeof(float)));

	glDrawArrays(GL_QUADS,0,idx);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glDepthMask(GL_TRUE);
	
	gl_texture_simple_end();

		// unbind vertex object
		
	view_unbind_current_vertex_object();
}

