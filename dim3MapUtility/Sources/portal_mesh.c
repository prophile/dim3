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
	portal_type			*portal;
	map_mesh_type		*mesh,*nptr;
	
	portal=&map->portals[portal_idx];

		// create new memory

	if (portal->mesh.nmesh==0) {
		mesh_idx=0;
		portal->mesh.meshes=(map_mesh_type*)valloc(sizeof(map_mesh_type));
		if (portal->mesh.meshes==NULL) return(FALSE);
	}
	else {
		mesh_idx=portal->mesh.nmesh;

		nptr=(map_mesh_type*)valloc((portal->mesh.nmesh+1)*sizeof(map_mesh_type));
		if (nptr==NULL) return(FALSE);

		memmove(nptr,portal->mesh.meshes,(portal->mesh.nmesh*sizeof(map_mesh_type)));
		free(portal->mesh.meshes);

		portal->mesh.meshes=nptr;
	}
	
	portal->mesh.nmesh++;

		// setup meshes
	
	mesh=&portal->mesh.meshes[mesh_idx];

	mesh->group_idx=-1;
		
	mesh->flag.on=TRUE;
	mesh->flag.pass_through=FALSE;
	mesh->flag.moveable=FALSE;
	mesh->flag.climbable=FALSE;
	mesh->flag.touched=FALSE;

	mesh->nvertex=0;
	mesh->vertexes=NULL;

	mesh->npoly=0;
	mesh->polys=NULL;

	return(mesh_idx);
}

bool map_portal_mesh_delete(map_type *map,int portal_idx,int mesh_idx)
{
	int					sz;
	portal_type			*portal;
	map_mesh_type		*mesh,*nptr;
	
	portal=&map->portals[portal_idx];

		// free the mesh data

	mesh=&portal->mesh.meshes[mesh_idx];

	if (mesh->vertexes!=NULL) free(mesh->vertexes);
	if (mesh->polys!=NULL) free(mesh->polys);

		// detele the mesh

	if (portal->mesh.nmesh<=1) {
		portal->mesh.nmesh=0;
		free(portal->mesh.meshes);
		portal->mesh.meshes=NULL;
		return(TRUE);
	}

	nptr=(map_mesh_type*)valloc((portal->mesh.nmesh-1)*sizeof(map_mesh_type));
	if (nptr==NULL) return(FALSE);

	if (mesh_idx>0) {
		sz=(mesh_idx+1)*sizeof(map_mesh_type);
		memmove(nptr,portal->mesh.meshes,sz);
	}

	sz=(portal->mesh.nmesh-mesh_idx)*sizeof(map_mesh_type);
	if (sz>0) memmove(&nptr[mesh_idx],&portal->mesh.meshes[mesh_idx+1],sz);

	free(portal->mesh.meshes);

	portal->mesh.meshes=nptr;
	portal->mesh.nmesh--;

	return(TRUE);
}

/* =======================================================

      Change Mesh Vertex and Poly Counts
      
======================================================= */

bool map_portal_mesh_set_vertex_count(map_type *map,int portal_idx,int mesh_idx,int nvertex)
{
	map_mesh_type		*mesh;
	d3pnt				*nptr;
	
	mesh=&map->portals[portal_idx].mesh.meshes[mesh_idx];
	if (mesh->nvertex==nvertex) return(TRUE);

		// new memory

	nptr=(d3pnt*)valloc(nvertex*sizeof(d3pnt));
	if (nptr==NULL) return(FALSE);

		// move and delete old vertexes

	if (mesh->vertexes!=NULL) {
		if (mesh->nvertex!=0) {
			memmove(nptr,mesh->vertexes,(mesh->nvertex*sizeof(d3pnt)));
		}
		free(mesh->vertexes);
	}

		// finish setup

	mesh->vertexes=nptr;
	mesh->nvertex=nvertex;

	return(TRUE);
}

bool map_portal_mesh_set_poly_count(map_type *map,int portal_idx,int mesh_idx,int npoly)
{
	map_mesh_type		*mesh;
	map_mesh_poly_type	*nptr;

	mesh=&map->portals[portal_idx].mesh.meshes[mesh_idx];
	if (mesh->npoly==npoly) return(TRUE);

		// new memory

	nptr=(map_mesh_poly_type*)valloc(npoly*sizeof(map_mesh_poly_type));
	if (nptr==NULL) return(FALSE);

		// move and delete old polygons

	if (mesh->polys!=NULL) {
		if (mesh->npoly!=0) {
			memmove(nptr,mesh->polys,(mesh->npoly*sizeof(map_mesh_poly_type)));
		}
		free(mesh->polys);
	}

		// finish setup

	mesh->polys=nptr;
	mesh->npoly=npoly;

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
	
		// create new mesh
	
	portal=&map->portals[portal_idx];
		
	new_mesh_idx=map_portal_mesh_add(map,portal_idx);
	if (new_mesh_idx==-1) return(-1);

		// setup vertex and poly memory
		
	mesh=&portal->mesh.meshes[mesh_idx];
	new_mesh=&portal->mesh.meshes[new_mesh_idx];
	
	if (!map_portal_mesh_set_vertex_count(map,portal_idx,new_mesh_idx,mesh->nvertex)) {
		map_portal_mesh_delete(map,portal_idx,new_mesh_idx);
		return(-1);
	}
	
	if (!map_portal_mesh_set_poly_count(map,portal_idx,new_mesh_idx,mesh->npoly)) {
		map_portal_mesh_delete(map,portal_idx,new_mesh_idx);
		return(-1);
	}
	
		// mesh setup
		
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

	return(poly_idx);
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

