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

/* =======================================================

      Turn Fog On and Off
      
======================================================= */

void gl_fog_enable(int fog_type,float density,int near_z,int far_z,d3col *col)
{
	GLfloat			fogcolor[4];
	
	glEnable(GL_FOG);
	
	switch (fog_type) {
		case ft_linear:
			glFogf(GL_FOG_MODE,GL_LINEAR);
			break;
		case ft_exp:
			glFogf(GL_FOG_MODE,GL_EXP);
			break;
		case ft_exp2:
			glFogf(GL_FOG_MODE,GL_EXP2);
			break;
	}
	
	glFogf(GL_FOG_DENSITY,density);
	glFogi(GL_FOG_START,near_z);
	glFogi(GL_FOG_END,far_z);
	
	fogcolor[0]=col->r;
	fogcolor[1]=col->g;
	fogcolor[2]=col->b;
	fogcolor[3]=1;
	
	glFogfv(GL_FOG_COLOR,fogcolor);

	glHint(GL_FOG_HINT,GL_NICEST);
}

void gl_fog_disable(void)
{
	glDisable(GL_FOG);
}

