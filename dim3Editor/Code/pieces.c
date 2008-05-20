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
#include "portal_view.h"

extern int					cr,drag_mode;
extern CCrsrHandle			dragcur;

extern map_type				map;

/* =======================================================

      Move Pieces
      
======================================================= */

void piece_move_to_portal(int rn)
{
	int			n,nsel_count,type,portal_idx,main_idx,sub_idx;
	d3pnt		min,max;
			
	undo_clear();
	
	select_get_extent(&min,&max);
	
	nsel_count=select_count();
	
	for (n=0;n!=nsel_count;n++) {
		
		select_get(n,&type,&portal_idx,&main_idx,&sub_idx);
		
		switch (type) {
		
			case mesh_piece:
				main_idx=map_portal_mesh_switch_portal(&map,portal_idx,main_idx,rn);
				select_switch(n,type,rn,main_idx,0);
				break;
				
			case liquid_piece:
				main_idx=map_portal_liquid_switch_portal(&map,portal_idx,main_idx,rn);
				select_switch(n,type,rn,main_idx,0);
				break;
				
			case spot_piece:
				map.spots[main_idx].pos.rn=rn;
				select_switch(n,type,rn,main_idx,0);
				break;
				
			case scenery_piece:
				map.sceneries[main_idx].pos.rn=rn;
				select_switch(n,type,rn,main_idx,0);
				break;
				
			case node_piece:
				map.nodes[main_idx].pos.rn=rn;
				select_switch(n,type,rn,main_idx,0);
				break;
				
			case light_piece:
				map.lights[main_idx].pos.rn=rn;
				select_switch(n,type,rn,main_idx,0);
				break;
				
			case sound_piece:
				map.sounds[main_idx].pos.rn=rn;
				select_switch(n,type,rn,main_idx,0);
				break;
				
			case particle_piece:
				map.particles[main_idx].pos.rn=rn;
				select_switch(n,type,rn,main_idx,0);
				break;
				
		}
    }
	
		// shift selection to top-left
		
	cr=rn;
	select_move(rn,min.x,min.z,0);
	
	main_wind_draw();
}

/* =======================================================

      Duplicate Piece
      
======================================================= */

void piece_duplicate(void)
{
	int				n,i,nsel_count,type,portal_idx,main_idx,sub_idx,
					index,xadd,zadd;
	d3pnt			mpt;

	undo_clear();
	
		// duplicate selection
		
	select_duplicate_clear();
	
		// duplicate pieces
		
	nsel_count=select_count();
	
	for (n=0;n!=nsel_count;n++) {
		select_get(n,&type,&portal_idx,&main_idx,&sub_idx);
		
		switch (type) {
			
			case mesh_piece:
				index=map_portal_mesh_duplicate(&map,portal_idx,main_idx);
				if (index==-1) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Mesh","\pNot enough memory.",NULL,NULL);
					return;
				}
				
				map_portal_mesh_calculate_center(&map,portal_idx,index,&mpt);
				portal_duplicate_piece_offset(portal_idx,mpt.x,mpt.z,&xadd,&zadd);
				map_portal_mesh_move(&map,portal_idx,index,FALSE,xadd,0,zadd);
				
				select_duplicate_add(mesh_piece,portal_idx,index,0);
				break;
			
			case liquid_piece:
				index=map_portal_liquid_duplicate(&map,portal_idx,main_idx);
				if (index==-1) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Liquid","\pNot enough memory.",NULL,NULL);
					return;
				}
				
				map_portal_liquid_calculate_center(&map,portal_idx,index,&mpt);
				portal_duplicate_piece_offset(portal_idx,mpt.x,mpt.z,&xadd,&zadd);
				map_portal_liquid_move(&map,portal_idx,index,xadd,0,zadd);
				
				select_duplicate_add(liquid_piece,portal_idx,index,0);
				break;
				
			case spot_piece:
				if (map.nspot==max_spot) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Spot","\pYou've reached the maximum number of spots for a map.",NULL,NULL);
					return;
				}
				
				portal_duplicate_piece_offset(map.spots[main_idx].pos.rn,map.spots[main_idx].pos.x,map.spots[main_idx].pos.z,&xadd,&zadd);

				map.spots[map.nspot]=map.spots[main_idx];
				map.spots[map.nspot].pos.x+=xadd;
				map.spots[map.nspot].pos.z+=zadd;
				select_duplicate_add(spot_piece,portal_idx,map.nspot,0);
				map.nspot++;
				break;
				
			case scenery_piece:
				if (map.nscenery==max_map_scenery) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Scenery","\pYou've reached the maximum number of scenery for a map.",NULL,NULL);
					return;
				}
				
				portal_duplicate_piece_offset(map.sceneries[main_idx].pos.rn,map.sceneries[main_idx].pos.x,map.sceneries[main_idx].pos.z,&xadd,&zadd);

				map.sceneries[map.nscenery]=map.sceneries[main_idx];
				map.sceneries[map.nscenery].pos.x+=xadd;
				map.sceneries[map.nscenery].pos.z+=zadd;
				select_duplicate_add(scenery_piece,portal_idx,map.nscenery,0);
				map.nscenery++;
				break;
				
			case node_piece:
				if (map.nnode==max_node) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Node","\pYou've reached the maximum number of nodes for a map.",NULL,NULL);
					return;
				}

				portal_duplicate_piece_offset(map.nodes[main_idx].pos.rn,map.nodes[main_idx].pos.x,map.nodes[main_idx].pos.z,&xadd,&zadd);

				map.nodes[map.nnode]=map.nodes[main_idx];
				map.nodes[map.nnode].pos.x+=xadd;
				map.nodes[map.nnode].pos.z+=zadd;
				for (i=0;i!=max_node_link;i++) {
					map.nodes[map.nnode].link[i]=-1;
				}
				select_duplicate_add(node_piece,portal_idx,map.nnode,0);
				map.nnode++;
				break;
				
			case light_piece:
				if (map.nlight==max_map_light) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Light","\pYou've reached the maximum number of lights for a map.",NULL,NULL);
					return;
				}

				portal_duplicate_piece_offset(map.lights[main_idx].pos.rn,map.lights[main_idx].pos.x,map.lights[main_idx].pos.z,&xadd,&zadd);

				map.lights[map.nlight]=map.lights[main_idx];
				map.lights[map.nlight].pos.x+=xadd;
				map.lights[map.nlight].pos.z+=zadd;
				select_duplicate_add(light_piece,portal_idx,map.nlight,0);
				map.nlight++;
				break;
				
			case sound_piece:
				if (map.nsound==max_map_sound) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Sound","\pYou've reached the maximum number of sounds for a map.",NULL,NULL);
					return;
				}

				portal_duplicate_piece_offset(map.sounds[main_idx].pos.rn,map.sounds[main_idx].pos.x,map.sounds[main_idx].pos.z,&xadd,&zadd);

				map.sounds[map.nsound]=map.sounds[main_idx];
				map.sounds[map.nsound].pos.x+=xadd;
				map.sounds[map.nsound].pos.z+=zadd;
				select_duplicate_add(sound_piece,portal_idx,map.nsound,0);
				map.nsound++;
				break;
				
			case particle_piece:
				if (map.nparticle==max_map_particle) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Particle","\pYou've reached the maximum number of particles for a map.",NULL,NULL);
					return;
				}

				portal_duplicate_piece_offset(map.particles[main_idx].pos.rn,map.particles[main_idx].pos.x,map.particles[main_idx].pos.z,&xadd,&zadd);

				map.particles[map.nparticle]=map.particles[main_idx];
				map.particles[map.nparticle].pos.x+=xadd;
				map.particles[map.nparticle].pos.z+=zadd;
				select_duplicate_add(particle_piece,portal_idx,map.nparticle,0);
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
	int				type,portal_idx,main_idx,sub_idx;
	
	undo_clear();
	
		// only delete in polygon mode
		
	if (drag_mode!=drag_mode_polygon) return;
	
		// is a face selected?
		
	select_get(0,&type,&portal_idx,&main_idx,&sub_idx);
	if (type!=mesh_piece) return;
	if (sub_idx==-1) return;
	
	undo_save();
	
	map_portal_mesh_delete_poly(&map,portal_idx,main_idx,sub_idx);
	
	select_switch(0,mesh_piece,portal_idx,main_idx,0);
	
	main_wind_draw();
}

void piece_delete(void)
{
	int				n,i,k,nsel_count,
					type,portal_idx,main_idx,sub_idx;
	
	undo_save();
	
		// sort segment so higher indexes are deleted first
		
	select_sort();
	
		// delete selection
	
	nsel_count=select_count();
	
	for (n=0;n!=nsel_count;n++) {
		select_get(n,&type,&portal_idx,&main_idx,&sub_idx);
	
		switch (type) {
			
			case mesh_piece:
				if (drag_mode==drag_mode_polygon) {
					map_portal_mesh_delete_poly(&map,portal_idx,main_idx,sub_idx);
					break;
				}
				if (drag_mode==drag_mode_mesh) {
					map_portal_mesh_delete(&map,portal_idx,main_idx);
					break;
				}
				break;
				
			case liquid_piece:
				map_portal_liquid_delete(&map,portal_idx,main_idx);
				break;
				
			case spot_piece:
				for (i=main_idx;i<map.nspot;i++) {
					map.spots[i]=map.spots[i+1];
				}
				map.nspot--;
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
	int				n,sel_count,type,portal_idx,mesh_idx,poly_idx;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&portal_idx,&mesh_idx,&poly_idx);
		if (type==mesh_piece) map_portal_mesh_tesselate(&map,portal_idx,mesh_idx);
	}
	
	main_wind_draw();
}

/* =======================================================

      Piece Resize
      
======================================================= */

void piece_resize(void)
{
	int				n,sel_count,type,portal_idx,mesh_idx,poly_idx;
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
		select_get(n,&type,&portal_idx,&mesh_idx,&poly_idx);
		
		if (type==mesh_piece) {
			map_portal_mesh_calculate_extent(&map,portal_idx,mesh_idx,&min,&max);
			map_portal_mesh_calculate_center(&map,portal_idx,mesh_idx,&mpt);
						
			min.x=((min.x-mpt.x)*fct_x)+mpt.x;
			min.y=((min.y-mpt.y)*fct_y)+mpt.y;
			min.z=((min.z-mpt.z)*fct_z)+mpt.z;
			
			max.x=((max.x-mpt.x)*fct_x)+mpt.x;
			max.y=((max.y-mpt.y)*fct_y)+mpt.y;
			max.z=((max.z-mpt.z)*fct_z)+mpt.z;

			map_portal_mesh_resize(&map,portal_idx,mesh_idx,&min,&max);
		}
	}
	
	main_wind_draw();
}

/* =======================================================

      Piece Transforms
      
======================================================= */

void piece_flip(bool flip_x,bool flip_y,bool flip_z)
{
	int				n,sel_count,type,portal_idx,mesh_idx,poly_idx;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&portal_idx,&mesh_idx,&poly_idx);
		if (type==mesh_piece) map_portal_mesh_flip(&map,portal_idx,mesh_idx,flip_x,flip_y,flip_z);
	}
	
	main_wind_draw();
}

void piece_rotate(float rot_x,float rot_y,float rot_z)
{
	int				n,sel_count,type,portal_idx,mesh_idx,poly_idx;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&portal_idx,&mesh_idx,&poly_idx);
		if (type==mesh_piece) map_portal_mesh_rotate(&map,portal_idx,mesh_idx,FALSE,rot_x,rot_y,rot_z);
	}
	
	main_wind_draw();
}

void piece_free_rotate(void)
{
	float			rot_x,rot_y,rot_z;
	
	if (!dialog_free_rotate_run(&rot_x,&rot_y,&rot_z)) return;
	
	piece_rotate(rot_x,rot_y,rot_z);
}

/* =======================================================

      Snap Mesh to Grid
      
======================================================= */

void piece_snap_to_grid(void)
{
	int						n,k,sel_count,
							type,portal_idx,mesh_idx,poly_idx,nvertex;
	d3pnt					*pt;
	portal_type				*portal;
	map_mesh_type			*mesh;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&portal_idx,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;

		portal=&map.portals[portal_idx];
		mesh=&portal->mesh.meshes[mesh_idx];

		nvertex=mesh->nvertex;
		pt=mesh->vertexes;

		for (k=0;k!=nvertex;k++) {
			walk_view_click_grid(pt);
			pt++;
		}
	}
}

/* =======================================================

      Reset Mesh UVs
      
======================================================= */

void piece_reset_uvs(bool poly_only)
{
	int						n,sel_count,
							type,portal_idx,mesh_idx,poly_idx;

	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&portal_idx,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
		if (map.portals[portal_idx].mesh.meshes[mesh_idx].flag.lock_uv) continue;
		
		if (poly_only) {
			map_portal_mesh_reset_poly_uv(&map,portal_idx,mesh_idx,poly_idx);
		}
		else {
			map_portal_mesh_reset_uv(&map,portal_idx,mesh_idx);
		}
	}
}

/* =======================================================

      Piece Holes
      
======================================================= */

void piece_poly_hole(int hole_type)
{
	int				n,sel_count,type,portal_idx,mesh_idx,poly_idx;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&portal_idx,&mesh_idx,&poly_idx);
		if (type==mesh_piece) map_portal_mesh_poly_punch_hole(&map,portal_idx,mesh_idx,poly_idx,hole_type);
	}
	
	select_clear();
	
	main_wind_draw();
}

/* =======================================================

      Piece Keys
      
======================================================= */

void piece_key(editor_3D_view_setup *view_setup,int view_move_dir,char ch)
{
	int				mv,xadd,yadd,zadd;
	
	mv=walk_view_get_grid();
	if (main_wind_shift_down()) mv*=10;
		
	switch (ch) {
	
		case 0x1C:
			walk_view_click_drag_movement(view_setup,view_move_dir,mv,0,&xadd,&yadd,&zadd);
			select_move(cr,xadd,yadd,zadd);
			main_wind_draw();
			break;
			
		case 0x1D:
			walk_view_click_drag_movement(view_setup,view_move_dir,-mv,0,&xadd,&yadd,&zadd);
			select_move(cr,xadd,yadd,zadd);
			main_wind_draw();
			break;
			
		case 0x1E:
			walk_view_click_drag_movement(view_setup,view_move_dir,0,mv,&xadd,&yadd,&zadd);
			select_move(cr,xadd,yadd,zadd);
			main_wind_draw();
			break;
			
		case 0x1F:
			walk_view_click_drag_movement(view_setup,view_move_dir,0,-mv,&xadd,&yadd,&zadd);
			select_move(cr,xadd,yadd,zadd);
			main_wind_draw();
			break;
			
		case 8:
			piece_delete();
			main_wind_draw();
			menu_fix_enable();
			main_wind_tool_fix_enable();
			break;
	}
}

