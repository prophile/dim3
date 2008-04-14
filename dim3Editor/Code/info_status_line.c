/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Info Status Line

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
#include "common_view.h"

extern int				cr;

extern map_type			map;

extern WindowRef		mainwind;

/* =======================================================

      Info Status Line Drawing
      
======================================================= */

void info_status_line_draw_selection(Rect *box)
{
	int				type,portal_idx,main_idx,sub_idx,len;
	char			txt[256];
	portal_type		*portal;
	
		// any selection?
		
	if (cr==-1) {
		MoveTo((box->left+4),(box->bottom-4));
		DrawText("No portal/piece selected",0,18);
		return;
	}
	
		// portal text
		
	portal=&map.portals[cr];
	sprintf(txt,"Portal: %d : (%d,%d) - (%d,%d) : (%d,%d) : %d",cr,portal->x,portal->z,portal->ex,portal->ez,(portal->ex-portal->x),(portal->ez-portal->z),portal->mesh.nmesh);
	
	MoveTo((box->left+4),(box->bottom-4));
	DrawText(txt,0,strlen(txt));
	
		// segment text
		
	txt[0]=0x0;
		
	if (select_count()!=1) {
		strcpy(txt,"No piece selected");
	}
	else {
	
		select_get(0,&type,&portal_idx,&main_idx,&sub_idx);

		switch (type) {
		
		// supergumba -- need work here
		
			case node_piece:
				sprintf(txt,"Node: %s (%d,%d,%d)",map.nodes[main_idx].name,map.nodes[main_idx].pos.x,map.nodes[main_idx].pos.y,map.nodes[main_idx].pos.z);
				break;
				
			case spot_piece:
				sprintf(txt,"Spot: %s (%d,%d,%d)",map.spots[main_idx].name,map.spots[main_idx].pos.x,map.spots[main_idx].pos.y,map.spots[main_idx].pos.z);
				break;
				
			case scenery_piece:
				sprintf(txt,"Scenery: %s (%d,%d,%d)",map.sceneries[main_idx].model_name,map.sceneries[main_idx].pos.x,map.sceneries[main_idx].pos.y,map.sceneries[main_idx].pos.z);
				break;
				
			case light_piece:
				sprintf(txt,"Light: (%d,%d,%d)",map.lights[main_idx].pos.x,map.lights[main_idx].pos.y,map.lights[main_idx].pos.z);
				break;
				
			case sound_piece:
				sprintf(txt,"Sound: %s (%d,%d,%d)",map.sounds[main_idx].name,map.sounds[main_idx].pos.x,map.sounds[main_idx].pos.y,map.sounds[main_idx].pos.z);
				break;
				
			case particle_piece:
				sprintf(txt,"Particle: %s (%d,%d,%d)",map.particles[main_idx].name,map.particles[main_idx].pos.x,map.particles[main_idx].pos.y,map.particles[main_idx].pos.z);
				break;
		}
	}
	
	len=TextWidth(txt,0,strlen(txt));
		
	MoveTo((box->right-(len+20)),(box->bottom-4));
	DrawText(txt,0,strlen(txt));
}

void info_status_line_draw(void)
{
	Rect				wbox,box;
	GrafPtr				saveport;
	RGBColor			blackcolor={0x0,0x0,0x0};
	
	GetPort(&saveport);
	SetPort(GetWindowPort(mainwind));
	
 	GetWindowPortBounds(mainwind,&wbox);
	
		// clear info window
		
	box=wbox;
	box.top=box.bottom-info_high;
	EraseRect(&box);
	
		// draw the line
		
	RGBForeColor(&blackcolor);
	
	MoveTo(box.left,box.top);
	LineTo(box.right,box.top);
	
		// draw selection
	
	TextSize(10);
		
		// the selection text
		
	info_status_line_draw_selection(&box);

	SetPort(saveport);
}

