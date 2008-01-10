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

      Get Enclosing Square for Polygon
      
======================================================= */

void map_prepare_set_mesh_poly_box(map_mesh_type *mesh,map_mesh_poly_type *mesh_poly)
{
	int				n,y,ptsz,
					min_x,max_x,min_z,max_z,min_y,max_y;
	bool			flat;
	d3pnt			*pt;
	
		// find enclosing square
		
	pt=&mesh->vertexes[mesh_poly->v[0]];

	min_x=max_x=pt->x;
	min_y=max_y=y=pt->y;
	min_z=max_z=pt->z;

	flat=TRUE;
    
    ptsz=mesh_poly->ptsz;
    
	for (n=1;n<ptsz;n++) {
		pt=&mesh->vertexes[mesh_poly->v[n]];

			// get min and max

		if (pt->x<min_x) min_x=pt->x;
		if (pt->x>max_x) max_x=pt->x;
		if (pt->y<min_y) min_y=pt->y;
		if (pt->y>max_y) max_y=pt->y;
		if (pt->z<min_z) min_z=pt->z;
		if (pt->z>max_z) max_z=pt->z;

			// check for flat y

		if (pt->y!=y) flat=FALSE;
	}
    
	mesh_poly->box.min.x=min_x;
	mesh_poly->box.max.x=max_x;
	mesh_poly->box.min.y=min_y;
	mesh_poly->box.max.y=max_y;
	mesh_poly->box.min.z=min_z;
	mesh_poly->box.max.z=max_z;
	
	mesh_poly->box.flat=flat;
}





// supergumba -- delete these

void map_prepare_set_fc_segment_square(segment_type *seg)
{
	int					i,y,ptsz,
						min_x,max_x,min_z,max_z,min_y,max_y;
	fc_segment_data		*fc;
	
	fc=&seg->data.fc;
	
		// find enclosing square
		
	min_x=max_x=fc->x[0];
	min_z=max_z=fc->z[0];
	min_y=max_y=fc->y[0];
    
    ptsz=fc->ptsz;
    
	for (i=1;i<ptsz;i++) {
		if (fc->x[i]<min_x) min_x=fc->x[i];
		if (fc->x[i]>max_x) max_x=fc->x[i];
		if (fc->z[i]<min_z) min_z=fc->z[i];
		if (fc->z[i]>max_z) max_z=fc->z[i];
		if (fc->y[i]<min_y) min_y=fc->y[i];
		if (fc->y[i]>max_y) max_y=fc->y[i];
	}
    
	fc->min_x=min_x;
	fc->max_x=max_x;
	fc->min_z=min_z;
	fc->max_z=max_z;
	fc->min_y=min_y;
	fc->max_y=max_y;
	
		// check for flat y's
		
	y=fc->y[0];
	fc->flat=TRUE;
	
	for (i=1;i<ptsz;i++) {
		if (fc->y[i]!=y) fc->flat=FALSE;
	}
}

void map_prepare_set_ambient_fc_segment_square(segment_type *seg)
{
	int							i,ptsz,
								min_x,max_x,min_z,max_z,min_y,max_y;
	ambient_fc_segment_data		*ambient_fc;
	
	ambient_fc=&seg->data.ambient_fc;
	
		// find enclosing square
		
	min_x=max_x=ambient_fc->x[0];
	min_z=max_z=ambient_fc->z[0];
	min_y=max_y=ambient_fc->y[0];
    
    ptsz=ambient_fc->ptsz;
    
	for (i=1;i<ptsz;i++) {
		if (ambient_fc->x[i]<min_x) min_x=ambient_fc->x[i];
		if (ambient_fc->x[i]>max_x) max_x=ambient_fc->x[i];
		if (ambient_fc->z[i]<min_z) min_z=ambient_fc->z[i];
		if (ambient_fc->z[i]>max_z) max_z=ambient_fc->z[i];
		if (ambient_fc->y[i]<min_y) min_y=ambient_fc->y[i];
		if (ambient_fc->y[i]>max_y) max_y=ambient_fc->y[i];
	}
    
	ambient_fc->min_x=min_x;
	ambient_fc->max_x=max_x;
	ambient_fc->min_z=min_z;
	ambient_fc->max_z=max_z;
	ambient_fc->min_y=min_y;
	ambient_fc->max_y=max_y;
}

/* =======================================================

      Prepare Segments
      
======================================================= */

void map_prepare_segments(map_type *map)
{
	int					i,n,k,t,ptsz;
	d3pnt				*pt;
	portal_type			*portal;
	segment_type		*seg;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	node_type			*node;
	map_scenery_type	*scenery;
	map_light_type		*light;
	map_sound_type		*sound;
	map_particle_type	*particle;
	spot_type			*spot;
	
		// optimizations
		
	map->optimizations.lod_light_distance*=map_enlarge;
	map->optimizations.lod_bump_distance*=map_enlarge;
	map->optimizations.lod_specular_distance*=map_enlarge;
	map->optimizations.lod_glow_distance*=map_enlarge;
	map->optimizations.lod_model_distance*=map_enlarge;
	map->optimizations.lod_shadow_distance*=map_enlarge;
	map->optimizations.lod_effect_distance*=map_enlarge;
	
		// portals
	
	portal=map->portals;
	
	for (i=0;i!=map->nportal;i++) {

			// portal middle

		portal->mx=(portal->x+portal->ex)>>1;
		portal->mz=(portal->z+portal->ez)>>1;

			// search for y size when
			// preparing meshes
		
		portal->ty=99999;
		portal->by=0;
		
			// prepare meshes

		mesh=portal->mesh.meshes;
		
		for (n=0;n!=portal->mesh.nmesh;n++) {
				
				// translate vertexes from portal to global
					
			pt=mesh->vertexes;
			
			for (k=0;k!=mesh->nvertex;k++) {
				pt->x+=portal->x;
				pt->z+=portal->z;
				pt++;
			}
			
				// run through the mesh polygons

			mesh_poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {
			
					// and find lowest/highest portal Y

				for (t=0;t!=mesh_poly->ptsz;t++) {
					pt=&mesh->vertexes[mesh_poly->v[t]];

					if (pt->y<portal->ty) portal->ty=pt->y;
					if (pt->y>portal->by) portal->by=pt->y;
				}
				
					// setup box

				map_prepare_set_mesh_poly_box(mesh,mesh_poly);

					// setup simple tessel and shifting flag

				mesh_poly->draw.txt_frame_offset=0;
				mesh_poly->draw.simple_tessel=(mesh_poly->ptsz==3);
				mesh_poly->draw.shiftable=((mesh_poly->x_shift!=0.0f) || (mesh_poly->y_shift!=0.0f));
			
				mesh_poly++;
			}
		
			mesh++;
		}
		
			// fix portal heights

		if ((portal->ty==99999) && (portal->by!=0)) portal->ty=portal->by;
		if ((portal->by==0) && (portal->ty!=99999)) portal->by=portal->ty;
		if ((portal->ty==99999) || (portal->by==0)) portal->ty=portal->by=0;

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


		// supergumba -- all needs to be deleted, etc

		// segments
    
	/*
    seg=map->segments;
    
	for (i=0;i!=map->nsegment;i++) {
		portal=&map->portals[seg->rn];
		
		seg->txt_offset=0;
		seg->simple_tessel=FALSE;
		
		switch (seg->type) {
		
			case sg_wall:
				seg->data.wall.lx=(seg->data.wall.lx*map_enlarge)+portal->x;
				seg->data.wall.rx=(seg->data.wall.rx*map_enlarge)+portal->x;
				seg->data.wall.lz=(seg->data.wall.lz*map_enlarge)+portal->z;
				seg->data.wall.rz=(seg->data.wall.rz*map_enlarge)+portal->z;
				seg->data.wall.ty=seg->data.wall.ty*map_enlarge;
				seg->data.wall.by=(seg->data.wall.by+1)*map_enlarge;
                
				if (seg->data.wall.ty<portal->ty) portal->ty=seg->data.wall.ty;
				if (seg->data.wall.by>portal->by) portal->by=seg->data.wall.by;
                
                map_prepare_create_wall_segment_polygon(seg);
                map_prepare_create_wall_segment_uv(seg);
 				break;
				
			case sg_floor:
            case sg_ceiling:
				ptsz=(int)seg->data.fc.ptsz;
                
				for ((t=0);(t!=ptsz);t++) {
					seg->data.fc.x[t]=(seg->data.fc.x[t]*map_enlarge)+portal->x;
					seg->data.fc.z[t]=(seg->data.fc.z[t]*map_enlarge)+portal->z;
                    seg->data.fc.y[t]=seg->data.fc.y[t]*map_enlarge;
                    
					if (seg->data.fc.y[t]<portal->ty) portal->ty=seg->data.fc.y[t];
					if (seg->data.fc.y[t]>portal->by) portal->by=seg->data.fc.y[t];
				}
				
				if (ptsz==3) seg->simple_tessel=TRUE;

				map_prepare_set_fc_segment_square(seg);
                map_prepare_create_fc_segment_uv(seg);
				map_prepare_create_fc_segment_slope(seg);
				break;

			case sg_liquid:
				seg->data.liquid.lft=(seg->data.liquid.lft*map_enlarge)+portal->x;
				seg->data.liquid.rgt=(seg->data.liquid.rgt*map_enlarge)+portal->x;
				seg->data.liquid.top=(seg->data.liquid.top*map_enlarge)+portal->z;
				seg->data.liquid.bot=(seg->data.liquid.bot*map_enlarge)+portal->z;

				seg->data.liquid.y=(seg->data.liquid.y+1)*map_enlarge;
				break;
				
			case sg_ambient_wall:
				seg->data.ambient_wall.lx=(seg->data.ambient_wall.lx*map_enlarge)+portal->x;
				seg->data.ambient_wall.rx=(seg->data.ambient_wall.rx*map_enlarge)+portal->x;
				seg->data.ambient_wall.lz=(seg->data.ambient_wall.lz*map_enlarge)+portal->z;
				seg->data.ambient_wall.rz=(seg->data.ambient_wall.rz*map_enlarge)+portal->z;
				seg->data.ambient_wall.ty=seg->data.ambient_wall.ty*map_enlarge;
				seg->data.ambient_wall.by=(seg->data.ambient_wall.by+1)*map_enlarge;
				
                map_prepare_create_push_ambient_wall_segment_polygon(seg);
                map_prepare_create_ambient_wall_segment_uv(seg);
				
				seg->simple_tessel=TRUE;
				break;
				
			case sg_ambient_fc:
				ptsz=(int)seg->data.ambient_fc.ptsz;
                
				for ((t=0);(t!=ptsz);t++) {
					seg->data.ambient_fc.x[t]=(seg->data.ambient_fc.x[t]*map_enlarge)+portal->x;
					seg->data.ambient_fc.z[t]=(seg->data.ambient_fc.z[t]*map_enlarge)+portal->z;
                    seg->data.ambient_fc.y[t]=seg->data.ambient_fc.y[t]*map_enlarge;
				}
				
				map_prepare_set_ambient_fc_segment_square(seg);
				map_prepare_push_ambient_fc_segment_polygon(seg);
                map_prepare_create_ambient_fc_segment_uv(seg);
				
				seg->simple_tessel=TRUE;
				break;
							
		}

			// quick fix for segments with a shift
			// but shiftable bit not set

		if ((seg->x_shift!=0.0f) || (seg->y_shift!=0.0f)) seg->shiftable=TRUE;
        
        seg++;
	}
	
		// fix portal heights
	
    portal=map->portals;
    
	for ((i=0);(i!=map->nportal);i++) {
		if ((portal->ty==99999) && (portal->by!=0)) portal->ty=portal->by;
		if ((portal->by==0) && (portal->ty!=99999)) portal->by=portal->ty;
		if ((portal->ty==99999) || (portal->by==0)) portal->ty=portal->by=0;
        portal++;
	}

		// map scenery
	
	scenery=map->sceneries;
	
	for ((i=0);(i!=map->nscenery);i++) {
		portal=&map->portals[scenery->pos.rn];
		scenery->pos.x=(scenery->pos.x*map_enlarge)+portal->x;
		scenery->pos.y=(scenery->pos.y+1)*map_enlarge;
		scenery->pos.z=(scenery->pos.z*map_enlarge)+portal->z;
		scenery++;
	}
		
		// map lights
	
	light=map->lights;
	
	for ((i=0);(i!=map->nlight);i++) {
		portal=&map->portals[light->pos.rn];
		light->intensity*=map_enlarge;
		light->pos.x=(light->pos.x*map_enlarge)+portal->x;
		light->pos.y=(light->pos.y+1)*map_enlarge;
		light->pos.z=(light->pos.z*map_enlarge)+portal->z;
		light++;
	}
	
		// map sounds
	
	sound=map->sounds;
	
	for ((i=0);(i!=map->nsound);i++) {
		portal=&map->portals[sound->pos.rn];
		sound->pos.x=(sound->pos.x*map_enlarge)+portal->x;
		sound->pos.y=(sound->pos.y+1)*map_enlarge;
		sound->pos.z=(sound->pos.z*map_enlarge)+portal->z;
		
		sound++;
	}
	
		// map particles
	
	particle=map->particles;
	
	for ((i=0);(i!=map->nparticle);i++) {
		portal=&map->portals[particle->pos.rn];
		particle->pos.x=(particle->pos.x*map_enlarge)+portal->x;
		particle->pos.y=(particle->pos.y+1)*map_enlarge;
		particle->pos.z=(particle->pos.z*map_enlarge)+portal->z;
		
		particle++;
	}
	
		// nodes

	node=map->nodes;
	
	for ((i=0);(i!=map->nnode);i++) {
        node->idx=i;
		portal=&map->portals[node->pos.rn];
		node->pos.x=(node->pos.x*map_enlarge)+portal->x;
		node->pos.y=(node->pos.y+1)*map_enlarge;
		node->pos.z=(node->pos.z*map_enlarge)+portal->z;
		node++;
	}
    
		// object starts
		
	spot=map->spots;

	for ((i=0);(i!=map->nspot);i++) {
		portal=&map->portals[spot->pos.rn];
		spot->pos.x=(spot->pos.x*map_enlarge)+portal->x;
		spot->pos.y=(spot->pos.y+1)*map_enlarge;
		spot->pos.z=(spot->pos.z*map_enlarge)+portal->z;
		spot++;
	}
	*/
}

/* =======================================================

      Set up Curves and Clipped Segments
      
======================================================= */

void map_prepare_curves_clips(map_type *map,int curve_level)
{
	int					i,orig_nsegment;
	segment_type		*seg;
	        
	map_prepare_setup_curve_constant(curve_level);   
 
		// create new segments for curves
		
    seg=map->segments;
    orig_nsegment=map->nsegment;
    
	for (i=0;i!=orig_nsegment;i++) {
    
		switch (seg->type) {
			
			case sg_wall:
				if (seg->curve!=cv_none) {
					map_prepare_create_wall_curve(map,seg);
					break;
				}
				if ((seg->clip>=wc_tessel_start) && (seg->clip<=wc_tessel_end)) {
					map_prepare_create_wall_clip(map,seg);
					break;
				}
				break;
				
			case sg_floor:
			case sg_ceiling:
				if (seg->curve!=cv_none) map_prepare_create_fc_curve(map,seg);
				break;
				
		}
        
        seg++;
	}
}

/* =======================================================

      Prepare Map For Engine Usage
	        
======================================================= */

void map_prepare(map_type *map,int curve_level)
{
	map_prepare_segments(map);
	map_prepare_curves_clips(map,curve_level);
	map_prepare_create_segment_middles(map);
}

