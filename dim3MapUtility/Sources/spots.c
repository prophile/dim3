/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Spot Routines

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

/* =======================================================

      Find spots
      
======================================================= */

int map_count_spot(map_type *map,char *name,char *type)
{
	int				n,nspot,count;
	spot_type		*spot;
	
	count=0;
	nspot=map->nspot;
	
	for (n=0;n!=nspot;n++) {
		spot=&map->spots[n];
		
		if (name!=NULL) if (strcmp(spot->name,name)!=0) continue;
		if (type!=NULL) if (strcmp(spot->type,type)!=0) continue;
		
		count++;
	}
	
	return(count);
}

int map_find_spot(map_type *map,char *name,char *type)
{
	int				n,nspot;
	spot_type		*spot;
	
	nspot=map->nspot;
	
	for (n=0;n!=nspot;n++) {
		spot=&map->spots[n];
		
		if (name!=NULL) if (strcmp(spot->name,name)!=0) continue;
		if (type!=NULL) if (strcmp(spot->type,type)!=0) continue;
		
		return(n);
	}
	
	return(-1);
}

/* =======================================================

      Random Spot Find
      
======================================================= */

void map_find_random_spot_clear(map_type *map,char *name,char *type)
{
	int				n,nspot;
	spot_type		*spot;
	
	nspot=map->nspot;
	
	for (n=0;n!=nspot;n++) {
		spot=&map->spots[n];
		
		if (name!=NULL) if (strcmp(spot->name,name)!=0) continue;
		if (type!=NULL) if (strcmp(spot->type,type)!=0) continue;

		spot->random_hit=FALSE;
	}
}

int map_find_random_spot(map_type *map,char *name,char *type)
{
	int				n,nspot,count,spot_count,free_spot_count;
	spot_type		*spot;
	
		// count the spots
		
	spot_count=0;
	free_spot_count=0;
	
	nspot=map->nspot;

	for (n=0;n!=nspot;n++) {
		spot=&map->spots[n];
		
		if (name!=NULL) if (strcmp(spot->name,name)!=0) continue;
		if (type!=NULL) if (strcmp(spot->type,type)!=0) continue;
		
		spot_count++;
		if (!spot->random_hit) free_spot_count++;
	}
	
	if (spot_count==0) return(-1);
	
		// do we need to clear the round robin flags?
		
	if (free_spot_count==0) {
		map_find_random_spot_clear(map,name,type);
		free_spot_count=spot_count;
	}
	
		// find random number in count
		
	count=random_int(free_spot_count);
	
	for (n=0;n!=nspot;n++) {
		spot=&map->spots[n];
		if (spot->random_hit) continue;
		
		if (name!=NULL) if (strcmp(spot->name,name)!=0) continue;
		if (type!=NULL) if (strcmp(spot->type,type)!=0) continue;
		
		if (count==0) {
			spot->random_hit=TRUE;
			return(n);
		}
		
		count--;
	}
	
	return(-1);
}

/* =======================================================

      Clear Attaches
      
======================================================= */

void map_spot_clear_attach(map_type *map)
{
	int				i;
	spot_type		*spot;
	
	spot=map->spots;
	
	for (i=0;i!=map->nspot;i++) {
		spot->attach=FALSE;
		spot++;
	}
}

/* =======================================================

      Attach Object to Spot
      
======================================================= */

void map_spot_attach_object(spot_type *spot,char *name,char *type,char *script,char *params)
{
	strcpy(spot->attach_name,name);
	strcpy(spot->attach_type,type);
	strcpy(spot->attach_script,script);
	strcpy(spot->attach_params,params);
	
	spot->attach=TRUE;
}

