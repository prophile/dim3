/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Edit View Poly Routines

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
	#include "dim3editor.h"
#endif

extern map_type			map;

/* =======================================================

      Get Polygons for Segments
      
======================================================= */

void edit_view_poly_find_bound_rect(int ptsz,int *x,int *z,int *lft,int *rgt,int *top,int *bot)
{
	int			i,lx,rx,ty,by;
	
	lx=rx=x[0];
	ty=by=z[0];
	for (i=1;i<ptsz;i++) {
		if (x[i]<lx) lx=x[i];
		if (x[i]>rx) rx=x[i];
		if (z[i]<ty) ty=z[i];
		if (z[i]>by) by=z[i];
	}
	*lft=lx;
	*rgt=rx;
	*top=ty;
	*bot=by;
}

int edit_view_poly_for_wall(segment_type *seg,int *x,int *y,int *z,float *gx,float *gy,int xadd,int zadd)
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

void edit_view_poly_for_fc(segment_type *seg,int *x,int *y,int *z,float *gx,float *gy,int xadd,int zadd)
{
	int			n,ptsz,lft,rgt,top,bot;
	float		x_txtoff,x_txtfact,z_txtoff,z_txtfact;


	ptsz=seg->data.fc.ptsz;

	edit_view_poly_find_bound_rect(ptsz,seg->data.fc.x,seg->data.fc.z,&lft,&rgt,&top,&bot);
	lft=lft*map_enlarge;
	rgt=rgt*map_enlarge;
	top=top*map_enlarge;
	bot=bot*map_enlarge;
	
	x_txtoff=seg->x_txtoff;
	x_txtfact=seg->x_txtfact;
	z_txtoff=seg->y_txtoff;
	z_txtfact=seg->y_txtfact;
	
	for (n=0;n<ptsz;n++) {
		x[n]=seg->data.fc.x[n]*map_enlarge;
		y[n]=seg->data.fc.y[n]*map_enlarge;
		z[n]=seg->data.fc.z[n]*map_enlarge;

		gx[n]=x_txtoff+(((float)(x[n]-lft)*x_txtfact)/(float)(rgt-lft));
		gy[n]=z_txtoff+(((float)(z[n]-top)*z_txtfact)/(float)(bot-top));
		
        x[n]+=xadd;
        z[n]+=zadd;
	}
}

void edit_view_poly_for_poly(segment_type *seg,int *x,int *y,int *z,float *gx,float *gy,int xadd,int zadd)
{
	int			n,ptsz,lft,rgt,top,bot;

	ptsz=seg->data.fc.ptsz;

	edit_view_poly_find_bound_rect(ptsz,seg->data.fc.x,seg->data.fc.z,&lft,&rgt,&top,&bot);
	lft=lft*map_enlarge;
	rgt=rgt*map_enlarge;
	top=top*map_enlarge;
	bot=bot*map_enlarge;

	for (n=0;n<ptsz;n++) {
		x[n]=seg->data.fc.x[n]*map_enlarge;
		y[n]=seg->data.fc.y[n]*map_enlarge;
		z[n]=seg->data.fc.z[n]*map_enlarge;
	}

	map_prepare_create_poly_segment_uv(seg);

	for (n=0;n<ptsz;n++) {
        x[n]+=xadd;
        z[n]+=zadd;
	}
}

void edit_view_poly_for_ambient_wall(segment_type *seg,int *x,int *y,int *z,int xadd,int zadd)
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
	y[0]=y[3]=by;
	y[1]=y[2]=ty;
	z[0]=z[1]=lz;
	z[2]=z[3]=rz;
}

void edit_view_poly_for_ambient_fc(segment_type *seg,int *x,int *y,int *z,float *gx,float *gy,int xadd,int zadd)
{
	int				n,ptsz,lft,rgt,top,bot;
	float			x_txtoff,x_txtfact,z_txtoff,z_txtfact;

	ptsz=seg->data.ambient_fc.ptsz;

	edit_view_poly_find_bound_rect(ptsz,seg->data.ambient_fc.x,seg->data.ambient_fc.z,&lft,&rgt,&top,&bot);
	lft=lft*map_enlarge;
	rgt=rgt*map_enlarge;
	top=top*map_enlarge;
	bot=bot*map_enlarge;
	
	x_txtoff=seg->x_txtoff;
	x_txtfact=seg->x_txtfact;
	z_txtoff=seg->y_txtoff;
	z_txtfact=seg->y_txtfact;
	
	for (n=0;n<ptsz;n++) {
		x[n]=seg->data.ambient_fc.x[n]*map_enlarge;
		y[n]=seg->data.ambient_fc.y[n]*map_enlarge;
		z[n]=seg->data.ambient_fc.z[n]*map_enlarge;

		if (gx!=NULL) {
			gx[n]=x_txtoff+(((float)(x[n]-lft)*x_txtfact)/(float)(rgt-lft));
			gy[n]=z_txtoff+(((float)(z[n]-top)*z_txtfact)/(float)(bot-top));
		}
		
		x[n]+=xadd;
		z[n]+=zadd;
		
		switch (seg->data.ambient_fc.push) {
			case ap_up:
				y[n]-=16;
				break;
			case ap_down:
				y[n]+=16;
				break;
		}
	}
}

