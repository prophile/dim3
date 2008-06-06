/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Portals

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

      Create a Portal
      
======================================================= */

int map_portal_create(map_type *map,int x,int z,int ex,int ez)
{
	int				rn;
	portal_type		*portal;
	
	if (map->nportal==max_portal) return(-1);
	
	rn=map->nportal;
	map->nportal++;
	
	portal=&map->portals[rn];

	bzero(portal,sizeof(portal_type));
	
	portal->x=x;
    portal->z=z;
	portal->ex=ex;
    portal->ez=ez;
	portal->ty=portal->by=0;
	portal->msg.entry_on=portal->msg.exit_on=portal->msg.base_on=portal->msg.map_change_on=FALSE;
	portal->msg.entry_id=portal->msg.exit_id=0;
	portal->msg.base_team=0;
	portal->msg.map_name[0]=0x0;
	strcpy(portal->msg.map_spot_name,"Start");
	strcpy(portal->msg.map_spot_type,"Player");

	portal->mesh.nmesh=0;
	portal->mesh.meshes=NULL;
	
	portal->liquid.nliquid=0;
	portal->liquid.liquids=NULL;
    
	map_portal_sight_clear(map,rn);

	return(rn);
}

/* =======================================================

      Find Portals
      
======================================================= */

int map_find_portal(map_type *map,int x,int y,int z)
{
	int				n,nportal;
	portal_type		*portal;
	
	y=0;		// temporary fix to stop compiler warning; later on portals could become 3D

	nportal=map->nportal;
	portal=map->portals;
	
	for (n=0;n!=nportal;n++) {
		if ((x>=portal->x) && (x<=portal->ex) && (z>=portal->z) && (z<=portal->ez)) return(n);
		portal++;
	}
	
	return(-1);
}

int map_find_portal_hint(map_type *map,int hint_rn,int x,int y,int z)
{
	portal_type		*portal;
	
		// check hint first
		
	if (hint_rn!=-1) {
		portal=&map->portals[hint_rn];
		if ((x>=portal->x) && (x<=portal->ex) && (z>=portal->z) && (z<=portal->ez)) return(hint_rn);
	}
	
		// check all others
		
	return(map_find_portal(map,x,y,z));
}

bool map_find_portal_by_pos(map_type *map,d3pos *pos)
{
	int			rn;
	
	rn=map_find_portal(map,pos->x,pos->y,pos->z);
	if (rn==-1) return(FALSE);
	
	pos->rn=rn;
	return(TRUE);
}

/* =======================================================

      Calculate Centers for Portals
      
======================================================= */

void map_portal_calculate_y_extent(map_type *map,int rn,int *p_ty,int *p_by)
{
	int					n,k,ty,by;
	d3pnt				*pt;
	portal_type			*portal;
	map_mesh_type		*mesh;
	
	portal=&map->portals[rn];
	
	ty=1000000;
	by=-1000000;
	
		// run through all the vertexes
	
	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		pt=mesh->vertexes;
		
		for (k=0;k!=mesh->nvertex;k++) {
			if (pt->y>by) by=pt->y;
			if (pt->y<ty) ty=pt->y;
			pt++;
		}
	
		mesh++;
	}
	
		// corrections if no hits
		
	if (ty==1000000) {
		ty=by=0;
	}
	
	*p_ty=ty;
	*p_by=by;
}

void map_portal_calculate_center(map_type *map,int rn,int *x,int *y,int *z)
{
	int				ty,by;
	portal_type		*portal;
	
	portal=&map->portals[rn];
	
	*x=((portal->x+portal->ex)/2)-portal->x;
	*z=((portal->z+portal->ez)/2)-portal->z;
	
	map_portal_calculate_y_extent(map,rn,&ty,&by);
	
	*y=(ty+by)/2;
}

/* =======================================================

      Delete Portal
      
======================================================= */

void map_portal_delete(map_type *map,int rn)
{
	int				n;
	portal_type		*portal;
	
		// delete meshes and liquids
		
	portal=&map->portals[rn];
		
	while (portal->mesh.nmesh>0) {
		if (!map_portal_mesh_delete(map,rn,(portal->mesh.nmesh-1))) break;
	}
	
	while (portal->liquid.nliquid>0) {
		if (!map_portal_liquid_delete(map,rn,(portal->liquid.nliquid-1))) break;
	}

		// fix all map portal indexes
		
	for (n=0;n!=map->nnode;n++) {
		if (map->nodes[n].pos.rn>rn) map->nodes[n].pos.rn--;
	}
	
	for (n=0;n!=map->nspot;n++) {
		if (map->spots[n].pos.rn>rn) map->spots[n].pos.rn--;
	}
	
	for (n=0;n!=map->nscenery;n++) {
		if (map->sceneries[n].pos.rn>rn) map->sceneries[n].pos.rn--;
	}
	
	for (n=0;n!=map->nsound;n++) {
		if (map->sounds[n].pos.rn>rn) map->sounds[n].pos.rn--;
	}
	
	for (n=0;n!=map->nlight;n++) {
		if (map->lights[n].pos.rn>rn) map->lights[n].pos.rn--;
	}
	
	for (n=0;n!=map->nparticle;n++) {
		if (map->particles[n].pos.rn>rn) map->particles[n].pos.rn--;
	}
	
		// adjust sight paths
		
	map_portal_sight_delete_adjust_path(map,rn);
	
		// remove portal
		
	for (n=rn;n!=map->nportal;n++) {
		map->portals[n]=map->portals[n+1];
	}
	
	map->nportal--;
}

/* =======================================================

      Duplicate Portal
      
======================================================= */

int map_portal_duplicate(map_type *map,int rn,int x,int z)
{
	int				n,old_rn,mesh_idx,liq_idx;
	portal_type		*portal,*old_portal;
	map_mesh_type	*mesh,*old_mesh;
	map_liquid_type	*liq,*old_liq;
	
		// create new portal
		
	old_rn=rn;
	
	map->portals[map->nportal]=map->portals[rn];
	rn=map->nportal;
	map->nportal++;
	
	map->portals[rn].x=x;
	map->portals[rn].ex=x+(map->portals[old_rn].ex-map->portals[old_rn].x);
	map->portals[rn].z=z;
	map->portals[rn].ez=z+(map->portals[old_rn].ez-map->portals[old_rn].z);
	map_portal_sight_clear(map,rn);
	
		// duplicate meshes
		
	old_portal=&map->portals[old_rn];
	portal=&map->portals[rn];
	
	portal->mesh.nmesh=0;
	portal->mesh.meshes=NULL;
	
	for (n=0;n!=old_portal->mesh.nmesh;n++) {
		mesh_idx=map_portal_mesh_add(map,rn);
		if (mesh_idx==-1) break;
	
		old_mesh=&old_portal->mesh.meshes[n];
		mesh=&portal->mesh.meshes[mesh_idx];
	
		if (!map_portal_mesh_set_vertex_count(map,rn,mesh_idx,old_mesh->nvertex)) break;		
		if (!map_portal_mesh_set_poly_count(map,rn,mesh_idx,old_mesh->npoly)) break;
					
		mesh->group_idx=old_mesh->group_idx;
		memmove(&mesh->flag,&old_mesh->flag,sizeof(map_mesh_flag_type));

		memmove(mesh->vertexes,old_mesh->vertexes,(sizeof(d3pnt)*old_mesh->nvertex));
		memmove(mesh->polys,old_mesh->polys,(sizeof(map_mesh_poly_type)*old_mesh->npoly));
	}
	
		// duplicate liquids
		
	portal->liquid.nliquid=0;
	portal->liquid.liquids=NULL;
	
	for (n=0;n!=old_portal->liquid.nliquid;n++) {
		liq_idx=map_portal_liquid_add(map,rn);
		if (liq_idx==-1) break;
	
		old_liq=&old_portal->liquid.liquids[n];
		liq=&portal->liquid.liquids[liq_idx];
	
		memmove(liq,old_liq,sizeof(map_liquid_type));
	}
		
		// duplicate the other items
		// except for nodes
		
	for (n=0;n!=map->nspot;n++) {
		if (map->spots[n].pos.rn==old_rn) {
			if (map->nspot>=max_spot) break;

			memmove(&map->spots[map->nspot],&map->spots[n],sizeof(spot_type));
			map->spots[map->nspot].pos.rn=rn;
			map->nspot++;
		}
	}
	
	for (n=0;n!=map->nscenery;n++) {
		if (map->sceneries[n].pos.rn==old_rn) {
			if (map->nscenery>=max_map_scenery) break;

			memmove(&map->sceneries[map->nscenery],&map->sceneries[n],sizeof(map_scenery_type));
			map->sceneries[map->nscenery].pos.rn=rn;
			map->nscenery++;
		}
	}
	
	for (n=0;n!=map->nsound;n++) {
		if (map->sounds[n].pos.rn==old_rn) {
			if (map->nsound>=max_map_sound) break;

			memmove(&map->sounds[map->nsound],&map->sounds[n],sizeof(map_sound_type));
			map->sounds[map->nsound].pos.rn=rn;
			map->nsound++;
		}
	}
	
	for (n=0;n!=map->nlight;n++) {
		if (map->lights[n].pos.rn==old_rn) {
			if (map->nlight>=max_map_light) break;

			memmove(&map->lights[map->nlight],&map->lights[n],sizeof(map_light_type));
			map->lights[map->nlight].pos.rn=rn;
			map->nlight++;
		}
	}
	
	for (n=0;n!=map->nparticle;n++) {
		if (map->particles[n].pos.rn==old_rn) {
			if (map->nparticle>=max_map_particle) break;

			memmove(&map->particles[map->nparticle],&map->particles[n],sizeof(map_particle_type));
			map->particles[map->nparticle].pos.rn=rn;
			map->nparticle++;
		}
	}
	
	return(rn);
}

/* =======================================================

      Portals touching other Portals
      
======================================================= */

bool map_portal_touching_portal(map_type *map,int base_rn,int rn)
{
	int							i;
	portal_sight_list_type		*sight;

		// is this the base portal?
		
	if (base_rn==rn) return(TRUE);
	
		// then is the portal a root site path?

	sight=map->portals[base_rn].sight;

	for (i=0;i!=max_sight_list;i++) {
        if (sight->root) {
			if (sight->rn==rn) return(TRUE);
		}
		sight++;
	}
	
	return(FALSE);
}

/* =======================================================

      Sorting Portals for Drawing
  
======================================================= */

int map_portal_draw_sort(map_type *map,int rn,int cx,int cy,int cz,int *portal_list)
{
	int						n,k,d,idx,sz,cnt,
							dist[max_portal];
	portal_type				*portal;
	
	cnt=0;
	
	for (n=0;n!=map->nportal;n++) {
		if (n==rn) continue;
		
		portal=&map->portals[n];
		if (!portal->in_path) continue;
		
		d=distance_get(portal->mx,portal->by,portal->mz,cx,cy,cz);
		
		idx=-1;
		
		for (k=0;k!=cnt;k++) {
			if (dist[k]<d) {
				idx=k;
				break;
			}
		}
		
			// insert at end of list
			
		if (idx==-1) {
			dist[cnt]=d;
			portal_list[cnt]=n;
			cnt++;
			continue;
		}
		
			// insert in list
			
		sz=sizeof(int)*(cnt-idx);
		memmove(&dist[idx+1],&dist[idx],sz);
		memmove(&portal_list[idx+1],&portal_list[idx],sz);
		
		dist[idx]=d;
		portal_list[idx]=n;
		
		cnt++;
	}
	
	portal_list[cnt]=rn;
	
	return(cnt+1);
}



