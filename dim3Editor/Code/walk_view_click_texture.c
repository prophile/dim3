/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Texture Routines

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
#include "walk_view.h"

#define drag_texture_shift_factor		100.0f

extern int					cr,cx,cz,cy,vertex_mode;
extern bool					dp_auto_texture;
extern CCrsrHandle			dragcur;

extern map_type				map;

/* =======================================================

      Drag Mesh
      
======================================================= */

bool walk_view_click_drag_texture(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir,bool entire_mesh)
{
	int						n,k,x,y,
							type,portal_idx,mesh_idx,poly_idx;
	bool					first_drag;
	float					gx_add,gy_add;
	d3pnt					old_pt;
	Point					uipt;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	MouseTrackingResult		track;
	
		// any selection?
		
	if (select_count()==0) return(FALSE);
	
	select_get(0,&type,&portal_idx,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);
	
	portal=&map.portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
	
		// drag
		
    if (!Button()) return(FALSE);
			
	first_drag=TRUE;
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	do {
		TrackMouseLocation(NULL,&uipt,&track);
		pt->x=uipt.h-view_setup->box.left;
		pt->y=uipt.v-view_setup->box.top;
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			SetCCursor(dragcur);
			first_drag=FALSE;
		}
		
			// get UV change

		switch (view_move_dir) {
		
			case vm_dir_forward:
				gx_add=((float)x)/drag_texture_shift_factor;
				gy_add=((float)y)/drag_texture_shift_factor;
				if (view_setup->swap_on) gx_add=-gx_add;
				break;
				
			case vm_dir_side:
				gx_add=((float)x)/drag_texture_shift_factor;
				gy_add=((float)y)/drag_texture_shift_factor;
				if (view_setup->swap_on) gx_add=-gx_add;
				break;
				
			case vm_dir_top:
				gx_add=((float)x)/drag_texture_shift_factor;
				gy_add=((float)y)/drag_texture_shift_factor;
				break;
				
			default:
				gx_add=gy_add=0.0f;
				break;
		}
		
			// alter UVs
			
		poly=mesh->polys;

		for (n=0;n!=mesh->npoly;n++) {
		
			if ((entire_mesh) || (n==poly_idx)) {
		
				for (k=0;k!=poly->ptsz;k++) {
					poly->gx[k]+=gx_add;
					poly->gy[k]+=gy_add;
				}
				
			}
			
			poly++;
		}

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	InitCursor();
	
	return(!first_drag);
}
