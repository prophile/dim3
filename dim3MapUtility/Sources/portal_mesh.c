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

bool map_portal_mesh_add(map_type *map,int portal_idx,int add_count)
{
	int					n,start_idx,end_idx;
	portal_mesh_type	*portal_mesh;
	map_mesh_type		*map_mesh,*nptr;
	
	portal_mesh=&map->portals[portal_idx].mesh;

		// create new memory

	if (portal_mesh->nmesh==0) {
		start_idx=0;
		portal_mesh->meshes=(map_mesh_type*)valloc(add_count*sizeof(map_mesh_type));
		if (portal_mesh->meshes==NULL) return(FALSE);
	}
	else {
		start_idx=portal_mesh->nmesh;

		nptr=(map_mesh_type*)valloc((portal_mesh->nmesh+add_count)*sizeof(map_mesh_type));
		if (nptr==NULL) return(FALSE);

		memmove(nptr,portal_mesh->meshes,(portal_mesh->nmesh*sizeof(map_mesh_type)));
		free(portal_mesh->meshes);

		portal_mesh->meshes=nptr;
	}

		// setup meshes
	
	map_mesh=&portal_mesh->meshes[start_idx];
	end_idx=start_idx+add_count;

	for (n=start_idx;n!=end_idx;n++) {
		map_mesh->nvertex=map_mesh->npoly=0;
		map_mesh->vertexes=NULL;
		map_mesh->polys=NULL;
		map_mesh++;
	}

	return(TRUE);
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


