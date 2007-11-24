/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Handle Clicking Routines

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

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern int					cr,cx,cz,cy,vertex_mode;
extern bool					dp_auto_texture;
extern CCrsrHandle			dragcur;

extern map_type				map;

/* =======================================================

      Move Similiar Handles
      
======================================================= */

void walk_view_move_similiar_handle(int rn,int x,int z,int y,int xadd,int zadd,int yadd)
{
    int				i,k,ptsz,nx,nz,nrn;
    segment_type	*seg;
    fc_segment_data	*fc;
    
    x+=map.portals[rn].x;
    z+=map.portals[rn].z;
    
    seg=map.segments;
    
    for ((i=0);(i!=map.nsegment);i++) {
	
		if (seg->primitive_uid!=-1) {
			seg++;
			continue;
		}
    
        if ((seg->type==sg_floor) || (seg->type==sg_ceiling)) {
        	fc=&seg->data.fc;
            
            nrn=seg->rn;
            
            ptsz=fc->ptsz;
            for ((k=0);(k!=ptsz);k++) {
                nx=fc->x[k]+map.portals[nrn].x;
                nz=fc->z[k]+map.portals[nrn].z;
                
                if ((nx==x) && (nz==z) && (fc->y[k]==y)) {
                    fc->x[k]-=xadd;
                    fc->z[k]-=zadd;
                    fc->y[k]-=yadd;
                }
            }
        }
    
        seg++;
    }
}

/* =======================================================

      Drag Single Segment Handles
      
======================================================= */

void walk_view_move_single_handle(int index,int wpt,int xadd,int zadd,int yadd)
{
	segment_type	*seg;
    
    seg=&map.segments[index];
    
    switch (seg->type) {
    
        case sg_wall:
            switch (wpt) {
                case 0:
                    seg->data.wall.ty-=yadd;
                    if (seg->data.wall.ty<0) seg->data.wall.ty=0;
                    if (seg->data.wall.ty>seg->data.wall.by) seg->data.wall.ty=seg->data.wall.by;
                    break;
                case 1:
                    seg->data.wall.rx-=xadd;
                    seg->data.wall.rz-=zadd;
                    break;
                case 2:
                    seg->data.wall.by-=yadd;
                    if (seg->data.wall.by<seg->data.wall.ty) seg->data.wall.by=seg->data.wall.ty;
                    break;
                case 3:
                    seg->data.wall.lx-=xadd;
                    seg->data.wall.lz-=zadd;
                    break;
            }
            break;
            
        case sg_floor:
        case sg_ceiling:
            if (vertex_mode==vm_lock) {
                walk_view_move_similiar_handle(seg->rn,seg->data.fc.x[wpt],seg->data.fc.z[wpt],seg->data.fc.y[wpt],xadd,zadd,yadd);
                break;
            }
           
            seg->data.fc.x[wpt]-=xadd;
            seg->data.fc.z[wpt]-=zadd;
            seg->data.fc.y[wpt]-=yadd;
            break;
            
        case sg_liquid:
            switch (wpt) {
                case 0:
                    seg->data.liquid.top-=zadd;
                    break;
                case 1:
                    seg->data.liquid.rgt-=xadd;
                    break;
                case 2:
                    seg->data.liquid.bot-=zadd;
                    break;
                case 3:
                    seg->data.liquid.lft-=xadd;
                    break;
            }
            seg->data.liquid.y-=yadd;
            break;	

        case sg_ambient_wall:
            switch (wpt) {
                case 0:
                    seg->data.ambient_wall.ty-=yadd;
                    if (seg->data.ambient_wall.ty<0) seg->data.ambient_wall.ty=0;
                    if (seg->data.ambient_wall.ty>seg->data.ambient_wall.by) seg->data.ambient_wall.ty=seg->data.ambient_wall.by;
                    break;
                case 1:
                    seg->data.ambient_wall.rx-=xadd;
                    seg->data.ambient_wall.rz-=zadd;
                    break;
                case 2:
                    seg->data.ambient_wall.by-=yadd;
                    if (seg->data.ambient_wall.by<seg->data.ambient_wall.ty) seg->data.ambient_wall.by=seg->data.ambient_wall.ty;
                    break;
                case 3:
                    seg->data.ambient_wall.lx-=xadd;
                    seg->data.ambient_wall.lz-=zadd;
                    break;
            }
            break;
			
        case sg_ambient_fc:
            if (vertex_mode==vm_lock) {
                walk_view_move_similiar_handle(seg->rn,seg->data.ambient_fc.x[wpt],seg->data.ambient_fc.z[wpt],seg->data.ambient_fc.y[wpt],xadd,zadd,yadd);
                break;
            }
           
            seg->data.ambient_fc.x[wpt]-=xadd;
            seg->data.ambient_fc.z[wpt]-=zadd;
            seg->data.ambient_fc.y[wpt]-=yadd;
            break;
    }
	
	if (dp_auto_texture) segment_reset_texture_uvs(seg);
}

bool walk_view_drag_segment_handle(Point pt,int index,int wpt)
{
	int						x,y,xadd,zadd,yadd;
	Point					oldpt;
	bool					first_drag;
	MouseTrackingResult		track;

    if (!Button()) return(FALSE);
	
	first_drag=TRUE;
	
	oldpt=pt;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);
		
		if ((pt.h==oldpt.h) && (pt.v==oldpt.v)) continue;
		
		x=(oldpt.h-pt.h)/5;
		y=(oldpt.v-pt.v)/5;
		if ((x==0) && (y==0)) continue;
		
		oldpt=pt;
		
			// turn on drag cursor
			
		if (first_drag) {
			SetCCursor(dragcur);
			first_drag=FALSE;
		}
		
			// move item

		walk_view_get_piece_movement(FALSE,x,y,&xadd,&zadd,&yadd);
		walk_view_move_single_handle(index,wpt,xadd,zadd,yadd);

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	InitCursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Primitive Handles
      
======================================================= */

bool walk_view_drag_primitive_handle(Point pt,int index,int wpt)
{
	int						primitive_uid,x,y,old_x,old_y,xadd,zadd,yadd,
							org_minx,org_maxx,org_minz,org_maxz,org_miny,org_maxy,
							new_minx,new_maxx,new_minz,new_maxz,new_miny,new_maxy;
	Point					oldpt;
	bool					first_drag;
	MouseTrackingResult		track;
	
    if (!Button()) return(FALSE);
	
	primitive_uid=map.segments[index].primitive_uid;
	primitive_get_extend(primitive_uid,&org_minx,&org_maxx,&org_minz,&org_maxz,&org_miny,&org_maxy);
	primitive_backup(primitive_uid);
	
	first_drag=TRUE;
	
	oldpt=pt;
	old_x=old_y=0;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);
		
		if ((pt.h==oldpt.h) && (pt.v==oldpt.v)) continue;
		
		x=(oldpt.h-pt.h)/5;
		y=(oldpt.v-pt.v)/5;
		
		if ((x==old_x) && (y==old_y)) continue;
		
		old_x=x;
		old_y=y;
		
			// turn on drag cursor
			
		if (first_drag) {
			SetCCursor(dragcur);
			first_drag=FALSE;
		}
		
			// move primitive
			
		walk_view_get_piece_movement(FALSE,x,y,&xadd,&zadd,&yadd);
		
		new_minx=org_minx;
		new_maxx=org_maxx;
		new_minz=org_minz;
		new_maxz=org_maxz;
		new_miny=org_miny;
		new_maxy=org_maxy;
		
		switch (wpt) {
			case 0:
				new_minx-=xadd;
				break;
			case 1:
				new_maxx-=xadd;
				break;
			case 2:
				new_minz-=zadd;
				break;
			case 3:
				new_maxz-=zadd;
				break;
			case 4:
				new_miny-=yadd;
				break;
			case 5:
				new_maxy-=yadd;
				break;
		}
			
		primitive_restore(primitive_uid);
		primitive_resize(primitive_uid,new_minx,new_maxx,new_minz,new_maxz,new_miny,new_maxy);

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	InitCursor();
	
	return(!first_drag);
}

