/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Piece Click Routines

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
#include "walk_view.h"

extern int					cr,cx,cy,cz,magnify_factor,drag_mode,grid_mode;
extern bool					dp_liquid,dp_object,dp_lightsoundparticle,dp_node;
extern Rect					main_wind_box;

extern CCrsrHandle			towardcur,dragcur;

extern map_type				map;

int							walk_view_vport[4];
double						walk_view_mod_matrix[16],walk_view_proj_matrix[16];

/* =======================================================

      Click Utilities
      
======================================================= */

void walk_view_click_setup_project(editor_3D_view_setup *view_setup)
{
	int				rn;
	
		// setup walk view
		
	rn=walk_view_find_start_portal();
	walk_view_sight_path_mark(rn);
	
	main_wind_set_viewport(&view_setup->box,0.75f);
	main_wind_set_3D_projection(&view_setup->box,&view_setup->ang,view_setup->fov,walk_view_near_z,walk_view_far_z,walk_view_near_offset);
	
		// get projection
		
	glGetDoublev(GL_MODELVIEW_MATRIX,walk_view_mod_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,walk_view_proj_matrix);
	glGetIntegerv(GL_VIEWPORT,(GLint*)walk_view_vport);
}

bool walk_view_click_rotate_polygon_behind_z(int x,int y,int z)
{
	int				rz;
	double			dx,dy,dz;
	
	dx=(double)x;
	dy=(double)y;
	dz=(double)z;
	
	rz=-(int)((dx*walk_view_mod_matrix[2])+(dy*walk_view_mod_matrix[6])+(dz*walk_view_mod_matrix[10])+walk_view_mod_matrix[14]);
	return(rz<=walk_view_near_z);
}

void walk_view_click_project_point(Rect *box,int *x,int *y,int *z)
{
	double		dx,dy,dz;
	
	gluProject(*x,*y,*z,walk_view_mod_matrix,walk_view_proj_matrix,(GLint*)walk_view_vport,&dx,&dy,&dz);
	*x=((int)dx)-box->left;
	*y=(main_wind_box.bottom-((int)dy))-box->top;
	*z=(int)((dz)*10000.0f);
}

/* =======================================================

      Walk View Drag Movement
      
======================================================= */

void walk_view_click_drag_movement(editor_3D_view_setup *view_setup,int view_move_dir,int x,int y,int *xadd,int *yadd,int *zadd)
{
	int				sz;
	
	switch (view_move_dir) {
	
		case vm_dir_forward:
			*xadd=-(x*25);
			*yadd=-(y*25);
			*zadd=0;
			rotate_2D_point_center(xadd,zadd,-view_setup->ang.y);
			return;
			
		case vm_dir_side:
			*xadd=x*25;
			*yadd=-(y*25);
			*zadd=0;
			rotate_2D_point_center(xadd,zadd,view_setup->ang.y);
			return;
			
		case vm_dir_top:
			sz=(magnify_factor_max-magnify_factor)>>1;
			if (sz<10) sz=10;
			
			*xadd=-(x*sz);
			*yadd=0;
			*zadd=-(y*sz);
			rotate_2D_point_center(xadd,zadd,view_setup->ang.y);
			return;
			
		default:
			*xadd=*yadd=*zadd=0;
			return;
	}
}

void walk_view_click_grid(d3pnt *pt)
{
	int			sz;
	
	switch (grid_mode) {
		case grid_mode_none:
			return;
		case grid_mode_small:
			sz=map_enlarge;
			break;
		case grid_mode_medium:
			sz=map_enlarge<<1;
			break;
		case grid_mode_large:
			sz=map_enlarge<<3;
			break;
	}
	
	pt->x/=sz;
	pt->y/=sz;
	pt->z/=sz;
	
	pt->x*=sz;
	pt->y*=sz;
	pt->z*=sz;
}

/* =======================================================

      Piece Dragging
      
======================================================= */

bool walk_view_piece_drag(Point pt)
{
	int						x,y,xadd,zadd,yadd,dv,maxx,maxz;
	Point					oldpt;
	bool					first_drag,ctrl_key;
	MouseTrackingResult		track;
	
    if (!Button()) return(FALSE);
	
	undo_set_segment_move();
	
	maxx=map.portals[cr].ex-map.portals[cr].x;
	maxz=map.portals[cr].ez-map.portals[cr].z;
   
	first_drag=TRUE;
	ctrl_key=main_wind_control_down();
	
	oldpt=pt;
	dv=ctrl_key?2:5;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);
		
		if ((pt.h==oldpt.h) && (pt.v==oldpt.v)) continue;
		
		x=(oldpt.h-pt.h)/dv;
		y=(oldpt.v-pt.v)/dv;
		if ((x==0) && (y==0)) continue;
		
		oldpt=pt;
		
			// turn on drag cursor
			
		if (first_drag) {
			SetCCursor(ctrl_key?towardcur:dragcur);
			first_drag=FALSE;
		}
		
			// move item

		walk_view_get_piece_movement(ctrl_key,x,y,&xadd,&zadd,&yadd);
		select_move(cr,xadd,zadd,yadd);

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	InitCursor();
	
	return(!first_drag);
}

/* =======================================================

      View Polygon Clicking
      
======================================================= */

bool walk_view_poly_click_index(editor_3D_view_setup *view_setup,d3pnt *click_pt,portal_type *portal,map_mesh_type *mesh,int poly_idx,int *hit_z)
{
	int					t,fz,px[8],py[8],pz[8];
	bool				off_left,off_right,off_top,off_bottom;
	d3pnt				*pt;
	map_mesh_poly_type	*mesh_poly;

	mesh_poly=&mesh->polys[poly_idx];

	fz=0;

	for (t=0;t!=mesh_poly->ptsz;t++) {
		pt=&mesh->vertexes[mesh_poly->v[t]];
		px[t]=(pt->x+portal->x)-view_setup->cpt.x;
		py[t]=pt->y-view_setup->cpt.y;
		pz[t]=view_setup->cpt.z-(pt->z+portal->z);
		
		if (walk_view_click_rotate_polygon_behind_z(px[t],py[t],pz[t])) return(FALSE);
				
		walk_view_click_project_point(&view_setup->box,&px[t],&py[t],&pz[t]);
		fz+=pz[t];
	}
	
		// check if outside box
		
	off_left=off_right=off_top=off_bottom=TRUE;
	
	for (t=0;t!=mesh_poly->ptsz;t++) {
		off_left=off_left&&(px[t]<0);
		off_right=off_right&&(px[t]>(view_setup->box.right-view_setup->box.left));
		off_top=off_top&&(py[t]<0);
		off_bottom=off_bottom&&(py[t]>(view_setup->box.bottom-view_setup->box.top));
	}
	
	if ((off_left) || (off_right) || (off_top) || (off_bottom)) return(FALSE);
	
		// check hits
		
	if (!polygon_2D_point_inside(mesh_poly->ptsz,px,py,click_pt->x,click_pt->y)) return(FALSE);
	
	*hit_z=fz/mesh_poly->ptsz;
	return(TRUE);
}

void walk_view_mesh_click_index(editor_3D_view_setup *view_setup,d3pnt *click_pt,int *type,int *portal_idx,int *main_idx,int *sub_idx,bool sel_only)
{
	int					i,n,k,fz,box_wid,box_high,hit_z;
	portal_type			*portal;
	map_mesh_type		*mesh;
	
	walk_view_click_setup_project(view_setup);
	
	box_wid=view_setup->box.right-view_setup->box.left;
	box_high=view_setup->box.bottom-view_setup->box.top;
	
	*type=-1;
	hit_z=100000;
	
		// run through the portal meshes
		
	for (i=0;i!=map.nportal;i++) {
		portal=&map.portals[i];
        if (!portal->in_path) continue;
		
		mesh=portal->mesh.meshes;
		
		for (n=0;n!=portal->mesh.nmesh;n++) {
		
			for (k=0;k!=mesh->npoly;k++) {
			
				if (walk_view_poly_click_index(view_setup,click_pt,portal,mesh,k,&fz)) {
					if (fz<hit_z) {
						hit_z=fz;
						*type=mesh_piece;
						*portal_idx=i;
						*main_idx=n;
						*sub_idx=k;
					}
				}

			}
		
			mesh++;
		}
	}
}
	
void walk_view_click_piece_normal(editor_3D_view_setup *view_setup,d3pnt *pt,bool dblclick)
{
	int				type,portal_idx,main_idx,sub_idx;
	
		// anything clicked?
		
	walk_view_mesh_click_index(view_setup,pt,&type,&portal_idx,&main_idx,&sub_idx,FALSE);
	
		// if a selection, make sure in right portal
		
	if (type!=-1) cr=portal_idx;
	
		// add to selection
		
	if (!main_wind_shift_down()) {
		if (!select_check(type,portal_idx,main_idx,sub_idx)) {			// keep selection if selecting an already selected piece
			select_clear();	
			select_add(type,portal_idx,main_idx,sub_idx);
		}
	}
	else {
		select_flip(type,portal_idx,main_idx,sub_idx);
	}
	
		// redraw
		
	menu_fix_enable();
	main_wind_tool_fix_enable();
	
	main_wind_draw();
	texture_palette_reset();
}

/* =======================================================

      Double Clicking
      
======================================================= */

void walk_view_click_info(void)
{
	int				type,portal_idx,main_idx,sub_idx;
	
    if (select_count()!=1) return;
	
	select_get(0,&type,&portal_idx,&main_idx,&sub_idx);
/* supergumba
	switch (type) {
	
		case segment_piece:
		case primitive_piece:
			dialog_segment_setting_run(index);
			break;
			
		case node_piece:
			dialog_node_settings_run(&map.nodes[index]);
			break;
		
		case spot_piece:
			dialog_spot_setting_run(&map.spots[index]);
			break;
			
		case scenery_piece:
			dialog_scenery_setting_run(&map.sceneries[index]);
			break;
		
		case light_piece:
			dialog_map_light_settings_run(&map.lights[index]);
			break;
		
		case sound_piece:
			dialog_map_sound_settings_run(&map.sounds[index]);
			break;
			
		case particle_piece:
			dialog_map_particle_settings_run(&map.particles[index]);
			break;
	}
*/

}

/* =======================================================

      View Map Clicking
      
======================================================= */

void walk_view_click_piece(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir,bool dblclick)
{
		// put click within box
		
	pt->x-=view_setup->box.left;
	pt->y-=view_setup->box.top;
	
		// any vertex drags
		
	if (drag_mode==drag_mode_vertex) {
		if (walk_view_click_drag_vertex(view_setup,pt,view_move_dir)) return;
	}
	
		// select mesh/polygon
		
	walk_view_click_piece_normal(view_setup,pt,dblclick);

		// double-click info
		
	if (dblclick) {
		walk_view_click_info();
		return;
	}
	
		// do any mesh or poly drags
		
	switch (drag_mode) {
	
		case drag_mode_mesh:
			if (walk_view_click_drag_mesh(view_setup,pt,view_move_dir)) return;
			break;

		case drag_mode_polygon:
			if (walk_view_click_drag_mesh_poly(view_setup,pt,view_move_dir)) return;
			break;
			
	}
}



