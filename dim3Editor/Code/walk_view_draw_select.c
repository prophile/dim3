/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Selection Drawing Routines

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

#include "common_view.h"
#include "walk_view.h"

extern int					drag_mode;

extern map_type				map;
extern setup_type			setup;

/* =======================================================

      Draw Selection for Mesh
      
======================================================= */

void walk_view_draw_select_mesh_get_grow_handles(int mesh_idx,int *px,int *py,int *pz)
{
	d3pnt			min,max;
	
	map_mesh_calculate_extent(&map,mesh_idx,&min,&max);
	
	px[0]=px[3]=px[4]=px[7]=min.x;
	pz[0]=pz[1]=pz[4]=pz[5]=min.z;
	px[1]=px[2]=px[5]=px[6]=max.x;
	pz[2]=pz[3]=pz[6]=pz[7]=max.z;
	py[0]=py[1]=py[2]=py[3]=min.y;
	py[4]=py[5]=py[6]=py[7]=max.y;
}

void walk_view_draw_select_mesh(d3pnt *cpt,int mesh_idx)
{
	int						n,k,t,x,y,z,px[8],py[8],pz[8];
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	
	mesh=&map.mesh.meshes[mesh_idx];
	
		// draw selected mesh
		
	glColor4f(setup.col.mesh_sel.r,setup.col.mesh_sel.g,setup.col.mesh_sel.b,1.0f);
	
	glEnable(GL_DEPTH_TEST);
	
	mesh_poly=mesh->polys;
	
	for (k=0;k!=mesh->npoly;k++) {
	
		glBegin(GL_LINE_LOOP);
		
		for (t=0;t!=mesh_poly->ptsz;t++) {
			pt=&mesh->vertexes[mesh_poly->v[t]];
			x=pt->x-cpt->x;
			y=pt->y-cpt->y;
			z=cpt->z-pt->z;
			glVertex3i(x,y,z);
		}
		
		glEnd();
		
		mesh_poly++;
	}
	
		// is mesh only, draw resize handles
		
	if (drag_mode==drag_mode_mesh) {
	
		glEnable(GL_DEPTH_TEST);

		glColor4f(0.0f,0.0f,0.0f,1.0f);
		glPointSize(walk_view_handle_size);
		
		walk_view_draw_select_mesh_get_grow_handles(mesh_idx,px,py,pz);
		
		glBegin(GL_POINTS);

		for (n=0;n!=8;n++) {
			x=px[n]-cpt->x;
			y=py[n]-cpt->y;
			z=cpt->z-pz[n];
			glVertex3i(x,y,z);
		}

		glEnd();
	
		return;
	}
}

void walk_view_draw_select_mesh_vertex(d3pnt *cpt,int mesh_idx)
{
	int						n,x,y,z;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	
	glEnable(GL_DEPTH_TEST);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glPointSize(walk_view_handle_size);
	
	mesh=&map.mesh.meshes[mesh_idx];
	pt=mesh->vertexes;
	
	glBegin(GL_POINTS);

	for (n=0;n!=mesh->nvertex;n++) {
		x=pt->x-cpt->x;
		y=pt->y-cpt->y;
		z=cpt->z-pt->z;
		glVertex3i(x,y,z);
		
		pt++;
	}

	glEnd();
}

void walk_view_draw_select_mesh_poly(d3pnt *cpt,int mesh_idx,int poly_idx)
{
	int						n,x,y,z;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	
	glDisable(GL_DEPTH_TEST);
	
	glLineWidth(2.0f);

	glColor4f(setup.col.poly_sel.r,setup.col.poly_sel.g,setup.col.poly_sel.b,1.0f);
	
	mesh=&map.mesh.meshes[mesh_idx];
	mesh_poly=&mesh->polys[poly_idx];
	
	glBegin(GL_LINE_LOOP);
	
	for (n=0;n!=mesh_poly->ptsz;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n]];
		x=pt->x-cpt->x;
		y=pt->y-cpt->y;
		z=cpt->z-pt->z;
		glVertex3i(x,y,z);
	}
	
	glEnd();

	glLineWidth(1.0f);
}

/* =======================================================

      Draw Selection for Liquid
      
======================================================= */

void walk_view_draw_select_liquid_get_grow_handles(int liquid_idx,int *px,int *py,int *pz)
{
	map_liquid_type			*liq;
	
	liq=&map.liquid.liquids[liquid_idx];
	
	px[0]=px[3]=liq->lft;
	px[1]=px[2]=liq->rgt;
	pz[0]=pz[1]=liq->top;
	pz[2]=pz[3]=liq->bot;
	py[0]=py[1]=py[2]=py[3]=liq->y;
}

void walk_view_draw_select_liquid(d3pnt *cpt,int liquid_idx)
{
	int						n,x,y,z,px[4],py[4],pz[4];
	map_liquid_type			*liq;
	
	liq=&map.liquid.liquids[liquid_idx];
	
	glEnable(GL_DEPTH_TEST);

	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glPointSize(walk_view_handle_size);
		
	walk_view_draw_select_liquid_get_grow_handles(liquid_idx,px,py,pz);
		
	glBegin(GL_POINTS);

	for (n=0;n!=4;n++) {
		x=px[n]-cpt->x;
		y=py[n]-cpt->y;
		z=cpt->z-pz[n];
		glVertex3i(x,y,z);
	}

	glEnd();
}

/* =======================================================

      Draw Selection for Sprite
      
======================================================= */

void walk_view_sprite_select_size(d3pnt *cpt,d3pnt *pnt,int *px,int *pz,int *ty,int *by)
{
	int				x,y,z,wid;
	
    x=pnt->x-cpt->x;
    y=pnt->y-cpt->y;
    z=cpt->z-pnt->z;
    
    wid=map_enlarge*3;

	px[0]=px[3]=x-wid;
	px[1]=px[2]=x+wid;
	
	pz[0]=pz[1]=z-wid;
	pz[2]=pz[3]=z+wid;
	
	*by=y;
	*ty=y-(map_enlarge*4);
}

void walk_view_draw_select_sprite(d3pnt *cpt,d3pnt *pnt)
{
    int			x,z,y,wid,high;
  
    x=pnt->x-cpt->x;
    y=pnt->y-cpt->y;
    z=pnt->z-cpt->z;
    
    wid=map_enlarge*3;
    high=map_enlarge*4;
    
 	glColor4f(1,0,0,1);

	glEnable(GL_DEPTH_TEST);
	glLineWidth(2);
    
	glBegin(GL_LINE_LOOP);
	glVertex3i((x-wid),(y-high),-(z-wid));
	glVertex3i((x+wid),(y-high),-(z-wid));
	glVertex3i((x+wid),y,-(z-wid));
	glVertex3i((x-wid),y,-(z-wid));
	glEnd();

	glBegin(GL_LINE_LOOP);
 	glVertex3i((x-wid),(y-high),-(z+wid));
	glVertex3i((x+wid),(y-high),-(z+wid));
	glVertex3i((x+wid),y,-(z+wid));
	glVertex3i((x-wid),y,-(z+wid));
	glEnd();
	
	glBegin(GL_LINE_LOOP);
 	glVertex3i((x-wid),(y-high),-(z-wid));
	glVertex3i((x-wid),(y-high),-(z+wid));
	glVertex3i((x-wid),y,-(z+wid));
	glVertex3i((x-wid),y,-(z-wid));
	glEnd();
	
	glBegin(GL_LINE_LOOP);
	glVertex3i((x+wid),(y-high),-(z-wid));
	glVertex3i((x+wid),(y-high),-(z+wid));
	glVertex3i((x+wid),y,-(z+wid));
	glVertex3i((x+wid),y,-(z-wid));
	glEnd();
    
    glLineWidth(1);
}

/* =======================================================

      Draw Selections for Map
      
======================================================= */

void walk_view_draw_select(d3pnt *cpt)
{
	int						n,sel_count,
							type,main_idx,sub_idx;
	unsigned char			draw_mesh_once[max_mesh];
	
	sel_count=select_count();
	if (sel_count==0) return;
	
		// only draw mesh selections once
		// as there could be multiple polygon selections
		// for a single mesh
		
	memset(draw_mesh_once,0x0,max_mesh);
	
		// draw the selection of meshes, liquids,
		// and other pieces, drawing the selected
		// polygons later
		
	for (n=(sel_count-1);n>=0;n--) {
	
		select_get(n,&type,&main_idx,&sub_idx);
		
			// draw selection
			
		switch (type) {
		
			case mesh_piece:
				if (draw_mesh_once[main_idx]==0x0) {
					draw_mesh_once[main_idx]=0x1;
					walk_view_draw_select_mesh(cpt,main_idx);
				}
				break;
				
			case liquid_piece:
				walk_view_draw_select_liquid(cpt,main_idx);
				break;
				
			case node_piece:
				walk_view_draw_select_sprite(cpt,&map.nodes[main_idx].pnt);
				break;
				
			case spot_piece:
				if (!walk_view_model_draw_select(cpt,&map.spots[main_idx].pnt,&map.spots[main_idx].ang,map.spots[main_idx].display_model)) {
					walk_view_draw_select_sprite(cpt,&map.spots[main_idx].pnt);
				}
				break;
				
			case scenery_piece:
				if (!walk_view_model_draw_select(cpt,&map.sceneries[main_idx].pnt,&map.sceneries[main_idx].ang,map.sceneries[main_idx].model_name)) {
					walk_view_draw_select_sprite(cpt,&map.sceneries[main_idx].pnt);
				}
				break;
				
			case light_piece:
				walk_view_draw_select_sprite(cpt,&map.lights[main_idx].pnt);
				break;
				
			case sound_piece:
				walk_view_draw_select_sprite(cpt,&map.sounds[main_idx].pnt);
				break;
				
			case particle_piece:
				walk_view_draw_select_sprite(cpt,&map.particles[main_idx].pnt);
				break;
		}
	}
	
		// draw selected polygons
		// draw afterwards so mesh highlighting doesn't
		// effect selection
		
	if (drag_mode==drag_mode_polygon) {
	
		for (n=(sel_count-1);n>=0;n--) {
		
			select_get(n,&type,&main_idx,&sub_idx);
			if (type!=mesh_piece) continue;
			
			walk_view_draw_select_mesh_poly(cpt,main_idx,sub_idx);
		}
		
	}
	
		// finish with selecte mesh vertexes
		
	if (drag_mode==drag_mode_vertex) {

		memset(draw_mesh_once,0x0,max_mesh);
			
		for (n=(sel_count-1);n>=0;n--) {
		
			select_get(n,&type,&main_idx,&sub_idx);
			if (type!=mesh_piece) continue;
			
			if (draw_mesh_once[main_idx]==0x0) {
				draw_mesh_once[main_idx]=0x1;
				walk_view_draw_select_mesh_vertex(cpt,main_idx);
			}
		}
	}
}
			
