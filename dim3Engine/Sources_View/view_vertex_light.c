/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Portal Vertex Lists

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

extern map_type			map;
extern view_type		view;

d3col					light_trace_base_col;

extern bool light_trace_map(d3pnt *spt,d3pnt *ept);

/* =======================================================

      Ray Traced Lighting
      
======================================================= */

void light_trace_set_base_color(d3col *col)
{
	light_trace_base_col.r=col->r;
	light_trace_base_col.g=col->g;
	light_trace_base_col.b=col->b;
}

void light_trace_calculate_light_color(portal_type *portal,float x,float y,float z,float *cf)
{
	int					n,nspot;
	d3pnt				spt,ept;
	double				mult,r,g,b,dx,dz,dy,d;
	light_spot_type		*lspot;

	spt.x=(int)x;
	spt.y=(int)y;
	spt.z=(int)z;

		// combine all light spot colors
		// attenuated for distance
		
	nspot=portal->light.nspot;
	lspot=portal->light.spots;
	
	r=g=b=0.0f;
	
	for (n=0;n!=nspot;n++) {

		ept.x=lspot->pnt.x;
		ept.y=lspot->pnt.y;
		ept.z=lspot->pnt.z;

		if (light_trace_map(&spt,&ept)) {
			lspot++;
			continue;
		}
		
		dx=lspot->d_x-x;
		dy=lspot->d_y-y;
		dz=lspot->d_z-z;
		
		d=(dx*dx)+(dz*dz)+(dy*dy);
		if (d<=lspot->d_intensity) {
			mult=(lspot->d_intensity-d)*lspot->d_inv_intensity;
			r+=(lspot->d_col_r*mult);
			g+=(lspot->d_col_g*mult);
			b+=(lspot->d_col_b*mult);
		}
		
		lspot++;
	}

	*cf++=light_trace_base_col.r+(float)r;
	*cf++=light_trace_base_col.g+(float)g;
	*cf=light_trace_base_col.b+(float)b;
}







