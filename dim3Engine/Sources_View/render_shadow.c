/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shadow Drawing

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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

#include "consoles.h"
#include "models.h"
#include "lights.h"
#include "video.h"
#include "physics.h"

extern bool				dim3_debug;

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern setup_type		setup;

int						shadow_stencil_poly_vertex_count;
float					shadow_alpha;
float					*shadow_stencil_vbo;
bool					*shadow_hits;
d3pnt					*shadow_spt,*shadow_ept,*shadow_hpt;

/* =======================================================

      Initialize and Shutdown Shadows
      
======================================================= */

bool shadow_initialize(char *err_str)
{
	int					sz;
	
		// setup shadow alpha

	shadow_alpha=1.0f-((map.ambient.light_color.r+map.ambient.light_color.g+map.ambient.light_color.b)/3.0f);
	if (shadow_alpha<0.2f) shadow_alpha=0.2f;
	if (shadow_alpha>0.8f) shadow_alpha=0.8f;
	
		// create ray trace points
		
	sz=sizeof(d3pnt)*max_model_vertex;
	
	shadow_spt=(d3pnt*)malloc(sz);
	shadow_ept=(d3pnt*)malloc(sz);
	shadow_hpt=(d3pnt*)malloc(sz);
	
	shadow_hits=(bool*)malloc(sizeof(bool)*max_model_vertex);
	
		// create memory for stencil vbo
		
	shadow_stencil_vbo=(float*)malloc(((stencil_poly_end-stencil_poly_start)*3)*sizeof(float));
	
		// check for memory errors
	
	if ((shadow_spt==NULL) || (shadow_ept==NULL) || (shadow_hpt==NULL) || (shadow_hits==NULL) || (shadow_stencil_vbo==NULL)) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}

	return(TRUE);
}

void shadow_shutdown(void)
{
	free(shadow_spt);
	free(shadow_ept);
	free(shadow_hpt);
	free(shadow_hits);
	free(shadow_stencil_vbo);
}

/* =======================================================

      Stencil Shadows Polygons
      
======================================================= */

void shadow_stencil_poly_set(int item_count,ray_trace_check_item_type *item_list)
{
	int						k,n,offset,stencil_idx;
	float					*vp,*vertex_ptr;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	
		// setup memory
		
	shadow_stencil_poly_vertex_count=0;
	vp=shadow_stencil_vbo;
	
	for (k=0;k!=item_count;k++) {
		mesh=&map.mesh.meshes[item_list[k].index];
		poly=&mesh->polys[item_list[k].index_2];
		
		for (n=0;n!=poly->ptsz;n++) {
			pt=&mesh->vertexes[poly->v[n]];
			*vp++=(float)pt->x;
			*vp++=(float)pt->y;
			*vp++=(float)pt->z;
		}
		
		shadow_stencil_poly_vertex_count+=poly->ptsz;
	}

		// setup vertex ptr
		
	vertex_ptr=view_bind_map_next_vertex_object(shadow_stencil_poly_vertex_count*3);
	if (vertex_ptr==NULL) return;
	
	memmove(vertex_ptr,shadow_stencil_vbo,((shadow_stencil_poly_vertex_count*3)*sizeof(float)));
	
	view_unmap_current_vertex_object();
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);
		
		// setup the stenciling
		
	glDisable(GL_BLEND);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
	
		// stencil the polygons
		
	stencil_idx=stencil_poly_start;
	offset=0;
		
	for (k=0;k!=item_count;k++) {
		poly=&map.mesh.meshes[item_list[k].index].polys[item_list[k].index_2];

		glStencilFunc(GL_ALWAYS,stencil_idx++,0xFF);
		glDrawArrays(GL_POLYGON,offset,poly->ptsz);
		
		offset+=poly->ptsz;
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
	view_unbind_current_vertex_object();
		
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
}

void shadow_stencil_poly_clear(int item_count,ray_trace_check_item_type *item_list)
{
	int						k,offset;
	float					*vertex_ptr;
	map_mesh_poly_type		*poly;
	
		// use the set data to setup the clear vbo
		
	vertex_ptr=view_bind_map_next_vertex_object(shadow_stencil_poly_vertex_count*3);
	if (vertex_ptr==NULL) return;
	
	memmove(vertex_ptr,shadow_stencil_vbo,((shadow_stencil_poly_vertex_count*3)*sizeof(float)));
	
	view_unmap_current_vertex_object();
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);
		
		// setup the stenciling
		
	glDisable(GL_BLEND);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);
	glStencilFunc(GL_ALWAYS,0x0,0xFF);
	
		// stencil the polygons
		
	offset=0;
		
	for (k=0;k!=item_count;k++) {
		poly=&map.mesh.meshes[item_list[k].index].polys[item_list[k].index_2];
		glDrawArrays(GL_POLYGON,offset,poly->ptsz);
		offset+=poly->ptsz;
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
	view_unbind_current_vertex_object();
		
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
}

/* =======================================================

      Render Shadows
      
======================================================= */

void shadow_render(model_draw *draw)
{
	int							n,k,item_count,stencil_idx,trig_count;
	float						f_dist;
	float						*vp,*vertex_ptr;
	unsigned short				*index_ptr;
	double						dx,dy,dz;
	d3vct						ray_move;
	d3pnt						*spt,*ept,*hpt;
    model_trig_type				*trig;
	model_mesh_type				*mesh;
	model_type					*mdl;
	view_light_spot_type		*lspot,temp_lspot;
	ray_trace_contact_type		base_contact;
	ray_trace_check_item_type	*item_list;
	
		// shadows on for this object?
		
	if ((!setup.shadow_on) || (!draw->shadow.on)) return;

		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;
	
		// get nearest light
		// or from directly above if no light

	lspot=gl_light_find_closest_light(draw->pnt.x,draw->pnt.y,draw->pnt.z);
	
	if (lspot==NULL) {
		lspot=&temp_lspot;
		memmove(&lspot->pnt,&draw->pnt,sizeof(d3pnt));
		lspot->pnt.y-=map_enlarge*100;
		lspot->intensity=(map_enlarge*100)+draw->size.y;
	}

		// draw distance
	
	dx=(lspot->pnt.x-draw->pnt.x);
	dy=(lspot->pnt.y-(draw->pnt.y-(draw->size.y>>1)));
	dz=(lspot->pnt.z-draw->pnt.z);
	
	f_dist=(float)lspot->intensity-(float)sqrt((dx*dx)+(dy*dy)+(dz*dz));
	
		// start stenciling
		
	glEnable(GL_STENCIL_TEST);
	glDepthMask(GL_FALSE);

	glDisable(GL_ALPHA_TEST);

		// translate all the vertexes

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

		mesh=&mdl->meshes[n];
		
		vp=draw->setup.mesh_arrays[n].gl_vertex_array;
		
		spt=shadow_spt;
		ept=shadow_ept;
		
		for (k=0;k!=mesh->nvertex;k++) {
			spt->x=(int)*vp++;
			spt->y=(int)*vp++;
			spt->z=(int)*vp++;
			
			vector_create(&ray_move,lspot->pnt.x,lspot->pnt.y,lspot->pnt.z,spt->x,spt->y,spt->z);
				
			ept->x=shadow_spt->x-(int)(ray_move.x*f_dist);
			ept->y=shadow_spt->y-(int)(ray_move.y*f_dist);
			ept->z=shadow_spt->z-(int)(ray_move.z*f_dist);
			
			spt++;
			ept++;
		}
		
		base_contact.obj.on=FALSE;
		base_contact.obj.ignore_uid=-1;

		base_contact.proj.on=FALSE;
		base_contact.proj.ignore_uid=-1;

		base_contact.hit_mode=poly_ray_trace_hit_mode_all;
		base_contact.origin=poly_ray_trace_origin_object;

			// run ray trace

		ray_trace_map_by_point_array_no_contact(mesh->nvertex,shadow_spt,shadow_ept,shadow_hpt,shadow_hits,&base_contact);

			// get list of hit items
			// reduce list if it's over the number of items we can stencil
			
		item_list=ray_trace_get_last_item_list(&item_count);
		
		if (item_count==0) continue;
		if (item_count>(stencil_poly_end-stencil_poly_start)) item_count=stencil_poly_end-stencil_poly_start;
		
			// stencil shadow polygons
			
		shadow_stencil_poly_set(item_count,item_list);

			// create the vertex object
			
		vertex_ptr=view_bind_map_next_vertex_object(mesh->nvertex*3);
		if (vertex_ptr==NULL) {
			glDisable(GL_STENCIL_TEST);
			glDepthMask(GL_TRUE);
			return;
		}
		
		hpt=shadow_hpt;
		
		for (k=0;k!=mesh->nvertex;k++) {
			*vertex_ptr++=(float)hpt->x;
			*vertex_ptr++=(float)hpt->y;
			*vertex_ptr++=(float)hpt->z;
			hpt++;
		}
		
		view_unmap_current_vertex_object();

			// create the index object
			
		index_ptr=view_bind_map_next_index_object(mesh->ntrig*3);
		if (index_ptr==NULL) {
			view_unbind_current_vertex_object();
			glDisable(GL_STENCIL_TEST);
			glDepthMask(GL_TRUE);
			return;
		}

		trig_count=0;
		trig=mesh->trigs;

		for (k=0;k!=mesh->ntrig;k++) {

			if ((shadow_hits[trig->v[0]]) && (shadow_hits[trig->v[1]]) && (shadow_hits[trig->v[2]])) {
			
				*index_ptr++=trig->v[0];
				*index_ptr++=trig->v[1];
				*index_ptr++=trig->v[2];
				
				trig_count++;
			}
			
			trig++;
		}

		view_unmap_current_index_object();

			// draw the trigs for each hit polygon
			// clear the stencil so we don't overdraw and destroy
			// the shadow alpha
		
		stencil_idx=stencil_poly_start;

		glColor4f(0.0f,0.0f,0.0f,shadow_alpha);
			
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		
		glDisable(GL_DEPTH_TEST);
		
		glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,0);
			
		for (k=0;k!=item_count;k++) {
			glStencilFunc(GL_EQUAL,stencil_idx++,0xFF);
			glDrawRangeElements(GL_TRIANGLES,0,mesh->nvertex,(trig_count*3),GL_UNSIGNED_SHORT,(GLvoid*)0);
		}
	
			// unbind the vertex and index object
			
		glDisableClientState(GL_VERTEX_ARRAY);
			
		view_unbind_current_vertex_object();
		view_unbind_current_index_object();
		
			// clear shadow polygons

		shadow_stencil_poly_clear(item_count,item_list);
	}
	
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
}
