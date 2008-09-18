/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Meshes

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

      Add Meshes to Portals
      
======================================================= */

int map_mesh_add(map_type *map)
{
	int					mesh_idx;
	map_mesh_type		*mesh,*nptr;

		// create new memory

	if (map->mesh.nmesh==0) {
		mesh_idx=0;
		map->mesh.meshes=(map_mesh_type*)valloc(sizeof(map_mesh_type));
		if (map->mesh.meshes==NULL) return(FALSE);
	}
	else {
		mesh_idx=map->mesh.nmesh;

		nptr=(map_mesh_type*)valloc((map->mesh.nmesh+1)*sizeof(map_mesh_type));
		if (nptr==NULL) return(FALSE);

		memmove(nptr,map->mesh.meshes,(map->mesh.nmesh*sizeof(map_mesh_type)));
		free(map->mesh.meshes);

		map->mesh.meshes=nptr;
	}
	
	map->mesh.nmesh++;

		// setup meshes
	
	mesh=&map->mesh.meshes[mesh_idx];

	mesh->group_idx=-1;
		
	mesh->flag.on=TRUE;
	mesh->flag.pass_through=FALSE;
	mesh->flag.moveable=FALSE;
	mesh->flag.climbable=FALSE;
	mesh->flag.hilite=FALSE;
	mesh->flag.touched=FALSE;
	mesh->flag.lock_uv=FALSE;
	mesh->flag.no_self_obscure=FALSE;

	mesh->rot_off.x=mesh->rot_off.y=mesh->rot_off.z=0;
	
	mesh->nvertex=0;
	mesh->vertexes=NULL;

	mesh->npoly=0;
	mesh->polys=NULL;

	bzero(mesh->obscure.visibility_flag,max_mesh_visibility_bytes);

	return(mesh_idx);
}

bool map_mesh_delete(map_type *map,int mesh_idx)
{
	int					sz;
	map_mesh_type		*mesh,*nptr;

		// free the mesh data

	mesh=&map->mesh.meshes[mesh_idx];

	if (mesh->vertexes!=NULL) free(mesh->vertexes);
	if (mesh->polys!=NULL) free(mesh->polys);

		// detele the mesh

	if (map->mesh.nmesh<=1) {
		map->mesh.nmesh=0;
		free(map->mesh.meshes);
		map->mesh.meshes=NULL;
		return(TRUE);
	}

	nptr=(map_mesh_type*)valloc((map->mesh.nmesh-1)*sizeof(map_mesh_type));
	if (nptr==NULL) return(FALSE);

	if (mesh_idx>0) {
		sz=mesh_idx*sizeof(map_mesh_type);
		memmove(nptr,map->mesh.meshes,sz);
	}

	sz=((map->mesh.nmesh-mesh_idx)-1)*sizeof(map_mesh_type);
	if (sz>0) memmove(&nptr[mesh_idx],&map->mesh.meshes[mesh_idx+1],sz);

	free(map->mesh.meshes);

	map->mesh.meshes=nptr;
	map->mesh.nmesh--;

	return(TRUE);
}

/* =======================================================

      Change Mesh Vertex and Poly Counts
      
======================================================= */

bool map_mesh_set_vertex_count(map_type *map,int mesh_idx,int nvertex)
{
	map_mesh_type		*mesh;
	d3pnt				*nptr;
	
	mesh=&map->mesh.meshes[mesh_idx];
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

bool map_mesh_set_poly_count(map_type *map,int mesh_idx,int npoly)
{
	map_mesh_type		*mesh;
	map_mesh_poly_type	*nptr;

	mesh=&map->mesh.meshes[mesh_idx];
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

int map_mesh_duplicate(map_type *map,int mesh_idx)
{
	int					new_mesh_idx;
	map_mesh_type		*mesh,*new_mesh;
	
		// create new mesh
	
	new_mesh_idx=map_mesh_add(map);
	if (new_mesh_idx==-1) return(-1);

		// setup vertex and poly memory
		
	mesh=&map->mesh.meshes[mesh_idx];
	new_mesh=&map->mesh.meshes[new_mesh_idx];
	
	if (!map_mesh_set_vertex_count(map,new_mesh_idx,mesh->nvertex)) {
		map_mesh_delete(map,new_mesh_idx);
		return(-1);
	}
	
	if (!map_mesh_set_poly_count(map,new_mesh_idx,mesh->npoly)) {
		map_mesh_delete(map,new_mesh_idx);
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

int map_mesh_count_total_poly(map_type *map)
{
	int				n,cnt;
	map_mesh_type	*mesh;

	mesh=map->mesh.meshes;

	cnt=0;

	for (n=0;n!=map->mesh.nmesh;n++) {
		cnt+=mesh->npoly;
		mesh++;
	}

	return(cnt);
}

int map_mesh_add_vertex(map_type *map,int mesh_idx,d3pnt *pt)
{
	int				v_idx;
	map_mesh_type	*mesh;

	mesh=&map->mesh.meshes[mesh_idx];
	
	v_idx=mesh->nvertex;
	
	if (!map_mesh_set_vertex_count(map,mesh_idx,(mesh->nvertex+1))) return(-1);
	
	memmove(&mesh->vertexes[v_idx],pt,sizeof(d3pnt));
	
	return(v_idx);
}

int map_mesh_add_poly(map_type *map,int mesh_idx,int ptsz,int *x,int *y,int *z,float *gx,float *gy,int txt_idx)
{
	int					t,k,poly_idx,v_idx;
	d3pnt				*pt,chk_pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=&map->mesh.meshes[mesh_idx];

		// create new poly
		
	poly_idx=mesh->npoly;
	if (!map_mesh_set_poly_count(map,mesh_idx,(mesh->npoly+1))) return(-1);
	
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
			v_idx=map_mesh_add_vertex(map,mesh_idx,&chk_pt);
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

bool map_mesh_delete_poly(map_type *map,int mesh_idx,int poly_idx)
{
	int					n,k,t,sz,ptsz,v_idx,del_idx[8];
	bool				del_ok[8];
	d3pnt				*nvertex_ptr;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly,*chk_poly,*nmesh_ptr;

	mesh=&map->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
	ptsz=poly->ptsz;

		// mark all vertexes only owned
		// by this poly and delete

	for (k=0;k!=ptsz;k++) {

		del_idx[k]=poly->v[k];
		del_ok[k]=TRUE;

		chk_poly=mesh->polys;

		for (n=0;n!=mesh->npoly;n++) {
			
			if (n!=poly_idx) {
				for (t=0;t!=chk_poly->ptsz;t++) {
					if (chk_poly->v[t]==del_idx[k]) {
						del_ok[k]=FALSE;
						break;
					}
				}
			}
			
			if (!del_ok[k]) break;

			chk_poly++;
		}
	}

		// remove the polygon

	if (mesh->npoly<=1) {
		free(mesh->polys);
		mesh->npoly=0;
		mesh->polys=NULL;
	}
	else {
		nmesh_ptr=(map_mesh_poly_type*)valloc((mesh->npoly-1)*sizeof(map_mesh_poly_type));
		if (nmesh_ptr==NULL) return(FALSE);

		if (poly_idx>0) {
			sz=poly_idx*sizeof(map_mesh_poly_type);
			memmove(nmesh_ptr,mesh->polys,sz);
		}

		sz=((mesh->npoly-poly_idx)-1)*sizeof(map_mesh_poly_type);
		if (sz>0) memmove(&nmesh_ptr[poly_idx],&mesh->polys[poly_idx+1],sz);

		free(mesh->polys);

		mesh->polys=nmesh_ptr;
		mesh->npoly--;
	}

		// remove the vertexes

	for (k=0;k!=ptsz;k++) {
		if (!del_ok[k]) continue;

		v_idx=del_idx[k];

			// fix all vertex indexes

		chk_poly=mesh->polys;

		for (n=0;n!=mesh->npoly;n++) {
			for (t=0;t!=chk_poly->ptsz;t++) {
				if (chk_poly->v[t]>v_idx) chk_poly->v[t]--;
			}
			chk_poly++;
		}
		
			// fix other deleted vertexes
			
		for (n=(k+1);n<ptsz;n++) {
			if (del_ok[n]) {
				if (del_idx[n]>v_idx) del_idx[n]--;
			}
		}

			// delete vertex

		if (mesh->nvertex<=1) {
			free(mesh->vertexes);
			mesh->nvertex=0;
			mesh->vertexes=NULL;
			break;
		}

		nvertex_ptr=(d3pnt*)valloc((mesh->nvertex-1)*sizeof(d3pnt));
		if (nvertex_ptr==NULL) return(FALSE);

		if (v_idx>0) {
			sz=v_idx*sizeof(d3pnt);
			memmove(nvertex_ptr,mesh->vertexes,sz);
		}

		sz=((mesh->nvertex-v_idx)-1)*sizeof(d3pnt);
		if (sz>0) memmove(&nvertex_ptr[v_idx],&mesh->vertexes[v_idx+1],sz);

		free(mesh->vertexes);

		mesh->vertexes=nvertex_ptr;
		mesh->nvertex--;
	}

	return(TRUE);
}

/* =======================================================

      Delete Unused Vertexes
      
======================================================= */

bool map_mesh_delete_unused_vertexes(map_type *map,int mesh_idx)
{
	int					n,k,t,sz;
	bool				attach;
	d3pnt				*nvertex_ptr;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=&map->mesh.meshes[mesh_idx];
	
		// find all unconnected vertexes
		
	n=0;
	
	while (n<mesh->nvertex) {
	
			// attached to any polygon?
			
		attach=FALSE;
		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			
			for (t=0;t!=poly->ptsz;t++) {
				if (n==poly->v[t]) {
					attach=TRUE;
					break;
				}
			}
			
			if (attach) break;
			
			poly++;
		}
		
		if (attach) {
			n++;
			continue;
		}
		
			// change poly indexes
		
		poly=mesh->polys;

		for (k=0;k!=mesh->npoly;k++) {
			
			for (t=0;t!=poly->ptsz;t++) {
				if (n<poly->v[t]) poly->v[t]--;
			}
			
			poly++;
		}
		
			// move vertexes
		
		nvertex_ptr=(d3pnt*)valloc((mesh->nvertex-1)*sizeof(d3pnt));
		if (nvertex_ptr==NULL) return(FALSE);

		if (n>0) {
			sz=n*sizeof(d3pnt);
			memmove(nvertex_ptr,mesh->vertexes,sz);
		}

		sz=((mesh->nvertex-n)-1)*sizeof(d3pnt);
		if (sz>0) memmove(&nvertex_ptr[n],&mesh->vertexes[n+1],sz);

		free(mesh->vertexes);

		mesh->vertexes=nvertex_ptr;
		mesh->nvertex--;
	}
	
	return(TRUE);
}

/* =======================================================

      Get Mesh Center
      
======================================================= */

void map_mesh_calculate_extent(map_type *map,int mesh_idx,d3pnt *min,d3pnt *max)
{
	int					n,nvertex;
	d3pnt				*pt;
	map_mesh_type		*mesh;

	mesh=&map->mesh.meshes[mesh_idx];
	
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

void map_mesh_calculate_center(map_type *map,int mesh_idx,d3pnt *mpt)
{
	int					n,nvertex,mx,my,mz;
	d3pnt				*pt;
	map_mesh_type		*mesh;

	mesh=&map->mesh.meshes[mesh_idx];

	mx=my=mz=0;

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		mx+=pt->x;
		my+=pt->y;
		mz+=pt->z;
		pt++;
	}

	mpt->x=mx/nvertex;
	mpt->y=my/nvertex;
	mpt->z=mz/nvertex;
}

void map_mesh_calculate_uv_center(map_type *map,int mesh_idx,float *gx,float *gy)
{
	int					n,k,cnt;
	float				kx,ky,f_cnt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=&map->mesh.meshes[mesh_idx];

	kx=ky=0.0f;
	cnt=0;

	poly=mesh->polys;

	for (n=0;n!=mesh->npoly;n++) {

		for (k=0;k!=poly->ptsz;k++) {
			kx+=poly->gx[k];
			ky+=poly->gy[k];
		}

		cnt+=poly->ptsz;

		poly++;
	}

	f_cnt=(float)cnt;

	*gx=kx/f_cnt;
	*gy=ky/f_cnt;
}

/* =======================================================

      Find Mesh
      
======================================================= */

// supergumba -- subtitute and test
/*
int map_find_mesh(map_type *map,d3pnt *pnt)
{
	int				n,d,dist,mesh_idx;
	map_mesh_type	*mesh;

	mesh_idx=-1;

		// look for meshes we are inside of

	dist=map_max_size;

	for (n=0;n!=map->mesh.nmesh;n++) {

			// are we within this mesh?

		mesh=&map->mesh.meshes[n];
		if ((pnt->x<mesh->box.min.x) || (pnt->x>mesh->box.max.x) || (pnt->y<mesh->box.min.y) || (pnt->y>mesh->box.max.y) || (pnt->z<mesh->box.min.z) || (pnt->z>mesh->box.max.z)) continue;

			// might be within multiple meshes, check distances to find best mesh

		d=distance_get(mesh->box.mid.x,mesh->box.mid.y,mesh->box.mid.z,pnt->x,pnt->y,pnt->z);
		if ((mesh_idx==-1) || (d<dist)) {
			dist=d;
			mesh_idx=n;
		}
	}

	if (mesh_idx!=-1) return(mesh_idx);

		// if inside no meshes, then closest mesh

	mesh=map->mesh.meshes;
	dist=map_max_size;

	for (n=0;n!=map->mesh.nmesh;n++) {

		d=distance_get(mesh->box.mid.x,mesh->box.mid.y,mesh->box.mid.z,pnt->x,pnt->y,pnt->z);
		if (d<dist) {
			dist=d;
			mesh_idx=n;
		}

		mesh++;
	}

	if (mesh_idx==-1) return(0);

	return(mesh_idx);
}

int map_calculate_mesh_distance(map_mesh_type *mesh,d3pnt *pnt)
{
	int			d,d2;
	
	d=distance_get(mesh->box.mid.x,mesh->box.mid.y,mesh->box.mid.z,pnt->x,pnt->y,pnt->z);

	d2=distance_get(mesh->box.min.x,mesh->box.mid.y,mesh->box.min.z,pnt->x,pnt->y,pnt->z);
	if (d2<d) d=d2;
	
	d2=distance_get(mesh->box.min.x,mesh->box.mid.y,mesh->box.max.z,pnt->x,pnt->y,pnt->z);
	if (d2<d) d=d2;
	
	d2=distance_get(mesh->box.max.x,mesh->box.mid.y,mesh->box.min.z,pnt->x,pnt->y,pnt->z);
	if (d2<d) d=d2;
	
	d2=distance_get(mesh->box.max.x,mesh->box.mid.y,mesh->box.max.z,pnt->x,pnt->y,pnt->z);
	if (d2<d) d=d2;
	
	return(d);
}
*/

int map_find_mesh(map_type *map,d3pnt *pnt)
{
	int				n,mesh_idx;
	double			d,dx,dy,dz,dist;
	map_mesh_type	*mesh;

	mesh_idx=-1;

		// look for meshes we are inside of

	dist=0.0;

	for (n=0;n!=map->mesh.nmesh;n++) {

			// are we within this mesh?

		mesh=&map->mesh.meshes[n];
		if ((pnt->x<mesh->box.min.x) || (pnt->x>mesh->box.max.x) || (pnt->y<mesh->box.min.y) || (pnt->y>mesh->box.max.y) || (pnt->z<mesh->box.min.z) || (pnt->z>mesh->box.max.z)) continue;

			// might be within multiple meshes, check distances to find best mesh
			// don't need to do the sqrt since we are only checking against other
			// distances

		dx=(double)(mesh->box.mid.x-pnt->x);
		dy=(double)(mesh->box.mid.y-pnt->y);
		dz=(double)(mesh->box.mid.z-pnt->z);
		d=(dx*dx)+(dy*dy)+(dz*dz);
			
		if ((mesh_idx==-1) || (d<dist)) {
			dist=d;
			mesh_idx=n;
		}
	}

	if (mesh_idx!=-1) return(mesh_idx);

		// if inside no meshes, then closest mesh

	mesh=map->mesh.meshes;
	dist=0.0;

	for (n=0;n!=map->mesh.nmesh;n++) {

		dx=(double)(mesh->box.mid.x-pnt->x);
		dy=(double)(mesh->box.mid.y-pnt->y);
		dz=(double)(mesh->box.mid.z-pnt->z);
		d=(dx*dx)+(dy*dy)+(dz*dz);

		if ((n==0) || (d<dist)) {
			dist=d;
			mesh_idx=n;
		}

		mesh++;
	}

	if (mesh_idx==-1) return(0);

	return(mesh_idx);
}

int map_calculate_mesh_distance(map_mesh_type *mesh,d3pnt *pnt)
{
	double			d,d2,dx,dy,dz;
	
		// check middle

	dx=(double)(mesh->box.mid.x-pnt->x);
	dy=(double)(mesh->box.mid.y-pnt->y);
	dz=(double)(mesh->box.mid.z-pnt->z);
	d=(dx*dx)+(dy*dy)+(dz*dz);

		// and the 8 corners

	dx=(double)(mesh->box.min.x-pnt->x);
	dy=(double)(mesh->box.min.y-pnt->y);
	dz=(double)(mesh->box.min.z-pnt->z);
	d2=(dx*dx)+(dy*dy)+(dz*dz);
	if (d2<d) d=d2;

	dx=(double)(mesh->box.max.x-pnt->x);
	dy=(double)(mesh->box.min.y-pnt->y);
	dz=(double)(mesh->box.min.z-pnt->z);
	d2=(dx*dx)+(dy*dy)+(dz*dz);
	if (d2<d) d=d2;

	dx=(double)(mesh->box.max.x-pnt->x);
	dy=(double)(mesh->box.min.y-pnt->y);
	dz=(double)(mesh->box.max.z-pnt->z);
	d2=(dx*dx)+(dy*dy)+(dz*dz);
	if (d2<d) d=d2;

	dx=(double)(mesh->box.min.x-pnt->x);
	dy=(double)(mesh->box.min.y-pnt->y);
	dz=(double)(mesh->box.max.z-pnt->z);
	d2=(dx*dx)+(dy*dy)+(dz*dz);
	if (d2<d) d=d2;

	dx=(double)(mesh->box.min.x-pnt->x);
	dy=(double)(mesh->box.max.y-pnt->y);
	dz=(double)(mesh->box.min.z-pnt->z);
	d2=(dx*dx)+(dy*dy)+(dz*dz);
	if (d2<d) d=d2;

	dx=(double)(mesh->box.max.x-pnt->x);
	dy=(double)(mesh->box.max.y-pnt->y);
	dz=(double)(mesh->box.min.z-pnt->z);
	d2=(dx*dx)+(dy*dy)+(dz*dz);
	if (d2<d) d=d2;

	dx=(double)(mesh->box.max.x-pnt->x);
	dy=(double)(mesh->box.max.y-pnt->y);
	dz=(double)(mesh->box.max.z-pnt->z);
	d2=(dx*dx)+(dy*dy)+(dz*dz);
	if (d2<d) d=d2;

	dx=(double)(mesh->box.min.x-pnt->x);
	dy=(double)(mesh->box.max.y-pnt->y);
	dz=(double)(mesh->box.max.z-pnt->z);
	d2=(dx*dx)+(dy*dy)+(dz*dz);
	if (d2<d) d=d2;
	
	return((int)sqrt(d));
}


