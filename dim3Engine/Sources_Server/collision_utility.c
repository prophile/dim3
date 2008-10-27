/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Collision Utility Routines

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

#include "physics.h"

extern map_type			map;
extern server_type		server;

/* =======================================================

      Colliding Object to Object
      
======================================================= */

void collide_object_polygon(obj_type *obj,int x_add,int z_add,int *px,int *pz)
{
	int			x,z,sz;
	float		rang;

	x=obj->pnt.x+x_add;
	sz=obj->size.x>>1;

	px[0]=px[3]=x-sz;
	px[1]=px[2]=x+sz;
	
	z=obj->pnt.z+z_add;
	sz=obj->size.z>>1;

	pz[0]=pz[1]=z-sz;
	pz[2]=pz[3]=z+sz;

	rang=angle_add(obj->ang.y,obj->draw.rot.y);
	rotate_2D_polygon(4,px,pz,x,z,rang);
}

void collide_object_hit_box_polygon(obj_type *obj,model_hit_box_type *hit_box,int *px,int *pz)
{
	int				x,z,x2,z2,wid;
	float			rang;

	x=obj->pnt.x+obj->draw.offset.x;
	z=obj->pnt.z+obj->draw.offset.z;

	x2=x+hit_box->box.offset.x;
	wid=hit_box->box.size.x>>1;
	px[0]=px[3]=x2-wid;
	px[1]=px[2]=x2+wid;
	
	z2=z+hit_box->box.offset.z;
	wid=hit_box->box.size.z>>1;
	pz[0]=pz[1]=z2-wid;
	pz[2]=pz[3]=z2+wid;
	
	rang=angle_add(obj->ang.y,obj->draw.rot.y);
	rotate_2D_polygon(4,px,pz,x,z,rang);
}

bool collide_object_to_object(obj_type *obj1,int x_add,int z_add,obj_type *obj2,bool include_y,bool include_stand)
{
	int		x,z,ls1,ms1,ls2,ms2,d,radius_1,radius_2;
	int		px_1[4],pz_1[4],px_2[4],pz_2[4];

		// check largest bounding circle first

	radius_1=obj1->size.x;
	if (obj1->size.z>radius_1) radius_1=obj1->size.z;

	radius_2=obj2->size.x;
	if (obj2->size.z>radius_2) radius_2=obj2->size.z;

	x=(obj1->pnt.x+x_add)-obj2->pnt.x;
	z=(obj1->pnt.z+z_add)-obj2->pnt.z;
	
	d=(int)sqrtf((float)((x*x)+(z*z)));
	if (d>(radius_1+radius_2)) return(FALSE);

		// check y
		
	if (include_y) {
		ms1=obj1->pnt.y;
		ls1=ms1-obj1->size.y;
		ms2=obj2->pnt.y;
		ls2=ms2-obj2->size.y;
		
		if (!include_stand) {
			if (ms1<=ls2) return(FALSE);
			if (ls1>=ms2) return(FALSE);
		}
		else {
			if (ms1<ls2) return(FALSE);
			if (ls1>=ms2) return(FALSE);
		}
	}

		// finally do polygonal collisions
		// use on vertex inside another polygon method

	collide_object_polygon(obj1,x_add,z_add,px_1,pz_1);
	polygon_2D_collision_setup(4,px_1,pz_1);

	collide_object_polygon(obj2,0,0,px_2,pz_2);
	return(polygon_2D_collision_polygon(4,px_2,pz_2));
}

bool collide_object_to_object_hit_box(obj_type *obj1,int x_add,int z_add,obj_type *obj2,model_hit_box_type *hit_box)
{
	int		x,z,ls1,ms1,ls2,ms2,d,radius_1,radius_2;
	int		px[4],pz[4];

		// check largest bounding circle first

	radius_1=obj1->size.x;
	if (obj1->size.z>radius_1) radius_1=obj1->size.z;

	radius_2=hit_box->box.size.x;
	if (hit_box->box.size.z>radius_2) radius_2=hit_box->box.size.z;

	x=(obj1->pnt.x+x_add)-(obj2->pnt.x+obj2->draw.offset.x+hit_box->box.offset.x);
	z=(obj1->pnt.z+z_add)-(obj2->pnt.z+obj2->draw.offset.z+hit_box->box.offset.z);
	
	d=(int)sqrtf((float)((x*x)+(z*z)));
	if (d>(radius_1+radius_2)) return(FALSE);

		// check y
		
	ms1=obj1->pnt.y;
	ls1=ms1-obj1->size.y;
	ms2=obj2->pnt.y+obj2->draw.offset.y+hit_box->box.offset.y;
	ls2=ms2-hit_box->box.size.y;
		
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);

		// finally do polygonal collisions
		// use on vertex inside another polygon method

	collide_object_polygon(obj1,x_add,z_add,px,pz);
	polygon_2D_collision_setup(4,px,pz);

	collide_object_hit_box_polygon(obj2,hit_box,px,pz);
	return(polygon_2D_collision_polygon(4,px,pz));
}

/* =======================================================

      Get Object to Object Collision Slide Line
      
======================================================= */

bool collide_polygon_to_polygon_slide(int *px_1,int *pz_1,int *px_2,int *pz_2,int *lx,int *rx,int *lz,int *rz)
{
	int		n,n2,k,k2;

		// get sliding angle which is the angle
		// of all the lines hit

	for (n=0;n!=4;n++) {
		n2=(n+1)&0x3;
		
		for (k=0;k!=4;k++) {
			k2=(k+1)&0x3;

				// is there a hit?

			if (!line_2D_test_intersect(px_1[n],pz_1[n],px_1[n2],pz_1[n2],px_2[k],pz_2[k],px_2[k2],pz_2[k2])) continue;
			
				// return the hit
				
			*lx=px_2[k];
			*rx=px_2[k2];
			*lz=pz_2[k];
			*rz=pz_2[k2];
			
			return(TRUE);
		}
	}
	
	return(FALSE);
}

bool collide_object_to_object_slide(obj_type *obj1,int x_add,int z_add,obj_type *obj2,int *lx,int *rx,int *lz,int *rz)
{
	int		px_1[4],pz_1[4],px_2[4],pz_2[4];

		// get collision polygons

	collide_object_polygon(obj1,x_add,z_add,px_1,pz_1);
	collide_object_polygon(obj2,0,0,px_2,pz_2);

		// slide between the polygons

	return(collide_polygon_to_polygon_slide(px_1,pz_1,px_2,pz_2,lx,rx,lz,rz));
}

bool collide_object_to_object_hit_box_slide(obj_type *obj1,int x_add,int z_add,obj_type *obj2,model_hit_box_type *hit_box,int *lx,int *rx,int *lz,int *rz)
{
	int		px_1[4],pz_1[4],px_2[4],pz_2[4];

		// get collision polygons

	collide_object_polygon(obj1,x_add,z_add,px_1,pz_1);
	collide_object_hit_box_polygon(obj2,hit_box,px_2,pz_2);

		// slide between the polygons

	return(collide_polygon_to_polygon_slide(px_1,pz_1,px_2,pz_2,lx,rx,lz,rz));
}

/* =======================================================

      Colliding Projectile to Object
      
======================================================= */

void collide_projectile_polygon(proj_type *proj,int *px,int *pz)
{
	int			x,z,sz;
	float		rang;

	x=proj->pnt.x;
	sz=proj->size.x>>1;

	px[0]=px[3]=x-sz;
	px[1]=px[2]=x+sz;
	
	z=proj->pnt.z;
	sz=proj->size.z>>1;

	pz[0]=pz[1]=z-sz;
	pz[2]=pz[3]=z+sz;

	rang=angle_add(proj->ang.y,proj->draw.rot.y);
	rotate_2D_polygon(4,px,pz,x,z,rang);
}

bool collide_projectile_to_object(proj_type *proj,obj_type *obj)
{
	int		x,z,ls1,ms1,ls2,ms2,d,
			radius,px[4],pz[4];
	
		// check largest bounding circle first

	radius=obj->size.x;
	if (obj->size.z>radius) radius=obj->size.z;

	x=obj->pnt.x-proj->pnt.x;
	z=obj->pnt.z-proj->pnt.z;
	
	d=(int)sqrtf((float)((x*x)+(z*z)));
	if (d>(radius+proj->size.radius)) return(FALSE);
	
		// check y
		
	ms1=proj->pnt.y;
	ls1=ms1-proj->size.y;
	ms2=obj->pnt.y;
	ls2=ms2-obj->size.y;
	
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);

		// finally do polygonal collisions
		// use on vertex inside another polygon method

	collide_projectile_polygon(proj,px,pz);
	polygon_2D_collision_setup(4,px,pz);

	collide_object_polygon(obj,0,0,px,pz);
	return(polygon_2D_collision_polygon(4,px,pz));
}

bool collide_projectile_to_object_hit_box(proj_type *proj,obj_type *obj,model_hit_box_type *hit_box)
{
	int					ls1,ms1,ls2,ms2,
						px[4],pz[4];
	
		// check y
		
	ms1=proj->pnt.y;
	ls1=ms1-proj->size.y;
	ms2=obj->pnt.y+obj->draw.offset.y+hit_box->box.offset.y;
	ls2=ms2-hit_box->box.size.y;
	
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);
	
		// 2D x/z polygon to polygon collision

	collide_projectile_polygon(proj,px,pz);
	polygon_2D_collision_setup(4,px,pz);
	
	collide_object_hit_box_polygon(obj,hit_box,px,pz);
	return(polygon_2D_collision_polygon(4,px,pz));
}

/* =======================================================

      Colliding Projectile to Projectile
	  Note: Projectiles are assumed to be small
	        so this just uses circles
      
======================================================= */

bool collide_projectile_to_projectile(proj_type *proj1,proj_type *proj2)
{
	int		x,z,ls1,ms1,ls2,ms2,d;
	
		// check x
		
	x=proj1->pnt.x;
	ls1=x-proj1->size.radius;
	ms1=x+proj1->size.radius;
	
	x=proj2->pnt.x;
	ls2=x-proj2->size.radius;
	ms2=x+proj2->size.radius;
		
	if (ms1<ls2) return(FALSE);
	if (ls1>ms2) return(FALSE);
	
		// check z
		
	z=proj1->pnt.z;
	ls1=z-proj1->size.radius;
	ms1=z+proj1->size.radius;
	
	z=proj2->pnt.z;
	ls2=z-proj2->size.radius;
	ms2=z+proj2->size.radius;
		
	if (ms1<ls2) return(FALSE);
	if (ls1>ms2) return(FALSE);
	
		// check y
		
	ms1=proj1->pnt.y;
	ls1=ms1-proj1->size.y;
	ms2=proj2->pnt.y;
	ls2=ms2-proj2->size.y;
	
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);
	
		// check radius
		
	x=proj1->pnt.x-proj2->pnt.x;
	z=proj1->pnt.z-proj2->pnt.z;
	
	d=(int)sqrtf((float)((x*x)+(z*z)));
	return(d<=(proj1->size.radius+proj2->size.radius));
}

/* =======================================================

      Colliding Sphere to Object
      
======================================================= */

bool collide_sphere_to_object(int sx,int sy,int sz,int radius,obj_type *obj)
{
	int		x,z,ls1,ms1,ls2,ms2,d;
	
		// check x
		
	ls1=sx-radius;
	ms1=sx+radius;
	
	x=obj->pnt.x;
	ls2=x-obj->size.radius;
	ms2=x+obj->size.radius;
		
	if (ms1<ls2) return(FALSE);
	if (ls1>ms2) return(FALSE);
	
		// check z
		
	ls1=sz-radius;
	ms1=sz+radius;
	
	z=obj->pnt.z;
	ls2=z-obj->size.radius;
	ms2=z+obj->size.radius;
		
	if (ms1<ls2) return(FALSE);
	if (ls1>ms2) return(FALSE);
	
		// check y
		
	ls1=sy-radius;
	ms1=sy+radius;
	ms2=obj->pnt.y;
	ls2=ms2-obj->size.y;
	
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);
	
		// check radius
		
	x=sx-obj->pnt.x;
	z=sz-obj->pnt.z;
	
	d=(int)sqrtf((float)((x*x)+(z*z)));
	return(d<=(radius+obj->size.radius));
}

bool collide_sphere_to_object_hit_box(int sx,int sy,int sz,int radius,obj_type *obj,model_hit_box_type *hit_box)
{
	int				x,z,wid,ls1,ms1,ls2,ms2,
					px[4],pz[4];
	float			rang;
	
		// check y
		
	ls1=sy-radius;
	ms1=sy+radius;
	ms2=obj->pnt.y+obj->draw.offset.y+hit_box->box.offset.y;
	ls2=ms2-hit_box->box.size.y;
	
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);
	
		// 2D x/z polygon to polygon collision

	px[0]=px[3]=sx-radius;
	px[1]=px[2]=sx+radius;
	
	pz[0]=pz[1]=sz-radius;
	pz[2]=pz[3]=sz+radius;
	
	polygon_2D_collision_setup(4,px,pz);
	
	x=obj->pnt.x+obj->draw.offset.x+hit_box->box.offset.x;
	wid=hit_box->box.size.x>>1;
	px[0]=px[3]=x-wid;
	px[1]=px[2]=x+wid;
	
	z=obj->pnt.z+obj->draw.offset.z+hit_box->box.offset.z;
	wid=hit_box->box.size.z>>1;
	pz[0]=pz[1]=z-wid;
	pz[2]=pz[3]=z+wid;
	
	x=obj->pnt.x+obj->draw.offset.x;
	z=obj->pnt.z+obj->draw.offset.z;
	rang=angle_add(obj->ang.y,obj->draw.rot.y);
	
	rotate_2D_polygon(4,px,pz,x,z,rang);
	return(polygon_2D_collision_polygon(4,px,pz));
}

/* =======================================================

      Colliding Sphere to Projectile
      
======================================================= */

bool collide_sphere_to_projectile(int sx,int sy,int sz,int radius,proj_type *proj)
{
	int		x,z,ls1,ms1,ls2,ms2,d;
	
		// check x
		
	ls1=sx-radius;
	ms1=sx+radius;
	
	x=proj->pnt.x;
	ls2=x-proj->size.radius;
	ms2=x+proj->size.radius;
		
	if (ms1<ls2) return(FALSE);
	if (ls1>ms2) return(FALSE);
	
		// check z
		
	ls1=sz-radius;
	ms1=sz+radius;
	
	z=proj->pnt.z;
	ls2=z-proj->size.radius;
	ms2=z+proj->size.radius;
		
	if (ms1<ls2) return(FALSE);
	if (ls1>ms2) return(FALSE);
	
		// check y's first
		
	ls1=sy-radius;
	ms1=sy+radius;
	ms2=proj->pnt.y;
	ls2=ms2-proj->size.y;
	
	if (ms1<=ls2) return(FALSE);
	if (ls1>=ms2) return(FALSE);
	
		// check radius
		
	x=sx-proj->pnt.x;
	z=sz-proj->pnt.z;
	
	d=(int)sqrtf((float)((x*x)+(z*z)));
	return(d<=(radius+proj->size.radius));
}

