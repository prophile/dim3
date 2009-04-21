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

float					shadow_alpha;

/* =======================================================

      Initialize and Shutdown Shadows
      
======================================================= */

bool shadow_initialize(char *err_str)
{
		// setup shadow alpha

	shadow_alpha=1.0f-((map.ambient.light_color.r+map.ambient.light_color.g+map.ambient.light_color.b)/3.0f);
	if (shadow_alpha<0.5f) shadow_alpha=0.5f;
	if (shadow_alpha>1.0f) shadow_alpha=1.0f;

	return(TRUE);
}

void shadow_shutdown(void)
{
}

/* =======================================================

      Render Shadows
      
======================================================= */

void shadow_stencil_poly(poly_pointer_type *poly_ptr,int stencil_idx)
{
	int						n;
	d3pnt					*pt;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	glDisable(GL_BLEND);

		// get the poly to stencil
		
	mesh=&map.mesh.meshes[poly_ptr->mesh_idx];
	poly=&mesh->polys[poly_ptr->poly_idx];

	glEnable(GL_DEPTH_TEST);
	
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
	glStencilFunc(GL_ALWAYS,stencil_idx,0xFF);

	glBegin(GL_POLYGON);

	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		glVertex3i(pt->x,pt->y,pt->z);
	}
	
	glEnd();

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
}

void shadow_render(model_draw *draw)
{
	int				n,k,t;
	float			*vp;
    model_trig_type	*trig;
	model_mesh_type	*mesh;
	model_type		*mdl;
	d3pnt			spt[1000],ept[1000],hpt[1000];
	view_light_spot_type	*lspot;
	int	p_dist,poly_count;
	double		dx,dy,dz;
	bool		add_poly;
	d3vct		ray_move;
	float		f_dist;
	bool		hits[1000];
	ray_trace_contact_type	base_contact,contacts[1000];
	poly_pointer_type	poly_ptrs[32];
	
	if (!setup.shadow_on) return;
	if (!draw->shadow.on) return;
	
		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;
	
		// get nearest light

	lspot=gl_light_find_closest_light(draw->pnt.x,draw->pnt.y,draw->pnt.z,&p_dist);		// supergumba -- passing back distance my not be necessary anymore
	if (lspot==NULL) return;
	
		// create vertex and uv lists
		// in the model's data area
		
	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

			// build model vertex list
			
		model_create_draw_vertexes(mdl,n,&draw->setup);
		if (draw->resize!=1) model_resize_draw_vertex(mdl,n,draw->resize);
		if (draw->flip_x) model_flip_draw_vertex(mdl,n);

			// translate vertex to view
			
		model_translate_draw_vertex(mdl,n,draw->pnt.x,draw->pnt.y,draw->pnt.z);
	}
	
		// draw distance
	
	dx=(lspot->pnt.x-draw->pnt.x);
	dy=(lspot->pnt.y-(draw->pnt.y-(draw->size.y>>1)));
	dz=(lspot->pnt.z-draw->pnt.z);
	
	f_dist=(float)lspot->intensity-(float)sqrt((dx*dx)+(dy*dy)+(dz*dz));
	
		// start stenciling
		
	glEnable(GL_STENCIL_TEST);

	glDisable(GL_ALPHA_TEST);
	
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

		// translate all the vertexes

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

		mesh=&mdl->meshes[n];
		
		vp=mesh->draw.gl_vertex_array;
		
		for (k=0;k!=mesh->nvertex;k++) {
			spt[k].x=(int)*vp++;
			spt[k].y=(int)*vp++;
			spt[k].z=(int)*vp++;
			
			vector_create(&ray_move,lspot->pnt.x,lspot->pnt.y,lspot->pnt.z,spt[k].x,spt[k].y,spt[k].z);
				
			ept[k].x=spt[k].x-(int)(ray_move.x*f_dist);
			ept[k].y=spt[k].y-(int)(ray_move.y*f_dist);
			ept[k].z=spt[k].z-(int)(ray_move.z*f_dist);
		}
		
		base_contact.obj.on=FALSE;
		base_contact.obj.ignore_uid=-1;

		base_contact.proj.on=FALSE;
		base_contact.proj.ignore_uid=-1;

		base_contact.hit_mode=poly_ray_trace_hit_mode_all;
		base_contact.origin=poly_ray_trace_origin_object;

			// run ray trace

		ray_trace_map_by_point_array(mesh->nvertex,spt,ept,hpt,hits,&base_contact,contacts);
		
			// find all hit polygons
			
		poly_count=0;
		
		for (k=0;k!=mesh->nvertex;k++) {
			if (!hits[k]) continue;
			
			add_poly=TRUE;
			
			for (t=0;t!=poly_count;t++) {
				if ((contacts[k].poly.mesh_idx==poly_ptrs[t].mesh_idx) && (contacts[k].poly.poly_idx==poly_ptrs[t].poly_idx)) {
					add_poly=FALSE;
					break;
				}
			}
			
			if (add_poly) {
				poly_ptrs[poly_count].mesh_idx=contacts[k].poly.mesh_idx;
				poly_ptrs[poly_count].poly_idx=contacts[k].poly.poly_idx;
				poly_count++;
				if (poly_count==32) break;
			}
		}
		
			// draw the trigs for each hit polygon
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(0.0f,0.0f,0.0f,shadow_alpha);
			
		for (t=0;t!=poly_count;t++) {
		
				// stencil the polygon
				
			shadow_stencil_poly(&poly_ptrs[t],0x5);

				// draw the shadow
				
			glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
			glStencilFunc(GL_EQUAL,0x5,0xFF);
			
			glDisable(GL_DEPTH_TEST);
		
			glBegin(GL_TRIANGLES);
			
			trig=mesh->trigs;

			for (k=0;k!=mesh->ntrig;k++) {
			
					// skip if no hit
					
				if ((!hits[trig->v[0]]) || (!hits[trig->v[1]]) || (!hits[trig->v[2]])) {
					trig++;
					continue;
				}
				
				glVertex3i(hpt[trig->v[0]].x,hpt[trig->v[0]].y,hpt[trig->v[0]].z);
				glVertex3i(hpt[trig->v[1]].x,hpt[trig->v[1]].y,hpt[trig->v[1]].z);
				glVertex3i(hpt[trig->v[2]].x,hpt[trig->v[2]].y,hpt[trig->v[2]].z);
				
				trig++;
			}
			
			glEnd();
			
				// remove the stencil
				
			shadow_stencil_poly(&poly_ptrs[t],0x0);
		}
	}
	
	glDepthMask(GL_TRUE);
	glDisable(GL_STENCIL_TEST);
}








/*
supergumba

  int shadow_render_stencil_mesh(int mesh_idx,int y,int ty,int by,int cnt,poly_pointer_type *poly_list)
{
	int					k;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=&map.mesh.meshes[mesh_idx];

		// is mesh in collision?

	if ((mesh->box.min.y<ty) || (mesh->box.max.y>by)) return(cnt);
	if (!polygon_2D_collision_bound_box(mesh->box.min.x,mesh->box.max.x,mesh->box.min.z,mesh->box.max.z)) return(cnt);

		// check polygons

	poly=mesh->polys;

	for (k=0;k!=mesh->npoly;k++) {
		
		if (poly->box.wall_like) {
			cnt=shadow_render_stencil_wall_like_poly(mesh_idx,k,ty,y,cnt,poly_list);
		}
		else {
			cnt=shadow_render_stencil_floor_like_poly(mesh_idx,k,ty,by,cnt,poly_list);
		}
		
		if (cnt>=max_shadow_poly) return(max_shadow_poly);

		poly++;
	}

	return(cnt);
}

int shadow_render_stencil_map(model_draw_shadow *shadow,poly_pointer_type *poly_list)
{
	int				n,cnt,ty,by,y,high;
		
		// setup shadow volume to compare against map
		
	cnt=0;
	
	polygon_2D_collision_setup(4,shadow->px,shadow->pz);
	
		// find polygons that intersect shadow volume
	
	y=shadow->pnt.y;
	high=shadow->high;

	high=high>>1;	
	ty=y-high;
	by=y+high;

		// stencil

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_EQUAL);
	glDepthMask(GL_FALSE);
	
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

		// run through show meshes

	for (n=0;n!=view.render->draw_list.count;n++) {
		if (view.render->draw_list.items[n].type==view_render_type_mesh) {
			cnt=shadow_render_stencil_mesh(view.render->draw_list.items[n].idx,y,ty,by,cnt,poly_list);
		}
	}

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

	glDisable(GL_STENCIL_TEST);
	
		// shadow poly count
		
	return(cnt);
}

*/
