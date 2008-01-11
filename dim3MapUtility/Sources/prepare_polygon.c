/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Pre-Calc Polygons of Segments

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif


// supergumba -- tons of this can be deleted

/* =======================================================

      Re-Orient UV Coordinates
      
======================================================= */

void map_prepare_orient_uv_polygon(int ptsz,float *gx,float *gy,int txt_ang)
{
	int				n;
	float			g;
	
	if (txt_ang==ta_0) return;
	
	for (n=0;n!=ptsz;n++) {
	
		switch (txt_ang) {
		
			case ta_90:
				g=gx[n];
				gx[n]=gy[n];
				gy[n]=-g;
				break;
				
			case ta_180:
				gx[n]=-gx[n];
				gy[n]=-gy[n];
				break;
				
			case ta_270:
				g=gx[n];
				gx[n]=-gy[n];
				gy[n]=g;
				break;
		
		}
	}
}

/* =======================================================

      Create Polygons for Wall Segments
      
======================================================= */

void map_prepare_create_wall_segment_polygon(segment_type *seg)
{
	int					my;
	wall_segment_data	*wall;
    
    wall=&seg->data.wall;
    
	switch (seg->clip) {
        case wc_none:
            wall->ptsz=4;
            wall->x[0]=wall->x[3]=wall->lx;
            wall->x[1]=wall->x[2]=wall->rx;
            wall->z[0]=wall->z[3]=wall->lz;
            wall->z[1]=wall->z[2]=wall->rz;
            wall->y[0]=wall->y[1]=wall->ty;
            wall->y[2]=wall->y[3]=wall->by;
            break;
		case wc_bottom:
			wall->ptsz=3;
			wall->x[0]=wall->lx;
			wall->x[1]=wall->x[2]=wall->rx;
			wall->z[0]=wall->lz;
			wall->z[1]=wall->z[2]=wall->rz;
			wall->y[0]=wall->y[1]=wall->ty;
			wall->y[2]=wall->by;
			break;
		case wc_top:
			wall->ptsz=3;
			wall->x[0]=wall->x[2]=wall->lx;
			wall->x[1]=wall->rx;
			wall->z[0]=wall->z[2]=wall->lz;
			wall->z[1]=wall->rz;
			wall->y[0]=wall->ty;
			wall->y[1]=wall->y[2]=wall->by;
			break;
		case wc_slant:
			wall->ptsz=4;
			my=(wall->by+wall->ty)>>1;
			wall->x[0]=wall->x[3]=wall->lx;
			wall->x[1]=wall->x[2]=wall->rx;
			wall->z[0]=wall->z[3]=wall->lz;
			wall->z[1]=wall->z[2]=wall->rz;
			wall->y[0]=wall->ty;
			wall->y[1]=wall->y[3]=(wall->by+wall->ty)>>1;
			wall->y[2]=wall->by;
			break;
	}
}

void map_prepare_create_wall_segment_uv(segment_type *seg)
{
	segment_draw_type	*draw;

    draw=&seg->draw;
    
	switch (seg->clip) {
        case wc_none:
            draw->gx[0]=draw->gx[3]=seg->x_txtoff;
            draw->gx[1]=draw->gx[2]=seg->x_txtoff+seg->x_txtfact;
            draw->gy[0]=draw->gy[1]=seg->y_txtoff;
            draw->gy[2]=draw->gy[3]=seg->y_txtoff+seg->y_txtfact;
            break;
		case wc_bottom:
			draw->gx[0]=seg->x_txtoff;
			draw->gx[1]=draw->gx[2]=seg->x_txtoff+seg->x_txtfact;
			draw->gy[0]=draw->gy[1]=seg->y_txtoff;
			draw->gy[2]=seg->y_txtoff+seg->y_txtfact;
			break;
		case wc_top:
			draw->gx[0]=draw->gx[2]=seg->x_txtoff;
			draw->gx[1]=seg->x_txtoff+seg->x_txtfact;
			draw->gy[0]=seg->y_txtoff;
			draw->gy[1]=draw->gy[2]=seg->y_txtoff+seg->y_txtfact;
			break;
		case wc_slant:
			draw->gx[0]=draw->gx[3]=seg->x_txtoff;
			draw->gx[1]=draw->gx[2]=seg->x_txtoff+seg->x_txtfact;
			draw->gy[0]=seg->y_txtoff;
			draw->gy[1]=draw->gy[3]=seg->y_txtoff+(seg->y_txtfact/2);
			draw->gy[2]=seg->y_txtoff+seg->y_txtfact;
			break;
	}
	
	map_prepare_orient_uv_polygon(seg->data.wall.ptsz,draw->gx,draw->gy,seg->txt_ang);
}

/* =======================================================

      Create UVs for FC Segments
      
======================================================= */

void map_prepare_create_fc_segment_uv(segment_type *seg)
{
	int					i,ptsz,lft,top;
    float				xsize,ysize,x_txtoff,y_txtoff,x_txtfact,y_txtfact;
    fc_segment_data		*fc;
	segment_draw_type	*draw;
    
	fc=&seg->data.fc;
	draw=&seg->draw;
    
 	lft=fc->min_x;
	top=fc->min_z;
	xsize=(float)(fc->max_x-lft);
	ysize=(float)(fc->max_z-top);
 
	x_txtoff=seg->x_txtoff;
	x_txtfact=seg->x_txtfact;
	y_txtoff=seg->y_txtoff;
	y_txtfact=seg->y_txtfact;

    ptsz=fc->ptsz;
   
    for (i=0;i!=ptsz;i++) {
		draw->gx[i]=x_txtoff+((x_txtfact*(float)(fc->x[i]-lft))/xsize);
		draw->gy[i]=y_txtoff+((y_txtfact*(float)(fc->z[i]-top))/ysize);
    }
	
	map_prepare_orient_uv_polygon(fc->ptsz,draw->gx,draw->gy,seg->txt_ang);
}

/* =======================================================

      Create Polygons for Ambient Wall Segments
      
======================================================= */

void map_prepare_create_push_ambient_wall_segment_polygon(segment_type *seg)
{
	int							lx,rx,lz,rz,ty,by;
	ambient_wall_segment_data	*ambient_wall;
    
	ambient_wall=&seg->data.ambient_wall;

	lx=ambient_wall->lx;
	rx=ambient_wall->rx;
	lz=ambient_wall->lz;
	rz=ambient_wall->rz;
	ty=ambient_wall->ty;
	by=ambient_wall->by;
	
	switch (ambient_wall->push) {
		case ap_top:
			lz-=ambient_push_size;
			rz-=ambient_push_size;
			break;
		case ap_bottom:
			lz+=ambient_push_size;
			rz+=ambient_push_size;
			break;
		case ap_left:
			lx-=ambient_push_size;
			rx-=ambient_push_size;
			break;
		case ap_right:
			lx+=ambient_push_size;
			rx+=ambient_push_size;
			break;
		case ap_up:
			ty-=ambient_push_size;
			by-=ambient_push_size;
			break;
		case ap_down:
			ty+=ambient_push_size;
			by+=ambient_push_size;
			break;
	}
   
	ambient_wall->x[0]=ambient_wall->x[3]=lx;
	ambient_wall->x[1]=ambient_wall->x[2]=rx;
	ambient_wall->z[0]=ambient_wall->z[3]=lz;
	ambient_wall->z[1]=ambient_wall->z[2]=rz;
	ambient_wall->y[0]=ambient_wall->y[1]=ty;
	ambient_wall->y[2]=ambient_wall->y[3]=by;
}

void map_prepare_create_ambient_wall_segment_uv(segment_type *seg)
{
	segment_draw_type	*draw;
    
	draw=&seg->draw;

	draw->gx[0]=draw->gx[3]=seg->x_txtoff+seg->x_txtfact;
	draw->gx[1]=draw->gx[2]=seg->x_txtoff;
	draw->gy[0]=draw->gy[1]=seg->y_txtoff;
	draw->gy[2]=draw->gy[3]=seg->y_txtoff+seg->y_txtfact;
	
	map_prepare_orient_uv_polygon(4,draw->gx,draw->gy,seg->txt_ang);
}

/* =======================================================

      Create Polygons for Ambient FC Segments
      
======================================================= */

void map_prepare_push_ambient_fc_segment_polygon(segment_type *seg)
{
	int							i,ptsz;
    ambient_fc_segment_data		*ambient_fc;
    
	ambient_fc=&seg->data.ambient_fc;
	
	ptsz=ambient_fc->ptsz;
   
    for (i=0;i!=ptsz;i++) {
	
		switch (ambient_fc->push) {
			case ap_top:
				ambient_fc->z[i]-=ambient_push_size;
				break;
			case ap_bottom:
				ambient_fc->z[i]+=ambient_push_size;
				break;
			case ap_left:
				ambient_fc->x[i]-=ambient_push_size;
				break;
			case ap_right:
				ambient_fc->x[i]+=ambient_push_size;
				break;
			case ap_up:
				ambient_fc->y[i]-=ambient_push_size;
				break;
			case ap_down:
				ambient_fc->y[i]+=ambient_push_size;
				break;
		}
    }
}

void map_prepare_create_ambient_fc_segment_uv(segment_type *seg)
{
	int						i,ptsz,lft,top;
    float					xsize,ysize,x_txtoff,y_txtoff,x_txtfact,y_txtfact;
    ambient_fc_segment_data	*ambient_fc;
	segment_draw_type		*draw;
    
	ambient_fc=&seg->data.ambient_fc;
 	draw=&seg->draw;
   
 	lft=ambient_fc->min_x;
	top=ambient_fc->min_z;
	xsize=(float)(ambient_fc->max_x-lft);
	ysize=(float)(ambient_fc->max_z-top);
 
	x_txtoff=seg->x_txtoff;
	x_txtfact=seg->x_txtfact;
	y_txtoff=seg->y_txtoff;
	y_txtfact=seg->y_txtfact;

    ptsz=ambient_fc->ptsz;
   
    for (i=0;i!=ptsz;i++) {
		draw->gx[i]=x_txtoff+((x_txtfact*(float)(ambient_fc->x[i]-lft))/xsize);
		draw->gy[i]=y_txtoff+((y_txtfact*(float)(ambient_fc->z[i]-top))/ysize);
    }
	
	map_prepare_orient_uv_polygon(ambient_fc->ptsz,draw->gx,draw->gy,seg->txt_ang);
}
