/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Ambient Routines

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

#include "sounds.h"

int								al_ambient_count;					
al_ambient_type					al_ambients[al_max_ambient];

extern al_source_type			al_sources[al_max_source];

#ifdef SDL_SOUND

void al_ambient_list_clear(void)
{
}	

void al_ambient_list_add(int buffer_idx,int x,int y,int z,float pitch)
{
}

void al_ambients_run(void)
{
}

#else

/* =======================================================

      Add Ambient Sounds To List
      
======================================================= */

void al_ambient_list_clear(void)
{
	al_ambient_count=0;
}	

void al_ambient_list_add(int buffer_idx,int x,int y,int z,float pitch)
{
	int					n,dist,max_dist;
	al_ambient_type		*ambient;

		// get distance to ambient
		
	dist=al_distance_to_listener(x,y,z);
	max_dist=al_get_buffer_max_dist(buffer_idx);

	if (dist>max_dist) return;
	
		// already playing this sound?

	ambient=al_ambients;
	
	for (n=0;n!=al_ambient_count;n++) {
	
		if (ambient->buffer_idx==buffer_idx) {		// replace with closer ambient

			if (dist<ambient->dist) {
				ambient->pnt.x=x;
				ambient->pnt.y=y;
				ambient->pnt.z=z;
				ambient->dist=dist;
				ambient->pitch=pitch;
			}
			
			return;
		}
	
		ambient++;
	}

		// add to list
		
	if (al_ambient_count>=al_max_ambient) return;
	
	ambient=&al_ambients[al_ambient_count];
	al_ambient_count++;
	
	ambient->buffer_idx=buffer_idx;
	ambient->pnt.x=x;
	ambient->pnt.y=y;
	ambient->pnt.z=z;
	ambient->dist=dist;
	ambient->pitch=pitch;
	
	ambient->hit=FALSE;
}

/* =======================================================

      Run Ambient Sounds
      
======================================================= */

void al_ambients_run(void)
{
	int					n,source_idx,buffer_idx;
	bool				amb_ok;
	al_ambient_type		*ambient;
	al_source_type		*source;
			
		// remove any current ambients not in the new list
		
	source_idx=-1;
	
	while (TRUE) {
		source_idx=al_get_next_source_ambient(source_idx);
		if (source_idx==-1) break;

		buffer_idx=al_sources[source_idx].buffer_idx;

			// is this sound in our ambient list?

		amb_ok=FALSE;
		ambient=al_ambients;
	
		for (n=0;n!=al_ambient_count;n++) {
		
			if (ambient->buffer_idx==buffer_idx) {
			
					// found sound in ambients
					
				ambient->hit=TRUE;
				amb_ok=TRUE;
				
					// update ambient
					
				source=&al_sources[source_idx];
				alSource3f(source->al_id,AL_POSITION,(ALfloat)ambient->pnt.x,(ALfloat)ambient->pnt.y,(ALfloat)ambient->pnt.z);
				alSourcef(source->al_id,AL_PITCH,ambient->pitch);
				
				break;
			}
			
			ambient++;
		}
		
			// sound not in our list, so kill it
			
		if (!amb_ok) al_stop_source(source_idx);
	}
	
		// add the rest as new sounds
	
	ambient=al_ambients;

	for (n=0;n!=al_ambient_count;n++) {
		if ((!ambient->hit) && (ambient->buffer_idx!=-1)) {
			al_play_source(ambient->buffer_idx,ambient->pnt.x,ambient->pnt.y,ambient->pnt.z,ambient->pitch,TRUE,TRUE,FALSE,FALSE);
		}
		ambient++;
	}
}

#endif