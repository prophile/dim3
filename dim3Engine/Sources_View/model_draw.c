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

extern bool				hilite_on;

extern map_type			map;
extern server_type		server;
extern view_type		view;
extern setup_type		setup;

extern void model_build_color_setup_lights(model_type *mdl,model_draw *draw);
extern void model_build_color(model_type *mdl,int mesh_idx,int rn,int x,int z,int y,model_draw *draw);
extern void model_tint_team_color(model_type *mdl,int mesh_idx,model_draw *draw);

/* =======================================================

      Model Drawing Arrays
      
======================================================= */

void model_draw_start_mesh_material_array(model_type *mdl,model_mesh_type *mesh,model_material_type *material)
{
	int				n,trig_count,idx,sz;
	float			*vl,*tl,*cl,*nl,*vp,*cp,*np,
					*vertex_array,*coord_array,*color_array,*normal_array;
    model_trig_type	*trig;
	
	trig_count=material->trig_count;
	if (trig_count==0) return;
	
	trig=&mesh->trigs[material->trig_start];
        
		// build the arrays

	gl_render_array_start();
	
	vl=vertex_array=gl_render_array_get_current_vertex();
	tl=coord_array=gl_render_array_get_current_coord();
	cl=color_array=gl_render_array_get_current_color();
	nl=normal_array=gl_render_array_get_current_normal();

	for (n=0;n!=trig_count;n++) {

			// vertex 0

		idx=trig->v[0]*3;
		vp=mesh->gl_vertex_array+idx;
		cp=mesh->gl_color_array+idx;
		np=mesh->gl_vertex_normal_array+idx;

		*vl++=*vp++;
		*vl++=*vp++;
		*vl++=*vp;

		*tl++=trig->gx[0];
		*tl++=trig->gy[0];

		*cl++=*cp++;
		*cl++=*cp++;
		*cl++=*cp;
		
		*nl++=*np++;
		*nl++=*np++;
		*nl++=*np;

			// vertex 1

		idx=trig->v[1]*3;
		vp=mesh->gl_vertex_array+idx;
		cp=mesh->gl_color_array+idx;
		np=mesh->gl_vertex_normal_array+idx;

		*vl++=*vp++;
		*vl++=*vp++;
		*vl++=*vp;

		*tl++=trig->gx[1];
		*tl++=trig->gy[1];

		*cl++=*cp++;
		*cl++=*cp++;
		*cl++=*cp;
		
		*nl++=*np++;
		*nl++=*np++;
		*nl++=*np;

			// vertex 2

		idx=trig->v[2]*3;
		vp=mesh->gl_vertex_array+idx;
		cp=mesh->gl_color_array+idx;
		np=mesh->gl_vertex_normal_array+idx;

		*vl++=*vp++;
		*vl++=*vp++;
		*vl++=*vp;

		*tl++=trig->gx[2];
		*tl++=trig->gy[2];

		*cl++=*cp++;
		*cl++=*cp++;
		*cl++=*cp;
		
		*nl++=*np++;
		*nl++=*np++;
		*nl++=*np;
		
		trig++;
	}

		// set the arrays
		
	sz=(trig_count*9)*sizeof(float);

#ifdef D3_OS_MAC
	glVertexArrayRangeAPPLE(sz,vertex_array);
	glEnableClientState(GL_VERTEX_ARRAY_RANGE_APPLE);
#endif
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,vertex_array);

	glClientActiveTexture(GL_TEXTURE2);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,coord_array);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,coord_array);

	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,coord_array);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,color_array);
	
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT,0,normal_array);
	
#ifdef D3_OS_MAC
	glFlushVertexArrayRangeAPPLE(sz,vertex_array);
#endif
}

void model_draw_stop_mesh_material_array(void)
{
	glDisableClientState(GL_NORMAL_ARRAY);
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

	gl_render_array_stop();
}

/* =======================================================

      Model Shadow Drawing Arrays
      
======================================================= */

void model_draw_start_mesh_shadow_array(model_type *mdl,model_mesh_type *mesh)
{
	int				n,trig_count,sz;
	float			*vl,*vp,*vertex_array;
    model_trig_type	*trig;
	
		// start array

  	gl_render_array_start();
	vl=vertex_array=gl_render_array_get_current_vertex();
      
		// build the arrays
		// there's no z coordinate in these arrays


	trig_count=mesh->ntrig;
	trig=mesh->trigs;

	for (n=0;n!=trig_count;n++) {

			// vertex 0

		vp=mesh->gl_vertex_array+(trig->v[0]*2);

		*vl++=*vp++;
		*vl++=*vp;

			// vertex 1

		vp=mesh->gl_vertex_array+(trig->v[1]*2);

		*vl++=*vp++;
		*vl++=*vp;

			// vertex 2

		vp=mesh->gl_vertex_array+(trig->v[2]*2);

		*vl++=*vp++;
		*vl++=*vp;
		
		trig++;
	}

		// set the arrays
		
	sz=(trig_count*6)*sizeof(float);

#ifdef D3_OS_MAC
	glVertexArrayRangeAPPLE(sz,vertex_array);
	glEnableClientState(GL_VERTEX_ARRAY_RANGE_APPLE);
#endif
		
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,vertex_array);
	
#ifdef D3_OS_MAC
	glFlushVertexArrayRangeAPPLE(sz,vertex_array);
#endif
}

void model_draw_stop_mesh_shadow_array(void)
{
	glDisableClientState(GL_VERTEX_ARRAY);
		
#ifdef D3_OS_MAC
	glDisableClientState(GL_VERTEX_ARRAY_RANGE_APPLE);
#endif

	gl_render_array_stop();
}

/* =======================================================

      Draw Model Trigs
      
======================================================= */

void model_draw_opaque_trigs(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						i,frame,trig_count;
	float					alpha;
	model_mesh_type			*mesh;
    texture_type			*texture;
	model_material_type		*material;
	
	mesh=&mdl->meshes[mesh_idx];
	
		// run through the materials

	texture=mdl->textures;
	material=mesh->materials;

	for (i=0;i!=max_model_texture;i++) {
	
			// no shaders
			
		if (texture->shader.on) {
			texture++;
			material++;
			continue;
		}

			// any opaque trigs?
			
		frame=texture->animate.current_frame;
		
		alpha=draw->alpha;
		if (draw->mesh_fades[mesh_idx].on) alpha=draw->mesh_fades[mesh_idx].alpha;
			
		if ((texture->bitmaps[frame].alpha_mode==alpha_mode_transparent) || (alpha!=1.0)) {
			texture++;
			material++;
			continue;
		}

			// trig count

		trig_count=material->trig_count;
		if (trig_count==0) {
			texture++;
			material++;
			continue;
		}

			// create and start drawing arrays

		model_draw_start_mesh_material_array(mdl,mesh,material);
			
			// regular texture

		glDisable(GL_BLEND);
			
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL,0);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);

		gl_texture_opaque_start();
		gl_texture_opaque_set(texture->bitmaps[frame].gl_id);

		glDrawArrays(GL_TRIANGLES,0,(trig_count*3));
			
		gl_texture_opaque_end();

			// bump mapping
			// need to switch to normal array for bumping
			
		if ((setup.bump_mapping) && (texture->bumpmaps[frame].gl_id!=-1)) {

			glColorPointer(3,GL_FLOAT,0,gl_render_array_get_current_normal());

			glDisable(GL_BLEND);
			
			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_NOTEQUAL,0);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_TRUE);

			gl_texture_opaque_tesseled_bump_start();
			gl_texture_opaque_tesseled_bump_set(texture->bumpmaps[frame].gl_id);

			glDrawArrays(GL_TRIANGLES,0,(trig_count*3));
			
			gl_texture_opaque_tesseled_bump_end();

			glColorPointer(3,GL_FLOAT,0,gl_render_array_get_current_color());
		}

		if (!hilite_on) {

				// lighting

			glEnable(GL_BLEND);
			glBlendFunc(GL_ZERO,GL_SRC_COLOR);

			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_NOTEQUAL,0);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_FALSE);
			
			gl_texture_tesseled_lighting_start();
			gl_texture_tesseled_lighting_set(texture->bitmaps[frame].gl_id,1.0f);

			glDrawArrays(GL_TRIANGLES,0,(trig_count*3));
			
			gl_texture_tesseled_lighting_end();

				// specular

			if ((setup.specular_mapping) && (texture->specularmaps[frame].gl_id!=-1)) {

				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE,GL_ONE);

				glDisable(GL_ALPHA_TEST);
				
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_EQUAL);
				glDepthMask(GL_FALSE);

				gl_texture_tesseled_specular_start();
				gl_texture_tesseled_specular_set(texture->specularmaps[frame].gl_id);

				glDrawArrays(GL_TRIANGLES,0,(trig_count*3));
				
				gl_texture_tesseled_specular_end();

			}
		}

			// glow mapped textures

		if ((setup.glow_mapping) && (texture->glowmaps[frame].gl_id!=-1)) {
			
			glDisable(GL_BLEND);

			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_NOTEQUAL,0);
			
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_EQUAL);
			glDepthMask(GL_FALSE);

			gl_texture_opaque_glow_start();
			gl_texture_opaque_glow_set(texture->bitmaps[frame].gl_id,texture->glowmaps[frame].gl_id,texture->glow.current_color);

			glDrawArrays(GL_TRIANGLES,0,(trig_count*3));
			
			gl_texture_opaque_glow_end();
		}

			// stop drawing arrays

		model_draw_stop_mesh_material_array();

		texture++;
		material++;
	}
}

void model_draw_shader_trigs(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						n,trig_count,frame;
	d3pnt					pnt;
	model_mesh_type			*mesh;
    texture_type			*texture;
	model_material_type		*material;
	
	mesh=&mdl->meshes[mesh_idx];
	
		// run through the materials

	texture=mdl->textures;
	material=mesh->materials;

	for (n=0;n!=max_model_texture;n++) {

			// only shaders
			
		if (!texture->shader.on) {
			texture++;
			material++;
			continue;
		}

			// don't draw if no trigs

		trig_count=material->trig_count;
		if (trig_count==0) {
			texture++;
			material++;
			continue;
		}

			// create and start drawing arrays

		model_draw_start_mesh_material_array(mdl,mesh,material);
		
			// run the shader
			
		gl_shader_program_start();
		gl_texture_shader_start();
		
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		
		frame=texture->animate.current_frame;
		
		gl_texture_shader_set(texture->bitmaps[frame].gl_id,texture->bumpmaps[frame].gl_id,texture->specularmaps[frame].gl_id,texture->glowmaps[frame].gl_id);
		gl_shader_set_program(texture->shader.program_obj);
	
		pnt.x=draw->pos.x;
		pnt.y=draw->pos.y;
		pnt.z=draw->pos.z;
	
		gl_shader_set_variables(texture->shader.program_obj,&pnt,texture);
		
		glDrawArrays(GL_TRIANGLES,0,(trig_count*3));
			
		gl_texture_shader_end();
		gl_shader_program_end();

			// stop drawing arrays

		model_draw_stop_mesh_material_array();

		texture++;
		material++;
	}
}

void model_draw_transparent_trigs(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int						i,frame,trig_count;
	float					alpha;
	model_mesh_type			*mesh;
    texture_type			*texture;
	model_material_type		*material;

	mesh=&mdl->meshes[mesh_idx];
	
		// run through textures

	texture=mdl->textures;
	material=mesh->materials;

	for (i=0;i!=max_model_texture;i++) {
	
			// no shaders
			
		if (texture->shader.on) {
			texture++;
			material++;
			continue;
		}
	
			// any transparent trigs?
			
		frame=texture->animate.current_frame;
		
		alpha=draw->alpha;
		if (draw->mesh_fades[mesh_idx].on) alpha=draw->mesh_fades[mesh_idx].alpha;
			
		if (!((texture->bitmaps[frame].alpha_mode==alpha_mode_transparent) || (alpha!=1.0))) {
			texture++;
			material++;
			continue;
		}

			// don't draw if no trigs

		trig_count=material->trig_count;
		if (trig_count==0) {
			texture++;
			material++;
			continue;
		}

			// create and start drawing arrays

		model_draw_start_mesh_material_array(mdl,mesh,material);
		
			// transparent textures
			
		glEnable(GL_BLEND);
		if ((mesh->blend_add) || (texture->additive)) {
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		}
		else {
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		}

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL,0);
			
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);
		
		gl_texture_transparent_start();
		gl_texture_transparent_set(texture->bitmaps[texture->animate.current_frame].gl_id,alpha);

		glDrawArrays(GL_TRIANGLES,0,(trig_count*3));

		gl_texture_transparent_end();

			// specular mapped textures

		if ((setup.specular_mapping) && (texture->specularmaps[frame].gl_id!=-1)) {

			
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR,GL_ONE);

			glDisable(GL_ALPHA_TEST);
			
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_EQUAL);
			glDepthMask(GL_FALSE);
			
			gl_texture_transparent_specular_start();
			gl_texture_transparent_specular_set(texture->specularmaps[texture->animate.current_frame].gl_id,alpha);
		
			glDrawArrays(GL_TRIANGLES,0,(trig_count*3));
			
			gl_texture_transparent_specular_end();
		}

			// glow mapped textures

		if ((setup.glow_mapping) && (texture->glowmaps[frame].gl_id!=-1)) {

			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_ALPHA_TEST);
			glAlphaFunc(GL_NOTEQUAL,0);
			
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_EQUAL);
			glDepthMask(GL_FALSE);
			
			gl_texture_transparent_glow_start();
			gl_texture_transparent_glow_set(texture->bitmaps[texture->animate.current_frame].gl_id,texture->glowmaps[texture->animate.current_frame].gl_id,alpha,texture->glow.current_color);

			glDrawArrays(GL_TRIANGLES,0,(trig_count*3));
			
			gl_texture_transparent_glow_end();
		}

			// stop drawing arrays

		model_draw_stop_mesh_material_array();
		
		texture++;
		material++;
	}
}

/* =======================================================

      Draw Normal Model
      
======================================================= */

void model_render(int tick,model_draw *draw)
{
	int						n,rn,x,y,z,tx,ty,tz,mesh_mask;
	model_type				*mdl;
	
		// get model

	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;

		// setup animated textures

	model_setup_animated_textures(mdl,draw->cur_texture_frame,tick);
	
		// create light list for model and single drawing normal
		
	model_build_color_setup_lights(mdl,draw);
	map_portal_calculate_light_normal(&map.portals[draw->pos.rn],(double)draw->pos.x,(double)draw->pos.y,(double)draw->pos.z,draw->normal);

		// get the meshes to be drawn

	mesh_mask=draw->mesh_mask;

	for (n=0;n!=mdl->nmesh;n++) {
		if (draw->mesh_fades[n].on) {
			mesh_mask|=(0x1<<n);
		}
	}

		// create vertex and color lists
		
	rn=draw->pos.rn;
	x=draw->pos.x;
	y=draw->pos.y;
	z=draw->pos.z;
		
	tx=x-view.camera.pos.x;
	ty=y-view.camera.pos.y;
	tz=z-view.camera.pos.z;
	
	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))==0) continue;

			// build model vertex list
			
		model_create_draw_vertexes(mdl,n,&draw->setup);
		if (draw->resize!=1) model_resize_draw_vertex(mdl,n,draw->resize);

			// build color lists
			
		model_build_color(mdl,n,rn,x,z,y,draw);
		model_tint_team_color(mdl,n,draw);

			// translate vertex to view
			
		model_translate_draw_vertex(mdl,n,tx,ty,tz);
	}

		// texture binding optimization

	gl_texture_bind_start();

		// draw opaque materials

	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))!=0) {
			model_draw_opaque_trigs(mdl,n,draw);
			model_draw_shader_trigs(mdl,n,draw);
		}
	}
	
		// draw transparent materials

	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))!=0) model_draw_transparent_trigs(mdl,n,draw);
	}
}

/* =======================================================

      Draw Flat Model
      
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
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	
		// run through all the meshes
		
	mesh_mask=draw->mesh_mask;
	
	for (n=0;n!=mdl->nmesh;n++) {
		if ((mesh_mask&(0x1<<n))==0) continue;
		
		mesh=&mdl->meshes[n];
		if (mesh->ntrig==0) continue;

			// create vertexes

		model_create_draw_2D_vertexes(mdl,n,&draw->setup,xoff,yoff,x_resize,y_resize);
		
			// draw trigs
		
		model_draw_start_mesh_shadow_array(mdl,mesh);
		glDrawArrays(GL_TRIANGLES,0,(mesh->ntrig*3));
		model_draw_stop_mesh_shadow_array();
	}

	glDisable(GL_POLYGON_SMOOTH);
}



