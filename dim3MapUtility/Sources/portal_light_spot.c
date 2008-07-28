/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Portal Light Spots

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

d3col					map_light_base;


// supergumba -- will more than likely be able to delete all of this

/* 

void map_set_light_base(d3col *col)
{
	map_light_base.r=col->r;
	map_light_base.g=col->g;
	map_light_base.b=col->b;
}


bool map_portal_light_hit(portal_type *portal,int x,int y,int z,int intensity)
{
	if ((x+intensity)<portal->x) return(FALSE);
	if ((x-intensity)>portal->ex) return(FALSE);

	if ((z+intensity)<portal->z) return(FALSE);
	if ((z-intensity)>portal->ez) return(FALSE);

	if ((y+intensity)<portal->ty) return(FALSE);
	if ((y-intensity)>portal->by) return(FALSE);

	return(TRUE);
}


void map_portal_clear_lights(portal_type *portal)
{
	portal->light.nspot=0;
}

void map_portal_add_light(map_type *map,portal_type *portal,light_spot_type *lspot)
{
	if (portal->light.nspot==max_light_spot) return;
	
		// check if light hits this portal

	if (!map_portal_light_hit(portal,lspot->pnt.x,lspot->pnt.y,lspot->pnt.z,(int)lspot->intensity)) return;

		// calculate inverses

	lspot->inv_intensity=1.0/lspot->intensity;
	
	lspot->d_intensity=lspot->intensity*lspot->intensity;
	lspot->d_inv_intensity=1.0/lspot->d_intensity;
	
		// use this constant to get rough calculation close to better sqrt calc
		
	lspot->d_inv_intensity*=map->ambient.light_drop_off_factor;
	
		// double light values
		
	lspot->d_x=(double)lspot->pnt.x;
	lspot->d_y=(double)lspot->pnt.y;
	lspot->d_z=(double)lspot->pnt.z;
	
	lspot->d_col_r=(double)lspot->col.r;
	lspot->d_col_g=(double)lspot->col.g;
	lspot->d_col_b=(double)lspot->col.b;
		
		// add light to portal

	memmove(&portal->light.spots[portal->light.nspot],lspot,sizeof(light_spot_type));
	portal->light.nspot++;
}


bool map_portal_light_check_changes_single_spot(light_spot_type *lspot,light_spot_type *lspot2)
{
	if (lspot->pnt.x!=lspot2->pnt.x) return(TRUE);
	if (lspot->pnt.y!=lspot2->pnt.y) return(TRUE);
	if (lspot->pnt.z!=lspot2->pnt.z) return(TRUE);
	if (lspot->intensity!=lspot2->intensity) return(TRUE);
	if (lspot->col.r!=lspot2->col.r) return(TRUE);
	if (lspot->col.g!=lspot2->col.g) return(TRUE);
	return(lspot->col.b!=lspot2->col.b);
}

bool map_portal_light_check_changes(portal_type *portal)
{
	int							n,nspot,sz;
	bool						changed;
	light_spot_type				*lspot,*lspot2;

	nspot=portal->light.nspot;

		// check new light with last light

	changed=(nspot!=portal->light_last.nspot);

	if (!changed) {

		lspot=portal->light.spots;
		lspot2=portal->light_last.spots;
		
		for (n=0;n!=nspot;n++) {
			if (map_portal_light_check_changes_single_spot(lspot,lspot2)) {
				changed=TRUE;
				break;
			}
			lspot++;
			lspot2++;
		}
	}

		// if changed, backup last lighting

	if (!changed) return(FALSE);

	sz=nspot*sizeof(light_spot_type);
	memmove(portal->light_last.spots,portal->light.spots,sz);

	portal->light_last.nspot=nspot;

	return(TRUE);
}

void map_portal_light_check_changes_reset(portal_type *portal)
{
	portal->light_last.nspot=-1;
}

void map_portal_light_check_changes_reset_all(map_type *map)
{
	int					n;
	portal_type			*portal;

	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
		map_portal_light_check_changes_reset(portal);
		portal++;
	}
}


bool map_portal_light_in_view(map_type *map,int x,int y,int z,int intensity)
{
	int					i;
	portal_type			*portal;
	
	portal=map->portals;
	
	for (i=0;i!=map->nportal;i++) {
		if (portal->in_path) {
			if (map_portal_light_hit(portal,x,y,z,intensity)) return(TRUE);
		}
		portal++;
	}
	
	return(FALSE);
}


void map_portal_get_light(portal_type *portal,double x,double y,double z,d3col *col)
{
	int						i,nspot;
	double					mult,r,g,b,d,dx,dz,dy;
	light_spot_type			*lspot;

		// map default lighting
	
	r=g=b=0;
	
		// lights from portal
		
	nspot=portal->light.nspot;
	lspot=portal->light.spots;
	
	for (i=0;i!=nspot;i++) {
		dx=lspot->d_x-x;
		dz=lspot->d_z-z;
		dy=lspot->d_y-y;
		
		d=(dx*dx)+(dz*dz)+(dy*dy);
		if (d<=lspot->d_intensity) {
			mult=(lspot->d_intensity-d)*lspot->d_inv_intensity;
			r+=(lspot->d_col_r*mult);
			g+=(lspot->d_col_g*mult);
			b+=(lspot->d_col_b*mult);
		}
		
		lspot++;
	}

	col->r=map_light_base.r+(float)r;
	col->g=map_light_base.g+(float)g;
	col->b=map_light_base.b+(float)b;
}


light_spot_type* map_portal_find_closest_light(portal_type *portal,double x,double y,double z,int *p_dist)
{
	int					i,k,nspot;
	double				dx,dz,dy,d,dist;
	light_spot_type		*lspot;
	
	k=-1;
	dist=-1;
	
	nspot=portal->light.nspot;
	lspot=portal->light.spots;
	
	for (i=0;i!=nspot;i++) {
		
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
				k=i;
			}
		}
		
		lspot++;
	}
	
	if (k==-1) return(NULL);
	
	if (p_dist!=NULL) *p_dist=(int)sqrt(dist);

	return(&portal->light.spots[k]);
}

void map_portal_calculate_light_color(portal_type *portal,double x,double y,double z,float *cf)
{
	int					i,nspot;
	double				mult,r,g,b,dx,dz,dy,d;
	light_spot_type		*lspot;

		// combine all light spot colors
		// attenuated for distance
		
	nspot=portal->light.nspot;
	lspot=portal->light.spots;
	
	r=g=b=0;
	
	for (i=0;i!=nspot;i++) {
		
		dx=lspot->d_x-x;
		dy=lspot->d_y-y;
		dz=lspot->d_z-z;
		
		d=(dx*dx)+(dz*dz)+(dy*dy);
		if (d<=lspot->d_intensity) {
			mult=(lspot->d_intensity-d)*lspot->d_inv_intensity;
			r+=(lspot->d_col_r*mult);
			g+=(lspot->d_col_g*mult);
			b+=(lspot->d_col_b*mult);
		}
		
		lspot++;
	}

	*cf++=map_light_base.r+(float)r;
	*cf++=map_light_base.g+(float)g;
	*cf=map_light_base.b+(float)b;
}

void map_portal_calculate_light_normal(portal_type *portal,double x,double y,double z,float *nf)
{
	int					i,nspot,cnt;
	double				dx,dz,dy,nx,nz,ny,d,mult;
	light_spot_type		*lspot;

		// combine all light spot normals
		// attenuated for distance
		
	cnt=0;
	
	nx=ny=nz=0;
	
	nspot=portal->light.nspot;
	lspot=portal->light.spots;
	
	for (i=0;i!=nspot;i++) {
		
		dx=lspot->d_x-x;
		dy=lspot->d_y-y;
		dz=lspot->d_z-z;
		
		d=(dx*dx)+(dz*dz)+(dy*dy);
		if (d<=lspot->d_intensity) {
			mult=(lspot->d_intensity-d)*lspot->d_inv_intensity;
			nx+=(dx*mult);
			ny+=(dy*mult);
			nz+=(dz*mult);

			cnt++;
		}
		
		lspot++;
	}
	
		// no hits, then default normal

	if (cnt==0) {
		*nf++=0.5f;
		*nf++=0.5f;
		*nf=1.0f;
		return;
	}
	
		// average the vector
		
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

		// normalize

	d=sqrt((nx*nx)+(ny*ny));
	if (d!=0.0) {
		d=1.0/d;
		nx*=d;
		ny*=d;
	}
	
		// convert to needed format
		// x (1 = right [light from left], 0 = left [light from right])
		// y (1 = top [light from bottom], 0 = bottom [light from top])
		
	*nf++=(float)((nx*0.5)+0.5);
	*nf++=1.0f-(float)((ny*0.5)+0.5);
	*nf=1.0f;
}

void map_portal_calculate_light_color_normal(portal_type *portal,double x,double y,double z,float *cf,float *nf)
{
	int					i,nspot,cnt;
	double				dx,dz,dy,r,g,b,nx,nz,ny,d,mult;
	light_spot_type		*lspot;

		// combine all light spot normals
		// attenuated for distance
		
	cnt=0;
	
	r=g=b=0;
	nx=ny=nz=0;
	
	nspot=portal->light.nspot;
	lspot=portal->light.spots;
	
	for (i=0;i!=nspot;i++) {
		
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

	*cf++=map_light_base.r+(float)r;
	*cf++=map_light_base.g+(float)g;
	*cf=map_light_base.b+(float)b;
	
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


bool map_portal_create_light_spots(map_type *map)
{
	int					i,sz;
	portal_type			*portal;
	
	portal=map->portals;
	sz=max_light_spot*sizeof(light_spot_type);
	
	for (i=0;i!=map->nportal;i++) {
		portal->light.spots=(light_spot_type*)valloc(sz);
		if (portal->light.spots==NULL) return(FALSE);

		portal->light_last.spots=(light_spot_type*)valloc(sz);
		if (portal->light_last.spots==NULL) {
			free(portal->light.spots);
			return(FALSE);
		}

		bzero(portal->light.spots,sz);
		bzero(portal->light_last.spots,sz);

		portal->light.nspot=0;
		portal->light_last.nspot=-1;
		
		portal++;
	}
	
	return(TRUE);
}

void map_portal_dispose_light_spots(map_type *map)
{
	int					i;
	portal_type			*portal;
	
	portal=map->portals;
	
	for (i=0;i!=map->nportal;i++) {
		free(portal->light.spots);
		free(portal->light_last.spots);
		portal++;
	}
}

*/