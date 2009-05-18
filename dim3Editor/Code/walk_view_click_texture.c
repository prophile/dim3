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

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

#define drag_texture_shift_factor		100.0f

extern int					vertex_mode,main_wind_uv_layer;
extern bool					dp_auto_texture;

extern map_type				map;

/* =======================================================

      Drag Mesh
      
======================================================= */

bool walk_view_click_drag_texture(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir,bool entire_mesh)
{
	int						n,k,x,y,
							type,mesh_idx,poly_idx;
	bool					first_drag;
	float					gx_add,gy_add;
	d3pnt					old_pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
		// any selection?
		
	if (select_count()==0) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);
	
	mesh=&map.mesh.meshes[mesh_idx];
	
		// drag
		
    if (!os_button_down()) return(FALSE);
			
	first_drag=TRUE;
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	while (!os_track_mouse_location(pt,&view_setup->box)) {
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			os_set_drag_cursor();
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
					poly->uv[main_wind_uv_layer].x[k]+=gx_add;
					poly->uv[main_wind_uv_layer].y[k]+=gy_add;
				}
				
			}
			
			poly++;
		}

        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

