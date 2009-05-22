/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Vertex Lists

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
#include "video.h"

extern bool					dim3_debug;

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Setup Vertex, UV, and possible Color List for Meshes
      
======================================================= */

bool view_compile_mesh_gl_list_init(void)
{
	int					n,k,t,uv_idx,vertex_cnt,uv_cnt,i_idx;
	unsigned int		v_poly_start_idx;
	unsigned int		*index_ptr;
	float				x_shift_offset,y_shift_offset;
	float				*vertex_ptr,*pv,*pp,*pc;
	d3pnt				*pnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// get total number of vertexes and indexes
		// and their offsets to setup vertex object for map
		
		// do the UV count separately so we can work around
		// multiple UVs for layers
		
	vertex_cnt=0;
	uv_cnt=0;
	
		// setup meshes
		
	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {

			// mesh has not been moved

		mesh->draw.moved=FALSE;

			// setup offsets

		mesh->draw.vertex_offset=vertex_cnt;
		mesh->draw.uv_offset=uv_cnt;

			// poly vertexes
			
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			poly->draw.vertex_offset=vertex_cnt;
			vertex_cnt+=poly->ptsz;
			poly++;
		}
			
			// poly uvs
				
		for (uv_idx=0;uv_idx!=mesh->nuv;uv_idx++) {
		
			poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {
				poly->draw.uv[uv_idx].uv_offset=uv_cnt;
				uv_cnt+=poly->ptsz;
				poly++;
			}
		}
		
		mesh++;
	}

		// total vertexes

	map.mesh.vbo_vertex_count=vertex_cnt;
	map.mesh.vbo_uv_count=uv_cnt;

		// initial vertex VBO
		
	view_init_map_vertex_object((vertex_cnt*(3+3))+(uv_cnt*2));

	vertex_ptr=view_bind_map_map_vertex_object();
	if (vertex_ptr==NULL) return(FALSE);

	pv=vertex_ptr;
	pp=pv+(vertex_cnt*3);
	pc=pp+(uv_cnt*2);
	
		// vertexes and color
		// we run this separate from the UVs
		// as they are grouped for layers

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
	
			// vertexes and colors

		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {

			for (t=0;t!=poly->ptsz;t++) {
			
				pnt=&mesh->vertexes[poly->v[t]];

				*pv++=(float)pnt->x;
				*pv++=(float)pnt->y;
				*pv++=(float)pnt->z;

				*pc++=1.0f;
				*pc++=1.0f;
				*pc++=1.0f;
			}

			poly++;
		}
		
		mesh++;
	}
	
		// uvs
				
	for (uv_idx=0;uv_idx!=mesh->nuv;uv_idx++) {

		mesh=map.mesh.meshes;

		for (n=0;n!=map.mesh.nmesh;n++) {
		
			poly=mesh->polys;
				
			for (k=0;k!=mesh->npoly;k++) {

					// polygon vertexes

				x_shift_offset=poly->draw.uv[uv_idx].x_shift_offset;
				y_shift_offset=poly->draw.uv[uv_idx].y_shift_offset;

				for (t=0;t!=poly->ptsz;t++) {
					*pp++=poly->uv[uv_idx].x[t]+x_shift_offset;
					*pp++=poly->uv[uv_idx].y[t]+y_shift_offset;
				}

				poly++;
			}
		}
		
		mesh++;
	}

		// unmap vertex VBO
		
	view_unmap_map_vertex_object();
	view_unbind_map_vertex_object();

		// initialize index VBO
		
	view_init_map_index_object(vertex_cnt);
		
	index_ptr=view_bind_map_map_index_object();
	if (index_ptr==NULL) {
		view_unmap_map_vertex_object();
		view_unbind_map_vertex_object();
		return(FALSE);
	}

		// map the indexes

	i_idx=0;
	
	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
	
			// run through the polys

		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
				// polygon indexes
				
			v_poly_start_idx=poly->draw.vertex_offset;
			poly->draw.gl_poly_index_offset=i_idx*sizeof(unsigned int);
				
			for (t=0;t!=poly->ptsz;t++) {
				*index_ptr++=v_poly_start_idx+t;
				i_idx++;
			}
			
				// min/max for range element draws
				
			poly->draw.gl_poly_index_min=v_poly_start_idx;
			poly->draw.gl_poly_index_max=v_poly_start_idx+poly->ptsz;

			poly++;
		}
		
		mesh++;
	}
	
		// unmap index VBO

	view_unmap_map_index_object();
	view_unbind_map_index_object();

	return(TRUE);
}

void view_compile_mesh_gl_list_free(void)
{
}

/* =======================================================

      Compile OpenGL Lists MainLine
      
======================================================= */

bool view_compile_mesh_gl_lists(int tick)
{
	int							n,k,t,uv_idx,vertex_cnt,uv_cnt;
	float						x_shift_offset,y_shift_offset;
	float						*vertex_ptr,*pv,*pp,*pc;
	d3pnt						*pnt;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;

		// total number of vertexes

	vertex_cnt=map.mesh.vbo_vertex_count;
	uv_cnt=map.mesh.vbo_uv_count;

		// map VBO to memory

	vertex_ptr=view_bind_map_map_vertex_object();
	if (vertex_ptr==NULL) return(FALSE);
	
		// run throught the meshes
		// in this scene and update any
		// relevant data
		
	for (n=0;n!=view.render->draw_list.count;n++) {

		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];

			// recalculate the vertexes if this
			// mesh is moving

		if (mesh->draw.moved) {

			pv=vertex_ptr+(mesh->draw.vertex_offset*3);

			poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {

					// polygon vertexes

				for (t=0;t!=poly->ptsz;t++) {
				
					pnt=&mesh->vertexes[poly->v[t]];

					*pv++=(float)pnt->x;
					*pv++=(float)pnt->y;
					*pv++=(float)pnt->z;
				}

				poly++;
			}
		}

			// recalculate the uvs if this
			// mesh has shiftable uvs

		if (mesh->flag.shiftable) {

			pp=vertex_ptr+((vertex_cnt*3)+(mesh->draw.vertex_offset*2));

				// the UV layers
			
			for (uv_idx=0;uv_idx!=mesh->nuv;uv_idx++) {
			
				poly=mesh->polys;
				
				for (k=0;k!=mesh->npoly;k++) {

						// polygon uvs

					x_shift_offset=poly->draw.uv[uv_idx].x_shift_offset;
					y_shift_offset=poly->draw.uv[uv_idx].y_shift_offset;

					for (t=0;t!=poly->ptsz;t++) {
						*pp++=poly->uv[uv_idx].x[t]+x_shift_offset;
						*pp++=poly->uv[uv_idx].y[t]+y_shift_offset;
					}

					poly++;
				}
			}
		}

			// if in debug, we are using high lighting,
			// so recalculate the lights for the mesh

		if ((dim3_debug) || (mesh->flag.hilite)) {
			
			pc=vertex_ptr+((vertex_cnt*3)+(uv_cnt*2)+(mesh->draw.vertex_offset*3));

			poly=mesh->polys;
				
			for (k=0;k!=mesh->npoly;k++) {
				for (t=0;t!=poly->ptsz;t++) {
					*pc++=1.0f;
					*pc++=1.0f;
					*pc++=1.0f;
				}
				poly++;
			}
		}

			// if the mesh has some non-shaders in it,
			// recalculate the lighting
			// supergumba -- this needs to be optimized 

		else {

			if (mesh->render.has_no_shader) {

				pc=vertex_ptr+((vertex_cnt*3)+(uv_cnt*2)+(mesh->draw.vertex_offset*3));

				poly=mesh->polys;
				
				for (k=0;k!=mesh->npoly;k++) {

					for (t=0;t!=poly->ptsz;t++) {
						pnt=&mesh->vertexes[poly->v[t]];
						gl_lights_calc_vertex((double)pnt->x,(double)pnt->y,(double)pnt->z,pc);
						pc+=3;
					}

					poly++;
				}
			}
		}

			// reset the moved list so we'll catch
			// and update any moved vertexes next time
			// we hit this function

		mesh->draw.moved=FALSE;

		mesh++;
	}

		// unmap VBO

	view_unmap_map_vertex_object();
	view_unbind_map_vertex_object();
	
	return(TRUE);
}

/* =======================================================

      Use Map OpenGL Lists
      
======================================================= */

void view_compile_gl_list_attach(void)
{
		// use last compiled buffer

	view_bind_map_vertex_object();
	view_bind_map_index_object();

		// vertexes

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);
}

void view_compile_gl_list_uv_layer_attach(int uv_idx)
{
	int				offset;

	offset=((map.mesh.vbo_vertex_count*3)*(map.mesh.vbo_vertex_count*(uv_idx*2)))*sizeof(float);
	
	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
}

void view_compile_gl_list_enable_color(void)
{
	int			offset;

	offset=((map.mesh.vbo_vertex_count*3)+(map.mesh.vbo_uv_count*2))*sizeof(float);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,(void*)offset);
}

void view_compile_gl_list_disable_color(void)
{
	glDisableClientState(GL_COLOR_ARRAY);
}

void view_compile_gl_list_dettach(void)
{
	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_VERTEX_ARRAY);

	view_unbind_map_index_object();
	view_unbind_map_vertex_object();
}
