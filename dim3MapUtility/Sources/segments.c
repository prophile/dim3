/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map Segment Routines

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

      Find Segment Group By Name
      
======================================================= */

int map_segment_group_find_name(map_type *map,char *name)
{
	int					i;
	group_type			*group;
	
	group=map->groups;
	
	for (i=0;i!=map->ngroup;i++) {
		if (strcmp(group->name,name)==0) return(i);
		group++;
	}
	
	return(-1);
}

/* =======================================================

      Move Segments
      
======================================================= */

void map_segment_move(map_type *map,segment_type *seg,int x,int y,int z)
{
	int							i,k,
								ptsz,idx,v_cnt,v_idx[light_tessel_max_vertex+8];
	float						f_xmove,f_ymove,f_zmove;
	bool						hit;
	portal_vertex_list_type		*vl;
	wall_segment_data			*wall;
    fc_segment_data				*fc;
    liquid_segment_data			*liq;
    ambient_wall_segment_data	*ambient_wall;
    ambient_fc_segment_data		*ambient_fc;
	portal_type					*portal;
    
		// move the segment
		
	switch (seg->type) {
	
		case sg_wall:
            wall=&seg->data.wall;
            
			ptsz=wall->ptsz;
			for (i=0;i!=ptsz;i++) {
				wall->x[i]+=x;
				wall->z[i]+=z;
				wall->y[i]+=y;
			}
            
			wall->lx+=x;
			wall->rx+=x;
			wall->lz+=z;
			wall->rz+=z;
			wall->ty+=y;
			wall->by+=y;
			break;
			
		case sg_floor:
		case sg_ceiling:
            fc=&seg->data.fc;
            
			ptsz=fc->ptsz;
			for (i=0;i!=ptsz;i++) {
				fc->x[i]+=x;
				fc->z[i]+=z;
				fc->y[i]+=y;
			}
            
			fc->min_x+=x;
			fc->max_x+=x;
			fc->min_z+=z;
			fc->max_z+=z;
			fc->min_y+=y;
			fc->max_y+=y;
			break;
		
		case sg_liquid:
            liq=&seg->data.liquid;
            
			liq->lft+=x;
			liq->rgt+=x;
			liq->top+=z;
			liq->bot+=z;
			liq->y+=y;
			break;
			
		case sg_ambient_wall:
            ambient_wall=&seg->data.ambient_wall;
			
			for (i=0;i!=4;i++) {
				ambient_wall->x[i]+=x;
				ambient_wall->z[i]+=z;
				ambient_wall->y[i]+=y;
			}
            
			ambient_wall->lx+=x;
			ambient_wall->rx+=x;
			ambient_wall->lz+=z;
			ambient_wall->rz+=z;
			ambient_wall->ty+=y;
			ambient_wall->by+=y;
			break;
			
		case sg_ambient_fc:
            ambient_fc=&seg->data.ambient_fc;
            
			ptsz=ambient_fc->ptsz;
			for (i=0;i!=ptsz;i++) {
				ambient_fc->x[i]+=x;
				ambient_fc->z[i]+=z;
				ambient_fc->y[i]+=y;
			}

			ambient_fc->min_x+=x;
			ambient_fc->max_x+=x;
			ambient_fc->min_z+=z;
			ambient_fc->max_z+=z;
			ambient_fc->min_y+=y;
			ambient_fc->max_y+=y;
			break;
			
	}
	
		// find all movable vertexes in portal vertex list
		// there can be shared vertexes in this list
		
	portal=&map->portals[seg->rn];
	
	v_cnt=0;
	
	ptsz=seg->draw.ptsz;
	for (i=0;i!=ptsz;i++) {
		idx=seg->draw.idx[i];
		
		hit=FALSE;
		for (k=(v_cnt-1);k>=0;k--) {
			if (idx==v_idx[k]) {
				hit=TRUE;
				break;
			}
		}
		
		if (!hit) {
			v_idx[v_cnt]=idx;
			v_cnt++;
		}
	}
	
	ptsz=seg->light.trig_count*3;
	
	for (i=0;i!=ptsz;i++) {
		idx=seg->light.trig_vertex_idx[i];
		
		hit=FALSE;
		for (k=(v_cnt-1);k>=0;k--) {
			if (idx==v_idx[k]) {
				hit=TRUE;
				break;
			}
		}
		
		if (!hit) {
			v_idx[v_cnt]=idx;
			v_cnt++;
		}
	}
	
		// move the precompiled portal vertex lists
		
	f_xmove=(float)x;
	f_ymove=(float)y;
	f_zmove=(float)z;
	
	for (i=0;i!=v_cnt;i++) {
		vl=portal->vertexes.vertex_list+v_idx[i];
		vl->x+=f_xmove;
		vl->z+=f_zmove;
		vl->y+=f_ymove;
	}
		
		// recalculate middles
    
	map_prepare_calculate_segment_vertex_middle(seg);
	
	seg->touched=TRUE;
}

/* =======================================================

      Calculate Segment Extent or Center
      
======================================================= */

void map_segment_calculate_extent(map_type *map,int seg_idx,int *min_x,int *min_z,int *min_y,int *max_x,int *max_z,int *max_y)
{
	int					i;
	segment_type		*seg;
	
	seg=&map->segments[seg_idx];
	
	switch (seg->type) {
		case sg_wall:
			if (seg->data.wall.lx<seg->data.wall.rx) {
				*min_x=seg->data.wall.lx;
				*max_x=seg->data.wall.rx;
			}
			else {
				*min_x=seg->data.wall.rx;
				*max_x=seg->data.wall.lx;
			}
			if (seg->data.wall.lz<seg->data.wall.rz) {
				*min_z=seg->data.wall.lz;
				*max_z=seg->data.wall.rz;
			}
			else {
				*min_z=seg->data.wall.rz;
				*max_z=seg->data.wall.lz;
			}
			*min_y=seg->data.wall.ty;
			*max_y=seg->data.wall.by;
			return;
			
		case sg_floor:
		case sg_ceiling:
			*min_x=*max_x=seg->data.fc.x[0];
			*min_z=*max_z=seg->data.fc.z[0];
			*min_y=*max_y=seg->data.fc.y[0];
			for (i=0;i!=seg->data.fc.ptsz;i++) {
				if (seg->data.fc.x[i]<(*min_x)) *min_x=seg->data.fc.x[i];
				if (seg->data.fc.x[i]>(*max_x)) *max_x=seg->data.fc.x[i];
				if (seg->data.fc.z[i]<(*min_z)) *min_z=seg->data.fc.z[i];
				if (seg->data.fc.z[i]>(*max_z)) *max_z=seg->data.fc.z[i];
				if (seg->data.fc.y[i]<(*min_y)) *min_y=seg->data.fc.y[i];
				if (seg->data.fc.y[i]>(*max_y)) *max_y=seg->data.fc.y[i];
			}
			return;
			
		case sg_liquid:
			*min_x=seg->data.liquid.lft;
			*max_x=seg->data.liquid.rgt;
			*min_z=seg->data.liquid.top;
			*max_z=seg->data.liquid.bot;
			*min_y=*max_y=seg->data.liquid.y;
			return;
			
		case sg_ambient_wall:
			if (seg->data.ambient_wall.lx<seg->data.ambient_wall.rx) {
				*min_x=seg->data.ambient_wall.lx;
				*max_x=seg->data.ambient_wall.rx;
			}
			else {
				*min_x=seg->data.ambient_wall.rx;
				*max_x=seg->data.ambient_wall.lx;
			}
			if (seg->data.ambient_wall.lz<seg->data.ambient_wall.rz) {
				*min_z=seg->data.ambient_wall.lz;
				*max_z=seg->data.ambient_wall.rz;
			}
			else {
				*min_z=seg->data.ambient_wall.rz;
				*max_z=seg->data.ambient_wall.lz;
			}
			*min_y=seg->data.ambient_wall.ty;
			*max_y=seg->data.ambient_wall.by;
			return;
			
		case sg_ambient_fc:
			*min_x=*max_x=seg->data.ambient_fc.x[0];
			*min_z=*max_z=seg->data.ambient_fc.z[0];
			*min_y=*max_y=seg->data.ambient_fc.y[0];
			for (i=0;i!=seg->data.ambient_fc.ptsz;i++) {
				if (seg->data.ambient_fc.x[i]<(*min_x)) *min_x=seg->data.ambient_fc.x[i];
				if (seg->data.ambient_fc.x[i]>(*max_x)) *max_x=seg->data.ambient_fc.x[i];
				if (seg->data.ambient_fc.z[i]<(*min_z)) *min_z=seg->data.ambient_fc.z[i];
				if (seg->data.ambient_fc.z[i]>(*max_z)) *max_z=seg->data.ambient_fc.z[i];
				if (seg->data.ambient_fc.y[i]<(*min_y)) *min_y=seg->data.ambient_fc.y[i];
				if (seg->data.ambient_fc.y[i]>(*max_y)) *max_y=seg->data.ambient_fc.y[i];
			}
			return;
			
	}
}

void map_segment_calculate_center(map_type *map,int seg_idx,int *x,int *y,int *z)
{
	int				i,cx,cy,cz;
    segment_type	*seg;
	
	cx=cy=cz=0;
	
	seg=&map->segments[seg_idx];
	
	switch (seg->type) {
		case sg_wall:
			cx=(seg->data.wall.lx+seg->data.wall.rx)/2;
			cz=(seg->data.wall.lz+seg->data.wall.rz)/2;
			cy=(seg->data.wall.ty+seg->data.wall.by)/2;
			break;
		case sg_floor:
		case sg_ceiling:
			cx=cz=cy=0;
            for (i=0;i!=seg->data.fc.ptsz;i++) {
                cx+=seg->data.fc.x[i];
				cz+=seg->data.fc.z[i];
				cy+=seg->data.fc.y[i];
            }
			cx/=seg->data.fc.ptsz;
			cz/=seg->data.fc.ptsz;
			cy/=seg->data.fc.ptsz;
			break;
       case sg_liquid:
 			cx=(seg->data.liquid.lft+seg->data.liquid.rgt)/2;
			cz=(seg->data.liquid.top+seg->data.liquid.bot)/2;
			cy=seg->data.liquid.y;
			break;
        case sg_ambient_wall:
			cx=(seg->data.ambient_wall.lx+seg->data.ambient_wall.rx)/2;
			cz=(seg->data.ambient_wall.lz+seg->data.ambient_wall.rz)/2;
			cy=(seg->data.ambient_wall.ty+seg->data.ambient_wall.by)/2;
			break;
		case sg_ambient_fc:
			cx=cz=cy=0;
            for (i=0;i!=seg->data.ambient_fc.ptsz;i++) {
                cx+=seg->data.ambient_fc.x[i];
				cz+=seg->data.ambient_fc.z[i];
				cy+=seg->data.ambient_fc.y[i];
            }
			cx/=seg->data.ambient_fc.ptsz;
			cz/=seg->data.ambient_fc.ptsz;
			cy/=seg->data.ambient_fc.ptsz;
			break;
      }
	  
	  *x=cx;
	  *y=cy;
	  *z=cz;
}

/* =======================================================

      Clear Touchs
      
======================================================= */

void map_segments_clear_touch(map_type *map)
{
	int				n,nsegment;
	segment_type	*seg;
	
	nsegment=map->nsegment;
	seg=map->segments;
	
	for (n=0;n!=nsegment;n++) {
		seg->touched=FALSE;
		seg++;
	}
}

int map_segments_count_touch(map_type *map)
{
	int				n,nsegment,count;
	segment_type	*seg;
	
	nsegment=map->nsegment;
	seg=map->segments;
	
	count=0;
	
	for (n=0;n!=nsegment;n++) {
		if (seg->touched) count++;
		seg++;
	}
	
	return(count);
}

