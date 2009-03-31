/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Draw A Model

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

#include "model.h"

extern float					ang_y;
extern bool						play_animate,model_bump_on;

/* =======================================================

      Setup Model Textures
      
======================================================= */

void model_start_texture(texture_type *texture)
{
	int			bitmap_gl_id,bumpmap_gl_id;
	GLfloat		col4[4];
	
		// blending
		
	if (texture->additive) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
	
		// get texture ids
		
	if (play_animate) {
		bitmap_gl_id=texture->frames[texture->animate.current_frame].bitmap.gl_id;
		bumpmap_gl_id=texture->frames[texture->animate.current_frame].bumpmap.gl_id;
	}
	else {
		bitmap_gl_id=texture->frames[0].bitmap.gl_id;
		bumpmap_gl_id=texture->frames[0].bumpmap.gl_id;
	}
	
		// no bump maps
		
	if ((texture->frames[0].bumpmap.gl_id==-1) || (!model_bump_on)) {
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_TEXTURE_2D);
		
        glBindTexture(GL_TEXTURE_2D,bitmap_gl_id);
		
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
		
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_REPLACE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
		
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
		
		return;
	}
	
		// dot3 bump maps in texture unit 0
		
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,bumpmap_gl_id);

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_DOT3_RGB);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	
		// fake normals from rotations
		
	col4[0]=(float)sin(ang_y*ANG_to_RAD);
	col4[1]=0;
	col4[2]=1;
	col4[3]=1;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);

		// regular texture in texture unit 1
		
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,bitmap_gl_id);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
}

void model_end_texture(texture_type *texture)
{
		// reset blending
		
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// no bump maps
		
	if (texture->frames[0].bumpmap.gl_id==-1) {
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_TEXTURE_2D);
		return;
	}
	
		// bump maps
		
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	
		// reset texture combine
		
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
}

/* =======================================================

      Draw Model
      
======================================================= */

void draw_model_material(model_type *model,int mesh_idx,texture_type *texture,model_material_type *material)
{
	int					k,trig_count;
    model_trig_type		*trig;

	trig_count=material->trig_count;
	if (trig_count==0) return;

	model_start_texture(texture);
	
	glBegin(GL_TRIANGLES);
	
	trig=&model->meshes[mesh_idx].trigs[material->trig_start];

	for (k=0;k!=trig_count;k++) {
	
		if (!vertex_check_hide_mask_trig(mesh_idx,trig)) {
			glTexCoord2f(trig->gx[0],trig->gy[0]);
			glMultiTexCoord2f(GL_TEXTURE1,trig->gx[0],trig->gy[0]);
			glArrayElement(trig->v[0]);
			glTexCoord2f(trig->gx[1],trig->gy[1]);
			glMultiTexCoord2f(GL_TEXTURE1,trig->gx[1],trig->gy[1]);
			glArrayElement(trig->v[1]);
			glTexCoord2f(trig->gx[2],trig->gy[2]);
			glMultiTexCoord2f(GL_TEXTURE1,trig->gx[2],trig->gy[2]);
			glArrayElement(trig->v[2]);
		}
		
		trig++;
	}
	
	glEnd();
	
	model_end_texture(texture);
}

void draw_model(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int						n;
    texture_type			*texture;
	model_material_type		*material;
	model_mesh_type			*mesh;
	
		// model vertexes and normal arrays
		
	mesh=&model->meshes[mesh_idx];
	
	glVertexPointer(3,GL_FLOAT,0,mesh->draw.gl_vertex_array);
	glNormalPointer(GL_FLOAT,0,mesh->draw.gl_vertex_normal_array);
		
	glLockArraysEXT(0,mesh->nvertex);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
		// setup the current texture frames
		
	if (play_animate) {
		model_setup_animated_textures(model,NULL,time_get());
	}
	
	glColor4f(1,1,1,1);
    
        // run through the opaque textures

	glDisable(GL_BLEND);

    texture=model->textures;
	material=mesh->materials;
    
    for (n=0;n!=max_model_texture;n++) {
		if (texture->frames[0].bitmap.alpha_mode!=alpha_mode_transparent) draw_model_material(model,mesh_idx,texture,material);
		texture++;
		material++;
	}
	
		// run through the transparent textures
		
	glEnable(GL_BLEND);

	glDepthMask(GL_FALSE);
	
	texture=model->textures;
	material=mesh->materials;
    
    for (n=0;n!=max_model_texture;n++) {
		if (texture->frames[0].bitmap.alpha_mode==alpha_mode_transparent) draw_model_material(model,mesh_idx,texture,material);
		texture++;
		material++;
	}
	
	glDepthMask(GL_TRUE);
    
	glDisable(GL_ALPHA_TEST);
	
	glUnlockArraysEXT();
	
	glFlush();
}

/* =======================================================

      Draw Faded Model
      
======================================================= */

void draw_model_faded(model_type *model,int mesh_idx,model_draw_setup *draw_setup)
{
	int						n;
    texture_type			*texture;
	model_material_type		*material;
	model_mesh_type			*mesh;
		
		// model vertexes and normal arrays
		
	mesh=&model->meshes[mesh_idx];
	
	glVertexPointer(3,GL_FLOAT,0,mesh->draw.gl_vertex_array);
	glNormalPointer(GL_FLOAT,0,mesh->draw.gl_vertex_normal_array);
		
	glLockArraysEXT(0,mesh->nvertex);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
		// setup the current texture frames
		
	if (play_animate) {
		model_setup_animated_textures(model,NULL,time_get());
	}
	
	glColor4f(1,1,1,0.25);
    
        // all textures are transparent

	glDepthMask(GL_FALSE);
	
    texture=model->textures;
	material=mesh->materials;
    
    for (n=0;n!=max_model_texture;n++) {
		draw_model_material(model,mesh_idx,texture,material);
		texture++;
		material++;
	}
	
	glDepthMask(GL_TRUE);
	
 	glDisable(GL_ALPHA_TEST);
   
	glUnlockArraysEXT();
	
	glFlush();
}

