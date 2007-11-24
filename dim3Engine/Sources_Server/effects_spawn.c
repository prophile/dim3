/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Effect Spawing

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

#include "lights.h"
#include "effects.h"

extern map_type				map;
extern server_type			server;

/* =======================================================

      Flash Effects
      
======================================================= */

bool effect_spawn_flash(d3pnt *pt,d3col *col,int intensity,int flash_msec,int fade_msec)
{
	effect_type			*effect;
	flash_effect_data	*flash;

	effect=effect_spawn(ef_flash,pt,(flash_msec+fade_msec));
	if (effect==NULL) return(FALSE);

	flash=&effect->data.flash;
	memmove(&flash->col,col,sizeof(d3col));
	flash->intensity=intensity;
	flash->mid_tick=flash_msec;
	
	effect->size=0;
	
	return(TRUE);
}

/* =======================================================

      Lightning Effects
      
======================================================= */

bool effect_spawn_lightning(int tick,d3pnt *start_pt,d3pnt *end_pt,int wid,float varient,d3col *col,int life_msec)
{
	effect_type				*effect;
	lightning_effect_data	*lightning;

	effect=effect_spawn(ef_lightning,start_pt,life_msec);
	if (effect==NULL) return(FALSE);
	
	lightning=&effect->data.lightning;
	
	memmove(&lightning->start_pnt,start_pt,sizeof(d3pnt));
	memmove(&lightning->end_pnt,end_pt,sizeof(d3pnt));
	
	lightning->wid=wid;
	lightning->line_offset=tick&0xF;
	lightning->varient=varient;
	
	memmove(&lightning->col,col,sizeof(d3col));
	
	effect->size=(abs(end_pt->x-start_pt->x)+abs(end_pt->z-start_pt->z))>>2;

	return(TRUE);
}

/* =======================================================

      Ray Effects
      
======================================================= */

bool effect_spawn_ray(d3pnt *start_pt,d3pnt *end_pt,int wid,d3col *col,int life_msec)
{
	effect_type				*effect;
	ray_effect_data			*ray;

	effect=effect_spawn(ef_ray,start_pt,life_msec);
	if (effect==NULL) return(FALSE);
	
	ray=&effect->data.ray;
	
	memmove(&ray->start_pnt,start_pt,sizeof(d3pnt));
	memmove(&ray->end_pnt,end_pt,sizeof(d3pnt));
	
	ray->wid=wid;
	
	memmove(&ray->col,col,sizeof(d3col));
	
	effect->size=(abs(end_pt->x-start_pt->x)+abs(end_pt->z-start_pt->z))>>2;

	return(TRUE);
}

/* =======================================================

      Globe Effects
      
======================================================= */

bool effect_spawn_globe(d3pnt *pt,int start_sz,int end_sz,float start_alpha,float end_alpha,d3col *col,int life_msec)
{
	effect_type				*effect;
	globe_effect_data		*globe;

	effect=effect_spawn(ef_globe,pt,life_msec);
	if (effect==NULL) return(FALSE);
	
	globe=&effect->data.globe;
	
	globe->start_size=start_sz;
	globe->end_size=end_sz;
	
	memmove(&globe->col,col,sizeof(d3col));
	
	globe->start_alpha=start_alpha;
	globe->end_alpha=end_alpha;

	if (start_sz>end_sz) {
		effect->size=start_sz;
	}
	else {
		effect->size=end_sz;
	}
	
	return(TRUE);
}

/* =======================================================

      Shake Effects
      
======================================================= */

bool effect_spawn_shake(d3pnt *pt,int distance,int size,int life_msec)
{
	effect_type			*effect;

	effect=effect_spawn(ef_shake,pt,life_msec);
	if (effect==NULL) return(FALSE);
	
	effect->data.shake.distance=distance;
	effect->size=size;
	
	return(TRUE);
}

