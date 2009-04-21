/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Transform Routines

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
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Model Boxes
      
======================================================= */

void model_get_size(model_type *model,int *x,int *y,int *z)
{
    *x=model->view_box.size.x;
    *y=model->view_box.size.y;
    *z=model->view_box.size.z;
}

/* =======================================================

      Model Extents
      
======================================================= */

void model_get_vertex_extent(model_type *model,int mesh_idx,int *p_minx,int *p_maxx,int *p_minz,int *p_maxz,int *p_miny,int *p_maxy)
{
	int					n,nvertex,
						minx,maxx,minz,maxz,miny,maxy;
	model_vertex_type	*vertex;
	
	nvertex=model->meshes[mesh_idx].nvertex;
	vertex=model->meshes[mesh_idx].vertexes;

	if (nvertex==0) {
		*p_minx=*p_maxx=*p_minz=*p_maxz=*p_miny=*p_maxy=0;
		return;
	}
	
	minx=maxx=vertex->pnt.x;
	miny=maxy=vertex->pnt.y;
	minz=maxz=vertex->pnt.z;
	
	for (n=1;n<nvertex;n++) {
		if (vertex->pnt.x<minx) minx=vertex->pnt.x;
		if (vertex->pnt.x>maxx) maxx=vertex->pnt.x;
		if (vertex->pnt.z<minz) minz=vertex->pnt.z;
		if (vertex->pnt.z>maxz) maxz=vertex->pnt.z;
		if (vertex->pnt.y<miny) miny=vertex->pnt.y;
		if (vertex->pnt.y>maxy) maxy=vertex->pnt.y;
		
		vertex++;
	}
    
    *p_minx=minx;
    *p_maxx=maxx;
    *p_minz=minz;
    *p_maxz=maxz;
    *p_miny=miny;
    *p_maxy=maxy;
}

void model_get_vertex_extent_all(model_type *model,int *p_minx,int *p_maxx,int *p_minz,int *p_maxz,int *p_miny,int *p_maxy)
{
	int					n,minx,maxx,minz,maxz,miny,maxy,
						minx2,maxx2,minz2,maxz2,miny2,maxy2;
	
	model_get_vertex_extent(model,0,&minx,&maxx,&minz,&maxz,&miny,&maxy);
	
	for (n=1;n<model->nmesh;n++) {
		model_get_vertex_extent(model,n,&minx2,&maxx2,&minz2,&maxz2,&miny2,&maxy2);
		if (minx2<minx) minx=minx2;
		if (maxx2>maxx) maxx=maxx2;
		if (minz2<minz) minz=minz2;
		if (maxz2>maxz) maxz=maxz2;
		if (miny2<miny) miny=miny2;
		if (maxy2>maxy) maxy=maxy2;
	}
	
	*p_minx=minx;
	*p_maxx=maxx;
	*p_minz=minz;
	*p_maxz=maxz;
	*p_miny=miny;
	*p_maxy=maxy;
}

/* =======================================================

      Model Complex Bound Box
      
======================================================= */

void model_get_view_complex_bounding_box(model_type *model,d3pnt *pnt,d3ang *ang,int *px,int *py,int *pz)
{
    int				n,sz;
	float			fx,fy,fz;
	matrix_type		rot_x_mat,rot_z_mat,rot_y_mat;
	
    sz=model->view_box.size.x>>1;
    px[0]=px[3]=px[4]=px[7]=-sz;
    px[1]=px[2]=px[5]=px[6]=sz;
    
    py[4]=py[5]=py[6]=py[7]=0;
    py[0]=py[1]=py[2]=py[3]=-model->view_box.size.y;
    
    sz=model->view_box.size.z>>1;
    pz[0]=pz[1]=pz[4]=pz[5]=-sz;
    pz[2]=pz[3]=pz[6]=pz[7]=sz;

	matrix_rotate_x(&rot_x_mat,ang->x);
	matrix_rotate_z(&rot_z_mat,ang->z);
	matrix_rotate_y(&rot_y_mat,ang->y);

	for (n=0;n!=8;n++) {
		fx=(float)px[n];
		fy=(float)py[n];
		fz=(float)pz[n];
		matrix_vertex_multiply(&rot_x_mat,&fx,&fy,&fz);
		matrix_vertex_multiply(&rot_z_mat,&fx,&fy,&fz);
		matrix_vertex_multiply(&rot_y_mat,&fx,&fy,&fz);
		px[n]=((int)fx)+pnt->x;
		py[n]=((int)fy)+pnt->y;
		pz[n]=((int)fz)+pnt->z;
	}
}

/* =======================================================

      Calculate Model Boxes
      
======================================================= */

void model_recalc_boxes(model_type *model)
{
	int					minx,maxx,minz,maxz,miny,maxy;

	model_get_vertex_extent_all(model,&minx,&maxx,&minz,&maxz,&miny,&maxy);
	
	model->view_box.size.x=abs(maxx-minx);
	model->view_box.size.z=abs(maxz-minz);
	model->view_box.size.y=abs(maxy-miny);
    model->view_box.offset.x=model->view_box.offset.z=model->view_box.offset.y=0;
}

