/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Fog

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

#include "consoles.h"
#include "video.h"

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Textured Fog
      
======================================================= */

void fog_draw_textured(int tick)
{
	int					n,count,outer_radius,inner_radius,
						high,drop,radius_add,radius,frame;
	unsigned long		gl_id;
	float				txt_x_off,txt_x_turn,txt_x_off_add,
						txt_x_fact,txt_y_fact;
	d3ang				ang;
	texture_type		*texture;
	GLUquadricObj		*quadric;
	
		// textured fog on?
		
	if ((!map.fog.on) || (map.fog.use_solid_color)) return;

		// setup viewpoint
		
	ang.x=angle_add(view.camera.ang.x,90.0f);
	ang.y=ang.z=0;

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&ang);
	gl_setup_project();
	
		// translate to correct height

	high=map.fog.high;
	drop=map.fog.drop;
		
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(0.0f,0.0f,-(float)(high-drop));

		// setup drawing
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

		// drawing layers
	
	count=map.fog.count;
	outer_radius=map.fog.outer_radius;
	inner_radius=map.fog.inner_radius;

	radius_add=(inner_radius-outer_radius)/count;

		// textured or solid color

	txt_x_fact=map.fog.txt_x_fact;						// need to declare these outside to avoid warning
	txt_y_fact=map.fog.txt_y_fact;
	
	txt_x_off=((float)(tick>>7))*map.fog.speed;
	txt_x_turn=(txt_x_fact*(view.camera.ang.y/360.0f));			// change texture offset with camera rotation
	txt_x_off_add=1.0f/(float)count;

	glMatrixMode(GL_TEXTURE);

	texture=&map.textures[map.fog.texture_idx];
	frame=texture->animate.current_frame&max_texture_frame_mask;
	gl_id=texture->bitmaps[frame].gl_id;

	gl_texture_simple_start();
	gl_texture_simple_set(gl_id,FALSE,1.0f,1.0f,1.0f,map.fog.alpha);

		// draw obscuring cylinders

	quadric=gluNewQuadric();
	gluQuadricNormals(quadric,GLU_NONE);
	gluQuadricTexture(quadric,GL_TRUE);
	
	radius=outer_radius;
	
	for (n=0;n!=count;n++) {

		glLoadIdentity();

		if ((n&0x1)==0x0) {
			glTranslatef((txt_x_turn+txt_x_off),0.0f,0.0f);
		}
		else {
			glTranslatef((txt_x_turn-txt_x_off),0.0f,0.0f);
		}
		txt_x_off+=txt_x_off_add;

		glScalef(txt_x_fact,txt_y_fact,1.0f);

		gluCylinder(quadric,radius,radius,high,16,1);
		radius+=radius_add;
	}
	
	gluDeleteQuadric(quadric);

		// turn off texture

	gl_texture_simple_end();

	glLoadIdentity();
}

/* =======================================================

      Solid Fog
      
======================================================= */

inline bool fog_solid_on(void)
{
	return((map.fog.on) && (map.fog.use_solid_color));
}

void fog_solid_start(void)
{
	GLfloat		col[4];

	glEnable(GL_FOG);

	col[0]=map.fog.col.r;
	col[1]=map.fog.col.g;
	col[2]=map.fog.col.b;
	col[3]=1.0f;

	glFogfv(GL_FOG_COLOR,col);
	glFogi(GL_FOG_MODE,GL_LINEAR);
	glFogi(GL_FOG_START,map.fog.inner_radius);
	glFogi(GL_FOG_END,map.fog.outer_radius);
}

void fog_solid_end(void)
{
	glDisable(GL_FOG);
}

