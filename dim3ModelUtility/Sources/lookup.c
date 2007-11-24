/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Lookup Routines

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Model Tag Routines
      
======================================================= */

model_tag text_to_model_tag(char *str)
{
	model_tag			tag;
	
	while (strlen(str)<4) {
		strcat(str," ");
	}
	
	str[4]=0x0;
	
	memmove(&tag,str,4);
	return(tag);
}

void model_tag_to_text(model_tag tag,char *str)
{
	memmove(str,&tag,4);
	str[4]=0x0;
}

/* =======================================================

      Model XML-Tag Routines
      
======================================================= */

model_tag xml_get_attribute_model_tag(int xml_tag,char *name)
{
	char			str[16];
    
	if (!xml_get_attribute_raw(xml_tag,name,str,16)) return(model_null_tag);
	return(text_to_model_tag(str));
}

bool xml_add_attribute_model_tag(char *name,model_tag tag)
{
	char		str[16];
    
    if (tag==model_null_tag) return(TRUE);
    
	model_tag_to_text(tag,str);
	return(xml_add_attribute_text(name,str));
}

/* =======================================================

      Model Lookup Routines
      
======================================================= */

int model_find_mesh(model_type *model,char *mesh_name)
{
	int					n,nmesh;
	model_mesh_type		*mesh;
	
	nmesh=model->nmesh;
	mesh=model->meshes;
	
	for (n=0;n!=nmesh;n++) {
		if (strcmp(mesh->name,mesh_name)==0) return(n);
		mesh++;
	}
	
	return(-1);
}

int model_find_bone(model_type *model,model_tag tag)
{
	int					n,nbone;
	model_bone_type		*bone;
	
	if (tag==model_null_tag) return(-1);
	
	nbone=model->nbone;
	bone=model->bones;
	
	for (n=0;n!=nbone;n++) {
		if (bone->tag==tag) return(n);
		bone++;
	}
	
	return(-1);
}

int model_find_pose(model_type *model,char *pose_name)
{
	int					n,npose;
	model_pose_type		*pose;
	
	npose=model->npose;
	pose=model->poses;
	
	for (n=0;n!=npose;n++) {
		if (strcmp(pose->name,pose_name)==0) return(n);
		pose++;
	}
	
	return(-1);
}

int model_find_animate(model_type *model,char *animate_name)
{
	int					n,nanimate;
	model_animate_type	*animate;
	
	nanimate=model->nanimate;
	animate=model->animates;
	
	for (n=0;n!=nanimate;n++) {
		if (strcmp(animate->name,animate_name)==0) return(n);
		animate++;
	}
	
	return(-1);
}
