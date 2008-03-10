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

// supergumba -- delete?
void walk_view_move_similiar_handle(int rn,int x,int z,int y,int xadd,int zadd,int yadd)
{
    int				i,k,ptsz,nx,nz,nrn;
    segment_type	*seg;
    fc_segment_data	*fc;
    
    x+=map.portals[rn].x;
    z+=map.portals[rn].z;
    
    seg=map.segments;
    
    for ((i=0);(i!=map.nsegment);i++) {
	
		if (seg->primitive_uid[0]!=-1) {
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

      Drag Single Mesh Handles
      
======================================================= */

bool walk_view_drag_segment_handle(editor_3D_view_setup *view_setup,d3pnt *pt,int view_move_dir,int portal_idx,int mesh_idx,int vertex_idx)
{
	int						x,y,xadd,zadd,yadd;
	d3pnt					old_pt,*dpt;
	Point					uipt;
	bool					first_drag;
	MouseTrackingResult		track;
	portal_type				*portal;
	map_mesh_type			*mesh;

    if (!Button()) return(FALSE);
	
		// get mesh point
		
	portal=&map.portals[portal_idx];
	mesh=&portal->mesh.meshes[mesh_idx];
	dpt=&mesh->vertexes[vertex_idx];
		
		// drag mesh
	
	first_drag=TRUE;
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	do {
		TrackMouseLocation(NULL,&uipt,&track);
		pt->x=uipt.h-view_setup->box.left;
		pt->y=uipt.v-view_setup->box.top;
		
		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;
		
		memmove(&old_pt,pt,sizeof(d3pnt));
		
			// turn on drag cursor
			
		if (first_drag) {
			SetCCursor(dragcur);
			first_drag=FALSE;
		}
		
			// move vertex

		walk_view_click_drag_movement(view_setup,view_move_dir,x,y,&xadd,&yadd,&zadd);
			
		dpt->x+=xadd;
		dpt->y+=yadd;
		dpt->z+=zadd;

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	InitCursor();
	
	return(!first_drag);
}

/* =======================================================

      Drag Primitive Handles
      
======================================================= */

// supergumba -- can probably delete from new version

bool walk_view_drag_primitive_handle(Point pt,int index,int wpt)
{
	int						primitive_uid,x,y,old_x,old_y,xadd,zadd,yadd,
							org_minx,org_maxx,org_minz,org_maxz,org_miny,org_maxy,
							new_minx,new_maxx,new_minz,new_maxz,new_miny,new_maxy;
	Point					oldpt;
	bool					first_drag;
	MouseTrackingResult		track;
	
    if (!Button()) return(FALSE);
	
	primitive_uid=map.segments[index].primitive_uid[0];
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

