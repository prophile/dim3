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

extern int					cr,cx,cy,cz,magnify_factor,vertex_mode,drag_mode,grid_mode;
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

int walk_view_get_grid(void)
{
	switch (grid_mode) {
		case grid_mode_small:
			return(map_enlarge);
		case grid_mode_medium:
			return(map_enlarge<<1);
		case grid_mode_large:
			return(map_enlarge<<3);
	}

	return(1);
}

void walk_view_click_grid(d3pnt *pt)
{
	int			sz;
	
	sz=walk_view_get_grid();
	
	pt->x/=sz;
	pt->y/=sz;
	pt->z/=sz;
	
	pt->x*=sz;
	pt->y*=sz;
	pt->z*=sz;
}

void walk_view_click_snap(int portal_idx,int mesh_idx,int vertex_idx,d3pnt *pt,d3pnt *mpt)
{
	int				n,k,t,chk_x,chk_y,chk_z;
	d3pnt			*dpt;
	portal_type		*portal,*chk_portal;
	map_mesh_type	*mesh;
	
	if (vertex_mode!=vertex_mode_snap) return;
	
		// put vertexes in map space
		
	chk_portal=&map.portals[portal_idx];
	
	chk_x=(pt->x+mpt->x)+chk_portal->x;
	chk_y=pt->y+mpt->y;
	chk_z=(pt->z+mpt->z)+chk_portal->z;
	
		// any vertexes to snap to?
		
	portal=map.portals;
	
	for (n=0;n!=map.nportal;n++) {
	
		mesh=portal->mesh.meshes;
		
		for (k=0;k!=portal->mesh.nmesh;k++) {
			
			if ((n==portal_idx) && (k==mesh_idx)) {
				mesh++;
				continue;
			}
	
			dpt=mesh->vertexes;
			
			for (t=0;t!=mesh->nvertex;t++) {
			
				if (distance_get((dpt->x+portal->x),dpt->y,(dpt->z+portal->z),chk_x,chk_y,chk_z)<(map_enlarge*5)) {
					mpt->x=(dpt->x+portal->x)-(pt->x+chk_portal->x);
					mpt->y=dpt->y-pt->y;
					mpt->z=(dpt->z+portal->z)-(pt->z+chk_portal->z);
					return;
				}
				
				dpt++;
			}
		
			mesh++;
		}
		
		portal++;
	}
}

/* =======================================================

      View Piece Clicking Utility
      
======================================================= */

bool walk_view_mesh_poly_click_index(editor_3D_view_setup *view_setup,d3pnt *click_pt,portal_type *portal,map_mesh_type *mesh,int poly_idx,int *hit_z)
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

bool walk_view_quad_click_index(editor_3D_view_setup *view_setup,d3pnt *click_pt,portal_type *portal,int *px,int *py,int *pz,int *hit_z)
{
	int					t,fz;
	bool				off_left,off_right,off_top,off_bottom;
	
	fz=0;

	for (t=0;t!=4;t++) {
		if (walk_view_click_rotate_polygon_behind_z(px[t],py[t],pz[t])) return(FALSE);
		walk_view_click_project_point(&view_setup->box,&px[t],&py[t],&pz[t]);
		fz+=pz[t];
	}
	
		// check if outside box
		
	off_left=off_right=off_top=off_bottom=TRUE;
	
	for (t=0;t!=4;t++) {
		off_left=off_left&&(px[t]<0);
		off_right=off_right&&(px[t]>(view_setup->box.right-view_setup->box.left));
		off_top=off_top&&(py[t]<0);
		off_bottom=off_bottom&&(py[t]>(view_setup->box.bottom-view_setup->box.top));
	}
	
	if ((off_left) || (off_right) || (off_top) || (off_bottom)) return(FALSE);
	
		// check hits
		
	if (!polygon_2D_point_inside(4,px,py,click_pt->x,click_pt->y)) return(FALSE);
	
	*hit_z=fz/4;
	
	return(TRUE);
}

bool walk_view_cube_click_index(editor_3D_view_setup *view_setup,d3pnt *click_pt,portal_type *portal,int *x,int *z,int ty,int by,int *hit_z)
{
	int					px[4],py[4],pz[4];
	bool				hit;
	
	hit=FALSE;
	
		// top
	
	memmove(px,x,(4*sizeof(int)));
	memmove(pz,z,(4*sizeof(int)));
	py[0]=py[1]=py[2]=py[3]=ty;

	hit=hit||walk_view_quad_click_index(view_setup,click_pt,portal,px,py,pz,hit_z);
	
		// bottom
		
	memmove(px,x,(4*sizeof(int)));
	memmove(pz,z,(4*sizeof(int)));
	py[0]=py[1]=py[2]=py[3]=by;

	hit=hit||walk_view_quad_click_index(view_setup,click_pt,portal,px,py,pz,hit_z);
	
		// front
		
	px[0]=px[3]=x[3];
	px[1]=px[2]=x[2];
	pz[0]=pz[3]=z[3];
	pz[1]=pz[2]=z[2];
	py[0]=py[1]=ty;
	py[2]=py[3]=by;

	hit=hit||walk_view_quad_click_index(view_setup,click_pt,portal,px,py,pz,hit_z);

		// back
		
	px[0]=px[3]=x[0];
	px[1]=px[2]=x[1];
	pz[0]=pz[3]=z[0];
	pz[1]=pz[2]=z[1];
	py[0]=py[1]=ty;
	py[2]=py[3]=by;

	hit=hit||walk_view_quad_click_index(view_setup,click_pt,portal,px,py,pz,hit_z);
	
		// left
		
	px[0]=px[3]=x[0];
	px[1]=px[2]=x[3];
	pz[0]=pz[3]=z[0];
	pz[1]=pz[2]=z[3];
	py[0]=py[1]=ty;
	py[2]=py[3]=by;

	hit=hit||walk_view_quad_click_index(view_setup,click_pt,portal,px,py,pz,hit_z);

		// right
		
	px[0]=px[3]=x[1];
	px[1]=px[2]=x[2];
	pz[0]=pz[3]=z[1];
	pz[1]=pz[2]=z[2];
	py[0]=py[1]=ty;
	py[2]=py[3]=by;

	return(hit||walk_view_quad_click_index(view_setup,click_pt,portal,px,py,pz,hit_z));
}

bool walk_view_liquid_click(editor_3D_view_setup *view_setup,d3pnt *click_pt,portal_type *portal,map_liquid_type *liq,int *hit_z)
{
	int				px[4],py[4],pz[4];

	px[0]=px[3]=(liq->lft+portal->x)-view_setup->cpt.x;
	px[1]=px[2]=(liq->rgt+portal->x)-view_setup->cpt.x;
	py[0]=py[1]=py[2]=py[3]=liq->y-view_setup->cpt.y;
	pz[0]=pz[1]=view_setup->cpt.z-(liq->top+portal->z);
	pz[2]=pz[3]=view_setup->cpt.z-(liq->bot+portal->z);
	
	return(walk_view_quad_click_index(view_setup,click_pt,portal,px,py,pz,hit_z));
}

/* =======================================================

      View Piece Get Clicked Index
      
======================================================= */

void walk_view_mesh_click_index(editor_3D_view_setup *view_setup,d3pnt *click_pt,int *type,int *portal_idx,int *main_idx,int *sub_idx,bool sel_only)
{
	int					i,n,k,fz,box_wid,box_high,
						px[4],pz[4],ty,by,hit_z;
	portal_type			*portal;
	map_mesh_type		*mesh;
	spot_type			*spot;
	map_scenery_type	*scenery;
	map_light_type		*map_light;
	map_sound_type		*map_sound;
	map_particle_type	*map_particle;
	node_type			*node;
	
	walk_view_click_setup_project(view_setup);
	
	box_wid=view_setup->box.right-view_setup->box.left;
	box_high=view_setup->box.bottom-view_setup->box.top;
	
	*type=-1;
	hit_z=100000;
	
		// run through the portals
		
	for (i=0;i!=map.nportal;i++) {
	
		portal=&map.portals[i];
        if (!portal->in_path) continue;
		
			// portal meshes
		
		mesh=portal->mesh.meshes;
		
		for (n=0;n!=portal->mesh.nmesh;n++) {
		
			for (k=0;k!=mesh->npoly;k++) {
			
				if (walk_view_mesh_poly_click_index(view_setup,click_pt,portal,mesh,k,&fz)) {
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
		
		if (dp_liquid) {
		
				// portal liquids
				
			for (n=0;n!=portal->liquid.nliquid;n++) {
				if (walk_view_liquid_click(view_setup,click_pt,portal,&portal->liquid.liquids[n],&fz)) {
					if (fz<hit_z) {
						hit_z=fz;
						*type=liquid_piece;
						*portal_idx=i;
						*main_idx=n;
						*sub_idx=-1;
					}
				}
			}
		}
		
		if (dp_object) {
		
				// portal spots
				
			for (n=0;n!=map.nspot;n++) {
				
				spot=&map.spots[n];
				if (spot->pos.rn!=i)  continue;
				
				if (!walk_view_model_click_select_size(&view_setup->cpt,spot->display_model,&spot->pos,&spot->ang,px,pz,&ty,&by)) continue;
				
				if (walk_view_cube_click_index(view_setup,click_pt,portal,px,pz,ty,by,&fz)) {
					if (fz<hit_z) {
						hit_z=fz;
						*type=spot_piece;
						*portal_idx=i;
						*main_idx=n;
						*sub_idx=-1;
					}
				}
			}
		
				// portal scenery
				
			for (n=0;n!=map.nscenery;n++) {
				
				scenery=&map.sceneries[n];
				if (scenery->pos.rn!=i)  continue;
				
				if (!walk_view_model_click_select_size(&view_setup->cpt,scenery->model_name,&scenery->pos,&scenery->ang,px,pz,&ty,&by)) continue;
				
				if (walk_view_cube_click_index(view_setup,click_pt,portal,px,pz,ty,by,&fz)) {
					if (fz<hit_z) {
						hit_z=fz;
						*type=scenery_piece;
						*portal_idx=i;
						*main_idx=n;
						*sub_idx=-1;
					}
				}
			}
		}
		
		if (dp_lightsoundparticle) {
		
				// map lights
				
			for (n=0;n!=map.nlight;n++) {
			
				map_light=&map.lights[n];
				if (map_light->pos.rn!=i) continue;
				
				walk_view_sprite_select_size(&view_setup->cpt,&map_light->pos,px,pz,&ty,&by);
				
				if (walk_view_cube_click_index(view_setup,click_pt,portal,px,pz,ty,by,&fz)) {
					if (fz<hit_z) {
						hit_z=fz;
						*type=light_piece;
						*portal_idx=i;
						*main_idx=n;
						*sub_idx=-1;
					}
				}
			}
			
				// map sound
				
			for (n=0;n!=map.nsound;n++) {
			
				map_sound=&map.sounds[n];
				if (map_sound->pos.rn!=i) continue;
				
				walk_view_sprite_select_size(&view_setup->cpt,&map_sound->pos,px,pz,&ty,&by);
				
				if (walk_view_cube_click_index(view_setup,click_pt,portal,px,pz,ty,by,&fz)) {
					if (fz<hit_z) {
						hit_z=fz;
						*type=sound_piece;
						*portal_idx=i;
						*main_idx=n;
						*sub_idx=-1;
					}
				}
			}
			
				// map particle
				
			for (n=0;n!=map.nparticle;n++) {
			
				map_particle=&map.particles[n];
				if (map_particle->pos.rn!=i) continue;
				
				walk_view_sprite_select_size(&view_setup->cpt,&map_particle->pos,px,pz,&ty,&by);
				
				if (walk_view_cube_click_index(view_setup,click_pt,portal,px,pz,ty,by,&fz)) {
					if (fz<hit_z) {
						hit_z=fz;
						*type=particle_piece;
						*portal_idx=i;
						*main_idx=n;
						*sub_idx=-1;
					}
				}
			}
		}
		
		if (dp_node) {
		
				// map node
				
			for (n=0;n!=map.nnode;n++) {
			
				node=&map.nodes[n];
				if (node->pos.rn!=i) continue;
				
				walk_view_sprite_select_size(&view_setup->cpt,&node->pos,px,pz,&ty,&by);
				
				if (walk_view_cube_click_index(view_setup,click_pt,portal,px,pz,ty,by,&fz)) {
					if (fz<hit_z) {
						hit_z=fz;
						*type=node_piece;
						*portal_idx=i;
						*main_idx=n;
						*sub_idx=-1;
					}
				}
			}
		}	
	}
}
	
void walk_view_click_piece_normal(editor_3D_view_setup *view_setup,d3pnt *pt,bool dblclick)
{
	int				type,portal_idx,main_idx,sub_idx;
	
		// anything clicked?
		
	walk_view_mesh_click_index(view_setup,pt,&type,&portal_idx,&main_idx,&sub_idx,FALSE);
	
		// if a selection, make sure right portal is selected
				
	if (type!=-1) cr=portal_idx;
	
		// clear or add to selection
		
	if (type==-1) {
		if (!main_wind_shift_down()) select_clear();
	}
	else {
		if (!main_wind_shift_down()) {
			if (!select_check(type,portal_idx,main_idx,sub_idx)) {			// keep selection if selecting an already selected piece
				select_clear();	
				select_add(type,portal_idx,main_idx,sub_idx);
			}
		}
		else {
			select_flip(type,portal_idx,main_idx,sub_idx);
		}
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
	map_mesh_type	*mesh;
	map_liquid_type	*liq;
	
    if (select_count()!=1) return;
	
	select_get(0,&type,&portal_idx,&main_idx,&sub_idx);

	switch (type) {
	
		case mesh_piece:
			mesh=&map.portals[portal_idx].mesh.meshes[main_idx];
			dialog_mesh_setting_run(mesh,sub_idx);
			break;
			
		case liquid_piece:
			liq=&map.portals[portal_idx].liquid.liquids[main_idx];
			dialog_liquid_settings_run(liq);
			break;
	
		case node_piece:
			dialog_node_settings_run(&map.nodes[main_idx]);
			break;
		
		case spot_piece:
			dialog_spot_setting_run(&map.spots[main_idx]);
			break;
			
		case scenery_piece:
			dialog_scenery_setting_run(&map.sceneries[main_idx]);
			break;
		
		case light_piece:
			dialog_map_light_settings_run(&map.lights[main_idx]);
			break;
		
		case sound_piece:
			dialog_map_sound_settings_run(&map.sounds[main_idx]);
			break;
			
		case particle_piece:
			dialog_map_particle_settings_run(&map.particles[main_idx]);
			break;
	}

}

/* =======================================================

      View Map Clicking
      
======================================================= */

void walk_view_click_piece(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir,bool dblclick)
{
		// put click within box
	
	pt->x-=view_setup->box.left;
	pt->y-=view_setup->box.top;
	
		// liquid vertex drags
		
	if (walk_view_click_drag_liquid_vertex(view_setup,pt,view_move_dir)) return;
	
		// mesh vertex drags
		
	switch (drag_mode) {
	
		case drag_mode_vertex:
			if (walk_view_click_drag_vertex(view_setup,pt,view_move_dir)) return;
			break;
	
		case drag_mode_mesh:
			if (walk_view_click_drag_mesh_handle(view_setup,pt,view_move_dir)) return;
			break;
			
	}
		
		// select mesh/polygon
		
	walk_view_click_piece_normal(view_setup,pt,dblclick);
	
		// return if no selection
		
	if (select_count()==0) return;

		// double-click info
		
	if (dblclick) {
		walk_view_click_info();
		return;
	}
	
		// do any item drags
			
	if (walk_view_click_drag_item(view_setup,pt,view_move_dir)) return;
	
		// do any liquid drags
		
	if (walk_view_click_drag_liquid(view_setup,pt,view_move_dir)) return;
	
		// do any mesh or poly drags
		
	switch (drag_mode) {
	
		case drag_mode_mesh:
			walk_view_click_drag_mesh(view_setup,pt,view_move_dir);
			break;

		case drag_mode_polygon:
			walk_view_click_drag_mesh_poly(view_setup,pt,view_move_dir);
			break;
			
	}
}



