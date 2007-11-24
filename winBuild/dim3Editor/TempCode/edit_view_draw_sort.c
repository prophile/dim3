/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Edit View Draw Sorting Routines

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

short					edit_sort_seg_list[max_segment];
float					edit_sort_z_list[max_segment];

int						vport[4];
double					mod_matrix[16],proj_matrix[16];

extern int edit_view_poly_for_wall(segment_type *seg,int *x,int *y,int *z,float *gx,float *gy,int xadd,int zadd);
extern void edit_view_poly_for_fc(segment_type *seg,int *x,int *y,int *z,float *gx,float *gy,int xadd,int zadd);
extern void edit_view_poly_for_poly(segment_type *seg,int *x,int *y,int *z,float *gx,float *gy,int xadd,int zadd);
extern void edit_view_poly_for_ambient_wall(segment_type *seg,int *x,int *y,int *z,int xadd,int zadd);
extern void edit_view_poly_for_ambient_fc(segment_type *seg,int *x,int *y,int *z,float *gx,float *gy,int xadd,int zadd);

/* =======================================================

      Projection Utilities
      
======================================================= */

void gl_setup_project(void)
{
	glGetDoublev(GL_MODELVIEW_MATRIX,mod_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX,proj_matrix);
	glGetIntegerv(GL_VIEWPORT,(GLint*)vport);
}

bool gl_rotate_point_on_screen(int x,int y,int z)
{
	double			dx,dy,dz;
	
	dx=(double)x;
	dy=(double)y;
	dz=(double)z;
	
	return(((dx*mod_matrix[2])+(dy*mod_matrix[6])+(dz*mod_matrix[10])+mod_matrix[14])<0.0);
}

float gl_project_point_z(int x,int y,int z)
{
	double		dx,dy,dz;

	gluProject(x,y,z,mod_matrix,proj_matrix,(GLint*)vport,&dx,&dy,&dz);
	return((float)dz);
}

/* =======================================================

      Setup Segment Polygons
      
======================================================= */

void edit_view_draw_portal_segments_setup_sort(segment_type *seg)
{
	int				rn,xadd,zadd;
	float			gx[8],gy[8];

		// portal offset

	rn=seg->rn;

	xadd=map.portals[rn].x*map_enlarge;
	zadd=map.portals[rn].z*map_enlarge;

		// get points

	switch (seg->type) {
		
		case sg_wall:
			seg->data.wall.ptsz=edit_view_poly_for_wall(seg,seg->data.wall.x,seg->data.wall.y,seg->data.wall.z,gx,gy,xadd,zadd);
			break;
				
		case sg_floor:
		case sg_ceiling:
			edit_view_poly_for_fc(seg,seg->data.fc.x,seg->data.fc.y,seg->data.fc.z,gx,gy,xadd,zadd);
			break;

		case sg_poly:
			edit_view_poly_for_poly(seg,seg->data.fc.x,seg->data.fc.y,seg->data.fc.z,gx,gy,xadd,zadd);
			break;
					
		case sg_ambient_wall:
			seg->data.wall.ptsz=4;
			edit_view_poly_for_ambient_wall(seg,seg->data.ambient_wall.x,seg->data.ambient_wall.y,seg->data.ambient_wall.z,xadd,zadd);
			break;
			
		case sg_ambient_fc:
			edit_view_poly_for_ambient_fc(seg,seg->data.ambient_fc.x,seg->data.ambient_fc.y,seg->data.ambient_fc.z,gx,gy,xadd,zadd);
			break;
	}
}

/* =======================================================

      Segment Sorting
      
======================================================= */

float edit_view_draw_portal_segments_far_z(segment_type *seg,d3pnt *camera_pt)
{
	int				n,ptsz;
	float			dist,d;
	int				*x,*y,*z;

		// get points

	ptsz=0;

	switch (seg->type) {
		
		case sg_wall:
			ptsz=seg->data.wall.ptsz;
			x=seg->data.wall.x;
			y=seg->data.wall.y;
			z=seg->data.wall.z;
			break;
				
		case sg_floor:
		case sg_ceiling:
			ptsz=seg->data.fc.ptsz;
			x=seg->data.fc.x;
			y=seg->data.fc.y;
			z=seg->data.fc.z;
			break;
					
		case sg_ambient_wall:
			ptsz=4;
			x=seg->data.ambient_wall.x;
			y=seg->data.ambient_wall.y;
			z=seg->data.ambient_wall.z;
			break;
			
		case sg_ambient_fc:
			ptsz=seg->data.ambient_fc.ptsz;
			x=seg->data.ambient_fc.x;
			y=seg->data.ambient_fc.y;
			z=seg->data.ambient_fc.z;
			break;
	}

	if (ptsz==0) return(0.0f);

		// calculate the farest z
		// that is on screen

	dist=0.0f;

	for (n=0;n!=ptsz;n++) {
		x[n]-=camera_pt->x;
		y[n]-=camera_pt->y;
		z[n]=camera_pt->z-z[n];
		if (gl_rotate_point_on_screen(x[n],y[n],z[n])) {
			d=gl_project_point_z(x[n],y[n],z[n]);
			if (d>dist) dist=d;
		}
	}

	return(dist);
}

int edit_view_draw_portal_segments_sort(int cnt,short *seg_list,d3pnt *camera_pt)
{
	int				n,k,sort_cnt,seg_idx,sort_idx;
	float			dist;
	short			*sptr;
	segment_type	*seg;

		// setup the ptsz/x/y/z rendering in the
		// segment so we can use the same routines as engine

	gl_setup_project();

	sptr=seg_list;

	for (n=0;n!=cnt;n++) {
		edit_view_draw_portal_segments_setup_sort(&map.segments[*sptr++]);
	}

		// always put first one at top of list

	sptr=seg_list;

	seg_idx=*sptr++;
	seg=&map.segments[seg_idx];

	sort_cnt=1;
	edit_sort_seg_list[0]=seg_idx;
	edit_sort_z_list[0]=edit_view_draw_portal_segments_far_z(seg,camera_pt);

		// sort in other segments

	for (n=1;n!=cnt;n++) {
		seg_idx=*sptr++;

			// get segment farthest z factor

		seg=&map.segments[seg_idx];

		dist=edit_view_draw_portal_segments_far_z(seg,camera_pt);

			// find position in sort list

		sort_idx=sort_cnt;

		for (k=0;k!=sort_cnt;k++) {
			if (dist>edit_sort_z_list[k]) {
				sort_idx=k;
				break;
			}
		}

			// add to sort list

		if (sort_idx<sort_cnt) {
			memmove(&edit_sort_seg_list[sort_idx+1],&edit_sort_seg_list[sort_idx],((sort_cnt-sort_idx)*sizeof(short)));
			memmove(&edit_sort_z_list[sort_idx+1],&edit_sort_z_list[sort_idx],((sort_cnt-sort_idx)*sizeof(float)));
		}

		edit_sort_seg_list[sort_idx]=seg_idx;
		edit_sort_z_list[sort_idx]=dist;

		sort_cnt++;
	}

		// replace with sorted list

	memmove(seg_list,edit_sort_seg_list,(sort_cnt*sizeof(short)));

	return(sort_cnt);
}
