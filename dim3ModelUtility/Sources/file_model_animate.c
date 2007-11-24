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
	int						i,k,tag,model_head,poses_tag,animations_tag,animation_tag;
    char					sub_path[1024],path[1024],posename[256];
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

            pose_move->effect_bone_idx=model_find_bone(model,xml_get_attribute_model_tag(tag,"effect_bone"));

			xml_get_attribute_text(tag,"sound",pose_move->sound.name,name_str_len);
			pose_move->sound.pitch=xml_get_attribute_float_default(tag,"sound_pitch",1.0f);
         
			xml_get_attribute_text(tag,"particle",pose_move->particle.name,name_str_len);
			pose_move->particle.rotate=xml_get_attribute_boolean(tag,"particle_rotate");
			pose_move->particle.motion=xml_get_attribute_boolean(tag,"particle_motion");
			pose_move->particle.motion_factor=xml_get_attribute_float_default(tag,"particle_motion_factor",1.0f);
			pose_move->particle.stick=xml_get_attribute_boolean(tag,"particle_stick");
			xml_get_attribute_3_coord_int(tag,"particle_slop",&pose_move->particle.slop.x,&pose_move->particle.slop.y,&pose_move->particle.slop.z);

			xml_get_attribute_text(tag,"ring",pose_move->ring.name,name_str_len);
			pose_move->ring.angle=xml_get_attribute_boolean(tag,"ring_angle");
			xml_get_attribute_3_coord_int(tag,"ring_slop",&pose_move->ring.slop.x,&pose_move->ring.slop.y,&pose_move->ring.slop.z);

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
	int						i,k;
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
			
			if (pose_move->effect_bone_idx==-1) {
				xml_add_attribute_model_tag("effect_bone",model_null_tag);
			}
			else {
				xml_add_attribute_model_tag("effect_bone",model->bones[pose_move->effect_bone_idx].tag);
			}
			
            xml_add_attribute_text("sound",pose_move->sound.name);
            xml_add_attribute_float("sound_pitch",pose_move->sound.pitch);
			
			xml_add_attribute_text("particle",pose_move->particle.name);
			xml_add_attribute_boolean("particle_rotate",pose_move->particle.rotate);
			xml_add_attribute_boolean("particle_motion",pose_move->particle.motion);
			xml_add_attribute_float("particle_motion_factor",pose_move->particle.motion_factor);
			xml_add_attribute_boolean("particle_stick",pose_move->particle.stick);
			xml_add_attribute_3_coord_int("particle_slop",pose_move->particle.slop.x,pose_move->particle.slop.y,pose_move->particle.slop.z);

			xml_add_attribute_text("ring",pose_move->ring.name);
			xml_add_attribute_boolean("ring_angle",pose_move->ring.angle);
			xml_add_attribute_3_coord_int("ring_slop",pose_move->ring.slop.x,pose_move->ring.slop.y,pose_move->ring.slop.z);

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
			
            xml_add_tagend(TRUE);
            
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

