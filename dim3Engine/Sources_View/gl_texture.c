/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Texture Routines

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

#include "video.h"

extern map_type				map;
extern server_type			server;
extern setup_type			setup;
extern render_info_type		render_info;

int							cur_txt_id,cur_bump_id,cur_specular_id,cur_glow_id;
float						cur_alpha,cur_dark_factor;
GLfloat						cur_decal_alpha[4];
bitmap_type					null_bitmap;

/* =======================================================

      Texture Utility Initialization
      
======================================================= */

void gl_texture_initialize(void)
{
	d3col			col;
	
	col.r=col.b=col.g=0.0f;
	bitmap_color(&null_bitmap,"[null]",&col,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE);
}

void gl_texture_shutdown(void)
{
	bitmap_close(&null_bitmap);
}

/* =======================================================

      Opaque Texture Drawing
      
======================================================= */

void gl_texture_opaque_start(void)
{
		// texture unit 0
		// contains texture
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
}

void gl_texture_opaque_end(void)
{
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_opaque_set(int txt_id)
{
	glBindTexture(GL_TEXTURE_2D,txt_id);
}

/* =======================================================

      Opaque Lighted Texture Drawing
      
======================================================= */

void gl_texture_opaque_lighting_start(void)
{
	GLfloat				dark_fct[4];
	
		// texture unit 0
		// contains texture modulated with primary colors
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	
		// texture unit 1
		// contains texture modulated with
		// darkness factor

		// this step is ignored for simplified rendering as older
		// renderers seem to have a problem with constant colors

	if ((setup.segment_darken) && (!setup.ray_trace_lighting)) {

		glActiveTexture(GL_TEXTURE1);
		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D,null_bitmap.gl_id);				// texture is not used in this unit

		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_CONSTANT);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);
		
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
		
			// default dark factor
			
		dark_fct[0]=dark_fct[1]=dark_fct[2]=dark_fct[3]=1.0f;
	
		glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,dark_fct);
	}
}

void gl_texture_opaque_lighting_end(void)
{
	if ((setup.segment_darken) && (!setup.ray_trace_lighting)) {
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
	}

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_opaque_lighting_set(int txt_id)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,txt_id);
}

inline void gl_texture_opaque_lighting_factor(float dark_factor)
{
	GLfloat				dark_fct[4];

	if (!((setup.segment_darken) && (!setup.ray_trace_lighting))) return;

	dark_fct[0]=dark_fct[1]=dark_fct[2]=dark_factor;
	dark_fct[3]=1.0f;
	
	glActiveTexture(GL_TEXTURE1);
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,dark_fct);
}

/* =======================================================

      Opaque Bump Texture Drawing
      
======================================================= */

void gl_texture_opaque_bump_start(void)
{
		// texture unit 0
		// dot3 product between normal and bump map
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_DOT3_RGB);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);

		// texture unit 1
		// contains bump mapping modulated with texture

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
}

void gl_texture_opaque_bump_end(void)
{
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_opaque_bump_set(int txt_id,int bump_id)
{
		// the bump

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,bump_id);

		// the texture

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,txt_id);
}

inline void gl_texture_opaque_bump_factor(float *normal)
{
	GLfloat				dot3[4];

		// bump normal

	dot3[0]=normal[0];
	dot3[1]=normal[1];
	dot3[2]=normal[2];
	dot3[3]=1.0f;

	glActiveTexture(GL_TEXTURE0);
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,dot3);
}

/* =======================================================

      Opaque Bump Lighted Texture Drawing
      
======================================================= */

void gl_texture_opaque_bump_lighting_start(void)
{
		// texture unit 0
		// dot3 product between normal and bump map
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_DOT3_RGB);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);

		// texture unit 1
		// contains bump map modulated with texture

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);

		// texture unit 2
		// contains bump+texture modulated with lighting

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D,null_bitmap.gl_id);				// texture is not used in this unit

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
}

void gl_texture_opaque_bump_lighting_end(void)
{
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_opaque_bump_lighting_set(int txt_id,int bump_id)
{
		// the bump
		
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,bump_id);

		// the texture

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,txt_id);
}

inline void gl_texture_opaque_bump_lighting_factor(float *normal)
{
	GLfloat			dot3[4];

	dot3[0]=normal[0];
	dot3[1]=normal[1];
	dot3[2]=normal[2];
	dot3[3]=1.0f;

	glActiveTexture(GL_TEXTURE0);
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,dot3);
}

/* =======================================================

      Tesseled Lighting Drawing
      
======================================================= */

void gl_texture_tesseled_lighting_start(void)
{
	GLfloat			dark_fct[4];
	
		// texture unit 0
		// this contains the lighting color modulated
		// with the darkness factor.
		//
		// this step is skipped if simplified rendering
		// as simplified renders seem to have a problem with
		// the constant color

	if (!((setup.segment_darken) && (!setup.ray_trace_lighting))) return;

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,null_bitmap.gl_id);				// texture is not used in this unit
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
		
		// default dark factor
			
	dark_fct[0]=dark_fct[1]=dark_fct[2]=dark_fct[3]=1.0f;
	
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,dark_fct);
}

void gl_texture_tesseled_lighting_end(void)
{
	if (!((setup.segment_darken) && (!setup.ray_trace_lighting))) return;

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_tesseled_lighting_factor(float dark_factor)
{
	GLfloat			dark_fct[4];

	if (!((setup.segment_darken) && (!setup.ray_trace_lighting))) return;

	dark_fct[0]=dark_fct[1]=dark_fct[2]=dark_factor;
	dark_fct[3]=1.0f;

	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,dark_fct);
}

/* =======================================================

      Opaque Specular Texture Drawing
      
======================================================= */

void gl_texture_opaque_specular_start(void)
{
		// texture unit 0
		// the lighting multiplied by lighting to
		// get exponitial drop off
		
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,null_bitmap.gl_id);				// texture is not used in this unit
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);

		// texture unit 1
		// lighting added to specular factor

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,null_bitmap.gl_id);				// texture is not used in this unit
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_ADD);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
		
		// texture unit 2
		// the specular texture combined with the modulated lighting

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
}

void gl_texture_opaque_specular_end(void)
{
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_opaque_specular_set(int specular_id)
{
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,specular_id);
}

inline void gl_texture_opaque_specular_factor(float specular_factor)
{
	GLfloat		fct[4];

	glActiveTexture(GL_TEXTURE1);
	
	fct[0]=fct[1]=fct[2]=specular_factor;
	fct[3]=1.0f;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,fct);
}

/* =======================================================

      Opaque Glow Texture Drawing
      
======================================================= */

void gl_texture_opaque_glow_start(void)
{
		// texture unit 0
		// the glow map modulated with glow constant

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	
		// texture unit 1
		// the modulated glow color added with lighting

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,null_bitmap.gl_id);				// texture is not used in this unit

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_ADD);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	
		// texture unit 2
		// the lit glow modulated with texture
	
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
}

void gl_texture_opaque_glow_end(void)
{
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_opaque_glow_set(int txt_id,int glow_id)
{

		// the glow texture
		
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,glow_id);
	
		// the original texture
		
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,txt_id);
}

inline void gl_texture_opaque_glow_color(float glow_color)
{
	GLfloat			col4[4];

	glActiveTexture(GL_TEXTURE0);

	col4[0]=col4[1]=col4[2]=glow_color;
	col4[3]=1.0f;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);
}

/* =======================================================

      Decal Texture Drawing
      
======================================================= */

void gl_texture_decal_start(void)
{
	cur_txt_id=-1;
	cur_alpha=-1.0f;

		// texture unit 0
		// contains texture

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA,GL_SRC_ALPHA);
}

void gl_texture_decal_end(void)
{
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_decal_set(int txt_id,float alpha)
{
	GLfloat			alpha_fct[4];

		// texture already set?
		
	if (cur_txt_id!=txt_id) {
		cur_txt_id=txt_id;
		glBindTexture(GL_TEXTURE_2D,txt_id);
	}
	
		// alpha already set?
	 
	if (cur_alpha!=alpha) {
		cur_alpha=alpha;

		alpha_fct[0]=alpha_fct[1]=alpha_fct[2]=1.0f;
		alpha_fct[3]=alpha;

		glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,alpha_fct);
	}
}

/* =======================================================

      Transparent Texture Drawing
      
======================================================= */

void gl_texture_transparent_start(void)
{
		// texture unit 0
		// contains texture

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA,GL_SRC_ALPHA);
}

void gl_texture_transparent_end(void)
{
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_transparent_set(int txt_id,float alpha)
{
	GLfloat			col4[4];
	
		// set texture and alpha

	glBindTexture(GL_TEXTURE_2D,txt_id);
	
	col4[0]=col4[1]=col4[2]=1;
	col4[3]=alpha;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);
}

/* =======================================================

      Transparent Specular Texture Drawing
      
======================================================= */

void gl_texture_transparent_specular_start(void)
{
		// texture unit 0
		// the lighting multiplied by lighting to
		// get exponitial drop off
		
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,null_bitmap.gl_id);				// texture is not used in this unit
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	
		// texture unit 1
		// lighting added to specular factor
		
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,null_bitmap.gl_id);				// texture is not used in this unit
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_ADD);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
		
		// texture unit 2
		// the specular texture combined with the modulated lighting

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA,GL_SRC_ALPHA);
}

void gl_texture_transparent_specular_end(void)
{
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_transparent_specular_set(int specular_id,float alpha)
{
	GLfloat		col4[4];

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,specular_id);

	col4[0]=col4[1]=col4[2]=1;
	col4[3]=alpha;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);
}

inline void gl_texture_transparent_specular_factor(float specular_factor)
{
	GLfloat		col4[4];

	glActiveTexture(GL_TEXTURE1);

	col4[0]=col4[1]=col4[2]=specular_factor;
	col4[3]=1.0f;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);
}

/* =======================================================

      Transparent Glow Texture Drawing
      
======================================================= */

void gl_texture_transparent_glow_start(void)
{
		// texture unit 0
		// the glow map modulated with glow constant

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	
		// texture unit 1
		// the modulated glow color added with lighting

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,null_bitmap.gl_id);				// texture is not used in this unit

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_ADD);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA,GL_SRC_ALPHA);
	
		// texture unit 2
		// the lit glow modulated with texture
	
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
}

void gl_texture_transparent_glow_end(void)
{
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_transparent_glow_set(int txt_id,int glow_id,float alpha)
{
	GLfloat			col4[4];

		// the glow texture
		
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,glow_id);

		// the alpha

	glActiveTexture(GL_TEXTURE1);
		
	col4[0]=col4[1]=col4[2]=1.0f;
	col4[3]=alpha;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);
	
		// the original texture
		
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,txt_id);
}

inline void gl_texture_transparent_glow_color(float glow_color)
{
	GLfloat			col4[4];

	glActiveTexture(GL_TEXTURE0);
		
	col4[0]=col4[1]=col4[2]=glow_color;
	col4[3]=1.0f;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);
}

/* =======================================================

      Shader Texture Drawing
      
======================================================= */

void gl_texture_shader_start(void)
{
	cur_txt_id=-1;
	cur_bump_id=-1;
	cur_specular_id=-1;
	cur_glow_id=-1;
}

void gl_texture_shader_end(void)
{
	if (render_info.texture_unit_count>=4) {
		glActiveTexture(GL_TEXTURE3);
		glDisable(GL_TEXTURE_2D);
	}
	
	if (render_info.texture_unit_count>=3) {
		glActiveTexture(GL_TEXTURE2);
		glDisable(GL_TEXTURE_2D);
	}
	
	if (render_info.texture_unit_count>=2) {
		glActiveTexture(GL_TEXTURE1);
		glDisable(GL_TEXTURE_2D);
	}
	
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

void gl_texture_shader_set(int txt_id,int bump_id,int specular_id,int glow_id)
{
		// already set?
		
	if ((cur_txt_id==txt_id) && (cur_bump_id==bump_id) && (cur_specular_id==specular_id) && (cur_glow_id==glow_id)) return;
	
	cur_txt_id=txt_id;
	cur_bump_id=bump_id;
	cur_specular_id=specular_id;
	cur_glow_id=glow_id;

		// set textures to all replace
		
	if (render_info.texture_unit_count>=4) {
	
		glActiveTexture(GL_TEXTURE3);
		
		if (glow_id!=-1) {	
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,glow_id);
			
			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
			
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
				
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
		}
		else {
			glDisable(GL_TEXTURE_2D);
		}
	}
	
	if (render_info.texture_unit_count>=3) {
	
		glActiveTexture(GL_TEXTURE2);
		
		if (specular_id!=-1) {	
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,specular_id);
			
			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
			
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
				
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
		}
		else {
			glDisable(GL_TEXTURE_2D);
		}
	}
	
	if (render_info.texture_unit_count>=2) {

		glActiveTexture(GL_TEXTURE1);
		
		if (bump_id!=-1) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,bump_id);
			
			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
			
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
				
			glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
			glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
			glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
		}
		else {
			glDisable(GL_TEXTURE_2D);
		}
	}
	
	glActiveTexture(GL_TEXTURE0);
		
	if (txt_id!=-1) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,txt_id);
		
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
		
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_REPLACE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
			
		glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
		glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}
}

/* =======================================================

      Simple Texture Drawing
      
======================================================= */

void gl_texture_simple_start(void)
{
		// preset texture unit 0
		// it simply modultes the texture with the preset color/alpha
		
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA,GL_SRC_ALPHA);
}

void gl_texture_simple_end(void)
{
		// reset any possible clamping
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	
	glDisable(GL_TEXTURE_2D);
}

void gl_texture_simple_set(int txt_id,bool clamp,float r,float g,float b,float alpha)
{
	GLfloat			col4[4];
	
		// set the texture
		
	glBindTexture(GL_TEXTURE_2D,txt_id);
	
		// set the clamping
		
	if (clamp) {
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	}
	
		// put the constants in environment color
		
	col4[0]=r;
	col4[1]=g;
	col4[2]=b;
	col4[3]=alpha;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);
}

/* =======================================================

      Movie Texture Drawing
      
======================================================= */

void gl_texture_movie_start(int txt_id)
{
		// setup the texture
		
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
		// movie texture
		
	glBindTexture(GL_TEXTURE_2D,txt_id);
	
		// texture combines
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
		
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
		
		// clamp to single texture
		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
}

void gl_texture_movie_end(void)
{
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	
	glDisable(GL_TEXTURE_2D);
}

