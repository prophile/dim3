/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Main Routines

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

modelutility_settings_type		modelutility_settings;

/* =======================================================

      Model Setup Routines
      
======================================================= */

void model_setup(file_path_setup_type *file_path_setup,int anisotropic_mode,int texture_quality_mode,int mipmap_mode,bool use_card_generated_mipmaps,bool use_compression)
{
	memmove(&modelutility_settings.file_path_setup,file_path_setup,sizeof(file_path_setup_type));
	modelutility_settings.anisotropic_mode=anisotropic_mode;
	modelutility_settings.texture_quality_mode=texture_quality_mode;
	modelutility_settings.mipmap_mode=mipmap_mode;
	modelutility_settings.card_generated_mipmaps=use_card_generated_mipmaps;
	modelutility_settings.compression=use_compression;
}

/* =======================================================

      New Model
      
======================================================= */

bool model_new(model_type *model,char *name)
{
	int				k;
	char			sub_path[1024];
	
    memset(model,0x0,sizeof(model_type));
	
		// model name and default path
		
	strcpy(model->name,name);
	
	sprintf(sub_path,"Models/%s",name);
	file_paths_data_default(&modelutility_settings.file_path_setup,model->load_base_path,sub_path,NULL,NULL);
		
		// default settings
	
	model->nmesh=1;
	model->deform_mode=deform_mode_comulative_rotate;
	
	model->shadow_fudge=0;
	
	for (k=0;k!=max_model_light;k++) {
		model->tags.light_bone_tag[k]=model_null_tag;
		model->tags.light_bone_idx[k]=-1;
	}

	for (k=0;k!=max_model_halo;k++) {
		model->tags.halo_bone_tag[k]=model_null_tag;
		model->tags.halo_bone_idx[k]=-1;
	}

	model->tags.name_bone_tag=model_null_tag;
	model->tags.name_bone_idx=-1;

	model->light.nspot=0;
	
		// get memory
		
	model->meshes[0].vertexes=valloc(max_model_vertex*sizeof(model_vertex_type));
	model->meshes[0].trigs=valloc(max_model_trig*sizeof(model_trig_type));
	model->meshes[0].materials=valloc(max_model_texture*sizeof(model_material_type));
	model->meshes[0].vertex_sel_mask=valloc(vertex_sel_hide_mask_sz);
	model->meshes[0].vertex_hide_mask=valloc(vertex_sel_hide_mask_sz);
	model->meshes[0].gl_vertex_array=valloc((max_model_vertex*3)*sizeof(float));
	model->meshes[0].gl_color_array=valloc((max_model_vertex*3)*sizeof(float));
	model->meshes[0].gl_vertex_normal_array=valloc((max_model_vertex*3)*sizeof(float));
	model->meshes[0].gl_light_normal_array=valloc((max_model_vertex*3)*sizeof(float));

	model->light.spots=(light_spot_type*)valloc(max_model_light_spot*sizeof(light_spot_type));
	
	model->bones=valloc(max_model_bone*sizeof(model_bone_type));
	model->poses=valloc(max_model_pose*sizeof(model_pose_type));
	model->animates=valloc(max_model_animate*sizeof(model_animate_type));
	model->hit_boxes=valloc(max_model_hit_box*sizeof(model_hit_box_type));
	model->textures=valloc(max_model_texture*sizeof(texture_type));
	
		// memory OK?
		
	if (model->meshes[0].vertexes==NULL) return(FALSE);
	if (model->meshes[0].trigs==NULL) return(FALSE);
	if (model->meshes[0].materials==NULL) return(FALSE);
	if (model->meshes[0].vertex_sel_mask==NULL) return(FALSE);
	if (model->meshes[0].vertex_hide_mask==NULL) return(FALSE);
	if (model->meshes[0].gl_vertex_array==NULL) return(FALSE);
	if (model->meshes[0].gl_color_array==NULL) return(FALSE);
	if (model->meshes[0].gl_vertex_normal_array==NULL) return(FALSE);
	if (model->meshes[0].gl_light_normal_array==NULL) return(FALSE);
	if (model->light.spots==NULL) return(FALSE);
	if (model->bones==NULL) return(FALSE);
	if (model->poses==NULL) return(FALSE);
	if (model->animates==NULL) return(FALSE);
	if (model->hit_boxes==NULL) return(FALSE);
	if (model->textures==NULL) return(FALSE);

		// zero memory
		
	bzero(model->meshes[0].vertexes,(max_model_vertex*sizeof(model_vertex_type)));
	bzero(model->meshes[0].trigs,(max_model_trig*sizeof(model_trig_type)));
	bzero(model->meshes[0].materials,(max_model_texture*sizeof(model_material_type)));
	bzero(model->meshes[0].vertex_sel_mask,vertex_sel_hide_mask_sz);
	bzero(model->meshes[0].vertex_hide_mask,vertex_sel_hide_mask_sz);
	bzero(model->meshes[0].gl_vertex_array,(max_model_vertex*3)*sizeof(float));
	bzero(model->meshes[0].gl_color_array,(max_model_vertex*3)*sizeof(float));
	bzero(model->meshes[0].gl_vertex_normal_array,(max_model_vertex*3)*sizeof(float));
	bzero(model->meshes[0].gl_light_normal_array,(max_model_vertex*3)*sizeof(float));

	bzero(model->light.spots,max_model_light_spot*sizeof(light_spot_type));

	bzero(model->bones,(max_model_bone*sizeof(model_bone_type)));
	bzero(model->poses,(max_model_pose*sizeof(model_pose_type)));
	bzero(model->animates,(max_model_animate*sizeof(model_animate_type)));
	bzero(model->hit_boxes,(max_model_hit_box*sizeof(model_hit_box_type)));
	bzero(model->textures,(max_model_texture*sizeof(texture_type)));

	model_textures_clear(model);
	
	return(TRUE);
}

/* =======================================================

      Open Model
      
======================================================= */

bool model_open(model_type *model,char *name,bool load_bitmaps,bool setup_glowmaps,bool load_shaders)
{
	char			sub_path[1024];
	
		// create model
		
	if (!model_new(model,name)) return(FALSE);
	
		// load base path is location of mesh.xml
		
	sprintf(sub_path,"Models/%s",model->name);
	file_paths_data(&modelutility_settings.file_path_setup,model->load_base_path,sub_path,NULL,NULL);
	
		// read XML
		
	if (!read_mesh_xml(model)) {
		model_close(model);
		return(FALSE);
	}
	
	read_pose_xml(model);
	read_animate_xml(model);
	
		// pre-calculate parents
		
	model_calculate_parents(model);
	
		// read bitmaps

	if (load_bitmaps) model_textures_read(model);
	if (setup_glowmaps) model_textures_setup_glowmaps(model);
	
		// read shaders
		
	if (load_shaders) model_shaders_read(model);
	
	return(TRUE);
}

/* =======================================================

      Save Model
      
======================================================= */

bool model_save(model_type *model,bool save_bitmaps)
{
	bool			ok,ok2,ok3;
	
		// write XML

    ok=write_mesh_xml(model);
    ok2=write_pose_xml(model);
    ok3=write_animate_xml(model);
	
		// write bitmaps
		
    if (save_bitmaps) model_textures_write(model);
	
	return(ok||ok2||ok3);
}

/* =======================================================

      Close Model
      
======================================================= */

void model_close(model_type *model)
{
	int				n;
	
	model_shaders_close(model);
	model_textures_close(model);
	
	for (n=0;n!=model->nmesh;n++) {
		free(model->meshes[n].vertexes);
		free(model->meshes[n].trigs);
		free(model->meshes[n].materials);
		free(model->meshes[n].vertex_sel_mask);
		free(model->meshes[n].vertex_hide_mask);
		free(model->meshes[n].gl_vertex_array);
		free(model->meshes[n].gl_color_array);
		free(model->meshes[n].gl_vertex_normal_array);
		free(model->meshes[n].gl_light_normal_array);
	}

	free(model->light.spots);
	
	free(model->bones);
	free(model->poses);
	free(model->animates);
	free(model->textures);
	free(model->hit_boxes);
}

/* =======================================================

      Refresh Model Textures
      
======================================================= */

void model_refresh_textures(model_type *model)
{
	model_textures_close(model);
	model_textures_read(model);
}
