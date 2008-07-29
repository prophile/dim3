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
	contact->obj_uid=-1;
	contact->proj_uid=-1;
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

float ray_trace_rotated_box(d3pnt *spt,d3vct *vct,d3pnt *hpt,int x,int z,int lx,int rx,int tz,int bz,int ty,int by,float rang)
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
		return(hit_t);
	}
	
	return(-1.0f);
}

/* =======================================================

      Ray Trace Objects
      
======================================================= */

float ray_trace_object(d3pnt *spt,d3pnt *ept,d3vct *vct,d3pnt *hpt,obj_type *obj)
{
	int					n,nhit_box,wid,
						x,y,z,lx,rx,tz,bz,ty,by,radius,
						x2,z2;
	float				rang,hit_t;
	model_draw			*draw;
	model_type			*model;
	model_hit_box_type	*hit_box;
	
		// rough y check
		
	ty=obj->pos.y-obj->size.y;
	if ((spt->y<ty) && (ept->y<ty)) return(-1.0f);
	
	y=by=obj->pos.y;
	if ((spt->y>by) && (ept->y>by)) return(-1.0f);
	
		// rough x/z checks
		
	x=obj->pos.x;
	z=obj->pos.z;

	radius=obj->size.x;
	if (obj->size.z>radius) radius=obj->size.z;
	
	radius=radius>>1;
	
	lx=x-radius;
	if ((spt->x<lx) && (ept->x<lx)) return(-1.0f);
	
	rx=x+radius;
	if ((spt->x>rx) && (ept->x>rx)) return(-1.0f);
	
	tz=z-radius;
	if ((spt->z<tz) && (ept->z<tz)) return(-1.0f);
	
	bz=z+radius;
	if ((spt->z>bz) && (ept->z>bz)) return(-1.0f);
	
		// setup fine check
		
	wid=obj->size.x>>1;
	lx=x-wid;
	rx=x+wid;
	
	wid=obj->size.z>>1;
	tz=z-wid;
	bz=z+wid;
		
	rang=angle_add(obj->ang.y,obj->draw.rot.y);

		// regular collisions

	if (!obj->hit_box.on) {
		return(ray_trace_rotated_box(spt,vct,hpt,x,z,lx,rx,tz,bz,ty,by,rang));
	}

		// hit box collisions
		
	obj->hit_box.hit=FALSE;
	
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
		
		hit_t=ray_trace_rotated_box(spt,vct,hpt,x,z,lx,rx,tz,bz,ty,by,rang);
		if (hit_t!=-1.0f) {
			obj->hit_box.hit=TRUE;
			obj->hit_box.model_uid=draw->uid;
			obj->hit_box.hit_box_idx=n;
			return(hit_t);
		}

		hit_box++;
	}

	return(-1.0f);
}

/* =======================================================

      Ray Trace Projectiles
      
======================================================= */

float ray_trace_projectile(d3pnt *spt,d3pnt *ept,d3vct *vct,d3pnt *hpt,proj_type *proj)
{
	int			wid,x,z,lx,rx,tz,bz,ty,by,radius;
	float		rang;
	
		// rough y check
		
	ty=proj->pos.y-proj->size.y;
	if ((spt->y<ty) && (ept->y<ty)) return(-1.0f);
	
	by=proj->pos.y;
	if ((spt->y>by) && (ept->y>by)) return(-1.0f);
	
		// rough x/z checks
		
	x=proj->pos.x;
	z=proj->pos.z;

	radius=proj->size.radius>>1;
	
	lx=x-radius;
	if ((spt->x<lx) && (ept->x<lx)) return(-1.0f);
	
	rx=x+radius;
	if ((spt->x>rx) && (ept->x>rx)) return(-1.0f);
	
	tz=z-radius;
	if ((spt->z<tz) && (ept->z<tz)) return(-1.0f);
	
	bz=z+radius;
	if ((spt->z>bz) && (ept->z>bz)) return(-1.0f);
	
		// setup fine check
		
	wid=proj->size.x>>1;
	lx=x-wid;
	rx=x+wid;
	
	wid=proj->size.z>>1;
	tz=z-wid;
	bz=z+wid;
		
	rang=angle_add(proj->ang.y,proj->draw.rot.y);

		// box collisions

	return(ray_trace_rotated_box(spt,vct,hpt,x,z,lx,rx,tz,bz,ty,by,rang));
}

/* =======================================================

      Ray Trace Map
      
======================================================= */

void ray_trace_map(d3pnt *spt,d3pnt *ept,d3vct *vct,d3pnt *hpt,float *hit_t,ray_trace_contact_type *contact)
{
	int					n,k;
	float				t;
	d3pnt				pt;
	obj_type			*obj;
	proj_type			*proj;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
	pt.x=pt.y=pt.z=0;
	
		// check objects
		
	if (contact->obj_on) {
	
		for (n=0;n!=server.count.obj;n++) {
			obj=&server.objs[n];
			if ((obj->hidden) || (obj->pickup.on)) continue;
			if (obj->uid==contact->obj_ignore_uid) continue;
			if ((contact->origin==poly_ray_trace_origin_object) && (!obj->contact.object_on)) continue;
			if ((contact->origin==poly_ray_trace_origin_projectile) && (!obj->contact.projectile_on)) continue;
			
			t=ray_trace_object(spt,ept,vct,&pt,obj);
			if (t==-1.0f) continue;
				
				// closer hit?
					
			if (t>=(*hit_t)) continue;
				
			*hit_t=t;
			hpt->x=pt.x;
			hpt->y=pt.y;
			hpt->z=pt.z;
			
			ray_trace_contact_clear(contact);
			contact->obj_uid=obj->uid;
		}
		
	}
	
		// check projectiles
		
	if (contact->proj_on) {
	
		for (n=0;n!=server.count.proj;n++) {
			proj=&server.projs[n];
			if (proj->uid==contact->proj_ignore_uid) continue;
			
			t=ray_trace_projectile(spt,ept,vct,&pt,proj);
			if (t==-1.0f) continue;
				
				// closer hit?
					
			if (t>=(*hit_t)) continue;
				
			*hit_t=t;
			hpt->x=pt.x;
			hpt->y=pt.y;
			hpt->z=pt.z;
			
			ray_trace_contact_clear(contact);
			contact->proj_uid=proj->uid;
		}
	
	}

		// check the meshes

	for (n=0;n!=map.mesh.nmesh;n++) {
		
		mesh=&map.mesh.meshes[n];
		
			// skip pass through meshes

		if (mesh->flag.pass_through) {
			mesh++;
			continue;
		}

			// rough bounds check

		if ((spt->y<mesh->box.min.y) && (ept->y<mesh->box.min.y)) continue;
		if ((spt->y>mesh->box.max.y) && (ept->y>mesh->box.max.y)) continue;
		if ((spt->x<mesh->box.min.x) && (ept->x<mesh->box.min.x)) continue;
		if ((spt->x>mesh->box.max.x) && (ept->x>mesh->box.max.x)) continue;
		if ((spt->z<mesh->box.min.z) && (ept->z<mesh->box.min.z)) continue;
		if ((spt->z>mesh->box.max.z) && (ept->z>mesh->box.max.z)) continue;

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

			if ((spt->y<poly->box.min.y) && (ept->y<poly->box.min.y)) continue;
			if ((spt->y>poly->box.max.y) && (ept->y>poly->box.max.y)) continue;
			if ((spt->x<poly->box.min.x) && (ept->x<poly->box.min.x)) continue;
			if ((spt->x>poly->box.max.x) && (ept->x>poly->box.max.x)) continue;
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
			contact->poly.mesh_idx=n;
			contact->poly.poly_idx=k;
		}

		mesh++;
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

	pt->x+=(x*dist)/td;
	pt->y+=(y*dist)/td;
	pt->z+=(z*dist)/td;
}

/* =======================================================

      Ray Trace By Angle and Distance
      
======================================================= */

bool ray_trace_map_by_angle(d3pnt *spt,d3ang *ang,int dist,d3pnt *hpt,ray_trace_contact_type *contact)
{
	float				hit_t;
	d3pnt				ept;
	d3vct				vct;
	matrix_type			mat;
	
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
	
		// default is past end of line
		
	hit_t=2.0f;
	
	hpt->x=ept.x;
	hpt->y=ept.y;
	hpt->z=ept.z;
		
		// ray trace map

	ray_trace_map(spt,&ept,&vct,hpt,&hit_t,contact);
	
	return((hit_t>=0.0f) && (hit_t<=1.0f));
}

/* =======================================================

      Ray Trace By Points
      
======================================================= */

bool ray_trace_map_by_point(d3pnt *spt,d3pnt *ept,d3pnt *hpt,ray_trace_contact_type *contact)
{
	float				hit_t;
	d3vct				vct;
	
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
		
		// check against all portals

	ray_trace_map(spt,ept,&vct,hpt,&hit_t,contact);
	
	return((hit_t>=0.0f) && (hit_t<=1.0f));
}
