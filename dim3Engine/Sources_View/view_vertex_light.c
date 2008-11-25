/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Map Vertex Lighting

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

#include "lights.h"

extern map_type				map;
extern view_type			view;
extern setup_type			setup;

extern int					nlight;
extern light_spot_type		lspot_cache[max_light_spot];

int							nlight_reduce,light_reduce_list[max_light_spot];

extern bool light_trace_map(d3pnt *spt,d3pnt *ept);

/* =======================================================

      Reduce Lighting that Hit Scene
      
======================================================= */

void map_calculate_light_reduce_mesh(map_mesh_type *mesh)
{
	int					n;
	light_spot_type		*lspot;

	nlight_reduce=0;

	for (n=0;n!=nlight;n++) {

		lspot=&lspot_cache[n];

			// does this light hit the mesh?

		if ((lspot->pnt.x+lspot->intensity)<mesh->box.min.x) continue;
		if ((lspot->pnt.x-lspot->intensity)>mesh->box.max.x) continue;
		if ((lspot->pnt.z+lspot->intensity)<mesh->box.min.z) continue;
		if ((lspot->pnt.z-lspot->intensity)>mesh->box.max.z) continue;
		if ((lspot->pnt.y+lspot->intensity)<mesh->box.min.y) continue;
		if ((lspot->pnt.y-lspot->intensity)>mesh->box.max.y) continue;

			// this light can be considered

		light_reduce_list[nlight_reduce++]=n;
	}
}

void map_calculate_light_reduce_liquid(map_liquid_type *liq)
{
	int					n;
	light_spot_type		*lspot;

	nlight_reduce=0;

	for (n=0;n!=nlight;n++) {

		lspot=&lspot_cache[n];

			// does this light hit the liquid?

		if ((lspot->pnt.x+lspot->intensity)<liq->lft) continue;
		if ((lspot->pnt.x-lspot->intensity)>liq->rgt) continue;
		if ((lspot->pnt.z+lspot->intensity)<liq->top) continue;
		if ((lspot->pnt.z-lspot->intensity)>liq->bot) continue;
		if ((lspot->pnt.y+lspot->intensity)<liq->y) continue;
		if ((lspot->pnt.y-lspot->intensity)>liq->y) continue;

			// this light can be considered

		light_reduce_list[nlight_reduce++]=n;
	}
}

void map_calculate_light_reduce_model(model_draw *draw)
{
	int					n,cx,cy,cz,sz;
	float				fx,fy,fz;
	d3pnt				pnt,min,max;
	matrix_type			mat;
	light_spot_type		*lspot;
	
		// need to move model if no rot on

	memmove(&pnt,&draw->pnt,sizeof(d3pnt));
		
	if (draw->no_rot.on) {
		matrix_rotate_y(&mat,draw->no_rot.ang.y);

		fx=(float)(pnt.x-draw->no_rot.center.x);
		fy=(float)(pnt.y-draw->no_rot.center.y);
		fz=(float)(pnt.z-draw->no_rot.center.z);
		
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
		
		pnt.x=((int)fx)+draw->no_rot.center.x;
		pnt.y=((int)fy)+draw->no_rot.center.y;
		pnt.z=((int)fz)+draw->no_rot.center.z;
	}

		// get model bounds

	sz=draw->size.x>>1;
	min.x=pnt.x-sz;
	max.x=pnt.x+sz;

	sz=draw->size.z>>1;
	min.z=pnt.z-sz;
	max.z=pnt.z+sz;

	min.y=pnt.y-draw->size.y;
	max.y=pnt.y;

		// any rotations

	cx=pnt.x+draw->center.x;
	cy=pnt.y+draw->center.y;
	cz=pnt.z+draw->center.z;

	rotate_point(&min.x,&min.y,&min.z,cx,cy,cz,draw->rot.x,draw->rot.y,draw->rot.z);
	rotate_point(&max.x,&max.y,&max.z,cx,cy,cz,draw->rot.x,draw->rot.y,draw->rot.z);

		// find lights within these bounds

	nlight_reduce=0;

	for (n=0;n!=nlight;n++) {

		lspot=&lspot_cache[n];

			// does this light hit the model?

		if ((lspot->pnt.x+lspot->intensity)<min.x) continue;
		if ((lspot->pnt.x-lspot->intensity)>max.x) continue;
		if ((lspot->pnt.z+lspot->intensity)<min.z) continue;
		if ((lspot->pnt.z-lspot->intensity)>max.z) continue;
		if ((lspot->pnt.y+lspot->intensity)<min.y) continue;
		if ((lspot->pnt.y-lspot->intensity)>max.y) continue;

			// this light can be considered

		light_reduce_list[nlight_reduce++]=n;
	}
}

void map_calculate_light_reduce_effect(effect_type *effect)
{
	int					n,sz;
	d3pnt				min,max;
	light_spot_type		*lspot;

		// get effect bounds

	sz=effect->size>>1;

	min.x=effect->pnt.x-sz;
	max.x=effect->pnt.x+sz;
	min.y=effect->pnt.y-sz;
	max.y=effect->pnt.y+sz;
	min.z=effect->pnt.z-sz;
	max.z=effect->pnt.z+sz;

		// find lights within these bounds

	nlight_reduce=0;

	for (n=0;n!=nlight;n++) {

		lspot=&lspot_cache[n];

			// does this light hit the effect?

		if ((lspot->pnt.x+lspot->intensity)<min.x) continue;
		if ((lspot->pnt.x-lspot->intensity)>max.x) continue;
		if ((lspot->pnt.z+lspot->intensity)<min.z) continue;
		if ((lspot->pnt.z-lspot->intensity)>max.z) continue;
		if ((lspot->pnt.y+lspot->intensity)<min.y) continue;
		if ((lspot->pnt.y-lspot->intensity)>max.y) continue;

			// this light can be considered

		light_reduce_list[nlight_reduce++]=n;
	}
}

void map_calculate_light_reduce_all(void)
{
	int					n;

	for (n=0;n!=nlight;n++) {
		light_reduce_list[n]=n;
	}
	
	nlight_reduce=nlight;
}

/* =======================================================

      Check Reductions for Changes
      
======================================================= */

bool map_calculate_light_reduce_check_equal(map_mesh_type *mesh)
{
	int						n,count;
	float					f_count;
	double					intensity;
	bool					recalc;
	d3pnt					pnt;
	d3col					col;
	light_spot_type			*lspot;
	
		// create light averages to compare
		// against last light calculation
		
	count=0;
	intensity=0.0;
	pnt.x=pnt.y=pnt.z=0;
	col.r=col.g=col.b=0.0f;

	if (nlight_reduce!=0) {
	
		for (n=0;n!=nlight_reduce;n++) {
			lspot=&lspot_cache[light_reduce_list[n]];
			
			intensity+=lspot->intensity;
			pnt.x+=(lspot->pnt.x-mesh->box.mid.x);
			pnt.y+=(lspot->pnt.x-mesh->box.mid.y);
			pnt.z+=(lspot->pnt.x-mesh->box.mid.z);
			col.r+=lspot->col.r;
			col.g+=lspot->col.r;
			col.b+=lspot->col.r;
		}
		
		count=nlight_reduce;
		f_count=(float)count;
		
		intensity/=(double)f_count;
		pnt.x/=count;
		pnt.y/=count;
		pnt.z/=count;
		col.r/=f_count;
		col.g/=f_count;
		col.b/=f_count;
	}
	
		// if light averages have changed, then
		// we will need to rebuild light lists
		
	recalc=(mesh->light_cache.light_count!=count);
	recalc=recalc||(mesh->light_cache.intensity!=intensity);
	recalc=recalc||(mesh->light_cache.pnt.x!=pnt.x);
	recalc=recalc||(mesh->light_cache.pnt.y!=pnt.y);
	recalc=recalc||(mesh->light_cache.pnt.z!=pnt.z);
	recalc=recalc||(mesh->light_cache.col.r!=col.r);
	recalc=recalc||(mesh->light_cache.col.g!=col.g);
	recalc=recalc||(mesh->light_cache.col.b!=col.b);

	if (!recalc) return(TRUE);

	mesh->light_cache.light_count=count;
	mesh->light_cache.intensity=intensity;
	mesh->light_cache.pnt.x=pnt.x;
	mesh->light_cache.pnt.y=pnt.y;
	mesh->light_cache.pnt.z=pnt.z;
	mesh->light_cache.col.r=col.r;
	mesh->light_cache.col.g=col.g;
	mesh->light_cache.col.b=col.b;
	
	return(FALSE);
}

void map_calculate_light_clear_all(void)
{
	int					n;
	map_mesh_type		*mesh;

	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
		mesh->light_cache.light_count=-1;
		mesh++;
	}
}

/* =======================================================

      Normal Lighting
      
======================================================= */

void map_calculate_light_color_normal(double x,double y,double z,float *cf,float *nf)
{
	int					n,cnt;
	double				dx,dz,dy,r,g,b,nx,nz,ny,d,mult;
	light_spot_type		*lspot;

		// no lights in scene

	if (nlight_reduce==0) {
		*cf++=(map.ambient.light_color.r+setup.gamma);
		*cf++=(map.ambient.light_color.g+setup.gamma);
		*cf=(map.ambient.light_color.b+setup.gamma);
		*nf++=0.5f;
		*nf++=0.5f;
		*nf=1.0f;
		return;
	}

		// combine all light spot normals
		// attenuated for distance
		
	cnt=0;
	
	r=g=b=0.0f;
	nx=ny=nz=0.0f;
	
	for (n=0;n!=nlight_reduce;n++) {

		lspot=&lspot_cache[light_reduce_list[n]];

		dx=lspot->d_x-x;
		dy=lspot->d_y-y;
		dz=lspot->d_z-z;
		
		d=(dx*dx)+(dz*dz)+(dy*dy);

		if (d<=lspot->d_intensity) {
			mult=(lspot->d_intensity-d)*lspot->d_inv_intensity;

			r+=(lspot->d_col_r*mult);
			g+=(lspot->d_col_g*mult);
			b+=(lspot->d_col_b*mult);

			nx+=(dx*mult);
			ny+=(dy*mult);
			nz+=(dz*mult);

			cnt++;
		}
		
		lspot++;
	}

		// set light value

	*cf++=(map.ambient.light_color.r+setup.gamma)+(float)r;
	*cf++=(map.ambient.light_color.g+setup.gamma)+(float)g;
	*cf=(map.ambient.light_color.b+setup.gamma)+(float)b;
	
		// no hits, then default normal

	if (cnt==0) {
		*nf++=0.5f;
		*nf++=0.5f;
		*nf=1.0f;
		return;
	}
	
		// average the normal vector
		
	d=(1.0/((double)cnt));
	nx*=d;
	ny*=d;
	nz*=d;
	
		// combine x and z together to make x
		// factor.  Note that this does not always
		// give the correct results but will be close
		// most of the time -- otherwise we're going to
		// have to calculate polygon vectors
		
	nx+=nz;

		// normalize normal vector

	d=sqrt((nx*nx)+(ny*ny));
	if (d!=0.0) {
		d=1.0/d;
		nx*=d;
		ny*=d;
	}
	
		// convert to needed format
		// x (1 = right [light from left], 0 = left [light from right])
		// y (1 = top [light from bottom], 0 = bottom [light from top])

		// supergumba -- possible (check math) that in the future
		// we can use the z coord for a distance hardness factor
		
	*nf++=(float)((nx*0.5)+0.5);
	*nf++=1.0f-(float)((ny*0.5)+0.5);
	*nf=1.0f;
}

light_spot_type* map_find_closest_light(double x,double y,double z,int *p_dist)
{
	int					n,k;
	double				dx,dz,dy,d,dist;
	light_spot_type		*lspot;

		// no lights in scene

	if (nlight_reduce==0) return(NULL);

		// find closest light
	
	k=-1;
	dist=-1;
	
	for (n=0;n!=nlight_reduce;n++) {

		lspot=&lspot_cache[light_reduce_list[n]];
		
			// get distance to light spot
			
		dx=lspot->d_x-x;
		dz=lspot->d_z-z;
		dy=lspot->d_y-y;

		d=(dx*dx)+(dz*dz)+(dy*dy);
		
			// reject lights outside globe
			
		if (d<=lspot->d_intensity) {
		
				// compare distances
		
			if ((d<dist) || (dist==-1)) {
				dist=d;
				k=n;
			}
		}
		
		lspot++;
	}
	
	if (k==-1) return(NULL);
	
	if (p_dist!=NULL) *p_dist=(int)sqrt(dist);

	return(&lspot_cache[k]);
}

/* =======================================================

      Ray Traced Lighting
      
======================================================= */

void map_calculate_ray_trace_light_color_normal(double x,double y,double z,float *cf,float *nf)
{
	int					n,cnt;
	d3pnt				spt,ept;
	double				mult,r,g,b,dx,dz,dy,nx,ny,nz,d;
	light_spot_type		*lspot;

		// no lights in scene

	if (nlight_reduce==0) {
		*cf++=(map.ambient.light_color.r+setup.gamma);
		*cf++=(map.ambient.light_color.g+setup.gamma);
		*cf=(map.ambient.light_color.b+setup.gamma);
		*nf++=0.5f;
		*nf++=0.5f;
		*nf=1.0f;
		return;
	}

		// starting ray trace point

	spt.x=(int)x;
	spt.y=(int)y;
	spt.z=(int)z;

		// combine all light spot colors
		// attenuated for distance
		
	cnt=0;
	
	r=g=b=0.0f;
	nx=ny=nz=0.0f;
	
	for (n=0;n!=nlight_reduce;n++) {

		lspot=&lspot_cache[light_reduce_list[n]];

		ept.x=lspot->pnt.x;
		ept.y=lspot->pnt.y;
		ept.z=lspot->pnt.z;

		if (light_trace_map(&spt,&ept)) continue;
		
		dx=lspot->d_x-x;
		dy=lspot->d_y-y;
		dz=lspot->d_z-z;
		
		d=(dx*dx)+(dz*dz)+(dy*dy);

		if (d<=lspot->d_intensity) {
			mult=(lspot->d_intensity-d)*lspot->d_inv_intensity;

			r+=(lspot->d_col_r*mult);
			g+=(lspot->d_col_g*mult);
			b+=(lspot->d_col_b*mult);

			nx+=(dx*mult);
			ny+=(dy*mult);
			nz+=(dz*mult);

			cnt++;
		}
		
		lspot++;
	}

		// set light value

	*cf++=(map.ambient.light_color.r+setup.gamma)+(float)r;
	*cf++=(map.ambient.light_color.g+setup.gamma)+(float)g;
	*cf=(map.ambient.light_color.b+setup.gamma)+(float)b;

		// no hits, then default normal

	if (cnt==0) {
		*nf++=0.5f;
		*nf++=0.5f;
		*nf=1.0f;
		return;
	}

		// average the normal vector
		
	d=(1.0/((double)cnt));
	nx*=d;
	ny*=d;
	nz*=d;
	
		// combine x and z together to make x
		// factor.  Note that this does not always
		// give the correct results but will be close
		// most of the time -- otherwise we're going to
		// have to calculate polygon vectors
		
	nx+=nz;

		// normalize normal vector

	d=sqrt((nx*nx)+(ny*ny));
	if (d!=0.0) {
		d=1.0/d;
		nx*=d;
		ny*=d;
	}
	
		// convert to needed format
		// x (1 = right [light from left], 0 = left [light from right])
		// y (1 = top [light from bottom], 0 = bottom [light from top])

		// supergumba -- possible (check math) that in the future
		// we can use the z coord for a distance hardness factor
		
	*nf++=(float)((nx*0.5)+0.5);
	*nf++=1.0f-(float)((ny*0.5)+0.5);
	*nf=1.0f;
}







