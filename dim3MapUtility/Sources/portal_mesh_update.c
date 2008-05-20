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

extern void map_prepare_mesh_poly(map_mesh_type *mesh,map_mesh_poly_type *mesh_poly);

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

		// get original size and uv center
		
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

		// resize vertexes

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {

		if (org_dif.x!=0) {
			pt->x=(((pt->x-org_min.x)*dif.x)/org_dif.x)+min->x;
		}
		else {
			pt->x=min->x;
		}

		if (org_dif.y!=0) {
			pt->y=(((pt->y-org_min.y)*dif.y)/org_dif.y)+min->y;
		}
		else {
			pt->y=min->y;
		}

		if (org_dif.z!=0) {
			pt->z=(((pt->z-org_min.z)*dif.z)/org_dif.z)+min->z;
		}
		else {
			pt->z=min->z;
		}

		pt++;
	}
}

/* =======================================================

      Flip Mesh
      
======================================================= */

void map_portal_mesh_flip(map_type *map,int portal_idx,int mesh_idx,bool flip_x,bool flip_y,bool flip_z)
{
	int						n,nvertex;
	d3pnt					mpt;
	d3pnt					*pt;
	portal_type				*portal;
	map_mesh_type			*mesh;

		// get center

	map_portal_mesh_calculate_center(map,portal_idx,mesh_idx,&mpt);

		// flip vertexes

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		if (flip_x) pt->x=mpt.x-(pt->x-mpt.x);
		if (flip_y) pt->y=mpt.y-(pt->y-mpt.y);
		if (flip_z) pt->z=mpt.z-(pt->z-mpt.z);
		pt++;
	}
}

/* =======================================================

      Rotate Mesh
      
======================================================= */

void map_portal_mesh_rotate(map_type *map,int portal_idx,int mesh_idx,bool do_portal_vertex_list,float rot_x,float rot_y,float rot_z)
{
	int						n,k,idx,nvertex,npoly,nlight;
	float					fx,fy,fz;
	unsigned char			*phit;
	d3vct					f_mpt;
	d3pnt					*pt,mpt;
	matrix_type				mat;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;
	portal_vertex_list_type	*pv;

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];

		// get center

	map_portal_mesh_calculate_center(map,portal_idx,mesh_idx,&mpt);
	f_mpt.x=(float)(mpt.x+mesh->rot_off.x);
	f_mpt.y=(float)(mpt.y+mesh->rot_off.y);
	f_mpt.z=(float)(mpt.z+mesh->rot_off.z);

		// matrixes

	matrix_rotate_xyz(&mat,rot_x,rot_y,rot_z);

		// rotate vertexes

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		fx=((float)pt->x)-f_mpt.x;
		fy=((float)pt->y)-f_mpt.y;
		fz=((float)pt->z)-f_mpt.z;

		matrix_vertex_multiply(&mat,&fx,&fy,&fz);

		pt->x=(int)(fx+f_mpt.x);
		pt->y=(int)(fy+f_mpt.y);
		pt->z=(int)(fz+f_mpt.z);

		pt++;
	}
	
		// rotate vertexes in portal compiled
		// vertex list

	if (!do_portal_vertex_list) return;

		// clear the hit list so we don't
		// rotate combined vertexes twice

	phit=portal->vertexes.phit;
	bzero(phit,portal->vertexes.nvlist);

		// rotate portal vertexes

	npoly=mesh->npoly;
	poly=mesh->polys;

	for (n=0;n!=npoly;n++) {

			// vertexes

		for (k=0;k!=poly->ptsz;k++) {
			idx=poly->draw.portal_v[k];

			if (phit[idx]==0x0) {
				phit[idx]=0x1;
				pv=&portal->vertexes.vertex_list[idx];
				
				fx=((float)pv->x)-f_mpt.x;
				fy=((float)pv->y)-f_mpt.y;
				fz=((float)pv->z)-f_mpt.z;

				matrix_vertex_multiply(&mat,&fx,&fy,&fz);

				pv->x=fx+f_mpt.x;
				pv->y=fy+f_mpt.y;
				pv->z=fz+f_mpt.z;
			}
		}

			// lighting vertexes

		nlight=poly->light.trig_count*3;

		for (k=0;k!=nlight;k++) {
			idx=poly->light.trig_vertex_idx[k];

			if (phit[idx]==0x0) {
				phit[idx]=0x1;
				pv=&portal->vertexes.vertex_list[idx];
				
				fx=pv->x-f_mpt.x;
				fy=pv->y-f_mpt.y;
				fz=pv->z-f_mpt.z;

				matrix_vertex_multiply(&mat,&fx,&fy,&fz);

				pv->x=fx+f_mpt.x;
				pv->y=fy+f_mpt.y;
				pv->z=fz+f_mpt.z;
			}
		}

		poly++;
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

      Tesselate Mesh
      
======================================================= */

bool map_portal_mesh_poly_punch_hole(map_type *map,int portal_idx,int mesh_idx,int poly_idx,int hole_type)
{
	int						n,ptsz,mx,my,mz,
							px[8],py[8],pz[8],
							k,kx[4],ky[4],kz[4];
	float					gx[8],gy[8],mgx,mgy,
							k_gx[4],k_gy[4];
	d3pnt					*pt;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

	ptsz=poly->ptsz;

		// new hole vertexes

	mx=my=mz=0;
	mgx=mgy=0.0f;

	for (n=0;n!=ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];

		px[n]=pt->x;
		py[n]=pt->y;
		pz[n]=pt->z;

		gx[n]=poly->gx[n];
		gy[n]=poly->gy[n];

		mx+=pt->x;
		my+=pt->y;
		mz+=pt->z;

		mgx+=gx[n];
		mgy+=gy[n];
	}

	mx/=ptsz;
	my/=ptsz;
	mz/=ptsz;

	mgx=mgx/(float)ptsz;
	mgy=mgy/(float)ptsz;

	for (n=0;n!=ptsz;n++) {
		px[n]=((px[n]-mx)/2)+mx;
		py[n]=((py[n]-my)/2)+my;
		pz[n]=((pz[n]-mz)/2)+mz;

		gx[n]=((gx[n]-mgx)/2.0f)+mgx;
		gy[n]=((gy[n]-mgy)/2.0f)+mgy;
	}

		// new polygon for each vertex

	for (n=0;n!=ptsz;n++) {

		poly=&mesh->polys[poly_idx];			// add might force memory move, need to always rebuild pointer

		k=n+1;
		if (k==ptsz) k=0;

		pt=&mesh->vertexes[poly->v[n]];
		kx[0]=pt->x;
		ky[0]=pt->y;
		kz[0]=pt->z;

		pt=&mesh->vertexes[poly->v[k]];
		kx[1]=pt->x;
		ky[1]=pt->y;
		kz[1]=pt->z;

		kx[2]=px[k];
		ky[2]=py[k];
		kz[2]=pz[k];

		kx[3]=px[n];
		ky[3]=py[n];
		kz[3]=pz[n];

		k_gx[0]=poly->gx[n];
		k_gy[0]=poly->gy[n];

		k_gx[1]=poly->gx[k];
		k_gy[1]=poly->gy[k];

		k_gx[2]=gx[k];
		k_gy[2]=gy[k];

		k_gx[3]=gx[n];
		k_gy[3]=gy[n];

		if (map_portal_mesh_add_poly(map,portal_idx,mesh_idx,4,kx,ky,kz,k_gx,k_gy,poly->txt_idx)==-1) return(FALSE);
	}

		// finish by deleting original polygon

	return(map_portal_mesh_delete_poly(map,portal_idx,mesh_idx,poly_idx));
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

/* =======================================================

      Poly UV Coordinates as Offset/Size
      
======================================================= */

float map_get_texture_reduce_coord(float f)
{
	int			i;

	i=(int)f;
	return(f-(float)i);
}

float map_get_texture_round_coord(float f)
{
	int			i;

	i=(int)(f*100);
	return(((float)i)/100);
}

void map_portal_mesh_get_poly_uv_as_box(map_type *map,int portal_idx,int mesh_idx,int poly_idx,float *x_txtoff,float *y_txtoff,float *x_txtfact,float *y_txtfact)
{
	int						n;
	float					gx_min,gx_max,gy_min,gy_max;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

		// get UV extends

	gx_min=gx_max=poly->gx[0];
	gy_min=gy_max=poly->gy[0];

	for (n=1;n<poly->ptsz;n++) {
		if (poly->gx[n]<gx_min) gx_min=poly->gx[n];
		if (poly->gx[n]>gx_max) gx_max=poly->gx[n];
		if (poly->gy[n]<gy_min) gy_min=poly->gy[n];
		if (poly->gy[n]>gy_max) gy_max=poly->gy[n];
	}

		// create boxed coordinates

	*x_txtoff=map_get_texture_round_coord(gx_min);
	*y_txtoff=map_get_texture_round_coord(gy_min);

	*x_txtfact=map_get_texture_round_coord(gx_max-gx_min);
	*y_txtfact=map_get_texture_round_coord(gy_max-gy_min);
}

void map_portal_mesh_set_poly_uv_as_box(map_type *map,int portal_idx,int mesh_idx,int poly_idx,float x_txtoff,float y_txtoff,float x_txtfact,float y_txtfact)
{
	int						n;
	float					gx,gy,
							org_x_txtoff,org_y_txtoff,org_x_txtfact,org_y_txtfact;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];

		// get current box coordinates

	map_portal_mesh_get_poly_uv_as_box(map,portal_idx,mesh_idx,poly_idx,&org_x_txtoff,&org_y_txtoff,&org_x_txtfact,&org_y_txtfact);

		// reset to new coordinates

	for (n=0;n!=poly->ptsz;n++) {
	
		if (poly->gx[n]==org_x_txtoff) {
			poly->gx[n]=x_txtoff;
		}
		else {
			if (poly->gx[n]==(org_x_txtoff+org_x_txtfact)) {
				poly->gx[n]=x_txtoff+x_txtfact;
			}
			else {
				gx=poly->gx[n]-org_x_txtoff;
				gx/=org_x_txtfact;
				gx*=x_txtfact;
				poly->gx[n]=x_txtoff+gx;
			}
		}
		
		if (poly->gy[n]==org_y_txtoff) {
			poly->gy[n]=y_txtoff;
		}
		else {
			if (poly->gy[n]==(org_y_txtoff+org_y_txtfact)) {
				poly->gy[n]=y_txtoff+y_txtfact;
			}
			else {
				gy=poly->gy[n]-org_y_txtoff;
				gy/=org_y_txtfact;
				gy*=y_txtfact;
				poly->gy[n]=y_txtoff+gy;
			}
		}
	}
}

/* =======================================================

      Recalculate UVs
      
======================================================= */

void map_get_texture_uv_get_scale(map_type *map,int txt_idx,float *txt_scale_x,float *txt_scale_y)
{
	texture_type		*texture;

	texture=&map->textures[txt_idx];
	if (!texture->scale.on) {
		*txt_scale_x=map->settings.txt_scale_x/(float)map_enlarge;			// need to reflect original scale
		*txt_scale_y=map->settings.txt_scale_y/(float)map_enlarge;
		return;
	}
	
	*txt_scale_x=texture->scale.x/(float)map_enlarge;
	*txt_scale_y=texture->scale.y/(float)map_enlarge;
}

void map_portal_mesh_reset_poly_uv(map_type *map,int portal_idx,int mesh_idx,int poly_idx)
{
	int						n,kx,ky,kz;
	float					fx,fy,fz,ltxtx,rtxtx,ltxty,rtxty,ltxtz,rtxtz,
							x_txtoff,y_txtoff,x_txtfact,y_txtfact,x_txtfact_2,
							f_dist_1,f_dist_2,txt_scale_x,txt_scale_y;
	double					dx,dz,d;
	d3pnt					*pt;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*poly;

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
	poly=&mesh->polys[poly_idx];
	
		// get scale

	map_get_texture_uv_get_scale(map,poly->txt_idx,&txt_scale_x,&txt_scale_y);
	
		// setup box and slope
		// needed for texture calculations

	map_prepare_mesh_poly(mesh,poly);

		// walls-like polygons
		
	if (poly->box.wall_like) {
		
		ltxtx=(float)((poly->box.min.x+portal->x)+(poly->box.min.z+portal->z))*txt_scale_x;
		rtxtx=(float)((poly->box.max.x+portal->x)+(poly->box.max.z+portal->z))*txt_scale_x;
			
			// get point texture factor
				
		x_txtfact=rtxtx-ltxtx;
			
			// get distance texture factor
				
		dx=(double)(poly->box.min.x-poly->box.max.x);
		dz=(double)(poly->box.min.z-poly->box.max.z);
		d=(dx*dx)+(dz*dz);
		x_txtfact_2=(float)(sqrt(d)*txt_scale_x);
		if (x_txtfact<0) x_txtfact_2=-x_txtfact_2;
		
		if (fabs(x_txtfact_2)>fabs(x_txtfact)) x_txtfact=x_txtfact_2;		// if distance calc is longer, use that
		
		x_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxtx));
		x_txtfact=map_get_texture_round_coord(x_txtfact);
		
		ltxty=((float)poly->box.min.y)*txt_scale_y;
		rtxty=((float)poly->box.max.y)*txt_scale_y;
		
		y_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxty));
		y_txtfact=map_get_texture_round_coord(rtxty-ltxty);
	
			// deal with offset locks
			
		if (map->textures[poly->txt_idx].scale.lock_offset) {
			x_txtoff=0.0f;
			y_txtoff=0.0f;
		}
				
			// create the polygons UVs across the line
			
		dx=(double)(poly->line.rx-poly->line.lx);
		dz=(double)(poly->line.rz-poly->line.lz);
		f_dist_2=(float)sqrt((dx*dx)+(dz*dz));
			
		for (n=0;n!=poly->ptsz;n++) {
			pt=&mesh->vertexes[poly->v[n]];

			dx=(double)(pt->x-poly->line.lx);
			dz=(double)(pt->z-poly->line.lz);
			f_dist_1=(float)sqrt((dx*dx)+(dz*dz));
			
			ky=pt->y-poly->box.min.y;

			fx=f_dist_1/f_dist_2;
			fy=(float)ky/(float)(poly->box.max.y-poly->box.min.y);

			poly->gx[n]=x_txtoff+(x_txtfact*fx);
			poly->gy[n]=y_txtoff+(y_txtfact*fy);
		}
		
		return;
	}
			
		// floor-like polygon

	ltxtx=((float)(poly->box.min.x+portal->x))*txt_scale_x;
	rtxtx=((float)(poly->box.max.x+portal->x))*txt_scale_x;

	ltxtz=((float)(poly->box.min.z+portal->z))*txt_scale_y;
	rtxtz=((float)(poly->box.max.z+portal->z))*txt_scale_y;

	x_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxtx));
	x_txtfact=map_get_texture_round_coord(rtxtx-ltxtx);
	
	y_txtoff=map_get_texture_round_coord(map_get_texture_reduce_coord(ltxtz));
	y_txtfact=map_get_texture_round_coord(rtxtz-ltxtz);
	
		// deal with offset locks
		
	if (map->textures[poly->txt_idx].scale.lock_offset) {
		x_txtoff=0.0f;
		y_txtoff=0.0f;
	}

		// set the polygon UVs
		
	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];

		kx=pt->x-poly->box.min.x;
		kz=pt->z-poly->box.min.z;

		fx=(float)kx/(float)(poly->box.max.x-poly->box.min.x);
		fz=(float)kz/(float)(poly->box.max.z-poly->box.min.z);

		poly->gx[n]=x_txtoff+(x_txtfact*fx);
		poly->gy[n]=y_txtoff+(y_txtfact*fz);
	}
}

void map_portal_mesh_reset_uv(map_type *map,int portal_idx,int mesh_idx)
{
	int						n,npoly;
	portal_type				*portal;
	map_mesh_type			*mesh;

	portal=&map->portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];

	npoly=mesh->npoly;

	for (n=0;n!=npoly;n++) {
		map_portal_mesh_reset_poly_uv(map,portal_idx,mesh_idx,n);
	}
}


