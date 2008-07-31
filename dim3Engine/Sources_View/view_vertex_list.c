/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Portal Vertex Lists

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "lights.h"

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

GLuint						vertex_vbo=-1;		// supergumba

extern void map_calculate_light_color_normal(double x,double y,double z,float *cf,float *nf);

/* =======================================================

      Compile Portal OpenGL Lists
      
======================================================= */

void view_compile_mesh_gl_lists(int tick,int mesh_cnt,int *mesh_list)
{
	int									n,k,t,ntrig,
										v_idx,v_light_start_idx;
	float								x,y,z,fx,fy,fz;
	float								*pv,*pp,*pc,*pn;
	d3pnt								*pnt;
	map_mesh_type						*mesh;
	map_mesh_poly_type					*poly;
	map_mesh_poly_tessel_vertex_type	*lv;

		// the arrays

	pv=(float*)map.vertexes.pvert;
	pp=(float*)map.vertexes.pcoord;
	pc=(float*)map.vertexes.pcolor;
	pn=(float*)map.vertexes.pnormal;

		// the eye offset

	fx=(float)view.camera.pnt.x;
	fy=(float)view.camera.pnt.y;
	fz=(float)view.camera.pnt.z;

	v_idx=0;
	
		// run throught the vertexes

	for (n=0;n!=mesh_cnt;n++) {
		mesh=&map.mesh.meshes[mesh_list[n]];
		
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
				// polygon vertexes

				// these vertexes are used to draw the textured
				// polygons -- there's no normal or lighting usage on these
				// vertexes, so we skip that step
				
			for (t=0;t!=poly->ptsz;t++) {
			
				pnt=&mesh->vertexes[poly->v[t]];
				
				x=(float)pnt->x;
				y=(float)pnt->y;
				z=(float)pnt->z;

				pc+=3;
				pn+=3;

				*pv++=(x-fx);
				*pv++=(y-fy);
				*pv++=(fz-z);
				
				*pp++=poly->gx[t];
				*pp++=poly->gy[t];
				
				poly->draw.portal_v[t]=v_idx;
				
				v_idx++;
			}
			
				// polygon tesseled lighting

				// these vertexes are used for tesselated effects
				// like lighting, bump, specular, etc

			v_light_start_idx=v_idx;

			lv=poly->light.vertexes;

				// ray traced version

			if (setup.ray_trace_lighting) {

				for (t=0;t!=poly->light.nvertex;t++) {

					x=(float)lv->x;
					y=(float)lv->y;
					z=(float)lv->z;

					map_calculate_ray_trace_light_color_normal((double)x,(double)y,(double)z,pc,pn);
					pc+=3;
					pn+=3;

					*pv++=(x-fx);
					*pv++=(y-fy);
					*pv++=(fz-z);
					
					*pp++=lv->gx;
					*pp++=lv->gy;
					
					lv++;
					v_idx++;
				}


			}

				// regular lighting

			else {

				for (t=0;t!=poly->light.nvertex;t++) {

					x=(float)lv->x;
					y=(float)lv->y;
					z=(float)lv->z;

					map_calculate_light_color_normal((double)x,(double)y,(double)z,pc,pn);
					pc+=3;
					pn+=3;

					*pv++=(x-fx);
					*pv++=(y-fy);
					*pv++=(fz-z);
					
					*pp++=lv->gx;
					*pp++=lv->gy;
					
					lv++;
					v_idx++;
				}

			}

				// create draw indexes by offset

			ntrig=poly->light.ntrig*3;

			for (t=0;t!=ntrig;t++) {
				poly->light.trig_vertex_draw_idx[t]=poly->light.trig_vertex_idx[t]+v_light_start_idx;
			}

			poly++;
		}
		
		mesh++;
	}

	map.vertexes.draw_vertex_count=v_idx;
}

/* =======================================================

      Use Map OpenGL Lists
      
======================================================= */

void view_compile_gl_list_attach(void)
{

	// supergumba -- we'll want to map this data in client space


#ifdef D3_OS_MAC
	glVertexArrayRangeAPPLE(sz,map.vertexes.pvert);
	glEnableClientState(GL_VERTEX_ARRAY_RANGE_APPLE);
#endif

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,map.vertexes.pvert);


#ifdef D3_OS_MAC
	glFlushVertexArrayRangeAPPLE(sz,map.vertexes.pvert);
#endif

/*
	if (vertex_vbo==-1) {
		glGenBuffersARB(1,&vertex_vbo);
	}
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vertex_vbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,((map.vertexes.draw_vertex_count*3)sizeof(float)),map.vertexes.pvert,GL_STREAM_DRAW_ARB);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);
*/


		// uvs
		
	glClientActiveTexture(GL_TEXTURE2);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,map.vertexes.pcoord);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,map.vertexes.pcoord);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,map.vertexes.pcoord);

		// color array

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,map.vertexes.pcolor);
}

void view_compile_gl_list_dettach(void)
{
	glDisableClientState(GL_COLOR_ARRAY);

	glClientActiveTexture(GL_TEXTURE2);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_VERTEX_ARRAY);

#ifdef D3_OS_MAC
	glDisableClientState(GL_VERTEX_ARRAY_RANGE_APPLE);
#endif

//	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

}
