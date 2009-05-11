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
float					*shadow_poly_stencil_vbo;
bool					*shadow_hits;
d3pnt					*shadow_spt,*shadow_ept,*shadow_hpt;
poly_pointer_type		*shadow_poly_ptrs;
view_light_spot_type	shadow_above_lspot;

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
	
		// memory for stencil vbo and poly ptrs
	
	shadow_poly_ptrs=(poly_pointer_type*)malloc(shadow_max_poly_draw*sizeof(poly_pointer_type));
	shadow_poly_stencil_vbo=(float*)malloc((3*8)*sizeof(float));
	
		// check for memory errors
	
	if ((shadow_spt==NULL) || (shadow_ept==NULL) || (shadow_hpt==NULL) || (shadow_hits==NULL) || (shadow_poly_ptrs==NULL) || (shadow_poly_stencil_vbo==NULL)) {
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
	free(shadow_poly_ptrs);
	free(shadow_poly_stencil_vbo);
}

/* =======================================================

      Shadow Draw Volume
      
======================================================= */

view_light_spot_type* shadow_get_light_spot(model_draw *draw)
{
	view_light_spot_type			*lspot;

		// get closest light

	lspot=gl_light_find_closest_light(draw->pnt.x,draw->pnt.y,draw->pnt.z);

		// if no light, get light directly above

	if (lspot==NULL) {
		lspot=&shadow_above_lspot;
		memmove(&lspot->pnt,&draw->pnt,sizeof(d3pnt));
		lspot->pnt.y-=map_enlarge*100;
		lspot->intensity=(map_enlarge*100)+draw->size.y;
	}

	return(lspot);
}

bool shadow_get_volume(model_draw *draw,int *px,int *py,int *pz)
{
	int							n;
	float						f_dist;
	bool						hits[8];
	d3pnt						spt[8],ept[8],hpt[8],*sp,*ep;
	d3vct						ray_move;
	model_type					*mdl;
	view_light_spot_type		*lspot;
	ray_trace_contact_type		base_contact;

		// get bounding box

	if ((draw->uid==-1) || (!draw->on)) return(FALSE);
	
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return(FALSE);
		
	model_get_view_complex_bounding_box(mdl,&draw->pnt,&draw->setup.ang,px,py,pz);

		// get light and draw distance

	lspot=shadow_get_light_spot(draw);
	f_dist=(float)lspot->intensity;

		// ray trace bounding box
		
	sp=spt;
	ep=ept;

	for (n=0;n!=8;n++) {
		sp->x=px[n];
		sp->y=py[n];
		sp->z=pz[n];

		vector_create(&ray_move,lspot->pnt.x,lspot->pnt.y,lspot->pnt.z,sp->x,sp->y,sp->z);
				
		ep->x=sp->x-(int)(ray_move.x*f_dist);
		ep->y=sp->y-(int)(ray_move.y*f_dist);
		ep->z=sp->z-(int)(ray_move.z*f_dist);
		
		sp++;
		ep++;
	}

	base_contact.obj.on=FALSE;
	base_contact.obj.ignore_uid=-1;

	base_contact.proj.on=FALSE;
	base_contact.proj.ignore_uid=-1;

	base_contact.hit_mode=poly_ray_trace_hit_mode_all;
	base_contact.origin=poly_ray_trace_origin_object;

	ray_trace_map_by_point_array_no_contact(8,spt,ept,hpt,hits,&base_contact);

		// set the volume

	for (n=0;n!=8;n++) {
		px[n]=hpt[n].x;
		py[n]=hpt[n].y;
		pz[n]=hpt[n].z;
	}

	return(TRUE);
}

/* =======================================================

      Find Polygons that Shadows Cross
      
======================================================= */

int shadow_build_poly_set(model_draw *draw)
{
	int					n,k,cnt,px[8],py[8],pz[8];
	d3pnt				min,max;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// get shadow volume for collision check
		
	if (!shadow_get_volume(draw,px,py,pz)) return(0);
	
	min.x=max.x=px[0];
	min.y=max.y=py[0];
	min.z=max.z=pz[0];
	
	for (n=1;n!=8;n++) {
		if (px[n]<min.x) min.x=px[n];
		if (px[n]>max.x) max.x=px[n];
		if (py[n]<min.y) min.y=py[n];
		if (py[n]>max.y) max.y=py[n];
		if (pz[n]<min.z) min.z=pz[n];
		if (pz[n]>max.z) max.z=pz[n];
	}

		// add in some slop for shadows
		// that directly collide with a polygon
		// and don't pass through it

	min.x-=map_enlarge;
	max.x+=map_enlarge;
	min.y-=map_enlarge;
	max.y+=map_enlarge;
	min.z-=map_enlarge;
	max.z+=map_enlarge;
	
		// check meshes
		
	cnt=0;
	
	for (n=0;n!=view.render->draw_list.count;n++) {
	
		if (view.render->draw_list.items[n].type!=view_render_type_mesh) continue;

		mesh=&map.mesh.meshes[view.render->draw_list.items[n].idx];
		
			// find polys colliding with shadow
			
		for (k=0;k!=mesh->npoly;k++) {
			poly=&mesh->polys[k];
			
				// skip all transparent polys
				
			if (poly->render.transparent_on) continue;

				// check if within shadow volume
				
			if ((poly->box.max.x<min.x) || (poly->box.min.x>max.x)) continue;
			if ((poly->box.max.z<min.z) || (poly->box.min.z>max.z)) continue;
			if ((poly->box.max.y<min.y) || (poly->box.min.y>max.y)) continue;		// check Y last as X/Z are usually better eliminations
			
			shadow_poly_ptrs[cnt].mesh_idx=view.render->draw_list.items[n].idx;
			shadow_poly_ptrs[cnt].poly_idx=k;
			cnt++;
			
				// can only shadow for max stencil count
				
			if (cnt==shadow_max_poly_draw) return(cnt);
		}
	}
	
	return(cnt);
}

/* =======================================================

      Stencil Shadows Polygons
      
======================================================= */

void shadow_stencil_mesh_poly(int mesh_idx,int poly_idx)
{
	int						n;
	float					*vp,*vertex_ptr;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	mesh=&map.mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
		// setup memory
		
	vp=shadow_poly_stencil_vbo;
	
	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		*vp++=(float)pt->x;
		*vp++=(float)pt->y;
		*vp++=(float)pt->z;
	}
	
	shadow_stencil_poly_vertex_count=poly->ptsz;

		// setup vertex ptr
		
	vertex_ptr=view_bind_map_next_vertex_object(shadow_stencil_poly_vertex_count*3);
	if (vertex_ptr==NULL) return;
	
	memmove(vertex_ptr,shadow_poly_stencil_vbo,((shadow_stencil_poly_vertex_count*3)*sizeof(float)));
	
	view_unmap_current_vertex_object();
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);
		
		// stencil the polygon
		
	glDisable(GL_BLEND);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
	glStencilFunc(GL_ALWAYS,stencil_shadow,0xFF);
	
	glDrawArrays(GL_POLYGON,0,shadow_stencil_poly_vertex_count);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	view_unbind_current_vertex_object();
		
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
}

void shadow_stencil_clear_mesh_poly(void)
{
	float					*vertex_ptr;
	
		// use the set data to setup the clear vbo
		
	vertex_ptr=view_bind_map_next_vertex_object(shadow_stencil_poly_vertex_count*3);
	if (vertex_ptr==NULL) return;
	
	memmove(vertex_ptr,shadow_poly_stencil_vbo,((shadow_stencil_poly_vertex_count*3)*sizeof(float)));
	
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
	
		// stencil the polygon
		
	glDrawArrays(GL_POLYGON,0,shadow_stencil_poly_vertex_count);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	view_unbind_current_vertex_object();
		
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
}

/* =======================================================

      Render Shadows
      
======================================================= */

void shadow_render(model_draw *draw)
{
	int							n,k,t,poly_count,trig_count;
	float						f_dist;
	float						*vp,*vertex_ptr;
	unsigned short				*index_ptr;
	d3vct						ray_move;
	d3pnt						*spt,*ept,*hpt;
    model_trig_type				*trig;
	model_mesh_type				*mesh;
	model_type					*mdl;
	view_light_spot_type		*lspot;
	
		// shadows on for this object?
		
	if ((!setup.shadow_on) || (!draw->shadow.on)) return;

		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;
	
		// find all polys the shadow ray hits
		
	poly_count=shadow_build_poly_set(draw);
	if (poly_count==0) return;
	
		// get light and draw distance

	lspot=shadow_get_light_spot(draw);
	f_dist=(float)lspot->intensity;

		// start stenciling
		// never write to the depth buffer during this operation

	glEnable(GL_STENCIL_TEST);
	glDisable(GL_ALPHA_TEST);

	glDepthMask(GL_FALSE);

		// setup blending

	glColor4f(0.0f,0.0f,0.0f,shadow_alpha);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		// run through all the stenciled polygons
		// and ray trace against their planes
		// to create the shadow polygons
			
	for (k=0;k!=poly_count;k++) {

			// stencil the poly we are going
			// to draw a shadow on

		shadow_stencil_mesh_poly(shadow_poly_ptrs[k].mesh_idx,shadow_poly_ptrs[k].poly_idx);

			// shadows are blended and stenciled
			// we clear the stencil as we draw so
			// we can maintain the alpha levels

		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);

		glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);
		glStencilFunc(GL_EQUAL,stencil_shadow,0xFF);

			// run through all the meshes of this model

		for (n=0;n!=mdl->nmesh;n++) {
			if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

				// ray trace the mesh against
				// the plane of the polygon

			mesh=&mdl->meshes[n];
			
			vp=draw->setup.mesh_arrays[n].gl_vertex_array;
			
			spt=shadow_spt;
			ept=shadow_ept;
			
			for (t=0;t!=mesh->nvertex;t++) {
				spt->x=(int)*vp++;
				spt->y=(int)*vp++;
				spt->z=(int)*vp++;
				
				vector_create(&ray_move,lspot->pnt.x,lspot->pnt.y,lspot->pnt.z,spt->x,spt->y,spt->z);
					
				ept->x=spt->x-(int)(ray_move.x*f_dist);
				ept->y=spt->y-(int)(ray_move.y*f_dist);
				ept->z=spt->z-(int)(ray_move.z*f_dist);
				
				spt++;
				ept++;
			}

			ray_trace_mesh_poly_plane(mesh->nvertex,shadow_spt,shadow_ept,shadow_hpt,shadow_hits,shadow_poly_ptrs[k].mesh_idx,shadow_poly_ptrs[k].poly_idx);

				// setup the vertex objects

			vertex_ptr=view_bind_map_next_vertex_object(mesh->nvertex*3);
			if (vertex_ptr==NULL) {
				glDisable(GL_STENCIL_TEST);
				glDepthMask(GL_TRUE);
				return;
			}
			
			hpt=shadow_hpt;
			
			for (t=0;t!=mesh->nvertex;t++) {
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

			for (t=0;t!=mesh->ntrig;t++) {

				if ((shadow_hits[trig->v[0]]) && (shadow_hits[trig->v[1]]) && (shadow_hits[trig->v[2]])) {
				
					*index_ptr++=trig->v[0];
					*index_ptr++=trig->v[1];
					*index_ptr++=trig->v[2];
					
					trig_count++;
				}
				
				trig++;
			}

			view_unmap_current_index_object();

				// draw the shadow trigs
			
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3,GL_FLOAT,0,0);
				
			glDrawRangeElements(GL_TRIANGLES,0,mesh->nvertex,(trig_count*3),GL_UNSIGNED_SHORT,(GLvoid*)0);

			glDisableClientState(GL_VERTEX_ARRAY);
		
				// unbind the vertex and index object
				
			view_unbind_current_vertex_object();
			view_unbind_current_index_object();
		}

		shadow_stencil_clear_mesh_poly();
	}

		// restore depth buffer and turn
		// off stenciling
		
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
}
