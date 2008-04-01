/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Portal Meshes

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

      Add Meshes to Portals
      
======================================================= */

int map_portal_mesh_add(map_type *map,int portal_idx)
{
	int					mesh_idx;
	portal_mesh_type	*portal_mesh;
	map_mesh_type		*map_mesh,*nptr;
	
	portal_mesh=&map->portals[portal_idx].mesh;

		// create new memory

	if (portal_mesh->nmesh==0) {
		mesh_idx=0;
		portal_mesh->meshes=(map_mesh_type*)valloc(sizeof(map_mesh_type));
		if (portal_mesh->meshes==NULL) return(FALSE);
	}
	else {
		mesh_idx=portal_mesh->nmesh;

		nptr=(map_mesh_type*)valloc((portal_mesh->nmesh+1)*sizeof(map_mesh_type));
		if (nptr==NULL) return(FALSE);

		memmove(nptr,portal_mesh->meshes,(portal_mesh->nmesh*sizeof(map_mesh_type)));
		free(portal_mesh->meshes);

		portal_mesh->meshes=nptr;
	}
	
	portal_mesh->nmesh++;

		// setup meshes
	
	map_mesh=&portal_mesh->meshes[mesh_idx];

	map_mesh->nvertex=map_mesh->npoly=0;
	map_mesh->group_idx=-1;
		
	map_mesh->flag.on=TRUE;
	map_mesh->flag.pass_through=FALSE;
	map_mesh->flag.moveable=FALSE;
	map_mesh->flag.climbable=FALSE;
	map_mesh->flag.touched=FALSE;

	map_mesh->vertexes=NULL;
	map_mesh->polys=NULL;

	return(mesh_idx);
}

bool map_portal_mesh_delete(map_type *map,int portal_idx,int mesh_idx)
{
	int					sz;
	portal_mesh_type	*portal_mesh;
	map_mesh_type		*map_mesh,*nptr;
	
	portal_mesh=&map->portals[portal_idx].mesh;

		// free the mesh data

	map_mesh=&portal_mesh->meshes[mesh_idx];

	if (map_mesh->vertexes!=NULL) free(map_mesh->vertexes);
	if (map_mesh->polys!=NULL) free(map_mesh->polys);

		// detele the mesh

	if (portal_mesh->nmesh<=1) {
		portal_mesh->nmesh=0;
		free(portal_mesh->meshes);
		return(TRUE);
	}

	nptr=(map_mesh_type*)valloc((portal_mesh->nmesh-1)*sizeof(map_mesh_type));
	if (nptr==NULL) return(FALSE);

	if (mesh_idx>0) {
		sz=(mesh_idx+1)*sizeof(map_mesh_type);
		memmove(nptr,portal_mesh->meshes,sz);
	}

	sz=(portal_mesh->nmesh-mesh_idx)*sizeof(map_mesh_type);
	if (sz>0) memmove(&nptr[mesh_idx],&portal_mesh->meshes[mesh_idx+1],sz);

	free(portal_mesh->meshes);

	portal_mesh->meshes=nptr;
	portal_mesh->nmesh--;

	return(TRUE);
}

/* =======================================================

      Change Mesh Vertex and Poly Counts
      
======================================================= */

bool map_portal_mesh_set_vertex_count(map_type *map,int portal_idx,int mesh_idx,int nvertex)
{
	map_mesh_type		*map_mesh;
	d3pnt				*nptr;
	
	map_mesh=&map->portals[portal_idx].mesh.meshes[mesh_idx];

		// new memory

	nptr=(d3pnt*)valloc(nvertex*sizeof(d3pnt));
	if (nptr==NULL) return(FALSE);

		// move and delete old vertexes

	if (map_mesh->vertexes!=NULL) {
		if (map_mesh->nvertex!=0) {
			memmove(nptr,map_mesh->vertexes,(map_mesh->nvertex*sizeof(d3pnt)));
		}
		free(map_mesh->vertexes);
	}

		// finish setup

	map_mesh->vertexes=nptr;
	map_mesh->nvertex=nvertex;

	return(TRUE);
}

bool map_portal_mesh_set_poly_count(map_type *map,int portal_idx,int mesh_idx,int npoly)
{
	map_mesh_type		*map_mesh;
	map_mesh_poly_type	*nptr;

	map_mesh=&map->portals[portal_idx].mesh.meshes[mesh_idx];

		// new memory

	nptr=(map_mesh_poly_type*)valloc(npoly*sizeof(map_mesh_poly_type));
	if (nptr==NULL) return(FALSE);

		// move and delete old polygons

	if (map_mesh->polys!=NULL) {
		if (map_mesh->npoly!=0) {
			memmove(nptr,map_mesh->polys,(map_mesh->npoly*sizeof(map_mesh_poly_type)));
		}
		free(map_mesh->polys);
	}

		// finish setup

	map_mesh->polys=nptr;
	map_mesh->npoly=npoly;

	return(TRUE);
}

/* =======================================================

      Duplicate Mesh
      
======================================================= */

int map_portal_mesh_duplicate(map_type *map,int portal_idx,int mesh_idx)
{
	int					new_mesh_idx;
	portal_type			*portal;
	map_mesh_type		*mesh,*new_mesh;
	
		// original mesh
	
	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
	
		// new mesh
		
	new_mesh_idx=map_portal_mesh_add(map,portal_idx);
	if (new_mesh_idx==-1) return(-1);
	
	if (!map_portal_mesh_set_vertex_count(map,portal_idx,new_mesh_idx,mesh->nvertex)) {
		map_portal_mesh_delete(map,portal_idx,new_mesh_idx);
		return(-1);
	}
	
	if (!map_portal_mesh_set_poly_count(map,portal_idx,new_mesh_idx,mesh->npoly)) {
		map_portal_mesh_delete(map,portal_idx,new_mesh_idx);
		return(-1);
	}
	
		// mesh setup
		
	new_mesh=&portal->mesh.meshes[new_mesh_idx];

	new_mesh->group_idx=mesh->group_idx;
	memmove(&new_mesh->flag,&mesh->flag,sizeof(map_mesh_flag_type));

	memmove(new_mesh->vertexes,mesh->vertexes,(sizeof(d3pnt)*mesh->nvertex));
	memmove(new_mesh->polys,mesh->polys,(sizeof(map_mesh_poly_type)*mesh->npoly));

	return(new_mesh_idx);
}

/* =======================================================

      Mesh Vertex/Poly Utilities
      
======================================================= */

int map_portal_mesh_count_total_poly(map_type *map,int portal_idx)
{
	int				n,cnt;
	portal_type		*portal;
	map_mesh_type	*mesh;

	portal=&map->portals[portal_idx];
	mesh=portal->mesh.meshes;

	cnt=0;

	for (n=0;n!=portal->mesh.nmesh;n++) {
		cnt+=mesh->npoly;
		mesh++;
	}

	return(cnt);
}

int map_portal_mesh_add_vertex(map_type *map,int portal_idx,int mesh_idx,d3pnt *pt)
{
	int				v_idx;
	portal_type		*portal;
	map_mesh_type	*mesh;

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
	
	v_idx=mesh->nvertex;
	
	if (!map_portal_mesh_set_vertex_count(map,portal_idx,mesh_idx,(mesh->nvertex+1))) return(-1);
	
	memmove(&mesh->vertexes[v_idx],pt,sizeof(d3pnt));
	
	return(v_idx);
}

int map_portal_mesh_add_poly(map_type *map,int portal_idx,int mesh_idx,int ptsz,int *x,int *y,int *z,float *gx,float *gy,int txt_idx)
{
	int					t,k,poly_idx,v_idx;
	d3pnt				*pt,chk_pt;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];

		// create new poly
		
	poly_idx=mesh->npoly;
	if (!map_portal_mesh_set_poly_count(map,portal_idx,mesh_idx,(mesh->npoly+1))) return(-1);
	
	poly=&mesh->polys[poly_idx];
	
		// add in the vertexes, checking for combines
		
	for (t=0;t!=ptsz;t++) {
		chk_pt.x=x[t];
		chk_pt.y=y[t];
		chk_pt.z=z[t];
		
			// this vertex already in mesh?
		
		v_idx=-1;
		pt=mesh->vertexes;

		for (k=0;k!=mesh->nvertex;k++) {
			if ((pt->x==chk_pt.x) && (pt->y==chk_pt.y) && (pt->z==chk_pt.z)) {
				v_idx=k;
				break;
			}
			
			pt++;
		}

			// need to add new vertex
			
		if (v_idx==-1) {
			v_idx=map_portal_mesh_add_vertex(map,portal_idx,mesh_idx,&chk_pt);
			if (v_idx==-1) {
				mesh->npoly--;
				return(-1);
			}
		}

		poly->v[t]=v_idx;
		
			// add in UV coords
			
		poly->gx[t]=gx[t];
		poly->gy[t]=gy[t];
	}
	
		// finish up
		
	poly->ptsz=ptsz;
	poly->txt_idx=txt_idx;
	
	poly->x_shift=poly->y_shift=0.0f;
	poly->dark_factor=1.0f;
	poly->alpha=1.0f;

	return(-1);
}

/* =======================================================

      Mesh Transparency Sorting Lists
      
======================================================= */

bool map_portal_mesh_create_transparent_sort_lists(map_type *map)
{
	int				n,npoly;
	portal_type		*portal;

		// clear out pointers

	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
		portal->mesh.draw.sort_cnt=0;
		portal->mesh.draw.sort_list=NULL;
		portal++;
	}
	
		// create sort lists

	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
		npoly=map_portal_mesh_count_total_poly(map,n);

		portal->mesh.draw.sort_list=(map_mesh_poly_sort_type*)valloc(npoly*sizeof(map_mesh_poly_sort_type));
		if (portal->mesh.draw.sort_list==NULL) return(FALSE);

		portal++;
	}
	
	return(TRUE);
}

void map_portal_mesh_dispose_transparent_sort_lists(map_type *map)
{
	int				n;
	portal_type		*portal;
	
	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
		if (portal->mesh.draw.sort_list!=NULL) free(portal->mesh.draw.sort_list);
		portal++;
	}
}

/* =======================================================

      Get Mesh Center
      
======================================================= */

void map_portal_mesh_calculate_extent(map_type *map,int portal_idx,int mesh_idx,d3pnt *min,d3pnt *max)
{
	int					n,nvertex;
	d3pnt				*pt;
	portal_type			*portal;
	map_mesh_type		*mesh;

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
	
	nvertex=mesh->nvertex;
	
	if (nvertex==0) {
		min->x=min->y=min->z=0;
		max->x=max->y=max->z=0;
		return;
	}
	
	pt=mesh->vertexes;
	
	min->x=max->x=pt->x;
	min->y=max->y=pt->y;
	min->z=max->z=pt->z;
	
	if (nvertex==1) return;

	pt++;

	for (n=1;n!=nvertex;n++) {
		if (pt->x<min->x) min->x=pt->x;
		if (pt->x>max->x) max->x=pt->x;
		if (pt->y<min->y) min->y=pt->y;
		if (pt->y>max->y) max->y=pt->y;
		if (pt->z<min->z) min->z=pt->z;
		if (pt->z>max->z) max->z=pt->z;
		
		pt++;
	}
}

void map_portal_mesh_calculate_center(map_type *map,int portal_idx,int mesh_idx,int *x,int *y,int *z)
{
	int					n,nvertex,mx,my,mz;
	d3pnt				*pt;
	portal_type			*portal;
	map_mesh_type		*mesh;

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];

	mx=my=mz=0;

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		mx+=pt->x;
		my+=pt->y;
		mz+=pt->z;
		pt++;
	}

	*x=mx/nvertex;
	*y=my/nvertex;
	*z=mz/nvertex;
}

