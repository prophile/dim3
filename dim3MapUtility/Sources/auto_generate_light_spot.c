/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto-Generate Map Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
	#include "dim3maputility.h"
#endif

extern int map_auto_generate_random_int(int max);

extern unsigned char				corridor_flags[max_portal];

extern auto_generate_settings_type	ag_settings;

/* =======================================================

      Create Lights
      
======================================================= */

void map_auto_generate_lights(map_type *map)
{
	int					n,x,z,y,intensity,lt_type;
	float				r,g,b;
	double				dx,dz;
	portal_type			*portal;
	map_light_type		*lit;
	
	if (ag_settings.light_type_on[ag_light_type_include]==0) return;

	portal=map->portals;
	
	for (n=0;n!=map->nportal;n++) {

		if (map->nlight>=max_map_light) break;
	
			// find light position
			
		x=(portal->ex-portal->x)/2;
		z=(portal->ez-portal->z)/2;
		y=portal->ty+((portal->by-portal->ty)/2);
		
			// get intensity
			
		dx=(portal->ex-portal->x);
		dz=(portal->ez-portal->z);
		intensity=((int)sqrt((dx*dx)+(dz*dz)))>>1;			// radius, so use half
		if (intensity<100) intensity=100;
		
			// get the color
			
		r=g=b=1.0f;
		
			// tinting
			// tinted lights on in portals
			
		if ((ag_settings.light_type_on[ag_light_type_tint]!=0) && (corridor_flags[n]==ag_corridor_flag_portal)) {
			
			switch (map_auto_generate_random_int(4)) {
				case 0:
					g=b=0.4f;
					break;
				case 1:
					r=b=0.4f;
					break;
				case 2:
					r=g=0.4f;
					break;
			}
		}
		
			// get the type
			// animated lights on in rooms
			
		lt_type=lt_normal;
		
		if ((ag_settings.light_type_on[ag_light_type_animated]!=0) && (corridor_flags[n]==ag_corridor_flag_portal)) {
		
			if (map_auto_generate_random_int(100)<(int)(ag_constant_light_animate_percentage*100.0f)) {
				switch (map_auto_generate_random_int(3)) {
					case 0:
						lt_type=lt_glow;
						break;
					case 1:
						lt_type=lt_pulse;
						break;
					case 2:
						lt_type=lt_flicker;
						break;
				}
			}
		}
		
			// create the light
	
		lit=&map->lights[map->nlight];
		map->nlight++;
		
		lit->pnt.x=x;
		lit->pnt.y=y;
		lit->pnt.z=z;
		lit->type=lt_type;
		lit->col.r=r;
		lit->col.g=g;
		lit->col.b=b;
		lit->intensity=intensity;
		lit->confine_to_portal=(corridor_flags[n]!=ag_corridor_flag_portal);
		lit->on=TRUE;

		portal++;
	}
}

/* =======================================================

      Add Spots
      
======================================================= */

void map_auto_generate_spots(map_type *map)
{
/* supergumba -- fix this!

	int				n,k,x,y,z,idx;
	segment_type	*seg;
	spot_type		*spot;
	
	if (!ag_settings.spots) return;
	
	for (n=0;n!=ag_settings.spot_count;n++) {
		idx=map_auto_generate_rough_floor_find_floor(map,TRUE);
		seg=&map->segments[idx];
		
			// get middle
			
		x=y=z=0;
		
		for (k=0;k!=seg->data.fc.ptsz;k++) {
			x+=seg->data.fc.x[k];
			y+=seg->data.fc.y[k];
			z+=seg->data.fc.z[k];
		}
		
		x/=seg->data.fc.ptsz;
		y/=seg->data.fc.ptsz;
		z/=seg->data.fc.ptsz;
		
			// create spot
			
		if (map->nspot>max_spot) return;
		
		spot=&map->spots[map->nspot];
		
		spot->pos.rn=seg->rn;
		spot->pos.x=x;
		spot->pos.z=z;
		spot->pos.y=y;
		spot->ang.y=0;
		sprintf(spot->name,"Spot %d",map->nspot);
		strcpy(spot->type,"Spot");
		spot->script[0]=0x0;
		spot->display_model[0]=0x0;
		spot->params[0]=0x0;
		
		map->nspot++;
	}
	*/
}


