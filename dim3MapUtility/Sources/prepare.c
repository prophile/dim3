/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Preparing Map For Engine Rendering

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

/* =======================================================

      Prepare Single Polygon for Rendering
      
======================================================= */

void map_prepare_mesh_poly_determine_wall_like(map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int				n,ptsz,px[8],py[8];
	d3pnt			*pt;
	
	poly->box.wall_like=FALSE;
	
		// flat polygons are automatically not wall-like
		
	if (poly->box.flat) return;

		// if all x/z points in a line, then polygon
		// is automatically wall-like

	ptsz=poly->ptsz;

	for (n=0;n!=ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		px[n]=pt->x;
		py[n]=pt->z;
	}

	if (line_2D_all_points_in_line(ptsz,px,py,0.1f)) {
		poly->box.wall_like=TRUE;
		return;
	}
	
		// if the slope is greater than 60, then consider it a FC

	if (poly->slope.ang_y<60.0f) return;

		// otherwise it's a wall
	
	poly->box.wall_like=TRUE;
}

void map_prepare_mesh_poly(map_mesh_type *mesh,map_mesh_poly_type *poly)
{
	int				n,ptsz,y,lx,rx,lz,rz,
					px[8],py[8],pz[8];
	bool			flat;
	d3pnt			min,max,mid;
	d3pnt			*pt;

		// find enclosing square
		// and middle and if polygon is flat
		
	pt=&mesh->vertexes[poly->v[0]];

	min.x=max.x=mid.x=pt->x;
	min.y=max.y=mid.y=y=pt->y;
	min.z=max.z=mid.z=pt->z;

	flat=TRUE;
    
    ptsz=poly->ptsz;
    
	for (n=1;n<ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];

			// get min and max

		if (pt->x<min.x) min.x=pt->x;
		if (pt->x>max.x) max.x=pt->x;
		if (pt->y<min.y) min.y=pt->y;
		if (pt->y>max.y) max.y=pt->y;
		if (pt->z<min.z) min.z=pt->z;
		if (pt->z>max.z) max.z=pt->z;

			// add for middle

		mid.x+=pt->x;
		mid.y+=pt->y;
		mid.z+=pt->z;

			// check for flat y

		if (pt->y!=y) flat=FALSE;
	}
    
	memmove(&poly->box.min,&min,sizeof(d3pnt));
	memmove(&poly->box.max,&max,sizeof(d3pnt));
	
	poly->box.mid.x=mid.x/ptsz;
	poly->box.mid.y=mid.y/ptsz;
	poly->box.mid.z=mid.z/ptsz;
	
	poly->box.flat=flat;

		// setup slopes
		
	for (n=0;n!=poly->ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		px[n]=pt->x;
		py[n]=pt->y;
		pz[n]=pt->z;
	}

	if (poly->box.flat) {
		poly->slope.y=0.0f;
		poly->slope.ang_y=0.0f;
		poly->slope.move_x=0.0f;
		poly->slope.move_z=0.0f;
	}
	else {
		poly->slope.y=polygon_get_slope_y(poly->ptsz,px,py,pz,&poly->slope.ang_y);
		angle_get_movement_float(poly->slope.ang_y,(gravity_slope_factor*poly->slope.y),&poly->slope.move_x,&poly->slope.move_z);
	}

		// determine if wall like
		
	map_prepare_mesh_poly_determine_wall_like(mesh,poly);

		// create wall "line" for wall like polygons

	if (poly->box.wall_like) {
			
		lx=poly->box.min.x;
		rx=poly->box.max.x;
		lz=poly->box.min.z;
		rz=poly->box.max.z;
		
		for (n=0;n!=poly->ptsz;n++) {
			pt=&mesh->vertexes[poly->v[n]];
			
			if ((rx-lx)>(rz-lz)) {
				if (pt->x==lx) lz=pt->z;
				if (pt->x==rx) rz=pt->z;
			}
			else {
				if (pt->z==lz) lx=pt->x;
				if (pt->z==rz) rx=pt->x;
			}
		}

		poly->line.lx=lx;
		poly->line.rx=rx;
		poly->line.lz=lz;
		poly->line.rz=rz;
	}
}

void map_prepare_mesh_box(map_mesh_type *mesh)
{
	int						n;
	d3pnt					*pt,mesh_min,mesh_max,mesh_mid;
		
		// start mesh min/max/mid
			
	mesh_min.x=mesh_min.y=mesh_min.z=0;
	mesh_max.x=mesh_max.y=mesh_max.z=0;
	mesh_mid.x=mesh_mid.y=mesh_mid.z=0;

		// determine size
	
	pt=mesh->vertexes;

	for (n=0;n!=mesh->nvertex;n++) {
		
			// setup mesh min, max, mid
			
		if (n==0) {
			memmove(&mesh_min,pt,sizeof(d3pnt));
			memmove(&mesh_max,pt,sizeof(d3pnt));
			memmove(&mesh_mid,pt,sizeof(d3pnt));
		}
		else {
			if (pt->x<mesh_min.x) mesh_min.x=pt->x;
			if (pt->y<mesh_min.y) mesh_min.y=pt->y;
			if (pt->z<mesh_min.z) mesh_min.z=pt->z;
		
			if (pt->x>mesh_max.x) mesh_max.x=pt->x;
			if (pt->y>mesh_max.y) mesh_max.y=pt->y;
			if (pt->z>mesh_max.z) mesh_max.z=pt->z;
			
			mesh_mid.x+=pt->x;
			mesh_mid.y+=pt->y;
			mesh_mid.z+=pt->z;
		}
	
		pt++;
	}
	
		// setup mesh box
		
	memmove(&mesh->box.min,&mesh_min,sizeof(d3pnt));
	memmove(&mesh->box.max,&mesh_max,sizeof(d3pnt));
	
	if (mesh->nvertex!=0) {
		mesh->box.mid.x=mesh_mid.x/mesh->nvertex;
		mesh->box.mid.y=mesh_mid.y/mesh->nvertex;
		mesh->box.mid.z=mesh_mid.z/mesh->nvertex;
	}
}

/* =======================================================

      Prepare Map
      
======================================================= */

void map_prepare(map_type *map)
{
	int					n,k;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
		// prepare meshes

	mesh=map->mesh.meshes;
		
	for (n=0;n!=map->mesh.nmesh;n++) {

			// default some flags

		mesh->flag.touched=FALSE;
		mesh->flag.shiftable=FALSE;
		
			// run through the mesh polygons

		poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
			
				// setup box and slope

			map_prepare_mesh_poly(mesh,poly);
			
				// setup texture and shifting flags

			poly->draw.txt_frame_offset=0;
			poly->draw.shift_on=((poly->x_shift!=0.0f) || (poly->y_shift!=0.0f));

			poly->draw.x_shift_offset=0.0f;
			poly->draw.y_shift_offset=0.0f;

			mesh->flag.shiftable|=poly->draw.shift_on;

			poly++;
		}

			// setup boxes

		map_prepare_mesh_box(mesh);
		
		mesh++;
	}
}

/* =======================================================

      Center Map
      
======================================================= */

void map_center(map_type *map)
{
	int					n,k,x,y,z;
	bool				first_hit;
	d3pnt				*pt,min,max;
	map_mesh_type		*mesh;
	map_liquid_type		*liq;
 	map_scenery_type	*scenery;
	map_light_type		*light;
    map_sound_type		*sound;
	map_particle_type	*particle;
    node_type			*node;
    spot_type			*spot;
	
		// get map size

	first_hit=TRUE;
	
	mesh=map->mesh.meshes;
		
	for (n=0;n!=map->mesh.nmesh;n++) {

		pt=mesh->vertexes;

		for (k=0;k!=mesh->nvertex;k++) {
		
			if (first_hit) {
				min.x=max.x=pt->x;
				min.y=max.y=pt->y;
				min.z=max.z=pt->z;
				first_hit=FALSE;
			}
			else {
				if (pt->x<min.x) min.x=pt->x;
				if (pt->x>max.x) max.x=pt->x;
				if (pt->y<min.y) min.y=pt->y;
				if (pt->y>max.y) max.y=pt->y;
				if (pt->z<min.z) min.z=pt->z;
				if (pt->z>max.z) max.z=pt->z;
			}
			pt++;
		}

		mesh++;
	}
	
	if (!first_hit) return;

		// adjust map

	x=(max.x+min.x)/2;
	y=(max.y+min.y)/2;
	z=(max.z+min.z)/2;

	mesh=map->mesh.meshes;
		
	for (n=0;n!=map->mesh.nmesh;n++) {

		pt=mesh->vertexes;

		for (k=0;k!=mesh->nvertex;k++) {
			pt->x=(pt->x-x)+(map_max_size/2);
			pt->y=(pt->y-y)+(map_max_size/2);
			pt->z=(pt->z-z)+(map_max_size/2);
			pt++;
		}

		mesh++;
	}

	liq=map->liquid.liquids;

	for (n=0;n!=map->liquid.nliquid;n++) {

		liq->lft=(liq->lft-x)+(map_max_size/2);
		liq->rgt=(liq->rgt-x)+(map_max_size/2);
		liq->top=(liq->top-z)+(map_max_size/2);
		liq->bot=(liq->bot-z)+(map_max_size/2);
		liq->y=(liq->y-y)+(map_max_size/2);

		liq++;
	}
	
	scenery=map->sceneries;
	
	for (n=0;n!=map->nscenery;n++) {
		scenery->pnt.x=(scenery->pnt.x-x)+(map_max_size/2);
		scenery->pnt.y=(scenery->pnt.y-y)+(map_max_size/2);
		scenery->pnt.z=(scenery->pnt.z-z)+(map_max_size/2);
		scenery++;
	}

	light=map->lights;
	
	for (n=0;n!=map->nlight;n++) {
		light->pnt.x=(light->pnt.x-x)+(map_max_size/2);
		light->pnt.y=(light->pnt.y-y)+(map_max_size/2);
		light->pnt.z=(light->pnt.z-z)+(map_max_size/2);
		light++;
	}

	sound=map->sounds;
	
	for (n=0;n!=map->nsound;n++) {
		sound->pnt.x=(sound->pnt.x-x)+(map_max_size/2);
		sound->pnt.y=(sound->pnt.y-y)+(map_max_size/2);
		sound->pnt.z=(sound->pnt.z-z)+(map_max_size/2);
		sound++;
	}

	particle=map->particles;
	
	for (n=0;n!=map->nparticle;n++) {
		particle->pnt.x=(particle->pnt.x-x)+(map_max_size/2);
		particle->pnt.y=(particle->pnt.y-y)+(map_max_size/2);
		particle->pnt.z=(particle->pnt.z-z)+(map_max_size/2);
		particle++;
	}

	node=map->nodes;
	
	for (n=0;n!=map->nnode;n++) {
		node->pnt.x=(node->pnt.x-x)+(map_max_size/2);
		node->pnt.y=(node->pnt.y-y)+(map_max_size/2);
		node->pnt.z=(node->pnt.z-z)+(map_max_size/2);
		node++;
	}

	spot=map->spots;
	
	for (n=0;n!=map->nspot;n++) {
		spot->pnt.x=(spot->pnt.x-x)+(map_max_size/2);
		spot->pnt.y=(spot->pnt.y-y)+(map_max_size/2);
		spot->pnt.z=(spot->pnt.z-z)+(map_max_size/2);
		spot++;
	}
}
