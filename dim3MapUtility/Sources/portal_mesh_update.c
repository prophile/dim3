/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Portal Mesh Updates

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
		
	mesh_idx=map_portal_mesh_add(map,portal_idx);
	if (mesh_idx==-1) return(-1);

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

      Move Mesh To Different Portal
      
======================================================= */

int map_portal_mesh_switch_portal(map_type *map,int portal_idx,int mesh_idx,int new_portal_idx)
{
	int						new_mesh_idx;
	map_mesh_type			*org_mesh,*new_mesh;
	
		// create new mesh
		
	new_mesh_idx=map_portal_mesh_add(map,new_portal_idx);
	if (new_mesh_idx==-1) return(-1);
	
		// switch meshes
		
	org_mesh=&map->portals[portal_idx].mesh.meshes[mesh_idx];
	new_mesh=&map->portals[new_portal_idx].mesh.meshes[new_mesh_idx];
	
	memmove(new_mesh,org_mesh,sizeof(map_mesh_type));
	
		// set old mesh ptrs to null so they
		// aren't freed as they now belong to new mesh
		// then delete
		
	org_mesh->vertexes=NULL;
	org_mesh->polys=NULL;
	
	map_portal_mesh_delete(map,portal_idx,mesh_idx);
	
	return(new_mesh_idx);
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
	
		// move all poly boxes
		
	npoly=mesh->npoly;
	poly=mesh->polys;
	
	for (n=0;n!=npoly;n++) {
		poly->box.min.x+=x;
		poly->box.min.y+=y;
		poly->box.min.z+=z;
		
		poly->box.max.x+=x;
		poly->box.max.y+=y;
		poly->box.max.z+=z;
		
		poly->box.mid.x+=x;
		poly->box.mid.y+=y;
		poly->box.mid.z+=z;
	
		poly++;
	}
	
		// move mesh box
		
	mesh->box.min.x+=x;
	mesh->box.min.y+=y;
	mesh->box.min.z+=z;
	
	mesh->box.max.x+=x;
	mesh->box.max.y+=y;
	mesh->box.max.z+=z;
	
	mesh->box.mid.x+=x;
	mesh->box.mid.y+=y;
	mesh->box.mid.z+=z;

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
	int						n,nvertex,mx,my,mz;
	d3pnt					*pt;
	portal_type				*portal;
	map_mesh_type			*mesh;

		// get center

	map_portal_mesh_calculate_center(map,portal_idx,mesh_idx,&mx,&my,&mz);

		// flip vertexes

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		if (flip_x) pt->x=mx-(pt->x-mx);
		if (flip_y) pt->y=my-(pt->y-my);
		if (flip_z) pt->z=mz-(pt->z-mz);
		pt++;
	}
}

void map_portal_mesh_rotate(map_type *map,int portal_idx,int mesh_idx,float rot_x,float rot_y,float rot_z)
{
	int						n,nvertex,mx,my,mz;
	float					fx,fy,fz;
	d3pnt					*pt;
	matrix_type				mat;
	portal_type				*portal;
	map_mesh_type			*mesh;

		// get center

	map_portal_mesh_calculate_center(map,portal_idx,mesh_idx,&mx,&my,&mz);

		// matrixes

	matrix_rotate_xyz(&mat,rot_x,rot_y,rot_z);

		// rotate vertexes

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		fx=(float)(pt->x-mx);
		fy=(float)(pt->y-my);
		fz=(float)(pt->z-mz);

		matrix_vertex_multiply(&mat,&fx,&fy,&fz);

		pt->x=((int)fx)+mx;
		pt->y=((int)fy)+my;
		pt->z=((int)fz)+mz;

		pt++;
	}
}

/* =======================================================

      Tesselate Mesh
      
======================================================= */

bool map_portal_mesh_tesselate(map_type *map,int portal_idx,int mesh_idx)
{
	int						n,k,cnt,ntrig,npoly;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly,*trig_polys,*trig_poly;

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];

	if (mesh->npoly==0) return(TRUE);

		// count number of eventually trigs

	npoly=mesh->npoly;
	poly=mesh->polys;

	ntrig=0;

	for (n=0;n!=npoly;n++) {
		ntrig+=(poly->ptsz-2);
		poly++;
	}

		// create trigs

	trig_polys=(map_mesh_poly_type*)valloc(sizeof(map_mesh_poly_type)*ntrig);
	if (trig_polys==NULL) return(FALSE);

	trig_poly=trig_polys;
	poly=mesh->polys;

	for (n=0;n!=npoly;n++) {

		cnt=poly->ptsz-2;

		for (k=0;k!=cnt;k++) {
			memmove(trig_poly,poly,sizeof(map_mesh_poly_type));

			trig_poly->ptsz=3;

			trig_poly->v[0]=poly->v[0];
			trig_poly->v[1]=poly->v[k+1];
			trig_poly->v[2]=poly->v[k+2];
			
			trig_poly->gx[0]=poly->gx[0];
			trig_poly->gx[1]=poly->gx[k+1];
			trig_poly->gx[2]=poly->gx[k+2];
			
			trig_poly->gy[0]=poly->gy[0];
			trig_poly->gy[1]=poly->gy[k+1];
			trig_poly->gy[2]=poly->gy[k+2];
			
			trig_poly++;
		}

		poly++;
	}

		// substitute the trigs

	free(mesh->polys);

	mesh->npoly=ntrig;
	mesh->polys=trig_polys;

	return(TRUE);
}

/* =======================================================

      Update Meshes for Texture Shifting
      
======================================================= */

inline float map_portal_mesh_shift_texture_single_coord(float f_tick,float shift)
{
	int				i_add;
	float			f_add;

	f_add=(f_tick*0.001f)*shift;
	i_add=(int)f_add;				// keep within 0..1
	return(f_add-((float)i_add));
}

void map_portal_mesh_shift_portal_vertex_list(map_type *map,int portal_idx,int tick)
{
	int						n,k,t,nmesh,npoly,ptsz,idx;
	float					f_tick,gx,gy,fx,fy;
	unsigned char			*phit;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	portal_vertex_list_type	*pv;
	
	portal=&map->portals[portal_idx];
	
	f_tick=(float)tick;

		// clear the hit list so we don't
		// shift combined vertexes twice

	phit=portal->vertexes.phit;
	bzero(phit,portal->vertexes.nvlist);

		// run through the meshes

	nmesh=portal->mesh.nmesh;
	mesh=portal->mesh.meshes;

	for (n=0;n!=nmesh;n++) {

			// shiftable?

		if (!mesh->flag.shiftable) {
			mesh++;
			continue;
		}

			// shift polygons

		npoly=mesh->npoly;
		poly=mesh->polys;

		for (k=0;k!=npoly;k++) {

				// shiftable?

			if (!poly->draw.shift_on) {
				poly++;
				continue;
			}

				// shift polys

			ptsz=poly->ptsz;

			fx=map_portal_mesh_shift_texture_single_coord(f_tick,poly->x_shift);
			fy=map_portal_mesh_shift_texture_single_coord(f_tick,poly->y_shift);

			for (t=0;t!=ptsz;t++) {

				gx=poly->gx[t]+fx;
				gy=poly->gy[t]+fy;
				
				idx=poly->draw.portal_v[t];

				if (phit[idx]==0x0) {
					phit[idx]=0x1;
					pv=&portal->vertexes.vertex_list[idx];
					pv->gx=gx;
					pv->gy=gy;
				}
			}

			poly++;
		}

		mesh++;
	}
}


