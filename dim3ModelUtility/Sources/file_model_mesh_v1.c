/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Mesh File v1

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

extern char					deform_mode_str[][32],model_bump_mode_str[][32];

extern model_tag			bone_parent_tag[max_model_bone],
                            major_bone_tag[max_model_vertex],
							minor_bone_tag[max_model_vertex];

/* =======================================================

      Read Mesh v1 XML
      
======================================================= */

void decode_mesh_v1_xml(model_type *model,int model_head)
{
	int						i,n,k,bone_idx,nbone,nfill,ntrig,frame_count,
							tag,hit_box_tag,vertex_tag,bone_tag,vtag,trig_tag,image_tag,
							fills_tag,fill_tag;
	model_hit_box_type		*hit_box;
    model_vertex_type		*vertex;
	model_material_type		*material;
    model_bone_type			*bone;
    model_trig_type			*trig;
    texture_type			*texture;
	
        // options
    
    tag=xml_findfirstchild("Options",model_head);
    if (tag!=-1) {
		model->deform_mode=xml_get_attribute_list(tag,"deform",(char*)deform_mode_str);
	}
	
        // center
    
    tag=xml_findfirstchild("Center",model_head);
    if (tag!=-1) {
		xml_get_attribute_3_coord_int(tag,"offset",&model->center.x,&model->center.y,&model->center.z);
    }
    
        // boxes
    
    tag=xml_findfirstchild("Shadow_Box",model_head);
    if (tag!=-1) {
        xml_get_attribute_3_coord_int(tag,"size",&model->shadow_box.size.x,&model->shadow_box.size.y,&model->shadow_box.size.z);
		xml_get_attribute_3_coord_int(tag,"offset",&model->shadow_box.offset.x,&model->shadow_box.offset.y,&model->shadow_box.offset.z);
    }
    
    tag=xml_findfirstchild("View_Box",model_head);
    if (tag!=-1) {
        xml_get_attribute_3_coord_int(tag,"size",&model->view_box.size.x,&model->view_box.size.y,&model->view_box.size.z);
        xml_get_attribute_3_coord_int(tag,"offset",&model->view_box.offset.x,&model->view_box.offset.y,&model->view_box.offset.z);
    }
	
        // light
        
    tag=xml_findfirstchild("Light",model_head);
    if (tag!=-1) {
        model->tags.light_bone_tag[0]=xml_get_attribute_model_tag(tag,"light_bone");
        model->tags.halo_bone_tag[0]=xml_get_attribute_model_tag(tag,"halo_bone");
    }
	
        // hit boxes
    
    hit_box_tag=xml_findfirstchild("Hit_Boxes",model_head);
	if (hit_box_tag!=-1) {
	
		model->nhit_box=xml_countchildren(hit_box_tag);
		tag=xml_findfirstchild("Hit_Box",hit_box_tag);
		
		hit_box=model->hit_boxes;
		
		for (i=0;i!=model->nhit_box;i++) {
			xml_get_attribute_text(tag,"name",hit_box->name,64);
			xml_get_attribute_3_coord_int(tag,"size",&hit_box->box.size.x,&hit_box->box.size.y,&hit_box->box.size.z);
			xml_get_attribute_3_coord_int(tag,"offset",&hit_box->box.offset.x,&hit_box->box.offset.y,&hit_box->box.offset.z);
			
			hit_box++;
			tag=xml_findnextchild(tag);
		}
    }
	else {
		model->nhit_box=0;
	}
	
		// single mesh
		
	model->nmesh=1;
	strcpy(model->meshes[0].name,"Default");
	
        // vertexes
    
    vertex_tag=xml_findfirstchild("Vertexes",model_head);

    model->meshes[0].nvertex=xml_countchildren(vertex_tag);
	tag=xml_findfirstchild("v",vertex_tag);
	
    vertex=model->meshes[0].vertexes;
    
    for (i=0;i!=model->meshes[0].nvertex;i++) {
        xml_get_attribute_3_coord_int(tag,"c3",&vertex->pnt.x,&vertex->pnt.y,&vertex->pnt.z);
        xml_get_attribute_3_coord_float(tag,"n3",&vertex->normal.x,&vertex->normal.y,&vertex->normal.z);
		
        major_bone_tag[i]=xml_get_attribute_model_tag(tag,"major");
        minor_bone_tag[i]=xml_get_attribute_model_tag(tag,"minor");
        vertex->bone_factor=xml_get_attribute_float_default(tag,"factor",1);
    
        vertex++;
		tag=xml_findnextchild(tag);
    }
    
        // bones

    bone_tag=xml_findfirstchild("Bones",model_head);

    nbone=xml_countchildren(bone_tag);
	tag=xml_findfirstchild("Bone",bone_tag);
    
    for (i=0;i!=nbone;i++) {

			// add new bone

		bone_idx=model_bone_add(model,0,0,0);
		if (bone_idx==-1) break;

			// fill in bone

		bone=&model->bones[bone_idx];

        bone->tag=xml_get_attribute_model_tag(tag,"tag");
		xml_get_attribute_text(tag,"name",bone->name,name_str_len);
        
        xml_get_attribute_3_coord_int(tag,"c3",&bone->pnt.x,&bone->pnt.y,&bone->pnt.z);
        bone_parent_tag[i]=xml_get_attribute_model_tag(tag,"parent");
    
		tag=xml_findnextchild(tag);
    }
	
		// find the light bone
		
	model->tags.light_bone_idx[0]=model_find_bone(model,model->tags.light_bone_tag[0]);
	model->tags.halo_bone_idx[0]=model_find_bone(model,model->tags.halo_bone_tag[0]);
   
		// reset the vertexes from tags to indexes
		
	vertex=model->meshes[0].vertexes;
	
	for (i=0;i!=model->meshes[0].nvertex;i++) {
		vertex->major_bone_idx=model_find_bone(model,major_bone_tag[i]);
		vertex->minor_bone_idx=model_find_bone(model,minor_bone_tag[i]);
        if ((vertex->major_bone_idx==-1) || (vertex->minor_bone_idx==-1)) vertex->bone_factor=1;
		vertex++;
	}
		
		// reset the bones from tags to indexes
		
	bone=model->bones;
	
	for (i=0;i!=model->nbone;i++) {
		bone->parent_idx=model_find_bone(model,bone_parent_tag[i]);
		bone++;
	}
    
        // fills

	bzero(model->textures,(max_model_texture*sizeof(texture_type)));

		// count total # of trigs
		
    fills_tag=xml_findfirstchild("Fills",model_head);
	
    nfill=xml_countchildren(fills_tag);
	fill_tag=xml_findfirstchild("Fill",fills_tag);
    
	ntrig=0;
	
    for (n=0;n!=nfill;n++) {
        trig_tag=xml_findfirstchild("Triangles",fill_tag);
        ntrig+=xml_countchildren(trig_tag)/3;
		fill_tag=xml_findnextchild(fill_tag);
	}

		// load the fills and trigs
		
    ntrig=0;
    trig=model->meshes[0].trigs;

	texture=model->textures;
	material=model->meshes[0].materials;
	
	fill_tag=xml_findfirstchild("Fill",fills_tag);
    
    for (n=0;n!=nfill;n++) {
		texture->animate.current_frame=0;
		texture->animate.total_wait=0;
		
		texture->bump_mode=xml_get_attribute_list(fill_tag,"bump",(char*)model_bump_mode_str);
      	texture->animate.on=xml_get_attribute_boolean(fill_tag,"animate");
		
		xml_get_attribute_text(fill_tag,"vertex_shader",texture->shader.vertex_name,file_str_len);
		xml_get_attribute_text(fill_tag,"fragment_shader",texture->shader.fragment_name,file_str_len);
         
		image_tag=xml_findfirstchild("Images",fill_tag);
			
		frame_count=xml_countchildren(image_tag);
		tag=xml_findfirstchild("Image",image_tag);
		
		for (k=0;k!=frame_count;k++) {
			texture->animate.wait[k]=xml_get_attribute_int(tag,"wait");
			texture->animate.end_tick[k]=texture->animate.total_wait=texture->animate.total_wait+texture->animate.wait[k];
			
			xml_get_attribute_text(tag,"bitmap",texture->bitmaps[k].name,64);
			xml_get_attribute_text(tag,"bumpmap",texture->bumpmaps[k].name,64);
			
			tag=xml_findnextchild(tag);
		}
        
            // fill triangles
    
        trig_tag=xml_findfirstchild("Triangles",fill_tag);
        
        material->trig_start=ntrig;
        material->trig_count=xml_countchildren(trig_tag)/3;
		
		vtag=xml_findfirstchild("v",trig_tag);
    
        for (i=0;i!=material->trig_count;i++) {
    
            for (k=0;k!=3;k++) {
                trig->v[k]=xml_get_attribute_int(vtag,"id");
				xml_get_attribute_2_coord_float(vtag,"uv",&trig->gx[k],&trig->gy[k]);

				vtag=xml_findnextchild(vtag);
            }
            
            trig++;
            ntrig++;
        }
		
		texture++;
		material++;
		
		fill_tag=xml_findnextchild(fill_tag);
    }
    
    model->meshes[0].ntrig=ntrig;
}

