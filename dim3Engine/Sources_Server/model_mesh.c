/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Meshes

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

#include "models.h"

extern server_type				server;

extern int game_time_get(void);

/* =======================================================

      Model Meshes
      
======================================================= */

int model_find_mesh_from_draw(model_draw *draw,char *name)
{
	model_type					*mdl;
	
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return(-1);
	
	return(model_find_mesh(mdl,name));
}

bool model_show_mesh(model_draw *draw,char *name)
{
	int							idx;
	
	idx=model_find_mesh_from_draw(draw,name);
	if (idx==-1) return(FALSE);
	
	draw->mesh_mask|=(0x1<<idx);
    
	return(TRUE);
}

bool model_show_all_mesh(model_draw *draw)
{
	draw->mesh_mask=0xFFFF;
    
	return(TRUE);
}

bool model_show_only_mesh(model_draw *draw,char *name)
{
	int							idx;
	
	idx=model_find_mesh_from_draw(draw,name);
	if (idx==-1) return(FALSE);
	
	draw->mesh_mask=(0x1<<idx);
    
	return(TRUE);
}

bool model_hide_mesh(model_draw *draw,char *name)
{
	int							idx;
	
	idx=model_find_mesh_from_draw(draw,name);
	if (idx==-1) return(FALSE);
	
	draw->mesh_mask&=((0x1<<idx)^0xFF);
    
	return(TRUE);
}

bool model_hide_all_mesh(model_draw *draw)
{
	draw->mesh_mask=0x0;
    
	return(TRUE);
}
