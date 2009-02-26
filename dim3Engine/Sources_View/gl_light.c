/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: OpenGL Lights

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

#include "weapons.h"
#include "models.h"
#include "lights.h"
#include "cameras.h"
#include "consoles.h"
#include "video.h"

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;

// supergumba -- can redo a lot of what's in light_spot_type

int							gl_light_spots_count;
light_spot_type				gl_light_spots[max_light_spot];

extern double light_get_intensity(int light_type,int intensity);		// supergumba -- move this

/* =======================================================

      Compile Light List
      
======================================================= */

void gl_lights_compile_add(d3pnt *pnt,int light_type,int intensity,d3col *col)
{
	light_spot_type			*lspot;
	
	if (gl_light_spots_count==max_light_spot) return;

	lspot=&gl_light_spots[gl_light_spots_count];
	
		// create intensity for light type
		
	lspot->intensity=light_get_intensity(light_type,intensity);
	if (lspot->intensity<=0) return;
	
		// create light position and color
		
	memmove(&lspot->pnt,pnt,sizeof(d3pnt));
	memmove(&lspot->col,col,sizeof(d3col));

	gl_light_spots_count++;
}

void gl_lights_compile_model_add(model_draw *draw)
{
	int					n;
	d3pnt				pnt;
	model_type			*mdl;
	model_draw_light	*light;

		// any model?
		
	mdl=NULL;
	if ((draw->uid!=-1) && (draw->on)) mdl=model_find_uid(draw->uid);
	
		// add lights
		
	light=draw->lights;
	
	for (n=0;n!=max_model_light;n++) {

		if (light->on) {
			memmove(&pnt,&draw->pnt,sizeof(d3pnt));
			
			if (mdl!=NULL) {
				model_get_light_position(mdl,&draw->setup,n,&pnt.x,&pnt.y,&pnt.z);
				if (draw->no_rot.on) gl_project_fix_rotation(&view.camera,console_y_offset(),&pnt.x,&pnt.y,&pnt.z);
			}
			
			gl_lights_compile_add(&pnt,light->type,light->intensity,&light->col);
		}

		light++;
	}
}

void gl_lights_compile_effect_add(int tick,effect_type *effect)
{
	int					intensity,count,mid_tick,fade_tick;
	flash_effect_data	*flash;
	
	if (effect->effecttype!=ef_flash) return;

		// calculate flash
		
	flash=&effect->data.flash;

	intensity=flash->intensity;
	mid_tick=flash->mid_tick;
	
	count=tick-effect->start_tick;

	if (count<mid_tick) {										// start of flash
		if (mid_tick>0) intensity=(intensity*count)/mid_tick;
	}
	else {
		if (count>mid_tick) {									// fade of flash
			fade_tick=(effect->life_tick-flash->mid_tick);
			count=fade_tick-(count-mid_tick);
	
			if (fade_tick>0) intensity=(intensity*count)/fade_tick;
		}
	}
	
	gl_lights_compile_add(&effect->pnt,lt_normal,intensity,&flash->col);
}

void gl_lights_compile(int tick)
{
	int					n;
	map_light_type		*maplight;
	obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	effect_type			*effect;

		// map lights
		
	maplight=map.lights;
		
	for (n=0;n!=map.nlight;n++) {
		if (maplight->on) gl_lights_compile_add(&maplight->pnt,maplight->type,maplight->intensity,&maplight->col);
		maplight++;
	}	

		// lights from objects and their weapons
	
	obj=server.objs;
	
	for (n=0;n!=server.count.obj;n++) {
		gl_lights_compile_model_add(&obj->draw);
		if (obj->held_weapon.current_uid!=-1) {
			weap=weapon_find_uid(obj->held_weapon.current_uid);
			if (weap!=NULL) gl_lights_compile_model_add(&weap->draw);
		}
		obj++;
	}
	
		// lights from projectiles

	proj=server.projs;
	
	for (n=0;n!=server.count.proj;n++) {
		gl_lights_compile_model_add(&proj->draw);
		proj++;
	}
	
		// lights from effects
		
	effect=server.effects;
		
	for (n=0;n!=server.count.effect;n++) {
		gl_lights_compile_effect_add(tick,effect);		
		effect++;
	}
}

/* =======================================================

      Lights
      
======================================================= */

void gl_lights_start(void)
{
	GLfloat				glf[4];	

	glEnable(GL_LIGHTING);
	
		// enable colored materials for dark factor

	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

		// ambient lighting

	glf[0]=map.ambient.light_color.r+setup.gamma;
	glf[1]=map.ambient.light_color.g+setup.gamma;
	glf[2]=map.ambient.light_color.b+setup.gamma;
	glf[3]=1.0f;
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,glf);
}

void gl_lights_end(void)
{
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);
}


// supergumba -- use all lights version
// we will need to clean this up as it's way to slow
// this version does not consider the reduced list and is a lot slower

void gl_lights_build_from_reduced_light_list(d3pnt *pnt,bool *light_on)
{
	int						n,k,light_id,
							idx,cnt,sort_list[max_light_spot];
	float					f;
	double					d,dx,dy,dz,sort_dist[max_light_spot];
	light_spot_type			*lspot;
	GLfloat					glf[4];

		// sort the light list

	cnt=0;

	for (n=0;n!=gl_light_spots_count;n++) {
		lspot=&gl_light_spots[n];

			// get distance

		dx=(double)(lspot->pnt.x-pnt->x);
		dy=(double)(lspot->pnt.y-pnt->y);
		dz=(double)(lspot->pnt.z-pnt->z);

		d=sqrt((dx*dx)+(dy*dy)+(dz*dz));
		if (d>(double)lspot->intensity) continue;

			// find position in list (top is closest)

		idx=-1;
	
		for (k=0;k!=cnt;k++) {
			if (sort_dist[k]>d) {
				idx=k;
				break;
			}
		}
	
			// insert at end of list
			
		if (idx==-1) {
			sort_dist[cnt]=d;
			sort_list[cnt]=n;
			cnt++;
			continue;
		}
		
			// insert in list
			
		memmove(&sort_dist[idx+1],&sort_dist[idx],(sizeof(double)*(cnt-idx)));
		memmove(&sort_list[idx+1],&sort_list[idx],(sizeof(int)*(cnt-idx)));
		
		sort_dist[idx]=d;
		sort_list[idx]=n;
		
		cnt++;
	}
	
		// only use max_view_lights_per_poly lights per polygon
		
	for (n=0;n!=max_view_lights_per_poly;n++) {
		lspot=&gl_light_spots[sort_list[n]];
		
		light_id=GL_LIGHT0+n;
		
			// null lights
			
		if (n>=cnt) {
			glDisable(light_id);
			
			glLightf(light_id,GL_CONSTANT_ATTENUATION,0.0f);
			glLightf(light_id,GL_LINEAR_ATTENUATION,0.0f);
			glLightf(light_id,GL_QUADRATIC_ATTENUATION,0.0f);

			light_on[n]=FALSE;
		}
		
			// regular lights
			
		else {
			glEnable(light_id);
			
			glf[0]=(float)lspot->pnt.x;
			glf[1]=(float)lspot->pnt.y;
			glf[2]=(float)lspot->pnt.z;
			glf[3]=1.0f;
			glLightfv(light_id,GL_POSITION,glf);

			glLightf(light_id,GL_CONSTANT_ATTENUATION,0.0f);
			glLightf(light_id,GL_LINEAR_ATTENUATION,0.0f);

			f=(float)lspot->intensity;
			f=f*0.4f;
			f=f*f;
			glLightf(light_id,GL_QUADRATIC_ATTENUATION,(1.0f/f));

			glf[0]=lspot->col.r;
			glf[1]=lspot->col.g;
			glf[2]=lspot->col.b;
			glf[3]=1.0f;
			glLightfv(light_id,GL_AMBIENT,glf);
			glLightfv(light_id,GL_DIFFUSE,glf);
			glLightfv(light_id,GL_SPECULAR,glf);

			light_on[n]=TRUE;
		}
	}
}

