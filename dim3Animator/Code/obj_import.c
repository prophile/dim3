/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model OBJ importer

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

#include "dialog.h"
#include "import.h"

#define obj_max_face_vertex		128

extern int						cur_mesh;
extern model_type				model;

float							obj_gx[max_model_vertex],obj_gy[max_model_vertex];

/* =======================================================

      Import OBJ File
      
======================================================= */

bool import_obj(char *path,bool *found_normals,char *err_str)
{
	int						i,k,t,nvertex,ntrig,nobj_uv,nobj_normal,nline,ntexture,texture_idx,
							pvtx[obj_max_face_vertex],npt;
	char					txt[256],*c,vstr[256],vtstr[256],material_name[256];
    float					*gx,*gy,pgx[obj_max_face_vertex],pgy[obj_max_face_vertex];
	bool					single_material,first_material;
	model_vertex_type		*vertex,*normal_vertex;
    model_trig_type			*trig;
	model_material_type		*material;
		
		// clear mesh materials
    
    clear_materials();
	
		// load the file
		
	if (!textdecode_open(path,0x20)) {
		strcpy(err_str,"Couldn't open file.");
		return(FALSE);
    }
	
    nline=textdecode_count();
	
		// count materials
		
	ntexture=0;
	
    for (i=0;i!=nline;i++) {
        textdecode_get_piece(i,0,txt);
        if (strcmp(txt,"usemtl")==0) ntexture++;
	}

		// can't import if no textures
		
	if (ntexture==0) {
		textdecode_close();
		strcpy(err_str,"OBJ contains no materials.  Animator can only import texture-mapped OBJs.");
		return(FALSE);
	}
	
		// can't import if too many materials?
		
	if ((ntexture+texture_count())>=max_model_texture) {
		textdecode_close();
		sprintf(err_str,"Too many materials, models can have a maximum of %d materials.",max_model_texture);
		return(FALSE);
	}
	
		// get the vertex and uv

    nvertex=0;
	vertex=model.meshes[cur_mesh].vertexes;
    
    nobj_uv=0;
    gx=obj_gx;
    gy=obj_gy;
	
	nobj_normal=0;
	normal_vertex=model.meshes[cur_mesh].vertexes;
   
    for ((i=0);(i!=nline);i++) {

        textdecode_get_piece(i,0,txt);
        
            // a vertex
            
        if (strcmp(txt,"v")==0) {
            if (nvertex>=max_model_vertex) {
				textdecode_close();
				sprintf(err_str,"Too many vertexes, models can have a maximum of %d vertexes.",max_model_vertex);
				return(FALSE);
			}
        
			textdecode_get_piece(i,1,txt);
			vertex->pnt.x=-(int)(strtod(txt,NULL)*import_scale_factor);
			textdecode_get_piece(i,2,txt);
			vertex->pnt.y=-(int)(strtod(txt,NULL)*import_scale_factor);
			textdecode_get_piece(i,3,txt);
			vertex->pnt.z=-(int)(strtod(txt,NULL)*import_scale_factor);
            
            vertex->major_bone_idx=vertex->minor_bone_idx=-1;
            vertex->bone_factor=1;
        
            vertex++;
            nvertex++;
        }
        else {
        
            // a uv
            
            if (strcmp(txt,"vt")==0) {
                if (nobj_uv>=max_model_vertex) continue;
            
                textdecode_get_piece(i,1,txt);
                *gx++=strtod(txt,NULL);
                textdecode_get_piece(i,2,txt);
                *gy++=strtod(txt,NULL);
                
                nobj_uv++;
            }
			else {
			
				// a normal
				
				if (strcmp(txt,"vn")==0) {
					if (nobj_normal>=max_model_vertex) continue;
				
					textdecode_get_piece(i,1,txt);
					normal_vertex->normal.x=-strtod(txt,NULL);
					textdecode_get_piece(i,2,txt);
					normal_vertex->normal.y=-strtod(txt,NULL);
					textdecode_get_piece(i,2,txt);
					normal_vertex->normal.z=-strtod(txt,NULL);
					
					normal_vertex++;
					nobj_normal++;
				}
			}
        }
    }
	
		// can't import if no UVs
		
	if (nobj_uv==0) {
		textdecode_close();
		sprintf(err_str,"There are no UVs in this OBJ, please texture map the model before importing.");
		return(FALSE);
	}

		// set new vertexes
    
    model.meshes[cur_mesh].nvertex=nvertex;
	
		// single material import?
		
	single_material=FALSE;
	
	if (ntexture>1) {
		single_material=texture_use_single();
	}

		// get the triangles
		
	*found_normals=(nobj_normal!=0);

	first_material=TRUE;
	material=NULL;
    
    ntrig=0;
	trig=model.meshes[cur_mesh].trigs;
        
    for (i=0;i!=nline;i++) {

        textdecode_get_piece(i,0,txt);
        
            // material change
            
        if (strcmp(txt,"usemtl")==0) {
		
				// single material?
				
			if ((!first_material) && (single_material)) continue;
            
                // close last material
            
            if (!first_material) {
				material->trig_count=ntrig-material->trig_start;
            }
			
			first_material=FALSE;
            
                // start new material
             
            textdecode_get_piece(i,1,material_name);
            texture_idx=texture_pick(material_name,err_str);
			if (texture_idx==-1) {
				textdecode_close();
				return(FALSE);
			}
            
			material=&model.meshes[cur_mesh].materials[texture_idx];
            material->trig_start=ntrig;
            
            continue;
        }

            // a face
            
        if (strcmp(txt,"f")!=0) continue;
        
            // get the face points
        
        npt=0;
        
        for (k=0;k!=obj_max_face_vertex;k++) {
            textdecode_get_piece(i,(k+1),txt);
            if (txt[0]==0x0) break;
            
            vtstr[0]=0x0;
            
            strcpy(vstr,txt);
            c=strchr(vstr,'/');
            if (c!=NULL) {
                strcpy(vtstr,(c+1));
                *c=0x0;
            }
            c=strchr(vtstr,'/');
            if (c!=NULL) *c=0x0;
            
            pvtx[npt]=atoi(vstr)-1;
            
			if (vtstr[0]==0x0) {
				pgx[npt]=pgy[npt]=0.0f;
			}
			else {
				t=atoi(vtstr)-1;
				pgx[npt]=obj_gx[t];
				pgy[npt]=1-obj_gy[t];
            }
			
            npt++;
        }
		
        for (k=0;k!=(npt-2);k++) {
            if (ntrig>=max_model_trig) {
				textdecode_close();
				sprintf(err_str,"Too many triangles, models can have a maximum of %d triangles.",max_model_trig);
				return(FALSE);
			}

            trig->v[0]=pvtx[0];
            trig->v[1]=pvtx[k+1];
            trig->v[2]=pvtx[k+2];
                
            trig->gx[0]=pgx[0];
            trig->gx[1]=pgx[k+1];
            trig->gx[2]=pgx[k+2];
            
            trig->gy[0]=pgy[0];
            trig->gy[1]=pgy[k+1];
            trig->gy[2]=pgy[k+2];
            
            trig++;
            ntrig++;
        }
    }
    
    if (!first_material) {
        material->trig_count=ntrig-material->trig_start;
    }
	
	model.meshes[cur_mesh].ntrig=ntrig;
	
	textdecode_close();

	return(TRUE);
}

