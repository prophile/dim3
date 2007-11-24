/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Particle XML

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

#include "xmls.h"

extern server_type			server;
extern setup_type			setup;

/* =======================================================

      Read Particle XML
      
======================================================= */

void read_settings_particle(void)
{
	int					n,particle_data_head_tag,particle_head_tag,particle_tag,
						particle_group_head_tag,particle_group_tag,tag,
						ring_radius,ring_size;
	float				ring_min_move,ring_max_move;
	bool				ring_flip;
	char				path[1024];
	particle_type		*particle;

		// read in particles from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Particles","xml");
	if (!xml_open_file(path)) return;
	
		// can accept file with a particle_data tag or not
		
	particle_data_head_tag=xml_findrootchild("Particle_Data");
	
		// particles
       
	if (particle_data_head_tag==-1) {
		particle_head_tag=xml_findrootchild("Particles");
	}
	else {
		particle_head_tag=xml_findfirstchild("Particles",particle_data_head_tag);
	}
	
    if (particle_head_tag==-1) {
		xml_close_file();
		return;
	}

	particle_tag=xml_findfirstchild("Particle",particle_head_tag);
    
	while (particle_tag!=-1) {
	
			// create a new particle
			
		particle=&server.particles[server.count.particle];
		
		xml_get_attribute_text(particle_tag,"name",particle->name,name_str_len);
		
		particle->group.on=FALSE;

		particle->count=1;
		particle->life_msec=1000;
		particle->reverse=FALSE;
		particle->ambient_factor=1.0f;
		particle->blend_add=FALSE;

        tag=xml_findfirstchild("Settings",particle_tag);
        if (tag!=-1) {
            particle->count=xml_get_attribute_int(tag,"count");
            particle->life_msec=xml_get_attribute_int(tag,"time");
            particle->reverse=xml_get_attribute_boolean(tag,"reverse");
            particle->ambient_factor=xml_get_attribute_float_default(tag,"ambient",1.0f);
            particle->blend_add=xml_get_attribute_boolean(tag,"additive");
		}

		if (particle->count>max_particle_count) particle->count=max_particle_count;
		
		particle->animate.image_count=1;
		particle->animate.image_per_row=1;
		particle->animate.msec=200;
		particle->animate.loop=TRUE;
		particle->animate.loop_back=FALSE;
		
        tag=xml_findfirstchild("Image",particle_tag);
        if (tag!=-1) {
            xml_get_attribute_text(tag,"file",particle->bitmap.name,file_str_len);
            particle->animate.image_count=xml_get_attribute_int(tag,"count");
			particle->animate.image_per_row=(int)sqrt((float)particle->animate.image_count);
			particle->animate.msec=xml_get_attribute_int(tag,"time");
            particle->animate.loop=xml_get_attribute_boolean(tag,"loop");
            particle->animate.loop_back=xml_get_attribute_boolean(tag,"loop_back");
		}
		
        tag=xml_findfirstchild("Trail",particle_tag);
        if (tag!=-1) {
            particle->trail_count=xml_get_attribute_int(tag,"count");
            particle->trail_step=xml_get_attribute_int(tag,"step");
            particle->reduce_pixel_fact=xml_get_attribute_float(tag,"reduce");
        }
		
		if (particle->trail_step>max_particle_trail) particle->trail_step=max_particle_trail;
		
        tag=xml_findfirstchild("Gravity",particle_tag);
        if (tag!=-1) {
            particle->start_gravity=xml_get_attribute_float(tag,"start");
            particle->gravity_add=xml_get_attribute_float(tag,"add");
        }
		
        tag=xml_findfirstchild("Size",particle_tag);
        if (tag!=-1) {
            particle->start_pixel_size=xml_get_attribute_int(tag,"start");
            particle->end_pixel_size=xml_get_attribute_int(tag,"end");
        }

			// old way of defining particles

        tag=xml_findfirstchild("Ring",particle_tag);
        if (tag!=-1) {
            ring_radius=xml_get_attribute_int(tag,"radius");
            ring_size=xml_get_attribute_int(tag,"size");
            ring_min_move=xml_get_attribute_float(tag,"min_move");
            ring_max_move=xml_get_attribute_float(tag,"max_move");
			ring_flip=xml_get_attribute_boolean(tag,"flip");

			particle->rot.x=particle->rot.y=particle->rot.z=0.0f;
			particle->rot_accel.x=particle->rot_accel.y=particle->rot_accel.z=0.0f;
		
			if (ring_flip) {
				particle->pt.x=ring_radius+ring_size;
				particle->pt.z=ring_radius+ring_size;
				particle->pt.y=0;
		
				particle->vct.x=ring_max_move;
				particle->vct.z=ring_max_move;
				particle->vct.y=0;
			}
			else {
				particle->pt.x=ring_radius+ring_size;
				particle->pt.y=ring_radius+ring_size;
				particle->pt.z=ring_radius+ring_size;
		
				particle->vct.x=ring_max_move;
				particle->vct.y=ring_max_move;
				particle->vct.z=ring_max_move;
			}
		}

			// new way of defining particles

		else {

			tag=xml_findfirstchild("X",particle_tag);
			if (tag!=-1) {
				particle->pt.x=xml_get_attribute_int(tag,"offset");
				particle->vct.x=xml_get_attribute_float(tag,"move");
				particle->rot.x=xml_get_attribute_float_default(tag,"rot",0.0f);
				particle->rot_accel.x=xml_get_attribute_float_default(tag,"rot_accel",0.0f);
			}

			tag=xml_findfirstchild("Y",particle_tag);
			if (tag!=-1) {
				particle->pt.y=xml_get_attribute_int(tag,"offset");
				particle->vct.y=xml_get_attribute_float(tag,"move");
				particle->rot.y=xml_get_attribute_float_default(tag,"rot",0.0f);
				particle->rot_accel.y=xml_get_attribute_float_default(tag,"rot_accel",0.0f);
			}

			tag=xml_findfirstchild("Z",particle_tag);
			if (tag!=-1) {
				particle->pt.z=xml_get_attribute_int(tag,"offset");
				particle->vct.z=xml_get_attribute_float(tag,"move");
				particle->rot.z=xml_get_attribute_float_default(tag,"rot",0.0f);
				particle->rot_accel.z=xml_get_attribute_float_default(tag,"rot_accel",0.0f);
			}

		}

        tag=xml_findfirstchild("Color",particle_tag);
        if (tag!=-1) {
			xml_get_attribute_color(tag,"start",&particle->start_color);
			xml_get_attribute_color(tag,"end",&particle->end_color);
        }
		
        tag=xml_findfirstchild("Alpha",particle_tag);
        if (tag!=-1) {
            particle->start_alpha=xml_get_attribute_float(tag,"start");
            particle->end_alpha=xml_get_attribute_float(tag,"end");
        }
	
			// move on to next particle
			
		server.count.particle++;
		
		particle_tag=xml_findnextchild(particle_tag);
	}

		// particle groups

	if (particle_data_head_tag==-1) {
		particle_group_head_tag=xml_findrootchild("Particle_Groups");
	}
	else {
		particle_group_head_tag=xml_findfirstchild("Particle_Groups",particle_data_head_tag);
	}

    if (particle_group_head_tag==-1) {
		xml_close_file();
		return;
	}

	particle_group_tag=xml_findfirstchild("Particle_Group",particle_group_head_tag);
    
	while (particle_group_tag!=-1) {
	
			// create a new particle group
			
		particle=&server.particles[server.count.particle];
		
		xml_get_attribute_text(particle_group_tag,"name",particle->name,name_str_len);
		
		particle->group.on=TRUE;
		particle->group.count=0;

        particle_head_tag=xml_findfirstchild("Particles",particle_group_tag);
        if (particle_head_tag!=-1) {

			particle->group.count=xml_countchildren(particle_head_tag);
			if (particle->group.count>max_particle_group) particle->group.count=max_particle_group;

			particle_tag=xml_findfirstchild("Particle",particle_head_tag);
    
			for (n=0;n!=particle->group.count;n++) {
				xml_get_attribute_text(particle_tag,"name",particle->group.particles[n].name,name_str_len);
				particle->group.particles[n].shift=xml_get_attribute_int_default(particle_tag,"offset",0);
				particle_tag=xml_findnextchild(particle_tag);
			}
		}

			// move on to next particle group
			
		server.count.particle++;
		
		particle_group_tag=xml_findnextchild(particle_group_tag);
	}
	
	xml_close_file();
}

