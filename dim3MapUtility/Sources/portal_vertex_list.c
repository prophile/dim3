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

      Build Portal Vertex List for Mesh Poly
      
======================================================= */

int map_portal_add_mesh_poly_single_vertex_list(portal_vertex_list_type *vl,int vl_cnt,map_mesh_type *mesh,map_mesh_poly_type *mesh_poly)
{
	int				n;
	d3pnt			*pt;

	for (n=0;n!=mesh_poly->ptsz;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n]];
		vl_cnt=map_portal_add_single_vertex_list(vl,vl_cnt,pt->x,pt->y,pt->z,mesh_poly->gx[n],mesh_poly->gy[n],mesh->flag.moveable,mesh_poly->draw.shift_on,&mesh_poly->draw.portal_v[n]);
	}

	return(vl_cnt);
}

/* =======================================================

      Build Portal Vertex List for Light Tessel
      
======================================================= */

int map_portal_add_light_xy_tessel_vertex_list(portal_vertex_list_type *vl,int vl_cnt,map_mesh_type *mesh,map_mesh_poly_type *mesh_poly)
{
	int							n,x,y,z,ptsz,px[8],py[8],pz[8],
								xdist,ydist,xtot,ytot,xskip,yskip,ntrig;
	int							grid_x[light_tessel_grid_sz+1],
								grid_y[light_tessel_grid_sz+1],
								idx[light_tessel_grid_sz+1][light_tessel_grid_sz+1];
	d3pnt						*pt;
	map_mesh_poly_light_type	*light;

		// get tessel size, possible total of 64 triangles, 8 grid spots (max)

	xdist=mesh_poly->box.max.x-mesh_poly->box.min.x;
	ydist=mesh_poly->box.max.y-mesh_poly->box.min.y;

	xtot=(xdist*light_tessel_grid_sz)/light_tessel_max_size;
	if (xtot>light_tessel_grid_sz) xtot=light_tessel_grid_sz;
	if (xtot<=0) xtot=1;

	ytot=(ydist*light_tessel_grid_sz)/light_tessel_max_size;
	if (ytot>light_tessel_grid_sz) ytot=light_tessel_grid_sz;
	if (ytot<=0) ytot=1;

	xskip=xdist/xtot;
	yskip=ydist/ytot;

		// create the grid overlay

	for (x=0;x<=xtot;x++) {
		if (x==xtot) {
			grid_x[x]=mesh_poly->box.max.x;
		}
		else {
			grid_x[x]=mesh_poly->box.min.x+(x*xskip);
		}
	}

	for (y=0;y<=ytot;y++) {
		if (y==ytot) {
			grid_y[y]=mesh_poly->box.max.y;
		}
		else {
			grid_y[y]=mesh_poly->box.min.y+(y*yskip);
		}
	}

		// get vertexes for grid

	ptsz=mesh_poly->ptsz;

	for (n=0;n!=ptsz;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n]];
		px[n]=pt->x;
		py[n]=pt->y;
		pz[n]=pt->z;
	}

	for (y=0;y<=ytot;y++) {
		for (x=0;x<=xtot;x++) {
			z=polygon_find_y(ptsz,px,pz,py,grid_x[x],grid_y[y]);
			if (z==-1) z=polygon_infinite_find_y(ptsz,px,pz,py,grid_x[x],grid_y[y]);
			vl_cnt=map_portal_add_single_light_vertex_list(vl,vl_cnt,grid_x[x],grid_y[y],z,mesh->flag.moveable,&idx[x][y]);
		}
	}

		// tesselate grid into triangles

	light=&mesh_poly->light;

	ntrig=0;

	for (y=0;y!=ytot;y++) {
		
		for (x=0;x!=xtot;x++) {
		
			if (((y+x)&0x1)==0) {
				light->trig_vertex_idx[(ntrig*3)]=idx[x][y];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[x+1][y];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[x+1][y+1];
				ntrig++;
				
				light->trig_vertex_idx[(ntrig*3)]=idx[x][y];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[x][y+1];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[x+1][y+1];
				ntrig++;
			}
			else {
				light->trig_vertex_idx[(ntrig*3)]=idx[x+1][y];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[x][y];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[x][y+1];
				ntrig++;
				
				light->trig_vertex_idx[(ntrig*3)]=idx[x+1][y];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[x+1][y+1];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[x][y+1];
				ntrig++;
			}
			
		}
	}

	light->trig_count=ntrig;

	return(vl_cnt);
}

int map_portal_add_light_xz_tessel_vertex_list(portal_vertex_list_type *vl,int vl_cnt,map_mesh_type *mesh,map_mesh_poly_type *mesh_poly)
{
	int							n,x,y,z,ptsz,px[8],py[8],pz[8],
								xdist,zdist,xtot,ztot,xskip,zskip,ntrig;
	int							grid_x[light_tessel_grid_sz+1],
								grid_z[light_tessel_grid_sz+1],
								idx[light_tessel_grid_sz+1][light_tessel_grid_sz+1];
	d3pnt						*pt;
	map_mesh_poly_light_type	*light;

		// get tessel size, possible total of 64 triangles, 8 grid spots (max)

	xdist=mesh_poly->box.max.x-mesh_poly->box.min.x;
	zdist=mesh_poly->box.max.z-mesh_poly->box.min.z;

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
			grid_x[x]=mesh_poly->box.max.x;
		}
		else {
			grid_x[x]=mesh_poly->box.min.x+(x*xskip);
		}
	}

	for (z=0;z<=ztot;z++) {
		if (z==ztot) {
			grid_z[z]=mesh_poly->box.max.z;
		}
		else {
			grid_z[z]=mesh_poly->box.min.z+(z*zskip);
		}
	}

		// get vertexes for grid

	ptsz=mesh_poly->ptsz;

	for (n=0;n!=ptsz;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n]];
		px[n]=pt->x;
		py[n]=pt->y;
		pz[n]=pt->z;
	}

	for (z=0;z<=ztot;z++) {
		for (x=0;x<=xtot;x++) {
			y=polygon_find_y(ptsz,px,py,pz,grid_x[x],grid_z[z]);
			if (y==-1) y=polygon_infinite_find_y(ptsz,px,py,pz,grid_x[x],grid_z[z]);
			vl_cnt=map_portal_add_single_light_vertex_list(vl,vl_cnt,grid_x[x],y,grid_z[z],mesh->flag.moveable,&idx[x][z]);
		}
	}

		// tesselate grid into triangles

	light=&mesh_poly->light;

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

int map_portal_add_light_yz_tessel_vertex_list(portal_vertex_list_type *vl,int vl_cnt,map_mesh_type *mesh,map_mesh_poly_type *mesh_poly)
{
	int							n,x,y,z,ptsz,px[8],py[8],pz[8],
								zdist,ydist,ztot,ytot,zskip,yskip,ntrig;
	int							grid_z[light_tessel_grid_sz+1],
								grid_y[light_tessel_grid_sz+1],
								idx[light_tessel_grid_sz+1][light_tessel_grid_sz+1];
	d3pnt						*pt;
	map_mesh_poly_light_type	*light;

		// get tessel size, possible total of 64 triangles, 8 grid spots (max)

	zdist=mesh_poly->box.max.z-mesh_poly->box.min.z;
	ydist=mesh_poly->box.max.y-mesh_poly->box.min.y;

	ztot=(zdist*light_tessel_grid_sz)/light_tessel_max_size;
	if (ztot>light_tessel_grid_sz) ztot=light_tessel_grid_sz;
	if (ztot<=0) ztot=1;

	ytot=(ydist*light_tessel_grid_sz)/light_tessel_max_size;
	if (ytot>light_tessel_grid_sz) ytot=light_tessel_grid_sz;
	if (ytot<=0) ytot=1;

	zskip=zdist/ztot;
	yskip=ydist/ytot;

		// create the grid overlay

	for (z=0;z<=ztot;z++) {
		if (z==ztot) {
			grid_z[z]=mesh_poly->box.max.z;
		}
		else {
			grid_z[z]=mesh_poly->box.min.z+(z*zskip);
		}
	}

	for (y=0;y<=ytot;y++) {
		if (y==ytot) {
			grid_y[y]=mesh_poly->box.max.y;
		}
		else {
			grid_y[y]=mesh_poly->box.min.y+(y*yskip);
		}
	}

		// get vertexes for grid

	ptsz=mesh_poly->ptsz;

	for (n=0;n!=ptsz;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n]];
		px[n]=pt->x;
		py[n]=pt->y;
		pz[n]=pt->z;
	}

	for (y=0;y<=ytot;y++) {
		for (z=0;z<=ztot;z++) {
			x=polygon_find_y(ptsz,pz,px,py,grid_z[z],grid_y[y]);
			if (z==-1) x=polygon_infinite_find_y(ptsz,pz,px,py,grid_z[z],grid_y[y]);
			vl_cnt=map_portal_add_single_light_vertex_list(vl,vl_cnt,x,grid_y[y],grid_z[z],mesh->flag.moveable,&idx[z][y]);
		}
	}

		// tesselate grid into triangles

	light=&mesh_poly->light;

	ntrig=0;

	for (y=0;y!=ytot;y++) {
		
		for (z=0;z!=ztot;z++) {
		
			if (((y+z)&0x1)==0) {
				light->trig_vertex_idx[(ntrig*3)]=idx[z][y];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[z+1][y];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[z+1][y+1];
				ntrig++;
				
				light->trig_vertex_idx[(ntrig*3)]=idx[z][y];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[z][y+1];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[z+1][y+1];
				ntrig++;
			}
			else {
				light->trig_vertex_idx[(ntrig*3)]=idx[z+1][y];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[z][y];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[z][y+1];
				ntrig++;
				
				light->trig_vertex_idx[(ntrig*3)]=idx[z+1][y];
				light->trig_vertex_idx[(ntrig*3)+1]=idx[z+1][y+1];
				light->trig_vertex_idx[(ntrig*3)+2]=idx[z][y+1];
				ntrig++;
			}
			
		}
	}

	light->trig_count=ntrig;

	return(vl_cnt);
}

int map_portal_add_light_simple_vertex_list(portal_vertex_list_type *vl,int vl_cnt,map_mesh_type *mesh,map_mesh_poly_type *mesh_poly)
{
	int							n,k,ntrig,tx[3],ty[3],tz[3];
	d3pnt						*pt;
	map_mesh_poly_light_type	*light;

	light=&mesh_poly->light;

	light->trig_count=0;

		// break up polygon into triangles
		// but no tesselation of triangles
		// this is used for simple lighting
		
	ntrig=mesh_poly->ptsz-3;

	pt=&mesh->vertexes[mesh_poly->v[0]];
	tx[0]=pt->x;
	ty[0]=pt->y;
	tz[0]=pt->z;
	
	for (n=0;n<=ntrig;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n+1]];
		tx[1]=pt->x;
		ty[1]=pt->y;
		tz[1]=pt->z;

		pt=&mesh->vertexes[mesh_poly->v[n+2]];
		tx[2]=pt->x;
		ty[2]=pt->y;
		tz[2]=pt->z;

		for (k=0;k!=3;k++) {
			vl_cnt=map_portal_add_single_light_vertex_list(vl,vl_cnt,tx[k],ty[k],tz[k],mesh->flag.moveable,&light->trig_vertex_idx[(n*3)+k]);
		}

		light->trig_count++;
	}
	
	return(vl_cnt);
}

int map_portal_add_light_single_vertex_list(portal_vertex_list_type *vl,int vl_cnt,bool high_quality_lighting,map_mesh_type *mesh,map_mesh_poly_type *mesh_poly)
{
	int			xsz,ysz,zsz;
	
		// simple light tessels

	if ((mesh_poly->draw.simple_tessel) || (mesh_poly->ptsz==3) || (!high_quality_lighting)) return(map_portal_add_light_simple_vertex_list(vl,vl_cnt,mesh,mesh_poly));

		// tessel depending on shape of polygon
		// first, automatically tessel on the other two
		// axises if any axies' length is 0

	xsz=mesh_poly->box.max.x-mesh_poly->box.min.x;
	if (xsz==0) return(map_portal_add_light_yz_tessel_vertex_list(vl,vl_cnt,mesh,mesh_poly));

	ysz=mesh_poly->box.max.y-mesh_poly->box.min.y;
	if (ysz==0) return(map_portal_add_light_xz_tessel_vertex_list(vl,vl_cnt,mesh,mesh_poly));

	zsz=mesh_poly->box.max.z-mesh_poly->box.min.z;
	if (zsz==0) return(map_portal_add_light_xy_tessel_vertex_list(vl,vl_cnt,mesh,mesh_poly));

		// polygons standing straight up will have common x/z's

	if (mesh_poly->box.common_xz) {
		if (xsz>zsz) return(map_portal_add_light_xy_tessel_vertex_list(vl,vl_cnt,mesh,mesh_poly));
		return(map_portal_add_light_yz_tessel_vertex_list(vl,vl_cnt,mesh,mesh_poly));
	}

		// else treat as floor or ceiling like polygon

	return(map_portal_add_light_xz_tessel_vertex_list(vl,vl_cnt,mesh,mesh_poly));
}

/* =======================================================

      Build Portal Vertex List
      
======================================================= */

bool map_portal_build_single_vertex_list(map_type *map,int rn,bool high_quality_lighting)
{
	int							n,k,vl_cnt;
	portal_vertex_list_type		*vl;
	portal_type					*portal;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*mesh_poly;
	
	portal=&map->portals[rn];
	
	vl_cnt=0;
	vl=portal->vertexes.vertex_list;
	
		// build list of vertexes and colors

	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			vl_cnt=map_portal_add_mesh_poly_single_vertex_list(vl,vl_cnt,mesh,mesh_poly);
			vl_cnt=map_portal_add_light_single_vertex_list(vl,vl_cnt,high_quality_lighting,mesh,mesh_poly);
			mesh_poly++;
		}
	
		mesh++;
	}
	
		// total number of vertexes
		
	portal->vertexes.nvlist=vl_cnt;
	
	return(TRUE);
}

/* =======================================================

      Create/Destroy Portal Vertex List
      
======================================================= */

bool map_portal_create_single_vertex_list(map_type *map,int rn,bool high_quality_lighting)
{
	int							n,k,nvlist,sz,rough_sz;
	portal_vertex_list_type		*nvl;
	portal_type					*portal;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*mesh_poly;
	
	portal=&map->portals[rn];

		// find maximum possible number of vertexes

	nvlist=0;

	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			nvlist+=mesh_poly->ptsz;
			nvlist+=light_tessel_max_vertex;			// maximum number of lighting elements in list
			mesh_poly++;
		}
	
		mesh++;
	}
	
		// create rough vertex list
	
	rough_sz=nvlist*sizeof(portal_vertex_list_type);
	portal->vertexes.vertex_list=(portal_vertex_list_type*)valloc(rough_sz);
	if (portal->vertexes.vertex_list==NULL) return(FALSE);
	
	bzero(portal->vertexes.vertex_list,rough_sz);
	
		// build vertex list
		
	if (!map_portal_build_single_vertex_list(map,rn,high_quality_lighting)) {
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

		// hit list

	portal->vertexes.phit=(unsigned char*)valloc(portal->vertexes.nvlist);
	if (portal->vertexes.phit==NULL) return(FALSE);

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
	free(portal->vertexes.phit);
}

/* =======================================================

      Create/Destroy Portal Vertex Lists
      
======================================================= */

bool map_portal_create_vertex_lists(map_type *map,bool high_quality_lighting)
{
	int			n;
	
	for (n=0;n!=map->nportal;n++) {
		if (!map_portal_create_single_vertex_list(map,n,high_quality_lighting)) return(FALSE);
	}
	
	return(TRUE);
}

void map_portal_dispose_vertex_lists(map_type *map)
{
	int			n;
	
	for (n=0;n!=map->nportal;n++) {
		map_portal_dispose_single_vertex_list(map,n);
	}
}

void map_portal_rebuild_vertex_lists(map_type *map,bool high_quality_lighting)
{
	int			n;
	
	for (n=0;n!=map->nportal;n++) {
		map_portal_build_single_vertex_list(map,n,high_quality_lighting);
	}
}

