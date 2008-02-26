/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Portal View Drawing Routines

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
#include "portal_view.h"

extern int					cr,cx,cz,magnify_factor;
extern float				walk_view_y_angle;

extern map_type				map;

/* =======================================================

      Portal Sizes
      
======================================================= */

void portal_view_get_portal(int rn,int *x,int *z,int *ex,int *ez)
{
	portal_type		*portal;
	
	portal=&map.portals[rn];
	
	*x=portal->x;
	*z=portal->z;
	portal_view_map_to_pane(x,z);
	
	*ex=portal->ex;
	*ez=portal->ez;
	portal_view_map_to_pane(ex,ez);
}

/* =======================================================

      Draw Portal View Mesh
      
======================================================= */

void portal_view_draw_mesh(int rn)
{
	int							n,k,t,x,z;
	d3pnt						*pt;
	portal_type					*portal;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*mesh_poly;
	
	portal=&map.portals[rn];

		// draw mesh outlines
		
	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			glBegin(GL_LINE_LOOP);
			
			for (t=0;t!=mesh_poly->ptsz;t++) {
				pt=&mesh->vertexes[mesh_poly->v[t]];
				x=pt->x+portal->x;
				z=pt->z+portal->z;
				portal_view_map_to_pane(&x,&z);
				glVertex2i(x,z);
			}
			
			glEnd();
		
			mesh_poly++;
		}

		mesh++;
	}
}

/* =======================================================

      Draw Portal Pieces
      
======================================================= */

void portal_view_draw_portals(void)
{
	int				n,x,z,ex,ez;
	
		// draw portal block and mesh
		
	for (n=0;n!=map.nportal;n++) {
		portal_view_get_portal(n,&x,&z,&ex,&ez);
	
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	
		glBegin(GL_QUADS);
		glVertex2i(x,z);
		glVertex2i(ex,z);
		glVertex2i(ex,ez);
		glVertex2i(x,ez);
		glEnd();
		
		glColor4f(0.5f,0.5f,1.0f,1.0f);
	
		portal_view_draw_mesh(n);
	}

		// draw portal outlines
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	for (n=0;n!=map.nportal;n++) {
		portal_view_get_portal(n,&x,&z,&ex,&ez);
	
		glBegin(GL_LINE_LOOP);
		glVertex2i(x,z);
		glVertex2i(ex,z);
		glVertex2i(ex,ez);
		glVertex2i(x,ez);
		glEnd();
	}
}

void portal_view_draw_selection(void)
{
	int			x,z,ex,ez;
	
	if (cr==-1) return;
	
		// draw portal selection
		
	portal_view_get_portal(cr,&x,&z,&ex,&ez);

	glLineWidth(2.0f);
	glColor4f(1.0f,0.0f,1.0f,0.75f);
	
	glBegin(GL_LINE_LOOP);
	glVertex2i(x,z);
	glVertex2i(ex,z);
	glVertex2i(ex,ez);
	glVertex2i(x,ez);
	glEnd();
	
	glLineWidth(1.0f);
	
	glBegin(GL_QUADS);
	
	glVertex2i((x-5),(z-5));
	glVertex2i((x+5),(z-5));
	glVertex2i((x+5),(z+5));
	glVertex2i((x-5),(z+5));
	
	glVertex2i((ex-5),(ez-5));
	glVertex2i((ex+5),(ez-5));
	glVertex2i((ex+5),(ez+5));
	glVertex2i((ex-5),(ez+5));
	
	glEnd();
}	

/* =======================================================

      Draw Portal Position
      
======================================================= */

void portal_view_draw_position(void)
{
    int			x,z,sz;
   
    x=cx;
    z=cz;
	portal_view_map_to_pane(&x,&z);
	
	sz=10;
	
	glColor4f(0.0f,0.0f,0.0f,0.75f);

	glLineWidth(3.0f);
	
	glBegin(GL_LINES);
	glVertex2i(x,(z-sz));
	glVertex2i(x,(z+sz));
	glVertex2i((x-sz),z);
	glVertex2i((x+sz),z);
	glEnd();
	
	glLineWidth(1.0f);
}

