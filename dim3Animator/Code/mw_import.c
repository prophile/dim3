/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Meshwork Importer

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

#include "import.h"

model_tag						bone_parent_tag[max_model_bone],
								major_bone_tag[max_model_vertex],minor_bone_tag[max_model_vertex];

extern int						cur_mesh;
extern model_type				model;

/* =======================================================

      Import Colored Material
      
======================================================= */

bool import_meshwork_color_trigs(int material_start,int material_end)
{
	int					i,ntrig;
	float				r,g,b;
	char				txt[256];
	model_trig_type		*trig;
	
	ntrig=model.meshes[cur_mesh].ntrig;
	
		// material settings
		
	textdecode_get_piece(material_start,1,txt);
	r=strtod(txt,NULL)/65535;
	textdecode_get_piece(material_start,2,txt);
	g=strtod(txt,NULL)/65535;
	textdecode_get_piece(material_start,3,txt);
	b=strtod(txt,NULL)/65535;

		// material triangles
	
	trig=&model.meshes[cur_mesh].trigs[ntrig];
	
	for ((i=(material_start+1));(i!=material_end);i++) {
	
		if (ntrig>=max_model_trig) return(FALSE);
		
		textdecode_get_piece(i,0,txt);
		trig->v[0]=(short)atoi(txt);
		textdecode_get_piece(i,1,txt);
		trig->v[1]=(short)atoi(txt);
		textdecode_get_piece(i,2,txt);
		trig->v[2]=(short)atoi(txt);
		
		ntrig++;
		trig++;
	}
	
	model.meshes[cur_mesh].ntrig=ntrig;
	
	return(TRUE);
}

/* =======================================================

      Import Textured Material
      
======================================================= */

bool import_meshwork_texture_trigs(int material_start,int material_end,int uv_start)
{
	int					i,k,n,q,
						ntrig,ntxt_start,uv_end;
	float				gx,gy;
	char				txt[256];
	model_trig_type		*trig;
	
	ntrig=model.meshes[cur_mesh].ntrig;

		// find uv's
		
	uv_end=textdecode_find((uv_start+1),"MATERIAL");
	if (uv_end==-1) {
		uv_end=textdecode_find((uv_start+1),"END");
	}
	if (uv_end==-1) return(TRUE);
	
	ntxt_start=ntrig;
	trig=&model.meshes[cur_mesh].trigs[ntrig];
	
		// material triangles
	
	for ((i=(material_start+1));(i!=material_end);i++) {
	
		if (ntrig>=max_model_trig) return(FALSE);
		
		textdecode_get_piece(i,0,txt);
		trig->v[0]=(short)atoi(txt);
		textdecode_get_piece(i,1,txt);
		trig->v[1]=(short)atoi(txt);
		textdecode_get_piece(i,2,txt);
		trig->v[2]=(short)atoi(txt);
		
		trig->gx[0]=trig->gx[1]=trig->gx[2]=0;
		trig->gy[0]=trig->gy[1]=trig->gy[2]=0;
		
		ntrig++;
		trig++;
	}
	
		// material uvs
	
	for ((i=(uv_start+1));(i!=uv_end);i++) {
	
		textdecode_get_piece(i,0,txt);
		k=atoi(txt);
		
		textdecode_get_piece(i,1,txt);
		gx=strtod(txt,NULL);
		textdecode_get_piece(i,2,txt);
		gy=1-strtod(txt,NULL);
		
		trig=&model.meshes[cur_mesh].trigs[ntxt_start];
		
		for ((n=ntxt_start);(n!=ntrig);n++) {		// find vertex in triangle list
			for ((q=0);(q!=3);q++) {
				if (trig->v[q]==k) {
					trig->gx[q]=gx;
					trig->gy[q]=gy;
				}
			}
			trig++;
		}
	}
	
	model.meshes[cur_mesh].ntrig=ntrig;
	
	return(TRUE);
}

/* =======================================================

      Import Meshwork Texture Material
      
======================================================= */

bool import_meshwork_has_texture(int pict_idx,char *path)
{
	short				curfile,tfile;
	bool				has_texture;
	Handle				pic;
	FSRef				fref;
	
	curfile=CurResFile();

	FSPathMakeRef((unsigned char*)path,&fref,NULL);
	tfile=FSOpenResFile(&fref,fsRdPerm);
	UseResFile(tfile);
	
	has_texture=FALSE;
	
	pic=Get1Resource('PICT',pict_idx);
	if (pic!=NULL) {
		has_texture=TRUE;
		ReleaseResource(pic);
	}
	
	UseResFile(curfile);
	CloseResFile(tfile);
	
	return(has_texture);
}

void import_meshwork_texture_material(int n,int pict_idx,char *path)
{
	int					xsz,ysz;
	short				curfile,tfile;
	ptr					txtptr;
	PicHandle			pic;
    texture_type		*texture;
	FSRef				fref;
	
    texture=&model.textures[n];
	
	curfile=CurResFile();

	FSPathMakeRef((unsigned char*)path,&fref,NULL);
	tfile=FSOpenResFile(&fref,fsRdPerm);
	UseResFile(tfile);
	
	pic=(PicHandle)Get1Resource('PICT',pict_idx);
	if (pic!=NULL) {
	
		pic_get_size(pic,&xsz,&ysz);
		txtptr=pic_to_memory(pic,xsz,ysz);
		
        sprintf(texture->bitmaps[0].name,"Image_%d_0",n);
		texture->bitmaps[0].wid=xsz;
		texture->bitmaps[0].high=ysz;
		texture->bitmaps[0].data=rgb_to_opengl(xsz,ysz,txtptr);
		texture->bumpmaps[0].data=NULL;
        
        texture->animate.on=FALSE;

		free(txtptr);
		ReleaseResource((Handle)pic);
	}
	
	UseResFile(curfile);
	CloseResFile(tfile);
}

/* =======================================================

      Import Meshwork Texture Color
      
======================================================= */

void import_meshwork_color_material(int n,int material_start)
{
    float				r,g,b;
    char				txt[256];
	unsigned char		*uc;
    texture_type		*texture;

    textdecode_get_piece(material_start,1,txt);
    r=strtod(txt,NULL)/0xFFFF;
    textdecode_get_piece(material_start,2,txt);
    g=strtod(txt,NULL)/0xFFFF;
    textdecode_get_piece(material_start,3,txt);
    b=strtod(txt,NULL)/0xFFFF;
	
    texture=&model.textures[n];
	
		// turn color into texture
		
	sprintf(texture->bitmaps[0].name,"Image_%d_0",n);
	texture->bitmaps[0].wid=1;
	texture->bitmaps[0].high=1;
	
	texture->bitmaps[0].data=valloc(4);
	
	uc=texture->bitmaps[0].data;
	
	*uc++=(unsigned char)(r*0xFF);
	*uc++=(unsigned char)(g*0xFF);
	*uc++=(unsigned char)(b*0xFF);
	*uc=0xFF;
	
	texture->bumpmaps[0].data=NULL;
	
		// fill in rest of pieces

	texture->animate.on=FALSE;
}

/* =======================================================

      Import Meshwork File
      
======================================================= */

bool import_meshwork(char *path,char *err_str)
{
	int					i,nvertex,nbone,
						texture_idx,pict_idx,vertices_start,edges_start,
						material_start,material_end,uv_start;
	char				txt[256];
	bool				bad_trig;
	model_vertex_type	*vertex;
	model_bone_type		*bone;
	
		// clear materials
    
	clear_materials();
    	
		// load the file
		
	if (!textdecode_open(path,0x9)) {
		strcpy(err_str,"Couldn't open file.");
		return(FALSE);
    }
	
		// get vertices
		
	vertices_start=textdecode_find(0,"VERTICES");
	edges_start=textdecode_find(vertices_start,"EDGES");
	if ((vertices_start==-1) || (edges_start==-1)) {
		return(FALSE);
	}
	
	nvertex=nbone=0;
	vertex=model.meshes[cur_mesh].vertexes;
	bone=model.bones;
	
	for ((i=(vertices_start+1));(i!=edges_start);i++) {

		textdecode_get_piece(i,7,txt);
		
		if (nvertex>=max_model_vertex) {
			textdecode_close();
			sprintf(err_str,"Too many vertexes, models can have a maximum of %d vertexes.",max_model_vertex);
			return(FALSE);
		}
		
			// bone
			
		if (txt[0]!=0x0) {
			
			if (nbone>=max_model_bone) {
				textdecode_close();
				sprintf(err_str,"Too many bones, models can have a maximum of %d bones.",max_model_bone);
				return(FALSE);
			}
			
			bone_parent_tag[nbone]=text_to_model_tag(txt);
			
			textdecode_get_piece(i,1,txt);
			bone->pnt.x=strtod(txt,NULL)*10;
			textdecode_get_piece(i,2,txt);
			bone->pnt.y=-strtod(txt,NULL)*10;
			textdecode_get_piece(i,3,txt);
			bone->pnt.z=-strtod(txt,NULL)*10;
			
			textdecode_get_piece(i,4,txt);
			bone->tag=text_to_model_tag(txt);
			
			bone++;
			nbone++;

            vertex->pnt.x=vertex->pnt.z=vertex->pnt.y=0;				// bones count as vertexes in meshwork
            major_bone_tag[nvertex]=minor_bone_tag[nvertex]=FOUR_CHAR_CODE('None');
            vertex->bone_factor=1;
            vertex++;
            nvertex++;
            
            continue;
		}
		
			// vertex
			
		textdecode_get_piece(i,1,txt);
		vertex->pnt.x=strtod(txt,NULL)*10;
		textdecode_get_piece(i,2,txt);
		vertex->pnt.y=-strtod(txt,NULL)*10;
		textdecode_get_piece(i,3,txt);
		vertex->pnt.z=-strtod(txt,NULL)*10;
		
		textdecode_get_piece(i,4,txt);
		major_bone_tag[nvertex]=text_to_model_tag(txt);
		textdecode_get_piece(i,5,txt);
		minor_bone_tag[nvertex]=text_to_model_tag(txt);
		
		textdecode_get_piece(i,6,txt);
		vertex->bone_factor=((float)atoi(txt))/100;
		
		vertex++;
		nvertex++;
	}
	
	model.meshes[cur_mesh].nvertex=nvertex;
	model.nbone=nbone;
	
		// get the triangles

	pict_idx=128;
	material_end=edges_start;
	
	while (TRUE) {
		material_start=textdecode_find(material_end,"MATERIAL");
		if (material_start==-1) break;
		
		material_end=textdecode_find((material_start+1),"MATERIAL");
		if (material_end==-1) {
			material_end=textdecode_find((material_start+1),"END");
		}
		if (material_end==-1) break;

		uv_start=textdecode_find((material_start+1),"UVS");		// uvs in this material?
		if (uv_start!=-1) {
			if (uv_start<material_end) {
				material_end=uv_start;
			}
			else {
				uv_start=-1;			// uvs in another material
			}
		}
		
			// no triangles in this material?
			
		if ((material_start+1)>=material_end) {
			pict_idx++;
			continue;
		}
		
			// grab the triangles

		texture_idx=texture_find_free();
        model.meshes[cur_mesh].materials[texture_idx].trig_start=model.meshes[cur_mesh].ntrig;
		
		bad_trig=FALSE;
		
		if (!import_meshwork_has_texture(pict_idx,path)) {
		
				// if no texture, import as color
				
            import_meshwork_color_material(texture_idx,material_start);
			bad_trig=!import_meshwork_color_trigs(material_start,material_end);
		}
		else {
				
				// import as texture
				
			if (uv_start==-1) {				// textures must be pinned
				textdecode_close();
				sprintf(err_str,"Material #%d is not pinned, can only import models with pinned textures.",(pict_idx-127));
				return(FALSE);
			}

			import_meshwork_texture_material(texture_idx,pict_idx,path);
			bad_trig=!import_meshwork_texture_trigs(material_start,material_end,uv_start);
		}
		
		if (bad_trig) {
			textdecode_close();
			sprintf(err_str,"Too many triangles, models can have a maximum of %d triangles.",max_model_trig);
			return(FALSE);
		}
        
        model.meshes[cur_mesh].materials[texture_idx].trig_count=model.meshes[cur_mesh].ntrig-model.meshes[cur_mesh].materials[texture_idx].trig_start;
		
		pict_idx++;
	}

	textdecode_close();
	
		// reset the vertexes from tags to indexes
		
	vertex=model.meshes[cur_mesh].vertexes;
	
	for ((i=0);(i!=nvertex);i++) {
		vertex->major_bone_idx=model_find_bone(&model,major_bone_tag[i]);
		vertex->minor_bone_idx=model_find_bone(&model,minor_bone_tag[i]);
		vertex++;
	}
		
		// reset the bones from tags to indexes
		
	bone=model.bones;
	
	for ((i=0);(i!=nbone);i++) {
		bone->parent_idx=model_find_bone(&model,bone_parent_tag[i]);
		bone++;
	}
		
	return(TRUE);
}

