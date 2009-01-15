/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Piece Routines

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

extern int					drag_mode;

extern map_type				map;
extern setup_type			setup;

/* =======================================================

      Duplicate Piece
      
======================================================= */

void piece_duplicate_offset(int *xadd,int *zadd)
{
	*xadd=setup.duplicate_offset*map_enlarge;
	*zadd=setup.duplicate_offset*map_enlarge;
}

void piece_duplicate(void)
{
	int				n,i,nsel_count,type,main_idx,sub_idx,
					index,xadd,zadd;
	d3pnt			mpt,mov_pt;

	undo_clear();
	
		// duplicate selection
		
	select_duplicate_clear();
	
		// duplicate pieces
		
	nsel_count=select_count();
	
	for (n=0;n!=nsel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);
		
		switch (type) {
			
			case mesh_piece:
				index=map_mesh_duplicate(&map,main_idx);
				if (index==-1) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Mesh","\pNot enough memory.",NULL,NULL);
					return;
				}
				
				map_mesh_calculate_center(&map,index,&mpt);
				piece_duplicate_offset(&xadd,&zadd);
				
				mov_pt.x=xadd;
				mov_pt.y=0;
				mov_pt.z=zadd;
				
				map_mesh_move(&map,index,&mov_pt,TRUE);
				
				select_duplicate_add(mesh_piece,index,0);
				break;
			
			case liquid_piece:
				index=map_liquid_duplicate(&map,main_idx);
				if (index==-1) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Liquid","\pNot enough memory.",NULL,NULL);
					return;
				}
				
				map_liquid_calculate_center(&map,index,&mpt);
				piece_duplicate_offset(&xadd,&zadd);
				map_liquid_move(&map,index,xadd,0,zadd);
				
				select_duplicate_add(liquid_piece,index,0);
				break;
				
			case spot_piece:
				if (map.nspot==max_spot) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Spot","\pYou've reached the maximum number of spots for a map.",NULL,NULL);
					return;
				}
				
				piece_duplicate_offset(&xadd,&zadd);

				map.spots[map.nspot]=map.spots[main_idx];
				map.spots[map.nspot].pnt.x+=xadd;
				map.spots[map.nspot].pnt.z+=zadd;
				select_duplicate_add(spot_piece,map.nspot,0);
				map.nspot++;
				
				main_wind_tool_fill_object_combo();
				break;
				
			case scenery_piece:
				if (map.nscenery==max_map_scenery) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Scenery","\pYou've reached the maximum number of scenery for a map.",NULL,NULL);
					return;
				}
				
				piece_duplicate_offset(&xadd,&zadd);

				map.sceneries[map.nscenery]=map.sceneries[main_idx];
				map.sceneries[map.nscenery].pnt.x+=xadd;
				map.sceneries[map.nscenery].pnt.z+=zadd;
				select_duplicate_add(scenery_piece,map.nscenery,0);
				map.nscenery++;
				break;
				
			case node_piece:
				if (map.nnode==max_node) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Node","\pYou've reached the maximum number of nodes for a map.",NULL,NULL);
					return;
				}

				piece_duplicate_offset(&xadd,&zadd);

				map.nodes[map.nnode]=map.nodes[main_idx];
				map.nodes[map.nnode].pnt.x+=xadd;
				map.nodes[map.nnode].pnt.z+=zadd;
				for (i=0;i!=max_node_link;i++) {
					map.nodes[map.nnode].link[i]=-1;
				}
				select_duplicate_add(node_piece,map.nnode,0);
				map.nnode++;
				
				main_wind_tool_fill_node_combo();
				break;
				
			case light_piece:
				if (map.nlight==max_map_light) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Light","\pYou've reached the maximum number of lights for a map.",NULL,NULL);
					return;
				}

				piece_duplicate_offset(&xadd,&zadd);

				map.lights[map.nlight]=map.lights[main_idx];
				map.lights[map.nlight].pnt.x+=xadd;
				map.lights[map.nlight].pnt.z+=zadd;
				select_duplicate_add(light_piece,map.nlight,0);
				map.nlight++;
				break;
				
			case sound_piece:
				if (map.nsound==max_map_sound) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Sound","\pYou've reached the maximum number of sounds for a map.",NULL,NULL);
					return;
				}

				piece_duplicate_offset(&xadd,&zadd);

				map.sounds[map.nsound]=map.sounds[main_idx];
				map.sounds[map.nsound].pnt.x+=xadd;
				map.sounds[map.nsound].pnt.z+=zadd;
				select_duplicate_add(sound_piece,map.nsound,0);
				map.nsound++;
				break;
				
			case particle_piece:
				if (map.nparticle==max_map_particle) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Particle","\pYou've reached the maximum number of particles for a map.",NULL,NULL);
					return;
				}

				piece_duplicate_offset(&xadd,&zadd);

				map.particles[map.nparticle]=map.particles[main_idx];
				map.particles[map.nparticle].pnt.x+=xadd;
				map.particles[map.nparticle].pnt.z+=zadd;
				select_duplicate_add(particle_piece,map.nparticle,0);
				map.nparticle++;
				break;
				
		}
    }
	
		// switch selections
		
	select_duplicate_copy();
	
	main_wind_draw();
}

/* =======================================================

      Delete Piece
      
======================================================= */

void piece_delete_face(void)
{
	int				type,main_idx,sub_idx;
	
	undo_clear();
	
		// only delete in polygon mode
		
	if (drag_mode!=drag_mode_polygon) return;
	
		// is a face selected?
		
	select_get(0,&type,&main_idx,&sub_idx);
	if (type!=mesh_piece) return;
	if (sub_idx==-1) return;
	
	undo_save();
	
	map_mesh_delete_poly(&map,main_idx,sub_idx);
	
	select_switch(0,mesh_piece,main_idx,0);
	
	main_wind_draw();
}

void piece_delete(void)
{
	int				n,i,k,nsel_count,
					type,main_idx,sub_idx;
	
	undo_save();
	
		// sort segment so higher indexes are deleted first
		
	select_sort();
	
		// delete selection
	
	nsel_count=select_count();
	
	for (n=0;n!=nsel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);
	
		switch (type) {
			
			case mesh_piece:
				if (drag_mode==drag_mode_polygon) {
					map_mesh_delete_poly(&map,main_idx,sub_idx);
					select_delete_move_index(mesh_piece,main_idx,sub_idx);
					break;
				}
				if (drag_mode==drag_mode_mesh) {
					map_mesh_delete(&map,main_idx);
					break;
				}
				break;
				
			case liquid_piece:
				map_liquid_delete(&map,main_idx);
				break;
				
			case spot_piece:
				for (i=main_idx;i<map.nspot;i++) {
					map.spots[i]=map.spots[i+1];
				}
				map.nspot--;
				
				main_wind_tool_fill_object_combo();
				break;
				
			case scenery_piece:
				for (i=main_idx;i<map.nscenery;i++) {
					map.sceneries[i]=map.sceneries[i+1];
				}
				map.nscenery--;
				break;
				
			case node_piece:
				for (i=0;i!=map.nnode;i++) {			// clear all linkage
					if (i==main_idx) continue;

					for (k=0;k!=max_node_link;k++) {
						if (map.nodes[i].link[k]==main_idx) map.nodes[i].link[k]=-1;
						if (map.nodes[i].link[k]>main_idx) map.nodes[i].link[k]--;
					}
				}

				for (i=main_idx;i<map.nnode;i++) {
					map.nodes[i]=map.nodes[i+1];
				}
				
				map.nnode--;
				
				main_wind_tool_fill_node_combo();
				break;
				
			case light_piece:
				for (i=main_idx;i<map.nlight;i++) {
					map.lights[i]=map.lights[i+1];
				}
				map.nlight--;
				break;
				
			case sound_piece:
				for (i=main_idx;i<map.nsound;i++) {
					map.sounds[i]=map.sounds[i+1];
				}
				map.nsound--;
				break;
				
			case particle_piece:
				for (i=main_idx;i<map.nparticle;i++) {
					map.particles[i]=map.particles[i+1];
				}
				map.nparticle--;
				break;
				
		}
	}
	
	select_clear();
    
	main_wind_draw();
}

/* =======================================================

      Piece Tesselate
      
======================================================= */

void piece_tesselate(void)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==mesh_piece) map_mesh_tesselate(&map,mesh_idx);
	}
	
	main_wind_draw();
}

/* =======================================================

      Piece Resize and Reposition
      
======================================================= */

void piece_resize(void)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	float			fct_x,fct_y,fct_z;
	d3pnt			min,max,mpt;
	
		// get the resize factor
		
    dialog_resize_run(&fct_x,&fct_y,&fct_z);
	
	fct_x=fct_x/100.0f;
	fct_y=fct_y/100.0f;
	fct_z=fct_z/100.0f;
	
	if ((fct_x<=0.0f) || (fct_y<=0.0f) || (fct_z<=0.0f)) return;
	
		// resize meshes
		
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		
		if (type==mesh_piece) {
			map_mesh_calculate_extent(&map,mesh_idx,&min,&max);
			map_mesh_calculate_center(&map,mesh_idx,&mpt);
						
			min.x=((min.x-mpt.x)*fct_x)+mpt.x;
			min.y=((min.y-mpt.y)*fct_y)+mpt.y;
			min.z=((min.z-mpt.z)*fct_z)+mpt.z;
			
			max.x=((max.x-mpt.x)*fct_x)+mpt.x;
			max.y=((max.y-mpt.y)*fct_y)+mpt.y;
			max.z=((max.z-mpt.z)*fct_z)+mpt.z;

			map_mesh_resize(&map,mesh_idx,&min,&max);
		}
	}
	
	main_wind_draw();
}

void piece_reposition(void)
{
	int				type,mesh_idx,poly_idx;
	map_mesh_type	*mesh;
	
	if (select_count()==0) return;
	
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return;
	
	mesh=&map.mesh.meshes[mesh_idx];
	map_prepare_mesh_box(mesh);
	
		// get the reposition
		
    dialog_reposition_run(&mesh->box.min,&mesh->box.max);
	
		// reposition
		
	map_mesh_resize(&map,mesh_idx,&mesh->box.min,&mesh->box.max);
	
	main_wind_draw();
}

/* =======================================================

      Piece Transforms
      
======================================================= */

void piece_flip(bool flip_x,bool flip_y,bool flip_z)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==mesh_piece) map_mesh_flip(&map,mesh_idx,flip_x,flip_y,flip_z);
	}
	
	main_wind_draw();
}

void piece_rotate(float rot_x,float rot_y,float rot_z)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	d3pnt			center_pnt;
	d3ang			rot;
	
	sel_count=select_count();
	
	rot.x=rot_x;
	rot.y=rot_y;
	rot.z=rot_z;
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		map_mesh_calculate_center(&map,mesh_idx,&center_pnt);
		map_mesh_rotate(&map,mesh_idx,&center_pnt,&rot,TRUE);
	}
	
	main_wind_draw();
}

void piece_free_rotate(void)
{
	float			rot_x,rot_y,rot_z;
	
	if (!dialog_free_rotate_run(&rot_x,&rot_y,&rot_z)) return;
	
	piece_rotate(rot_x,rot_y,rot_z);
}

void piece_move(int move_x,int move_y,int move_z)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	d3pnt			mov_pnt;
	
	sel_count=select_count();
	
	mov_pnt.x=move_x;
	mov_pnt.y=move_y;
	mov_pnt.z=move_z;
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==mesh_piece) map_mesh_move(&map,mesh_idx,&mov_pnt,TRUE);
	}
	
	main_wind_draw();
}

/* =======================================================

      Select All Polys in Mesh
      
======================================================= */

void piece_mesh_select_all_poly(void)
{
	int				n,k,sel_count,type,mesh_idx,poly_idx;
	map_mesh_type	*mesh;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		mesh=&map.mesh.meshes[mesh_idx];
		
		for (k=0;k!=mesh->npoly;k++) {
			if (!select_check(mesh_piece,mesh_idx,k)) select_add(mesh_piece,mesh_idx,k);
		}
	}
}

/* =======================================================

      Snap Mesh to Grid
      
======================================================= */

void mesh_snap_to_grid(int mesh_idx)
{
	int						n,nvertex,x,y,z;
	d3pnt					mpt,mpt2;
	d3pnt					*pt;
	map_mesh_type			*mesh;

	mesh=&map.mesh.meshes[mesh_idx];

	map_mesh_calculate_center(&map,mesh_idx,&mpt);
	memmove(&mpt2,&mpt,sizeof(d3pnt));
	walk_view_click_grid(&mpt2);
	
	x=mpt2.x-mpt.x;
	y=mpt2.y-mpt.y;
	z=mpt2.z-mpt.z;
	
	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		pt->x+=x;
		pt->y+=y;
		pt->z+=z;
		pt++;
	}
}

void mesh_poly_snap_to_grid(int mesh_idx,int poly_idx)
{
	int						n;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	mesh=&map.mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		walk_view_click_grid(pt);
	}
}

void mesh_vertexes_snap_to_grid(int mesh_idx)
{
	int						n,nvertex;
	d3pnt					*pt;
	map_mesh_type			*mesh;

	mesh=&map.mesh.meshes[mesh_idx];

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		walk_view_click_grid(pt);
		pt++;
	}
}

void piece_mesh_snap_to_grid(void)
{
	int			n,sel_count,type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		mesh_snap_to_grid(mesh_idx);
	}
}

void piece_mesh_snap_closest_vertex(void)
{
	int					n,k,sel_count,type,mesh_idx,poly_idx,
						mesh_1_idx,poly_1_idx,
						mesh_2_idx,poly_2_idx,
						d,dist,x,y,z;
	d3pnt				*pt_1,*pt_2;
	map_mesh_type		*mesh_1,*mesh_2;
	map_mesh_poly_type	*poly_1,*poly_2;

		// find two portals to snap together
		
	mesh_1_idx=mesh_2_idx=-1;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		if (mesh_1_idx==-1) {
			mesh_1_idx=mesh_idx;
			poly_1_idx=poly_idx;
		}
		else {
			mesh_2_idx=mesh_idx;
			poly_2_idx=poly_idx;
			break;
		}
	}
	
		// two meshes?
		
	if (mesh_2_idx==-1) return;
	
		// find closest vertexes
	
	x=y=z=0;	
	dist=-1;
	
	mesh_1=&map.mesh.meshes[mesh_1_idx];
	mesh_2=&map.mesh.meshes[mesh_2_idx];
	
		// find out of all vertexes
		
	if (poly_1_idx==-1) {
	
		pt_1=mesh_1->vertexes;
	
		for (n=0;n!=mesh_1->nvertex;n++) {

			pt_2=mesh_2->vertexes;
		
			for (k=0;k!=mesh_2->nvertex;k++) {
				d=distance_get(pt_1->x,pt_1->y,pt_1->z,pt_2->x,pt_2->y,pt_2->z);
				if ((d<dist) || (dist==-1)) {
					x=pt_2->x-pt_1->x;
					y=pt_2->y-pt_1->y;
					z=pt_2->z-pt_1->z;
					dist=d;
				}
				pt_2++;
			}
			
			pt_1++;
		}
		
	}
	
		// find out of poly vertexes only
		
	else {
	
		poly_1=&mesh_1->polys[poly_1_idx];
		poly_2=&mesh_2->polys[poly_2_idx];
	
		for (n=0;n!=poly_1->ptsz;n++) {

			pt_1=&mesh_1->vertexes[poly_1->v[n]];
		
			for (k=0;k!=poly_2->ptsz;k++) {
				pt_2=&mesh_2->vertexes[poly_2->v[k]];
				
				d=distance_get(pt_1->x,pt_1->y,pt_1->z,pt_2->x,pt_2->y,pt_2->z);
				if ((d<dist) || (dist==-1)) {
					x=pt_2->x-pt_1->x;
					y=pt_2->y-pt_1->y;
					z=pt_2->z-pt_1->z;
					dist=d;
				}
			}
		}
	
	}
	
		// move together
		
	pt_1=mesh_1->vertexes;
	
	for (n=0;n!=mesh_1->nvertex;n++) {
		pt_1->x+=x;
		pt_1->y+=y;
		pt_1->z+=z;
		pt_1++;
	}
}

void piece_mesh_poly_snap_to_grid(void)
{
	int			n,sel_count,type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		mesh_poly_snap_to_grid(mesh_idx,poly_idx);
	}
}

void piece_mesh_vertexes_snap_to_grid(void)
{
	int			n,sel_count,type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		
		mesh_vertexes_snap_to_grid(mesh_idx);
	}
}

/* =======================================================

      Reset/Rotate Mesh UVs
      
======================================================= */

void piece_reset_uvs(bool poly_only)
{
	int						n,sel_count,
							type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		if (map.mesh.meshes[mesh_idx].flag.lock_uv) continue;
		
		if (poly_only) {
			map_mesh_reset_poly_uv(&map,mesh_idx,poly_idx);
		}
		else {
			map_mesh_reset_uv(&map,mesh_idx);
		}
	}
}

void piece_rotate_uvs(void)
{
	int						n,sel_count,
							type,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;

		map_mesh_rotate_poly_uv(&map,mesh_idx,poly_idx,90.0f);
	}
}

/* =======================================================

      Piece Holes
      
======================================================= */

void piece_poly_hole(void)
{
	int				n,sel_count,type,mesh_idx,poly_idx;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==mesh_piece) map_mesh_poly_punch_hole(&map,mesh_idx,poly_idx,NULL);
	}
	
	select_clear();
	
	main_wind_draw();
}

/* =======================================================

      Piece Keys
      
======================================================= */

void piece_key(editor_3D_view_setup *view_setup,int view_move_dir,char ch)
{
	int				n,sel_count,type,main_idx,sub_idx,mv,xadd,yadd,zadd;
	d3pnt			pt;
	
		// special check for delete key
		
	if (ch==0x08) {
		piece_delete();
		main_wind_draw();
		menu_fix_enable();
		main_wind_tool_fix_enable();
		return;
	}
	
		// nudge keys movement
	
	mv=walk_view_get_grid();
	if (!main_wind_shift_down()) mv/=10;
	
	if (mv<1) mv=1;
	
	xadd=yadd=zadd=0;
		
	switch (ch) {
	
		case 0x1C:
			walk_view_click_drag_movement(view_setup,view_move_dir,mv,0,&xadd,&yadd,&zadd);
			break;
			
		case 0x1D:
			walk_view_click_drag_movement(view_setup,view_move_dir,-mv,0,&xadd,&yadd,&zadd);
			break;
			
		case 0x1E:
			walk_view_click_drag_movement(view_setup,view_move_dir,0,mv,&xadd,&yadd,&zadd);
			break;
			
		case 0x1F:
			walk_view_click_drag_movement(view_setup,view_move_dir,0,-mv,&xadd,&yadd,&zadd);
			break;
			
	}
	
	if ((xadd==0) && (yadd==0) && (zadd==0)) return;
	
		// move selection
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&main_idx,&sub_idx);

		switch (type) {
		
			case mesh_piece:
				pt.x=xadd;
				pt.y=yadd;
				pt.z=zadd;
				map_mesh_move(&map,main_idx,&pt,TRUE);
				break;
				
			case liquid_piece:
				map_liquid_move(&map,main_idx,xadd,yadd,zadd);
				break;
				
			case node_piece:
				map.nodes[main_idx].pnt.x+=xadd;
				map.nodes[main_idx].pnt.y+=yadd;
				map.nodes[main_idx].pnt.z+=zadd;
				break;
				
			case spot_piece:
				map.spots[main_idx].pnt.x+=xadd;
				map.spots[main_idx].pnt.y+=yadd;
				map.spots[main_idx].pnt.z+=zadd;
				break;
				
			case scenery_piece:
				map.sceneries[main_idx].pnt.x+=xadd;
				map.sceneries[main_idx].pnt.y+=yadd;
				map.sceneries[main_idx].pnt.z+=zadd;
				break;
				
			case light_piece:
				map.lights[main_idx].pnt.x+=xadd;
				map.lights[main_idx].pnt.y+=yadd;
				map.lights[main_idx].pnt.z+=zadd;
				break;
				
			case sound_piece:
				map.sounds[main_idx].pnt.x+=xadd;
				map.sounds[main_idx].pnt.y+=yadd;
				map.sounds[main_idx].pnt.z+=zadd;
				break;
				
			case particle_piece:
				map.particles[main_idx].pnt.x+=xadd;
				map.particles[main_idx].pnt.y+=yadd;
				map.particles[main_idx].pnt.z+=zadd;
				break;
				
		}
	}
	
	main_wind_draw();
}

