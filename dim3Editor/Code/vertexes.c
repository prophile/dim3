/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Vertex Routines

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

#include "common_view.h"

extern int					cr,vertex_mode;
extern bool					dp_auto_texture;

extern map_type				map;

/* =======================================================

      Locked Vertexes in 3D
      
======================================================= */

void vertexes_move_similiar_to_vertex(segment_type *seg,int x,int y,int z,int xadd,int yadd,int zadd)
{
    int					n,k,lx,lz,rx,rz;
    segment_type		*seg2;
	wall_segment_data	*wall;
    fc_segment_data		*fc;
	portal_type			*portal;
    
    seg2=map.segments;
    
    for (n=0;n!=map.nsegment;n++) {
		
			// don't drag any vertexes of primitives
			
		if (seg2->primitive_uid[0]!=-1) {
			seg2++;
			continue;
		}

			// don't drag any vertexes of already selected segments
			
		if ((select_check_segment(n)) || (seg==seg2)) {
			seg2++;
			continue;
		}
		
			// special code for only dragging within a portal
			// this is current set to off, there's no UI to set this
			// it's basically an addition for later that might not
			// have any real use
	/*		
		if (seg2->rn!=seg->rn) {
			seg2++;
			continue;
		}
	*/	
			// drag the connected vertexes
			
		portal=&map.portals[seg2->rn];
		
		switch (seg2->type) {
			case sg_wall:
				wall=&seg2->data.wall;
				
				lx=wall->lx+portal->x;
				lz=wall->lz+portal->z;
				rx=wall->rx+portal->x;
				rz=wall->rz+portal->z;
				
				if ((lx==x) && (lz==z) && (wall->ty==y)) {
					wall->lx-=xadd;
					wall->lz-=zadd;
					wall->ty-=yadd;
					if (dp_auto_texture) segment_reset_texture_uvs(seg2);
					break;
				}
				
				if ((lx==x) && (lz==z) && ((wall->by+1)==y)) {
					wall->lx-=xadd;
					wall->lz-=zadd;
					wall->by-=yadd;
					if (dp_auto_texture) segment_reset_texture_uvs(seg2);
					break;
				}
				
				if ((rx==x) && (rz==z) && (wall->ty==y)) {
					wall->rx-=xadd;
					wall->rz-=zadd;
					wall->ty-=yadd;
					if (dp_auto_texture) segment_reset_texture_uvs(seg2);
					break;
				}
				
				if ((rx==x) && (rz==z) && ((wall->by+1)==y)) {
					wall->rx-=xadd;
					wall->rz-=zadd;
					wall->by-=yadd;
					if (dp_auto_texture) segment_reset_texture_uvs(seg2);
					break;
				}
					
				break;
				
			case sg_floor:
			case sg_ceiling:
				fc=&seg2->data.fc;
				for (k=0;k!=fc->ptsz;k++) {
					if (((fc->x[k]+portal->x)==x) && ((fc->z[k]+portal->z)==z) && (fc->y[k]==y)) {
						fc->x[k]-=xadd;
						fc->z[k]-=zadd;
						fc->y[k]-=yadd;
						if (dp_auto_texture) segment_reset_texture_uvs(seg2);
					}
				}
				break;
		}
		
		seg2++;
    }
}

void vertexes_move_similiar_to_segment(segment_type *seg,int xadd,int yadd,int zadd)
{
	int					n;
	portal_type			*portal;
	wall_segment_data	*wall;
	fc_segment_data		*fc;
	
	portal=&map.portals[seg->rn];
	
	switch (seg->type) {
		case sg_wall:
			wall=&seg->data.wall;
			vertexes_move_similiar_to_vertex(seg,(wall->lx+portal->x),wall->ty,(wall->lz+portal->z),xadd,yadd,zadd);
			vertexes_move_similiar_to_vertex(seg,(wall->rx+portal->x),wall->ty,(wall->rz+portal->z),xadd,yadd,zadd);
			vertexes_move_similiar_to_vertex(seg,(wall->lx+portal->x),(wall->by+1),(wall->lz+portal->z),xadd,yadd,zadd);
			vertexes_move_similiar_to_vertex(seg,(wall->rx+portal->x),(wall->by+1),(wall->rz+portal->z),xadd,yadd,zadd);
			break;
		case sg_floor:
		case sg_ceiling:
			fc=&seg->data.fc;
			for (n=0;n!=fc->ptsz;n++) {
				vertexes_move_similiar_to_vertex(seg,(fc->x[n]+portal->x),fc->y[n],(fc->z[n]+portal->z),xadd,yadd,zadd);
			}
			break;
	}
}

/* =======================================================

      Locked Vertexes in 2D
      
======================================================= */

void vertexes_move_to_2D_similiar_to_vertex(segment_type *seg,int x,int y,int z,int to_x,int to_z)
{
    int					n,lx,lz,rx,rz;
    segment_type		*seg2;
	wall_segment_data	*wall;
    fc_segment_data		*fc;
	portal_type			*portal;
    
    seg2=map.segments;
    
    for (n=0;n!=map.nsegment;n++) {
	
		if (seg==seg2) {
			seg2++;
			continue;
		}
		if (seg->rn!=seg2->rn) {
			seg2++;
			continue;
		}
		if (seg2->primitive_uid[0]!=-1) {
			seg2++;
			continue;
		}
		
		portal=&map.portals[seg2->rn];
		
		switch (seg2->type) {
			case sg_wall:
				wall=&seg2->data.wall;
				
				lx=wall->lx+portal->x;
				lz=wall->lz+portal->z;
				rx=wall->rx+portal->x;
				rz=wall->rz+portal->z;
				
				if ((lx==x) && (lz==z)) {
					wall->lx=to_x;
					wall->lz=to_z;
					if (dp_auto_texture) segment_reset_texture_uvs(seg2);
					break;
				}
				
				if ((rx==x) && (rz==z)) {
					wall->rx=to_x;
					wall->rz=to_z;
					if (dp_auto_texture) segment_reset_texture_uvs(seg2);
					break;
				}
					
				break;
				
			case sg_floor:
			case sg_ceiling:
				fc=&seg2->data.fc;
				for (n=0;n!=fc->ptsz;n++) {
					if (((fc->x[n]+portal->x)==x) && ((fc->z[n]+portal->z)==z) && (fc->y[n]==y)) {
						fc->x[n]=to_x;
						fc->z[n]=to_z;
						if (dp_auto_texture) segment_reset_texture_uvs(seg2);
					}
				}
				break;
		}
		
		seg2++;
    }
}

void vertexes_move_to_2D_similiar_to_segment(segment_type *seg,int whand,int x,int z)
{
	portal_type			*portal;
	wall_segment_data	*wall;
	fc_segment_data		*fc;
	
	portal=&map.portals[seg->rn];
	
	switch (seg->type) {
	
		case sg_wall:
			wall=&seg->data.wall;
			if (whand==0) {
				vertexes_move_to_2D_similiar_to_vertex(seg,(wall->lx+portal->x),wall->ty,(wall->lz+portal->z),x,z);
				vertexes_move_to_2D_similiar_to_vertex(seg,(wall->lx+portal->x),(wall->by+1),(wall->lz+portal->z),x,z);
				return;
			}
			if (whand==1) {
				vertexes_move_to_2D_similiar_to_vertex(seg,(wall->rx+portal->x),wall->ty,(wall->rz+portal->z),x,z);
				vertexes_move_to_2D_similiar_to_vertex(seg,(wall->rx+portal->x),(wall->by+1),(wall->rz+portal->z),x,z);
				return;
			}
			return;
			
		case sg_floor:
		case sg_ceiling:
			fc=&seg->data.fc;
			vertexes_move_to_2D_similiar_to_vertex(seg,(fc->x[whand]+portal->x),fc->y[whand],(fc->z[whand]+portal->z),x,z);
			break;
	}
}

/* =======================================================

      Move To Segment Vertex in 2D
      
======================================================= */

void vertexes_move_to_2D(segment_type *seg,int whand,int x,int z)
{
    switch (seg->type) {
    
        case sg_wall:
            if (whand==0) {
                seg->data.wall.lx=x;
				seg->data.wall.lz=z;
				if (dp_auto_texture) segment_reset_texture_uvs(seg);
                return;
            }
            if (whand==1) {
                seg->data.wall.rx=x;
                seg->data.wall.rz=z;
				if (dp_auto_texture) segment_reset_texture_uvs(seg);
                return;
            }
            return;
            
        case sg_floor:
        case sg_ceiling:
            seg->data.fc.x[whand]=x;
            seg->data.fc.z[whand]=z;
			if (dp_auto_texture) segment_reset_texture_uvs(seg);
			return;
			
		case sg_liquid:
			switch (whand) {
				case 0:
					seg->data.liquid.lft=x;
					seg->data.liquid.top=z;
					break;
				case 1:
					seg->data.liquid.lft=x;
					seg->data.liquid.bot=z;
					break;
				case 2:
					seg->data.liquid.rgt=x;
					seg->data.liquid.top=z;
					break;
				case 3:
					seg->data.liquid.rgt=x;
					seg->data.liquid.bot=z;
					break;
			}
			if (dp_auto_texture) segment_reset_texture_uvs(seg);
			return;
			
        case sg_ambient_wall:
            if (whand==0) {
                seg->data.ambient_wall.lx=x;
                seg->data.ambient_wall.lz=z;
				if (dp_auto_texture) segment_reset_texture_uvs(seg);
                return;
            }
            if (whand==1) {
                seg->data.ambient_wall.rx=x;
                seg->data.ambient_wall.rz=z;
				if (dp_auto_texture) segment_reset_texture_uvs(seg);
                return;
            }
            return;
			
        case sg_ambient_fc:
            seg->data.ambient_fc.x[whand]=x;
            seg->data.ambient_fc.z[whand]=z;
			if (dp_auto_texture) segment_reset_texture_uvs(seg);
            return;
            
    }
}

