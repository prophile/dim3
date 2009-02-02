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

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

/* =======================================================

      Setup Color and Normal Lists for
	  Each Mesh
	  
	  Create the original map mesh vertex and
	  uv lists
      
======================================================= */

bool view_compile_mesh_gl_list_init(void)
{
	int					n,k,t,v_cnt,i_cnt,v_idx,i_idx;
	unsigned int		v_poly_start_idx,v_light_start_idx;
	unsigned int		*index_ptr,*qd;
	float				x_shift_offset,y_shift_offset;
	float				*vertex_ptr,*pv,*pp,*pc,*pn;
	d3pnt				*pnt,*lv_pt;
	d3uv				*lv_uv;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// get total number of vertexes and indexes
		// and their offsets to setup vertex object for map
		
	v_cnt=0;
	i_cnt=0;
	
		// setup meshes
		
	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {

			// clear the light cache

		mesh->light.nlight_cache=-1;

			// mesh has not been moved

		mesh->draw.moved=FALSE;

			// setup offsets

		mesh->draw.vertex_offset=v_cnt;

		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			poly->draw.vertex_offset=v_cnt;
			
			v_cnt+=(poly->ptsz+poly->light.nvertex);
			i_cnt+=(poly->ptsz+(poly->light.nquad*4));
			
			poly++;
		}

		mesh->draw.vertex_count=v_cnt-mesh->draw.vertex_offset;

		mesh++;
	}

		// total vertexes

	map.mesh.vbo_vertex_count=v_cnt;

		// initial vertex VBO
		
	view_init_map_vertex_object(v_cnt*(3+2+3+3));

	vertex_ptr=view_bind_map_map_vertex_object();
	if (vertex_ptr==NULL) return(FALSE);

	pv=vertex_ptr;
	pp=pv+(v_cnt*3);
	pc=pp+(v_cnt*2);
	pn=pc+(v_cnt*3);

		// create the vertexes

	v_idx=0;
		
	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {

			// run through the polys

		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {

				// polygon vertexes

			x_shift_offset=poly->draw.x_shift_offset;
			y_shift_offset=poly->draw.y_shift_offset;

			for (t=0;t!=poly->ptsz;t++) {
			
				pnt=&mesh->vertexes[poly->v[t]];

				*pv++=(float)pnt->x;
				*pv++=(float)pnt->y;
				*pv++=(float)pnt->z;

				*pc++=1.0f;
				*pc++=1.0f;
				*pc++=1.0f;
			
				*pn++=0.5f;
				*pn++=0.5f;
				*pn++=1.0f;
				
				*pp++=poly->gx[t]+x_shift_offset;
				*pp++=poly->gy[t]+y_shift_offset;
				
				v_idx++;
			}

				// tesseled lighting vertexes

			if (!poly->light.simple_tessel) {

					// create the vertexes

				lv_pt=mesh->light.quad_vertexes+poly->light.vertex_offset;
				lv_uv=mesh->light.quad_uvs+poly->light.vertex_offset;

				for (t=0;t!=poly->light.nvertex;t++) {

					*pv++=(float)lv_pt->x;
					*pv++=(float)lv_pt->y;
					*pv++=(float)lv_pt->z;
					
					*pc++=1.0f;
					*pc++=1.0f;
					*pc++=1.0f;
				
					*pn++=0.5f;
					*pn++=0.5f;
					*pn++=1.0f;
					
					*pp++=(lv_uv->x)+x_shift_offset;
					*pp++=(lv_uv->y)+y_shift_offset;

					lv_pt++;
					lv_uv++;

					v_idx++;
				}
			}

			poly++;
		}
	
		mesh++;
	}

		// unmap vertex VBO
		
	view_unmap_map_vertex_object();
	view_unbind_map_vertex_object();

		// initialize index VBO
		
	view_init_map_index_object(i_cnt);
		
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

				// tesseled lighting quads indexes

			if (!poly->light.simple_tessel) {
					
				v_light_start_idx=(unsigned int)(v_poly_start_idx+poly->ptsz);

				qd=mesh->light.quad_indexes+poly->light.quad_index_offset;
				poly->light.gl_quad_index_offset=i_idx*sizeof(unsigned int);

				for (t=0;t!=(poly->light.nquad*4);t++) {
				
					if (*qd>=1000) {
						*index_ptr=v_poly_start_idx+((*qd)-1000);
					}
					else {
						*index_ptr=(*qd)+v_light_start_idx;
					}
					
					qd++;
					index_ptr++;
					
					i_idx++;
				}
			}
			
				// min/max range of element
				
			poly->draw.gl_poly_index_min=v_poly_start_idx;
			poly->draw.gl_poly_index_max=v_poly_start_idx+(poly->ptsz+poly->light.nvertex);	// rough but will work to get indexes within smaller ranges

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

      Calculate Mesh Light Cache
      
======================================================= */

void view_compile_mesh_gl_lists_normal(map_mesh_type *mesh,float *pc,float *pn)
{
	int							n,k;
	d3pnt						*pnt,*lv_pt;
	map_mesh_poly_type			*poly;

	poly=mesh->polys;
		
	for (n=0;n!=mesh->npoly;n++) {

			// polygon lighting vertexes

		for (k=0;k!=poly->ptsz;k++) {
			pnt=&mesh->vertexes[poly->v[k]];
			map_calculate_light_color_normal((double)pnt->x,(double)pnt->y,(double)pnt->z,pc,pn);
			pc+=3;
			pn+=3;
		}

			// if not simple, calculate the lighting mesh

		if (!poly->light.simple_tessel) {

				// tesseled lighting vertexes

			lv_pt=mesh->light.quad_vertexes+poly->light.vertex_offset;

			for (k=0;k!=poly->light.nvertex;k++) {
				map_calculate_light_color_normal((double)lv_pt->x,(double)lv_pt->y,(double)lv_pt->z,pc,pn);
				pc+=3;
				pn+=3;
				lv_pt++;
			}
		}

		poly++;
	}
}

void view_compile_mesh_gl_lists_ray_trace(map_mesh_type *mesh,float *pc,float *pn)
{
	int							n,k;
	d3pnt						*pnt,*lv_pt;
	map_mesh_poly_type			*poly;

	poly=mesh->polys;
		
	for (n=0;n!=mesh->npoly;n++) {

			// polygon lighting vertexes

		for (k=0;k!=poly->ptsz;k++) {
			pnt=&mesh->vertexes[poly->v[k]];
			map_calculate_ray_trace_light_color_normal((double)pnt->x,(double)pnt->y,(double)pnt->z,pc,pn);
			pc+=3;
			pn+=3;
		}

			// if not simple, calculate the lighting mesh

		if (!poly->light.simple_tessel) {

				// tesseled lighting vertexes

			lv_pt=mesh->light.quad_vertexes+poly->light.vertex_offset;

			for (k=0;k!=poly->light.nvertex;k++) {
				map_calculate_ray_trace_light_color_normal((double)lv_pt->x,(double)lv_pt->y,(double)lv_pt->z,pc,pn);
				pc+=3;
				pn+=3;
				lv_pt++;
			}
		}

		poly++;
	}
}

/* =======================================================

      Compile OpenGL Lists MainLine
      
======================================================= */

bool view_compile_mesh_gl_lists(int tick,int mesh_cnt,int *mesh_list)
{
	int							n,k,t,v_count;
	float						x_shift_offset,y_shift_offset;
	float						*vertex_ptr,*pv,*pp,*pc,*pn;
	bool						recalc_light;
	d3pnt						*pnt,*lv_pt;
	d3uv						*lv_uv;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;

		// total number of vertexes

	v_count=map.mesh.vbo_vertex_count;

		// map VBO to memory

	vertex_ptr=view_bind_map_map_vertex_object();
	if (vertex_ptr==NULL) return(FALSE);
	
		// run throught the meshes
		// in this scene and update any
		// relevant data
		
	for (n=0;n!=mesh_cnt;n++) {

		mesh=&map.mesh.meshes[mesh_list[n]];

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

					// tesseled lighting vertexes

				if (!poly->light.simple_tessel) {

					lv_pt=mesh->light.quad_vertexes+poly->light.vertex_offset;

					for (t=0;t!=poly->light.nvertex;t++) {

						*pv++=(float)lv_pt->x;
						*pv++=(float)lv_pt->y;
						*pv++=(float)lv_pt->z;

						lv_pt++;
					}
				}

				poly++;
			}
		}

			// recalculate the uvs if this
			// mesh has shiftable uvs

		if (mesh->flag.shiftable) {

			pp=vertex_ptr+((v_count*3)+(mesh->draw.vertex_offset*2));

			poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {

					// polygon uvs

				x_shift_offset=poly->draw.x_shift_offset;
				y_shift_offset=poly->draw.y_shift_offset;

				for (t=0;t!=poly->ptsz;t++) {
					*pp++=poly->gx[t]+x_shift_offset;
					*pp++=poly->gy[t]+y_shift_offset;
				}

					// tesseled lighting uvs

				if (!poly->light.simple_tessel) {

					lv_uv=mesh->light.quad_uvs+poly->light.vertex_offset;

					for (t=0;t!=poly->light.nvertex;t++) {
						*pp++=(lv_uv->x)+x_shift_offset;
						*pp++=(lv_uv->y)+y_shift_offset;
						lv_uv++;
					}
				}

				poly++;
			}
		}

			// calculate the lights touching
			// this mesh and find out if they need
			// to be recalculated

			// moveable meshes always have their
			// lights recalculated

		if (!mesh->flag.hilite) {

			map_calculate_light_reduce_mesh(mesh);

			if (mesh->draw.moved) {
				recalc_light=TRUE;
			}
			else {
				recalc_light=!map_calculate_light_reduce_check_equal(mesh);
			}

			if (recalc_light) {
				
				pc=vertex_ptr+((v_count*(3+2))+(mesh->draw.vertex_offset*3));
				pn=vertex_ptr+((v_count*(3+2+3))+(mesh->draw.vertex_offset*3));

				if (setup.ray_trace_lighting) {
					view_compile_mesh_gl_lists_ray_trace(mesh,pc,pn);
				}
				else {
					view_compile_mesh_gl_lists_normal(mesh,pc,pn);
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
	int				v_cnt,offset;
	
	v_cnt=map.mesh.vbo_vertex_count;

		// use last compiled buffer

	view_bind_map_vertex_object();
	view_bind_map_index_object();

		// vertexes

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);

		// uvs
		
	offset=(v_cnt*3)*sizeof(float);
	
	glClientActiveTexture(GL_TEXTURE2);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)offset);

		// color array

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,(void*)((v_cnt*(3+2))*sizeof(float)));
}

void view_compile_gl_list_switch_to_color(void)
{
	int				v_cnt;
	
	v_cnt=map.mesh.vbo_vertex_count;
	glColorPointer(3,GL_FLOAT,0,(void*)((v_cnt*(3+2))*sizeof(float)));
}

void view_compile_gl_list_switch_to_normal(void)
{
	int				v_cnt;
	
	v_cnt=map.mesh.vbo_vertex_count;
	glColorPointer(3,GL_FLOAT,0,(void*)((v_cnt*(3+2+3))*sizeof(float)));
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

	view_unbind_map_index_object();
	view_unbind_map_vertex_object();
}
