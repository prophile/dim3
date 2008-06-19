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

int							cur_bind_txt_id[8];
float						cur_alpha,cur_dark_factor;
GLfloat						cur_decal_alpha[4];
bitmap_type					null_bitmap;

// supergumba -- will need to search this for deletes on unused stuff (note vars above, also)

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

      Texture Binding
      
======================================================= */

void gl_texture_bind_start(void)
{
	int					n;

		// remember each texture bind so we don't re-bind if already set

	for (n=0;n!=8;n++) {
		cur_bind_txt_id[n]=-1;
	}
}

inline void gl_texture_bind(int unit,int txt_id)
{
		// NOP if texture is already bound to this unit

	if (cur_bind_txt_id[unit]==txt_id) return;

		// bind to unit
		// GL_TEXTURE0+unit works for the OpenGL header values

	glActiveTexture(GL_TEXTURE0+unit);
	glBindTexture(GL_TEXTURE_2D,txt_id);

	cur_bind_txt_id[unit]=txt_id;
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
	gl_texture_bind(0,txt_id);
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
		// add dot3 product with normal distance factor
		// for hardness boost
	
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);

	gl_texture_bind(1,null_bitmap.gl_id);				// texture is not used in this unit

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_ADD);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);

		// texture unit 2
		// modulate dot3 with texture

	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_SUBTRACT);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_ONE_MINUS_SRC_COLOR);

	
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
}

void gl_texture_opaque_bump_end(void)
{
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_opaque_bump_set(int txt_id,int bump_id,float *normal,float normal_dist_factor)
{
	GLfloat				col4[4];

		// textures

	gl_texture_bind(0,bump_id);
	gl_texture_bind(2,txt_id);

		// bump normal

	col4[0]=normal[0];
	col4[1]=normal[1];
	col4[2]=normal[2];
	col4[3]=1.0f;

	glActiveTexture(GL_TEXTURE0);
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);

		// bump distance modulation
		// switch so the farther away the lights, the less bumping
		// and the closer the more extreme shadows
		
		// we want values from 0.0f (hardest bumping) to 0.3f
		// (softest bumping)
		
	col4[0]=col4[1]=col4[2]=((1.0f-normal_dist_factor)*0.3f);
	col4[3]=1.0f;

	glActiveTexture(GL_TEXTURE1);
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);
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

	if (setup.ray_trace_lighting) return;

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	gl_texture_bind(0,null_bitmap.gl_id);				// texture is not used in this unit
	
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
		
		// default dark factor
			
	dark_fct[0]=dark_fct[1]=dark_fct[2]=dark_fct[3]=1.0f;
	
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,dark_fct);
}

void gl_texture_tesseled_lighting_end(void)
{
	if (setup.ray_trace_lighting) return;

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_tesseled_lighting_set(int alpha_mask_id,float dark_factor)
{
	GLfloat			dark_fct[4];

		// using an alpha mask?

	if (alpha_mask_id!=-1) gl_texture_bind(0,alpha_mask_id);

		// darkness factor
		// ignore if ray tracing is on

	if (!setup.ray_trace_lighting) {

		dark_fct[0]=dark_fct[1]=dark_fct[2]=dark_factor;
		dark_fct[3]=1.0f;

		glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,dark_fct);
	}
}

/* =======================================================

      Tesseled Specular Drawing
      
======================================================= */

void gl_texture_tesseled_specular_start(void)
{
		// texture unit 0
		// the lighting multiplied by lighting to
		// get exponitial drop off
		
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	gl_texture_bind(0,null_bitmap.gl_id);				// texture is not used in this unit
	
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
		// the specular texture combined with the modulated lighting

	glActiveTexture(GL_TEXTURE1);
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

void gl_texture_tesseled_specular_end(void)
{
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_tesseled_specular_set(int specular_id)
{
	gl_texture_bind(1,specular_id);
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
	
	gl_texture_bind(1,null_bitmap.gl_id);				// texture is not used in this unit

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

inline void gl_texture_opaque_glow_set(int txt_id,int glow_id,float glow_color)
{
	GLfloat			col4[4];

	// glow texture

	gl_texture_bind(0,glow_id);
	gl_texture_bind(2,txt_id);

		// glow color

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

		// texture
	
	gl_texture_bind(0,txt_id);
	
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

	gl_texture_bind(0,txt_id);
	
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
	
	gl_texture_bind(0,null_bitmap.gl_id);				// texture is not used in this unit
	
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
		// the specular texture combined with the modulated lighting

	glActiveTexture(GL_TEXTURE1);
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
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
}

inline void gl_texture_transparent_specular_set(int specular_id,float alpha)
{
	GLfloat		col4[4];

	gl_texture_bind(1,specular_id);

	col4[0]=col4[1]=col4[2]=1;
	col4[3]=alpha;
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
	
	gl_texture_bind(1,null_bitmap.gl_id);				// texture is not used in this unit

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

inline void gl_texture_transparent_glow_set(int txt_id,int glow_id,float alpha,float glow_color)
{
	GLfloat			col4[4];

		// the glow texture and color
		
	gl_texture_bind(0,glow_id);

	glActiveTexture(GL_TEXTURE0);
		
	col4[0]=col4[1]=col4[2]=glow_color;
	col4[3]=1.0f;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);

		// the alpha

	glActiveTexture(GL_TEXTURE1);
		
	col4[0]=col4[1]=col4[2]=1.0f;
	col4[3]=alpha;
	glTexEnvfv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_COLOR,col4);
	
		// the original texture
		
	gl_texture_bind(2,txt_id);
}

/* =======================================================

      Shader Texture Drawing
      
======================================================= */

void gl_texture_shader_start(void)
{
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
		// set textures to all replace
		
	if (render_info.texture_unit_count>=4) {
	
		glActiveTexture(GL_TEXTURE3);
		
		if (glow_id!=-1) {
			glEnable(GL_TEXTURE_2D);
			gl_texture_bind(3,glow_id);
			
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
			gl_texture_bind(2,specular_id);
			
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
			gl_texture_bind(1,bump_id);
			
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
		gl_texture_bind(0,txt_id);
		
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

