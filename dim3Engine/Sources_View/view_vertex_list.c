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
										v_count,v_idx,v_light_start_idx;
	float								x,y,z,fx,fy,fz;
	float								*pv,*pp,*pc,*pn;
	d3pnt								*pnt;
	map_mesh_type						*mesh;
	map_mesh_poly_type					*poly;
	map_mesh_poly_tessel_vertex_type	*lv;

		// find total number of vertexes in
		// this seen

	v_count=0;

	for (n=0;n!=mesh_cnt;n++) {
		mesh=&map.mesh.meshes[mesh_list[n]];
		
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			v_count+=poly->ptsz;
			v_count+=poly->light.nvertex;
			poly++;
		}
		
		mesh++;
	}

	map.vertexes.draw_vertex_count=v_count;



	
/* supergumba

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vertex_vbo);
		sz=(map.vertexes.draw_vertex_count*(3+2+3+3))*sizeof(float);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,NULL,GL_STREAM_DRAW_ARB);
	}

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vertex_vbo);
	ptr=(float*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
*/


		// arrays and offsets

	map.vertexes.vert.sz=(v_count*3)*sizeof(float);
	map.vertexes.vert.offset=0;

	pv=(float*)map.vertexes.ptr;

	map.vertexes.uv.sz=(v_count*2)*sizeof(float);
	map.vertexes.uv.offset=map.vertexes.vert.offset+map.vertexes.vert.sz;

	pp=pv+(v_count*3);

	map.vertexes.color.sz=(v_count*3)*sizeof(float);
	map.vertexes.color.offset=map.vertexes.uv.offset+map.vertexes.uv.sz;

	pc=pp+(v_count*2);

	map.vertexes.normal.sz=(v_count*3)*sizeof(float);
	map.vertexes.normal.offset=map.vertexes.color.offset+map.vertexes.color.sz;

	pn=pc+(v_count*3);

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
				
				*pp++=poly->gx[t]+poly->draw.x_shift_offset;
				*pp++=poly->gy[t]+poly->draw.y_shift_offset;
				
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
					
					*pp++=lv->gx+poly->draw.x_shift_offset;
					*pp++=lv->gy+poly->draw.y_shift_offset;
					
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
					
					*pp++=lv->gx+poly->draw.x_shift_offset;
					*pp++=lv->gy+poly->draw.y_shift_offset;
					
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


	/* supergumba
	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
	*/
}

/* =======================================================

      Use Map OpenGL Lists
      
======================================================= */

void view_compile_gl_list_attach(void)
{
	int					sz;


				// supergumba -- do this some other time
	if (vertex_vbo==-1) {
		glGenBuffersARB(1,&vertex_vbo);
	}


	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vertex_vbo);

		// data

	sz=(map.vertexes.draw_vertex_count*(3+2+3+3))*sizeof(float);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,(void*)map.vertexes.ptr,GL_STREAM_DRAW_ARB);

		// vertexes

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)map.vertexes.vert.offset);

		// uvs
		
	glClientActiveTexture(GL_TEXTURE2);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)map.vertexes.uv.offset);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)map.vertexes.uv.offset);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)map.vertexes.uv.offset);

		// color array

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,(void*)map.vertexes.color.offset);
}


void view_compile_gl_list_switch_to_color(void)
{
	glColorPointer(3,GL_FLOAT,0,(void*)map.vertexes.color.offset);
}

void view_compile_gl_list_switch_to_normal(void)
{
	glColorPointer(3,GL_FLOAT,0,(void*)map.vertexes.normal.offset);
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

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}
