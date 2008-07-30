/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Rain

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
extern view_type			view;
extern setup_type			setup;

int							rain_last_tick,rain_slant_add,rain_slant_next_add,
							rain_slant_next_start_tick,rain_slant_next_end_tick;
float						rain_slant_ang_y,rain_slant_next_ang_y;

/* =======================================================

      Setup Rain
      
======================================================= */

void rain_setup_single(rain_draw_type *rain_draw,int cx,int cy,int cz)
{
	rain_draw->x_off=random_int_spread(map.rain.radius);
	rain_draw->z_off=random_int_spread(map.rain.radius);

	rain_draw->x=cx+rain_draw->x_off;
	rain_draw->z=cz+rain_draw->z_off;

	rain_draw->y=cy+random_int_spread(map.rain.height);
	rain_draw->by=cy+map.rain.height;
}

void rain_setup_single_reset(rain_draw_type *rain_draw,int cx,int cy,int cz)
{
	rain_draw->x=cx+rain_draw->x_off;
	rain_draw->z=cz+rain_draw->z_off;
	rain_draw->y=(cy-map.rain.height)+((rain_draw->y-rain_draw->by)%(map.rain.height<<1));

	rain_draw->by=cy+map.rain.height;
}

void rain_setup_next_slant(int tick)
{
	if (map.rain.slant_time_msec==0) return;
	
	rain_slant_next_add=random_int(map.rain.slant_add);
	rain_slant_next_ang_y=random_float(360);

	rain_slant_next_start_tick=tick+random_int(map.rain.slant_time_msec);
	rain_slant_next_end_tick=rain_slant_next_start_tick+map.rain.slant_change_msec;
}

void rain_setup(int tick,int cx,int cy,int cz)
{
	int				n;
	rain_draw_type	*rain_draw;

	rain_draw=view.rain_draws;

	for (n=0;n!=max_rain_density;n++) {
		rain_setup_single(rain_draw,cx,cy,cz);
		rain_draw++;
	}
	
	rain_last_tick=tick;
	
	rain_slant_add=random_int(map.rain.slant_add);
	rain_slant_ang_y=random_float(360);
	
	rain_setup_next_slant(tick);
}

/* =======================================================

      Draw Rain
      
======================================================= */

void rain_draw(int tick)
{
	int				n,x,y,z,cx,cy,cz,xadd,yadd,zadd,density,
					slant_add,slant_mult,slant_div;
	float			slant_ang_y;
	rain_draw_type	*rain_draw;

		// is rain on and not under liquid?

	if (!map.rain.on) return;
	if (view.camera.under_liquid_idx!=-1) return;
	
	cx=view.camera.pnt.x;
	cy=view.camera.pnt.y;
	cz=view.camera.pnt.z;
	
		// reset on?
		
	if (map.rain.reset) {
		map.rain.reset=FALSE;
		rain_setup(tick,cx,cy,cz);
	}
	
		// rain slant
		
	slant_add=rain_slant_add;
	slant_ang_y=rain_slant_ang_y;
		
	if (map.rain.slant_time_msec!=0) {
		
			// time to change slant?
			
		if (tick>rain_slant_next_end_tick) {
			rain_slant_add=slant_add=rain_slant_next_add;
			rain_slant_ang_y=slant_ang_y=rain_slant_next_ang_y;
			rain_setup_next_slant(tick);
		}
		else {
		
				// slant in the middle of changing
				
			if (tick>rain_slant_next_start_tick) {
				slant_mult=tick-rain_slant_next_start_tick;
				slant_div=(rain_slant_next_end_tick-rain_slant_next_start_tick);
				
				slant_add=rain_slant_add+(((rain_slant_next_add-rain_slant_add)*slant_mult)/slant_div);
				slant_ang_y=rain_slant_ang_y+((rain_slant_next_ang_y-rain_slant_ang_y)*((float)slant_mult/(float)slant_div));
			}
			
		}
	}

	angle_get_movement(slant_ang_y,slant_add,&xadd,&zadd);

		// rain change

	xadd=(tick-rain_last_tick)*xadd;
	yadd=(tick-rain_last_tick)*map.rain.speed;
	zadd=(tick-rain_last_tick)*zadd;
	
	rain_last_tick=tick;

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.ang);
	gl_setup_project();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

		// rain lines

	rain_draw=view.rain_draws;

	glLineWidth((float)map.rain.line_width);
	glBegin(GL_LINES);
	
	density=map.rain.density;
	if (density>max_rain_density) density=max_rain_density;

	for (n=0;n!=density;n++) {

			// move rain

		rain_draw->x+=xadd;
		rain_draw->y+=yadd;
		rain_draw->z+=zadd;
		
		if (rain_draw->y>rain_draw->by) rain_setup_single_reset(rain_draw,cx,cy,cz);

			// draw rain

		x=rain_draw->x-view.camera.pnt.x;
		y=rain_draw->y-view.camera.pnt.y;
		z=view.camera.pnt.z-rain_draw->z;

		glColor4f(map.rain.start_color.r,map.rain.start_color.g,map.rain.start_color.b,map.rain.alpha);
		glVertex3i(x,y,z);

		x=(rain_draw->x-view.camera.pnt.x)+xadd;
		y=(rain_draw->y+map.rain.line_length)-view.camera.pnt.y;
		z=(view.camera.pnt.z-rain_draw->z)+zadd;

		glColor4f(map.rain.end_color.r,map.rain.end_color.g,map.rain.end_color.b,map.rain.alpha);
		glVertex3i(x,y,z);

		rain_draw++;
	}

	glEnd();
	glLineWidth(1);
}

