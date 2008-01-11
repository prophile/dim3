/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Deconstruct Curved Segments into Multiple Segments

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

int							curve_size;
float						curve_float_div,curve_ang_add;

/* =======================================================

      Setup Curve Constants
      
======================================================= */

void map_prepare_setup_curve_constant(int curve_level)
{
	curve_size=(curve_level+1)*5;
    curve_float_div=(float)curve_size;
    curve_ang_add=(ANG_to_RAD*(90/curve_float_div));
}

/* =======================================================

      Breakup FC into Curves
      
======================================================= */

void map_prepare_draw_frwd_4_curve(map_type *map,segment_type *seg)
{
	int					i;
	float				dx,dy,dz,dgx,dgy;
    double				r,r2;
    segment_type		*curve_seg;
	segment_draw_type	*draw,*curve_draw;
    fc_segment_data		*fc,*curve_fc;
	segment_type		org_seg;
 
	memmove(&org_seg,seg,sizeof(segment_type));

	draw=&org_seg.draw;
	fc=&org_seg.data.fc;

    r=0;
    
    for (i=0;i!=curve_size;i++) {
	
		if (i==0) {
			curve_seg=seg;				// replace original segment with first part of curve
		}
		else {
			if (map->nsegment>=max_segment) break;
        
			curve_seg=&map->segments[map->nsegment];
			memmove(curve_seg,&org_seg,sizeof(segment_type));
			map->nsegment++;
        }
		
		curve_seg->simple_tessel=TRUE;

		curve_draw=&curve_seg->draw;
        curve_fc=&curve_seg->data.fc;
        
        r2=r+curve_ang_add;
        
        dx=(float)(fc->x[0]-fc->x[1]);
        dy=(float)(fc->y[0]-fc->y[1]);
        dz=(float)(fc->z[0]-fc->z[1]);
        dgx=draw->gx[0]-draw->gx[1];
        dgy=draw->gy[0]-draw->gy[1];
        
		curve_fc->x[0]=fc->x[1]+(int)(cos(r)*dx);
		curve_fc->y[0]=fc->y[0]+(int)(-sin(r)*dy);
        curve_fc->z[0]=fc->z[1]+(int)(cos(r)*dz);
        curve_draw->gx[0]=draw->gx[0]-((dgx*(float)i)/curve_float_div);
        curve_draw->gy[0]=draw->gy[1]+((dgy*(float)i)/curve_float_div);
        
		curve_fc->x[1]=fc->x[1]+(int)(cos(r2)*dx);
		curve_fc->y[1]=fc->y[0]+(int)(-sin(r2)*dy);
        curve_fc->z[1]=fc->z[1]+(int)(cos(r2)*dz);
        curve_draw->gx[1]=draw->gx[0]-((dgx*(float)(i+1))/curve_float_div);
        curve_draw->gy[1]=draw->gy[1]+((dgy*(float)(i+1))/curve_float_div);
        
        dx=(float)(fc->x[3]-fc->x[2]);
        dy=(float)(fc->y[3]-fc->y[2]);
        dz=(float)(fc->z[3]-fc->z[2]);
        dgx=draw->gx[3]-draw->gx[2];
        dgy=draw->gy[3]-draw->gy[2];
        
		curve_fc->x[3]=fc->x[2]+(int)(cos(r)*dx);
		curve_fc->y[3]=fc->y[3]+(int)(-sin(r)*dy);
        curve_fc->z[3]=fc->z[2]+(int)(cos(r)*dz);
        curve_draw->gx[3]=draw->gx[3]-((dgx*(float)i)/curve_float_div);
        curve_draw->gy[3]=draw->gy[2]+((dgy*(float)i)/curve_float_div);
        
		curve_fc->x[2]=fc->x[2]+(int)(cos(r2)*dx);
		curve_fc->y[2]=fc->y[3]+(int)(-sin(r2)*dy);
        curve_fc->z[2]=fc->z[2]+(int)(cos(r2)*dz);
        curve_draw->gx[2]=draw->gx[3]-((dgx*(float)(i+1))/curve_float_div);
        curve_draw->gy[2]=draw->gy[2]+((dgy*(float)(i+1))/curve_float_div);
    
        curve_fc->ptsz=curve_draw->ptsz=4;
		
        curve_seg->curve=cv_none;
//        map_prepare_set_fc_segment_square(curve_seg);

        r=r2;
    }
}

void map_prepare_draw_frwd_3_curve(map_type *map,segment_type *seg)
{
	int					i;
	float				dx,dy,dz,dgx,dgy;
    double				r,r2;
    segment_type		*curve_seg;
	segment_draw_type	*draw,*curve_draw;
    fc_segment_data		*fc,*curve_fc;
	segment_type		org_seg;
 
	memmove(&org_seg,seg,sizeof(segment_type));

	draw=&org_seg.draw;
	fc=&org_seg.data.fc;

    r=0;
    
    for (i=0;i!=curve_size;i++) {

		if (i==0) {
			curve_seg=seg;				// replace original segment with first part of curve
		}
		else {
			if (map->nsegment>=max_segment) break;
        
			curve_seg=&map->segments[map->nsegment];
			memmove(curve_seg,&org_seg,sizeof(segment_type));
			map->nsegment++;
        }
		
		curve_seg->simple_tessel=TRUE;

		curve_draw=&curve_seg->draw;
        curve_fc=&curve_seg->data.fc;

        r2=r+curve_ang_add;
        
        dx=(float)(fc->x[0]-fc->x[1]);
        dy=(float)(fc->y[0]-fc->y[1]);
        dz=(float)(fc->z[0]-fc->z[1]);
        dgx=draw->gx[0]-draw->gx[1];
        dgy=draw->gy[0]-draw->gy[1];
        
		curve_fc->x[0]=fc->x[1]+(int)(cos(r)*dx);
		curve_fc->y[0]=fc->y[0]+(int)(-sin(r)*dy);
        curve_fc->z[0]=fc->z[1]+(int)(cos(r)*dz);
        curve_draw->gx[0]=draw->gx[0]-((dgx*(float)i)/curve_float_div);
        curve_draw->gy[0]=draw->gy[1]+((dgy*(float)i)/curve_float_div);
        
		curve_fc->x[1]=fc->x[1]+(int)(cos(r2)*dx);
		curve_fc->y[1]=fc->y[0]+(int)(-sin(r2)*dy);
        curve_fc->z[1]=fc->z[1]+(int)(cos(r2)*dz);
        curve_draw->gx[1]=draw->gx[0]-((dgx*(float)(i+1))/curve_float_div);
        curve_draw->gy[1]=draw->gy[1]+((dgy*(float)(i+1))/curve_float_div);
        
        dx=(float)(fc->x[2]-fc->x[1]);
        dy=(float)(fc->y[2]-fc->y[1]);
        dz=(float)(fc->z[2]-fc->z[1]);
        dgx=draw->gx[2]-draw->gx[1];
        dgy=draw->gy[2]-draw->gy[1];
        
		curve_fc->x[3]=fc->x[1]+(int)(cos(r)*dx);
		curve_fc->y[3]=fc->y[2]+(int)(-sin(r)*dy);
        curve_fc->z[3]=fc->z[1]+(int)(cos(r)*dz);
        curve_draw->gx[3]=draw->gx[2]-((dgx*(float)i)/curve_float_div);
        curve_draw->gy[3]=draw->gy[1]+((dgy*(float)i)/curve_float_div);
        
		curve_fc->x[2]=fc->x[1]+(int)(cos(r2)*dx);
		curve_fc->y[2]=fc->y[2]+(int)(-sin(r2)*dy);
        curve_fc->z[2]=fc->z[1]+(int)(cos(r2)*dz);
        curve_draw->gx[2]=draw->gx[2]-((dgx*(float)(i+1))/curve_float_div);
        curve_draw->gy[2]=draw->gy[1]+((dgy*(float)(i+1))/curve_float_div);
        
        curve_fc->ptsz=curve_draw->ptsz=4;
		
        curve_seg->curve=cv_none;
//        map_prepare_set_fc_segment_square(curve_seg);
        
        r=r2;
    }
}

void map_prepare_draw_bkwd_4_curve(map_type *map,segment_type *seg)
{
	int					i;
	float				dx,dy,dz,dgx,dgy;
    double				r,r2;
    segment_type		*curve_seg;
	segment_draw_type	*draw,*curve_draw;
    fc_segment_data		*fc,*curve_fc;
	segment_type		org_seg;
  
	memmove(&org_seg,seg,sizeof(segment_type));

	draw=&org_seg.draw;
	fc=&org_seg.data.fc;

    r=0;
    
    for (i=0;i!=curve_size;i++) {

		if (i==0) {
			curve_seg=seg;				// replace original segment with first part of curve
		}
		else {
			if (map->nsegment>=max_segment) break;
        
			curve_seg=&map->segments[map->nsegment];
			memmove(curve_seg,&org_seg,sizeof(segment_type));
			map->nsegment++;
        }
		
 		curve_seg->simple_tessel=TRUE;

		curve_draw=&curve_seg->draw;
        curve_fc=&curve_seg->data.fc;

        r2=r+curve_ang_add;
        
        dx=(float)(fc->x[0]-fc->x[1]);
        dy=(float)(fc->y[0]-fc->y[1]);
        dz=(float)(fc->z[0]-fc->z[1]);
        dgx=draw->gx[0]-draw->gx[1];
        dgy=draw->gy[0]-draw->gy[1];
        
		curve_fc->x[0]=fc->x[0]-(int)(cos(r)*dx);
		curve_fc->y[0]=fc->y[1]-(int)(-sin(r)*dy);
        curve_fc->z[0]=fc->z[0]-(int)(cos(r)*dz);
        curve_draw->gx[0]=draw->gx[1]+((dgx*(float)i)/curve_float_div);
        curve_draw->gy[0]=draw->gy[0]+((dgy*(float)i)/curve_float_div);
        
		curve_fc->x[1]=fc->x[0]-(int)(cos(r2)*dx);
		curve_fc->y[1]=fc->y[1]-(int)(-sin(r2)*dy);
        curve_fc->z[1]=fc->z[0]-(int)(cos(r2)*dz);
        curve_draw->gx[1]=draw->gx[1]+((dgx*(float)(i+1))/curve_float_div);
        curve_draw->gy[1]=draw->gy[0]+((dgy*(float)(i+1))/curve_float_div);
        
        dx=(float)(fc->x[3]-fc->x[2]);
        dy=(float)(fc->y[3]-fc->y[2]);
        dz=(float)(fc->z[3]-fc->z[2]);
        dgx=draw->gx[3]-draw->gx[2];
        dgy=draw->gy[3]-draw->gy[2];
        
		curve_fc->x[3]=fc->x[3]-(int)(cos(r)*dx);
		curve_fc->y[3]=fc->y[2]-(int)(-sin(r)*dy);
        curve_fc->z[3]=fc->z[3]-(int)(cos(r)*dz);
        curve_draw->gx[3]=draw->gx[2]+((dgx*(float)i)/curve_float_div);
        curve_draw->gy[3]=draw->gy[3]+((dgy*(float)i)/curve_float_div);
        
		curve_fc->x[2]=fc->x[3]-(int)(cos(r2)*dx);
		curve_fc->y[2]=fc->y[2]-(int)(-sin(r2)*dy);
        curve_fc->z[2]=fc->z[3]-(int)(cos(r2)*dz);
        curve_draw->gx[2]=draw->gx[2]+((dgx*(float)(i+1))/curve_float_div);
        curve_draw->gy[2]=draw->gy[3]+((dgy*(float)(i+1))/curve_float_div);

        curve_fc->ptsz=curve_draw->ptsz=4;
		
        curve_seg->curve=cv_none;
//        map_prepare_set_fc_segment_square(curve_seg);
        
        r=r2;
    }
}

void map_prepare_draw_bkwd_3_curve(map_type *map,segment_type *seg)
{
	int					i;
	float				dx,dy,dz,dgx,dgy;
    double				r,r2;
	segment_type		*curve_seg;
	segment_draw_type	*draw,*curve_draw;
    fc_segment_data		*fc,*curve_fc;
	segment_type		org_seg;

	memmove(&org_seg,seg,sizeof(segment_type));

	draw=&org_seg.draw;
  	fc=&org_seg.data.fc;
 
    r=0;
    
    for (i=0;i!=curve_size;i++) {

		if (i==0) {
			curve_seg=seg;				// replace original segment with first part of curve
		}
		else {
			if (map->nsegment>=max_segment) break;
        
			curve_seg=&map->segments[map->nsegment];
			memmove(curve_seg,&org_seg,sizeof(segment_type));
			map->nsegment++;
        }
		
		curve_seg->simple_tessel=TRUE;

		curve_draw=&curve_seg->draw;
        curve_fc=&curve_seg->data.fc;

        r2=r+curve_ang_add;
        
        dx=(float)(fc->x[0]-fc->x[1]);
        dy=(float)(fc->y[0]-fc->y[1]);
        dz=(float)(fc->z[0]-fc->z[1]);
        dgx=draw->gx[0]-draw->gx[1];
        dgy=draw->gy[0]-draw->gy[1];
        
		curve_fc->x[0]=fc->x[0]-(int)(cos(r)*dx);
		curve_fc->y[0]=fc->y[1]-(int)(-sin(r)*dy);
        curve_fc->z[0]=fc->z[1]-(int)(cos(r)*dz);
        curve_draw->gx[0]=draw->gx[1]+((dgx*(float)i)/curve_float_div);
        curve_draw->gy[0]=draw->gy[1]+((dgy*(float)i)/curve_float_div);
        
		curve_fc->x[1]=fc->x[0]-(int)(cos(r2)*dx);
		curve_fc->y[1]=fc->y[1]-(int)(-sin(r2)*dy);
        curve_fc->z[1]=fc->z[1]-(int)(cos(r2)*dz);
        curve_draw->gx[1]=draw->gx[1]+((dgx*(float)(i+1))/curve_float_div);
        curve_draw->gy[1]=draw->gy[1]+((dgy*(float)(i+1))/curve_float_div);
        
        dx=(float)(fc->x[2]-fc->x[1]);
        dy=(float)(fc->y[2]-fc->y[1]);
        dz=(float)(fc->z[2]-fc->z[1]);
        dgx=draw->gx[2]-draw->gx[1];
        dgy=draw->gy[2]-draw->gy[1];
        
		curve_fc->x[3]=fc->x[2]-(int)(cos(r)*dx);
		curve_fc->y[3]=fc->y[1]-(int)(-sin(r)*dy);
        curve_fc->z[3]=fc->z[1]-(int)(cos(r)*dz);
        curve_draw->gx[3]=draw->gx[1]+((dgx*(float)i)/curve_float_div);
        curve_draw->gy[3]=draw->gy[1]+((dgy*(float)i)/curve_float_div);
        
		curve_fc->x[2]=fc->x[2]-(int)(cos(r2)*dx);
		curve_fc->y[2]=fc->y[1]-(int)(-sin(r2)*dy);
        curve_fc->z[2]=fc->z[1]-(int)(cos(r2)*dz);
        curve_draw->gx[2]=draw->gx[1]+((dgx*(float)(i+1))/curve_float_div);
        curve_draw->gy[2]=draw->gy[1]+((dgy*(float)(i+1))/curve_float_div);

        curve_fc->ptsz=curve_draw->ptsz=4;
		
        curve_seg->curve=cv_none;
//        map_prepare_set_fc_segment_square(curve_seg);
        
        r=r2;
    }
}

/* =======================================================

      FC Curve Mainline
      
======================================================= */

void map_prepare_create_fc_curve(map_type *map,segment_type *seg)
{
    fc_segment_data		*fc;
    
	fc=&seg->data.fc;
    
    switch (seg->curve) {

        case cv_forward:
            if (fc->ptsz==3) {
                map_prepare_draw_frwd_3_curve(map,seg);
                break;
            }
            map_prepare_draw_frwd_4_curve(map,seg);
            break;

        case cv_backward:
            if (fc->ptsz==3) {
                map_prepare_draw_bkwd_3_curve(map,seg);
                break;
            }
            map_prepare_draw_bkwd_4_curve(map,seg);
            break;
    }
}

/* =======================================================

      Breakup Walls into Curves
      
======================================================= */

void map_prepare_create_frwd_wall_curve(map_type *map,segment_type *seg,int lx,int rx,int lz,int rz,int ty,int by,float x_txtoff,float x_txtfact)
{
	int					i;
	float				dx,dz,x_txtfact_div;
    double				r,r2;
    segment_type		*curve_seg;
	segment_draw_type	*draw;
    wall_segment_data	*wall;
	segment_type		org_seg;

    x_txtoff=x_txtoff+x_txtfact;
    x_txtfact_div=x_txtfact/curve_float_div;
    
    r=0;

	memmove(&org_seg,seg,sizeof(segment_type));
    
    for (i=0;i!=curve_size;i++) {
	
		if (i==0) {
			curve_seg=seg;				// replace original segment with first part of curve
		}
		else {
			if (map->nsegment>=max_segment) break;
        
			curve_seg=&map->segments[map->nsegment];
			memmove(curve_seg,&org_seg,sizeof(segment_type));
			map->nsegment++;
        }
		
		curve_seg->simple_tessel=TRUE;
    
        r2=r+curve_ang_add;
        
        dx=(float)(rx-lx);
        dz=(float)(rz-lz);
        
		draw=&curve_seg->draw;
        wall=&curve_seg->data.wall;

		if (i==0) {
			wall->x[0]=wall->x[3]=wall->lx=rx;
			wall->z[0]=wall->z[3]=wall->lz=rz;
			draw->gx[0]=draw->gx[3]=x_txtoff;
		}
		else {
			wall->x[0]=wall->x[3]=wall->lx=lx+(int)(cos(r)*dx);
			wall->z[0]=wall->z[3]=wall->lz=rz-(int)(sin(r)*dz);
			draw->gx[0]=draw->gx[3]=x_txtoff-(x_txtfact_div*(float)i);
        }

		if (i==(curve_size-1)) {
			wall->x[1]=wall->x[2]=wall->rx=lx;
			wall->z[1]=wall->z[2]=wall->rz=lz;
			draw->gx[1]=draw->gx[2]=x_txtoff-x_txtfact;
		}
		else {
			wall->x[1]=wall->x[2]=wall->rx=lx+(int)(cos(r2)*dx);
			wall->z[1]=wall->z[2]=wall->rz=rz-(int)(sin(r2)*dz);
			draw->gx[1]=draw->gx[2]=x_txtoff-(x_txtfact_div*(float)(i+1));
		}

        curve_seg->curve=cv_none;
        
        r=r2;
    }
}

void map_prepare_create_bkwd_wall_curve(map_type *map,segment_type *seg,int lx,int rx,int lz,int rz,int ty,int by,float x_txtoff,float x_txtfact)
{
	int					i;
	float				dx,dz,x_txtfact_div;
    double				r,r2;
    segment_type		*curve_seg;
	segment_draw_type	*draw;
    wall_segment_data	*wall;
	segment_type		org_seg;

    x_txtfact_div=x_txtfact/curve_float_div;
    
    r=0;
  
	memmove(&org_seg,seg,sizeof(segment_type));

    for (i=0;i!=curve_size;i++) {
	
		if (i==0) {
			curve_seg=seg;				// replace original segment with first part of curve
		}
		else {
			if (map->nsegment>=max_segment) break;
        
			curve_seg=&map->segments[map->nsegment];
			memmove(curve_seg,&org_seg,sizeof(segment_type));
			map->nsegment++;
        }
		
		curve_seg->simple_tessel=TRUE;

        r2=r+curve_ang_add;
        
        dx=(float)(rx-lx);
        dz=(float)(rz-lz);
        
		draw=&curve_seg->draw;
        wall=&curve_seg->data.wall;

		if (i==0) {
			wall->x[0]=wall->x[3]=wall->lx=lx;
			wall->z[0]=wall->z[3]=wall->lz=lz;
			draw->gx[0]=draw->gx[3]=x_txtoff;
		}
		else {
			wall->x[0]=wall->x[3]=wall->lx=rx-(int)(cos(r)*dx);
			wall->z[0]=wall->z[3]=wall->lz=lz+(int)(sin(r)*dz);
			draw->gx[0]=draw->gx[3]=x_txtoff+(x_txtfact_div*(float)i);
		}

		if (i==(curve_size-1)) {
			wall->x[1]=wall->x[2]=wall->rx=rx;
			wall->z[1]=wall->z[2]=wall->rz=rz;
			draw->gx[1]=draw->gx[2]=x_txtoff+x_txtfact;
		}
		else {
			wall->x[1]=wall->x[2]=wall->rx=rx-(int)(cos(r2)*dx);
			wall->z[1]=wall->z[2]=wall->rz=lz+(int)(sin(r2)*dz);
			draw->gx[1]=draw->gx[2]=x_txtoff+(x_txtfact_div*(float)(i+1));
		}

        curve_seg->curve=cv_none;
        
        r=r2;
    }
}

/* =======================================================

      Wall Curve Mainline
      
======================================================= */

void map_prepare_create_wall_curve(map_type *map,segment_type *seg)
{
	int				lx,rx,lz,rz,ty,by;
	float			x_txtoff,x_txtfact;
    
    lx=seg->data.wall.lx;
    rx=seg->data.wall.rx;
    lz=seg->data.wall.lz;
    rz=seg->data.wall.rz;
    ty=seg->data.wall.ty;
    by=seg->data.wall.by;
    
    x_txtoff=seg->x_txtoff;
    x_txtfact=seg->x_txtfact;
    
        // create new curved segments
        
    switch (seg->curve) {
        case cv_forward:
			map_prepare_create_frwd_wall_curve(map,seg,lx,rx,lz,rz,ty,by,x_txtoff,x_txtfact);
            break;
        case cv_backward:
			map_prepare_create_bkwd_wall_curve(map,seg,lx,rx,lz,rz,ty,by,x_txtoff,x_txtfact);
            break;
    }
}

/* =======================================================

      Curved Wall Clips
      
======================================================= */

void map_prepare_create_wall_top_curve(map_type *map,segment_type *seg,long lx,long lz,long rx,long rz,long ty,long by,float x_txtoff,float x_txtfact,float y_txtoff,float y_txtfact)
{
    int					i;
    float				dx,dz,dy;
    double				r,r2;
    segment_type		*curve_seg;
	segment_draw_type	*draw;
    wall_segment_data	*wall;
	segment_type		org_seg;
    
    r=0;
    
    dx=(float)(rx-lx);
    dz=(float)(rz-lz);
    dy=(float)(by-ty);
	
	memmove(&org_seg,seg,sizeof(segment_type));
    
    for (i=0;i!=curve_size;i++) {
        r2=r+curve_ang_add;
        
		if (i==0) {
			curve_seg=seg;				// replace original segment with first part of curve
		}
		else {
			if (map->nsegment>=max_segment) break;
    
			curve_seg=&map->segments[map->nsegment];
			memmove(curve_seg,&org_seg,sizeof(segment_type));
			map->nsegment++;
		}
		
		curve_seg->simple_tessel=TRUE;
		
        wall=&curve_seg->data.wall;
		draw=&curve_seg->draw;

        wall->x[0]=wall->x[1]=wall->lx=lx+(int)(cos(r)*dx);
        wall->x[2]=wall->x[3]=wall->rx=lx+(int)(cos(r2)*dx);
        wall->z[0]=wall->z[1]=wall->lz=lz+(int)(cos(r)*dz);
        wall->z[2]=wall->z[3]=wall->rz=lz+(int)(cos(r2)*dz);
        wall->y[0]=wall->y[3]=wall->by=by;
        wall->y[1]=wall->y[2]=wall->ty=by-(int)(sin(r)*dy);
            
        draw->gx[0]=draw->gx[1]=x_txtoff+(float)(cos(r)*x_txtfact);
        draw->gx[2]=draw->gx[3]=x_txtoff+(float)(cos(r2)*x_txtfact);
        draw->gy[0]=draw->gy[3]=y_txtoff+y_txtfact;
        draw->gy[1]=draw->gy[2]=(y_txtoff+y_txtfact)-(float)(sin(r)*y_txtfact);

        wall->ptsz=draw->ptsz=4;
        curve_seg->clip=wc_none;

        if (map->nsegment>=max_segment) break;
    
        curve_seg=&map->segments[map->nsegment];
		
        memmove(curve_seg,&org_seg,sizeof(segment_type));
		curve_seg->simple_tessel=TRUE;

        wall=&curve_seg->data.wall;
		draw=&curve_seg->draw;
        map->nsegment++;

        wall->x[0]=wall->x[1]=wall->lx=lx+(int)(cos(r2)*dx);
        wall->x[2]=wall->rx=lx+(int)(cos(r)*dx);
        wall->z[0]=wall->z[1]=wall->lz=lz+(int)(cos(r2)*dz);
        wall->z[2]=wall->rz=lz+(int)(cos(r)*dz);
        
        wall->y[1]=wall->ty=by-(int)(sin(r2)*dy);
        wall->y[0]=wall->y[2]=wall->by=by-(int)(sin(r)*dy);
        
        draw->gx[0]=draw->gx[1]=x_txtoff+(float)(cos(r2)*x_txtfact);
        draw->gx[2]=x_txtoff+(float)(cos(r)*x_txtfact);
        draw->gy[1]=(y_txtoff+y_txtfact)-(float)(sin(r2)*y_txtfact);
        draw->gy[0]=draw->gy[2]=(y_txtoff+y_txtfact)-(float)(sin(r)*y_txtfact);
    
        wall->ptsz=draw->ptsz=3;
        curve_seg->clip=wc_none;
        
        r=r2;
    }
}

void map_prepare_create_wall_top_arch(map_type *map,segment_type *seg,long lx,long lz,long rx,long rz,long ty,long by,float x_txtoff,float x_txtfact,float y_txtoff,float y_txtfact)
{
    int					i;
    float				dx,dz,dy;
    double				r,r2;
    segment_type		*curve_seg;
 	segment_draw_type	*draw;
	wall_segment_data	*wall;
	segment_type		org_seg;
    
    r=0;
    
    dx=(float)(rx-lx);
    dz=(float)(rz-lz);
    dy=(float)(by-ty);
 
	memmove(&org_seg,seg,sizeof(segment_type));
   
    for (i=0;i!=curve_size;i++) {
        r2=r+curve_ang_add;
        
		if (i==0) {
			curve_seg=seg;				// replace original segment with first part of curve
		}
		else {
			if (map->nsegment>=max_segment) break;
    
			curve_seg=&map->segments[map->nsegment];
			memmove(curve_seg,&org_seg,sizeof(segment_type));
			map->nsegment++;
		}
		
		curve_seg->simple_tessel=TRUE;

        wall=&curve_seg->data.wall;
		draw=&curve_seg->draw;

        wall->x[0]=wall->x[1]=wall->lx=lx+(int)(cos(r)*dx);
        wall->x[2]=wall->x[3]=wall->rx=lx+(int)(cos(r2)*dx);
        wall->z[0]=wall->z[1]=wall->lz=lz+(int)(cos(r)*dz);
        wall->z[2]=wall->z[3]=wall->rz=lz+(int)(cos(r2)*dz);
        wall->y[0]=wall->y[3]=wall->by=by-(int)(sin(r2)*dy);
        wall->y[1]=wall->y[2]=wall->ty=ty;
            
        draw->gx[0]=draw->gx[1]=x_txtoff+(float)(cos(r)*x_txtfact);
        draw->gx[2]=draw->gx[3]=x_txtoff+(float)(cos(r2)*x_txtfact);
        draw->gy[0]=draw->gy[3]=(y_txtoff+y_txtfact)-(float)(sin(r2)*y_txtfact);
        draw->gy[1]=draw->gy[2]=y_txtoff;

        wall->ptsz=draw->ptsz=4;
        curve_seg->clip=wc_none;
    
        if (map->nsegment>=max_segment) break;
    
        curve_seg=&map->segments[map->nsegment];
        memmove(curve_seg,&org_seg,sizeof(segment_type));
		curve_seg->simple_tessel=TRUE;

        wall=&curve_seg->data.wall;
		draw=&curve_seg->draw;
		map->nsegment++;
        
        wall->x[0]=wall->x[1]=wall->lx=lx+(int)(cos(r)*dx);
        wall->x[2]=wall->rx=lx+(int)(cos(r2)*dx);
        wall->z[0]=wall->z[1]=wall->lz=lz+(int)(cos(r)*dz);
        wall->z[2]=wall->rz=lz+(int)(cos(r2)*dz);
        
        wall->y[1]=wall->by=by-(int)(sin(r)*dy);
        wall->y[0]=wall->y[2]=wall->ty=by-(int)(sin(r2)*dy);
        
        draw->gx[0]=draw->gx[1]=x_txtoff+(float)(cos(r)*x_txtfact);
        draw->gx[2]=x_txtoff+(float)(cos(r2)*x_txtfact);
        draw->gy[1]=(y_txtoff+y_txtfact)-(float)(sin(r)*y_txtfact);
        draw->gy[0]=draw->gy[2]=(y_txtoff+y_txtfact)-(float)(sin(r2)*y_txtfact);

        wall->ptsz=draw->ptsz=3;
        curve_seg->clip=wc_none;

        r=r2;
    }
}

void map_prepare_create_wall_bottom_curve(map_type *map,segment_type *seg,long lx,long lz,long rx,long rz,long ty,long by,float x_txtoff,float x_txtfact,float y_txtoff,float y_txtfact)
{
    int					i;
    float				dx,dz,dy;
    double				r,r2;
    segment_type		*curve_seg;
	segment_draw_type	*draw;
    wall_segment_data	*wall;
	segment_type		org_seg;
    
    r=0;
    
    dx=(float)(rx-lx);
    dz=(float)(rz-lz);
    dy=(float)(by-ty);

	memmove(&org_seg,seg,sizeof(segment_type));
    
    for (i=0;i!=curve_size;i++) {
        r2=r+curve_ang_add;
        
		if (i==0) {
			curve_seg=seg;				// replace original segment with first part of curve
		}
		else {
			if (map->nsegment>=max_segment) break;
    
			curve_seg=&map->segments[map->nsegment];
			memmove(curve_seg,&org_seg,sizeof(segment_type));
			map->nsegment++;
		}
		
		curve_seg->simple_tessel=TRUE;
		
        wall=&curve_seg->data.wall;
		draw=&curve_seg->draw;

        wall->x[0]=wall->x[1]=wall->lx=rx-(int)(cos(r)*dx);
        wall->x[2]=wall->x[3]=wall->rx=rx-(int)(cos(r2)*dx);
        wall->z[0]=wall->z[1]=wall->lz=rz-(int)(cos(r)*dz);
        wall->z[2]=wall->z[3]=wall->rz=rz-(int)(cos(r2)*dz);
        wall->y[0]=wall->y[3]=wall->by=ty;
        wall->y[1]=wall->y[2]=wall->ty=ty+(int)(sin(r)*dy);
            
        draw->gx[0]=draw->gx[1]=x_txtoff-(float)(cos(r)*x_txtfact);
        draw->gx[2]=draw->gx[3]=x_txtoff-(float)(cos(r2)*x_txtfact);
        draw->gy[0]=draw->gy[3]=y_txtoff+y_txtfact;
        draw->gy[1]=draw->gy[2]=(y_txtoff+y_txtfact)+(float)(sin(r)*y_txtfact);

        wall->ptsz=draw->ptsz=4;
        curve_seg->clip=wc_none;

        if (map->nsegment>=max_segment) break;
    
        curve_seg=&map->segments[map->nsegment];
		
        memmove(curve_seg,&org_seg,sizeof(segment_type));
		curve_seg->simple_tessel=TRUE;

        wall=&curve_seg->data.wall;
 		draw=&curve_seg->draw;
		map->nsegment++;

        wall->x[0]=wall->x[1]=wall->lx=rx-(int)(cos(r2)*dx);
        wall->x[2]=wall->rx=rx-(int)(cos(r)*dx);
        wall->z[0]=wall->z[1]=wall->lz=rz-(int)(cos(r2)*dz);
        wall->z[2]=wall->rz=rz-(int)(cos(r)*dz);
        
        wall->y[1]=wall->ty=ty+(int)(sin(r2)*dy);
        wall->y[0]=wall->y[2]=wall->by=ty+(int)(sin(r)*dy);
        
        draw->gx[0]=draw->gx[1]=x_txtoff-(float)(cos(r2)*x_txtfact);
        draw->gx[2]=x_txtoff-(float)(cos(r)*x_txtfact);
        draw->gy[1]=(y_txtoff+y_txtfact)+(float)(sin(r2)*y_txtfact);
        draw->gy[0]=draw->gy[2]=(y_txtoff+y_txtfact)+(float)(sin(r)*y_txtfact);
    
        wall->ptsz=draw->ptsz=3;
        curve_seg->clip=wc_none;
        
        r=r2;
    }
}

void map_prepare_create_wall_bottom_arch(map_type *map,segment_type *seg,long lx,long lz,long rx,long rz,long ty,long by,float x_txtoff,float x_txtfact,float y_txtoff,float y_txtfact)
{
    int					i;
    float				dx,dz,dy;
    double				r,r2;
    segment_type		*curve_seg;
	segment_draw_type	*draw;
    wall_segment_data	*wall;
	segment_type		org_seg;
    
    r=0;
    
    dx=(float)(rx-lx);
    dz=(float)(rz-lz);
    dy=(float)(by-ty);
	
	memmove(&org_seg,seg,sizeof(segment_type));
    
    for (i=0;i!=curve_size;i++) {
        r2=r+curve_ang_add;
        
		if (i==0) {
			curve_seg=seg;				// replace original segment with first part of curve
		}
		else {
			if (map->nsegment>=max_segment) break;
    
			curve_seg=&map->segments[map->nsegment];
			memmove(curve_seg,&org_seg,sizeof(segment_type));
			map->nsegment++;
		}
		
		curve_seg->simple_tessel=TRUE;

        wall=&curve_seg->data.wall;
		draw=&curve_seg->draw;

        wall->x[0]=wall->x[1]=wall->lx=rx-(int)(cos(r)*dx);
        wall->x[2]=wall->x[3]=wall->rx=rx-(int)(cos(r2)*dx);
        wall->z[0]=wall->z[1]=wall->lz=rz-(int)(cos(r)*dz);
        wall->z[2]=wall->z[3]=wall->rz=rz-(int)(cos(r2)*dz);
        wall->y[0]=wall->y[3]=wall->by=ty+(int)(sin(r2)*dy);
        wall->y[1]=wall->y[2]=wall->ty=by;
            
        draw->gx[0]=draw->gx[1]=(x_txtoff+x_txtfact)-(float)(cos(r)*x_txtfact);
        draw->gx[2]=draw->gx[3]=(x_txtoff+x_txtfact)-(float)(cos(r2)*x_txtfact);
        draw->gy[0]=draw->gy[3]=y_txtoff+(float)(sin(r2)*y_txtfact);
        draw->gy[1]=draw->gy[2]=y_txtoff+y_txtfact;

        wall->ptsz=draw->ptsz=4;
        curve_seg->clip=wc_none;
    
        if (map->nsegment>=max_segment) break;
    
        curve_seg=&map->segments[map->nsegment];
        memmove(curve_seg,&org_seg,sizeof(segment_type));
		curve_seg->simple_tessel=TRUE;

        wall=&curve_seg->data.wall;
		draw=&curve_seg->draw;
		map->nsegment++;
        
        wall->x[0]=wall->x[1]=wall->lx=rx-(int)(cos(r)*dx);
        wall->x[2]=wall->rx=rx-(int)(cos(r2)*dx);
        wall->z[0]=wall->z[1]=wall->lz=rz-(int)(cos(r)*dz);
        wall->z[2]=wall->rz=rz-(int)(cos(r2)*dz);
        
        wall->y[1]=wall->by=ty+(int)(sin(r)*dy);
        wall->y[0]=wall->y[2]=wall->ty=ty+(int)(sin(r2)*dy);
        
        draw->gx[0]=draw->gx[1]=(x_txtoff+x_txtfact)-(float)(cos(r)*x_txtfact);
        draw->gx[2]=(x_txtoff+x_txtfact)-(float)(cos(r2)*x_txtfact);
        draw->gy[1]=y_txtoff+(float)(sin(r)*y_txtfact);
        draw->gy[0]=draw->gy[2]=y_txtoff+(float)(sin(r2)*y_txtfact);

        wall->ptsz=draw->ptsz=3;
        curve_seg->clip=wc_none;

        r=r2;
    }
}

/* =======================================================

      Wall Clip Curve Mainline
      
======================================================= */

void map_prepare_create_wall_clip(map_type *map,segment_type *seg)
{
    int					lx,rx,lz,rz,ty,by,my;
	float				x_txtoff,x_txtfact,y_txtoff,y_txtfact;
    wall_segment_data	*wall;

    wall=&seg->data.wall;

    lx=wall->lx;
    rx=wall->rx;
    lz=wall->lz;
    rz=wall->rz;
    ty=wall->ty;
    by=wall->by;
    
    x_txtoff=seg->x_txtoff;
    x_txtfact=seg->x_txtfact;
    y_txtoff=seg->y_txtoff;
    y_txtfact=seg->y_txtfact;

        // create new clipped segments

    switch (seg->clip) {
        case wc_top_curve:
            map_prepare_create_wall_top_curve(map,seg,lx,lz,rx,rz,ty,by,x_txtoff,x_txtfact,y_txtoff,y_txtfact);
            break;
        case wc_top_arch:
            map_prepare_create_wall_top_arch(map,seg,lx,lz,rx,rz,ty,by,x_txtoff,x_txtfact,y_txtoff,y_txtfact);
            break;
        case wc_bottom_curve:
            map_prepare_create_wall_bottom_curve(map,seg,lx,lz,rx,rz,ty,by,x_txtoff,x_txtfact,y_txtoff,y_txtfact);
            break;
        case wc_bottom_arch:
            map_prepare_create_wall_bottom_arch(map,seg,lx,lz,rx,rz,ty,by,x_txtoff,x_txtfact,y_txtoff,y_txtfact);
            break;
        case wc_top_curve_arch:
            my=(ty+by)/2;
            y_txtfact=y_txtfact/2;
            map_prepare_create_wall_top_curve(map,seg,lx,lz,rx,rz,ty,my,x_txtoff,x_txtfact,y_txtoff,y_txtfact);
            map_prepare_create_wall_top_arch(map,seg,lx,lz,rx,rz,my,by,x_txtoff,x_txtfact,(y_txtoff+y_txtfact),y_txtfact);
            break;
        case wc_bottom_curve_arch:
            my=(ty+by)/2;
            y_txtfact=y_txtfact/2;
            map_prepare_create_wall_bottom_arch(map,seg,lx,lz,rx,rz,ty,my,x_txtoff,x_txtfact,(y_txtoff+y_txtfact),-y_txtfact);
            map_prepare_create_wall_bottom_curve(map,seg,lx,lz,rx,rz,my,by,x_txtoff,x_txtfact,y_txtoff,y_txtfact);
            break;
    }
}

