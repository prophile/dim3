/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Particle Routines

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
#include "effects.h"
#include "cameras.h"

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern network_setup_type	net_setup;

/* =======================================================

      Initialize Particles
      
======================================================= */

void particle_initialize(void)
{
	server.count.particle=0;
}

/* =======================================================

      Setup Particles
      
======================================================= */

int particle_int(int iv)
{
	int			i;

	if (iv==0) return(0);

	i=random_int(iv);
	if (random_boolean()) return(-i);

	return(i);
}

float particle_float(float fv)
{
	float		f;

	if (fv==0.0f) return(0.0f);

	f=random_float(fv);
	if (random_boolean()) return(-f);

	return(f);
}

void particle_precalculate(particle_type *particle)
{
	int					n,k,count;
	particle_piece_type	*pps;

	count=particle->count;

	for (k=0;k!=max_particle_variation;k++) {

		pps=particle->pieces[k];

		for (n=0;n!=count;n++) {
	
				// start positions

			pps->pt.x=particle_int(particle->pt.x);
			pps->pt.y=particle_int(particle->pt.y);
			pps->pt.z=particle_int(particle->pt.z);

				// movement

			pps->vct.x=particle_float(particle->vct.x);
			pps->vct.y=particle_float(particle->vct.y);
			pps->vct.z=particle_float(particle->vct.z);

			pps++;
		}
	}

	particle->current_variation_idx=0;
}

void particle_globe_precalculate(particle_type *particle)
{
	int					n,k,y,count,row_count;
	float				rxz,ry,vy,r_xz_add,r_y_add,xz_reduce;
	double				d_x_radius,d_y_radius,d_z_radius,d_vx_radius,d_vz_radius;
	particle_piece_type	*pps;

		// get per-row count

	count=particle->count;
	row_count=count/20;

		// globes don't use variations

	pps=particle->pieces[0];

		// create particle globe

	d_y_radius=(double)particle->pt.y;

	r_xz_add=ANG_to_RAD*(360/row_count);

	ry=0.0f;
	r_y_add=ANG_to_RAD*(180/10);

	count=0;

	for (n=0;n!=19;n++) {				// the y

		if (n==10) {
			ry=-r_y_add;
			r_y_add=-r_y_add;			// both hemispheres
		}
		
		y=(int)(-sin(ry)*d_y_radius);
		vy=(float)(-sin(ry)*particle->vct.y);

		xz_reduce=(float)cos(ry);

		d_x_radius=(double)(((float)particle->pt.x)*xz_reduce);
		d_z_radius=(double)(((float)particle->pt.z)*xz_reduce);

		d_vx_radius=(double)(particle->vct.x*xz_reduce);
		d_vz_radius=(double)(particle->vct.z*xz_reduce);

		rxz=0.0;
			
		for (k=0;k!=row_count;k++) {			// the x and z
			pps->pt.x=(int)(-sin(rxz)*d_x_radius);
			pps->pt.y=y;
			pps->pt.z=(int)(cos(rxz)*d_z_radius);

			pps->vct.x=(float)(-sin(rxz)*d_vx_radius);
			pps->vct.y=vy;
			pps->vct.z=(float)(cos(rxz)*d_vz_radius);

			pps++;
			count++;
			
			rxz+=r_xz_add;
		}

		ry+=r_y_add;
	}

		// reset particle count to equal
		// globe count

	particle->count=count;

	particle->current_variation_idx=0;
}

void particle_precalculate_all(void)
{
	int					n;
	particle_type		*particle;

	particle=server.particles;
	
	for (n=0;n!=server.count.particle;n++) {
		if (!particle->globe) {
			particle_precalculate(particle);
		}
		else {
			particle_globe_precalculate(particle);
		}
		particle++;
	}
}

/* =======================================================

      Find Particle by Name
      
======================================================= */

particle_type* particle_find(char *name)
{
	int				n;
	particle_type	*particle;
	
	particle=server.particles;
	
	for (n=0;n!=server.count.particle;n++) {
		if (strcasecmp(particle->name,name)==0) return(particle);
		particle++;
	}
	
	return(NULL);
}

int particle_find_index(char *name)
{
	int				n;
	particle_type	*particle;
	
	particle=server.particles;
	
	for (n=0;n!=server.count.particle;n++) {
		if (strcasecmp(particle->name,name)==0) return(n);
		particle++;
	}
	
	return(-1);
}

/* =======================================================

      Find Total Size of Particle Effect
      
======================================================= */

int particle_get_effect_size(particle_type *particle)
{
	int			x,y,z;

	x=particle->pt.x+(int)particle->vct.x;
	y=particle->pt.y+(int)particle->vct.y;
	z=particle->pt.z+(int)particle->vct.z;

	return(distance_get(0,0,0,x,y,z));
}

/* =======================================================

      Spawn Particles
      
======================================================= */

bool particle_spawn_single(int particle_idx,int obj_uid,d3pnt *pt,particle_rotate *rot,particle_motion *motion)
{
	obj_type				*obj;
	effect_type				*effect;
	particle_effect_data	*eff_particle;
	particle_type			*particle;
	
	particle=&server.particles[particle_idx];
		
		// create particle

	effect=effect_spawn(ef_particle,pt,particle->life_msec);
	if (effect==NULL) return(FALSE);
	
	eff_particle=&effect->data.particle;
	eff_particle->particle_idx=particle_idx;

		// set variation

	if (!particle->globe) {
		eff_particle->variation_idx=particle->current_variation_idx;

		particle->current_variation_idx++;
		if (particle->current_variation_idx>=max_particle_variation) particle->current_variation_idx=0;
	}
	else {
		eff_particle->variation_idx=0;
	}

		// setup rotate and motion

	if (rot==NULL) {
		eff_particle->rot.on=FALSE;
	}
	else {
		memmove(&eff_particle->rot,rot,sizeof(particle_rotate));
	}

	if (motion==NULL) {
		eff_particle->motion.vct.x=eff_particle->motion.vct.y=eff_particle->motion.vct.z=0.0f;
		eff_particle->motion.bone_idx=-1;
	}
	else {
		memmove(&eff_particle->motion,motion,sizeof(particle_motion));
	}

		// setup size

	effect->size=particle_get_effect_size(particle);

		// any tinting

	eff_particle->tint.r=eff_particle->tint.g=eff_particle->tint.b=1.0f;

	if ((particle->team_tint) && (obj_uid!=-1)) {
		obj=object_find_uid(obj_uid);
		if (obj!=NULL) object_get_tint(obj,&eff_particle->tint);
	}
	
	return(TRUE);
}

bool particle_spawn(int particle_idx,int obj_uid,d3pnt *pt,particle_rotate *rot,particle_motion *motion)
{
	int						n,count,shift,idx;
	float					xoff,yoff,zoff;
	d3pnt					ppt;
	d3ang					ang;
	matrix_type				mat_x,mat_y;
	particle_type			*particle;
	
	particle=&server.particles[particle_idx];

		// single particles

	if (!particle->group.on) {
		return(particle_spawn_single(particle_idx,obj_uid,pt,rot,motion));
	}

		// get offset to camera position

	camera_get_angle_from(pt,&ang);
	matrix_rotate_x(&mat_x,ang.x);
	matrix_rotate_y(&mat_y,ang.y);

		// groups of particles

	count=particle->group.count;

	for (n=0;n!=count;n++) {
		idx=particle_find_index(particle->group.particles[n].name);
		if (idx==-1) continue;
		if (server.particles[idx].group.on) continue;		// don't respawn other groups

			// compute shifts

		memmove(&ppt,pt,sizeof(d3pnt));

		shift=server.particles[idx].group.particles[n].shift;

		if (shift!=0) {
			xoff=yoff=0.0f;
			zoff=-(float)shift;
			matrix_vertex_multiply(&mat_x,&xoff,&yoff,&zoff);
			matrix_vertex_multiply(&mat_y,&xoff,&yoff,&zoff);
			
			ppt.x+=(int)xoff;
			ppt.y+=(int)yoff;
			ppt.z+=(int)zoff;
		}

			// spawn particle

		if (!particle_spawn_single(idx,obj_uid,&ppt,rot,motion)) return(FALSE);
	}

	return(TRUE);
}

bool particle_line_spawn(int particle_idx,int obj_uid,d3pnt *start_pt,d3pnt *end_pt,int count)
{
	int				n,dx,dz,dy;
	d3pnt			pt;

	if (count<=0) return(TRUE);

	dx=end_pt->x-start_pt->x;
	dy=end_pt->y-start_pt->y;
	dz=end_pt->z-start_pt->z;

	for (n=0;n!=count;n++) {
		pt.x=start_pt->x+((dx*n)/count);
		pt.y=start_pt->y+((dy*n)/count);
		pt.z=start_pt->z+((dz*n)/count);

		if (!particle_spawn(particle_idx,obj_uid,&pt,NULL,NULL)) return(FALSE);
	}

	return(TRUE);
}

/* =======================================================

      Particle Gravity
      
======================================================= */

float particle_get_gravity(particle_type *particle,int count)
{
	float			gravity,f_count;
	
	count--;
	if (count<=0) return(0);
	
	f_count=(float)count;
	
		// gravity at this time
		
	gravity=particle->start_gravity+(particle->gravity_add*f_count);
	
		// particle movements multipled over time
		
	return(gravity*f_count);
}

/* =======================================================

      Map Particles
      
======================================================= */

void particle_map_set_next_tick(map_particle_type *particle)
{
	particle->next_spawn_tick=particle->spawn_tick;
	if (particle->slop_tick!=0) particle->next_spawn_tick+=random_int(particle->slop_tick);
}

void particle_map_initialize(void)
{
	int					n;
	map_particle_type	*particle;
	
	particle=map.particles;
	
	for (n=0;n!=map.nparticle;n++) {
		particle_map_set_next_tick(particle);
		particle++;
	}
}

void particle_map_run(void)
{
	int					n;
	map_particle_type	*particle;

	particle=map.particles;
	
	for (n=0;n!=map.nparticle;n++) {

		if (particle->particle_idx==-1) {
			particle++;
			continue;
		}

		particle->next_spawn_tick-=10;
		if (particle->next_spawn_tick<=0) {
			particle_spawn(particle->particle_idx,-1,&particle->pnt,NULL,NULL);
			particle_map_set_next_tick(particle);
		}

		particle++;
	}
}

