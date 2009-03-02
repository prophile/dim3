/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Lighting Routines

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

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern setup_type		setup;

int						nlight;
view_light_spot_type	lspot_cache[max_light_spot];

double					light_flicker_value[64]={
													1.00,1.00,1.00,1.00,1.00,1.00,1.00,1.00,
													0.92,0.85,0.70,0.85,0.92,1.00,0.92,0.85,
													0.92,1.00,1.00,1.00,1.00,0.92,0.85,0.70,
													0.50,0.70,0.85,0.70,0.85,0.92,1.00,1.00,
													1.00,1.00,1.00,1.00,0.92,0.85,0.70,0.85,
													0.70,0.85,0.92,1.00,1.00,0.92,0.85,0.70,
													0.50,0.70,0.85,0.92,1.00,1.00,0.92,1.00,
													0.92,0.85,0.70,0.85,0.92,1.00,1.00,1.00
												},
						light_fail_value[64]=	{
													0.00,0.00,0.00,0.00,0.00,0.00,0.00,0.00,
													0.00,0.15,0.45,0.15,0.00,0.00,0.00,0.15,
													0.00,0.00,0.00,0.00,0.00,0.00,0.15,0.45,
													0.85,0.45,0.15,0.45,0.15,0.00,0.00,0.00,
													0.00,0.00,0.00,0.00,0.00,0.15,0.45,0.15,
													0.45,0.15,0.00,0.00,0.00,0.00,0.15,0.45,
													0.85,0.45,0.15,0.00,0.00,0.00,0.00,0.00,
													0.00,0.15,0.45,0.15,0.00,0.00,0.00,0.00
												};

extern int game_time_get(void);


// supergumba -- all this needs to be removed, fixed, etc

/* =======================================================

      Clear Lights
      
======================================================= */

void light_clear(void)
{
	nlight=0;
}

/* =======================================================

      Get Light Intensity For Type
      
======================================================= */

double light_get_intensity(int light_type,int intensity)
{
	int				tick;
	double			d_tick,d_intensity;
	
	tick=game_time_get();
	d_intensity=(double)intensity;
	
	switch (light_type) {
	
		case lt_normal:
			return(d_intensity);
	
		case lt_blink:
			if ((tick&0x100)==0) return(d_intensity);
			return(0);
			
		case lt_glow:
			d_tick=(double)(tick&0x7FF);
			if (d_tick>1024) d_tick=2048-d_tick;
			return(d_intensity*(0.75+(d_tick/4096)));
			
		case lt_pulse:
			d_tick=(double)(tick&0x3FF);
			if (d_tick>512) d_tick=1024-d_tick;
			return(d_intensity*(0.5+(d_tick/1024)));
			
		case lt_flicker:
			d_intensity*=light_flicker_value[(tick>>7)&0x3F];
			break;
			
		case lt_failing:
			d_intensity*=light_fail_value[(tick>>7)&0x3F];
			break;
	}
	
	return(d_intensity);
}

/* =======================================================

      Add a Light to the Light List
      
======================================================= */

void light_add(d3pnt *pnt,int light_type,int intensity,float fall_off,d3col *col)
{
	view_light_spot_type			*lspot;
	
	if (nlight==max_light_spot) return;

	lspot=&lspot_cache[nlight];
	
		// create intensity for light type
		
	lspot->intensity=light_get_intensity(light_type,intensity);
	if (lspot->intensity<=0) return;
	
		// create light position and color
		
	memmove(&lspot->pnt,pnt,sizeof(d3pnt));
	memmove(&lspot->col,col,sizeof(d3col));

		// calculate inverses

	lspot->inv_intensity=1.0/lspot->intensity;
	
	lspot->d_intensity=lspot->intensity*lspot->intensity;
	lspot->d_inv_intensity=1.0/lspot->d_intensity;
	
		// use this constant to get rough calculation close to better sqrt calc
// supergumba -- do away with this		
//	if (fall_off==0.0f) fall_off=map.ambient.light_drop_off_factor;
//	lspot->d_inv_intensity*=(double)fall_off;
	
		// double light values
		
	lspot->d_x=(double)lspot->pnt.x;
	lspot->d_y=(double)lspot->pnt.y;
	lspot->d_z=(double)lspot->pnt.z;
	
	lspot->d_col_r=(double)lspot->col.r;
	lspot->d_col_g=(double)lspot->col.g;
	lspot->d_col_b=(double)lspot->col.b;
	
		// count the lights
		
	nlight++;
}

