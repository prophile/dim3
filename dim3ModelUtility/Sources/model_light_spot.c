/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Light Spots

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
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Check if Light Shines on Model
      
======================================================= */

bool model_light_hit(model_type *model,d3pos *model_pos,int x,int y,int z,int intensity)
{
	int			sz,mx,my,mz;
	double		dx,dy,dz;

		// this is a rough calc so we expand it
		// by distance to make it more fair
		// under heavy rotations

	dx=(model->view_box.size.x>>1);
	dy=(model->view_box.size.y>>1);
	dz=(model->view_box.size.z>>1);
	sz=(int)sqrt((dx*dx)+(dy*dy)+(dz*dz));

		// is light in view globe?

	mx=model_pos->x;
	my=model_pos->y-(model->view_box.size.y>>1);
	mz=model_pos->z;

	if ((x+intensity)<(mx-sz)) return(FALSE);
	if ((x-intensity)>(mx+sz)) return(FALSE);

	if ((y+intensity)<(my-sz)) return(FALSE);
	if ((y-intensity)>(my+sz)) return(FALSE);

	if ((z+intensity)<(mz-sz)) return(FALSE);
	if ((z-intensity)>(mz+sz)) return(FALSE);

	return(TRUE);
}

/* =======================================================

      Add Light Spot To Model
      
======================================================= */

void model_clear_lights(model_type *model)
{
	model->light.nspot=0;
}

void model_add_light(model_type *model,d3pos *model_pos,light_spot_type *lspot,float drop_off_factor)
{
	if (model->light.nspot==max_model_light_spot) return;
	
		// check if light hits this model

	if (!model_light_hit(model,model_pos,lspot->pnt.x,lspot->pnt.y,lspot->pnt.z,(int)lspot->intensity)) return;

		// calculate inverses

	lspot->inv_intensity=1.0/lspot->intensity;
	
	lspot->d_intensity=lspot->intensity*lspot->intensity;
	lspot->d_inv_intensity=1.0/lspot->d_intensity;
	
		// use this constant to get rough calculation close to better sqrt calc
		
	lspot->d_inv_intensity*=drop_off_factor;
	
		// double light values
		
	lspot->d_x=(double)lspot->pnt.x;
	lspot->d_y=(double)lspot->pnt.y;
	lspot->d_z=(double)lspot->pnt.z;
		
	lspot->d_col_r=(double)lspot->col.r;
	lspot->d_col_g=(double)lspot->col.g;
	lspot->d_col_b=(double)lspot->col.b;
	
		// save light
		
	memmove(&model->light.spots[model->light.nspot],lspot,sizeof(light_spot_type));
	model->light.nspot++;
}

/* =======================================================

      Find Closest Light Spot
      
======================================================= */

light_spot_type* model_find_closest_light(model_type *model,double x,double y,double z,int *p_dist)
{
	int					i,k,nspot;
	double				dx,dz,dy,d,dist;
	light_spot_type		*lspot;
	
	k=-1;
	dist=-1;
	
	nspot=model->light.nspot;
	lspot=model->light.spots;
	
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

	return(&model->light.spots[k]);
}

/* =======================================================

      Calculate Lights and Normals
      
======================================================= */

void model_calculate_light_color_and_normal(model_type *model,d3col *light_base,double x,double y,double z,float *cf,float *nf)
{
	int						i,nspot,cnt;
	double					mult,r,g,b,nx,nz,ny,d;
	light_spot_type			*lspot;

		// find distance for all light spots
		
	cnt=0;
	
	nspot=model->light.nspot;
	lspot=model->light.spots;
	
	for (i=0;i!=nspot;i++) {
		
		nx=lspot->d_x-x;
		nz=lspot->d_z-z;
		ny=lspot->d_y-y;
		
		d=(nx*nx)+(nz*nz)+(ny*ny);
		if (d<=lspot->d_intensity) {
			lspot->calc_light_mult=(lspot->d_intensity-d)*lspot->d_inv_intensity;
			cnt++;
		}
		else {
			lspot->calc_light_mult=-1;
		}
		
		lspot++;
	}
	
		// any light spots at all?
		
	if (cnt==0) {
		*cf++=light_base->r;
		*cf++=light_base->g;
		*cf=light_base->b;
	
		*nf++=1.0f;
		*nf++=0.0f;
		*nf=1.0f;
		return;
	}
	
		// combine the colors and normals
		
	r=g=b=0;
	nx=ny=nz=0;

	lspot=model->light.spots;
	
	for (i=0;i!=nspot;i++) {
		mult=lspot->calc_light_mult;
		
		if (mult!=-1) {
		
				// combine the lights for colors
				
			r+=(lspot->d_col_r*mult);
			g+=(lspot->d_col_g*mult);
			b+=(lspot->d_col_b*mult);
			
				// combine all lights vectors
							
			nx+=((lspot->d_x-x)*mult);
			ny+=((lspot->d_y-y)*mult);
			nz+=((lspot->d_z-z)*mult);
		}
		
		lspot++;
	}

		// set the color

	*cf++=light_base->r+(float)r;
	*cf++=light_base->g+(float)g;
	*cf=light_base->b+(float)b;
	
			// average the vector
		
	d=(1.0/((double)cnt));
	nx*=d;
	ny*=d;

		// normalize
		
	d=sqrt((nx*nx)+(ny*ny)+1);
	if (d!=0) {
		d=1.0/d;
		nx*=d;
		ny*=d;
	}

		// dot3 requires vectors in this format

	*nf++=-(float)nx;
	*nf++=1.0f-(float)ny;
	*nf=1.0f;
}

void model_calculate_normal_vector(model_type *model,double x,double y,double z,float *nf)
{
	int					i,nspot,cnt;
	double				dx,dz,dy,nx,nz,ny,d,mult;
	light_spot_type		*lspot;

		// combine all light spot normals
		// attenuated for distance
		
	cnt=0;
	
	nx=ny=nz=0;
	
	nspot=model->light.nspot;
	lspot=model->light.spots;
	
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
		*nf++=1.0f;
		*nf++=0.0f;
		*nf=1.0f;
		return;
	}
	
		// average the vector
		
	d=(1.0/((double)cnt));
	nx*=d;
	ny*=d;

		// normalize

	d=sqrt((nx*nx)+(ny*ny)+1);
	if (d!=0) {
		d=1.0/d;
		nx*=d;
		ny*=d;
	}

		// dot3 requires vectors in this format

	*nf++=(float)nx;
	*nf++=1.0f-(float)ny;
	*nf=1.0f;
}

