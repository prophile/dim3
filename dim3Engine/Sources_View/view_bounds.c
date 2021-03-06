/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Checking For Item In View Bounds

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
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
#include "remotes.h"
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "lights.h"
#include "effects.h"
#include "cameras.h"
#include "consoles.h"
#include "interfaces.h"
#include "video.h"

extern map_type				map;
extern camera_type			camera;
extern view_type			view;
extern server_type			server;
extern setup_type			setup;

extern void particle_draw_position(effect_type *effect,int count,int *x,int *y,int *z);
extern void ring_draw_position(effect_type *effect,int count,int *x,int *y,int *z);
extern bool fog_solid_on(void);
extern bool shadow_get_volume_mesh(map_mesh_type *mesh,int *px,int *py,int *pz);
extern bool shadow_get_volume_model(model_type *mdl,model_draw *draw,int *px,int *py,int *pz);

/* =======================================================

      Bounding Boxes in View
      
======================================================= */

bool complex_boundbox_inview(int *cbx,int *cby,int *cbz)
{
	int			n,
				px[14],py[14],pz[14];
	bool		hit,lft,rgt,top,bot;

		// add midpoints of faces to check
		
	memmove(px,cbx,(sizeof(int)*8));
	memmove(py,cby,(sizeof(int)*8));
	memmove(pz,cbz,(sizeof(int)*8));
	
	px[8]=(px[0]+px[1]+px[2]+px[3])>>2;
	py[8]=(py[0]+py[1]+py[2]+py[3])>>2;
	pz[8]=(pz[0]+pz[1]+pz[2]+pz[3])>>2;

	px[9]=(px[0]+px[1]+px[5]+px[4])>>2;
	py[9]=(py[0]+py[1]+py[5]+py[4])>>2;
	pz[9]=(pz[0]+pz[1]+pz[5]+pz[4])>>2;

	px[10]=(px[1]+px[2]+px[6]+px[5])>>2;
	py[10]=(py[1]+py[2]+py[6]+py[5])>>2;
	pz[10]=(pz[1]+pz[2]+pz[6]+pz[5])>>2;

	px[11]=(px[3]+px[2]+px[6]+px[7])>>2;
	py[11]=(py[3]+py[2]+py[6]+py[7])>>2;
	pz[11]=(pz[3]+pz[2]+pz[6]+pz[7])>>2;

	px[12]=(px[0]+px[3]+px[7]+px[4])>>2;
	py[12]=(py[0]+py[3]+py[7]+py[4])>>2;
	pz[12]=(pz[0]+pz[3]+pz[7]+pz[4])>>2;

	px[13]=(px[4]+px[5]+px[6]+px[7])>>2;
	py[13]=(py[4]+py[5]+py[6]+py[7])>>2;
	pz[13]=(pz[4]+pz[5]+pz[6]+pz[7])>>2;

		// check if points are behind z
		
	hit=FALSE;
	
	for (n=0;n!=14;n++) {
		if (gl_project_in_view_z(px[n],py[n],pz[n])) {
			hit=TRUE;
			break;
		}
	}
	
	if (!hit) return(FALSE);
		
		// project to screen

	gl_project_poly(14,px,py,pz);
	
		// off screen?

	lft=rgt=top=bot=TRUE;

	for (n=0;n!=14;n++) {
		lft=lft&&(px[n]<0);
		rgt=rgt&&(px[n]>=setup.screen.x_sz);
		top=top&&(py[n]<0);
		bot=bot&&(py[n]>=setup.screen.y_sz);
	}

	return(!(lft||rgt||top||bot));
}

bool boundbox_inview(int x,int z,int ex,int ez,int ty,int by)
{
	int				px[8],py[8],pz[8];
	
	px[0]=px[3]=px[4]=px[7]=x;
	px[1]=px[2]=px[5]=px[6]=ex;

	pz[0]=pz[1]=pz[4]=pz[5]=z;
	pz[2]=pz[3]=pz[6]=pz[7]=ez;

	py[0]=py[1]=py[2]=py[3]=ty;
	py[4]=py[5]=py[6]=py[7]=by;
	
	return(complex_boundbox_inview(px,py,pz));
}

/* =======================================================

      Pieces in View
      
======================================================= */

bool light_inview(d3pnt *pnt,int intensity)
{
	int			x,z,ex,ez,ty,by;

	x=pnt->x-intensity;
	ex=pnt->x+intensity;
	z=pnt->z-intensity;
	ez=pnt->z+intensity;
	ty=pnt->y-intensity;
	by=pnt->y+intensity;

	return(boundbox_inview(x,z,ex,ez,ty,by));
}

bool mesh_inview(map_mesh_type *mesh)
{
	int			n,nvertex,x,y,z;
	d3pnt		*pt;
	bool		lft,rgt,top,bot;

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	lft=rgt=top=bot=TRUE;

	for (n=0;n!=nvertex;n++) {
		x=pt->x;
		y=pt->y;
		z=pt->z;

		gl_project_point(&x,&y,&z);
		if (z>0) {
			pt++;
			continue;
		}

		lft=lft&&(x<0);
		rgt=rgt&&(x>=setup.screen.x_sz);
		top=top&&(y<0);
		bot=bot&&(y>=setup.screen.y_sz);
		
		pt++;
	}

	return(!(lft||rgt||top||bot));
}

bool mesh_shadow_inview(map_mesh_type *mesh)
{
	int				px[8],py[8],pz[8];

	if (!shadow_get_volume_mesh(mesh,px,py,pz)) return(FALSE);
	return(complex_boundbox_inview(px,py,pz));
}

bool model_inview(model_draw *draw)
{
	int				px[8],py[8],pz[8];
	model_type		*mdl;

	if ((draw->uid==-1) || (!draw->on)) return(FALSE);
	
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return(FALSE);
		
	model_get_view_complex_bounding_box(mdl,&draw->pnt,&draw->setup.ang,px,py,pz);
	return(complex_boundbox_inview(px,py,pz));
}

bool model_shadow_inview(model_draw *draw)
{
	int				px[8],py[8],pz[8];
	model_type		*mdl;

	if ((draw->uid==-1) || (!draw->on)) return(FALSE);
	
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return(FALSE);

	if (!shadow_get_volume_model(mdl,draw,px,py,pz)) return(FALSE);
	return(complex_boundbox_inview(px,py,pz));
}

bool effect_inview(effect_type *effect,int count)
{
	int				x,y,z,lx,rx,tz,bz,ty,by,gravity_y,k,size;
	particle_type	*particle;
	
	switch (effect->effecttype) {

		case ef_particle:
			particle_draw_position(effect,count,&x,&y,&z);

			size=effect->size;
			lx=x-(size>>1);
			rx=lx+size;
			tz=z-(size>>1);
			bz=tz+size;
			by=y;
			ty=by-size;

			particle=&server.particles[effect->data.particle.particle_idx];
			gravity_y=(int)particle_get_gravity(particle,count);

			if (gravity_y<0) {
				ty+=gravity_y;
			}
			else {
				by+=gravity_y;
			}
			break;

		case ef_ring:
			ring_draw_position(effect,count,&x,&y,&z);
			size=effect->size;
			lx=x-(size>>1);
			rx=lx+size;
			tz=z-(size>>1);
			bz=tz+size;
			by=y;
			ty=by-size;
			break;

		case ef_lightning:
			lx=effect->data.lightning.start_pnt.x;
			rx=effect->data.lightning.end_pnt.x;
			tz=effect->data.lightning.start_pnt.z;
			bz=effect->data.lightning.end_pnt.z;
			ty=effect->data.lightning.start_pnt.y;
			by=effect->data.lightning.end_pnt.y;
			break;

		case ef_ray:
			lx=effect->data.ray.start_pnt.x;
			rx=effect->data.ray.end_pnt.x;
			tz=effect->data.ray.start_pnt.z;
			bz=effect->data.ray.end_pnt.z;
			ty=effect->data.ray.start_pnt.y;
			by=effect->data.ray.end_pnt.y;
			break;

		default:
			size=effect->size;
			lx=effect->pnt.x-(size>>1);
			rx=lx+size;
			tz=effect->pnt.z-(size>>1);
			bz=tz+size;
			by=effect->pnt.y;
			ty=by-size;
			break;
	}
	
	if (lx>rx) {
		k=lx;
		lx=rx;
		rx=k;
	}
	if (tz>bz) {
		k=tz;
		tz=bz;
		bz=k;
	}
	if (ty>by) {
		k=ty;
		ty=by;
		by=k;
	}
	
	return(boundbox_inview(lx,tz,rx,bz,ty,by));
}

/* =======================================================

      Distances to View Center
      
======================================================= */

double distance_to_view_center(int x,int y,int z)
{
	double			dx,dz,dy;
	
	dx=(double)(x-view.render->camera.pnt.x);
	dy=(double)(y-view.render->camera.pnt.y);
	dz=(double)(z-view.render->camera.pnt.z);
	return(sqrt((dx*dx)+(dy*dy)+(dz*dz)));
}

