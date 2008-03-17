/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Dragging Routines

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

extern int					cr,cx,cz,cy,vertex_mode;
extern bool					dp_auto_texture;
extern CCrsrHandle			dragcur;

extern map_type				map;

/* =======================================================

      Drag Mesh
      
======================================================= */

bool walk_view_click_drag_mesh(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						n,x,y,mx,my,mz,xadd,yadd,zadd,
							type,portal_idx,mesh_idx,poly_idx,fz;
	bool					hit,first_drag;
	d3pnt					old_pt,*dpt,*old_dpt,*old_dpt_ptr;
	Point					uipt;
	portal_type				*portal;
	map_mesh_type			*mesh;
	MouseTrackingResult		track;
	
		// any selection?
		
	if (select_count()==0) return(FALSE);
	
	select_get(0,&type,&portal_idx,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);

		// are we clicking in mesh
		
	portal=&map.portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
	
	hit=FALSE;
		
	for (n=0;n!=mesh->npoly;n++) {
		if (walk_view_poly_click_index(view_setup,pt,portal,mesh,n,&fz)) {
			hit=TRUE;
			break;
		}
	}
	
	if (!hit) return(FALSE);
	
		// drag
		
    if (!Button()) return(FALSE);
			
	first_drag=TRUE;
	
	old_dpt=valloc(mesh->nvertex*sizeof(d3pnt));
	if (old_dpt==NULL) return(FALSE);
	
	memmove(old_dpt,mesh->vertexes,(mesh->nvertex*sizeof(d3pnt)));
	
	mx=my=mz=0;
	
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
		
			// move vertexes

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);
		
		mx+=xadd;
		my+=yadd;
		mz+=zadd;
		
		dpt=mesh->vertexes;
		old_dpt_ptr=old_dpt;
		
		for (n=0;n!=mesh->nvertex;n++) {
			dpt->x=old_dpt_ptr->x+mx;
			dpt->y=old_dpt_ptr->y+my;
			dpt->z=old_dpt_ptr->z+mz;
			walk_view_click_grid(dpt);
			
			dpt++;
			old_dpt_ptr++;
		}

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	InitCursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Mesh Poly
      
======================================================= */

bool walk_view_click_drag_mesh_poly(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						n,x,y,mx,my,mz,xadd,yadd,zadd,
							type,portal_idx,mesh_idx,poly_idx,fz;
	bool					first_drag;
	d3pnt					old_pt,*dpt,*old_dpt;
	Point					uipt;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	MouseTrackingResult		track;
	
		// any selection?
		
	if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&portal_idx,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);

		// are we clicking in mesh
		
	portal=&map.portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
	mesh_poly=&mesh->polys[poly_idx];
	
	if (!walk_view_poly_click_index(view_setup,pt,portal,mesh,poly_idx,&fz))  return(FALSE);
	
		// drag
		
    if (!Button()) return(FALSE);
			
	first_drag=TRUE;
	
	old_dpt=valloc(mesh_poly->ptsz*sizeof(d3pnt));
	if (old_dpt==NULL) return(FALSE);
	
	for (n=0;n!=mesh_poly->ptsz;n++) {
		memmove(&old_dpt[n],&mesh->vertexes[mesh_poly->v[n]],sizeof(d3pnt));
	}
	
	mx=my=mz=0;
	
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
		
			// move vertexes

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);
		
		mx+=xadd;
		my+=yadd;
		mz+=zadd;
		
		for (n=0;n!=mesh_poly->ptsz;n++) {
			dpt=&mesh->vertexes[mesh_poly->v[n]];
			dpt->x=old_dpt[n].x+mx;
			dpt->y=old_dpt[n].y+my;
			dpt->z=old_dpt[n].z+mz;
			walk_view_click_grid(dpt);
		}

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	InitCursor();
	
	free(old_dpt);
	
	return(!first_drag);
}

/* =======================================================

      Drag Mesh Vertex
      
======================================================= */

bool walk_view_click_drag_vertex(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						n,x,y,z,mx,my,mz,xadd,zadd,yadd,hit_z,sz,
							type,portal_idx,mesh_idx,poly_idx,vertex_idx;
	d3pnt					old_pt,*dpt,old_dpt;
	Point					uipt;
	bool					first_drag;
	MouseTrackingResult		track;
	portal_type				*portal;
	map_mesh_type			*mesh;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&portal_idx,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);
	
		// check for clicking points

	portal=&map.portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
		
	vertex_idx=-1;
	hit_z=100000;

	sz=(int)(walk_view_handle_size/2);
	
	dpt=mesh->vertexes;
	
	for (n=0;n!=mesh->nvertex;n++) {
		x=(dpt->x+portal->x)-view_setup->cpt.x;
		y=dpt->y-view_setup->cpt.y;
		z=view_setup->cpt.z-(dpt->z+portal->z);
		
		if (!walk_view_click_rotate_polygon_behind_z(x,y,z)) {
			walk_view_click_project_point(&view_setup->box,&x,&y,&z);
			
			if ((pt->x>=(x-sz)) && (pt->x<=(x+sz)) && (pt->y>=(y-sz)) && (pt->y<=(y+sz))) {
				if (z<hit_z) {
					hit_z=z;
					vertex_idx=n;
				}
			}
		}
		
		dpt++;
	}

    if (vertex_idx==-1) return(FALSE);
	
		// drag mesh
	
    if (!Button()) return(FALSE);

	first_drag=TRUE;
	
	dpt=&mesh->vertexes[vertex_idx];
	memmove(&old_dpt,dpt,sizeof(d3pnt));
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
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
		
			// move vertex

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);
			
		mx+=xadd;
		my+=yadd;
		mz+=zadd;
		
		dpt->x=old_dpt.x+mx;
		dpt->y=old_dpt.y+my;
		dpt->z=old_dpt.z+mz;
		
		walk_view_click_grid(dpt);

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	InitCursor();
	
	return(!first_drag);
}

