/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Shadow Drawing

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
#include "consoles.h"

extern map_type				map;
extern view_type			view;

int							shadow_stencil_idx;

/* =======================================================

      Stencil Shadow Utilities
      
======================================================= */

void shadow_render_init(void)
{
	shadow_stencil_idx=stencil_poly_start;

	glEnable(GL_STENCIL_TEST);
	glClear(GL_STENCIL_BUFFER_BIT);
	glDisable(GL_STENCIL_TEST);
}

int shadow_render_get_stencil(int offset)
{
	int			idx;
	
	idx=shadow_stencil_idx+offset;
	if (idx>=stencil_poly_end) idx=stencil_poly_start+(shadow_stencil_idx-stencil_poly_end);

	return(idx);
}

void shadow_render_stencil_add(int add)
{
	shadow_stencil_idx+=add;
	if (shadow_stencil_idx>=stencil_poly_end) shadow_stencil_idx=stencil_poly_start+(shadow_stencil_idx-stencil_poly_end);
}

/* =======================================================

      Stencil Shadow Polygons
      
======================================================= */

int shadow_render_stencil_wall_like_poly(int mesh_idx,int poly_idx,int ty,int y,int cnt,poly_pointer_type *poly_list)
{
	int					n;
	d3pnt				*pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=&map.mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

	if (poly->box.max.y<=ty) return(cnt);
	if (poly->box.min.y>=y) return(cnt);
	if (!polygon_2D_collision_line(poly->line.lx,poly->line.lz,poly->line.rx,poly->line.rz)) return(cnt);

	glStencilFunc(GL_ALWAYS,shadow_render_get_stencil(cnt),0xFF);
	
	poly_list[cnt].mesh_idx=mesh_idx;
	poly_list[cnt].poly_idx=poly_idx;
	cnt++;

	glBegin(GL_POLYGON);

	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		glVertex3i(pt->x,pt->y,pt->z);
	}

	glEnd();
	
	return(cnt);
}

int shadow_render_stencil_floor_like_poly(int mesh_idx,int poly_idx,int ty,int by,int cnt,poly_pointer_type *poly_list)
{
	int					n,px[8],pz[8];
	d3pnt				*pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=&map.mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
	if (poly->box.max.y<ty) return(cnt);
	if (poly->box.min.y>by) return(cnt);
	if (!polygon_2D_collision_bound_box(poly->box.min.x,poly->box.max.x,poly->box.min.z,poly->box.max.z)) return(cnt);

	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		px[n]=pt->x;
		pz[n]=pt->z;
	}

	if (!polygon_2D_collision_polygon(poly->ptsz,px,pz)) return(cnt);
	
	glStencilFunc(GL_ALWAYS,shadow_render_get_stencil(cnt),0xFF);

	poly_list[cnt].mesh_idx=mesh_idx;
	poly_list[cnt].poly_idx=poly_idx;
	cnt++;

	glBegin(GL_POLYGON);

	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		glVertex3i(pt->x,pt->y,pt->z);
	}

	glEnd();
	
	return(cnt);
}

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

	for (n=0;n!=view.render->mesh_draw.count;n++) {
		cnt=shadow_render_stencil_mesh(view.render->mesh_draw.items[n].idx,y,ty,by,cnt,poly_list);
	}

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

	glDisable(GL_STENCIL_TEST);
	
		// shadow poly count
		
	return(cnt);
}

/* =======================================================

      Draw Shadow To Stenciled Segment
      
======================================================= */

void shadow_render_draw_wall_like_poly(map_mesh_type *mesh,map_mesh_poly_type *poly,int x,int y,int z,int *bx,int *bz,float gx,float gy,int high)
{
	int			kx,kz,lx,rx,lz,rz,by,px[4],pz[4],py[4],d;
	float		slice_percent;
	
		// find where polygon lines intersect wall line
		
	lx=poly->line.lx;
    lz=poly->line.lz;
    rx=poly->line.rx;
    rz=poly->line.rz;
		
	if (!line_2D_get_intersect(bx[2],bz[2],bx[1],bz[1],lx,lz,rx,rz,&px[2],&pz[2])) return;
	px[1]=px[2];
	pz[1]=pz[2];

	if (!line_2D_get_intersect(bx[3],bz[3],bx[0],bz[0],lx,lz,rx,rz,&px[3],&pz[3])) return;
	px[0]=px[3];
	pz[0]=pz[3];
	
        // move shadow for wall bottom
        
    by=poly->box.max.y;
    if (y>by) by+=(y-by);
    
        // move shadow for distance
		
	kx=(px[0]+px[1])>>1;
	kz=(pz[0]+pz[1])>>1;
	
	d=distance_2D_get(x,z,kx,kz);
    by+=(high-(high-d));
    
    py[0]=py[1]=by-high;
    py[2]=py[3]=by;
	
		// draw the shadow to the stencil
		
	slice_percent=1.0f/(float)shadow_texture_slice_count;
	
	glBegin(GL_QUADS);
	
	glTexCoord2f(gx,gy);
	glVertex3i(px[0],py[0],pz[0]);
	glTexCoord2f((gx+slice_percent),gy);
	glVertex3i(px[1],py[1],pz[1]);
	glTexCoord2f((gx+slice_percent),(gy+slice_percent));
	glVertex3i(px[2],py[2],pz[2]);
	glTexCoord2f(gx,(gy+slice_percent));
	glVertex3i(px[3],py[3],pz[3]);

	glEnd();
}

void shadow_render_draw_floor_like_poly(map_mesh_type *mesh,map_mesh_poly_type *poly,int x,int y,int z,int *bx,int *bz,float gx,float gy)
{
	int				k,
					px[4],pz[4],py[4],
					fx[8],fy[8],fz[8];
	float			slice_percent;
	d3pnt			*pt;
	
		// get x,y,z position on floor
		
	memmove(px,bx,(sizeof(int)*4));
	memmove(pz,bz,(sizeof(int)*4));
	
	if (poly->box.flat) {
		py[0]=py[1]=py[2]=py[3]=poly->box.mid.y;
	}
	else {

		for (k=0;k!=poly->ptsz;k++) {
			pt=&mesh->vertexes[poly->v[k]];
			fx[k]=pt->x;
			fy[k]=pt->y;
			fz[k]=pt->z;
		}

		for (k=0;k!=4;k++) {
			py[k]=polygon_find_y(poly->ptsz,fx,fy,fz,px[k],pz[k]);
			if (py[k]==-1) py[k]=polygon_find_y_outside_point(poly->ptsz,fx,fy,fz,px[k],pz[k]);
		}
	}
	
		// draw shadow
		
	slice_percent=1.0f/(float)shadow_texture_slice_count;
	
	glBegin(GL_QUADS);

	glTexCoord2f(gx,gy);
	glVertex3i(px[0],py[0],pz[0]);
	glTexCoord2f((gx+slice_percent),gy);
	glVertex3i(px[1],py[1],pz[1]);
	glTexCoord2f((gx+slice_percent),(gy+slice_percent));
	glVertex3i(px[2],py[2],pz[2]);
	glTexCoord2f(gx,(gy+slice_percent));
	glVertex3i(px[3],py[3],pz[3]);

	glEnd();
}

void shadow_render_draw_map(model_draw_shadow *shadow,float gx,float gy,int poly_cnt,poly_pointer_type *poly_list)
{
	int					n,x,y,z,high;
	int					*bx,*bz;
	float				ang_y;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// get volume

	x=shadow->pnt.x;
	y=shadow->pnt.y;
	z=shadow->pnt.z;
	high=shadow->high;
	bx=shadow->px;
	bz=shadow->pz;
	ang_y=shadow->ang.y;

		// draw the shadow texture

	gl_shadow_texture_bind_start(shadow->alpha);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
		
	for (n=0;n!=poly_cnt;n++) {
		glStencilFunc(GL_EQUAL,shadow_render_get_stencil(n),0xFF);
		
		mesh=&map.mesh.meshes[poly_list[n].mesh_idx];
		poly=&mesh->polys[poly_list[n].poly_idx];

		if (poly->box.wall_like) {
			shadow_render_draw_wall_like_poly(mesh,poly,x,y,z,bx,bz,gx,gy,high);
		}
		else {
			shadow_render_draw_floor_like_poly(mesh,poly,x,y,z,bx,bz,gx,gy);
		}
	}
	
	glDisable(GL_STENCIL_TEST);

	gl_shadow_texture_bind_end();
}

/* =======================================================

      Shadow Testing Code
      
======================================================= */

void shadow_render_test(void)
{
	gl_setup_viewport(console_y_offset());
	gl_2D_view_screen();
		
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
		
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glBegin(GL_QUADS);
	glVertex2i(0,0);
	glVertex2i(256,0);
	glVertex2i(256,256);
	glVertex2i(0,256);
	glEnd();
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	gl_shadow_texture_bind_start(1.0f);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f);
	glVertex2i(0,0);
	glTexCoord2f(1.0f,0.0f);
	glVertex2i(256,0);
	glTexCoord2f(1.0f,1.0f);
	glVertex2i(256,256);
	glTexCoord2f(0.0f,1.0f);
	glVertex2i(0,256);
	glEnd();
	
	gl_shadow_texture_bind_end();		
}

/* =======================================================

      Draw Model Shadow
      
======================================================= */

void shadow_render(model_draw *draw)
{
	int					poly_cnt;
	float				gx,gy,slice_percent;
	poly_pointer_type	poly_list[max_shadow_poly];

		// stencil the segments that collide
		// with shadow volume
		
	poly_cnt=shadow_render_stencil_map(&draw->shadow,poly_list);
	if (poly_cnt==0) return;
	
		// get the texture offsets
		
	slice_percent=1.0f/(float)shadow_texture_slice_count;

	gx=((float)(draw->shadow.texture_idx%shadow_texture_slice_count))*slice_percent;
	gy=((float)(draw->shadow.texture_idx/shadow_texture_slice_count))*slice_percent;

		// draw shadow to stenciled segments

	shadow_render_draw_map(&draw->shadow,gx,gy,poly_cnt,poly_list);
	
		// move up the stencil index
		
	shadow_render_stencil_add(poly_cnt);
}

