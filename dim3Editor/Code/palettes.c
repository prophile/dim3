/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Editor Palette

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

#include "interface.h"
#include "dialog.h"
#include "common_view.h"

#define palette_type_none				0
#define palette_type_mesh				1
#define palette_type_liquid				2
#define palette_type_polygon			3

extern int				drag_mode;

int						palette_cur_type,palette_x,palette_y;

/* =======================================================

      Palette Init and Shutdown
      
======================================================= */

void palette_initialize(void)
{
	Rect			wbox;

	palette_cur_type=palette_type_none;
	
    GetAvailableWindowPositioningBounds(GetMainDevice(),&wbox);
	palette_x=wbox.right-325;
	palette_y=((wbox.bottom-wbox.top)/2)-150;
}

void palette_shutdown(void)
{
	palette_close();
}

/* =======================================================

      Close Current Palette
      
======================================================= */

void palette_close(void)
{
	if (palette_cur_type==palette_type_none) return;
	
	switch (palette_cur_type) {
	
		case palette_type_mesh:
			palette_mesh_close(&palette_x,&palette_y);
			break;
			
		case palette_type_liquid:
			palette_liquid_close(&palette_x,&palette_y);
			break;
			
		case palette_type_polygon:
			palette_polygon_close(&palette_x,&palette_y);
			break;
	
	}

	palette_cur_type=palette_type_none;
}

/* =======================================================

      Reset Palette
      
======================================================= */

void palette_reset(void)
{
	int			type,main_idx,sub_idx,cur_type;
	
		// if no palettes
		
	if (select_count()==0) {
		palette_close();
		return;
	}
		
		// get palette type
		
	cur_type=palette_type_none;
		
	select_get(0,&type,&main_idx,&sub_idx);
	
	switch (type) {
		
		case mesh_piece:
			if (drag_mode==drag_mode_polygon) {
				cur_type=palette_type_polygon;
			}
			else {
				cur_type=palette_type_mesh;
			}
			break;
			
		case liquid_piece:
			if (drag_mode==drag_mode_polygon) {
				cur_type=palette_type_polygon;
			}
			else {
				cur_type=palette_type_liquid;
			}
			break;
	}
	
		// if no palettes to open, then
		// close any open palette
		
	if (cur_type==palette_type_none) {
		palette_close();
		return;
	}
	
		// open palette
		
	switch (cur_type) {
		
		case palette_type_mesh:
		
			if (palette_cur_type!=palette_type_mesh) {
				palette_close();
				palette_cur_type=palette_type_mesh;
				palette_mesh_open(palette_x,palette_y);
			}
			
			palette_mesh_load();
			return;
			
		case palette_type_liquid:
		
			if (palette_cur_type!=palette_type_liquid) {
				palette_close();
				palette_cur_type=palette_type_liquid;
				palette_liquid_open(palette_x,palette_y);
			}
			
			palette_liquid_load();
			return;
			
		case palette_type_polygon:
		
			if (palette_cur_type!=palette_type_polygon) {
				palette_close();
				palette_cur_type=palette_type_polygon;
				palette_polygon_open(palette_x,palette_y);
			}
			
			palette_polygon_load();
			return;
	}
}

