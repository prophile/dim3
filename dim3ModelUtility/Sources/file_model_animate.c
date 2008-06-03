/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Animate File Loading

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

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

extern modelutility_settings_type		modelutility_settings;

/* =======================================================

      Read Animate XML
      
======================================================= */

bool read_animate_xml(model_type *model)
{
	int						i,k,t,tag,model_head,poses_tag,animations_tag,animation_tag,
							particles_head,particle_tag,rings_head,ring_tag;
    char					sub_path[1024],path[1024],posename[256];
	bool					old_particle_def,old_ring_def;
    model_pose_move_type	*pose_move;
	model_animate_type		*animate;

        // load the animate xml
        
	sprintf(sub_path,"Models/%s",model->name);
	file_paths_data(&modelutility_settings.file_path_setup,path,sub_path,"animate","xml");
	
	if (!xml_open_file(path)) return(FALSE);

    model_head=xml_findrootchild("Model");
    if (model_head==-1) return(FALSE);
    
        // animations
        
    animations_tag=xml_findfirstchild("Animations",model_head);

    model->nanimate=xml_countchildren(animations_tag);
	animation_tag=xml_findfirstchild("Animation",animations_tag);
	
    animate=model->animates;
    
    for (i=0;i!=model->nanimate;i++) {
        xml_get_attribute_text(animation_tag,"name",animate->name,64);
        
        tag=xml_findfirstchild("Loop",animation_tag);
        animate->loop=xml_get_attribute_boolean(tag,"repeat");
        animate->loop_start=xml_get_attribute_int(tag,"start");
        animate->loop_end=xml_get_attribute_int(tag,"end");
        
			// pose moves

        poses_tag=xml_findfirstchild("Poses",animation_tag);
        
        animate->npose_move=xml_countchildren(poses_tag);
		tag=xml_findfirstchild("Pose",poses_tag);
		
        pose_move=animate->pose_moves;
        
        for (k=0;k!=animate->npose_move;k++) {
            xml_get_attribute_text(tag,"name",posename,64);
            pose_move->pose_idx=model_find_pose(model,posename);
			
            pose_move->msec=xml_get_attribute_int(tag,"time");
            xml_get_attribute_3_coord_float(tag,"sway",&pose_move->sway.x,&pose_move->sway.y,&pose_move->sway.z);
            xml_get_attribute_3_coord_float(tag,"move",&pose_move->mov.x,&pose_move->mov.y,&pose_move->mov.z);

            pose_move->acceleration=xml_get_attribute_float_default(tag,"acceleration",0.0f);

			xml_get_attribute_text(tag,"sound",pose_move->sound.name,name_str_len);
			pose_move->sound.pitch=xml_get_attribute_float_default(tag,"sound_pitch",1.0f);
         
			xml_get_attribute_text(tag,"mesh_fade",pose_move->mesh_fade.name,name_str_len);
			pose_move->mesh_fade.fade_in_msec=xml_get_attribute_int(tag,"mesh_fade_in_time");
			pose_move->mesh_fade.fade_out_msec=xml_get_attribute_int(tag,"mesh_fade_out_time");
			pose_move->mesh_fade.mesh_idx=model_find_mesh(model,pose_move->mesh_fade.name);

			pose_move->flash.intensity=xml_get_attribute_int(tag,"flash_intensity");
			pose_move->flash.flash_msec=xml_get_attribute_int(tag,"flash_time");
			pose_move->flash.fade_msec=xml_get_attribute_int(tag,"flash_fade_time");
			xml_get_attribute_color(tag,"flash_color",&pose_move->flash.col);
			
			pose_move->shake.distance=xml_get_attribute_int(tag,"shake_distance");
			pose_move->shake.size=xml_get_attribute_int(tag,"shake_size");
			pose_move->shake.life_msec=xml_get_attribute_int(tag,"shake_time");

				// old particle defines

			pose_move->particle.count=0;
			
			old_particle_def=xml_get_attribute_text(tag,"particle",pose_move->particle.particles[0].name,name_str_len);
			if (old_particle_def) {
				if (pose_move->particle.particles[0].name[0]!=0x0) {
					pose_move->particle.count=1;

					pose_move->particle.particles[0].bone_idx=model_find_bone(model,xml_get_attribute_model_tag(tag,"effect_bone"));
					pose_move->particle.particles[0].rotate=xml_get_attribute_boolean(tag,"particle_rotate");
					pose_move->particle.particles[0].motion=xml_get_attribute_boolean(tag,"particle_motion");
					pose_move->particle.particles[0].motion_factor=xml_get_attribute_float_default(tag,"particle_motion_factor",1.0f);
					pose_move->particle.particles[0].stick=xml_get_attribute_boolean(tag,"particle_stick");
					xml_get_attribute_3_coord_int(tag,"particle_slop",&pose_move->particle.particles[0].slop.x,&pose_move->particle.particles[0].slop.y,&pose_move->particle.particles[0].slop.z);
				}
			}

				// new particle defines

			else {
				particles_head=xml_findfirstchild("Particles",tag);
        
				if (particles_head!=-1) {
					pose_move->particle.count=xml_countchildren(particles_head);
					particle_tag=xml_findfirstchild("Particle",particles_head);
        
					for (t=0;t!=pose_move->particle.count;t++) {
						pose_move->particle.particles[t].bone_idx=model_find_bone(model,xml_get_attribute_model_tag(particle_tag,"bone"));
						xml_get_attribute_text(particle_tag,"particle",pose_move->particle.particles[t].name,name_str_len);
						pose_move->particle.particles[t].rotate=xml_get_attribute_boolean(particle_tag,"particle_rotate");
						pose_move->particle.particles[t].motion=xml_get_attribute_boolean(particle_tag,"particle_motion");
						pose_move->particle.particles[t].motion_factor=xml_get_attribute_float_default(particle_tag,"particle_motion_factor",1.0f);
						pose_move->particle.particles[t].stick=xml_get_attribute_boolean(particle_tag,"particle_stick");
						xml_get_attribute_3_coord_int(particle_tag,"particle_slop",&pose_move->particle.particles[t].slop.x,&pose_move->particle.particles[t].slop.y,&pose_move->particle.particles[t].slop.z);

						particle_tag=xml_findnextchild(particle_tag);
					}
				}
			}

				// old ring defines

			pose_move->ring.count=0;

			old_ring_def=xml_get_attribute_text(tag,"ring",pose_move->ring.rings[0].name,name_str_len);
			if (old_ring_def) {
				if (pose_move->ring.rings[0].name[0]!=0x0) {
					pose_move->ring.count=1;

					pose_move->ring.rings[0].bone_idx=model_find_bone(model,xml_get_attribute_model_tag(tag,"effect_bone"));
					pose_move->ring.rings[0].angle=xml_get_attribute_boolean(tag,"ring_angle");
					xml_get_attribute_3_coord_int(tag,"ring_slop",&pose_move->ring.rings[0].slop.x,&pose_move->ring.rings[0].slop.y,&pose_move->ring.rings[0].slop.z);
				}
			}

				// new ring defines

			else {
				rings_head=xml_findfirstchild("Rings",tag);
        
				if (rings_head!=-1) {
					pose_move->ring.count=xml_countchildren(rings_head);
					ring_tag=xml_findfirstchild("Ring",rings_head);
        
					for (t=0;t!=pose_move->ring.count;t++) {
						pose_move->ring.rings[t].bone_idx=model_find_bone(model,xml_get_attribute_model_tag(ring_tag,"bone"));
						xml_get_attribute_text(ring_tag,"ring",pose_move->ring.rings[t].name,name_str_len);
						pose_move->ring.rings[t].angle=xml_get_attribute_boolean(ring_tag,"ring_angle");
						xml_get_attribute_3_coord_int(ring_tag,"ring_slop",&pose_move->ring.rings[t].slop.x,&pose_move->ring.rings[t].slop.y,&pose_move->ring.rings[t].slop.z);

						ring_tag=xml_findnextchild(ring_tag);
					}
				}
			}

            pose_move++;
			tag=xml_findnextchild(tag);
        }
        
 		if (animate->loop_start<0) animate->loop_start=0;
		if (animate->loop_end==-1) animate->loop_end=animate->npose_move-1;
        if (animate->loop_end>(animate->npose_move-1)) animate->loop_end=animate->npose_move-1;
         
        animate++;
		animation_tag=xml_findnextchild(animation_tag);
    }

    xml_close_file();
    
    return(TRUE);
}

/* =======================================================

      Write Animate XML
      
======================================================= */

bool write_animate_xml(model_type *model)
{
	int						i,k,t;
	char					path[1024];
	bool					ok;
    model_pose_move_type	*pose_move;
	model_animate_type		*animate;
    
    xml_new_file();
    
    xml_add_tagstart("Model");
    xml_add_tagend(FALSE);
    
        // model info
    
    xml_add_tagstart("Creator");
    xml_add_attribute_text("name","dim3 Animator");
    xml_add_attribute_text("version","1.0");
    xml_add_tagend(TRUE);
    
        // animations
   
    xml_add_tagstart("Animations");
    xml_add_tagend(FALSE);
    
    animate=model->animates;
    
    for (i=0;i!=model->nanimate;i++) {
    
        xml_add_tagstart("Animation");
        xml_add_attribute_text("name",animate->name);
        xml_add_tagend(FALSE);
        
        xml_add_tagstart("Loop");
        xml_add_attribute_boolean("repeat",animate->loop);
        xml_add_attribute_int("start",animate->loop_start);
        xml_add_attribute_int("end",animate->loop_end);
        xml_add_tagend(TRUE);
        
			// pose moves

        xml_add_tagstart("Poses");
        xml_add_tagend(FALSE);
        
        pose_move=animate->pose_moves;
        
        for (k=0;k!=animate->npose_move;k++) {
        
            xml_add_tagstart("Pose");
			
            xml_add_attribute_text("name",model->poses[pose_move->pose_idx].name);
			
            xml_add_attribute_int("time",pose_move->msec);
            xml_add_attribute_3_coord_float("sway",pose_move->sway.x,pose_move->sway.y,pose_move->sway.z);
            xml_add_attribute_3_coord_float("move",pose_move->mov.x,pose_move->mov.y,pose_move->mov.z);
 
			xml_add_attribute_float("acceleration",pose_move->acceleration);
			
            xml_add_attribute_text("sound",pose_move->sound.name);
            xml_add_attribute_float("sound_pitch",pose_move->sound.pitch);
			
			xml_add_attribute_text("mesh_fade",pose_move->mesh_fade.name);
			xml_add_attribute_int("mesh_fade_in_time",pose_move->mesh_fade.fade_in_msec);
			xml_add_attribute_int("mesh_fade_out_time",pose_move->mesh_fade.fade_out_msec);

			xml_add_attribute_int("flash_intensity",pose_move->flash.intensity);
			xml_add_attribute_int("flash_time",pose_move->flash.flash_msec);
			xml_add_attribute_int("flash_fade_time",pose_move->flash.fade_msec);
			xml_add_attribute_color("flash_color",&pose_move->flash.col);
			
			xml_add_attribute_int("shake_distance",pose_move->shake.distance);
			xml_add_attribute_int("shake_size",pose_move->shake.size);
			xml_add_attribute_int("shake_time",pose_move->shake.life_msec);
			
			xml_add_tagend(FALSE);

				// particles

			xml_add_tagstart("Particles");
			xml_add_tagend(FALSE);

			for (t=0;t!=pose_move->particle.count;t++) {
			
				xml_add_tagstart("Particle");
				
				if (pose_move->particle.particles[t].bone_idx==-1) {
					xml_add_attribute_model_tag("bone",model_null_tag);
				}
				else {
					xml_add_attribute_model_tag("bone",model->bones[pose_move->particle.particles[t].bone_idx].tag);
				}
				xml_add_attribute_text("particle",pose_move->particle.particles[t].name);
				xml_add_attribute_boolean("particle_rotate",pose_move->particle.particles[t].rotate);
				xml_add_attribute_boolean("particle_motion",pose_move->particle.particles[t].motion);
				xml_add_attribute_float("particle_motion_factor",pose_move->particle.particles[t].motion_factor);
				xml_add_attribute_boolean("particle_stick",pose_move->particle.particles[t].stick);
				xml_add_attribute_3_coord_int("particle_slop",pose_move->particle.particles[t].slop.x,pose_move->particle.particles[t].slop.y,pose_move->particle.particles[t].slop.z);
				
				xml_add_tagend(TRUE);
			}

			xml_add_tagclose("Particles");

				// rings

			xml_add_tagstart("Rings");
			xml_add_tagend(FALSE);

			for (t=0;t!=pose_move->ring.count;t++) {
			
				xml_add_tagstart("Ring");
				
				if (pose_move->ring.rings[t].bone_idx==-1) {
					xml_add_attribute_model_tag("bone",model_null_tag);
				}
				else {
					xml_add_attribute_model_tag("bone",model->bones[pose_move->ring.rings[t].bone_idx].tag);
				}
				xml_add_attribute_text("ring",pose_move->ring.rings[t].name);
				xml_add_attribute_boolean("ring_angle",pose_move->ring.rings[t].angle);
				xml_add_attribute_3_coord_int("ring_slop",pose_move->ring.rings[t].slop.x,pose_move->ring.rings[t].slop.y,pose_move->ring.rings[t].slop.z);
				
				xml_add_tagend(TRUE);
			}

			xml_add_tagclose("Rings");
			
			xml_add_tagclose("Pose");
            
            pose_move++;
        }
        
        xml_add_tagclose("Poses");
        xml_add_tagclose("Animation");
        
        animate++;
    }

    xml_add_tagclose("Animations");

        // finish model
        
    xml_add_tagclose("Model");
    
	sprintf(path,"%s/animate.xml",model->load_base_path);
	ok=xml_save_file(path);

    xml_close_file();

	return(ok);
}

