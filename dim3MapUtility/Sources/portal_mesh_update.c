/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Portal Mesh Updates

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

      Combine Meshes
      
======================================================= */

void map_portal_mesh_combine_single_mesh(map_mesh_type *mesh,map_mesh_type *mesh_copy)
{
	int					n,t,k,v_idx;
	d3pnt				*d3pt,*chk_d3pt;
	map_mesh_poly_type	*mesh_poly,*mesh_copy_poly;
	
		// get place we last stopped adding
		
	mesh_poly=&mesh->polys[mesh->npoly];
	mesh_copy_poly=mesh_copy->polys;
	
	for (n=0;n!=mesh_copy->npoly;n++) {
	
			// add the poly
			
		memmove(mesh_poly,mesh_copy_poly,sizeof(map_mesh_poly_type));
	
			// add in the vertexes, checking for combines
			
		for (t=0;t!=mesh_copy_poly->ptsz;t++) {
			chk_d3pt=&mesh_copy->vertexes[mesh_copy_poly->v[t]];
			
			v_idx=-1;
			d3pt=mesh->vertexes;

			for (k=0;k!=mesh->nvertex;k++) {
				if ((d3pt->x==chk_d3pt->x) && (d3pt->y==chk_d3pt->y) && (d3pt->z==chk_d3pt->z)) {
					v_idx=k;
					break;
				}
				
				d3pt++;
			}

				// need to add new vertex
				
			if (v_idx==-1) {
				v_idx=mesh->nvertex;
				mesh->nvertex++;
				
				d3pt=&mesh->vertexes[v_idx];
				d3pt->x=chk_d3pt->x;
				d3pt->y=chk_d3pt->y;
				d3pt->z=chk_d3pt->z;
			}

			mesh_poly->v[t]=v_idx;
		}
		
		mesh->npoly++;
		
		mesh_poly++;
		mesh_copy_poly++;
	}
}

int map_portal_mesh_combine(map_type *map,int portal_idx,int mesh_1_idx,int mesh_2_idx)
{
	int					mesh_idx;
	portal_type			*portal;
	map_mesh_type		*mesh,*mesh_1,*mesh_2;
	
		// get mesh portal
		
	portal=&map->portals[portal_idx];
	
		// create new combined mesh
		
	mesh_idx=portal->mesh.nmesh;
	if (!map_portal_mesh_add(map,portal_idx,1)) return(-1);

		// get combined meshes
		// need to get after mesh add as mesh add can change mesh pointers
		
	mesh_1=&portal->mesh.meshes[mesh_1_idx];
	mesh_2=&portal->mesh.meshes[mesh_2_idx];
	
		// setup enough vertexes and polys for new mesh
		
	if (!map_portal_mesh_set_vertex_count(map,portal_idx,mesh_idx,(mesh_1->nvertex+mesh_2->nvertex))) {
		map_portal_mesh_delete(map,portal_idx,mesh_idx);
		return(-1);
	}
	
	if (!map_portal_mesh_set_poly_count(map,portal_idx,mesh_idx,(mesh_1->npoly+mesh_2->npoly))) {
		map_portal_mesh_delete(map,portal_idx,mesh_idx);
		return(-1);
	}

		// combined meshes
		
	mesh=&portal->mesh.meshes[mesh_idx];

	mesh->npoly=0;
	mesh->nvertex=0;
		
	map_portal_mesh_combine_single_mesh(mesh,mesh_1);
	map_portal_mesh_combine_single_mesh(mesh,mesh_2);
	
		// get back to correct size
		// ignore failures as it's just a waste of space that
		// will be reclaimed later
		
	map_portal_mesh_set_vertex_count(map,portal_idx,mesh_idx,mesh->nvertex);
	map_portal_mesh_set_poly_count(map,portal_idx,mesh_idx,mesh->npoly);
	
		// delete original meshes, making sure to
		// change delete index depending on first delete
		// and return mesh index minus the two
		// deleted meshes
		
	map_portal_mesh_delete(map,portal_idx,mesh_1_idx);
	
	if (mesh_1_idx<mesh_2_idx) mesh_2_idx--;
	map_portal_mesh_delete(map,portal_idx,mesh_2_idx);	

	return(mesh_idx-2);
}

/* =======================================================

      Move Mesh
      
======================================================= */

void map_portal_mesh_move(map_type *map,int portal_idx,int mesh_idx,bool do_portal_vertex_list,int x,int y,int z)
{
	int						n,k,idx,nvertex,npoly,nlight;
	unsigned char			*phit;
	d3pnt					*pt;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	portal_vertex_list_type	*pv;

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];

		// move meshes vertexes

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		pt->x+=x;
		pt->y+=y;
		pt->z+=z;
		pt++;
	}

		// move vertexes in portal compiled
		// vertex list

	if (!do_portal_vertex_list) return;

		// clear the hit list so we don't
		// move combined vertexes twice

	phit=portal->vertexes.phit;
	bzero(phit,portal->vertexes.nvlist);

		// move portal vertexes

	npoly=mesh->npoly;
	poly=mesh->polys;

	for (n=0;n!=npoly;n++) {

			// vertexes

		for (k=0;k!=poly->ptsz;k++) {
			idx=poly->draw.portal_v[k];

			if (phit[idx]==0x0) {
				phit[idx]=0x1;
				pv=&portal->vertexes.vertex_list[idx];
				pv->x+=x;
				pv->y+=y;
				pv->z+=z;
			}
		}

			// lighting vertexes

		nlight=poly->light.trig_count*3;

		for (k=0;k!=nlight;k++) {
			idx=poly->light.trig_vertex_idx[k];

			if (phit[idx]==0x0) {
				phit[idx]=0x1;
				pv=&portal->vertexes.vertex_list[idx];
				pv->x+=x;
				pv->y+=y;
				pv->z+=z;
			}
		}


		poly++;
	}
}

/* =======================================================

      Resize Mesh
      
======================================================= */

void map_portal_mesh_resize(map_type *map,int portal_idx,int mesh_idx,d3pnt *min,d3pnt *max)
{
	int						n,nvertex;
	d3pnt					*pt,org_min,org_max,dif,org_dif;
	portal_type				*portal;
	map_mesh_type			*mesh;

		// get original size
		
	map_portal_mesh_calculate_extent(map,portal_idx,mesh_idx,&org_min,&org_max);
	
		// get resize factor
		
	dif.x=max->x-min->x;
	dif.y=max->y-min->y;
	dif.z=max->z-min->z;
	
	org_dif.x=org_max.x-org_min.x;
	org_dif.y=org_max.y-org_min.y;
	org_dif.z=org_max.z-org_min.z;
	
		// resize vertexes

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		pt->x=(((pt->x-org_min.x)*dif.x)/org_dif.x)+min->x;
		pt->y=(((pt->y-org_min.y)*dif.y)/org_dif.y)+min->y;
		pt->z=(((pt->z-org_min.z)*dif.z)/org_dif.z)+min->z;
		pt++;
	}
}

/* =======================================================

      Flip and Rotate Mesh
      
======================================================= */

void map_portal_mesh_flip(map_type *map,int portal_idx,int mesh_idx,bool flip_x,bool flip_y,bool flip_z)
{
}

void map_portal_mesh_rotate(map_type *map,int portal_idx,int mesh_idx,float rot_x,float rot_y,float rot_z)
{
}


