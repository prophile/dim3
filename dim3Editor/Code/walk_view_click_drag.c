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

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern int					vertex_mode;
extern bool					dp_auto_texture;

extern map_type				map;

/* =======================================================

      Drag Constraints
      
======================================================= */

void walk_view_click_drag_constraint_axis(int *x,int *y)
{
	int			ax,ay;
	
	if (!main_wind_shift_down()) return;
	
	ax=abs(*x);
	ay=abs(*y);
	
	if (ax>ay) {
		if ((*y)<0) {
			*y=-ax;
		}
		else {
			*y=ax;
		}
		
		return;
	}

	if ((*x)<0) {
		*x=-ay;
	}
	else {
		*x=ay;
	}
}

void walk_view_click_drag_constraint(int *x,int *y,int *z,int view_mode_dir)
{
	switch (view_mode_dir) {
		case vm_dir_forward:
			walk_view_click_drag_constraint_axis(x,y);
			break;
		case vm_dir_side:
			walk_view_click_drag_constraint_axis(z,y);
			break;
		case vm_dir_top:
			walk_view_click_drag_constraint_axis(x,z);
			break;
	}
}

/* =======================================================

      Drag Mesh Handles
      
======================================================= */

bool walk_view_click_drag_mesh_handle(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						n,x,y,z,mx,my,mz,xadd,yadd,zadd,sz,hit_z,
							px[8],py[8],pz[8],
							type,mesh_idx,poly_idx,handle_idx;
	bool					first_drag;
	d3pnt					old_pt,*old_dpt,mpt,
							org_min,org_max,min,max;
	map_mesh_type			*mesh;
	
		// any selection?
		
	if (select_count()==0) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);
	
	mesh=&map.mesh.meshes[mesh_idx];
	
		// are we clicking in the grow handles?
		
	handle_idx=-1;
	hit_z=100000;

	sz=(int)(walk_view_handle_size/2);
	
	walk_view_draw_select_mesh_get_grow_handles(mesh_idx,px,py,pz);

	for (n=0;n!=8;n++) {
		x=px[n]-view_setup->cpt.x;
		y=py[n]-view_setup->cpt.y;
		z=view_setup->cpt.z-pz[n];
		
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
		
    if (!os_button_down()) return(FALSE);
	
	undo_save();
	
		// backup all the vertexes
		
	old_dpt=malloc(mesh->nvertex*sizeof(d3pnt));
	if (old_dpt==NULL) return(FALSE);
	
	memmove(old_dpt,mesh->vertexes,(mesh->nvertex*sizeof(d3pnt)));
		
		// start dragging

	first_drag=TRUE;
	
	map_mesh_calculate_extent(&map,mesh_idx,&org_min,&org_max);
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
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
		
			// resize mesh

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);

		mx+=xadd;
		my+=yadd;
		mz+=zadd;
			
		walk_view_click_drag_constraint(&mx,&my,&mz,view_move_dir);

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
		map_mesh_resize(&map,mesh_idx,&min,&max);
		
		if ((dp_auto_texture) && (!mesh->flag.lock_uv)) map_mesh_reset_uv(&map,mesh_idx);

        main_wind_draw();
	}
	
	free(old_dpt);
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Mesh
      
======================================================= */

bool walk_view_click_drag_mesh(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						n,k,x,y,mx,my,mz,xadd,yadd,zadd,nsel,nvertex,
							type,mesh_idx,poly_idx,fz;
	bool					hit,first_drag;
	d3pnt					old_pt,*dpt,*old_dpt,*old_dpt_ptr,mpt;
	map_mesh_type			*mesh;
	
		// any selection?
		
	nsel=select_count();
	if (nsel==0) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);
	
	mesh=&map.mesh.meshes[mesh_idx];
		
		// are we clicking in a selected mesh
		
	hit=FALSE;
	
	for (k=0;k!=nsel;k++) {
		select_get(k,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		mesh=&map.mesh.meshes[mesh_idx];
		
		for (n=0;n!=mesh->npoly;n++) {
			if (walk_view_mesh_poly_click_index(view_setup,pt,mesh,n,&fz)) {
				hit=TRUE;
				break;
			}
		}
		
		if (hit) break;
	}
	
	if (!hit) return(FALSE);
	
		// start drag
		
    if (!os_button_down()) return(FALSE);
	
	undo_save();
	
		// save old vertexes
		
	nvertex=0;
	
	for (k=0;k!=nsel;k++) {
		select_get(k,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		nvertex+=map.mesh.meshes[mesh_idx].nvertex;
	}
	
	old_dpt=malloc(nvertex*sizeof(d3pnt));
	if (old_dpt==NULL) return(FALSE);
	
	old_dpt_ptr=old_dpt;
	
	for (k=0;k!=nsel;k++) {
		select_get(k,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		mesh=&map.mesh.meshes[mesh_idx];

		memmove(old_dpt_ptr,mesh->vertexes,(mesh->nvertex*sizeof(d3pnt)));
		old_dpt_ptr+=mesh->nvertex;
	}
	
		// drag meshes
		
	mx=my=mz=0;
			
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
		
			// get movement

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);
		
		mx+=xadd;
		my+=yadd;
		mz+=zadd;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		walk_view_click_grid(&mpt);

			// move vertexes
		
		old_dpt_ptr=old_dpt;
		
		for (k=0;k!=nsel;k++) {
			select_get(k,&type,&mesh_idx,&poly_idx);
			if (type!=mesh_piece) continue;
			
			mesh=&map.mesh.meshes[mesh_idx];
			dpt=mesh->vertexes;
		
			for (n=0;n!=mesh->nvertex;n++) {
				dpt->x=old_dpt_ptr->x+mpt.x;
				dpt->y=old_dpt_ptr->y+mpt.y;
				dpt->z=old_dpt_ptr->z+mpt.z;
				
				dpt++;
				old_dpt_ptr++;
			}
			
			if ((dp_auto_texture) && (!mesh->flag.lock_uv)) map_mesh_reset_uv(&map,mesh_idx);
		}
		
        main_wind_draw();
	}
	
	free(old_dpt);
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Mesh Poly
      
======================================================= */

bool walk_view_click_drag_mesh_poly(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						n,x,y,mx,my,mz,xadd,yadd,zadd,
							type,mesh_idx,poly_idx,fz;
	bool					first_drag;
	d3pnt					old_pt,*dpt,*old_dpt,mpt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	
		// any selection?
		
	if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);

		// are we clicking in mesh
		
	mesh=&map.mesh.meshes[mesh_idx];
	mesh_poly=&mesh->polys[poly_idx];
	
	if (!walk_view_mesh_poly_click_index(view_setup,pt,mesh,poly_idx,&fz))  return(FALSE);
	
		// drag
		
    if (!os_button_down()) return(FALSE);
	
	undo_save();
			
	first_drag=TRUE;
	
	old_dpt=malloc(mesh_poly->ptsz*sizeof(d3pnt));
	if (old_dpt==NULL) return(FALSE);
	
	for (n=0;n!=mesh_poly->ptsz;n++) {
		memmove(&old_dpt[n],&mesh->vertexes[mesh_poly->v[n]],sizeof(d3pnt));
	}
	
	mx=my=mz=0;
	
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
		
			// get vertex movement

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);
		
		mx+=xadd;
		my+=yadd;
		mz+=zadd;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		walk_view_click_grid(&mpt);
		
			// move vertexes
			
		for (n=0;n!=mesh_poly->ptsz;n++) {
			dpt=&mesh->vertexes[mesh_poly->v[n]];
			dpt->x=old_dpt[n].x+mpt.x;
			dpt->y=old_dpt[n].y+mpt.y;
			dpt->z=old_dpt[n].z+mpt.z;
		}

		if ((dp_auto_texture) && (!mesh->flag.lock_uv)) map_mesh_reset_poly_uv(&map,mesh_idx,poly_idx);

        main_wind_draw();
	}
	
	free(old_dpt);

	os_set_arrow_cursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Mesh Vertex
      
======================================================= */

bool walk_view_click_drag_vertex(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						n,x,y,z,mx,my,mz,xadd,zadd,yadd,hit_z,sz,
							type,mesh_idx,poly_idx,vertex_idx;
	d3pnt					old_pt,*dpt,old_dpt,mpt;
	bool					first_drag;
	map_mesh_type			*mesh;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);
	
		// check for clicking points

	mesh=&map.mesh.meshes[mesh_idx];
		
	vertex_idx=-1;
	hit_z=100000;

	sz=(int)(walk_view_handle_size/2);
	
	dpt=mesh->vertexes;
	
	for (n=0;n!=mesh->nvertex;n++) {
		x=dpt->x-view_setup->cpt.x;
		y=dpt->y-view_setup->cpt.y;
		z=view_setup->cpt.z-dpt->z;
		
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
	
    if (!os_button_down()) return(FALSE);
	
	undo_save();

	first_drag=TRUE;
	
	dpt=&mesh->vertexes[vertex_idx];
	memmove(&old_dpt,dpt,sizeof(d3pnt));
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
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
		
			// get vertex movement

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);
			
		mx+=xadd;
		my+=yadd;
		mz+=zadd;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		walk_view_click_grid(&mpt);
		walk_view_click_snap(mesh_idx,vertex_idx,&old_dpt,&mpt);
		
			// move vertex
			
		dpt->x=old_dpt.x+mpt.x;
		dpt->y=old_dpt.y+mpt.y;
		dpt->z=old_dpt.z+mpt.z;
		
		if ((dp_auto_texture) && (!mesh->flag.lock_uv)) map_mesh_reset_uv(&map,mesh_idx);

        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Liquid Vertex
      
======================================================= */

bool walk_view_click_drag_liquid_vertex(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						n,x,y,z,mx,my,mz,xadd,zadd,yadd,hit_z,sz,chk_x,chk_z,
							px[4],py[4],pz[4],
							type,liquid_idx,sub_idx,vertex_idx;
	d3pnt					old_pt,old_dpt,mpt;
	bool					first_drag;
	map_liquid_type			*liq;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&liquid_idx,&sub_idx);
	if (type!=liquid_piece) return(FALSE);
	
		// check for clicking points

	liq=&map.liquid.liquids[liquid_idx];
	
	walk_view_draw_select_liquid_get_grow_handles(liquid_idx,px,py,pz);
		
	vertex_idx=-1;
	hit_z=100000;

	sz=(int)(walk_view_handle_size/2);
	
	for (n=0;n!=4;n++) {
		x=px[n]-view_setup->cpt.x;
		y=py[n]-view_setup->cpt.y;
		z=view_setup->cpt.z-pz[n];
		
		if (!walk_view_click_rotate_polygon_behind_z(x,y,z)) {
			walk_view_click_project_point(&view_setup->box,&x,&y,&z);
			
			if ((pt->x>=(x-sz)) && (pt->x<=(x+sz)) && (pt->y>=(y-sz)) && (pt->y<=(y+sz))) {
				if (z<hit_z) {
					hit_z=z;
					vertex_idx=n;
				}
			}
		}
	}

    if (vertex_idx==-1) return(FALSE);
	
		// drag liquid vertex
	
    if (!os_button_down()) return(FALSE);
	
	undo_save();

	first_drag=TRUE;
	
	old_dpt.x=px[vertex_idx];
	old_dpt.y=py[vertex_idx];
	old_dpt.z=pz[vertex_idx];
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
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
		
			// get vertex move (never on Y)

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);
			
		mx+=xadd;
		my=liq->y;
		mz+=zadd;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		walk_view_click_grid(&mpt);
		
			// move vertex
			
		switch (vertex_idx) {
			case 0:
				liq->lft=old_dpt.x+mpt.x;
				liq->top=old_dpt.z+mpt.z;
				break;
			case 1:
				liq->rgt=old_dpt.x+mpt.x;
				liq->top=old_dpt.z+mpt.z;
				break;
			case 2:
				liq->rgt=old_dpt.x+mpt.x;
				liq->bot=old_dpt.z+mpt.z;
				break;
			case 3:
				liq->lft=old_dpt.x+mpt.x;
				liq->bot=old_dpt.z+mpt.z;
				break;
		}
		
		if (dp_auto_texture) map_liquid_reset_uv(&map,liquid_idx);

        main_wind_draw();
	}
	
		// fix any left/right swaps
		
	if (liq->lft>liq->rgt) {
		chk_x=liq->lft;
		liq->lft=liq->rgt;
		liq->rgt=chk_x;
	}
	
	if (liq->top>liq->bot) {
		chk_z=liq->top;
		liq->top=liq->bot;
		liq->bot=chk_z;
	}
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Item
      
======================================================= */

bool walk_view_click_drag_item(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						x,y,mx,my,mz,xadd,zadd,yadd,
							type,main_idx,sub_idx;
	d3pnt					*pnt,old_pt,old_pnt,mpt;
	bool					first_drag;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&main_idx,&sub_idx);
	
		// get item pos
		
	pnt=NULL;
	
	switch (type) {
		case node_piece:
			pnt=&map.nodes[main_idx].pnt;
			break;
		case spot_piece:
			pnt=&map.spots[main_idx].pnt;
			break;
		case scenery_piece:
			pnt=&map.sceneries[main_idx].pnt;
			break;
		case light_piece:
			pnt=&map.lights[main_idx].pnt;
			break;
		case sound_piece:
			pnt=&map.sounds[main_idx].pnt;
			break;
		case particle_piece:
			pnt=&map.particles[main_idx].pnt;
			break;
	}
	
	if (pnt==NULL) return(FALSE);
		
		// drag item
	
    if (!os_button_down()) return(FALSE);

	first_drag=TRUE;
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	memmove(&old_pnt,pnt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
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
		
			// get movement

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);
			
		mx+=xadd;
		my+=yadd;
		mz+=zadd;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		walk_view_click_grid(&mpt);
		
		pnt->x=old_pnt.x+mpt.x;
		pnt->y=old_pnt.y+mpt.y;
		pnt->z=old_pnt.z+mpt.z;

        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Liquid
      
======================================================= */

bool walk_view_click_drag_liquid(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir)
{
	int						x,y,mx,my,mz,xadd,zadd,yadd,
							old_lft,old_rgt,old_top,old_bot,old_y,
							type,main_idx,sub_idx;
	d3pnt					old_pt,mpt;
	bool					first_drag;
	map_liquid_type			*liq;
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&main_idx,&sub_idx);
	if (type!=liquid_piece) return(FALSE);
	
	liq=&map.liquid.liquids[main_idx];
		
		// drag item
	
    if (!os_button_down()) return(FALSE);
	
	undo_save();

	first_drag=TRUE;
	
	old_lft=liq->lft;
	old_rgt=liq->rgt;
	old_top=liq->top;
	old_bot=liq->bot;
	old_y=liq->y;
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	mx=my=mz=0;
	
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
		
			// get movement

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);
			
		mx+=xadd;
		my+=yadd;
		mz+=zadd;
		
		mpt.x=mx;
		mpt.y=my;
		mpt.z=mz;
		
		walk_view_click_grid(&mpt);
		
			// move liquid
			
		liq->lft=old_lft+mpt.x;
		liq->rgt=old_rgt+mpt.x;
		liq->top=old_top+mpt.z;
		liq->bot=old_bot+mpt.z;
		liq->y=old_y+mpt.y;

		if (dp_auto_texture) map_liquid_reset_uv(&map,main_idx);

        main_wind_draw();
	}
	
	os_set_arrow_cursor();
	
	return(!first_drag);
}

