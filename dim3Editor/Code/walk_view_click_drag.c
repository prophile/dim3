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

      Drag Mesh Handles
      
======================================================= */

bool walk_view_click_drag_mesh_handle(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						n,x,y,z,mx,my,mz,xadd,yadd,zadd,sz,hit_z,
							px[8],py[8],pz[8],
							type,portal_idx,mesh_idx,poly_idx,handle_idx;
	bool					first_drag;
	d3pnt					old_pt,*old_dpt,mpt,
							org_min,org_max,min,max;
	Point					uipt;
	portal_type				*portal;
	map_mesh_type			*mesh;
	MouseTrackingResult		track;
	
		// any selection?
		
	if (select_count()==0) return(FALSE);
	
	select_get(0,&type,&portal_idx,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);
	
	portal=&map.portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
	
		// are we clicking in the grow handles?
		
	handle_idx=-1;
	hit_z=100000;

	sz=(int)(walk_view_handle_size/2);
	
	walk_view_draw_select_mesh_get_grow_handles(portal_idx,mesh_idx,px,py,pz);

	for (n=0;n!=8;n++) {
		x=(px[n]+portal->x)-view_setup->cpt.x;
		y=py[n]-view_setup->cpt.y;
		z=view_setup->cpt.z-(pz[n]+portal->z);
		
		if (!walk_view_click_rotate_polygon_behind_z(x,y,z)) {
			walk_view_click_project_point(&view_setup->box,&x,&y,&z);
			
			if ((pt->x>=(x-sz)) && (pt->x<=(x+sz)) && (pt->y>=(y-sz)) && (pt->y<=(y+sz))) {
				if (z<hit_z) {
					hit_z=z;
					handle_idx=n;
				}
			}
		}
	}
	
	if (handle_idx==-1) return(FALSE);
	
		// are we dragging?
		
    if (!Button()) return(FALSE);
	
		// backup all the vertexes
		
	old_dpt=valloc(mesh->nvertex*sizeof(d3pnt));
	if (old_dpt==NULL) return(FALSE);
	
	memmove(old_dpt,mesh->vertexes,(mesh->nvertex*sizeof(d3pnt)));
		
		// start dragging

	first_drag=TRUE;
	
	map_portal_mesh_calculate_extent(&map,portal_idx,mesh_idx,&org_min,&org_max);
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
		
			// resize mesh

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);
			
		mx+=xadd;
		my+=yadd;
		mz+=zadd;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		walk_view_click_grid(&mpt);
		
		memmove(&min,&org_min,sizeof(d3pnt));
		memmove(&max,&org_max,sizeof(d3pnt));
		
			// use correct handle to alter the mesh
			
		if ((handle_idx==0) || (handle_idx==3) || (handle_idx==4) || (handle_idx==7)) {
			min.x+=mpt.x/2;
		}
		else {
			max.x+=mpt.x/2;
		}
		
		if (handle_idx<4) {
			min.y+=mpt.y/2;
		}
		else {
			max.y+=mpt.y/2;
		}
		
		if ((handle_idx==0) || (handle_idx==1) || (handle_idx==4) || (handle_idx==5)) {
			min.z+=mpt.z/2;
		}
		else {
			max.z+=mpt.z/2;
		}
		
			// always resize from original vertexes
			
		memmove(mesh->vertexes,old_dpt,(mesh->nvertex*sizeof(d3pnt)));
		map_portal_mesh_resize(&map,portal_idx,mesh_idx,&min,&max);

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	free(old_dpt);
	
	InitCursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Mesh
      
======================================================= */

bool walk_view_click_drag_mesh(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						n,x,y,mx,my,mz,xadd,yadd,zadd,
							type,portal_idx,mesh_idx,poly_idx,fz;
	bool					hit,first_drag;
	d3pnt					old_pt,*dpt,*old_dpt,*old_dpt_ptr,mpt;
	Point					uipt;
	portal_type				*portal;
	map_mesh_type			*mesh;
	MouseTrackingResult		track;
	
		// any selection?
		
	if (select_count()==0) return(FALSE);
	
	select_get(0,&type,&portal_idx,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);
	
	portal=&map.portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
		
		// are we clicking in mesh
		
	hit=FALSE;
		
	for (n=0;n!=mesh->npoly;n++) {
		if (walk_view_mesh_poly_click_index(view_setup,pt,portal,mesh,n,&fz)) {
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
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		walk_view_click_grid(&mpt);
		
		dpt=mesh->vertexes;
		old_dpt_ptr=old_dpt;
		
		for (n=0;n!=mesh->nvertex;n++) {
			dpt->x=old_dpt_ptr->x+mpt.x;
			dpt->y=old_dpt_ptr->y+mpt.y;
			dpt->z=old_dpt_ptr->z+mpt.z;
			
			dpt++;
			old_dpt_ptr++;
		}

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	free(old_dpt);
	
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
	d3pnt					old_pt,*dpt,*old_dpt,mpt;
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
	
	if (!walk_view_mesh_poly_click_index(view_setup,pt,portal,mesh,poly_idx,&fz))  return(FALSE);
	
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
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		walk_view_click_grid(&mpt);
		
		for (n=0;n!=mesh_poly->ptsz;n++) {
			dpt=&mesh->vertexes[mesh_poly->v[n]];
			dpt->x=old_dpt[n].x+mpt.x;
			dpt->y=old_dpt[n].y+mpt.y;
			dpt->z=old_dpt[n].z+mpt.z;
		}

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	free(old_dpt);

	InitCursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Mesh Vertex
      
======================================================= */

bool walk_view_click_drag_vertex(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						n,x,y,z,mx,my,mz,xadd,zadd,yadd,hit_z,sz,
							type,portal_idx,mesh_idx,poly_idx,vertex_idx;
	d3pnt					old_pt,*dpt,old_dpt,mpt;
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
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		walk_view_click_grid(&mpt);
		
		dpt->x=old_dpt.x+mpt.x;
		dpt->y=old_dpt.y+mpt.y;
		dpt->z=old_dpt.z+mpt.z;

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	InitCursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Item
      
======================================================= */

bool walk_view_click_drag_item(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						x,y,mx,my,mz,xadd,zadd,yadd,
							type,portal_idx,main_idx,sub_idx;
	d3pos					*pos,old_pos;
	d3pnt					old_pt,mpt;
	Point					uipt;
	bool					first_drag;
	MouseTrackingResult		track;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&portal_idx,&main_idx,&sub_idx);
	
		// get item pos
		
	pos=NULL;
	
	switch (type) {
		case node_piece:
			pos=&map.nodes[main_idx].pos;
			break;
		case spot_piece:
			pos=&map.spots[main_idx].pos;
			break;
		case scenery_piece:
			pos=&map.sceneries[main_idx].pos;
			break;
		case light_piece:
			pos=&map.lights[main_idx].pos;
			break;
		case sound_piece:
			pos=&map.sounds[main_idx].pos;
			break;
		case particle_piece:
			pos=&map.particles[main_idx].pos;
			break;
	}
	
	if (pos==NULL) return(FALSE);
		
		// drag item
	
    if (!Button()) return(FALSE);

	first_drag=TRUE;
	
	memmove(&old_pos,pos,sizeof(d3pos));
	
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
		
			// move item

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);
			
		mx+=xadd;
		my+=yadd;
		mz+=zadd;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		walk_view_click_grid(&mpt);
		
		pos->x=old_pos.x+mpt.x;
		pos->y=old_pos.y+mpt.y;
		pos->z=old_pos.z+mpt.z;

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	InitCursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Liquid
      
======================================================= */

bool walk_view_click_drag_liquid(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						x,y,mx,my,mz,xadd,zadd,yadd,
							old_lft,old_rgt,old_top,old_bot,old_y,
							type,portal_idx,main_idx,sub_idx;
	d3pnt					old_pt,mpt;
	Point					uipt;
	bool					first_drag;
	map_liquid_type			*liq;
	MouseTrackingResult		track;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&portal_idx,&main_idx,&sub_idx);
	if (type!=liquid_piece) return(FALSE);
	
	liq=&map.portals[portal_idx].liquid.liquids[main_idx];
		
		// drag item
	
    if (!Button()) return(FALSE);

	first_drag=TRUE;
	
	old_lft=liq->lft;
	old_rgt=liq->rgt;
	old_top=liq->top;
	old_bot=liq->bot;
	old_y=liq->y;
	
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
		
			// move item

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);
			
		mx+=xadd;
		my+=yadd;
		mz+=zadd;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		walk_view_click_grid(&mpt);
		
		liq->lft=old_lft+mpt.x;
		liq->rgt=old_rgt+mpt.x;
		liq->top=old_top+mpt.z;
		liq->bot=old_bot+mpt.z;
		liq->y=old_y+mpt.y;

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	InitCursor();
	
	return(!first_drag);
}

