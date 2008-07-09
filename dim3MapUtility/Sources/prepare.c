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
	int				n,k,ptsz,px[8],pz[8];
	d3pnt			*pt,*chk_pt;
	
	poly->box.wall_like=FALSE;
	
		// flat polygons are not wall-like
		
	if (poly->box.flat) return;
	
		// check for common xz points
		// and assume wall type if they exist
		
	ptsz=poly->ptsz;
	
	for (n=0;n!=ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];

		for (k=0;k!=ptsz;k++) {
			if (k==n) continue;
			
			chk_pt=&mesh->vertexes[poly->v[k]];
			if ((pt->x==chk_pt->x) && (pt->z==chk_pt->z)) {
				poly->box.wall_like=TRUE;
				return;
			}
		}
	}
	
		// over a certain slope means wall like

	if (poly->slope.y>1.5f) {
		poly->box.wall_like=TRUE;
		return;
	}

		// otherwise determine if all points are in a line

	for (n=0;n!=ptsz;n++) {
		pt=&mesh->vertexes[poly->v[n]];
		px[n]=pt->x;
		pz[n]=pt->z;
	}

	poly->box.wall_like=line_2D_all_points_in_line(ptsz,px,pz,0.0f);
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

	poly->slope.y=polygon_get_slope_y(poly->ptsz,px,py,pz,&poly->slope.ang_y);
	angle_get_movement_float(poly->slope.ang_y,(gravity_slope_factor*poly->slope.y),&poly->slope.move_x,&poly->slope.move_z);
	
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

/* =======================================================

      Prepare Map
      
======================================================= */

void map_prepare(map_type *map)
{
	int					i,n,k,t,simple_cnt;
	d3pnt				mesh_min,mesh_max,mesh_mid;
	d3pnt				*pt;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	map_liquid_type		*liq;
	node_type			*node;
	map_scenery_type	*scenery;
	map_light_type		*light;
	map_sound_type		*sound;
	map_particle_type	*particle;
	spot_type			*spot;
	
		// portals
	
	portal=map->portals;
	
	for (i=0;i!=map->nportal;i++) {

			// portal middle

		portal->mx=(portal->x+portal->ex)>>1;
		portal->mz=(portal->z+portal->ez)>>1;

			// search for y size when
			// preparing meshes
		
		portal->ty=map_max_size;
		portal->by=-map_max_size;
		
			// prepare meshes

		mesh=portal->mesh.meshes;
		
		for (n=0;n!=portal->mesh.nmesh;n++) {

				// default some flags

			mesh->flag.touched=FALSE;
			mesh->flag.shiftable=FALSE;
				
				// translate vertexes from portal to global
					
			pt=mesh->vertexes;
			
			for (k=0;k!=mesh->nvertex;k++) {
				pt->x+=portal->x;
				pt->z+=portal->z;
				pt++;
			}
			
				// start mesh min/max/mid
				
			mesh_min.x=mesh_min.y=mesh_min.z=0;
			mesh_max.x=mesh_max.y=mesh_max.z=0;
			mesh_mid.x=mesh_mid.y=mesh_mid.z=0;
			
				// run through the mesh polygons

			poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {
			
					// find lowest and highest Y for portal

				for (t=0;t!=poly->ptsz;t++) {
					pt=&mesh->vertexes[poly->v[t]];

					if (pt->y<portal->ty) portal->ty=pt->y;
					if (pt->y>portal->by) portal->by=pt->y;
				}
				
					// setup box and slope

				map_prepare_mesh_poly(mesh,poly);

					// detect simple tessel segments
					// if two of the axises are under simple
					// tessel size, then entire polygon is simple tessel

				simple_cnt=0;

				if ((poly->box.max.x-poly->box.min.x)<map_simple_tessel_size) simple_cnt++;
				if ((poly->box.max.z-poly->box.min.z)<map_simple_tessel_size) simple_cnt++;
				if ((poly->box.max.y-poly->box.min.y)<map_simple_tessel_size) simple_cnt++;
			
				poly->draw.simple_tessel=(simple_cnt>=2);

					// setup texture and shifting flags

				poly->draw.txt_frame_offset=0;
				poly->draw.shift_on=((poly->x_shift!=0.0f) || (poly->y_shift!=0.0f));

				mesh->flag.shiftable|=poly->draw.shift_on;
				
					// setup mesh min, max, mid
					
				if (k==0) {
					memmove(&mesh_min,&poly->box.min,sizeof(d3pnt));
					memmove(&mesh_max,&poly->box.max,sizeof(d3pnt));
					memmove(&mesh_mid,&poly->box.mid,sizeof(d3pnt));
				}
				else {
					if (poly->box.min.x<mesh_min.x) mesh_min.x=poly->box.min.x;
					if (poly->box.min.y<mesh_min.y) mesh_min.y=poly->box.min.y;
					if (poly->box.min.z<mesh_min.z) mesh_min.z=poly->box.min.z;
				
					if (poly->box.max.x>mesh_max.x) mesh_max.x=poly->box.max.x;
					if (poly->box.max.y>mesh_max.y) mesh_max.y=poly->box.max.y;
					if (poly->box.max.z>mesh_max.z) mesh_max.z=poly->box.max.z;
					
					mesh_mid.x+=poly->box.mid.x;
					mesh_mid.y+=poly->box.mid.y;
					mesh_mid.z+=poly->box.mid.z;
				}
			
				poly++;
			}
			
				// setup mesh box
				
			memmove(&mesh->box.min,&mesh_min,sizeof(d3pnt));
			memmove(&mesh->box.max,&mesh_max,sizeof(d3pnt));
			
			mesh->box.mid.x=mesh_mid.x/mesh->npoly;
			mesh->box.mid.y=mesh_mid.y/mesh->npoly;
			mesh->box.mid.z=mesh_mid.z/mesh->npoly;
		
			mesh++;
		}
		
			// liquids

		liq=portal->liquid.liquids;
		
		for (n=0;n!=portal->liquid.nliquid;n++) {
			liq->lft+=portal->x;
			liq->rgt+=portal->x;
			liq->top+=portal->z;
			liq->bot+=portal->z;
			liq++;
		}
		
			// fix portal heights

		if ((portal->ty==map_max_size) && (portal->by!=-map_max_size)) portal->ty=portal->by;
		if ((portal->by==-map_max_size) && (portal->ty!=map_max_size)) portal->by=portal->ty;
		if ((portal->ty==map_max_size) || (portal->by==-map_max_size)) portal->ty=portal->by=0;

		portal++;
	}
	
		// translate other objects from portal
		// to global coordinates
	
	scenery=map->sceneries;
	
	for (n=0;n!=map->nscenery;n++) {
		portal=&map->portals[scenery->pos.rn];
		scenery->pos.x+=portal->x;
		scenery->pos.z=scenery->pos.z+portal->z;
		scenery++;
	}
		
	light=map->lights;
	
	for (n=0;n!=map->nlight;n++) {
		portal=&map->portals[light->pos.rn];
		light->pos.x=light->pos.x+portal->x;
		light->pos.z=light->pos.z+portal->z;
		light++;
	}
	
		// map sounds
	
	sound=map->sounds;
	
	for (n=0;n!=map->nsound;n++) {
		portal=&map->portals[sound->pos.rn];
		sound->pos.x=sound->pos.x+portal->x;
		sound->pos.z=sound->pos.z+portal->z;
		sound++;
	}
	
		// map particles
	
	particle=map->particles;
	
	for (n=0;n!=map->nparticle;n++) {
		portal=&map->portals[particle->pos.rn];
		particle->pos.x=particle->pos.x+portal->x;
		particle->pos.z=particle->pos.z+portal->z;
		particle++;
	}
	
		// nodes

	node=map->nodes;
	
	for (n=0;n!=map->nnode;n++) {
		portal=&map->portals[node->pos.rn];
        node->idx=i;
		node->pos.x=node->pos.x+portal->x;
		node->pos.z=node->pos.z+portal->z;
		node++;
	}
    
		// object starts
		
	spot=map->spots;

	for (n=0;n!=map->nspot;n++) {
		portal=&map->portals[spot->pos.rn];
		spot->pos.x=spot->pos.x+portal->x;
		spot->pos.z=spot->pos.z+portal->z;
		spot++;
	}
}

