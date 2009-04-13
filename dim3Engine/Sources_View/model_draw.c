/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Drawing

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

#include "consoles.h"
#include "models.h"
#include "lights.h"
#include "video.h"

extern bool				dim3_debug;

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern setup_type		setup;

extern bool fog_solid_on(void);

/* =======================================================

      Model Drawing Arrays
      
======================================================= */

void model_draw_create_color_vertexes(model_type *mdl,int mesh_mask,model_draw *draw)
{
	int				n,k;
	float			*cp,*vp;
	model_mesh_type	*mesh;

		// if only shaders, then no color list required

	if (!mdl->has_no_shader) return;

		// need color lists

	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))==0) continue;

		mesh=&mdl->meshes[n];
		cp=mesh->draw.gl_color_array;

			// debug and hilited meshes

		if ((dim3_debug) || (mesh->no_lighting)) {

			if (mesh->tintable) {
				for (k=0;k!=mesh->nvertex;k++) {
					*cp++=draw->tint.r;
					*cp++=draw->tint.g;
					*cp++=draw->tint.b;
				}
			}
			else {
				for (k=0;k!=mesh->nvertex;k++) {
					*cp++=1.0f;
					*cp++=1.0f;
					*cp++=1.0f;
				}
			}

			continue;
		}

			// vertex lit

		vp=mesh->draw.gl_vertex_array;

		for (k=0;k!=mesh->nvertex;k++) {
			gl_lights_calc_vertex((double)*vp,(double)*(vp+1),(double)*(vp+2),cp);
			cp+=3;
			vp+=3;
		}

		if (mesh->tintable) {

			cp=mesh->draw.gl_color_array;

			for (k=0;k!=mesh->nvertex;k++) {
				*cp++=(*cp)*draw->tint.r;
				*cp++=(*cp)*draw->tint.g;
				*cp++=(*cp)*draw->tint.b;
			}
		}

	}
}

bool model_draw_initialize_vertex_objects(model_type *mdl,model_draw *draw)
{
	int				n,k,nvertex,ntrig,offset,t_idx;
	float			*vl,*tl,*cl,*vp,*cp,*vertex_ptr,
					*vertex_array,*coord_array;
	unsigned short	*index_ptr;
    model_trig_type	*trig;
	model_mesh_type	*mesh;
	
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

		// setup the colors

	model_draw_create_color_vertexes(mdl,draw->render_mesh_mask,draw);

		// get the total number of trigs and vertexes

	nvertex=0;
	ntrig=0;

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

		mesh=&mdl->meshes[n];

		nvertex+=mesh->nvertex;
		ntrig+=mesh->ntrig;
	}

	if ((nvertex==0) || (ntrig==0)) return(FALSE);
	
	draw->vbo_ptr.ntrig=ntrig;

 		// construct VBO

	vertex_ptr=view_bind_map_next_vertex_object(((ntrig*3)*(3+2+3)));
	if (vertex_ptr==NULL) return(FALSE);

	index_ptr=view_bind_map_next_index_object(ntrig*3);
	if (index_ptr==NULL) {
		view_unmap_current_vertex_object();
		view_unbind_current_vertex_object();
		return(FALSE);
	}
	
		// build the vertexes and indexes

	vl=vertex_array=vertex_ptr;
	tl=coord_array=vertex_ptr+((ntrig*3)*3);
	cl=coord_array=vertex_ptr+((ntrig*3)*(3+2));

	t_idx=0;

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))==0) continue;

		mesh=&mdl->meshes[n];
		trig=mesh->trigs;

		draw->vbo_ptr.index_offset[n]=t_idx;

		for (k=0;k!=mesh->ntrig;k++) {

				// vertex 0

			offset=trig->v[0]*3;
			vp=mesh->draw.gl_vertex_array+offset;
			cp=mesh->draw.gl_color_array+offset;

			*vl++=*vp++;
			*vl++=*vp++;
			*vl++=*vp;

			*tl++=trig->gx[0];
			*tl++=trig->gy[0];

			*cl++=*cp++;
			*cl++=*cp++;
			*cl++=*cp;

				// vertex 1

			offset=trig->v[1]*3;
			vp=mesh->draw.gl_vertex_array+offset;
			cp=mesh->draw.gl_color_array+offset;

			*vl++=*vp++;
			*vl++=*vp++;
			*vl++=*vp;

			*tl++=trig->gx[1];
			*tl++=trig->gy[1];

			*cl++=*cp++;
			*cl++=*cp++;
			*cl++=*cp;

				// vertex 2

			offset=trig->v[2]*3;
			vp=mesh->draw.gl_vertex_array+offset;
			cp=mesh->draw.gl_color_array+offset;

			*vl++=*vp++;
			*vl++=*vp++;
			*vl++=*vp;

			*tl++=trig->gx[2];
			*tl++=trig->gy[2];

			*cl++=*cp++;
			*cl++=*cp++;
			*cl++=*cp;

				// indexes

			*index_ptr++=(unsigned short)t_idx;
			*index_ptr++=(unsigned short)(t_idx+1);
			*index_ptr++=(unsigned short)(t_idx+2);

			t_idx+=3;
			
			trig++;
		}
	}

		// unmap VBO

	view_unmap_current_index_object();
	view_unmap_current_vertex_object();

		// set the pointers
		// glow maps use two texture units

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,(void*)0);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)(((ntrig*3)*3)*sizeof(float)));
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,(void*)(((ntrig*3)*3)*sizeof(float)));

	return(TRUE);
}

inline int model_draw_set_vertex_objects(model_type *mdl,int mesh_idx,model_draw *draw)
{
	return(draw->vbo_ptr.index_offset[mesh_idx]);
}

inline void model_draw_enable_color_array(model_draw *draw)
{
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,(void*)(((draw->vbo_ptr.ntrig*3)*(3+2))*sizeof(float)));
}

inline void model_draw_disable_color_array(void)
{
	glDisableClientState(GL_COLOR_ARRAY);
}

void model_draw_release_vertex_objects(void)
{
	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_VERTEX_ARRAY);

	view_unbind_current_index_object();
	view_unbind_current_vertex_object();
}

/* =======================================================

      Model Shadow Drawing Arrays
      
======================================================= */

bool model_draw_start_mesh_shadow_array(model_type *mdl,model_mesh_type *mesh)
{
	int				n,trig_count;
	float			*vl,*vp,*vertex_ptr;
	unsigned short	*index_ptr;
    model_trig_type	*trig;

	trig_count=mesh->ntrig;

 		// construct VBO

	vertex_ptr=view_bind_map_next_vertex_object(mesh->nvertex*2);
	if (vertex_ptr==NULL) return(FALSE);

	index_ptr=view_bind_map_next_index_object(trig_count*3);
	if (index_ptr==NULL) {
		view_unmap_current_vertex_object();
		view_unbind_current_vertex_object();
		return(FALSE);
	}
	
		// build the vertexs and indexes
		// there's no z coordinate in these arrays

	vl=vertex_ptr;
	vp=mesh->draw.gl_vertex_array;

	for (n=0;n!=mesh->nvertex;n++) {
		*vl++=*vp++;
		*vl++=*vp++;
	}

	trig=mesh->trigs;

	for (n=0;n!=trig_count;n++) {

		*index_ptr++=(unsigned short)trig->v[0];
		*index_ptr++=(unsigned short)trig->v[1];
		*index_ptr++=(unsigned short)trig->v[2];
		
		trig++;
	}

		// unmap VBO

	view_unmap_current_index_object();
	view_unmap_current_vertex_object();

		// set the arrays
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,0);
	
	return(TRUE);
}

void model_draw_stop_mesh_shadow_array(void)
{
	glDisableClientState(GL_VERTEX_ARRAY);

	view_unbind_current_index_object();
	view_unbind_current_vertex_object();
}

/* =======================================================

      Draw Model Trigs
      
======================================================= */

void model_draw_opaque_simple_trigs(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,frame,trig_count,
							trig_start_idx,trig_idx;
	float					alpha;
	bool					enabled;
	model_mesh_type			*mesh;
    texture_type			*texture;
	model_material_type		*material;
	
	mesh=&mdl->meshes[mesh_idx];

		// setup correct mesh pointers

	trig_start_idx=model_draw_set_vertex_objects(mdl,mesh_idx,draw);
	
		// setup drawing

	glDisable(GL_BLEND);
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	enabled=FALSE;

	gl_texture_opaque_start();

		// run through the materials

	for (n=0;n!=max_model_texture;n++) {
	
		texture=&mdl->textures[n];
		material=&mesh->materials[n];

			// skip shader textures

		if (texture->shader_idx!=-1) continue;

			// first color pointer enable?

		if (!enabled) {
			enabled=TRUE;
			model_draw_enable_color_array(draw);
		}
	
			// any opaque trigs?
			
		frame=texture->animate.current_frame;
		
		alpha=draw->alpha;
		if (draw->mesh_fades[mesh_idx].on) alpha=draw->mesh_fades[mesh_idx].alpha;
			
		if ((texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) || (alpha!=1.0)) continue;

			// trig count

		trig_count=material->trig_count;
		if (trig_count==0) continue;

		trig_idx=trig_start_idx+(material->trig_start*3);

			// draw texture

		gl_texture_opaque_set(texture->frames[frame].bitmap.gl_id);
		glDrawRangeElements(GL_TRIANGLES,trig_idx,(trig_idx+(trig_count*3)),(trig_count*3),GL_UNSIGNED_SHORT,(GLvoid*)(trig_idx*sizeof(unsigned short)));
	}

	if (enabled) model_draw_disable_color_array();
			
	gl_texture_opaque_end();
}

void model_draw_opaque_shader_trigs(model_type *mdl,int mesh_idx,model_draw *draw,view_glsl_light_list_type *light_list)
{
	int						n,trig_count,frame,
							trig_start_idx,trig_idx;
	float					alpha;
	model_mesh_type			*mesh;
    texture_type			*texture;
	model_material_type		*material;
	
	mesh=&mdl->meshes[mesh_idx];

		// setup correct mesh pointers

	trig_start_idx=model_draw_set_vertex_objects(mdl,mesh_idx,draw);
	
		// setup drawing

	glDisable(GL_BLEND);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	gl_shader_draw_start();

		// run through the materials

	for (n=0;n!=max_model_texture;n++) {
	
		texture=&mdl->textures[n];
		material=&mesh->materials[n];

			// skip non-shader textures

		if (texture->shader_idx==-1) continue;
		
			// any opaque trigs?
			
		frame=texture->animate.current_frame;
		
		alpha=draw->alpha;
		if (draw->mesh_fades[mesh_idx].on) alpha=draw->mesh_fades[mesh_idx].alpha;
			
		if ((texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) || (alpha!=1.0)) continue;

			// don't draw if no trigs

		trig_count=material->trig_count;
		if (trig_count==0) continue;

		trig_idx=trig_start_idx+(material->trig_start*3);
		
			// run the shader
			
		if (!mesh->no_lighting) {
			gl_shader_draw_execute(texture,n,texture->animate.current_frame,1.0f,1.0f,light_list);
		}
		else {
			if (mesh->tintable) {
				gl_shader_draw_hilite_execute(texture,n,texture->animate.current_frame,1.0f,1.0f,&draw->pnt,&draw->tint);
			}
			else {
				gl_shader_draw_hilite_execute(texture,n,texture->animate.current_frame,1.0f,1.0f,&draw->pnt,NULL);
			}
		}
		
		glDrawRangeElements(GL_TRIANGLES,trig_idx,(trig_idx+(trig_count*3)),(trig_count*3),GL_UNSIGNED_SHORT,(GLvoid*)(trig_idx*sizeof(unsigned short)));
			
		gl_shader_draw_end();
	}
}

void model_draw_transparent_simple_trigs(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,frame,trig_count,
							trig_start_idx,trig_idx;
	float					alpha;
	bool					enabled,cur_additive,is_additive;
	model_mesh_type			*mesh;
    texture_type			*texture;
	model_material_type		*material;

	mesh=&mdl->meshes[mesh_idx];

		// setup correct mesh pointers

	trig_start_idx=model_draw_set_vertex_objects(mdl,mesh_idx,draw);

		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
		
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	
	gl_texture_transparent_start();

		// minimize state changes

	enabled=FALSE;
	cur_additive=FALSE;
	
		// run through textures

	for (n=0;n!=max_model_texture;n++) {
	
		texture=&mdl->textures[n];
		material=&mesh->materials[n];

			// skip shader textures

		if (texture->shader_idx!=-1) continue;

			// first color pointer enable?

		if (!enabled) {
			enabled=TRUE;
			model_draw_enable_color_array(draw);
		}
	
			// any transparent trigs?
			
		frame=texture->animate.current_frame;
		
		alpha=draw->alpha;
		if (draw->mesh_fades[mesh_idx].on) alpha=draw->mesh_fades[mesh_idx].alpha;
			
		if (!((texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) || (alpha!=1.0))) continue;

			// don't draw if no trigs

		trig_count=material->trig_count;
		if (trig_count==0) continue;

		trig_idx=trig_start_idx+(material->trig_start*3);
		
			// transparent textures
			
		is_additive=(mesh->blend_add) || (texture->additive);
		if (is_additive!=cur_additive) {
			cur_additive=is_additive;
			if (cur_additive) {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			}
			else {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			}
		}

			// draw texture

		gl_texture_transparent_set(texture->frames[texture->animate.current_frame].bitmap.gl_id,alpha);
		glDrawRangeElements(GL_TRIANGLES,trig_idx,(trig_idx+(trig_count*3)),(trig_count*3),GL_UNSIGNED_SHORT,(GLvoid*)(trig_idx*sizeof(unsigned short)));
	}

	if (enabled) model_draw_disable_color_array();

	gl_texture_transparent_end();
}

void model_draw_transparent_shader_trigs(model_type *mdl,int mesh_idx,model_draw *draw,view_glsl_light_list_type *light_list)
{
	int						n,frame,trig_count,
							trig_start_idx,trig_idx;
	float					alpha;
	bool					cur_additive,is_additive;
	model_mesh_type			*mesh;
    texture_type			*texture;
	model_material_type		*material;

	mesh=&mdl->meshes[mesh_idx];

		// setup correct mesh pointers

	trig_start_idx=model_draw_set_vertex_objects(mdl,mesh_idx,draw);

		// setup drawing

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
		
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	
	gl_shader_draw_start();
	
		// minimize state changes

	cur_additive=FALSE;

		// run through the materials

	for (n=0;n!=max_model_texture;n++) {
	
		texture=&mdl->textures[n];
		material=&mesh->materials[n];

			// skip non-shader textures

		if (texture->shader_idx==-1) continue;
		
			// any transparent trigs?
			
		frame=texture->animate.current_frame;
		
		alpha=draw->alpha;
		if (draw->mesh_fades[mesh_idx].on) alpha=draw->mesh_fades[mesh_idx].alpha;
			
		if (!((texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) || (alpha!=1.0))) continue;

			// don't draw if no trigs

		trig_count=material->trig_count;
		if (trig_count==0) continue;

		trig_idx=trig_start_idx+(material->trig_start*3);
		
			// transparent textures
			
		is_additive=(mesh->blend_add) || (texture->additive);
		if (is_additive!=cur_additive) {
			cur_additive=is_additive;
			if (cur_additive) {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);
			}
			else {
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			}
		}
		
			// run the shader
			
		if (!mesh->no_lighting) {
			gl_shader_draw_execute(texture,n,texture->animate.current_frame,1.0f,alpha,light_list);
		}
		else {
			if (mesh->tintable) {
				gl_shader_draw_hilite_execute(texture,n,texture->animate.current_frame,1.0f,alpha,&draw->pnt,&draw->tint);
			}
			else {
				gl_shader_draw_hilite_execute(texture,n,texture->animate.current_frame,1.0f,alpha,&draw->pnt,NULL);
			}
		}
		
		glDrawRangeElements(GL_TRIANGLES,trig_idx,(trig_idx+(trig_count*3)),(trig_count*3),GL_UNSIGNED_SHORT,(GLvoid*)(trig_idx*sizeof(unsigned short)));
			
		gl_shader_draw_end();
	}
}

void model_draw_glow_trigs(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,frame,trig_count,
							trig_start_idx,trig_idx;
	model_mesh_type			*mesh;
    texture_type			*texture;
	model_material_type		*material;
	
	mesh=&mdl->meshes[mesh_idx];

		// setup correct mesh pointers

	trig_start_idx=model_draw_set_vertex_objects(mdl,mesh_idx,draw);

		// setup drawing

	glDisable(GL_BLEND);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	gl_texture_glow_start();
	
		// run through the materials

	for (n=0;n!=max_model_texture;n++) {
	
			// only draw glow textures

		texture=&mdl->textures[n];
		frame=texture->animate.current_frame;

		if (texture->frames[frame].glowmap.gl_id==-1) continue;

		material=&mesh->materials[n];
	
			// trig count

		trig_count=material->trig_count;
		if (trig_count==0) continue;

		trig_idx=trig_start_idx+(material->trig_start*3);

			// draw glow texture
		
		gl_texture_glow_set(texture->frames[frame].bitmap.gl_id,texture->frames[frame].glowmap.gl_id,texture->glow.current_color);
		glDrawRangeElements(GL_TRIANGLES,trig_idx,(trig_idx+(trig_count*3)),(trig_count*3),GL_UNSIGNED_SHORT,(GLvoid*)(trig_idx*sizeof(unsigned short)));
	}
	
	gl_texture_glow_end();
}

/* =======================================================

      Draw Normal Model
      
======================================================= */

void model_render_setup(int tick,model_draw *draw)
{
	int					n,k,frame;
	float				alpha;
	model_type			*mdl;
    texture_type		*texture;
	
		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

		// setup animated textures

	model_setup_animated_textures(mdl,draw->cur_texture_frame,tick);

		// setup the rendering mesh mask

	draw->render_mesh_mask=draw->mesh_mask;

	for (n=0;n!=mdl->nmesh;n++) {
		if (draw->mesh_fades[n].on) {
			draw->render_mesh_mask|=(0x1<<n);
		}
	}

		// check for opaque/transparent textures

	draw->has_opaque=FALSE;
	draw->has_transparent=FALSE;

	texture=mdl->textures;

	for (n=0;n!=max_model_texture;n++) {
	
			// need to detect if fades are on for transparencies

		alpha=draw->alpha;
		for (k=0;k!=mdl->nmesh;k++) {
			if (draw->mesh_fades[k].on) {
				alpha=draw->mesh_fades[k].alpha;
				break;
			}
		}
		
			// check texture

		frame=texture->animate.current_frame;
		
		if ((texture->frames[frame].bitmap.alpha_mode==alpha_mode_transparent) || (alpha!=1.0)) {
			draw->has_transparent=TRUE;
		}
		else {
			draw->has_opaque=TRUE;
		}

		texture++;
	}
}

void model_render_opaque(model_draw *draw)
{
	int							n;
	model_type					*mdl;
	view_glsl_light_list_type	light_list;

		// any opaque?

	if (!draw->has_opaque) return;
	
		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

		// start lighting

	gl_lights_build_from_model(draw,&light_list);

		// setup the vbo

	if (!model_draw_initialize_vertex_objects(mdl,draw)) return;

		// draw opaque materials

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))!=0) {
			model_draw_opaque_simple_trigs(mdl,n,draw);
			if (!dim3_debug) model_draw_opaque_shader_trigs(mdl,n,draw,&light_list);
		}
	}

		// draw glow materials

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))!=0) {
			model_draw_glow_trigs(mdl,n,draw);
		}
	}

		// release the vbo

	model_draw_release_vertex_objects();
}

void model_render_transparent(model_draw *draw)
{
	int							n;
	model_type					*mdl;
	view_glsl_light_list_type	light_list;

		// any transparent?

	if (!draw->has_transparent) return;
	
		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;
	
		// start lighting

	gl_lights_build_from_model(draw,&light_list);

		// setup the vbo

	if (!model_draw_initialize_vertex_objects(mdl,draw)) return;	
	
		// draw transparent materials

	for (n=0;n!=mdl->nmesh;n++) {
		if ((draw->render_mesh_mask&(0x1<<n))!=0) {
			model_draw_transparent_simple_trigs(mdl,n,draw);
			if (!dim3_debug) model_draw_transparent_shader_trigs(mdl,n,draw,&light_list);
		}
	}

		// release the vbo

	model_draw_release_vertex_objects();
}

/* =======================================================

      Draw Shadow Model
      
======================================================= */

void model_render_shadow(model_draw *draw,float draw_sz,int shadow_idx)
{
	int						n,mesh_mask,
							xsz,ysz,xoff,yoff;
	float					min_x,max_x,min_y,max_y,
							f,fx[8],fy[8],fz[8],
							x_resize,y_resize;
	model_type				*mdl;
	model_mesh_type			*mesh;
	matrix_type				rot_x_mat,rot_z_mat,rot_y_mat;
	
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;
	
		// get the resize box
		
	matrix_rotate_x(&rot_x_mat,draw->setup.ang.x);
	matrix_rotate_z(&rot_z_mat,draw->setup.ang.z);
	matrix_rotate_y(&rot_y_mat,draw->setup.ang.y);

	f=(float)(mdl->view_box.size.x>>1);
	fx[0]=fx[3]=fx[4]=fx[7]=-f;
	fx[1]=fx[2]=fx[5]=fx[6]=f;
	
	f=(float)(mdl->view_box.size.z>>1);
	fz[0]=fz[1]=fz[2]=fz[3]=-f;
	fz[4]=fz[5]=fz[6]=fz[7]=f;
	
	f=(float)(mdl->view_box.size.y>>1);
	fy[0]=fy[1]=fy[4]=fy[5]=-f;
	fy[2]=fy[3]=fy[6]=fy[7]=f;
	
	for (n=0;n!=8;n++) {
		matrix_vertex_multiply(&rot_x_mat,&fx[n],&fy[n],&fz[n]);
		matrix_vertex_multiply(&rot_z_mat,&fx[n],&fy[n],&fz[n]);
		matrix_vertex_multiply(&rot_y_mat,&fx[n],&fy[n],&fz[n]);
	}
	
	min_x=max_x=fx[0];
	min_y=max_y=fy[0];
	for (n=1;n!=8;n++) {
		if (fx[n]<min_x) min_x=fx[n];
		if (fx[n]>max_x) max_x=fx[n];
		if (fy[n]<min_y) min_y=fy[n];
		if (fy[n]>max_y) max_y=fy[n];
	}
	
	xsz=abs((int)(max_x-min_x));
	ysz=abs((int)(max_y-min_y));
	
	fx[0]=(float)(mdl->view_box.size.x>>1);
	fy[0]=(float)mdl->view_box.size.y;
	fz[0]=(float)(mdl->view_box.size.z>>1);
	
	matrix_vertex_multiply(&rot_x_mat,&fx[0],&fy[0],&fz[0]);
	matrix_vertex_multiply(&rot_z_mat,&fx[0],&fy[0],&fz[0]);
	matrix_vertex_multiply(&rot_y_mat,&fx[0],&fy[0],&fz[0]);

	x_resize=draw_sz/(float)xsz;
	y_resize=draw_sz/(float)ysz;

	xoff=(xsz>>1)+((int)((draw_sz/x_resize)-xsz)>>1);
	yoff=(int)fy[0];

		// draw setup
		
	glEnable(GL_POLYGON_SMOOTH);
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA_SATURATE,GL_ONE);		// smooth out the shadows
		
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	
		// run through all the meshes
		
	mesh_mask=draw->mesh_mask;
	
	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))==0) continue;
		
		mesh=&mdl->meshes[n];
		if (mesh->ntrig==0) continue;

			// create vertexes

		model_create_draw_2D_vertexes(mdl,n,&draw->setup,xoff,yoff,x_resize,y_resize);
		
			// draw trigs
		
		if (!model_draw_start_mesh_shadow_array(mdl,mesh)) continue;
		
		glDrawRangeElements(GL_TRIANGLES,0,mesh->nvertex,(mesh->ntrig*3),GL_UNSIGNED_SHORT,(GLvoid*)0);
		
		model_draw_stop_mesh_shadow_array();
	}

	glDisable(GL_POLYGON_SMOOTH);
}

/* =======================================================

      Draw Target Model
      
======================================================= */

void model_render_target(model_draw *draw,d3col *col)
{
	int				ty,by,lx,rx,lz,rz,wid,xadd,zadd;
	float			rang;
	model_type		*mdl;
	
		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

		// get draw coordinates

	ty=draw->pnt.y-mdl->view_box.size.y;
	by=draw->pnt.y;

	rang=angle_find(view.render->camera.pnt.x,view.render->camera.pnt.z,draw->pnt.x,draw->pnt.z);

	wid=(mdl->view_box.size.x+mdl->view_box.size.z)>>2;

	lx=-wid;
	rx=wid;
	lz=rz=0;

	rotate_2D_point_center(&lx,&lz,rang);
	rotate_2D_point_center(&rx,&rz,rang);

	lx+=draw->pnt.x;
	rx+=draw->pnt.x;
	lz+=draw->pnt.z;
	rz+=draw->pnt.z;

		// move towards camera

	angle_get_movement(rang,wid,&xadd,&zadd);

	lx-=xadd;
	rx-=xadd;
	lz-=zadd;
	rz-=zadd;

		// draw target
	
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
			
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glColor4f(col->r,col->g,col->b,1.0f);

	glBegin(GL_LINE_LOOP);
	glVertex3i(lx,ty,lz);
	glVertex3i(rx,ty,rz);
	glVertex3i(rx,by,rz);
	glVertex3i(lx,by,lz);
	glEnd();
}
