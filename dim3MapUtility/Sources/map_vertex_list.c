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

      Build Tesseled Lighting Triangles
      
======================================================= */

void map_portal_add_light_wall_tessel_vertex_list(map_mesh_type *mesh,map_mesh_poly_type *poly,int grid_split_sz)
{
	int									n,vl_cnt,x,y,ptsz,
										xzdist,xdist,ydist,zdist,xztot,ytot,xskip,yskip,zskip,ntrig;
	int									grid_x[light_tessel_grid_sz+1],grid_z[light_tessel_grid_sz+1],
										grid_y[light_tessel_grid_sz+1],
										idx[light_tessel_grid_sz+1][light_tessel_grid_sz+1];
	float								lgx,rgx,tgy,bgy,f_dist;
	double								dx,dz;
	d3pnt								*pt;
	map_mesh_poly_light_type			*light;
	map_mesh_poly_tessel_vertex_type	*vl;

	light=&poly->light;
	vl=light->vertexes;

		// get tessel size, possible total of 64 triangles, 8 grid spots (max)

	xdist=poly->line.rx-poly->line.lx;
	ydist=poly->box.max.y-poly->box.min.y;
	zdist=poly->line.rz-poly->line.lz;

	dx=(double)xdist;
	dz=(double)zdist;
	xzdist=(int)sqrt((dx*dx)+(dz*dz));
	xztot=xzdist/grid_split_sz;
	if (xztot<=0) xztot=1;
	if (xztot>light_tessel_grid_sz) xztot=light_tessel_grid_sz;

	ytot=ydist/grid_split_sz;
	if (ytot<=0) ytot=1;
	if (ytot>light_tessel_grid_sz) ytot=light_tessel_grid_sz;

	xskip=xdist/xztot;
	zskip=zdist/xztot;
	yskip=ydist/ytot;

		// create the grid overlay

	for (x=0;x<=xztot;x++) {
	
		if (x==0) {
			grid_x[x]=poly->line.lx-light_tessel_overlap_pixel;
			grid_z[x]=poly->line.lz-light_tessel_overlap_pixel;
		}
		else {
			if (x==xztot) {
				grid_x[x]=poly->line.rx+light_tessel_overlap_pixel;
				grid_z[x]=poly->line.rz+light_tessel_overlap_pixel;
			}
			else {
				grid_x[x]=poly->line.lx+(x*xskip);
				grid_z[x]=poly->line.lz+(x*zskip);
			}
		}
	}

	for (y=0;y<=ytot;y++) {
	
		if (y==0) {
			grid_y[y]=poly->box.min.y-light_tessel_overlap_pixel;
		}
		else {
			if (y==ytot) {
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

	vl_cnt=0;

	for (y=0;y<=ytot;y++) {
		for (x=0;x<=xztot;x++) {

			vl->x=grid_x[x];
			vl->y=grid_y[y];
			vl->z=grid_z[x];

			dx=(double)(vl->x-poly->line.lx);
			dz=(double)(vl->z-poly->line.lz);
			f_dist=(float)sqrt((dx*dx)+(dz*dz));

			vl->gx=lgx+(((rgx-lgx)*f_dist)/(float)xzdist);
			vl->gy=tgy+(((bgy-tgy)*(float)(vl->y-poly->box.min.y))/(float)(poly->box.max.y-poly->box.min.y));

			idx[x][y]=vl_cnt;

			vl_cnt++;
			vl++;
		}
	}

		// tesselate grid into triangles

	ntrig=0;

	for (y=0;y!=ytot;y++) {
		
		for (x=0;x!=xztot;x++) {
		
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

	light->nvertex=vl_cnt;
	light->ntrig=ntrig;
}

void map_portal_add_light_floor_tessel_vertex_list(map_mesh_type *mesh,map_mesh_poly_type *poly,int grid_split_sz)
{
	int									n,vl_cnt,x,y,z,ptsz,px[8],py[8],pz[8],
										xdist,zdist,xtot,ztot,xskip,zskip,ntrig;
	int									grid_x[light_tessel_grid_sz+1],
										grid_z[light_tessel_grid_sz+1],
										idx[light_tessel_grid_sz+1][light_tessel_grid_sz+1];
	float								lgx,rgx,lgy,rgy;
	d3pnt								*pt;
	map_mesh_poly_light_type			*light;
	map_mesh_poly_tessel_vertex_type	*vl;

	light=&poly->light;
	vl=light->vertexes;

		// get tessel size, possible total of 64 triangles, 8 grid spots (max)

	xdist=poly->box.max.x-poly->box.min.x;
	zdist=poly->box.max.z-poly->box.min.z;

	xtot=xdist/grid_split_sz;
	if (xtot>light_tessel_grid_sz) xtot=light_tessel_grid_sz;
	if (xtot<=0) xtot=1;

	ztot=zdist/grid_split_sz;
	if (ztot>light_tessel_grid_sz) ztot=light_tessel_grid_sz;
	if (ztot<=0) ztot=1;

	xskip=xdist/xtot;
	zskip=zdist/ztot;

		// create the grid overlay

	for (x=0;x<=xtot;x++) {
	
		if (x==0) {
			grid_x[x]=poly->box.min.x-light_tessel_overlap_pixel;
		}
		else {
			if (x==xtot) {
				grid_x[x]=poly->box.max.x+light_tessel_overlap_pixel;
			}
			else {
				grid_x[x]=poly->box.min.x+(x*xskip);
			}
		}
	}

	for (z=0;z<=ztot;z++) {
		if (z==0) {
			grid_z[z]=poly->box.min.z-light_tessel_overlap_pixel;
		}
		else {
			if (z==ztot) {
				grid_z[z]=poly->box.max.z+light_tessel_overlap_pixel;
			}
			else {
				grid_z[z]=poly->box.min.z+(z*zskip);
			}
		}
	}

		// get vertexes for grid
		// and find uv extends

	lgx=rgx=0.0f;
	lgy=rgy=0.0f;

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

	vl_cnt=0;

	for (z=0;z<=ztot;z++) {
		for (x=0;x<=xtot;x++) {

			if (poly->box.flat) {
				y=poly->box.mid.y;
			}
			else {
				y=polygon_find_y(ptsz,px,py,pz,grid_x[x],grid_z[z]);
				if (y==-1) y=polygon_infinite_find_y(ptsz,px,py,pz,grid_x[x],grid_z[z]);
			}

			vl->x=grid_x[x];
			vl->y=y;
			vl->z=grid_z[z];

			vl->gx=lgx+(((rgx-lgx)*(float)(vl->x-poly->box.min.x))/(float)(poly->box.max.x-poly->box.min.x));
			vl->gy=lgy+(((rgy-lgy)*(float)(vl->z-poly->box.min.z))/(float)(poly->box.max.z-poly->box.min.z));

			idx[x][z]=vl_cnt;

			vl_cnt++;
			vl++;
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

	light->nvertex=vl_cnt;
	light->ntrig=ntrig;
}

/* =======================================================

      Simple Lighting Tessel
      
======================================================= */

void map_portal_add_light_simple_vertex_list(map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int							n,ntrig;
	map_mesh_poly_light_type	*light;

	light=&poly->light;

		// simple tessel just breaks up the polygon
		// into triangle.  We don't create any
		// lighting vertexes as we can re-use the
		// polygon vertexes

	ntrig=poly->ptsz-2;

	for (n=0;n<ntrig;n++) {
		light->trig_vertex_idx[n*3]=0;
		light->trig_vertex_idx[(n*3)+1]=n+1;
		light->trig_vertex_idx[(n*3)+2]=n+2;
	}

	light->nvertex=0;
	light->ntrig=ntrig;
}

/* =======================================================

      Create/Dispose Tesseled Lighting Vertexes For Polygon
      
======================================================= */

void map_create_poly_tesseled_vertexes(map_mesh_type *mesh,map_mesh_poly_type *poly,int quality_mode)
{
	int			grid_split_sz;

		// memory for tesseled vertexes

	poly->light.trig_vertex_idx=valloc(sizeof(int)*(light_tessel_max_vertex*3));			// supergumba -- need to error check here
	poly->light.trig_vertex_draw_idx=valloc(sizeof(int)*(light_tessel_max_vertex*3));
	poly->light.vertexes=valloc(sizeof(map_mesh_poly_tessel_vertex_type)*light_tessel_max_vertex);

	poly->light.ntrig=0;
	poly->light.nvertex=0;
	
		// simple light tessels

	if ((quality_mode==quality_mode_low) || (poly->draw.simple_tessel) || (mesh->flag.hilite)) {

		poly->light.simple_tessel=TRUE;
		map_portal_add_light_simple_vertex_list(mesh,poly);

		return;
	}

		// tessel depending on shape of polygon
	
	poly->light.simple_tessel=FALSE;

	grid_split_sz=(quality_mode==quality_mode_medium)?(map_enlarge*16):(map_enlarge*8);

	if (poly->box.wall_like) {
		map_portal_add_light_wall_tessel_vertex_list(mesh,poly,grid_split_sz);
	}
	else {
		map_portal_add_light_floor_tessel_vertex_list(mesh,poly,grid_split_sz);
	}
}

void map_dispose_poly_tesseled_vertexes(map_mesh_poly_type *poly)
{
	free(poly->light.trig_vertex_idx);
	free(poly->light.trig_vertex_draw_idx);
	free(poly->light.vertexes);
}

/* =======================================================

      Create/Dispose Mesh Vertex Lists
      
======================================================= */

bool map_create_mesh_vertexes(map_type *map,int quality_mode)
{
	int					n,k,nvlist;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
		// find maximum possible number of vertexes
		// that could be drawn in a scene
	
	nvlist=0;
	
	mesh=map->mesh.meshes;
		
	for (n=0;n!=map->mesh.nmesh;n++) {

		nvlist+=mesh->nvertex;
		nvlist+=(mesh->npoly*light_tessel_max_vertex);
		
		mesh++;
	}
	
		// no indexes

	map->mesh_vertexes.index_ptr=NULL;

		// remember total

	map->mesh_vertexes.max_vertex_count=nvlist;
	map->mesh_vertexes.draw_vertex_count=0;
	
		// create tesseled lighting vertexes

	mesh=map->mesh.meshes;
	
	for (n=0;n!=map->mesh.nmesh;n++) {

		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			map_create_poly_tesseled_vertexes(mesh,poly,quality_mode);
			poly++;
		}
		
		mesh++;
	}

	return(TRUE);
}

void map_dispose_mesh_vertexes(map_type *map)
{
	int					n,k;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
		// dispose tesseled lighting vertexes
		
	mesh=map->mesh.meshes;
	
	for (n=0;n!=map->mesh.nmesh;n++) {
	
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			map_dispose_poly_tesseled_vertexes(poly);
			poly++;
		}
		
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
	map->liquid_vertexes.index_ptr=(int*)valloc(sz);
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
	map->sort.list=(map_poly_sort_item_type*)valloc(sz);
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
