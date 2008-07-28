/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Portal Vertex Lists

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

extern bool light_trace_map(d3pnt *spt,d3pnt *ept);

/* =======================================================

      Normal Lighting
      
======================================================= */

void map_calculate_light_color_normal(double x,double y,double z,float *cf,float *nf)
{
	int					i,cnt;
	double				dx,dz,dy,r,g,b,nx,nz,ny,d,mult;
	light_spot_type		*lspot;

		// combine all light spot normals
		// attenuated for distance
		
	cnt=0;
	
	r=g=b=0;
	nx=ny=nz=0;
	
	lspot=lspot_cache;
	
	for (i=0;i!=nlight;i++) {
		
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

light_spot_type* map_find_closest_light(portal_type *portal,double x,double y,double z,int *p_dist)
{
	int					i,k;
	double				dx,dz,dy,d,dist;
	light_spot_type		*lspot;
	
	k=-1;
	dist=-1;
	
	lspot=lspot_cache;
	
	for (i=0;i!=nlight;i++) {
		
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

	return(&lspot_cache[k]);
}

/* =======================================================

      Ray Traced Lighting
      
======================================================= */

void light_trace_calculate_light_color(float x,float y,float z,float *cf)
{
	int					n;
	d3pnt				spt,ept;
	double				mult,r,g,b,dx,dz,dy,d;
	light_spot_type		*lspot;

	spt.x=(int)x;
	spt.y=(int)y;
	spt.z=(int)z;

		// combine all light spot colors
		// attenuated for distance
		
	lspot=lspot_cache;
	
	r=g=b=0.0f;
	
	for (n=0;n!=nlight;n++) {

		ept.x=lspot->pnt.x;
		ept.y=lspot->pnt.y;
		ept.z=lspot->pnt.z;

		if (light_trace_map(&spt,&ept)) {
			lspot++;
			continue;
		}
		
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

	*cf++=(map.ambient.light_color.r+setup.gamma)+(float)r;
	*cf++=(map.ambient.light_color.g+setup.gamma)+(float)g;
	*cf=(map.ambient.light_color.b+setup.gamma)+(float)b;
}







