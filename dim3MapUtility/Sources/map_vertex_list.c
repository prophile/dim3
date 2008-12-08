/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Vertex Lists

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

/* =======================================================

      Division Reduction
      
======================================================= */

inline int map_portal_all_light_div_reduce(int dist,int quality_mode)
{
	int			div;
	
	if (quality_mode==quality_mode_high) {
		div=dist/(map_enlarge*12);
	}
	else {
		div=dist/(map_enlarge*16);
	}
	
	if (div<1) div=1;
	if (div>light_tessel_max_grid_div) div=light_tessel_max_grid_div;
	
	return(div);
}

/* =======================================================

      Triangle Tesselation
      
======================================================= */

void map_portal_add_light_trig_tessel_vertex_list(map_mesh_type *mesh,map_mesh_poly_type *poly,int quality_mode)
{
	int									vl_cnt,x,y,k,idx,div_x,div_y,
										d0,d1,d2;
	float								*vl_uv;
	d3pnt								p1,p2,chg_x,chg_y;
	d3vct								g1,g2,chg_gx,chg_gy;
	d3pnt								*pt[3],*vl_pt;
	
		// find largest side of triangle
		
	pt[0]=&mesh->vertexes[poly->v[0]];
	pt[1]=&mesh->vertexes[poly->v[1]];
	pt[2]=&mesh->vertexes[poly->v[2]];
	
	d0=distance_get(pt[0]->x,pt[0]->y,pt[0]->z,pt[1]->x,pt[1]->y,pt[1]->z);
	d1=distance_get(pt[1]->x,pt[1]->y,pt[1]->z,pt[2]->x,pt[2]->y,pt[2]->z);
	d2=distance_get(pt[2]->x,pt[2]->y,pt[2]->z,pt[0]->x,pt[0]->y,pt[0]->z);
	
	if ((d0>d1) && (d0>d2)) {
		idx=2;
	}
	else {
		if (d1>d2) {
			idx=0;
		}
		else {
			idx=1;
		}
	}
	
		// get size of shorter lines
		// and find divisions
		
	k=idx+1;
	if (k==3) k=0;
	
	chg_y.x=pt[k]->x-pt[idx]->x;
	chg_y.y=pt[k]->y-pt[idx]->y;
	chg_y.z=pt[k]->z-pt[idx]->z;
	
	chg_gy.x=poly->gx[k]-poly->gx[idx];
	chg_gy.y=poly->gy[k]-poly->gy[idx];
	
	d0=distance_get(0,0,0,chg_y.x,chg_y.y,chg_y.z);
	div_y=map_portal_all_light_div_reduce(d0,quality_mode);
	
	k=idx-1;
	if (k==-1) k=2;
	
	chg_x.x=pt[k]->x-pt[idx]->x;
	chg_x.y=pt[k]->y-pt[idx]->y;
	chg_x.z=pt[k]->z-pt[idx]->z;
	
	chg_gx.x=poly->gx[k]-poly->gx[idx];
	chg_gx.y=poly->gy[k]-poly->gy[idx];
	
	d0=distance_get(0,0,0,chg_x.x,chg_x.y,chg_x.z);
	div_x=map_portal_all_light_div_reduce(d0,quality_mode);
	
		// if a single div on both sides, then
		// make it simple
	
	if ((div_x==1) && (div_y==1)) {
		poly->light.simple_tessel=TRUE;
		return;
	}
	
		// split up trig on smaller sides
		
	vl_cnt=0;
	vl_pt=mesh->light.quad_vertexes+poly->light.vertex_offset;
	vl_uv=mesh->light.quad_uvs+(poly->light.vertex_offset<<1);

	for (y=0;y<=div_y;y++) {
	
		p1.x=pt[idx]->x+((chg_y.x*y)/div_y);
		p1.y=pt[idx]->y+((chg_y.y*y)/div_y);
		p1.z=pt[idx]->z+((chg_y.z*y)/div_y);
		
		g1.x=poly->gx[idx]+((chg_gy.x*((float)y))/((float)div_y));
		g1.y=poly->gy[idx]+((chg_gy.y*((float)y))/((float)div_y));
		
		p2.x=p1.x+chg_x.x;
		p2.y=p1.y+chg_x.y;
		p2.z=p1.z+chg_x.z;

		g2.x=g1.x+chg_gx.x;
		g2.y=g1.y+chg_gx.y;
		
		for (x=0;x<=div_x;x++) {
		
			vl_pt->x=p1.x+(((p2.x-p1.x)*x)/div_x);
			vl_pt->y=p1.y+(((p2.y-p1.y)*x)/div_x);
			vl_pt->z=p1.z+(((p2.z-p1.z)*x)/div_x);

			*vl_uv++=g1.x+(((g2.x-g1.x)*((float)x))/((float)div_x));
			*vl_uv++=g1.y+(((g2.y-g1.y)*((float)x))/((float)div_x));
		
			vl_cnt++;
			vl_pt++;
		}
	}
	
	poly->light.grid_x_sz=div_x;
	poly->light.grid_y_sz=div_y;

	poly->light.nvertex=vl_cnt;
	poly->light.nquad=div_x*div_y;
}

/* =======================================================

      Quad Tesselation
      
======================================================= */

void map_portal_add_light_quad_tessel_vertex_list(map_mesh_type *mesh,map_mesh_poly_type *poly,int quality_mode)
{
	int									vl_cnt,x,y,d,d2,div_x,div_y;
	float								*vl_uv;
	d3pnt								p1,p2;
	d3vct								g1,g2;
	d3pnt								*vertexes,*vl_pt;

	vertexes=mesh->vertexes;
	
		// get split sizes
		
	d=distance_get(vertexes[poly->v[0]].x,vertexes[poly->v[0]].y,vertexes[poly->v[0]].z,vertexes[poly->v[3]].x,vertexes[poly->v[3]].y,vertexes[poly->v[3]].z);
	d2=distance_get(vertexes[poly->v[1]].x,vertexes[poly->v[1]].y,vertexes[poly->v[1]].z,vertexes[poly->v[2]].x,vertexes[poly->v[2]].y,vertexes[poly->v[2]].z);
	if (d2>d) d=d2;
	
	div_y=map_portal_all_light_div_reduce(d,quality_mode);
	
	d=distance_get(vertexes[poly->v[0]].x,vertexes[poly->v[0]].y,vertexes[poly->v[0]].z,vertexes[poly->v[1]].x,vertexes[poly->v[1]].y,vertexes[poly->v[1]].z);
	d2=distance_get(vertexes[poly->v[2]].x,vertexes[poly->v[2]].y,vertexes[poly->v[2]].z,vertexes[poly->v[3]].x,vertexes[poly->v[3]].y,vertexes[poly->v[3]].z);
	if (d2>d) d=d2;
	
	div_x=map_portal_all_light_div_reduce(d,quality_mode);
	
		// if a single div on both sides, then
		// make it simple
	
	if ((div_x==1) && (div_y==1)) {
		poly->light.simple_tessel=TRUE;
		return;
	}
	
		// split up quad
		
	vl_cnt=0;
	vl_pt=mesh->light.quad_vertexes+poly->light.vertex_offset;
	vl_uv=mesh->light.quad_uvs+(poly->light.vertex_offset<<1);
		
	for (y=0;y<=div_y;y++) {
	
		p1.x=vertexes[poly->v[0]].x+(((vertexes[poly->v[3]].x-vertexes[poly->v[0]].x)*y)/div_y);
		p1.y=vertexes[poly->v[0]].y+(((vertexes[poly->v[3]].y-vertexes[poly->v[0]].y)*y)/div_y);
		p1.z=vertexes[poly->v[0]].z+(((vertexes[poly->v[3]].z-vertexes[poly->v[0]].z)*y)/div_y);
		
		g1.x=poly->gx[0]+(((poly->gx[3]-poly->gx[0])*((float)y))/((float)div_y));
		g1.y=poly->gy[0]+(((poly->gy[3]-poly->gy[0])*((float)y))/((float)div_y));
		
		p2.x=vertexes[poly->v[1]].x+(((vertexes[poly->v[2]].x-vertexes[poly->v[1]].x)*y)/div_y);
		p2.y=vertexes[poly->v[1]].y+(((vertexes[poly->v[2]].y-vertexes[poly->v[1]].y)*y)/div_y);
		p2.z=vertexes[poly->v[1]].z+(((vertexes[poly->v[2]].z-vertexes[poly->v[1]].z)*y)/div_y);

		g2.x=poly->gx[1]+(((poly->gx[2]-poly->gx[1])*((float)y))/((float)div_y));
		g2.y=poly->gy[1]+(((poly->gy[2]-poly->gy[1])*((float)y))/((float)div_y));
		
		for (x=0;x<=div_x;x++) {
			
			vl_pt->x=p1.x+(((p2.x-p1.x)*x)/div_x);
			vl_pt->y=p1.y+(((p2.y-p1.y)*x)/div_x);
			vl_pt->z=p1.z+(((p2.z-p1.z)*x)/div_x);

			*vl_uv++=g1.x+(((g2.x-g1.x)*((float)x))/((float)div_x));
			*vl_uv++=g1.y+(((g2.y-g1.y)*((float)x))/((float)div_x));
		
			vl_cnt++;
			vl_pt++;
		}
	}
	
	poly->light.grid_x_sz=div_x;
	poly->light.grid_y_sz=div_y;

	poly->light.nvertex=vl_cnt;
	poly->light.nquad=div_x*div_y;
}

/* =======================================================

      Catch All Wall Tessel
      
======================================================= */

void map_portal_add_light_wall_tessel_vertex_list(map_mesh_type *mesh,map_mesh_poly_type *poly,int quality_mode)
{
	int									n,vl_cnt,x,y,ptsz,div_x,div_y,
										xdist,ydist,zdist,xzdist,xskip,yskip,zskip;
	int									grid_x[light_tessel_max_grid_div+1],grid_z[light_tessel_max_grid_div+1],
										grid_y[light_tessel_max_grid_div+1];
	float								lgx,rgx,tgy,bgy,f_dist;
	float								*vl_uv;
	double								dx,dz;
	d3pnt								*pt,*vl_pt;

		// get tessel size

	xdist=poly->line.rx-poly->line.lx;
	ydist=poly->box.max.y-poly->box.min.y;
	zdist=poly->line.rz-poly->line.lz;
	
	xzdist=(int)(sqrt((double)(xdist*xdist)+(double)(zdist*zdist)));
	
	div_y=map_portal_all_light_div_reduce(ydist,quality_mode);
	div_x=map_portal_all_light_div_reduce(xzdist,quality_mode);

	xskip=xdist/div_x;
	zskip=zdist/div_x;
	yskip=ydist/div_y;

		// create the grid overlay

	for (x=0;x<=div_x;x++) {
	
		if (x==0) {
			grid_x[x]=poly->line.lx-light_tessel_overlap_pixel;
			grid_z[x]=poly->line.lz-light_tessel_overlap_pixel;
		}
		else {
			if (x==div_x) {
				grid_x[x]=poly->line.rx+light_tessel_overlap_pixel;
				grid_z[x]=poly->line.rz+light_tessel_overlap_pixel;
			}
			else {
				grid_x[x]=poly->line.lx+(x*xskip);
				grid_z[x]=poly->line.lz+(x*zskip);
			}
		}
	}

	for (y=0;y<=div_y;y++) {
	
		if (y==0) {
			grid_y[y]=poly->box.min.y-light_tessel_overlap_pixel;
		}
		else {
			if (y==div_y) {
				grid_y[y]=poly->box.max.y+light_tessel_overlap_pixel;
			}
			else {
				grid_y[y]=poly->box.min.y+(y*yskip);
			}
		}
	}

		// get vertexes for grid
		// and find uv extends

	lgx=rgx=0.0f;
	tgy=bgy=0.0f;

	ptsz=poly->ptsz;

	for (n=0;n!=ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		if ((pt->x==poly->line.lx) && (pt->z==poly->line.lz)) lgx=poly->gx[n];
		if ((pt->x==poly->line.rx) && (pt->z==poly->line.rz)) rgx=poly->gx[n];
		if (pt->y==poly->box.min.y) tgy=poly->gy[n];
		if (pt->y==poly->box.max.y) bgy=poly->gy[n];
	}
	
		// create vertexes
		
	vl_cnt=0;
	vl_pt=mesh->light.quad_vertexes+poly->light.vertex_offset;
	vl_uv=mesh->light.quad_uvs+(poly->light.vertex_offset<<1);

	for (y=0;y<=div_y;y++) {
		for (x=0;x<=div_x;x++) {

			vl_pt->x=grid_x[x];
			vl_pt->y=grid_y[y];
			vl_pt->z=grid_z[x];

			dx=(double)(vl_pt->x-poly->line.lx);
			dz=(double)(vl_pt->z-poly->line.lz);
			f_dist=(float)sqrt((dx*dx)+(dz*dz));

			*vl_uv++=lgx+(((rgx-lgx)*f_dist)/(float)xzdist);
			*vl_uv++=tgy+(((bgy-tgy)*(float)(vl_pt->y-poly->box.min.y))/(float)(poly->box.max.y-poly->box.min.y));
		
			vl_cnt++;
			vl_pt++;
		}
	}

		// setup light for drawing
	
	poly->light.grid_x_sz=div_x;
	poly->light.grid_y_sz=div_y;

	poly->light.nvertex=vl_cnt;
	poly->light.nquad=div_x*div_y;
}

/* =======================================================

      Catch All Floor/Ceiling Tessel
      
======================================================= */

void map_portal_add_light_floor_tessel_vertex_list(map_mesh_type *mesh,map_mesh_poly_type *poly,int quality_mode)
{
	int									n,vl_cnt,x,y,z,ptsz,px[8],py[8],pz[8],
										div_x,div_y,xdist,zdist,xskip,zskip;
	int									grid_x[light_tessel_max_grid_div+1],
										grid_z[light_tessel_max_grid_div+1];
	float								lgx,rgx,lgy,rgy;
	float								*vl_uv;
	d3pnt								*pt,*vl_pt;

		// get tessel size, possible total of 64 triangles, 8 grid spots (max)

	xdist=poly->box.max.x-poly->box.min.x;
	zdist=poly->box.max.z-poly->box.min.z;
	
	div_y=map_portal_all_light_div_reduce(zdist,quality_mode);
	div_x=map_portal_all_light_div_reduce(xdist,quality_mode);

	xskip=xdist/div_x;
	zskip=zdist/div_y;

		// create the grid overlay

	for (x=0;x<=div_x;x++) {
	
		if (x==0) {
			grid_x[x]=poly->box.min.x-light_tessel_overlap_pixel;
		}
		else {
			if (x==div_x) {
				grid_x[x]=poly->box.max.x+light_tessel_overlap_pixel;
			}
			else {
				grid_x[x]=poly->box.min.x+(x*xskip);
			}
		}
	}

	for (z=0;z<=div_y;z++) {
		if (z==0) {
			grid_z[z]=poly->box.min.z-light_tessel_overlap_pixel;
		}
		else {
			if (z==div_y) {
				grid_z[z]=poly->box.max.z+light_tessel_overlap_pixel;
			}
			else {
				grid_z[z]=poly->box.min.z+(z*zskip);
			}
		}
	}

		// get vertexes for grid
		// and setup uv and y calculations

	lgx=rgx=lgy=rgy=0.0f;

	ptsz=poly->ptsz;

	for (n=0;n!=ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		px[n]=pt->x;
		py[n]=pt->y;
		pz[n]=pt->z;

		if (pt->x==poly->box.min.x) lgx=poly->gx[n];
		if (pt->x==poly->box.max.x) rgx=poly->gx[n];
		if (pt->z==poly->box.min.z) lgy=poly->gy[n];
		if (pt->z==poly->box.max.z) rgy=poly->gy[n];
	}

		// create vertexes
		
	vl_cnt=0;
	vl_pt=mesh->light.quad_vertexes+poly->light.vertex_offset;
	vl_uv=mesh->light.quad_uvs+(poly->light.vertex_offset<<1);

	for (z=0;z<=div_y;z++) {
		for (x=0;x<=div_x;x++) {

			if (poly->box.flat) {
				y=poly->box.mid.y;
			}
			else {
				y=polygon_find_y(ptsz,px,py,pz,grid_x[x],grid_z[z]);
				if (y==-1) y=polygon_find_y_outside_point(ptsz,px,py,pz,grid_x[x],grid_z[z]);
			}

			vl_pt->x=grid_x[x];
			vl_pt->y=y;
			vl_pt->z=grid_z[z];

			*vl_uv++=lgx+(((rgx-lgx)*(float)(vl_pt->x-poly->box.min.x))/(float)(poly->box.max.x-poly->box.min.x));
			*vl_uv++=lgy+(((rgy-lgy)*(float)(vl_pt->z-poly->box.min.z))/(float)(poly->box.max.z-poly->box.min.z));
		
			vl_cnt++;
			vl_pt++;
		}
	}

		// setup light for drawing

	poly->light.grid_x_sz=div_x;
	poly->light.grid_y_sz=div_y;

	poly->light.nvertex=vl_cnt;
	poly->light.nquad=div_x*div_y;
}

/* =======================================================

      Create/Dispose Tesseled Lighting Vertexes For Polygon
      
======================================================= */

void map_create_poly_tesseled_vertexes(map_mesh_type *mesh,map_mesh_poly_type *poly,int vertex_offset,int quad_offset,int quality_mode)
{
	poly->light.nvertex=0;
	poly->light.vertex_offset=vertex_offset;
	
	poly->light.nquad=0;
	poly->light.quad_offset=quad_offset;
	
		// simple light tessels
		// these just use the polygon itself

	if ((quality_mode==quality_mode_low) || (poly->draw.simple_tessel) || (mesh->flag.hilite)) {
		poly->light.simple_tessel=TRUE;
		return;
	}
	
		// tesseled lighting needs draw and vertex lists
		// tessel depending on shape of polygon
	
	poly->light.simple_tessel=FALSE;
	
		// special trig/quad check

	if (poly->ptsz==3) {
		map_portal_add_light_trig_tessel_vertex_list(mesh,poly,quality_mode);
		return;
	}
		
	if (poly->ptsz==4) {
		map_portal_add_light_quad_tessel_vertex_list(mesh,poly,quality_mode);
		return;
	}

		// catch all for non-standard polygons

	if (poly->box.wall_like) {
		map_portal_add_light_wall_tessel_vertex_list(mesh,poly,quality_mode);
	}
	else {
		map_portal_add_light_floor_tessel_vertex_list(mesh,poly,quality_mode);
	}
}

/* =======================================================

      Create/Dispose Mesh Vertex Lists
      
======================================================= */

bool map_create_mesh_vertexes(map_type *map,int quality_mode)
{
	int					n,k,vertex_count,sz,
						vertex_offset,quad_offset;
	unsigned char		*nptr;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

		// indexes and max vertexes are only
		// used for liquids

	map->mesh_vertexes.index_ptr=NULL;
	map->mesh_vertexes.max_vertex_count=0;
	map->mesh_vertexes.draw_vertex_count=0;
	
		// create tesseled lighting vertexes

	mesh=map->mesh.meshes;
	
	for (n=0;n!=map->mesh.nmesh;n++) {

			// keep a total count of all vertexes
			// for this mesh (poly itself and lighting meshes)
			
		vertex_count=0;
		
			// allocate enough memory for all possible
			// lighting vertexes, and reduce when we have
			// real counts

			// quad vertex isn't used yet, so we'll set
			// that when we have exact numbers
		
		sz=sizeof(d3pnt)*(((light_tessel_max_grid_div+1)*(light_tessel_max_grid_div+1))*mesh->npoly);
		mesh->light.quad_vertexes=(d3pnt*)malloc(sz);
		if (mesh->light.quad_vertexes==NULL) return(FALSE);
		
		sz=(sizeof(float)*2)*(((light_tessel_max_grid_div+1)*(light_tessel_max_grid_div+1))*mesh->npoly);
		mesh->light.quad_uvs=(float*)malloc(sz);
		if (mesh->light.quad_uvs==NULL) return(FALSE);

		mesh->light.quad_vertex_idx=NULL;
		
		vertex_offset=0;
		quad_offset=0;

			// create lighting

		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			map_create_poly_tesseled_vertexes(mesh,poly,vertex_offset,quad_offset,quality_mode);
			vertex_count+=(poly->ptsz+poly->light.nvertex);
			
			vertex_offset+=poly->light.nvertex;
			quad_offset+=(poly->light.nquad<<2);
			
			poly++;
		}
		
			// fix for count
			// if no lighting mesh vertexes, then just NULL out
			
		if (vertex_offset==0) {
			free(mesh->light.quad_vertexes);
			mesh->light.quad_vertexes=NULL;
			
			free(mesh->light.quad_uvs);
			mesh->light.quad_uvs=NULL;
		}
		else {
			sz=sizeof(d3pnt)*vertex_offset;
			nptr=malloc(sz);
			if (nptr!=NULL) {
				memmove(nptr,mesh->light.quad_vertexes,sz);
				free(mesh->light.quad_vertexes);
				mesh->light.quad_vertexes=(d3pnt*)nptr;
			}
			
			sz=(sizeof(float)*2)*vertex_offset;
			nptr=malloc(sz);
			if (nptr!=NULL) {
				memmove(nptr,mesh->light.quad_uvs,sz);
				free(mesh->light.quad_uvs);
				mesh->light.quad_uvs=(float*)nptr;
			}
			
			sz=sizeof(int)*quad_offset;		// already contains *4
			mesh->light.quad_vertex_idx=malloc(sz);
			if (mesh->light.quad_vertex_idx==NULL) return(FALSE);
		}

			// remember the count

		mesh->draw.vertex_count=vertex_count;
		
		mesh++;
	}

	return(TRUE);
}

void map_dispose_mesh_vertexes(map_type *map)
{
	int					n;
	map_mesh_type		*mesh;
	
		// dispose tesseled lighting vertexes
		
	mesh=map->mesh.meshes;
	
	for (n=0;n!=map->mesh.nmesh;n++) {
		if (mesh->light.quad_vertexes!=NULL) free(mesh->light.quad_vertexes);
		if (mesh->light.quad_uvs!=NULL) free(mesh->light.quad_uvs);
		if (mesh->light.quad_vertex_idx!=NULL) free(mesh->light.quad_vertex_idx);
		mesh++;
	}
}

/* =======================================================

      Create/Dispose Liquid Vertex Lists
      
======================================================= */

bool map_create_liquid_vertexes(map_type *map)
{
	int					n,nvlist,sz;
	map_liquid_type		*liq;
	
		// find maximum possible number of vertexes
		// for liquid griding that could be drawn in a scene
	
	nvlist=0;
	
	liq=map->liquid.liquids;
		
	for (n=0;n!=map->liquid.nliquid;n++) {
	
		sz=(liq->tide.division+4)*(liq->tide.division+4);	// possible extra edges on side because of griding
		if (sz>nvlist) nvlist=sz;
		
		liq++;
	}

	if (sz==0) sz=4;
	
		// compiled index lists

	sz=nvlist*(sizeof(int)*4);
	map->liquid_vertexes.index_ptr=(int*)malloc(sz);
	if (map->liquid_vertexes.index_ptr==NULL) return(FALSE);

	bzero(map->liquid_vertexes.index_ptr,sz);

		// remember total

	map->liquid_vertexes.max_vertex_count=nvlist;
	map->liquid_vertexes.draw_vertex_count=0;

	return(TRUE);
}

void map_dispose_liquid_vertexes(map_type *map)
{
	free(map->liquid_vertexes.index_ptr);
}

/* =======================================================

      Create/Dispose Polygon Sorting Lists
      
======================================================= */

bool map_create_sort_lists(map_type *map)
{
	int					sz;

	sz=max_sort_poly*sizeof(map_poly_sort_item_type);
	map->sort.list=(map_poly_sort_item_type*)malloc(sz);
	if (map->sort.list==NULL) return(FALSE);

	bzero(map->sort.list,sz);
	
	return(TRUE);
}

void map_dispose_sort_lists(map_type *map)
{
	free(map->sort.list);
}

/* =======================================================

      Create/Dispose Map Vertex Lists
      
======================================================= */

bool map_create_vertex_lists(map_type *map,int quality_mode)
{
	if (!map_create_mesh_vertexes(map,quality_mode)) return(FALSE);
	if (!map_create_liquid_vertexes(map)) {
		map_dispose_mesh_vertexes(map);
		return(FALSE);
	}
	if (!map_create_sort_lists(map)) {
		map_dispose_liquid_vertexes(map);
		map_dispose_mesh_vertexes(map);
		return(FALSE);
	}
	
	return(TRUE);
}

void map_dispose_vertex_lists(map_type *map)
{
	map_dispose_sort_lists(map);
	map_dispose_liquid_vertexes(map);
	map_dispose_mesh_vertexes(map);
}
