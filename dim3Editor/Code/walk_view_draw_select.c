/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Selection Drawing Routines

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

#include "common_view.h"
#include "walk_view.h"

extern int					cr,cx,cz,cy;

extern map_type				map;

/* =======================================================

      Get Room Segments
      
======================================================= */

// supergumba -- delete!
int walk_view_get_wall_poly(segment_type *seg,int *x,int *z,int *y,float *gx,float *gy,int xadd,int zadd)
{
	int			ty,by,my;
	float		xtoff,ytoff,xfact,yfact;

	xtoff=seg->x_txtoff;
	ytoff=seg->y_txtoff;
	xfact=seg->x_txtfact;
	yfact=seg->y_txtfact;

	switch (seg->clip) {
	
		case wc_none:
			x[0]=x[1]=(seg->data.wall.lx*map_enlarge)+xadd;
			x[2]=x[3]=(seg->data.wall.rx*map_enlarge)+xadd;
			y[0]=y[3]=(seg->data.wall.by+1)*map_enlarge;
			y[1]=y[2]=seg->data.wall.ty*map_enlarge;
			z[0]=z[1]=(seg->data.wall.lz*map_enlarge)+zadd;
			z[2]=z[3]=(seg->data.wall.rz*map_enlarge)+zadd;
			
			if (gx!=NULL) {
				gx[0]=gx[1]=xtoff;
				gx[2]=gx[3]=xtoff+xfact;
				gy[0]=gy[3]=ytoff+yfact;
				gy[1]=gy[2]=ytoff;
			}
			return(4);
			
		case wc_top:
        case wc_top_curve:
		case wc_bottom_arch:
			x[0]=(seg->data.wall.rx*map_enlarge)+xadd;
			x[1]=x[2]=(seg->data.wall.lx*map_enlarge)+xadd;
			y[0]=y[2]=(seg->data.wall.by+1)*map_enlarge;
			y[1]=seg->data.wall.ty*map_enlarge;
			z[0]=(seg->data.wall.rz*map_enlarge)+zadd;
			z[1]=z[2]=(seg->data.wall.lz*map_enlarge)+zadd;

			if (gx!=NULL) {
				gx[0]=xtoff+xfact;
				gx[1]=gx[2]=xtoff;
				gy[0]=gy[2]=ytoff+yfact;
				gy[1]=ytoff;
			}
			return(3);
			
		case wc_bottom:
		case wc_bottom_curve:
 		case wc_top_arch:
			x[0]=x[1]=(seg->data.wall.rx*map_enlarge)+xadd;
			x[2]=(seg->data.wall.lx*map_enlarge)+xadd;
			y[0]=(seg->data.wall.by+1)*map_enlarge;
			y[1]=y[2]=seg->data.wall.ty*map_enlarge;
			z[0]=z[1]=(seg->data.wall.rz*map_enlarge)+zadd;
			z[2]=(seg->data.wall.lz*map_enlarge)+zadd;
		
			if (gx!=NULL) {
				gx[0]=gx[1]=xtoff+xfact;
				gx[2]=xtoff;
				gy[0]=ytoff+yfact;
				gy[1]=gy[2]=ytoff;
			}
			return(3);
			
		case wc_slant:
        case wc_top_curve_arch:
		case wc_bottom_curve_arch:
			ty=seg->data.wall.ty*map_enlarge;
			by=(seg->data.wall.by+1)*map_enlarge;
			my=(ty+by)/2;
			x[0]=x[1]=(seg->data.wall.lx*map_enlarge)+xadd;
			x[2]=x[3]=(seg->data.wall.rx*map_enlarge)+xadd;
			y[0]=y[2]=my;
			y[1]=ty;
			y[3]=by;
			z[0]=z[1]=(seg->data.wall.lz*map_enlarge)+zadd;
			z[2]=z[3]=(seg->data.wall.rz*map_enlarge)+zadd;
	
			if (gx!=NULL) {
				gx[0]=gx[1]=xtoff;
				gx[2]=gx[3]=xtoff+xfact;
				gy[0]=gy[2]=ytoff+(yfact/2);
				gy[1]=ytoff;
				gy[3]=ytoff+yfact;
			}
			return(4);
			
	}
	
	return(0);
}

void walk_view_get_ambient_wall_poly(segment_type *seg,int *x,int *z,int *y,int xadd,int zadd)
{
	int			lx,rx,lz,rz,ty,by;
	
	lx=(seg->data.ambient_wall.lx*map_enlarge)+xadd;
	rx=(seg->data.ambient_wall.rx*map_enlarge)+xadd;
	lz=(seg->data.ambient_wall.lz*map_enlarge)+zadd;
	rz=(seg->data.ambient_wall.rz*map_enlarge)+zadd;
	ty=seg->data.ambient_wall.ty*map_enlarge;
	by=(seg->data.ambient_wall.by+1)*map_enlarge;

	switch (seg->data.ambient_wall.push) {
		case ap_top:
			lz-=16;
			rz-=16;
			break;
		case ap_bottom:
			lz+=16;
			rz+=16;
			break;
		case ap_left:
			lx-=16;
			rx-=16;
			break;
		case ap_right:
			lx+=16;
			rx+=16;
			break;
	}

	x[0]=x[1]=lx;
	x[2]=x[3]=rx;
	z[0]=z[1]=lz;
	z[2]=z[3]=rz;
	y[0]=y[3]=by;
	y[1]=y[2]=ty;
}

void walk_view_get_ambient_fc_poly(segment_type *seg,int *x,int *z,int *y,float *gx,float *gy,int xadd,int zadd)
{
	int				t,ptsz,lft,rgt,top,bot;
	float			x_txtoff,x_txtfact,z_txtoff,z_txtfact;

	ptsz=seg->data.ambient_fc.ptsz;

	poly_find_bound_rect(ptsz,seg->data.ambient_fc.x,seg->data.ambient_fc.z,&lft,&rgt,&top,&bot);
	lft=lft*map_enlarge;
	rgt=rgt*map_enlarge;
	top=top*map_enlarge;
	bot=bot*map_enlarge;
	
	x_txtoff=seg->x_txtoff;
	x_txtfact=seg->x_txtfact;
	z_txtoff=seg->y_txtoff;
	z_txtfact=seg->y_txtfact;
	
	for (t=0;t<ptsz;t++) {
		x[t]=seg->data.ambient_fc.x[t]*map_enlarge;
		z[t]=seg->data.ambient_fc.z[t]*map_enlarge;
		y[t]=seg->data.ambient_fc.y[t]*map_enlarge;

		if (gx!=NULL) {
			gx[t]=x_txtoff+(((float)(x[t]-lft)*x_txtfact)/(float)(rgt-lft));
			gy[t]=z_txtoff+(((float)(z[t]-top)*z_txtfact)/(float)(bot-top));
		}
		
		x[t]+=xadd;
		z[t]+=zadd;
		
		switch (seg->data.ambient_fc.push) {
			case ap_up:
				y[t]-=16;
				break;
			case ap_down:
				y[t]+=16;
				break;
		}
	}
}

/* =======================================================

      Draw Selection for Segment
      
======================================================= */

void walk_view_draw_select_mesh(int rn,int mesh_idx,int poly_idx)
{
	int						k,t,x,y,z;
	d3pnt					*pt;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
	
	portal=&map.portals[rn];
	mesh=&portal->mesh.meshes[mesh_idx];
	
		// draw selected mesh
		
	glColor4f(1.0f,0.0f,0.0f,1.0f);
	
	glLineWidth(2.0f);
	
	mesh_poly=mesh->polys;
	
	for (k=0;k!=mesh->npoly;k++) {
	
		if (k==poly_idx) {
			glLineWidth(6.0f);
		}
		else {
			glLineWidth(2.0f);
		}
	
		glBegin(GL_LINE_LOOP);
		
		for (t=0;t!=mesh_poly->ptsz;t++) {
			pt=&mesh->vertexes[mesh_poly->v[t]];
			x=(pt->x+portal->x)-cx;
			y=pt->y-cy;
			z=cz-(pt->z+portal->z);
			glVertex3i(x,y,z);
		}
		
		glEnd();
		
		mesh_poly++;
	}
	
	glLineWidth(1.0f);
}

/* =======================================================

      Draw Selection for Sprite
      
======================================================= */

void walk_view_draw_select_sprite(d3pos *pos)
{
    int			x,z,y,wid,high;
	portal_type	*portal;
	
	portal=&map.portals[pos->rn];
  
    x=(pos->x+portal->x)-cx;
    y=(pos->y+1)-cy;
    z=(pos->z+portal->z)-cz;
    
    wid=map_enlarge*3;
    high=map_enlarge*4;
    
    glLineWidth(3);
	glColor4f(1,0,0,1);
    
	glBegin(GL_LINE_LOOP);
	glVertex3i((x-wid),(y-high),-(z-wid));
	glVertex3i((x+wid),(y-high),-(z-wid));
	glVertex3i((x+wid),y,-(z-wid));
	glVertex3i((x-wid),y,-(z-wid));
	glEnd();

	glBegin(GL_LINE_LOOP);
 	glVertex3i((x-wid),(y-high),-(z+wid));
	glVertex3i((x+wid),(y-high),-(z+wid));
	glVertex3i((x+wid),y,-(z+wid));
	glVertex3i((x-wid),y,-(z+wid));
	glEnd();
	
	glBegin(GL_LINE_LOOP);
 	glVertex3i((x-wid),(y-high),-(z-wid));
	glVertex3i((x-wid),(y-high),-(z+wid));
	glVertex3i((x-wid),y,-(z+wid));
	glVertex3i((x-wid),y,-(z-wid));
	glEnd();
	
	glBegin(GL_LINE_LOOP);
	glVertex3i((x+wid),(y-high),-(z-wid));
	glVertex3i((x+wid),(y-high),-(z+wid));
	glVertex3i((x+wid),y,-(z+wid));
	glVertex3i((x+wid),y,-(z-wid));
	glEnd();
    
    glLineWidth(1);
}

/* =======================================================

      Draw Select for Portal
      
======================================================= */

void walk_view_draw_select_portal(int rn)
{
	int						n,sel_count,
							type,portal_idx, main_idx,sub_idx;
	
	sel_count=select_count();
	if (sel_count==0) return;
	
		// draw the selection
		
	for (n=(sel_count-1);n>=0;n--) {
	
		select_get(n,&type,&portal_idx,&main_idx,&sub_idx);
		if (portal_idx!=rn) continue;
		
			// draw selection
			
		switch (type) {
		
			case mesh_piece:
				walk_view_draw_select_mesh(rn,main_idx,sub_idx);
				break;
				
			case node_piece:
				walk_view_draw_select_sprite(&map.nodes[main_idx].pos);
				break;
				
			case spot_piece:
				if (!walk_view_model_draw_select(&map.spots[main_idx].pos,&map.spots[main_idx].ang,map.spots[main_idx].display_model)) {
					walk_view_draw_select_sprite(&map.spots[main_idx].pos);
				}
				break;
				
			case scenery_piece:
				if (!walk_view_model_draw_select(&map.sceneries[main_idx].pos,&map.sceneries[main_idx].ang,map.sceneries[main_idx].model_name)) {
					walk_view_draw_select_sprite(&map.sceneries[main_idx].pos);
				}
				break;
				
			case light_piece:
				walk_view_draw_select_sprite(&map.lights[main_idx].pos);
				break;
				
			case sound_piece:
				walk_view_draw_select_sprite(&map.sounds[main_idx].pos);
				break;
				
			case particle_piece:
				walk_view_draw_select_sprite(&map.particles[main_idx].pos);
				break;
		}
	}
}
			
