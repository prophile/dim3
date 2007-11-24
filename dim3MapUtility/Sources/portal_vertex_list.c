/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Portal Vertex Lists

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

/* =======================================================

      Add Vertex to Portal Vertex List
      
======================================================= */

int map_portal_add_single_vertex_list(portal_vertex_list_type *vl,int vl_cnt,int x,int y,int z,float gx,float gy,bool moveable,bool shiftable,int *idx)
{
	int							i,vl_start;
	float						fx,fy,fz;
	portal_vertex_list_type		*v;

	fx=(float)x;
	fy=(float)y;
	fz=(float)z;
	
		// moveable or shiftable segments have non-combined vertexes

	if ((moveable) || (shiftable)) {
		v=vl+vl_cnt;
		
		v->x=fx;
		v->y=fy;
		v->z=fz;
		v->gx=gx;
		v->gy=gy;
		
		v->flags=0;
		if (moveable) v->flags|=flag_pvl_moveable;
		if (shiftable) v->flags|=flag_pvl_shiftable;
		
		*idx=vl_cnt;
		
		return(vl_cnt+1);
	}	

		// check for combined vertexes
		// go backwards for better chance of early hit

	if (vl_cnt!=0) {
	
		vl_start=vl_cnt-1;
		v=vl+vl_start;
	
		for (i=vl_start;i!=0;i--) {
			if (v->flags==flag_pvl_none) {
				if ((v->x==fx) && (v->y==fy) && (v->z==fz) && (v->gx==gx) && (v->gy==gy)) {
					*idx=i;
					return(vl_cnt);
				}
			}
			v--;
		}
	}

		// need a new vertex
		
	v=vl+vl_cnt;
	
	v->x=fx;
	v->y=fy;
	v->z=fz;
	v->gx=gx;
	v->gy=gy;
	v->flags=flag_pvl_none;
	
	*idx=vl_cnt;
	
	return(vl_cnt+1);
}

int map_portal_add_single_light_vertex_list(portal_vertex_list_type *vl,int vl_cnt,int x,int y,int z,bool moveable,int *idx)
{
	int							i,vl_start;
	float						fx,fy,fz;
	portal_vertex_list_type		*v;

	fx=(float)x;
	fy=(float)y;
	fz=(float)z;
	
		// moveable segments have non-combined vertexes

	if (moveable) {
		v=vl+vl_cnt;
		
		v->x=fx;
		v->y=fy;
		v->z=fz;
		v->gx=0.0f;
		v->gy=0.0f;
		v->flags=flag_pvl_moveable;
		
		*idx=vl_cnt;
		
		return(vl_cnt+1);
	}	

		// check for combined vertexes
		// go backwards for better chance of early hit

	if (vl_cnt!=0) {
	
		vl_start=vl_cnt-1;
		v=vl+vl_start;
	
		for (i=vl_start;i!=0;i--) {
			if ((v->flags&flag_pvl_moveable)==0) {
				if ((v->x==fx) && (v->y==fy) && (v->z==fz)) {
					*idx=i;
					return(vl_cnt);
				}
			}
			v--;
		}
	}

		// need a new vertex
		
	v=vl+vl_cnt;
	
	v->x=fx;
	v->y=fy;
	v->z=fz;
	v->gx=0.0f;
	v->gy=0.0f;
	v->flags=flag_pvl_none;
	
	*idx=vl_cnt;
	
	return(vl_cnt+1);
}

/* =======================================================

      Build Portal Vertex List for Segment
      
======================================================= */

int map_portal_add_segment_single_vertex_list(portal_vertex_list_type *vl,int vl_cnt,segment_type *seg)
{
	int							n,ptsz;
	bool						moveable,shiftable;
	segment_draw_type			*draw;
	wall_segment_data			*wall;
	fc_segment_data				*fc;
	ambient_wall_segment_data	*ambient_wall;
	ambient_fc_segment_data		*ambient_fc;
	
	moveable=seg->moveable;
	shiftable=seg->shiftable;
	
	draw=&seg->draw;
		
	switch (seg->type) {
	
			// walls
		
		case sg_wall:
			wall=&seg->data.wall;
			ptsz=wall->ptsz;
			
			for (n=0;n!=ptsz;n++) {
				vl_cnt=map_portal_add_single_vertex_list(vl,vl_cnt,wall->x[n],wall->y[n],wall->z[n],draw->gx[n],draw->gy[n],moveable,shiftable,&draw->idx[n]);
			}

			draw->ptsz=ptsz;
			
			return(vl_cnt);
	
			// floors and ceilings
		
		case sg_floor:
		case sg_ceiling:
		case sg_poly:
			fc=&seg->data.fc;
			ptsz=fc->ptsz;
					
			for (n=0;n!=ptsz;n++) {
				vl_cnt=map_portal_add_single_vertex_list(vl,vl_cnt,fc->x[n],fc->y[n],fc->z[n],draw->gx[n],draw->gy[n],moveable,shiftable,&draw->idx[n]);
			}

			draw->ptsz=ptsz;
			
			return(vl_cnt);
			
			// liquids
			
		case sg_liquid:
			vl_cnt=map_liquid_add_vertex_list(vl,vl_cnt,seg);
			return(vl_cnt);
			
			// ambient walls
		
		case sg_ambient_wall:
			ambient_wall=&seg->data.ambient_wall;
	
			for (n=0;n!=4;n++) {
				vl_cnt=map_portal_add_single_vertex_list(vl,vl_cnt,ambient_wall->x[n],ambient_wall->y[n],ambient_wall->z[n],draw->gx[n],draw->gy[n],moveable,shiftable,&draw->idx[n]);
			}

			draw->ptsz=4;
			
			return(vl_cnt);
			
			// ambient fcs
			
		case sg_ambient_fc:
			ambient_fc=&seg->data.ambient_fc;
			ptsz=ambient_fc->ptsz;
					
			for (n=0;n!=ptsz;n++) {
				vl_cnt=map_portal_add_single_vertex_list(vl,vl_cnt,ambient_fc->x[n],ambient_fc->y[n],ambient_fc->z[n],draw->gx[n],draw->gy[n],moveable,shiftable,&draw->idx[n]);
			}

			draw->ptsz=ptsz;
			
			return(vl_cnt);
	
	}
	
	return(vl_cnt);
}

/* =======================================================

      Build Portal Vertex List for Light Tessel
      
======================================================= */

int map_portal_add_light_tessel_wall_vertex_list(portal_vertex_list_type *vl,int vl_cnt,int lx,int rx,int lz,int rz,int ty,int by,bool moveable,segment_light_type *light)
{
	int			n,y,xdist,ydist,zdist,walldist,walltot,ytot,ntrig;
	int			grid_x[light_tessel_grid_sz+1],
				grid_y[light_tessel_grid_sz+1],
				grid_z[light_tessel_grid_sz+1],
				idx[light_tessel_grid_sz+1][light_tessel_grid_sz+1];
	double		dx,dz;

		// find splitting size, total of 64 triangles, 8 grid spots (max)
		// find best splits for grid spots

	xdist=rx-lx;
	zdist=rz-lz;
	
	dx=(double)xdist;
	dz=(double)zdist;
	walldist=(int)sqrt((dx*dx)+(dz*dz));

	ydist=abs(by-ty);

	walltot=(walldist*light_tessel_grid_sz)/light_tessel_max_size;
	if (walltot>light_tessel_grid_sz) walltot=light_tessel_grid_sz;
	if (walltot<=0) walltot=1;

	ytot=(ydist*light_tessel_grid_sz)/light_tessel_max_size;
	if (ytot>light_tessel_grid_sz) ytot=light_tessel_grid_sz;
	if (ytot<=0) ytot=1;

		// create the grid overlay

	for (n=0;n<=walltot;n++) {
		grid_x[n]=lx+((xdist*n)/walltot);
		grid_z[n]=lz+((zdist*n)/walltot);
	}

	for (n=0;n<=ytot;n++) {
		grid_y[n]=ty+((ydist*n)/ytot);
	}
	

		// get vertexes for grid

	for (y=0;y<=ytot;y++) {
		for (n=0;n<=walltot;n++) {
			vl_cnt=map_portal_add_single_light_vertex_list(vl,vl_cnt,grid_x[n],grid_y[y],grid_z[n],moveable,&idx[n][y]);
		}
	}

		// tesselate grid into triangles

	ntrig=0;

	for (y=0;y!=ytot;y++) {
		
		for (n=0;n!=walltot;n++) {

			if (((y+n)&0x1)==0) {
				light->trig_vertex_idx[(ntrig*3)]=idx[n][y];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[n+1][y];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[n+1][y+1];
				ntrig++;
				
				light->trig_vertex_idx[(ntrig*3)]=idx[n][y];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[n][y+1];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[n+1][y+1];
				ntrig++;
			}
			else {
				light->trig_vertex_idx[(ntrig*3)]=idx[n+1][y];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[n][y];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[n][y+1];
				ntrig++;
				
				light->trig_vertex_idx[(ntrig*3)]=idx[n+1][y];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[n+1][y+1];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[n][y+1];
				ntrig++;
			}
		}
	}

	light->trig_count=ntrig;

	return(vl_cnt);
}

int map_portal_add_light_tessel_floor_ceiling_vertex_list(portal_vertex_list_type *vl,int vl_cnt,int ptsz,int *px,int *py,int *pz,bool moveable,segment_light_type *light)
{
	int			n,x,z,y,min_x,max_x,min_z,max_z,
				xdist,zdist,xtot,ztot,xskip,zskip,ntrig;
	int			grid_x[light_tessel_grid_sz+1],
				grid_z[light_tessel_grid_sz+1],
				idx[light_tessel_grid_sz+1][light_tessel_grid_sz+1];

		// find splitting size, total of 64 triangles, 8 grid spots (max)
		// find best splits for grid spots

	min_x=max_x=px[0];
	min_z=max_z=pz[0];

	for (n=1;n!=ptsz;n++) {
		if (px[n]<min_x) min_x=px[n];
		if (px[n]>max_x) max_x=px[n];
		if (pz[n]<min_z) min_z=pz[n];
		if (pz[n]>max_z) max_z=pz[n];
	}

	xdist=max_x-min_x;
	zdist=max_z-min_z;

	xtot=(xdist*light_tessel_grid_sz)/light_tessel_max_size;
	if (xtot>light_tessel_grid_sz) xtot=light_tessel_grid_sz;
	if (xtot<=0) xtot=1;

	ztot=(zdist*light_tessel_grid_sz)/light_tessel_max_size;
	if (ztot>light_tessel_grid_sz) ztot=light_tessel_grid_sz;
	if (ztot<=0) ztot=1;

	xskip=xdist/xtot;
	zskip=zdist/ztot;

		// create the grid overlay

	for (x=0;x<=xtot;x++) {
		if (x==xtot) {
			grid_x[x]=max_x;
		}
		else {
			grid_x[x]=min_x+(x*xskip);
		}
	}

	for (z=0;z<=ztot;z++) {
		if (z==ztot) {
			grid_z[z]=max_z;
		}
		else {
			grid_z[z]=min_z+(z*zskip);
		}
	}

		// get vertexes for grid

	for (z=0;z<=ztot;z++) {
		for (x=0;x<=xtot;x++) {
			y=polygon_find_y(ptsz,px,py,pz,grid_x[x],grid_z[z]);
			if (y==-1) y=polygon_infinite_find_y(ptsz,px,py,pz,grid_x[x],grid_z[z]);
			vl_cnt=map_portal_add_single_light_vertex_list(vl,vl_cnt,grid_x[x],y,grid_z[z],moveable,&idx[x][z]);
		}
	}

		// tesselate grid into triangles

	ntrig=0;

	for (z=0;z!=ztot;z++) {
		
		for (x=0;x!=xtot;x++) {
		
			if (((z+x)&0x1)==0) {
				light->trig_vertex_idx[(ntrig*3)]=idx[x][z];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[x+1][z];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[x+1][z+1];
				ntrig++;
				
				light->trig_vertex_idx[(ntrig*3)]=idx[x][z];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[x][z+1];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[x+1][z+1];
				ntrig++;
			}
			else {
				light->trig_vertex_idx[(ntrig*3)]=idx[x+1][z];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[x][z];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[x][z+1];
				ntrig++;
				
				light->trig_vertex_idx[(ntrig*3)]=idx[x+1][z];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[x+1][z+1];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[x][z+1];
				ntrig++;
			}
			
		}
	}

	light->trig_count=ntrig;

	return(vl_cnt);
}

int map_portal_add_light_simple_vertex_list(portal_vertex_list_type *vl,int vl_cnt,int ptsz,int *px,int *py,int *pz,bool moveable,segment_light_type *light)
{
	int			n,k,ntrig,tx[3],ty[3],tz[3];
	
	light->trig_count=0;

		// break up polygon into triangles
		// but no tesselation of triangles
		// this is used for simple lighting on ambients
		
	ntrig=ptsz-3;

	tx[0]=px[0];
	ty[0]=py[0];
	tz[0]=pz[0];
	
	for (n=0;n<=ntrig;n++) {
		tx[1]=px[n+1];
		ty[1]=py[n+1];
		tz[1]=pz[n+1];
		tx[2]=px[n+2];
		ty[2]=py[n+2];
		tz[2]=pz[n+2];

		for (k=0;k!=3;k++) {
			vl_cnt=map_portal_add_single_light_vertex_list(vl,vl_cnt,tx[k],ty[k],tz[k],moveable,&light->trig_vertex_idx[(n*3)+k]);
		}

		light->trig_count++;
	}
	
	return(vl_cnt);
}

int map_portal_add_light_single_vertex_list(portal_vertex_list_type *vl,int vl_cnt,bool high_quality_lighting,segment_type *seg)
{
	bool						moveable;
	segment_light_type			*light;
	wall_segment_data			*wall;
	fc_segment_data				*fc;
	ambient_wall_segment_data	*ambient_wall;
	ambient_fc_segment_data		*ambient_fc;
	
	moveable=seg->moveable;
	light=&seg->light;
		
	switch (seg->type) {
	
			// walls
		
		case sg_wall:
			wall=&seg->data.wall;
			if ((seg->simple_tessel) || (wall->ptsz==3) || (!high_quality_lighting)) return(map_portal_add_light_simple_vertex_list(vl,vl_cnt,wall->ptsz,wall->x,wall->y,wall->z,moveable,light));
			return(map_portal_add_light_tessel_wall_vertex_list(vl,vl_cnt,wall->lx,wall->rx,wall->lz,wall->rz,wall->ty,wall->by,moveable,light));
	
			// floors and ceilings
		
		case sg_floor:
		case sg_ceiling:
		case sg_poly:
			fc=&seg->data.fc;
			if ((seg->simple_tessel) || (fc->ptsz==3) || (!high_quality_lighting)) return(map_portal_add_light_simple_vertex_list(vl,vl_cnt,fc->ptsz,fc->x,fc->y,fc->z,moveable,light));
			return(map_portal_add_light_tessel_floor_ceiling_vertex_list(vl,vl_cnt,fc->ptsz,fc->x,fc->y,fc->z,moveable,light));
			
			// liquids
			// liquids are already tesselated, no need to create separate light values
			
		case sg_liquid:
			light->trig_count=0;
			return(vl_cnt);
			
			// ambient walls
			// same as draw vertexes (no tesselation)
			// these will end up being no-ops as they will
			// already be in draw vertex list
		
		case sg_ambient_wall:
			ambient_wall=&seg->data.ambient_wall;
			return(map_portal_add_light_simple_vertex_list(vl,vl_cnt,4,ambient_wall->x,ambient_wall->y,ambient_wall->z,moveable,light));
			
			// ambient fcs
			// same as draw vertexes (no tesselation)
			// these will end up being no-ops as they will
			// already be in draw vertex list
			
		case sg_ambient_fc:
			ambient_fc=&seg->data.ambient_fc;
			return(map_portal_add_light_simple_vertex_list(vl,vl_cnt,ambient_fc->ptsz,ambient_fc->x,ambient_fc->y,ambient_fc->z,moveable,light));
	
	}
	
	return(vl_cnt);
}

/* =======================================================

      Build Portal Vertex List
      
======================================================= */

bool map_portal_build_single_vertex_list(map_type *map,int rn,int nvlist,bool high_quality_lighting)
{
	int							i,vl_cnt;
	portal_vertex_list_type		*vl;
	portal_type					*portal;
	segment_type				*seg;
	
	portal=&map->portals[rn];
	
	vl_cnt=0;
	vl=portal->vertexes.vertex_list;
	
		// build list of vertexes and colors
		
	seg=map->segments;
	
	for (i=0;i!=map->nsegment;i++) {
		if (seg->rn==rn) {
			vl_cnt=map_portal_add_segment_single_vertex_list(vl,vl_cnt,seg);
			vl_cnt=map_portal_add_light_single_vertex_list(vl,vl_cnt,high_quality_lighting,seg);
		}
		seg++;
	}
	
		// total number of vertexes
		
	portal->vertexes.nvlist=vl_cnt;
	
	return(TRUE);
}

/* =======================================================

      Count Vertexes for Segment
      
======================================================= */

int map_portal_create_vertex_list_count_segment(segment_type *seg)
{
	switch (seg->type) {
		case sg_wall:
			return(seg->data.wall.ptsz);
		case sg_floor:
		case sg_ceiling:
		case sg_poly:
			return(seg->data.fc.ptsz);
		case sg_liquid:
			return(map_liquid_count_vertexes(seg));
		case sg_ambient_wall:
			return(4);
		case sg_ambient_fc:
			return(seg->data.ambient_fc.ptsz);
	}
	
	return(0);
}

/* =======================================================

      Create/Destroy Portal Vertex List
      
======================================================= */

bool map_portal_create_single_vertex_list(map_type *map,int rn,bool high_quality_lighting)
{
	int							i,nvlist,sz,rough_sz;
	portal_vertex_list_type		*nvl;
	portal_type					*portal;
	segment_type				*seg;
	
	portal=&map->portals[rn];
	
		// count maximum number of vertexes
	
	nvlist=0;
	
	seg=map->segments;
	
	for (i=0;i!=map->nsegment;i++) {
		if (seg->rn==rn) {
			nvlist+=map_portal_create_vertex_list_count_segment(seg);
			nvlist+=light_tessel_max_vertex;			// maximum number of lighting elements in list
		}
		
		seg++;
	}
	
		// create rough vertex list
	
	rough_sz=nvlist*sizeof(portal_vertex_list_type);
	portal->vertexes.vertex_list=(portal_vertex_list_type*)valloc(rough_sz);
	if (portal->vertexes.vertex_list==NULL) return(FALSE);
	
	bzero(portal->vertexes.vertex_list,rough_sz);
	
		// build vertex list
		
	if (!map_portal_build_single_vertex_list(map,rn,nvlist,high_quality_lighting)) {
		free(portal->vertexes.vertex_list);
		portal->vertexes.vertex_list=NULL;
		return(FALSE);
	}
	
		// resize vertex list
		
	sz=portal->vertexes.nvlist*sizeof(portal_vertex_list_type);

	if (sz<rough_sz) {
		nvl=(portal_vertex_list_type*)valloc(sz);
		if (nvl==NULL) {
			free(portal->vertexes.vertex_list);
			portal->vertexes.vertex_list=NULL;
			return(FALSE);
		}
		
		memmove(nvl,portal->vertexes.vertex_list,sz);
		free(portal->vertexes.vertex_list);
		portal->vertexes.vertex_list=nvl;
	}

		// compiled vertex, coord and color lists
		
	sz=portal->vertexes.nvlist*(sizeof(float)*3);
	portal->vertexes.pvert=(float*)valloc(sz);
	if (portal->vertexes.pvert==NULL) return(FALSE);

	bzero(portal->vertexes.pvert,sz);

	sz=portal->vertexes.nvlist*(sizeof(float)*2);
	portal->vertexes.pcoord=(float*)valloc(sz);
	if (portal->vertexes.pcoord==NULL) return(FALSE);

	bzero(portal->vertexes.pcoord,sz);
	
	sz=portal->vertexes.nvlist*(sizeof(float)*3);
	portal->vertexes.pcolor=(float*)valloc(sz);
	if (portal->vertexes.pcolor==NULL) return(FALSE);

	bzero(portal->vertexes.pcolor,sz);

	return(TRUE);
}

void map_portal_dispose_single_vertex_list(map_type *map,int rn)
{
	portal_type			*portal;
	
	portal=&map->portals[rn];
	
	free(portal->vertexes.vertex_list);
	free(portal->vertexes.pvert);
	free(portal->vertexes.pcoord);
	free(portal->vertexes.pcolor);
}

/* =======================================================

      Create/Destroy Portal Vertex Lists
      
======================================================= */

bool map_portal_create_vertex_lists(map_type *map,bool high_quality_lighting)
{
	int			i;
	
	for (i=0;i!=map->nportal;i++) {
		if (!map_portal_create_single_vertex_list(map,i,high_quality_lighting)) return(FALSE);
	}
	
	return(TRUE);
}

void map_portal_dispose_vertex_lists(map_type *map)
{
	int			i;
	
	for (i=0;i!=map->nportal;i++) {
		map_portal_dispose_single_vertex_list(map,i);
	}
}

void map_portal_rebuild_vertex_lists(map_type *map,bool high_quality_lighting)
{
	int			i;
	
	for (i=0;i!=map->nportal;i++) {
		map_portal_build_single_vertex_list(map,i,map->portals[i].vertexes.nvlist,high_quality_lighting);
	}
}

