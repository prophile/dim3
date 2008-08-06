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

      Diffuse Color List for Models
      
======================================================= */

void model_diffuse_color(model_type *mdl,int mesh_idx,int x,int z,int y,model_draw *draw)
{
	int				i,nt;
	float			f;
	float			*pc,*pvn,*pln;
	d3ang			org_ang;
	d3vct			v1,v2;
	
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
	
		// create vertex light points
	
	nt=mdl->meshes[mesh_idx].nvertex;
	
	pc=mdl->meshes[mesh_idx].draw.gl_color_array;
	pvn=mdl->meshes[mesh_idx].draw.gl_vertex_normal_array;
	pln=mdl->meshes[mesh_idx].draw.gl_light_normal_array;
		
	for (i=0;i!=nt;i++) {
	
			// get the diffuse light factor
			
		v1.x=*pln++;
		v1.y=*pln++;
		v1.z=*pln++;
			
		v2.x=*pvn++;
		v2.y=*pvn++;
		v2.z=*pvn++;

		f=1-vector_dot_product(&v1,&v2);
		if (f<0) f=0;
		
			// reduce diffuse factor a bit
			
		f=(f*0.8f)+0.2f;
		
			// diffuse the lighting
		
		*pc=(*pc)*f;
		pc++;
		*pc=(*pc)*f;
		pc++;
		*pc=(*pc)*f;
		pc++;
	}
}

/* =======================================================

      Color Lists for Models
      
======================================================= */

void model_build_color(model_type *mdl,int mesh_idx,int x,int z,int y,model_draw *draw)
{
	int				i,nt,lit;
	float			col[3],normal[3],r,g,b,
					fx,fy,fz,cx,cy,cz,nx,ny,nz,kx,ky,kz;
	float			*pc,*pv,*pln;
	double			dx,dy,dz;
	matrix_type		mat;
	model_mesh_type	*mesh;
		
	fx=(float)x;
	fy=(float)y;
	fz=(float)z;

	lit=draw->lit_type;
	
		// run the lighting
		
	mesh=&mdl->meshes[mesh_idx];
	nt=mesh->nvertex;
	pc=mesh->draw.gl_color_array;
	pln=mesh->draw.gl_light_normal_array;

		// special per-mesh no lighting

	if (mesh->no_lighting) {

		for (i=0;i!=nt;i++) {
			*pc++=1.0f;
			*pc++=1.0f;
			*pc++=1.0f;
			
			*pln++=1.0f;
			*pln++=0.0f;
			*pln++=1.0f;
		}

		return;
	}
	
		// if no rotation on model, setup to
		// mesh and center to fix vertexes
		
	if (draw->no_rot.on) {
		matrix_rotate_y(&mat,draw->no_rot.ang.y);
		cx=(float)draw->no_rot.center.x;
		cy=(float)draw->no_rot.center.y;
		cz=(float)draw->no_rot.center.z;
	}
	else {
		cx=cy=cz=0.0f;				// dumb compiler throws warnings here
	}
	
		// flat lighting
		
	if (lit==ml_flat) {

		if (draw->no_rot.on) {
			fx-=cx;
			fy-=cy;
			fz-=cz;
			matrix_vertex_multiply(&mat,&fx,&fy,&fz);
			fx+=cx;
			fy+=cy;
			fz+=cz;
		}

		map_calculate_light_color_normal((double)fx,(double)fy,(double)fz,col,normal);
		
		r=col[0];
		g=col[1];
		b=col[2];
		nx=normal[0];
		ny=normal[1];
		nz=normal[2];
		
		for (i=0;i!=nt;i++) {
			*pc++=r;
			*pc++=g;
			*pc++=b;
			
			*pln++=nx;
			*pln++=ny;
			*pln++=nz;
		}
		
		return;
	}
	
		// hilighting
		
	if ((lit==ml_hilite) || (lit==ml_hilite_diffuse)) {

		if (draw->no_rot.on) {
			fx-=cx;
			fy-=cy;
			fz-=cz;
			matrix_vertex_multiply(&mat,&fx,&fy,&fz);
			fx+=cx;
			fy+=cy;
			fz+=cz;
		}

		map_calculate_light_color_normal((double)fx,(double)fy,(double)fz,col,normal);

		nx=normal[0];
		ny=normal[1];
		nz=normal[2];
		
		for (i=0;i!=nt;i++) {
			*pc++=draw->hilite.r;
			*pc++=draw->hilite.g;
			*pc++=draw->hilite.b;
			
			*pln++=nx;
			*pln++=ny;
			*pln++=nz;
		}
		
		if ((setup.diffuse_lighting) && (lit==ml_hilite_diffuse)) model_diffuse_color(mdl,mesh_idx,x,z,y,draw);
		
		return;
	}
	
		// vertex lighting
		
	pv=(float*)mesh->draw.gl_vertex_array;
	
		// vertex lighting with rotation
		
	if (!draw->no_rot.on) {
			
		for (i=0;i!=nt;i++) {
			dx=(double)((*pv++)+fx);
			dy=(double)((*pv++)+fy);
			dz=(double)((*pv++)+fz);

			map_calculate_light_color_normal(dx,dy,dz,pc,pln);

			pc+=3;
			pln+=3;
		}
	}
	
		// vertex lighting with no rotation
		
	else {
			
		for (i=0;i!=nt;i++) {
			kx=((*pv++)+fx)-cx;
			ky=((*pv++)+fy)-cy;
			kz=((*pv++)+fz)-cz;
			matrix_vertex_multiply(&mat,&kx,&ky,&kz);
			dx=(double)(kx+cx);
			dy=(double)(ky+cy);
			dz=(double)(kz+cz);

			map_calculate_light_color_normal(dx,dy,dz,pc,pln);

			pc+=3;
			pln+=3;
		}
	}
	
		// diffuse colors

	if (setup.diffuse_lighting) model_diffuse_color(mdl,mesh_idx,x,z,y,draw);
}

/* =======================================================

      Tint Models
      
======================================================= */

void model_tint_team_color(model_type *mdl,int mesh_idx,model_draw *draw)
{
	int				i,nt;
	float			*pc;

	if ((draw->tint.r==1.0f) && (draw->tint.g==1.0f) && (draw->tint.b==1.0f)) return;

		// tint the colors

	nt=mdl->meshes[mesh_idx].nvertex;
	pc=mdl->meshes[mesh_idx].draw.gl_color_array;
	
	for (i=0;i!=nt;i++) {
		*pc=(*pc)*draw->tint.r;
		pc++;
		*pc=(*pc)*draw->tint.g;
		pc++;
		*pc=(*pc)*draw->tint.b;
		pc++;
	}
}

