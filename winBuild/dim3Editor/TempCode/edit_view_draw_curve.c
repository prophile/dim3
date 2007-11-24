/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Edit View Curve Drawing Routines

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
	#include "dim3editor.h"
#endif

extern map_type				map;

extern void edit_view_draw_segment_poly(int ptsz,int *x,int *y,int *z,float *gx,float *gy,int txt,int txt_ang,float darken,float alpha);

/* =======================================================

      FC Forward and Backward Curves
      
======================================================= */

void edit_view_draw_frwd_4_curve(int *x,int *z,int *y,float *gx,float *gy,int fill,int txt_ang,float r,float g,float b,float dark_factor,float alpha)
{
	int				i,px[4],pz[4],py[4];
	float			pgx[4],pgy[4],dx,dy,dz,dgx,dgy;
    double			rd,rd2;
    
    rd=0;
    
    for (i=0;i!=18;i++) {
        rd2=rd+(ANG_to_RAD*5);
        
        dx=(float)(x[0]-x[1]);
        dy=(float)(y[0]-y[1]);
        dz=(float)(z[0]-z[1]);
        dgx=gx[0]-gx[1];
        dgy=gy[0]-gy[1];
        
		px[0]=x[1]+(int)(cos(rd)*dx);
		py[0]=y[0]+(int)(-sin(rd)*dy);
        pz[0]=z[1]+(int)(cos(rd)*dz);
        pgx[0]=gx[0]-((dgx*(float)i)/18);
        pgy[0]=gy[1]+((dgy*(float)i)/18);
        
		px[1]=x[1]+(int)(cos(rd2)*dx);
		py[1]=y[0]+(int)(-sin(rd2)*dy);
        pz[1]=z[1]+(int)(cos(rd2)*dz);
        pgx[1]=gx[0]-((dgx*(float)(i+1))/18);
        pgy[1]=gy[1]+((dgy*(float)(i+1))/18);
        
        dx=(float)(x[3]-x[2]);
        dy=(float)(y[3]-y[2]);
        dz=(float)(z[3]-z[2]);
        dgx=gx[3]-gx[2];
        dgy=gy[3]-gy[2];
        
		px[3]=x[2]+(int)(cos(rd)*dx);
		py[3]=y[3]+(int)(-sin(rd)*dy);
        pz[3]=z[2]+(int)(cos(rd)*dz);
        pgx[3]=gx[3]-((dgx*(float)i)/18);
        pgy[3]=gy[2]+((dgy*(float)i)/18);
        
		px[2]=x[2]+(int)(cos(rd2)*dx);
		py[2]=y[3]+(int)(-sin(rd2)*dy);
        pz[2]=z[2]+(int)(cos(rd2)*dz);
        pgx[2]=gx[3]-((dgx*(float)(i+1))/18);
        pgy[2]=gy[2]+((dgy*(float)(i+1))/18);

		edit_view_draw_segment_poly(4,px,py,pz,pgx,pgy,fill,txt_ang,dark_factor,alpha);
        
        rd=rd2;
    }
}

void edit_view_draw_frwd_3_curve(int *x,int *z,int *y,float *gx,float *gy,int fill,int txt_ang,float r,float g,float b,float dark_factor,float alpha)
{
	int				i,px[4],pz[4],py[4];
	float			pgx[4],pgy[4],dx,dy,dz,dgx,dgy;
    double			rd,rd2;
    
    rd=0;
    
    for (i=0;i!=18;i++) {
        rd2=rd+(ANG_to_RAD*5);
        
        dx=(float)(x[0]-x[1]);
        dy=(float)(y[0]-y[1]);
        dz=(float)(z[0]-z[1]);
        dgx=gx[0]-gx[1];
        dgy=gy[0]-gy[1];
        
		px[0]=x[1]+(int)(cos(rd)*dx);
		py[0]=y[0]+(int)(-sin(rd)*dy);
        pz[0]=z[1]+(int)(cos(rd)*dz);
        pgx[0]=gx[0]-((dgx*(float)i)/18);
        pgy[0]=gy[1]+((dgy*(float)i)/18);
        
		px[1]=x[1]+(int)(cos(rd2)*dx);
		py[1]=y[0]+(int)(-sin(rd2)*dy);
        pz[1]=z[1]+(int)(cos(rd2)*dz);
        pgx[1]=gx[0]-((dgx*(float)(i+1))/18);
        pgy[1]=gy[1]+((dgy*(float)(i+1))/18);
        
        dx=(float)(x[2]-x[1]);
        dy=(float)(y[2]-y[1]);
        dz=(float)(z[2]-z[1]);
        dgx=gx[2]-gx[1];
        dgy=gy[2]-gy[1];
        
		px[3]=x[1]+(int)(cos(rd)*dx);
		py[3]=y[2]+(int)(-sin(rd)*dy);
        pz[3]=z[1]+(int)(cos(rd)*dz);
        pgx[3]=gx[2]-((dgx*(float)i)/18);
        pgy[3]=gy[1]+((dgy*(float)i)/18);
        
		px[2]=x[1]+(int)(cos(rd2)*dx);
		py[2]=y[2]+(int)(-sin(rd2)*dy);
        pz[2]=z[1]+(int)(cos(rd2)*dz);
        pgx[2]=gx[2]-((dgx*(float)(i+1))/18);
        pgy[2]=gy[1]+((dgy*(float)(i+1))/18);
        
		edit_view_draw_segment_poly(4,px,py,pz,pgx,pgy,fill,txt_ang,dark_factor,alpha);
        
        rd=rd2;
    }
}

void edit_view_draw_bkwd_4_curve(int *x,int *z,int *y,float *gx,float *gy,int fill,int txt_ang,float r,float g,float b,float dark_factor,float alpha)
{
	int				i,px[4],pz[4],py[4];
	float			pgx[4],pgy[4],dx,dy,dz,dgx,dgy;
    double			rd,rd2;
    
    rd=0;
    
    for (i=0;i!=18;i++) {
        rd2=rd+(ANG_to_RAD*5);
        
        dx=(float)(x[0]-x[1]);
        dy=(float)(y[0]-y[1]);
        dz=(float)(z[0]-z[1]);
        dgx=gx[0]-gx[1];
        dgy=gy[0]-gy[1];
        
		px[0]=x[0]-(int)(cos(rd)*dx);
		py[0]=y[1]-(int)(-sin(rd)*dy);
        pz[0]=z[0]-(int)(cos(rd)*dz);
        pgx[0]=gx[1]+((dgx*(float)i)/18);
        pgy[0]=gy[0]+((dgy*(float)i)/18);
        
		px[1]=x[0]-(int)(cos(rd2)*dx);
		py[1]=y[1]-(int)(-sin(rd2)*dy);
        pz[1]=z[0]-(int)(cos(rd2)*dz);
        pgx[1]=gx[1]+((dgx*(float)(i+1))/18);
        pgy[1]=gy[0]+((dgy*(float)(i+1))/18);
        
        dx=(float)(x[3]-x[2]);
        dy=(float)(y[3]-y[2]);
        dz=(float)(z[3]-z[2]);
        dgx=gx[3]-gx[2];
        dgy=gy[3]-gy[2];
        
		px[3]=x[3]-(int)(cos(rd)*dx);
		py[3]=y[2]-(int)(-sin(rd)*dy);
        pz[3]=z[3]-(int)(cos(rd)*dz);
        pgx[3]=gx[2]+((dgx*(float)i)/18);
        pgy[3]=gy[3]+((dgy*(float)i)/18);
        
		px[2]=x[3]-(int)(cos(rd2)*dx);
		py[2]=y[2]-(int)(-sin(rd2)*dy);
        pz[2]=z[3]-(int)(cos(rd2)*dz);
        pgx[2]=gx[2]+((dgx*(float)(i+1))/18);
        pgy[2]=gy[3]+((dgy*(float)(i+1))/18);

		edit_view_draw_segment_poly(4,px,py,pz,pgx,pgy,fill,txt_ang,dark_factor,alpha);
        
        rd=rd2;
    }
}

void edit_view_draw_bkwd_3_curve(int *x,int *z,int *y,float *gx,float *gy,int fill,int txt_ang,float r,float g,float b,float dark_factor,float alpha)
{
	int				i,px[4],pz[4],py[4];
	float			pgx[4],pgy[4],dx,dy,dz,dgx,dgy;
    double			rd,rd2;
    
    rd=0;
    
    for (i=0;i!=18;i++) {
        rd2=rd+(ANG_to_RAD*5);
        
        dx=(float)(x[0]-x[1]);
        dy=(float)(y[0]-y[1]);
        dz=(float)(z[0]-z[1]);
        dgx=gx[0]-gx[1];
        dgy=gy[0]-gy[1];
        
		px[0]=x[0]-(int)(cos(rd)*dx);
		py[0]=y[1]-(int)(-sin(rd)*dy);
        pz[0]=z[1]-(int)(cos(rd)*dz);
        pgx[0]=gx[1]+((dgx*(float)i)/18);
        pgy[0]=gy[1]+((dgy*(float)i)/18);
        
		px[1]=x[0]-(int)(cos(rd2)*dx);
		py[1]=y[1]-(int)(-sin(rd2)*dy);
        pz[1]=z[1]-(int)(cos(rd2)*dz);
        pgx[1]=gx[1]+((dgx*(float)(i+1))/18);
        pgy[1]=gy[1]+((dgy*(float)(i+1))/18);
        
        dx=(float)(x[2]-x[1]);
        dy=(float)(y[2]-y[1]);
        dz=(float)(z[2]-z[1]);
        dgx=gx[2]-gx[1];
        dgy=gy[2]-gy[1];
        
		px[3]=x[2]-(int)(cos(rd)*dx);
		py[3]=y[1]-(int)(-sin(rd)*dy);
        pz[3]=z[1]-(int)(cos(rd)*dz);
        pgx[3]=gx[1]+((dgx*(float)i)/18);
        pgy[3]=gy[1]+((dgy*(float)i)/18);
        
		px[2]=x[2]-(int)(cos(rd2)*dx);
		py[2]=y[1]-(int)(-sin(rd2)*dy);
        pz[2]=z[1]-(int)(cos(rd2)*dz);
        pgx[2]=gx[1]+((dgx*(float)(i+1))/18);
        pgy[2]=gy[1]+((dgy*(float)(i+1))/18);

		edit_view_draw_segment_poly(4,px,py,pz,pgx,pgy,fill,txt_ang,dark_factor,alpha);
        
        rd=rd2;
    }
}

/* =======================================================

      FC Curve Mainline
      
======================================================= */

void edit_view_draw_fc_curve(int ptsz,int *x,int *z,int *y,float *gx,float *gy,int curve_type,int fill,int txt_ang,float r,float g,float b,float dark_factor,float alpha)
{
    switch (curve_type) {
        case cv_forward:
            if (ptsz==3) {
                edit_view_draw_frwd_3_curve(x,z,y,gx,gy,fill,txt_ang,r,g,b,dark_factor,alpha);
                break;
            }
            edit_view_draw_frwd_4_curve(x,z,y,gx,gy,fill,txt_ang,r,g,b,dark_factor,alpha);
            break;
        case cv_backward:
            if (ptsz==3) {
                edit_view_draw_bkwd_3_curve(x,z,y,gx,gy,fill,txt_ang,r,g,b,dark_factor,alpha);
                break;
            }
            edit_view_draw_bkwd_4_curve(x,z,y,gx,gy,fill,txt_ang,r,g,b,dark_factor,alpha);
            break;
    }
}

/* =======================================================

      Wall Forward and Backward Curves
      
======================================================= */

void edit_view_draw_frwd_wall_curve(int lx,int lz,int rx,int rz,int ty,int by,float x_txtoff,float x_txtfact,float y_txtoff,float y_txtfact,int fill,int txt_ang,float dark_factor,float alpha)
{
	int				i,x[4],z[4],y[4];
	float			gx[4],gy[4],dx,dz;
    double			r,r2;
    
    x_txtoff=x_txtoff+x_txtfact;
    x_txtfact=x_txtfact/18;
    
    r=0;
    
    for (i=0;i!=18;i++) {
        r2=r+(ANG_to_RAD*5);
        
        dx=(float)(rx-lx);
        dz=(float)(rz-lz);
        
		x[0]=x[3]=lx+(int)(cos(r)*dx);
		z[0]=z[3]=rz-(int)(sin(r)*dz);
        gx[0]=gx[3]=x_txtoff-(x_txtfact*(float)i);
        
		x[1]=x[2]=lx+(int)(cos(r2)*dx);
		z[1]=z[2]=rz-(int)(sin(r2)*dz);
        gx[1]=gx[2]=x_txtoff-(x_txtfact*(float)(i+1));
        
        y[0]=y[1]=ty;
        y[2]=y[3]=by;
        gy[0]=gy[1]=y_txtoff;
        gy[2]=gy[3]=y_txtoff+y_txtfact;

		edit_view_draw_segment_poly(4,x,y,z,gx,gy,fill,txt_ang,dark_factor,alpha);
       
        r=r2;
    }
}

void edit_view_draw_bkwd_wall_curve(int lx,int lz,int rx,int rz,int ty,int by,float x_txtoff,float x_txtfact,float y_txtoff,float y_txtfact,int fill,int txt_ang,float dark_factor,float alpha)
{
	int				i,x[4],z[4],y[4];
	float			gx[4],gy[4],dx,dz;
    double			r,r2;
    
    x_txtfact=x_txtfact/18;
    
    r=0;
    
    for (i=0;i!=18;i++) {
        r2=r+(ANG_to_RAD*5);
        
        dx=(float)(rx-lx);
        dz=(float)(rz-lz);
        
		x[0]=x[3]=rx-(int)(cos(r)*dx);
		z[0]=z[3]=lz+(int)(sin(r)*dz);
        gx[0]=gx[3]=x_txtoff+(x_txtfact*(float)i);
        
		x[1]=x[2]=rx-(int)(cos(r2)*dx);
		z[1]=z[2]=lz+(int)(sin(r2)*dz);
        gx[1]=gx[2]=x_txtoff+(x_txtfact*(float)(i+1));
        
        y[0]=y[1]=ty;
        y[2]=y[3]=by;
        gy[0]=gy[1]=y_txtoff;
        gy[2]=gy[3]=y_txtoff+y_txtfact;

		edit_view_draw_segment_poly(4,x,y,z,gx,gy,fill,txt_ang,dark_factor,alpha);
       
        r=r2;
    }
}

/* =======================================================

      Wall Curve Mainline
      
======================================================= */

void edit_view_draw_wall_curve(segment_type *seg,int xadd,int zadd,int fill,int txt_ang,float dark_factor,float alpha)
{
    int					lx,rx,lz,rz,ty,by;
    float				x_txtoff,y_txtoff,x_txtfact,y_txtfact;
    wall_segment_data	*wall;
    
    wall=&seg->data.wall;
    
    lx=(wall->lx*map_enlarge)+xadd;
    rx=(wall->rx*map_enlarge)+xadd;
    lz=(wall->lz*map_enlarge)+zadd;
    rz=(wall->rz*map_enlarge)+zadd;
    ty=wall->ty*map_enlarge;
    by=(wall->by+1)*map_enlarge;
    
    x_txtoff=seg->x_txtoff;
    x_txtfact=seg->x_txtfact;
    y_txtoff=seg->y_txtoff;
    y_txtfact=seg->y_txtfact;

    switch (seg->curve) {
        case cv_forward:
            edit_view_draw_frwd_wall_curve(lx,lz,rx,rz,ty,by,x_txtoff,x_txtfact,y_txtoff,y_txtfact,fill,txt_ang,dark_factor,alpha);
            break;
        case cv_backward:
            edit_view_draw_bkwd_wall_curve(lx,lz,rx,rz,ty,by,x_txtoff,x_txtfact,y_txtoff,y_txtfact,fill,txt_ang,dark_factor,alpha);
            break;
    }
}

/* =======================================================

      Top and Bottom Curved Wall Clips
      
======================================================= */

void edit_view_draw_wall_top_curve(int lx,int lz,int rx,int rz,int ty,int by,float x_txtoff,float x_txtfact,float y_txtoff,float y_txtfact,int fill,int txt_ang,float dark_factor,float alpha)
{
    int				i,x[4],z[4],y[4];
    float			gx[4],gy[4],dx,dz,dy;
    double			r,r2;
    
    r=0;
    
    dx=(float)(rx-lx);
    dz=(float)(rz-lz);
    dy=(float)(by-ty);
    
    for ((i=0);(i!=18);i++) {
        r2=r+(ANG_to_RAD*5);
        
		x[0]=x[1]=lx+(int)(cos(r)*dx);
		x[2]=x[3]=lx+(int)(cos(r2)*dx);
		z[0]=z[1]=lz+(int)(cos(r)*dz);
		z[2]=z[3]=lz+(int)(cos(r2)*dz);
		y[0]=y[3]=by;
		y[1]=y[2]=by-(int)(sin(r)*dy);
			
		gx[0]=gx[1]=x_txtoff+(float)(cos(r)*x_txtfact);
		gx[2]=gx[3]=x_txtoff+(float)(cos(r2)*x_txtfact);
		gy[0]=gy[3]=y_txtoff+y_txtfact;
		gy[1]=gy[2]=(y_txtoff+y_txtfact)-(float)(sin(r)*y_txtfact);

		edit_view_draw_segment_poly(4,x,y,z,gx,gy,fill,txt_ang,dark_factor,alpha);

		x[0]=x[1]=lx+(int)(cos(r2)*dx);
		x[2]=lx+(int)(cos(r)*dx);
		z[0]=z[1]=lz+(int)(cos(r2)*dz);
		z[2]=lz+(int)(cos(r)*dz);
		
		y[1]=by-(int)(sin(r2)*dy);
		y[0]=y[2]=by-(int)(sin(r)*dy);
		
		gx[0]=gx[1]=x_txtoff+(float)(cos(r2)*x_txtfact);
		gx[2]=x_txtoff+(float)(cos(r)*x_txtfact);
		gy[1]=(y_txtoff+y_txtfact)-(float)(sin(r2)*y_txtfact);
		gy[0]=gy[2]=(y_txtoff+y_txtfact)-(float)(sin(r)*y_txtfact);
	
		edit_view_draw_segment_poly(3,x,y,z,gx,gy,fill,txt_ang,dark_factor,alpha);
       
        r=r2;
    }
}

void edit_view_draw_wall_top_arch(int lx,int lz,int rx,int rz,int ty,int by,float x_txtoff,float x_txtfact,float y_txtoff,float y_txtfact,int fill,int txt_ang,float dark_factor,float alpha)
{
    int				i,x[4],z[4],y[4];
    float			gx[4],gy[4],dx,dz,dy;
    double			r,r2;
    
    r=0;
    
    dx=(float)(rx-lx);
    dz=(float)(rz-lz);
    dy=(float)(by-ty);
    
    for (i=0;i!=18;i++) {
		r2=r+(ANG_to_RAD*5);
		
		x[0]=x[1]=lx+(int)(cos(r)*dx);
		x[2]=x[3]=lx+(int)(cos(r2)*dx);
		z[0]=z[1]=lz+(int)(cos(r)*dz);
		z[2]=z[3]=lz+(int)(cos(r2)*dz);
		y[0]=y[3]=by-(int)(sin(r2)*dy);
		y[1]=y[2]=ty;
			
		gx[0]=gx[1]=x_txtoff+(float)(cos(r)*x_txtfact);
		gx[2]=gx[3]=x_txtoff+(float)(cos(r2)*x_txtfact);
		gy[0]=gy[3]=(y_txtoff+y_txtfact)-(float)(sin(r2)*y_txtfact);
		gy[1]=gy[2]=y_txtoff;

		edit_view_draw_segment_poly(4,x,y,z,gx,gy,fill,txt_ang,dark_factor,alpha);

		x[0]=x[1]=lx+(int)(cos(r)*dx);
		x[2]=lx+(int)(cos(r2)*dx);
		z[0]=z[1]=lz+(int)(cos(r)*dz);
		z[2]=lz+(int)(cos(r2)*dz);
		
		y[1]=by-(int)(sin(r)*dy);
		y[0]=y[2]=by-(int)(sin(r2)*dy);
		
		gx[0]=gx[1]=x_txtoff+(float)(cos(r)*x_txtfact);
		gx[2]=x_txtoff+(float)(cos(r2)*x_txtfact);
		gy[1]=(y_txtoff+y_txtfact)-(float)(sin(r)*y_txtfact);
		gy[0]=gy[2]=(y_txtoff+y_txtfact)-(float)(sin(r2)*y_txtfact);

		edit_view_draw_segment_poly(3,x,y,z,gx,gy,fill,txt_ang,dark_factor,alpha);

        r=r2;
    }
}

void edit_view_draw_wall_bottom_curve(int lx,int lz,int rx,int rz,int ty,int by,float x_txtoff,float x_txtfact,float y_txtoff,float y_txtfact,int fill,int txt_ang,float dark_factor,float alpha)
{
    int				i,x[4],z[4],y[4];
    float			gx[4],gy[4],dx,dz,dy;
    double			r,r2;
    
    r=0;
    
    dx=(float)(rx-lx);
    dz=(float)(rz-lz);
    dy=(float)(by-ty);
    
    for (i=0;i!=18;i++) {
        r2=r+(ANG_to_RAD*5);
        
		x[0]=x[1]=rx-(int)(cos(r)*dx);
		x[2]=x[3]=rx-(int)(cos(r2)*dx);
		z[0]=z[1]=rz-(int)(cos(r)*dz);
		z[2]=z[3]=rz-(int)(cos(r2)*dz);
		y[0]=y[3]=ty;
		y[1]=y[2]=ty+(int)(sin(r)*dy);
			
		gx[0]=gx[1]=x_txtoff-(float)(cos(r)*x_txtfact);
		gx[2]=gx[3]=x_txtoff-(float)(cos(r2)*x_txtfact);
		gy[0]=gy[3]=y_txtoff-y_txtfact;
		gy[1]=gy[2]=(y_txtoff-y_txtfact)+(float)(sin(r)*y_txtfact);

		edit_view_draw_segment_poly(4,x,y,z,gx,gy,fill,txt_ang,dark_factor,alpha);

		x[0]=x[1]=rx-(int)(cos(r2)*dx);
		x[2]=rx-(int)(cos(r)*dx);
		z[0]=z[1]=rz-(int)(cos(r2)*dz);
		z[2]=rz-(int)(cos(r)*dz);
		
		y[1]=ty+(int)(sin(r2)*dy);
		y[0]=y[2]=ty+(int)(sin(r)*dy);
		
		gx[0]=gx[1]=x_txtoff-(float)(cos(r2)*x_txtfact);
		gx[2]=x_txtoff-(float)(cos(r)*x_txtfact);
		gy[1]=(y_txtoff-y_txtfact)+(float)(sin(r2)*y_txtfact);
		gy[0]=gy[2]=(y_txtoff-y_txtfact)+(float)(sin(r)*y_txtfact);
	
		edit_view_draw_segment_poly(3,x,y,z,gx,gy,fill,txt_ang,dark_factor,alpha);
         
        r=r2;
    }
}

void edit_view_draw_wall_bottom_arch(int lx,int lz,int rx,int rz,int ty,int by,float x_txtoff,float x_txtfact,float y_txtoff,float y_txtfact,int fill,int txt_ang,float dark_factor,float alpha)
{
    int				i,x[4],z[4],y[4];
    float			gx[4],gy[4],dx,dz,dy;
    double			r,r2;
    
    r=0;
    
    dx=(float)(rx-lx);
    dz=(float)(rz-lz);
    dy=(float)(by-ty);
    
    for (i=0;i!=18;i++) {
        r2=r+(ANG_to_RAD*5);
	
		x[0]=x[1]=rx-(int)(cos(r)*dx);
		x[2]=x[3]=rx-(int)(cos(r2)*dx);
		z[0]=z[1]=rz-(int)(cos(r)*dz);
		z[2]=z[3]=rz-(int)(cos(r2)*dz);
		y[0]=y[3]=ty+(int)(sin(r2)*dy);
		y[1]=y[2]=by;
			
		gx[0]=gx[1]=(x_txtoff+x_txtfact)-(float)(cos(r)*x_txtfact);
		gx[2]=gx[3]=(x_txtoff+x_txtfact)-(float)(cos(r2)*x_txtfact);
		gy[0]=gy[3]=y_txtoff+(float)(sin(r2)*y_txtfact);
		gy[1]=gy[2]=y_txtoff+y_txtfact;

		edit_view_draw_segment_poly(4,x,y,z,gx,gy,fill,txt_ang,dark_factor,alpha);

		x[0]=x[1]=rx-(int)(cos(r)*dx);
		x[2]=rx-(int)(cos(r2)*dx);
		z[0]=z[1]=rz-(int)(cos(r)*dz);
		z[2]=rz-(int)(cos(r2)*dz);
		
		y[1]=ty+(int)(sin(r)*dy);
		y[0]=y[2]=ty+(int)(sin(r2)*dy);
		
		gx[0]=gx[1]=(x_txtoff+x_txtfact)-(float)(cos(r)*x_txtfact);
		gx[2]=(x_txtoff+x_txtfact)-(float)(cos(r2)*x_txtfact);
		gy[1]=y_txtoff+(float)(sin(r)*y_txtfact);
		gy[0]=gy[2]=y_txtoff+(float)(sin(r2)*y_txtfact);

		edit_view_draw_segment_poly(3,x,y,z,gx,gy,fill,txt_ang,dark_factor,alpha);

        r=r2;
    }
}

/* =======================================================

      Curve Wall Clips Mainline
      
======================================================= */

void edit_view_draw_wall_clip_curve(segment_type *seg,int xadd,int zadd,int fill,int txt_ang,float dark_factor,float alpha)
{
    int					lx,rx,lz,rz,ty,by,my;
    float				x_txtoff,y_txtoff,x_txtfact,y_txtfact;
    wall_segment_data	*wall;
    
    wall=&seg->data.wall;
    
    lx=(wall->lx*map_enlarge)+xadd;
    rx=(wall->rx*map_enlarge)+xadd;
    lz=(wall->lz*map_enlarge)+zadd;
    rz=(wall->rz*map_enlarge)+zadd;
    ty=wall->ty*map_enlarge;
    by=(wall->by+1)*map_enlarge;
    
    x_txtoff=seg->x_txtoff;
    x_txtfact=seg->x_txtfact;
    y_txtoff=seg->y_txtoff;
    y_txtfact=seg->y_txtfact;

    switch (seg->clip) {
        case wc_top_curve:
            edit_view_draw_wall_top_curve(lx,lz,rx,rz,ty,by,x_txtoff,x_txtfact,y_txtoff,y_txtfact,fill,txt_ang,dark_factor,alpha);
            break;
        case wc_top_arch:
            edit_view_draw_wall_top_arch(lx,lz,rx,rz,ty,by,x_txtoff,x_txtfact,y_txtoff,y_txtfact,fill,txt_ang,dark_factor,alpha);
            break;
        case wc_bottom_curve:
            edit_view_draw_wall_bottom_curve(lx,lz,rx,rz,ty,by,x_txtoff,x_txtfact,y_txtoff,y_txtfact,fill,txt_ang,dark_factor,alpha);
            break;
        case wc_bottom_arch:
            edit_view_draw_wall_bottom_arch(lx,lz,rx,rz,ty,by,x_txtoff,x_txtfact,y_txtoff,y_txtfact,fill,txt_ang,dark_factor,alpha);
            break;
        case wc_top_curve_arch:
            my=(ty+by)/2;
            y_txtfact=y_txtfact/2;
            edit_view_draw_wall_top_curve(lx,lz,rx,rz,ty,my,x_txtoff,x_txtfact,y_txtoff,y_txtfact,fill,txt_ang,dark_factor,alpha);
			edit_view_draw_wall_top_arch(lx,lz,rx,rz,my,by,x_txtoff,x_txtfact,(y_txtoff+y_txtfact),y_txtfact,fill,txt_ang,dark_factor,alpha);
            break;
		case wc_bottom_curve_arch:
            my=(ty+by)/2;
            y_txtfact=y_txtfact/2;
			edit_view_draw_wall_bottom_arch(lx,lz,rx,rz,ty,my,x_txtoff,x_txtfact,(y_txtoff+y_txtfact),-y_txtfact,fill,txt_ang,dark_factor,alpha);
            edit_view_draw_wall_bottom_curve(lx,lz,rx,rz,my,by,x_txtoff,x_txtfact,y_txtoff,y_txtfact,fill,txt_ang,dark_factor,alpha);
            break; 
   }
}

