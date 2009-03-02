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
view_light_spot_type		gl_light_spots[max_light_spot];

extern double light_get_intensity(int light_type,int intensity);		// supergumba -- move this

/* =======================================================

      Compile Light List
      
======================================================= */

// supergumba -- max light spot will need to chance as we are gathering all lights in the map,
// we might need to instead do some kind of mass elimination at the beginning

void gl_lights_compile_add(d3pnt *pnt,int light_type,int intensity,float exponent,d3col *col)
{
	view_light_spot_type			*lspot;
	
	if (gl_light_spots_count==max_light_spot) return;

	lspot=&gl_light_spots[gl_light_spots_count];
	
		// create intensity for light type
		
	lspot->intensity=light_get_intensity(light_type,intensity);
	if (lspot->intensity<=0) return;
	
	lspot->exponent=exponent;
	
		// create light position and color
		
	memmove(&lspot->pnt,pnt,sizeof(d3pnt));
	memmove(&lspot->col,col,sizeof(d3col));

		// non shader calculation speed ups

	lspot->d_intensity=lspot->intensity*lspot->intensity;
	lspot->d_inv_intensity=1.0/lspot->d_intensity;
		
	lspot->d_x=(double)lspot->pnt.x;
	lspot->d_y=(double)lspot->pnt.y;
	lspot->d_z=(double)lspot->pnt.z;
	
	lspot->d_col_r=(double)lspot->col.r;
	lspot->d_col_g=(double)lspot->col.g;
	lspot->d_col_b=(double)lspot->col.b;

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
			
			gl_lights_compile_add(&pnt,light->type,light->intensity,light->exponent,&light->col);
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
	
	gl_lights_compile_add(&effect->pnt,lt_normal,intensity,1.0f,&flash->col);
}

void gl_lights_compile(int tick)
{
	int					n;
	map_light_type		*maplight;
	obj_type			*obj;
	weapon_type			*weap;
	proj_type			*proj;
	effect_type			*effect;
	
	gl_light_spots_count=0;

		// map lights
		
	maplight=map.lights;
		
	for (n=0;n!=map.nlight;n++) {
		if (maplight->on) gl_lights_compile_add(&maplight->pnt,maplight->type,maplight->intensity,maplight->exponent,&maplight->col);
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

      Per Vertex non GLSL Lighting
      
======================================================= */

void gl_lights_calc_vertex(double x,double y,double z,float *cf)
{
	int						n;
	double					dx,dz,dy,r,g,b,d,mult;
	view_light_spot_type	*lspot;

		// no lights in scene

	if (gl_light_spots_count==0) {
		*cf++=(map.ambient.light_color.r+setup.gamma);
		*cf++=(map.ambient.light_color.g+setup.gamma);
		*cf=(map.ambient.light_color.b+setup.gamma);
		return;
	}

		// combine all light spots attenuated for distance
		
	r=g=b=0.0;
	
	lspot=gl_light_spots;

	for (n=0;n!=gl_light_spots_count;n++) {

		dx=lspot->d_x-x;
		dy=lspot->d_y-y;
		dz=lspot->d_z-z;
		
		d=(dx*dx)+(dz*dz)+(dy*dy);

		if (d<=lspot->d_intensity) {
			mult=(lspot->d_intensity-d)*lspot->d_inv_intensity;

			r+=(lspot->d_col_r*mult);
			g+=(lspot->d_col_g*mult);
			b+=(lspot->d_col_b*mult);
		}
		
		lspot++;
	}

		// supergumba -- do exponent here

		// set light value

	*cf++=(map.ambient.light_color.r+setup.gamma)+(float)r;
	*cf++=(map.ambient.light_color.g+setup.gamma)+(float)g;
	*cf=(map.ambient.light_color.b+setup.gamma)+(float)b;
}

view_light_spot_type* gl_light_find_closest_light(double x,double y,double z,int *p_dist)
{
	int					n,k;
	double				dx,dz,dy,d,dist;
	view_light_spot_type		*lspot;

		// no lights in scene

	if (gl_light_spots_count==0) return(NULL);

		// find closest light
	
	k=-1;
	dist=-1;
	
	for (n=0;n!=gl_light_spots_count;n++) {

		lspot=&gl_light_spots[n];
		
			// get distance to light spot
			
		dx=lspot->d_x-x;
		dz=lspot->d_z-z;
		dy=lspot->d_y-y;

		d=(dx*dx)+(dz*dz)+(dy*dy);
		
			// reject lights outside globe
			
		if (d<=lspot->d_intensity) {
		
				// compare distances
		
			if ((d<dist) || (dist==-1)) {
				dist=d;
				k=n;
			}
		}
		
		lspot++;
	}
	
	if (k==-1) return(NULL);
	
	*p_dist=(int)sqrt(dist);

	return(&gl_light_spots[k]);
}

/* =======================================================

      GLSL Lights
      
======================================================= */


// supergumba -- use all lights version
// we will need to clean this up as it's way to slow
// this version does not consider the reduced list and is a lot slower

void gl_lights_build_from_box(d3pnt *mid,d3pnt *min,d3pnt *max,view_glsl_light_list_type *light_list)
{
	int						n,k,i_add,
							idx,cnt,sort_list[max_light_spot];
	double					d,dx,dy,dz,sort_dist[max_light_spot];
	view_light_spot_type	*lspot;

		// sort the light list

	cnt=0;

	for (n=0;n!=gl_light_spots_count;n++) {
		lspot=&gl_light_spots[n];
		
			// does light hit this polygon?

		i_add=(int)lspot->intensity;
			
		if ((lspot->pnt.x+lspot->intensity)<min->x) continue;
		if ((lspot->pnt.x-lspot->intensity)>max->x) continue;
		if ((lspot->pnt.z+lspot->intensity)<min->z) continue;
		if ((lspot->pnt.z-lspot->intensity)>max->z) continue;
		if ((lspot->pnt.y+lspot->intensity)<min->y) continue;
		if ((lspot->pnt.y-lspot->intensity)>max->y) continue;

			// get distance

		dx=(double)(lspot->pnt.x-mid->x);
		dy=(double)(lspot->pnt.y-mid->y);
		dz=(double)(lspot->pnt.z-mid->z);

		d=sqrt((dx*dx)+(dy*dy)+(dz*dz));

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
		
			// null lights
			
		if (n>=cnt) {
			idx=n*3;
			light_list->pos[idx]=0.0f;
			light_list->pos[idx+1]=0.0f;
			light_list->pos[idx+2]=0.0f;
			
			light_list->col[idx]=0.0f;
			light_list->col[idx+1]=0.0f;
			light_list->col[idx+2]=0.0f;
			
			light_list->intensity[n]=0.0f;		// will effectively make this a light that never effects output
			light_list->exponent[n]=1.0f;
		}
		
			// regular lights
			
		else {
			idx=n*3;
			
			light_list->pos[idx]=(float)lspot->pnt.x;
			light_list->pos[idx+1]=(float)lspot->pnt.y;
			light_list->pos[idx+2]=(float)lspot->pnt.z;
			
			light_list->col[idx]=(float)lspot->col.r;
			light_list->col[idx+1]=(float)lspot->col.g;
			light_list->col[idx+2]=(float)lspot->col.b;
			
			light_list->intensity[n]=(float)lspot->intensity;
			light_list->exponent[n]=lspot->exponent;
		}
	}
}

inline void gl_lights_build_from_poly(map_mesh_poly_type *poly,view_glsl_light_list_type *light_list)
{
	gl_lights_build_from_box(&poly->box.mid,&poly->box.min,&poly->box.max,light_list);
}

void gl_lights_build_from_liquid(map_liquid_type *liq,view_glsl_light_list_type *light_list)
{
	d3pnt			mid,min,max;
	
	mid.x=(liq->lft+liq->rgt)>>1;
	mid.y=liq->y;
	mid.z=(liq->top+liq->bot)>>1;
	
	min.x=liq->lft;
	min.y=liq->y;
	min.z=liq->top;
	
	max.x=liq->rgt;
	max.y=liq->y;
	max.z=liq->bot;
	
	gl_lights_build_from_box(&mid,&min,&max,light_list);
}

void gl_lights_build_from_model(model_draw *draw,view_glsl_light_list_type *light_list)
{
	int					cx,cy,cz,sz;
	float				fx,fy,fz;
	d3pnt				pnt,min,max;
	matrix_type			mat;
	
		// need to move model if no rot on

	memmove(&pnt,&draw->pnt,sizeof(d3pnt));
		
	if (draw->no_rot.on) {
		matrix_rotate_y(&mat,draw->no_rot.ang.y);

		fx=(float)(pnt.x-draw->no_rot.center.x);
		fy=(float)(pnt.y-draw->no_rot.center.y);
		fz=(float)(pnt.z-draw->no_rot.center.z);
		
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
		
		pnt.x=((int)fx)+draw->no_rot.center.x;
		pnt.y=((int)fy)+draw->no_rot.center.y;
		pnt.z=((int)fz)+draw->no_rot.center.z;
	}

		// get model bounds

	sz=draw->size.x>>1;
	min.x=pnt.x-sz;
	max.x=pnt.x+sz;

	sz=draw->size.z>>1;
	min.z=pnt.z-sz;
	max.z=pnt.z+sz;

	min.y=pnt.y-draw->size.y;
	max.y=pnt.y;

		// any rotations

	cx=pnt.x+draw->center.x;
	cy=pnt.y+draw->center.y;
	cz=pnt.z+draw->center.z;

	rotate_point(&min.x,&min.y,&min.z,cx,cy,cz,draw->rot.x,draw->rot.y,draw->rot.z);
	rotate_point(&max.x,&max.y,&max.z,cx,cy,cz,draw->rot.x,draw->rot.y,draw->rot.z);

	gl_lights_build_from_box(&pnt,&min,&max,light_list);
}



