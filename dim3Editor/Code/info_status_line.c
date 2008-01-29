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

void info_status_line_get_segment_str(segment_type *seg,char *txt)
{
	int			x,y,z,lx,rx,lz,rz,ty,by;
	
	switch (seg->type) {
		case sg_wall:
			x=labs(seg->data.wall.rx-seg->data.wall.lx);
			y=labs(seg->data.wall.by-seg->data.wall.ty);
			z=labs(seg->data.wall.rz-seg->data.wall.lz);
			sprintf(txt,"Wall: (%d,%d,%d) - (%d,%d,%d) : (%d,%d,%d)",seg->data.wall.lx,seg->data.wall.ty,seg->data.wall.lz,seg->data.wall.rx,seg->data.wall.by,seg->data.wall.rz,x,y,z);
			return;
		case sg_floor:
			poly_find_bound_rect(seg->data.fc.ptsz,seg->data.fc.x,seg->data.fc.z,&lx,&rx,&lz,&rz);
			poly_find_bound_high(seg->data.fc.ptsz,seg->data.fc.y,&ty,&by);
			x=labs(rx-lx);
			y=labs(by-ty);
			z=labs(rz-lz);
			sprintf(txt,"Floor: (%d,%d,%d) - (%d,%d,%d) : (%d,%d,%d)",lx,ty,lz,rx,by,rz,x,y,z);
			return;
		case sg_ceiling:			
			poly_find_bound_rect(seg->data.fc.ptsz,seg->data.fc.x,seg->data.fc.z,&lx,&rx,&lz,&rz);
			poly_find_bound_high(seg->data.fc.ptsz,seg->data.fc.y,&ty,&by);
			x=labs(rx-lx);
			y=labs(by-ty);
			z=labs(rz-lz);
			sprintf(txt,"Ceiling: (%d,%d,%d) - (%d,%d,%d) : (%d,%d,%d)",lx,ty,lz,rx,by,rz,x,y,z);
			return;
		case sg_liquid:
			x=labs(seg->data.liquid.rgt-seg->data.liquid.lft);
			y=0;
			z=labs(seg->data.liquid.bot-seg->data.liquid.top);
			sprintf(txt,"Liquid: (%d,%d,%d) - (%d,%d,%d) : (%d,%d,%d)",seg->data.liquid.lft,seg->data.liquid.y,seg->data.liquid.top,seg->data.liquid.rgt,seg->data.liquid.y,seg->data.liquid.bot,x,y,z);
			return;
		case sg_ambient_wall:
			x=labs(seg->data.ambient_wall.rx-seg->data.ambient_wall.lx);
			y=labs(seg->data.ambient_wall.by-seg->data.ambient_wall.ty);
			z=labs(seg->data.ambient_wall.rz-seg->data.ambient_wall.lz);
			sprintf(txt,"Ambient Wall: (%d,%d,%d) - (%d,%d,%d) : (%d,%d,%d)",seg->data.ambient_wall.lx,seg->data.ambient_wall.ty,seg->data.ambient_wall.lz,seg->data.ambient_wall.rx,seg->data.ambient_wall.by,seg->data.ambient_wall.rz,x,y,z);
			return;
		case sg_ambient_fc:
			poly_find_bound_rect(seg->data.ambient_fc.ptsz,seg->data.ambient_fc.x,seg->data.ambient_fc.z,&lx,&rx,&lz,&rz);
			poly_find_bound_high(seg->data.ambient_fc.ptsz,seg->data.ambient_fc.y,&ty,&by);
			x=labs(rx-lx);
			y=labs(by-ty);
			z=labs(rz-lz);
			sprintf(txt,"Ambient Floor/Ceiling: (%d,%d,%d) - (%d,%d,%d) : (%d,%d,%d)",lx,ty,lz,rx,by,rz,x,y,z);
			return;
	}

}

void info_status_line_draw_selection(Rect *box)
{
	int				type,portal_idx,main_idx,sub_idx,len,
					minx,maxx,minz,maxz,miny,maxy;
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
	sprintf(txt,"Portal: %d : (%d,%d) - (%d,%d) : (%d,%d) : %d",cr,portal->x,portal->z,portal->ex,portal->ez,(portal->ex-portal->x),(portal->ez-portal->z),map_portal_count_segments(&map,cr));
	
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
		
			case segment_piece:
				info_status_line_get_segment_str(&map.segments[main_idx],txt);
				break;
				
			case primitive_piece:
				primitive_get_extend(map.segments[main_idx].primitive_uid[0],&minx,&maxx,&minz,&maxz,&miny,&maxy);
				sprintf(txt,"Primitive: (%d,%d,%d) - (%d,%d,%d) : (%d,%d,%d)",minx,miny,minz,maxx,maxy,maxz,(maxx-minx),(maxy-miny),(maxz-minz));
				break;
				
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

