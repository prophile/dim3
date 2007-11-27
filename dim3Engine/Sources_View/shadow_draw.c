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
	shadow_stencil_idx=stencil_segment_start;
}

int shadow_render_get_stencil(int offset)
{
	int			idx;
	
	idx=shadow_stencil_idx+offset;
	if (idx>=stencil_segment_end) idx=stencil_segment_start+(shadow_stencil_idx-stencil_segment_end);

	return(idx);
}

void shadow_render_stencil_add(int add)
{
	shadow_stencil_idx+=add;
	if (shadow_stencil_idx>=stencil_segment_end) shadow_stencil_idx=stencil_segment_start+(shadow_stencil_idx-stencil_segment_end);
}

/* =======================================================

      Stencil Shadow Segments
      
======================================================= */

int shadow_render_stencil_wall(int sptr_cnt,short *sptr,int ty,int y,int cnt,int *seg_idx)
{
	int					i,n,sx,sy,sz,idx;
	segment_type		*seg;
	wall_segment_data	*wall;
	
	for (i=0;i!=sptr_cnt;i++) {
		idx=(int)*sptr++;
		
		seg=&map.segments[idx];
		wall=&seg->data.wall;
		
		if (wall->by<=ty) continue;
		if (wall->ty>=y) continue;
		if (!polygon_2D_collision_line(wall->lx,wall->lz,wall->rx,wall->rz)) continue;

		glStencilFunc(GL_ALWAYS,shadow_render_get_stencil(cnt),0xFF);
		
		if (cnt>=max_shadow_segment) return(max_shadow_segment);
		seg_idx[cnt++]=idx;

		glBegin(GL_POLYGON);

		for (n=0;n!=wall->ptsz;n++) {
			sx=wall->x[n]-view.camera.pos.x;
			sy=wall->y[n]-view.camera.pos.y;
			sz=view.camera.pos.z-wall->z[n];
			glVertex3i(sx,sy,sz);
		}

		glEnd();
	}
	
	return(cnt);
}

int shadow_render_stencil_fc(int sptr_cnt,short *sptr,int ty,int by,int cnt,int *seg_idx)
{
	int					i,n,sx,sy,sz,idx;
	segment_type		*seg;
	fc_segment_data		*fc;
	
	for (i=0;i!=sptr_cnt;i++) {
		idx=(int)*sptr++;
		
		seg=&map.segments[idx];
		fc=&seg->data.fc;
		
		if (fc->max_y<ty) continue;
		if (fc->min_y>by) continue;
		if (!polygon_2D_collision_bound_polygon(fc->ptsz,fc->x,fc->z,fc->min_x,fc->max_x,fc->min_z,fc->max_z)) continue;
		
		glStencilFunc(GL_ALWAYS,shadow_render_get_stencil(cnt),0xFF);

		if (cnt>=max_shadow_segment) return(max_shadow_segment);
		seg_idx[cnt++]=idx;

		glBegin(GL_POLYGON);

		for (n=0;n!=fc->ptsz;n++) {
			sx=fc->x[n]-view.camera.pos.x;
			sy=fc->y[n]-view.camera.pos.y;
			sz=view.camera.pos.z-fc->z[n];
			glVertex3i(sx,sy,sz);
		}

		glEnd();
	}
	
	return(cnt);
}

int shadow_render_stencil_portal(portal_type *portal,int y,int ty,int by,int cnt,int *seg_idx)
{
		// wall segments
		
	cnt=shadow_render_stencil_wall(portal->wall_list_hit.count,portal->wall_list_hit.list,ty,y,cnt,seg_idx);
	if (cnt>=max_shadow_segment) return(max_shadow_segment);
	
		// floor segments
		
	cnt=shadow_render_stencil_fc(portal->fc_list_hit.count,portal->fc_list_hit.list,ty,by,cnt,seg_idx);
	
	return(cnt);
}

int shadow_render_stencil_map(model_draw_shadow *shadow,int *seg_idx)
{
	int						i,cnt,rn,ty,by,y,high;
	portal_type				*portal;
	portal_sight_list_type	*sight;
		
		// setup shadow volume to compare against map
		
	cnt=0;
	
	polygon_2D_collision_setup(4,shadow->px,shadow->pz);
	
		// find segments that intersect shadow volume
	
	rn=shadow->pos.rn;
	y=shadow->pos.y;
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
	
		// do current portal
		
	portal=&map.portals[rn];
	cnt=shadow_render_stencil_portal(portal,y,ty,by,cnt,seg_idx);
	
		// do portals touching current portal
		
	sight=portal->sight;

	for (i=0;i!=max_sight_list;i++) {
        if (sight->root) {
			cnt=shadow_render_stencil_portal(&map.portals[sight->rn],y,ty,by,cnt,seg_idx);
			if (cnt>=max_shadow_segment) break;
		}
		sight++;
	}

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

	glDisable(GL_STENCIL_TEST);
	
		// shadow segment count
		
	return(cnt);
}

/* =======================================================

      Draw Shadow To Stenciled Segment
      
======================================================= */

void shadow_render_draw_wall(segment_type *seg,int x,int y,int z,int *bx,int *bz,float gx,float gy,int high)
{
	int			n,kx,kz,lx,rx,lz,rz,by,px[4],pz[4],py[4],d;
	float		slice_percent;
	
		// find where polygon lines intersect wall line
		
	lx=seg->data.wall.lx;
    lz=seg->data.wall.lz;
    rx=seg->data.wall.rx;
    rz=seg->data.wall.rz;
		
	if (!line_2D_get_intersect(bx[2],bz[2],bx[1],bz[1],lx,lz,rx,rz,&px[2],&pz[2])) return;
	px[1]=px[2];
	pz[1]=pz[2];

	if (!line_2D_get_intersect(bx[3],bz[3],bx[0],bz[0],lx,lz,rx,rz,&px[3],&pz[3])) return;
	px[0]=px[3];
	pz[0]=pz[3];
	
        // move shadow for wall bottom
        
    by=seg->data.wall.by;
    if (y>by) by+=(y-by);
    
        // move shadow for distance
		
	kx=(px[0]+px[1])>>1;
	kz=(pz[0]+pz[1])>>1;
	
	d=distance_2D_get(x,z,kx,kz);
    by+=(high-(high-d));
    
    py[0]=py[1]=by-high;
    py[2]=py[3]=by;
	
		// draw the shadow to the stencil
		
	for (n=0;n!=4;n++) {
		px[n]-=view.camera.pos.x;
		py[n]-=view.camera.pos.y;
		pz[n]=view.camera.pos.z-pz[n];		// switch negative here
	}

	slice_percent=1.0f/(float)shadow_pbuffer_slice_count;
	
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

void shadow_render_draw_floor(segment_type *seg,int x,int y,int z,int *bx,int *bz,float gx,float gy)
{
	int				n,k,
					px[4],pz[4],py[4];
	float			slice_percent;
	fc_segment_data	*fc;
	
	fc=&seg->data.fc;
	
		// get x,y,z position on floor
		
	memmove(px,bx,(sizeof(int)*4));
	memmove(pz,bz,(sizeof(int)*4));
	
	if (fc->flat) {
		py[0]=py[1]=py[2]=py[3]=fc->y[0];
	}
	else {
		for (k=0;k!=4;k++) {
			py[k]=polygon_infinite_find_y(fc->ptsz,fc->x,fc->y,fc->z,px[k],pz[k]);
		}
	}
	
		// draw shadow
		
	for (n=0;n!=4;n++) {
		px[n]-=view.camera.pos.x;
		py[n]-=view.camera.pos.y;
		pz[n]=view.camera.pos.z-pz[n];		// switch negative here
	}

	slice_percent=1.0f/(float)shadow_pbuffer_slice_count;
	
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

void shadow_render_draw_segment(segment_type *seg,int x,int y,int z,int *bx,int *bz,float gx,float gy,int high)
{
	switch (seg->type) {
		case sg_wall:
			shadow_render_draw_wall(seg,x,y,z,bx,bz,gx,gy,high);
			return;
		case sg_floor:
			shadow_render_draw_floor(seg,x,y,z,bx,bz,gx,gy);
			return;
    }
}

void shadow_render_draw_map(model_draw_shadow *shadow,float gx,float gy,int seg_cnt,int *seg_idx)
{
	int				n,x,y,z,high;
	int				*px,*pz;
	float			ang_y;
	segment_type	*seg;

		// get volume

	x=shadow->pos.x;
	y=shadow->pos.y;
	z=shadow->pos.z;
	high=shadow->high;
	px=shadow->px;
	pz=shadow->pz;
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
		
	for (n=0;n!=seg_cnt;n++) {
		glStencilFunc(GL_EQUAL,shadow_render_get_stencil(n),0xFF);
		
		seg=&map.segments[seg_idx[n]];
		shadow_render_draw_segment(seg,x,y,z,px,pz,gx,gy,high);
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
	gl_2D_view();
		
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
	int					seg_cnt,
						seg_idx[max_shadow_segment];
	float				gx,gy,slice_percent;

		// stencil the segments that collide
		// with shadow volume
		
	seg_cnt=shadow_render_stencil_map(&draw->shadow,seg_idx);
	if (seg_cnt==0) return;
	
		// get the texture offsets
		
	slice_percent=1.0f/(float)shadow_pbuffer_slice_count;

	gx=((float)(draw->shadow.texture_idx%shadow_pbuffer_slice_count))*slice_percent;
	gy=((float)(draw->shadow.texture_idx/shadow_pbuffer_slice_count))*slice_percent;

		// draw shadow to stenciled segments

	shadow_render_draw_map(&draw->shadow,gx,gy,seg_cnt,seg_idx);
	
		// move up the stencil index
		
	shadow_render_stencil_add(seg_cnt);
}

