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
	int					n,k,t,cnt,sz,x,y,xtot,
						v_count,v_idx,v_light_start_idx;
	unsigned int		*qd;
	float				x_shift_offset,y_shift_offset;
	float				*vertex_ptr,*pv,*pp,*pc,*pn;
	d3pnt				*pnt,*lv_pt;
	d3uv				*lv_uv;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// get total number of vertexes
		// and their offsets to setup vertex object for map
		
	v_count=0;
	
		// setup meshes
		
	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {

			// clear the light cache

		mesh->light.light_count=-1;

			// mesh has not been moved

		mesh->draw.moved=FALSE;

			// get count for color and normal lists

		cnt=0;
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			cnt+=(poly->ptsz+poly->light.nvertex);
			poly++;
		}

		mesh->draw.vertex_count=cnt;

			// allocate the lists

		sz=(cnt*3)*sizeof(float);

		mesh->draw.p_color=(float*)malloc(sz);
		if (mesh->draw.p_color==NULL) return(FALSE);

		mesh->draw.p_normal=(float*)malloc(sz);
		if (mesh->draw.p_normal==NULL) return(FALSE);

			// defaults

		pc=mesh->draw.p_color;
		pn=mesh->draw.p_normal;

		for (t=0;t!=cnt;t++) {
			*pc++=1.0f;
			*pc++=1.0f;
			*pc++=1.0f;
			*pn++=0.5f;
			*pn++=0.5f;
			*pn++=1.0f;
		}
		
			// add up vertex count and set
			// vertex offset into vertex object
			
		mesh->draw.vertex_offset=v_count;
		v_count+=mesh->draw.vertex_count;

		mesh++;
	}

		// total vertexes

	map.mesh_vertexes.draw_vertex_count=v_count;

		// initial map VBO
		
	view_init_map_vertex_object((v_count*(3+2+3+3)));

	vertex_ptr=view_bind_map_map_vertex_object();
	if (vertex_ptr==NULL) return(FALSE);
	
		// arrays and offsets

	map.mesh_vertexes.vert.sz=(v_count*3)*sizeof(float);
	map.mesh_vertexes.vert.offset=0;

	pv=vertex_ptr;

	map.mesh_vertexes.uv.sz=(v_count*2)*sizeof(float);
	map.mesh_vertexes.uv.offset=map.mesh_vertexes.vert.offset+map.mesh_vertexes.vert.sz;

	pp=pv+(v_count*3);

	map.mesh_vertexes.color.sz=(v_count*3)*sizeof(float);
	map.mesh_vertexes.color.offset=map.mesh_vertexes.uv.offset+map.mesh_vertexes.uv.sz;

	pc=pp+(v_count*2);

	map.mesh_vertexes.normal.sz=(v_count*3)*sizeof(float);
	map.mesh_vertexes.normal.offset=map.mesh_vertexes.color.offset+map.mesh_vertexes.color.sz;

	pn=pc+(v_count*3);
	
		// fill in map geometery

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
				
				poly->draw.portal_v[t]=(unsigned int)v_idx;
				
				v_idx++;
			}

				// tesseled lighting vertexes

			if (!poly->light.simple_tessel) {

					// create the vertexes

				v_light_start_idx=v_idx;
				
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

					// create light mesh draw indexes

				qd=mesh->light.quad_indexes+poly->light.quad_index_offset;
				
				xtot=poly->light.grid_x_sz+1;

				for (y=0;y!=poly->light.grid_y_sz;y++) {
					for (x=0;x!=poly->light.grid_x_sz;x++) {
						*qd++=(unsigned int)(((y*xtot)+x)+v_light_start_idx);
						*qd++=(unsigned int)(((y*xtot)+(x+1))+v_light_start_idx);
						*qd++=(unsigned int)((((y+1)*xtot)+(x+1))+v_light_start_idx);
						*qd++=(unsigned int)((((y+1)*xtot)+x)+v_light_start_idx);
					}
				}
			}

			poly++;
		}
		
		mesh++;
	}
	
		// unmap VBO

	view_unmap_map_vertex_object();
	view_unbind_map_vertex_object();

	return(TRUE);
}

void view_compile_mesh_gl_list_free(void)
{
	int					n;
	map_mesh_type		*mesh;

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
		free(mesh->draw.p_color);
		free(mesh->draw.p_normal);
		mesh++;
	}
}

/* =======================================================

      Calculate Mesh Light Cache
      
======================================================= */

void view_compile_mesh_gl_lists_normal(map_mesh_type *mesh)
{
	int									n,k;
	float								*pc,*pn;
	d3pnt								*pnt,*lv_pt;
	map_mesh_poly_type					*poly;

	pc=mesh->draw.p_color;
	pn=mesh->draw.p_normal;

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

void view_compile_mesh_gl_lists_ray_trace(map_mesh_type *mesh)
{
	int									n,k;
	float								*pc,*pn;
	d3pnt								*pnt,*lv_pt;
	map_mesh_poly_type					*poly;

	pc=mesh->draw.p_color;
	pn=mesh->draw.p_normal;

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
	int									n,k,t,v_count,sz;
	float								x_shift_offset,y_shift_offset;
	float								*vertex_ptr,*pv,*pp,*pc,*pn;
	bool								recalc_light;
	d3pnt								*pnt,*lv_pt;
	d3uv								*lv_uv;
	map_mesh_type						*mesh;
	map_mesh_poly_type					*poly;

		// total number of vertexes

	v_count=map.mesh_vertexes.draw_vertex_count;

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

				if (setup.ray_trace_lighting) {
					view_compile_mesh_gl_lists_ray_trace(mesh);
				}
				else {
					view_compile_mesh_gl_lists_normal(mesh);
				}

					// run through the polys

				sz=(mesh->draw.vertex_count*3)*sizeof(float);

				pc=vertex_ptr+((v_count*(3+2))+(mesh->draw.vertex_offset*3));
				memmove(pc,mesh->draw.p_color,sz);

				pn=vertex_ptr+((v_count*(3+2+3))+(mesh->draw.vertex_offset*3));
				memmove(pn,mesh->draw.p_normal,sz);
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

		// vertexes

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)map.mesh_vertexes.vert.offset);

		// uvs
		
	glClientActiveTexture(GL_TEXTURE2);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)map.mesh_vertexes.uv.offset);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)map.mesh_vertexes.uv.offset);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)map.mesh_vertexes.uv.offset);

		// color array

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,(void*)map.mesh_vertexes.color.offset);
}

void view_compile_gl_list_switch_to_color(void)
{
	glColorPointer(3,GL_FLOAT,0,(void*)map.mesh_vertexes.color.offset);
}

void view_compile_gl_list_switch_to_normal(void)
{
	glColorPointer(3,GL_FLOAT,0,(void*)map.mesh_vertexes.normal.offset);
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

	view_unbind_map_vertex_object();
}
