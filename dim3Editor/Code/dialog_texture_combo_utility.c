/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Dialog Texture Combos

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

#include "interface.h"
#include "dialog.h"

extern map_type				map;

/* =======================================================

      Dialog Map Texture Combo Utilities
      
======================================================= */

void dialog_fill_texture_combo(WindowRef wind,unsigned long sig,int id,bool none,int idx)
{
	int					n;
	texture_type		*texture;
	
		// clear combo
		
	dialog_clear_combo(wind,sig,id);
	
		// none items
		
	if (none) {
		dialog_add_combo_item(wind,sig,id,"None",0);
		dialog_add_combo_item(wind,sig,id,"-",0);
	}
	
		// textures
		
	texture=map.textures;
	
	for (n=0;n!=max_map_texture;n++) {
	
		if (texture->bitmaps[0].data==NULL) {
			dialog_add_combo_item(wind,sig,id,"(no texture)",0);
		}
		else {
			dialog_add_combo_item(wind,sig,id,texture->bitmaps[0].name,0);
		}
		
		texture++;
	}
	
	if (idx==-1) {
		dialog_set_combo(wind,sig,id,0);
	}
	else {
		if (!none) {
			dialog_set_combo(wind,sig,id,idx);
		}
		else {
			dialog_set_combo(wind,sig,id,(idx+2));
		}
	}
}

int dialog_get_texture_combo(WindowRef wind,unsigned long sig,int id,bool none)
{
	int			idx;
	
	idx=dialog_get_combo(wind,sig,id);
	
	if (!none) return(idx);
	
	if (idx==0) return(-1);
	return(idx-2);
}

