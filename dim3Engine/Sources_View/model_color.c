/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Model Color and Diffuse

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
extern server_type			server;
extern setup_type			setup;
extern view_type			view;

/* =======================================================

      Model Color Utility Code
      
======================================================= */

inline void model_build_color_get_tint(model_mesh_type *mesh,model_draw *draw,float *tint_r,float *tint_g,float *tint_b)
{
	if (mesh->tintable) {
		*tint_r=draw->tint.r;
		*tint_g=draw->tint.g;
		*tint_b=draw->tint.b;
		return;
	}

	*tint_r=*tint_g=*tint_b=1.0f;
}

inline void model_build_color_get_matrix_center(model_draw *draw,matrix_type *mat,float *cx,float *cy,float *cz)
{
	if (draw->no_rot.on) {
		matrix_rotate_y(mat,draw->no_rot.ang.y);
		*cx=(float)draw->no_rot.center.x;
		*cy=(float)draw->no_rot.center.y;
		*cz=(float)draw->no_rot.center.z;
	}
}

void model_build_color_setup_model_normals(model_type *mdl,int mesh_idx,model_draw *draw)
{
	d3ang			org_ang;
	
		// get normals for diffuse lighting
		// if no rot, add in original rot to diffuse calc
	
	if (draw->no_rot.on) {
		memmove(&org_ang,&draw->setup.ang,sizeof(d3ang));
		draw->setup.ang.x=draw->no_rot.ang.x;
		draw->setup.ang.y=draw->no_rot.ang.y;
		draw->setup.ang.z=draw->no_rot.ang.z;
	}
	
		// get normals
		
	model_create_draw_normals(mdl,mesh_idx,&draw->setup);
	
		// restore angle if no rot
		
	if (draw->no_rot.on) {
		memmove(&draw->setup.ang,&org_ang,sizeof(d3ang));
	}
}

/* =======================================================

      No Lighting Mesh
      
======================================================= */

void model_build_color_no_light(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int				n,nt;
	float			tint_r,tint_g,tint_b;
	float			*pc;
	model_mesh_type	*mesh;
		
	mesh=&mdl->meshes[mesh_idx];
	
		// run the lighting
		
	nt=mesh->nvertex;
	pc=mesh->draw.gl_color_array;
	
	model_build_color_get_tint(mesh,draw,&tint_r,&tint_g,&tint_b);

		// special per-mesh no lighting

	for (n=0;n!=nt;n++) {
		*pc++=tint_r;
		*pc++=tint_g;
		*pc++=tint_b;
	}
}

/* =======================================================

      Flat Lighting Mesh
      
======================================================= */

void model_build_color_flat(model_type *mdl,int mesh_idx,float fx,float fy,float fz,model_draw *draw)
{
	int				n,nt;
	float			col[3],normal[3],f_intensity,r,g,b,tint_r,tint_g,tint_b,
					cx,cy,cz;
	float			*pc,*pn;
	matrix_type		mat;
	model_mesh_type	*mesh;
		
	mesh=&mdl->meshes[mesh_idx];
	
		// if no rotation on model, setup to
		// mesh and center to fix vertexes
		
	model_build_color_get_matrix_center(draw,&mat,&cx,&cy,&cz);
	
		// setup flat lighting value
		
	if (draw->no_rot.on) {
		fx-=cx;
		fy-=cy;
		fz-=cz;
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
		fx+=cx;
		fy+=cy;
		fz+=cz;
	}

	map_calculate_light_color_normal((double)fx,(double)fy,(double)fz,col,normal,&f_intensity);

	model_build_color_get_tint(mesh,draw,&tint_r,&tint_g,&tint_b);
	
	r=col[0]*tint_r;
	g=col[1]*tint_g;
	b=col[2]*tint_b;
	
		// build the color and specular color lists
		
	nt=mesh->nvertex;
	pc=mesh->draw.gl_color_array;
	pn=mesh->draw.gl_light_normal_array;
	
	for (n=0;n!=nt;n++) {
		*pc++=r;
		*pc++=g;
		*pc++=b;
		
		*pn++=0.5f;
		*pn++=0.5f;
		*pn++=1.0f;
	}
}

/* =======================================================

      Hilite Lighting Mesh
      
======================================================= */

void model_build_color_hilite(model_type *mdl,int mesh_idx,float fx,float fy,float fz,model_draw *draw)
{
	int				n,nt;
	float			r,g,b,tint_r,tint_g,tint_b;
	float			*pc,*pn;
	model_mesh_type	*mesh;
		
	mesh=&mdl->meshes[mesh_idx];
	
		// get high light color
		
	model_build_color_get_tint(mesh,draw,&tint_r,&tint_g,&tint_b);

	r=draw->hilite.r*tint_r;
	g=draw->hilite.g*tint_g;
	b=draw->hilite.b*tint_b;
	
		// setup the color and specular color list
		
	nt=mesh->nvertex;
	pc=mesh->draw.gl_color_array;
	pn=mesh->draw.gl_light_normal_array;

	for (n=0;n!=nt;n++) {
		*pc++=r;
		*pc++=g;
		*pc++=b;
		
		*pn++=0.5f;
		*pn++=0.5f;
		*pn++=1.0f;
	}
}

/* =======================================================

      Hilite Diffuse Lighting Mesh
      
======================================================= */

void model_build_color_hilite_diffuse(model_type *mdl,int mesh_idx,float fx,float fy,float fz,model_draw *draw)
{
	int				n,nt;
	float			col[3],normal[3],f_intensity,f,r,g,b,tint_r,tint_g,tint_b,
					cx,cy,cz;
	float			*pc,*pn,*pvn;
	d3vct			v1,v2;
	matrix_type		mat;
	model_mesh_type	*mesh;
		
	mesh=&mdl->meshes[mesh_idx];
	
		// if no rotation on model, setup to
		// mesh and center to fix vertexes
		
	model_build_color_get_matrix_center(draw,&mat,&cx,&cy,&cz);
	
		// create model normals for diffuse calculations
	
	model_build_color_setup_model_normals(mdl,mesh_idx,draw);
	pvn=mesh->draw.gl_vertex_normal_array;
	
		// get color and normal, need the normal for diffuse lighting
		// the color is ignored
		
	if (draw->no_rot.on) {
		fx-=cx;
		fy-=cy;
		fz-=cz;
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
		fx+=cx;
		fy+=cy;
		fz+=cz;
	}

	map_calculate_light_color_normal((double)fx,(double)fy,(double)fz,col,normal,&f_intensity);
	
	model_build_color_get_tint(mesh,draw,&tint_r,&tint_g,&tint_b);

	r=draw->hilite.r*tint_r;
	g=draw->hilite.g*tint_g;
	b=draw->hilite.b*tint_b;

	v1.x=normal[0];
	v1.y=normal[1];
	v1.z=normal[2];
	
		// create the colors and specular color
		
	nt=mesh->nvertex;
	pc=mesh->draw.gl_color_array;
	pn=mesh->draw.gl_light_normal_array;
	
	for (n=0;n!=nt;n++) {
	
			// get the diffuse light factor
		
		v2.x=*pvn++;
		v2.y=*pvn++;
		v2.z=*pvn++;

		f=1-vector_dot_product(&v1,&v2);
		if (f<0) f=0;
		
			// reduce diffuse factor a bit
			
		f=(f*0.8f)+0.2f;
		
			// set the color
			
		*pc++=r*f;
		*pc++=g*f;
		*pc++=b*f;
		
		*pn++=normal[0];
		*pn++=normal[1];
		*pn++=normal[2];
	}
}

/* =======================================================

      Vertex Lighting Mesh
      
======================================================= */

void model_build_color_vertex(model_type *mdl,int mesh_idx,float fx,float fy,float fz,model_draw *draw)
{
	int				n,nt;
	float			col[3],normal[3],f_intensity,f,tint_r,tint_g,tint_b,
					cx,cy,cz,kx,ky,kz;
	float			*pc,*pn,*pv,*pvn;
	double			dx,dy,dz;
	d3vct			v1,v2;
	matrix_type		mat;
	model_mesh_type	*mesh;
		
	mesh=&mdl->meshes[mesh_idx];
	
		// if no rotation on model, setup to
		// mesh and center to fix vertexes
		
	model_build_color_get_matrix_center(draw,&mat,&cx,&cy,&cz);

		// create model normals for diffuse calculations
	
	model_build_color_setup_model_normals(mdl,mesh_idx,draw);
	pvn=mesh->draw.gl_vertex_normal_array;
	
		// create the color and specular color values
		
	nt=mesh->nvertex;
	
	pv=(float*)mesh->draw.gl_vertex_array;
	pc=mesh->draw.gl_color_array;
	pn=mesh->draw.gl_light_normal_array;
	
	model_build_color_get_tint(mesh,draw,&tint_r,&tint_g,&tint_b);

		// vertex lighting with rotation
		
	if (!draw->no_rot.on) {
			
		for (n=0;n!=nt;n++) {
			dx=(double)((*pv++)+fx);
			dy=(double)((*pv++)+fy);
			dz=(double)((*pv++)+fz);

			map_calculate_light_color_normal(dx,dy,dz,col,normal,&f_intensity);
				
				// get the diffuse light factor
			
			v1.x=normal[0];
			v1.y=normal[1];
			v1.z=normal[2];
				
			v2.x=*pvn++;
			v2.y=*pvn++;
			v2.z=*pvn++;

			f=1-vector_dot_product(&v1,&v2);
			if (f<0) f=0;
			
				// reduce diffuse factor a bit
				
			f=(f*0.8f)+0.2f;
			
				// create the light
				
			*pc++=(col[0]*f)*tint_r;
			*pc++=(col[1]*f)*tint_g;
			*pc++=(col[2]*f)*tint_b;
			
			*pn++=normal[0];
			*pn++=normal[1];
			*pn++=normal[2];
		}
		
		return;
	}
	
		// vertex lighting with no rotation
		
	for (n=0;n!=nt;n++) {
		kx=((*pv++)+fx)-cx;
		ky=((*pv++)+fy)-cy;
		kz=((*pv++)+fz)-cz;
		matrix_vertex_multiply(&mat,&kx,&ky,&kz);
		dx=(double)(kx+cx);
		dy=(double)(ky+cy);
		dz=(double)(kz+cz);

		map_calculate_light_color_normal(dx,dy,dz,col,normal,&f_intensity);

			// get the diffuse light factor
		
		v1.x=normal[0];
		v1.y=normal[1];
		v1.z=normal[2];
			
		v2.x=*pvn++;
		v2.y=*pvn++;
		v2.z=*pvn++;

		f=1-vector_dot_product(&v1,&v2);
		if (f<0) f=0;
		
			// reduce diffuse factor a bit
			
		f=(f*0.8f)+0.2f;

			// create the light
			
		*pc++=(col[0]*f)*tint_r;
		*pc++=(col[1]*f)*tint_g;
		*pc++=(col[2]*f)*tint_b;
		
		*pn++=normal[0];
		*pn++=normal[1];
		*pn++=normal[2];
	}
}

/* =======================================================

      Mesh Lighting Mainline
      
======================================================= */

void model_build_color(model_type *mdl,int mesh_idx,int x,int y,int z,model_draw *draw)
{
		// no lighting meshes
		
	if (mdl->meshes[mesh_idx].no_lighting) {
		model_build_color_no_light(mdl,mesh_idx,draw);
		return;
	}

		// regular lighting types
		
	switch (draw->lit_type) {
	
		case ml_flat:
			model_build_color_flat(mdl,mesh_idx,(float)x,(float)y,(float)z,draw);
			return;
			
		case ml_hilite:
			model_build_color_hilite(mdl,mesh_idx,(float)x,(float)y,(float)z,draw);
			return;
			
		case ml_hilite_diffuse:
			model_build_color_hilite_diffuse(mdl,mesh_idx,(float)x,(float)y,(float)z,draw);
			return;
			
		default:
			model_build_color_vertex(mdl,mesh_idx,(float)x,(float)y,(float)z,draw);
			return;
	
	}
}

