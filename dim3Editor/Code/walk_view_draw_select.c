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

extern map_type				map;

/* =======================================================

      Draw Selection for Segment
      
======================================================= */

void walk_view_draw_select_mesh(int rn,d3pnt *cpt,int mesh_idx,int poly_idx)
{
	int						n,k,t,x,y,z;
	d3pnt					*pt;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	
	portal=&map.portals[rn];
	mesh=&portal->mesh.meshes[mesh_idx];
	
		// draw selected mesh
		
	glColor4f(1.0f,1.0f,0.0f,1.0f);
	
	glDisable(GL_DEPTH_TEST);
	
	mesh_poly=mesh->polys;
	
	for (k=0;k!=mesh->npoly;k++) {
	
		glBegin(GL_LINE_LOOP);
		
		for (t=0;t!=mesh_poly->ptsz;t++) {
			pt=&mesh->vertexes[mesh_poly->v[t]];
			x=(pt->x+portal->x)-cpt->x;
			y=pt->y-cpt->y;
			z=cpt->z-(pt->z+portal->z);
			glVertex3i(x,y,z);
		}
		
		glEnd();
		
		mesh_poly++;
	}
	
		// draw selected mesh poly
		
	glColor4f(1.0f,0.0f,0.0f,1.0f);
	glLineWidth(2.0f);
	
	mesh_poly=&mesh->polys[poly_idx];
	
	glBegin(GL_LINE_LOOP);
	
	for (t=0;t!=mesh_poly->ptsz;t++) {
		pt=&mesh->vertexes[mesh_poly->v[t]];
		x=(pt->x+portal->x)-cpt->x;
		y=pt->y-cpt->y;
		z=cpt->z-(pt->z+portal->z);
		glVertex3i(x,y,z);
	}
	
	glEnd();

	glLineWidth(1.0f);
	
		// draw the vertexes
		
	pt=mesh->vertexes;
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	glPointSize(walk_view_handle_size);
	
	glBegin(GL_POINTS);

	for (n=0;n!=mesh->nvertex;n++) {
		x=(pt->x+portal->x)-cpt->x;
		y=pt->y-cpt->y;
		z=cpt->z-(pt->z+portal->z);
		glVertex3i(x,y,z);
		
		pt++;
	}

	glEnd();
}

/* =======================================================

      Draw Selection for Sprite
      
======================================================= */

void walk_view_draw_select_sprite(d3pnt *cpt,d3pos *pos)
{
    int			x,z,y,wid,high;
	portal_type	*portal;
	
	portal=&map.portals[pos->rn];
  
    x=(pos->x+portal->x)-cpt->x;
    y=(pos->y+1)-cpt->y;
    z=(pos->z+portal->z)-cpt->z;
    
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

      Draw Select for Portal
      
======================================================= */

void walk_view_draw_select_portal(int rn,d3pnt *cpt)
{
	int						n,sel_count,
							type,portal_idx, main_idx,sub_idx;
	
	sel_count=select_count();
	if (sel_count==0) return;
	
		// draw the selection
		
	for (n=(sel_count-1);n>=0;n--) {
	
		select_get(n,&type,&portal_idx,&main_idx,&sub_idx);
		if (portal_idx!=rn) continue;
		
			// draw selection
			
		switch (type) {
		
			case mesh_piece:
				walk_view_draw_select_mesh(rn,cpt,main_idx,sub_idx);
				break;
				
			case node_piece:
				walk_view_draw_select_sprite(cpt,&map.nodes[main_idx].pos);
				break;
				
			case spot_piece:
				if (!walk_view_model_draw_select(cpt,&map.spots[main_idx].pos,&map.spots[main_idx].ang,map.spots[main_idx].display_model)) {
					walk_view_draw_select_sprite(cpt,&map.spots[main_idx].pos);
				}
				break;
				
			case scenery_piece:
				if (!walk_view_model_draw_select(cpt,&map.sceneries[main_idx].pos,&map.sceneries[main_idx].ang,map.sceneries[main_idx].model_name)) {
					walk_view_draw_select_sprite(cpt,&map.sceneries[main_idx].pos);
				}
				break;
				
			case light_piece:
				walk_view_draw_select_sprite(cpt,&map.lights[main_idx].pos);
				break;
				
			case sound_piece:
				walk_view_draw_select_sprite(cpt,&map.sounds[main_idx].pos);
				break;
				
			case particle_piece:
				walk_view_draw_select_sprite(cpt,&map.particles[main_idx].pos);
				break;
		}
	}
}
			
