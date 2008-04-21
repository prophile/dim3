/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto-Generate Map Routines

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
	int					n,k,x,z,y,intensity,lt_type;
	float				r,g,b;
	double				dx,dz;
	portal_type			*portal;
	map_light_type		*lit;
	
	if (!ag_settings.lights) return;

	portal=map->portals;
	
	for (n=0;n!=map->nportal;n++) {

		if (map->nlight>=max_map_light) break;
	
			// find light position
			
		x=(portal->ex-portal->x)/2;
		z=(portal->ez-portal->z)/2;
		y=portal->ty+((portal->by-portal->ty)/2);
		
			// add in the fudge
			
		if (ag_settings.light_fudge_factor>=2) {
			x+=(map_auto_generate_random_int(ag_settings.light_fudge_factor/2)-ag_settings.light_fudge_factor);
			z+=(map_auto_generate_random_int(ag_settings.light_fudge_factor/2)-ag_settings.light_fudge_factor);
		}
		
			// get intensity
			
		dx=(portal->ex-portal->x);
		dz=(portal->ez-portal->z);
		intensity=(int)sqrt((dx*dx)+(dz*dz));
		
		intensity=(intensity*ag_settings.light_fill_percentage)/200;			// radius, so use half
		if (intensity<100) intensity=100;
		
			// get the color
			
		r=g=b=1.0f;
		
		if (ag_settings.light_color_percentage!=100) {
			
			k=ag_settings.light_color_percentage+map_auto_generate_random_int(100-ag_settings.light_color_percentage);
			
			switch (map_auto_generate_random_int(3)) {
				case 0:
					g=(g*((float)k))/100.0f;
					b=(b*((float)k))/100.0f;
					break;
				case 1:
					r=(r*((float)k))/100.0f;
					b=(b*((float)k))/100.0f;
					break;
				case 2:
					r=(r*((float)k))/100.0f;
					g=(g*((float)k))/100.0f;
					break;
			}
		}
		
			// get the type
			
		lt_type=lt_normal;
		
		if (ag_settings.light_flicker_percentage>map_auto_generate_random_int(100)) {
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
		
			// create the light
	
		lit=&map->lights[map->nlight];
		map->nlight++;
		
		lit->pos.rn=n;
		lit->pos.x=x;
		lit->pos.z=z;
		lit->pos.y=y;
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


