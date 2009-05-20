/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Node Palette

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
#include "common_view.h"
#include "dialog.h"

#define kNodeName						FOUR_CHAR_CODE('name')
#define kNodeEventID					FOUR_CHAR_CODE('vale')
#define kNodeAngleX						FOUR_CHAR_CODE('angx')
#define kNodeAngleY						FOUR_CHAR_CODE('angy')
#define kNodeAngleZ						FOUR_CHAR_CODE('angz')
#define kNodeFollowCamera				FOUR_CHAR_CODE('fcam')

extern map_type				map;

WindowRef					palette_node_wind;

/* =======================================================

      Node Poly Data
      
======================================================= */

void palette_node_load(void)
{
	int						type,main_idx,poly_idx;
	node_type				*node;
	
		// get node
		
	select_get(0,&type,&main_idx,&poly_idx);
	node=&map.nodes[main_idx];
	
		// get controls
		
	dialog_set_text(palette_node_wind,kNodeName,0,node->name);
	dialog_set_int(palette_node_wind,kNodeEventID,0,node->event_id);
	dialog_set_float(palette_node_wind,kNodeAngleX,0,node->ang.x);
	dialog_set_float(palette_node_wind,kNodeAngleY,0,node->ang.y);
	dialog_set_float(palette_node_wind,kNodeAngleZ,0,node->ang.z);
	dialog_set_boolean(palette_node_wind,kNodeFollowCamera,0,node->follow_camera);
	
	DrawControls(palette_node_wind);
}

void palette_node_save(void)
{
	int						type,main_idx,poly_idx;
	node_type				*node;
	
		// get node
		
	select_get(0,&type,&main_idx,&poly_idx);
	node=&map.nodes[main_idx];
	
		// set controls
	
	dialog_get_text(palette_node_wind,kNodeName,0,node->name,name_str_len);
	node->event_id=dialog_get_int(palette_node_wind,kNodeEventID,0);
	node->ang.x=dialog_get_float(palette_node_wind,kNodeAngleX,0);
	node->ang.y=dialog_get_float(palette_node_wind,kNodeAngleY,0);
	node->ang.z=dialog_get_float(palette_node_wind,kNodeAngleZ,0);
	node->follow_camera=dialog_get_boolean(palette_node_wind,kNodeFollowCamera,0);
	
		// need to reset node combo
		
	main_wind_tool_fill_node_combo();
	
	main_wind_draw();
}

static pascal OSStatus palette_node_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	palette_node_save();
	return(eventNotHandledErr);
}

/* =======================================================

      Palette Node Open/Close
      
======================================================= */

void palette_node_open(int x,int y)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassControl,kEventControlHit},
										  {kEventClassKeyboard,kEventRawKeyUp}};

		// open the window
		
	dialog_open(&palette_node_wind,"NodePalette");
	MoveWindow(palette_node_wind,x,y,FALSE);

		// show palette
		
	ShowWindow(palette_node_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(palette_node_event_proc);
	InstallWindowEventHandler(palette_node_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
}

void palette_node_close(int *x,int *y)
{
	Rect			box;
	
	GetWindowBounds(palette_node_wind,kWindowGlobalPortRgn,&box);
	*x=box.left;
	*y=box.top;

	DisposeWindow(palette_node_wind);
}




