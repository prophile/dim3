/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Ray Tracing

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "physics.h"
#include "models.h"

extern map_type				map;
extern server_type			server;

/* =======================================================

      Ray Trace Vector Utilities
      
======================================================= */

void ray_trace_create_vector_from_points(d3vct *v,int x1,int y1,int z1,int x2,int y2,int z2)
{
	v->x=(float)(x1-x2);
	v->y=(float)(y1-y2);
	v->z=(float)(z1-z2);
}

void ray_trace_vector_cross_product(d3vct *cp,d3vct *v1,d3vct *v2)
{
	cp->x=(v1->y*v2->z)-(v2->y*v1->z);
	cp->y=(v1->z*v2->x)-(v2->z*v1->x);
	cp->z=(v1->x*v2->y)-(v2->x*v1->y);
}

float ray_trace_vector_inner_product(d3vct *v1,d3vct *v2)
{
	return((v1->x*v2->x)+(v1->y*v2->y)+(v1->z*v2->z));
}

/* =======================================================

      Ray Trace Contact Utility
      
======================================================= */

void ray_trace_contact_clear(ray_trace_contact_type *contact)
{
	contact->poly.mesh_idx=-1;
	contact->obj.uid=-1;
	contact->proj.uid=-1;
}

/* =======================================================

      Ray Trace Triangles and Polygons
      
======================================================= */

float ray_trace_triangle(d3pnt *spt,d3vct *vct,d3pnt *hpt,int *x,int *y,int *z)
{
	float				det,invDet,t,u,v;
	d3vct				perpVector,lineToTrigPointVector,lineToTrigPerpVector,v1,v2;
	
		// get triangle vectors
		
	ray_trace_create_vector_from_points(&v1,x[1],y[1],z[1],x[0],y[0],z[0]);
	ray_trace_create_vector_from_points(&v2,x[2],y[2],z[2],x[0],y[0],z[0]);
	
		// calculate the determinate

	ray_trace_vector_cross_product(&perpVector,vct,&v2);
	det=ray_trace_vector_inner_product(&v1,&perpVector);
	
		// is line on the same plane as triangle?
		
	if ((det>-0.00001f) && (det<0.00001f)) return(-1.0f);

		// get the inverse determinate

	invDet=1.0f/det;

		// calculate triangle U and test
	
	ray_trace_create_vector_from_points(&lineToTrigPointVector,spt->x,spt->y,spt->z,x[0],y[0],z[0]);
	u=invDet*ray_trace_vector_inner_product(&lineToTrigPointVector,&perpVector);
	if ((u<0.0f) || (u>1.0f)) return(-1.0f);
	
		// calculate triangle V and test

	ray_trace_vector_cross_product(&lineToTrigPerpVector,&lineToTrigPointVector,&v1);
	v=invDet*ray_trace_vector_inner_product(vct,&lineToTrigPerpVector);
	if ((v<0.0f) || ((u+v)>1.0f)) return(-1.0f);
	
		// get line T for point(t) =  start_point + (vector*t)
		// -t are on the negative vector behind the point, so ignore

	t=invDet*ray_trace_vector_inner_product(&v2,&lineToTrigPerpVector);
	if (t<0.0f) return(-1.0f);
	
		// get point on line of intersection
		
	hpt->x=spt->x+(int)(vct->x*t);
	hpt->y=spt->y+(int)(vct->y*t);
	hpt->z=spt->z+(int)(vct->z*t);
	
		// return t
		
	return(t);
}

float ray_trace_mesh_polygon(d3pnt *spt,d3vct *vct,d3pnt *hpt,map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int			n,trig_count;
	int			px[3],py[3],pz[3];
	float		t,hit_t;
	d3pnt		pt,*m_pt;
	
	hit_t=-1.0f;
	
		// first vertex is always 0

	m_pt=&mesh->vertexes[poly->v[0]];
		
	px[0]=m_pt->x;
	py[0]=m_pt->y;
	pz[0]=m_pt->z;
	
		// run through all the triangles of the polygon
		
	trig_count=poly->ptsz-2;
	
	for (n=0;n<trig_count;n++) {
		m_pt=&mesh->vertexes[poly->v[n+1]];
		px[1]=m_pt->x;
		py[1]=m_pt->y;
		pz[1]=m_pt->z;

		m_pt=&mesh->vertexes[poly->v[n+2]];
		px[2]=m_pt->x;
		py[2]=m_pt->y;
		pz[2]=m_pt->z;
		
			// check for hit
			
		t=ray_trace_triangle(spt,vct,&pt,px,py,pz);
		if (t==-1.0f) continue;
		
			// closer hit or first hit?
			
		if ((t<hit_t) || (hit_t==-1.0f)) {
			hit_t=t;
			hpt->x=pt.x;
			hpt->y=pt.y;
			hpt->z=pt.z;
		}
	}
	
	return(hit_t);
}

float ray_trace_polygon(d3pnt *spt,d3vct *vct,d3pnt *hpt,int ptsz,int *x,int *y,int *z)
{
	int			n,k,trig_count;
	int			px[3],py[3],pz[3];
	float		t,hit_t;
	d3pnt		pt;
	
	hit_t=-1.0f;
	
		// first vertex is always 0
		
	px[0]=x[0];
	py[0]=y[0];
	pz[0]=z[0];
	
		// run through all the triangles of the polygon
		
	trig_count=ptsz-2;
	
	for (n=0;n<trig_count;n++) {
		k=n+1;
		px[1]=x[k];
		py[1]=y[k];
		pz[1]=z[k];
		k++;
		px[2]=x[k];
		py[2]=y[k];
		pz[2]=z[k];
		
			// check for hit
			
		t=ray_trace_triangle(spt,vct,&pt,px,py,pz);
		if (t==-1.0f) continue;
		
			// closer hit or first hit?
			
		if ((t<hit_t) || (hit_t==-1.0f)) {
			hit_t=t;
			hpt->x=pt.x;
			hpt->y=pt.y;
			hpt->z=pt.z;
		}
	}
	
	return(hit_t);
}

float ray_trace_rotated_box(d3pnt *spt,d3vct *vct,d3pnt *hpt,int *hit_face,int x,int z,int lx,int rx,int tz,int bz,int ty,int by,float rang)
{
	int			n,idx,px[4],py[4],pz[4];
	float		t[6],hit_t;
	d3pnt		pt[6];

	px[0]=px[3]=lx;
	px[1]=px[2]=rx;
	py[0]=py[1]=ty;
	py[2]=py[3]=by;
	pz[0]=pz[1]=pz[2]=pz[3]=tz;
	rotate_2D_polygon(4,px,pz,x,z,rang);
	t[0]=ray_trace_polygon(spt,vct,&pt[0],4,px,py,pz);

	px[0]=px[3]=lx;
	px[1]=px[2]=rx;
	py[0]=py[1]=ty;
	py[2]=py[3]=by;
	pz[0]=pz[1]=pz[2]=pz[3]=bz;
	rotate_2D_polygon(4,px,pz,x,z,rang);
	t[1]=ray_trace_polygon(spt,vct,&pt[1],4,px,py,pz);
	
	px[0]=px[1]=px[2]=px[3]=lx;
	py[0]=py[1]=ty;
	py[2]=py[3]=by;
	pz[0]=pz[3]=tz;
	pz[1]=pz[2]=bz;
	rotate_2D_polygon(4,px,pz,x,z,rang);
	t[2]=ray_trace_polygon(spt,vct,&pt[2],4,px,py,pz);
	
	px[0]=px[1]=px[2]=px[3]=rx;
	py[0]=py[1]=ty;
	py[2]=py[3]=by;
	pz[0]=pz[3]=tz;
	pz[1]=pz[2]=bz;
	rotate_2D_polygon(4,px,pz,x,z,rang);
	t[3]=ray_trace_polygon(spt,vct,&pt[3],4,px,py,pz);
	
	px[0]=px[3]=lx;
	px[1]=px[2]=rx;
	py[0]=py[1]=py[2]=py[3]=ty;
	pz[0]=pz[1]=tz;
	pz[2]=pz[3]=bz;
	rotate_2D_polygon(4,px,pz,x,z,rang);
	t[4]=ray_trace_polygon(spt,vct,&pt[4],4,px,py,pz);
	
	px[0]=px[3]=lx;
	px[1]=px[2]=rx;
	py[0]=py[1]=py[2]=py[3]=ty;
	pz[0]=pz[1]=tz;
	pz[2]=pz[3]=bz;
	rotate_2D_polygon(4,px,pz,x,z,rang);
	t[5]=ray_trace_polygon(spt,vct,&pt[5],4,px,py,pz);
	
		// find closest hit
		
	idx=-1;
	hit_t=-1.0f;
	
	for (n=0;n!=5;n++) {
		if (t[n]==-1.0f) continue;
		if ((t[n]<hit_t) || (hit_t==-1.0f)) {
			idx=n;
			hit_t=t[n];
		}
	}
	
	if (idx!=-1) {
		hpt->x=pt[idx].x;
		hpt->y=pt[idx].y;
		hpt->z=pt[idx].z;
		*hit_face=idx;
		return(hit_t);
	}
	
	return(-1.0f);
}

/* =======================================================

      Ray Trace Objects
      
======================================================= */

float ray_trace_object(d3pnt *spt,d3pnt *ept,d3vct *vct,d3pnt *hpt,int *hit_face,int *hit_box_idx,obj_type *obj)
{
	int					n,nhit_box,wid,
						x,y,z,lx,rx,tz,bz,ty,by,
						x2,z2;
	float				rang,hit_t;
	model_draw			*draw;
	model_type			*model;
	model_hit_box_type	*hit_box;
	
	x=obj->pnt.x;
	z=obj->pnt.z;
	
	ty=obj->pnt.y-obj->size.y;
	y=by=obj->pnt.y;

		// setup fine check
		
	wid=obj->size.x>>1;
	lx=x-wid;
	rx=x+wid;
	
	wid=obj->size.z>>1;
	tz=z-wid;
	bz=z+wid;
		
	rang=angle_add(obj->ang.y,obj->draw.rot.y);

	*hit_box_idx=-1;

		// regular collisions

	if (!obj->hit_box.on) {
		return(ray_trace_rotated_box(spt,vct,hpt,hit_face,x,z,lx,rx,tz,bz,ty,by,rang));
	}

		// check model
		
	draw=&obj->draw;
	if ((draw->uid==-1) || (!draw->on)) return(-1.0f);
	
	model=model_find_uid(draw->uid);
	if (model==NULL) return(-1.0f);
	
		// check hit boxes

	x+=draw->offset.x;
	z+=draw->offset.z;
		
	hit_box=model->hit_boxes;
	nhit_box=model->nhit_box;
	
	for (n=0;n!=nhit_box;n++) {
		x2=x+hit_box->box.offset.x;
		wid=hit_box->box.size.x>>1;
		lx=x2-wid;
		rx=x2+wid;
		
		by=y+hit_box->box.offset.y;
		ty=by-hit_box->box.size.y;

		z2=z+hit_box->box.offset.z;
		wid=hit_box->box.size.z>>1;
		tz=z2-wid;
		bz=z2+wid;
		
		hit_t=ray_trace_rotated_box(spt,vct,hpt,hit_face,x,z,lx,rx,tz,bz,ty,by,rang);
		if (hit_t!=-1.0f) {
			*hit_box_idx=n;
			return(hit_t);
		}

		hit_box++;
	}

	return(-1.0f);
}

/* =======================================================

      Ray Trace Projectiles
      
======================================================= */

float ray_trace_projectile(d3pnt *spt,d3pnt *ept,d3vct *vct,d3pnt *hpt,int *hit_face,proj_type *proj)
{
	int			wid,x,z,lx,rx,tz,bz,ty,by;
	float		rang;

	x=proj->pnt.x;
	z=proj->pnt.z;
	
	ty=proj->pnt.y-proj->size.y;
	by=proj->pnt.y;
	
		// setup fine check
		
	wid=proj->size.x>>1;
	lx=x-wid;
	rx=x+wid;
	
	wid=proj->size.z>>1;
	tz=z-wid;
	bz=z+wid;
		
	rang=angle_add(proj->ang.y,proj->draw.rot.y);

		// box collisions

	return(ray_trace_rotated_box(spt,vct,hpt,hit_face,x,z,lx,rx,tz,bz,ty,by,rang));
}

/* =======================================================

      Ray Trace Map
      
======================================================= */

void ray_trace_map(int item_count,ray_trace_check_item_type *item_list,d3pnt *spt,d3pnt *ept,d3vct *vct,d3pnt *hpt,float *hit_t,ray_trace_contact_type *contact)
{
	int							n,k,hit_face,hit_box_idx;
	float						t;
	d3pnt						pt;
	obj_type					*obj;
	proj_type					*proj;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*poly;
	ray_trace_check_item_type	*item;
	
	pt.x=pt.y=pt.z=0;

		// check all hit candidates

	item=item_list;

	for (n=0;n!=item_count;n++) {
		
		switch (item->type) {

			case ray_trace_check_item_object:
				obj=&server.objs[item->index];

				t=ray_trace_object(spt,ept,vct,&pt,&hit_face,&hit_box_idx,obj);
				if (t==-1.0f) break;
				
					// closer hit?
					
				if (t>=(*hit_t)) break;
				
				*hit_t=t;
				hpt->x=pt.x;
				hpt->y=pt.y;
				hpt->z=pt.z;
			
				ray_trace_contact_clear(contact);

				contact->obj.uid=obj->uid;
				contact->obj.hit_face=hit_face;

				if (contact->origin!=poly_ray_trace_origin_projectile) {
					if (hit_box_idx!=-1) {
						obj->hit_box.obj_hit_box_idx=hit_box_idx;
					}
					else {
						obj->hit_box.obj_hit_box_idx=-1;
					}
				}
				else {
					if (hit_box_idx!=-1) {
						obj->hit_box.proj_hit_box_idx=hit_box_idx;
					}
					else {
						obj->hit_box.proj_hit_box_idx=-1;
					}
				}

				break;

			case ray_trace_check_item_projectile:
				proj=&server.projs[item->index];
				
				t=ray_trace_projectile(spt,ept,vct,&pt,&hit_face,proj);
				if (t==-1.0f) break;
					
					// closer hit?
						
				if (t>=(*hit_t)) break;
					
				*hit_t=t;
				hpt->x=pt.x;
				hpt->y=pt.y;
				hpt->z=pt.z;
				
				ray_trace_contact_clear(contact);

				contact->proj.uid=proj->uid;
				contact->proj.hit_face=hit_face;
				break;

			case ray_trace_check_item_mesh:

				mesh=&map.mesh.meshes[item->index];
						
					// rough bounds check
					// list building is a little rough to quickly eliminate
					// most meshes, so we'll still check it here just in case
					// we can eliminate a easy mesh

				if ((spt->x<mesh->box.min.x) && (ept->x<mesh->box.min.x)) break;
				if ((spt->x>mesh->box.max.x) && (ept->x>mesh->box.max.x)) break;
				if ((spt->y<mesh->box.min.y) && (ept->y<mesh->box.min.y)) break;
				if ((spt->y>mesh->box.max.y) && (ept->y>mesh->box.max.y)) break;
				if ((spt->z<mesh->box.min.z) && (ept->z<mesh->box.min.z)) break;
				if ((spt->z>mesh->box.max.z) && (ept->z>mesh->box.max.z)) break;

					// check polygons

				for (k=0;k!=mesh->npoly;k++) {

					poly=&mesh->polys[k];

						// poly ignores

					if (contact->hit_mode!=poly_ray_trace_hit_mode_all) {
						if (poly->box.wall_like) {
							if (contact->hit_mode==poly_ray_trace_hit_mode_floor_only) continue;
						}
						else {
							if (contact->hit_mode==poly_ray_trace_hit_mode_wall_only) continue;
						}
					}
					
						// rough bounds check

					if ((spt->x<poly->box.min.x) && (ept->x<poly->box.min.x)) continue;
					if ((spt->x>poly->box.max.x) && (ept->x>poly->box.max.x)) continue;
					if ((spt->y<poly->box.min.y) && (ept->y<poly->box.min.y)) continue;
					if ((spt->y>poly->box.max.y) && (ept->y>poly->box.max.y)) continue;
					if ((spt->z<poly->box.min.z) && (ept->z<poly->box.min.z)) continue;
					if ((spt->z>poly->box.max.z) && (ept->z>poly->box.max.z)) continue;

						// ray trace
						
					t=ray_trace_mesh_polygon(spt,vct,&pt,mesh,poly);
					if (t==-1.0f) continue;
						
						// closer hit?
					
					if (t>=(*hit_t)) continue;
					
					*hit_t=t;
					hpt->x=pt.x;
					hpt->y=pt.y;
					hpt->z=pt.z;
					
					ray_trace_contact_clear(contact);
					contact->poly.mesh_idx=item->index;
					contact->poly.poly_idx=k;
				}

				break;
		}
		
		item++;
	}
}

/* =======================================================

      Ray Utilities
      
======================================================= */

void ray_push(d3pnt *pt,d3ang *ang,int dist)
{
	d3vct			vct;
	matrix_type		mat;

	vct.x=0.0f;
	vct.y=0.0f;
	vct.z=(float)-dist;

	if (ang->x!=0.0f) {
		matrix_rotate_x(&mat,ang->x);
		matrix_vertex_multiply(&mat,&vct.x,&vct.y,&vct.z);
	}
	
	if (ang->z!=0.0f) {
		matrix_rotate_z(&mat,ang->z);
		matrix_vertex_multiply(&mat,&vct.x,&vct.y,&vct.z);
	}
	
	if (ang->y!=0.0f) {
		matrix_rotate_y(&mat,ang->y);
		matrix_vertex_multiply(&mat,&vct.x,&vct.y,&vct.z);
	}
	
	pt->x+=(int)vct.x;
	pt->y+=(int)vct.y;
	pt->z+=(int)vct.z;
}

void ray_push_to_end(d3pnt *pt,d3pnt *ept,int dist)
{
	int			x,y,z,td;

	x=ept->x-pt->x;
	y=ept->y-pt->y;
	z=ept->z-pt->z;

	td=abs(x)+abs(y)+abs(z);
	if (td==0) return;

	pt->x+=(x*dist)/td;
	pt->y+=(y*dist)/td;
	pt->z+=(z*dist)/td;
}

/* =======================================================

      Setup Item Hist Lists
      
======================================================= */

int ray_trace_map_item_list_setup(int cnt,d3pnt *spts,d3pnt *epts,ray_trace_contact_type *contact,ray_trace_check_item_type *item_list)
{
	int							n,x,z,radius,item_count;
	d3pnt						*spt,*ept,min,max;
	obj_type					*obj;
	proj_type					*proj;
	map_mesh_type				*mesh;
	ray_trace_check_item_type	*item;
	
	item_count=0;
	item=item_list;

		// get bounds for all vectors
		// and use those as a collision check
		// this is rough, but going through all
		// the meshes is actually longer than
		// dealing with a few missed hits

	spt=spts;
	ept=epts;
	
	if (spt->x<ept->x) {
		min.x=spt->x;
		max.x=ept->x;
	}
	else {
		min.x=ept->x;
		max.x=spt->x;
	}

	if (spt->y<ept->y) {
		min.y=spt->y;
		max.y=ept->y;
	}
	else {
		min.y=ept->y;
		max.y=spt->y;
	}

	if (spt->z<ept->z) {
		min.z=spt->z;
		max.z=ept->z;
	}
	else {
		min.z=ept->z;
		max.z=spt->z;
	}

	for (n=1;n<cnt;n++) {

		spt++;
		ept++;

		if (spt->x<min.x) min.x=spt->x;
		if (ept->x<min.x) min.x=ept->x;
		if (spt->x>max.x) max.x=spt->x;
		if (ept->x>max.x) max.x=ept->x;

		if (spt->y<min.y) min.y=spt->y;
		if (ept->y<min.y) min.y=ept->y;
		if (spt->y>max.y) max.y=spt->y;
		if (ept->y>max.y) max.y=ept->y;

		if (spt->z<min.z) min.z=spt->z;
		if (ept->z<min.z) min.z=ept->z;
		if (spt->z>max.z) max.z=spt->z;
		if (ept->z>max.z) max.z=ept->z;
	}

		// check objects
		
	if (contact->obj.on) {
	
		for (n=0;n!=server.count.obj;n++) {

			obj=&server.objs[n];

				// object a hit candidate?

			if ((obj->hidden) || (obj->pickup.on) || (obj->uid==contact->obj.ignore_uid)) continue;
			if (((contact->origin==poly_ray_trace_origin_object) && (!obj->contact.object_on)) || ((contact->origin==poly_ray_trace_origin_projectile) && (!obj->contact.projectile_on))) continue;
			
				// rough y vector box check
				
			if (max.y<(obj->pnt.y-obj->size.y)) continue;
			if (min.y>obj->pnt.y) continue;
			
				// rough x/z vector box checks
				
			x=obj->pnt.x;
			z=obj->pnt.z;

			radius=obj->size.x;
			if (obj->size.z>radius) radius=obj->size.z;
			
			radius=radius>>1;
			
			if (max.x<(x-radius)) continue;
			if (min.x>(x+radius)) continue;
			if (max.z<(z-radius)) continue;
			if (min.z>(z+radius)) continue;

				// add to item list
				
			item->type=ray_trace_check_item_object;
			item->index=n;

			item++;
			item_count++;
			
				// in list, no more checks required
			
			if (item_count==ray_trace_max_check_item) return(item_count);
		}
	}
	
		// check projectiles
		
	if (contact->proj.on) {
	
		for (n=0;n!=server.count.proj;n++) {

			proj=&server.projs[n];

				// is projectile a hit candidate?

			if (proj->uid==contact->proj.ignore_uid) continue;
			
				// rough y vector box check
				
			if (max.y<(proj->pnt.y-proj->size.y)) continue;
			if (min.y>proj->pnt.y) continue;
			
				// rough x/z vector box checks
				
			x=proj->pnt.x;
			z=proj->pnt.z;

			radius=proj->size.radius>>1;
			
			if (max.x<(x-radius)) continue;
			if (min.x>(x+radius)) continue;
			if (max.z<(z-radius)) continue;
			if (min.z>(z+radius)) continue;
			
				// add to item list

			item->type=ray_trace_check_item_projectile;
			item->index=n;

			item++;
			item_count++;
			
			if (item_count==ray_trace_max_check_item) return(item_count);
		}
	}
	
		// check all meshes

	for (n=0;n!=map.mesh.nmesh;n++) {

		mesh=&map.mesh.meshes[n];
		
			// rough vector box bounds check

		if (max.x<mesh->box.min.x) continue;
		if (min.x>mesh->box.max.x) continue;
		if (max.y<mesh->box.min.y) continue;
		if (min.y>mesh->box.max.y) continue;
		if (max.z<mesh->box.min.z) continue;
		if (min.z>mesh->box.max.z) continue;
			
			// skip pass through meshes
			// we do this second as I suspect there will
			// be a limited number of these, a lot less then
			// will be eliminated with the bounds check

		if (mesh->flag.pass_through) continue;
		
			// add to item list

		item->type=ray_trace_check_item_mesh;
		item->index=n;

		item++;
		item_count++;

		if (item_count==ray_trace_max_check_item) return(item_count);
	}
	
	return(item_count);
}

/* =======================================================

      Ray Trace Main Lines
      
======================================================= */

bool ray_trace_map_by_angle(d3pnt *spt,d3ang *ang,int dist,d3pnt *hpt,ray_trace_contact_type *contact)
{
	int							item_count;
	float						hit_t;
	d3pnt						ept;
	d3vct						vct;
	matrix_type					mat;
	ray_trace_check_item_type	item_list[ray_trace_max_check_item];
		
		// clear the contact
		
	ray_trace_contact_clear(contact);
	
		// get vector
		
	vct.x=0.0f;
	vct.y=0.0f;
	vct.z=(float)-dist;

	if (ang->x!=0.0f) {
		matrix_rotate_x(&mat,ang->x);
		matrix_vertex_multiply(&mat,&vct.x,&vct.y,&vct.z);
	}
	
	if (ang->z!=0.0f) {
		matrix_rotate_z(&mat,ang->z);
		matrix_vertex_multiply(&mat,&vct.x,&vct.y,&vct.z);
	}
	
	if (ang->y!=0.0f) {
		matrix_rotate_y(&mat,ang->y);
		matrix_vertex_multiply(&mat,&vct.x,&vct.y,&vct.z);
	}
	
		// get end point
	
	ept.x=spt->x+(int)vct.x;
	ept.y=spt->y+(int)vct.y;
	ept.z=spt->z+(int)vct.z;
	
		// setup item list
		
	item_count=ray_trace_map_item_list_setup(1,spt,&ept,contact,item_list);
	
		// default is past end of line
		
	hit_t=2.0f;
	
	hpt->x=ept.x;
	hpt->y=ept.y;
	hpt->z=ept.z;
		
		// ray trace map

	ray_trace_map(item_count,item_list,spt,&ept,&vct,hpt,&hit_t,contact);
	
	return((hit_t>=0.0f) && (hit_t<=1.0f));
}

bool ray_trace_map_by_point(d3pnt *spt,d3pnt *ept,d3pnt *hpt,ray_trace_contact_type *contact)
{
	int							item_count;
	float						hit_t;
	d3vct						vct;
	ray_trace_check_item_type	item_list[ray_trace_max_check_item];
		
		// setup item list
		
	item_count=ray_trace_map_item_list_setup(1,spt,ept,contact,item_list);
	
		// clear the contact
		
	ray_trace_contact_clear(contact);
	
		// create vector from points

	vct.x=(float)(ept->x-spt->x);
	vct.y=(float)(ept->y-spt->y);
	vct.z=(float)(ept->z-spt->z);
	
		// default is past end of line
		
	hit_t=2.0f;
	
	hpt->x=ept->x;
	hpt->y=ept->y;
	hpt->z=ept->z;
	
		// check against map

	ray_trace_map(item_count,item_list,spt,ept,&vct,hpt,&hit_t,contact);
	
	return((hit_t>=0.0f) && (hit_t<=1.0f));
}

void ray_trace_map_by_point_array(int cnt,d3pnt *spt,d3pnt *ept,d3pnt *hpt,bool *hits,ray_trace_contact_type *base_contact,ray_trace_contact_type *contacts)
{
	int							n,item_count;
	float						hit_t;
	d3vct						vct;
	ray_trace_check_item_type	item_list[ray_trace_max_check_item];
	
		// setup item list
		
	item_count=ray_trace_map_item_list_setup(cnt,spt,ept,base_contact,item_list);
	
		// run the ray array
		
	for (n=0;n!=cnt;n++) {
	
			// create contact from base
			
		memmove(&contacts[n],base_contact,sizeof(ray_trace_contact_type));
		ray_trace_contact_clear(&contacts[n]);
	
			// create vector from points

		vct.x=(float)(ept[n].x-spt[n].x);
		vct.y=(float)(ept[n].y-spt[n].y);
		vct.z=(float)(ept[n].z-spt[n].z);
		
			// default is past end of line
			
		hit_t=2.0f;
		
		hpt[n].x=ept[n].x;
		hpt[n].y=ept[n].y;
		hpt[n].z=ept[n].z;
		
			// check against map

		ray_trace_map(item_count,item_list,&spt[n],&ept[n],&vct,&hpt[n],&hit_t,&contacts[n]);
		
		hits[n]=(hit_t>=0.0f) && (hit_t<=1.0f);
	}
}

