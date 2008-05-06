/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Pose File Loading

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

      Read Pose XML
      
======================================================= */

bool read_pose_xml(model_type *model)
{
	int						i,k,t,cnt,tag,model_head,bone_tag,poses_tag,pose_tag,constraint_bone_idx;
	char					sub_path[1024],path[1024];
	model_tag				constraint_bone_tag;
    model_bone_move_type	*bone_move;
	model_pose_type			*pose;
	
        // load the pose xml
        
	sprintf(sub_path,"Models/%s",model->name);
	file_paths_data(&modelutility_settings.file_path_setup,path,sub_path,"pose","xml");

	if (!xml_open_file(path)) return(FALSE);

    model_head=xml_findrootchild("Model");
    if (model_head==-1) return(FALSE);
    
        // poses
        
    poses_tag=xml_findfirstchild("Poses",model_head);

    model->npose=xml_countchildren(poses_tag);
	pose_tag=xml_findfirstchild("Pose",poses_tag);

    pose=model->poses;
    
    for (i=0;i!=model->npose;i++) {
        xml_get_attribute_text(pose_tag,"name",pose->name,64);
        
        bone_move=pose->bone_moves;
        for (k=0;k!=model->nbone;k++) {
            bone_move->rot.x=bone_move->rot.z=bone_move->rot.y=0.0f;
            bone_move->mov.x=bone_move->mov.z=bone_move->mov.y=1.0f;
			bone_move->acceleration=0;
			bone_move->skip_blended=FALSE;
			bone_move->constraint.bone_idx=-1;
			bone_move->constraint.offset.x=bone_move->constraint.offset.y=bone_move->constraint.offset.z=0;
			bone_move++;
        }
            
        bone_tag=xml_findfirstchild("Bones",pose_tag);
        
        cnt=xml_countchildren(bone_tag);
		tag=xml_findfirstchild("Bone",bone_tag);
        
        for (k=0;k!=cnt;k++) {
		
            t=model_find_bone(model,xml_get_attribute_model_tag(tag,"tag"));
            if (t!=-1) {
                bone_move=&pose->bone_moves[t];
                xml_get_attribute_3_coord_float(tag,"rot",&bone_move->rot.x,&bone_move->rot.y,&bone_move->rot.z);
				xml_get_attribute_3_coord_float(tag,"move",&bone_move->mov.x,&bone_move->mov.y,&bone_move->mov.z);
				
				bone_move->acceleration=xml_get_attribute_float(tag,"acceleration");
				bone_move->skip_blended=xml_get_attribute_boolean(tag,"skip_blended");
				
				bone_move->constraint.bone_idx=-1;
				bone_move->constraint.offset.x=bone_move->constraint.offset.y=bone_move->constraint.offset.z=0.0f;
				
				constraint_bone_tag=xml_get_attribute_model_tag(tag,"constraint_bone");
				if (constraint_bone_tag!=model_null_tag) {
					constraint_bone_idx=model_find_bone(model,constraint_bone_tag);
					if (constraint_bone_idx!=-1) {
						bone_move->constraint.bone_idx=constraint_bone_idx;
						xml_get_attribute_3_coord_int(tag,"constraint_offset",&bone_move->constraint.offset.x,&bone_move->constraint.offset.y,&bone_move->constraint.offset.z);
					}
				}
			}
		  
			tag=xml_findnextchild(tag);
        }
    
        pose++;
		pose_tag=xml_findnextchild(pose_tag);
    }

    xml_close_file();
    
    return(TRUE);
}

/* =======================================================

      Write Pose XML
      
======================================================= */

bool write_bone_move(model_bone_move_type *bone_move)
{
    if ((bone_move->rot.x!=0) || (bone_move->rot.z!=0) || (bone_move->rot.y!=0)) return(TRUE);
    if ((bone_move->mov.x!=1) || (bone_move->mov.z!=1) || (bone_move->mov.y!=1)) return(TRUE);
	if (bone_move->acceleration!=0) return(TRUE);
	if (bone_move->skip_blended) return(TRUE);
    return(FALSE);
}

bool write_pose_xml(model_type *model)
{
	int						i,k;
	char					path[1024];
	bool					ok;
    model_bone_move_type	*bone_move;
	model_pose_type			*pose;
	
    xml_new_file();
    
    xml_add_tagstart("Model");
    xml_add_tagend(FALSE);
    
        // model info
    
    xml_add_tagstart("Creator");
    xml_add_attribute_text("name","dim3 Animator");
    xml_add_attribute_text("version","1.0");
    xml_add_tagend(TRUE);
    
        // poses
   
    xml_add_tagstart("Poses");
    xml_add_tagend(FALSE);
    
    pose=model->poses;
    
    for (i=0;i!=model->npose;i++) {
    
        xml_add_tagstart("Pose");
        xml_add_attribute_text("name",pose->name);
        xml_add_tagend(FALSE);
        
        xml_add_tagstart("Bones");
        xml_add_tagend(FALSE);
        
        bone_move=pose->bone_moves;
        
        for (k=0;k!=model->nbone;k++) {
        
            if (write_bone_move(bone_move)) {
                xml_add_tagstart("Bone");
                xml_add_attribute_model_tag("tag",model->bones[k].tag);
                xml_add_attribute_3_coord_float("rot",bone_move->rot.x,bone_move->rot.y,bone_move->rot.z);
                xml_add_attribute_3_coord_float("move",bone_move->mov.x,bone_move->mov.y,bone_move->mov.z);
				
				xml_add_attribute_float("acceleration",bone_move->acceleration);
				xml_add_attribute_boolean("skip_blended",bone_move->skip_blended);
				
				if (bone_move->constraint.bone_idx!=-1) {
					xml_add_attribute_model_tag("constraint_bone",model->bones[bone_move->constraint.bone_idx].tag);
					xml_add_attribute_3_coord_float("constraint_offset",bone_move->constraint.offset.x,bone_move->constraint.offset.y,bone_move->constraint.offset.z);
				}
				
				xml_add_tagend(TRUE);
            }
            
            bone_move++;
        }
            
        xml_add_tagclose("Bones");
        xml_add_tagclose("Pose");
    
        pose++;
    }
    
    xml_add_tagclose("Poses");

        // finish model
        
    xml_add_tagclose("Model");
    
	sprintf(path,"%s/pose.xml",model->load_base_path);
	ok=xml_save_file(path);

    xml_close_file();

	return(ok);
}
