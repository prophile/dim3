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
	int				n,xadd,yadd,zadd,density,
					slant_add,slant_mult,slant_div;
	float			slant_ang_y;
	float			*vertex_ptr,*col_ptr;
	rain_draw_type	*rain_draw;

		// is rain on and not under liquid?

	if (!map.rain.on) return;
	if (view.camera.under_liquid_idx!=-1) return;
	
		// reset on?
		
	if (map.rain.reset) {
		map.rain.reset=FALSE;
		rain_setup(tick,view.camera.pnt.x,view.camera.pnt.y,view.camera.pnt.z);
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

		// rain density

	density=map.rain.density;
	if (density>max_rain_density) density=max_rain_density;

		// construct VBO

	vertex_ptr=view_bind_map_next_vertex_object(((density*2)*(3+4)));
	if (vertex_ptr==NULL) return;

	col_ptr=vertex_ptr+((density*2)*3);

		// create vertexes

	rain_draw=view.rain_draws;

	for (n=0;n!=density;n++) {

			// move rain

		rain_draw->x+=xadd;
		rain_draw->y+=yadd;
		rain_draw->z+=zadd;
		
		if (rain_draw->y>rain_draw->by) rain_setup_single_reset(rain_draw,view.camera.pnt.x,view.camera.pnt.y,view.camera.pnt.z);

			// draw rain

		*vertex_ptr++=(float)rain_draw->x;
		*vertex_ptr++=(float)rain_draw->y;
		*vertex_ptr++=(float)rain_draw->z;

		*col_ptr++=map.rain.start_color.r;
		*col_ptr++=map.rain.start_color.g;
		*col_ptr++=map.rain.start_color.b;
		*col_ptr++=map.rain.alpha;

		*vertex_ptr++=(float)(rain_draw->x+xadd);
		*vertex_ptr++=(float)(rain_draw->y+map.rain.line_length);
		*vertex_ptr++=(float)(rain_draw->z+zadd);

		*col_ptr++=map.rain.end_color.r;
		*col_ptr++=map.rain.end_color.g;
		*col_ptr++=map.rain.end_color.b;
		*col_ptr++=map.rain.alpha;

		rain_draw++;
	}

  	view_unmap_current_vertex_object();

		// setup view

	gl_setup_viewport(console_y_offset());
	gl_3D_view(&view.camera);
	gl_3D_rotate(&view.camera.pnt,&view.camera.ang);
	gl_setup_project();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

		// draw the rain

	glLineWidth((float)map.rain.line_width);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);
		
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_FLOAT,0,(void*)(((density*2)*3)*sizeof(float)));

	glDrawArrays(GL_LINES,0,(density*2));

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glLineWidth(1);

		// unbind the vbo

	view_unbind_current_vertex_object();
}

