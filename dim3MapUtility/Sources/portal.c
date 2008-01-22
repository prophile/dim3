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

      Count Segments for Portals
      
======================================================= */

int map_portal_count_segments(map_type *map,int rn)
{
	int				n,count;
	portal_type		*portal;
	segment_type	*seg;
	
	portal=&map->portals[rn];

	count=0;
	seg=map->segments;
	
	for (n=0;n!=map->nsegment;n++) {
		if (seg->rn==rn) count++;
		seg++;
	}
	
	return(count);
}

/* =======================================================

      Calculate Centers for Portals
      
======================================================= */

void map_portal_calculate_center(map_type *map,int rn,int *x,int *y,int *z)
{
	int				n,ty,by,kx,kz,ky;
	portal_type		*portal;
	segment_type	*seg;
	
	portal=&map->portals[rn];
	
		// center in portal
		
	*x=((portal->x+portal->ex)/2)-portal->x;
	*z=((portal->z+portal->ez)/2)-portal->z;
	
		// scan for center y
		
	ty=by=-1;
	
	seg=map->segments;
	
	for (n=0;n!=map->nsegment;n++) {
		if (seg->rn==rn) {
			map_segment_calculate_center(map,n,&kx,&ky,&kz);
			if (ty==-1) {
				ty=ky;
			}
			else {
				if (ky<ty) ty=ky;
			}
			if (by==-1) {
				by=ky;
			}
			else {
				if (ky>by) by=ky;
			}
		}
		seg++;
	}
	
	if ((ty!=-1) && (by!=-1)) {
		*y=(ty+by)/2;
	}
	else {
		if (ty!=-1) {
			*y=ty;
		}
		else {
			if (by!=-1) {
				*y=by;
			}
			else {
				*y=0;
			}
		}
	}
}

void map_portal_calculate_center_floor(map_type *map,int rn,int *x,int *y,int *z)
{
	int				n,kx,kz,ky;
	portal_type		*portal;
	segment_type	*seg;
	
	portal=&map->portals[rn];
	
		// center in portal
		
	*x=((portal->x+portal->ex)/2)-portal->x;
	*z=((portal->z+portal->ez)/2)-portal->z;
	
		// scan for bottom y
		
	*y=0;
	
	seg=map->segments;
	
	for (n=0;n!=map->nsegment;n++) {
		if (seg->rn==rn) {
			map_segment_calculate_center(map,n,&kx,&ky,&kz);
			ky--;
			if (ky>(*y)) *y=ky;
		}
		seg++;
	}
}

/* =======================================================

      Delete Portal
      
======================================================= */

void map_portal_delete(map_type *map,int rn)
{
	int				n,i;
	
		// delete segments
	
	n=0;
	while (n<map->nsegment) {
		if (map->segments[n].rn==rn) {
			for (i=n;i<map->nsegment;i++) {
				map->segments[i]=map->segments[i+1];
			}
			map->nsegment--;
		}
		else {
			n++;
		}
	}

		// fix all portal segments and pieces
		
	for (n=0;n!=map->nsegment;n++) {
		if (map->segments[n].rn>rn) map->segments[n].rn--;
	}

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
	int				n,k,old_rn;
	int				primitive_switch[255][2];
	bool			primitive_hit;
	
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
	
		// duplicate the segments
		
	for (n=0;n!=map->nsegment;n++) {
		if (map->segments[n].rn==old_rn) {
			if (map->nsegment>=max_segment) break;

			memmove(&map->segments[map->nsegment],&map->segments[n],sizeof(segment_type));
			map->segments[map->nsegment].rn=rn;
			map->nsegment++;
		}
	}
	
		// fix any primitives
		
	for (k=0;k!=255;k++) {
		primitive_switch[k][0]=-1;
	}
		
	for (n=0;n!=map->nsegment;n++) {
		if (map->segments[n].rn!=rn) continue;
		if (map->segments[n].primitive_uid[0]==-1) continue;
			
		primitive_hit=FALSE;
		
		for (k=0;k!=255;k++) {
			if (primitive_switch[k][0]==map->segments[n].primitive_uid[0]) {
				map->segments[n].primitive_uid[0]=primitive_switch[k][1];
				primitive_hit=TRUE;
				break;
			}
		}
		
		if (!primitive_hit) {
			for (k=0;k!=255;k++) {
				if (primitive_switch[k][0]==-1) {
					primitive_switch[k][0]=map->segments[n].primitive_uid[0];
					primitive_switch[k][1]=map_primitive_create_uid(map);
					map->segments[n].primitive_uid[0]=primitive_switch[k][1];
					break;
				}
			}
		}
	}
	
		// duplicate the spots, sceneries, lights, and sounds
		
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



