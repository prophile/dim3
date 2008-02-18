/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Site Path View Clicking Routines

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
#include "portal_view.h"
#include "site_path_view.h"
#include "walk_view.h"

extern int					cr,cx,cz,vertex_mode,
							site_path_view_drag_hilite_rn;
extern float				walk_view_y_angle;
extern bool					site_path_view_drag_on;
extern d3pnt				site_path_view_drag_start_pt,site_path_view_drag_end_pt;
extern Rect					site_path_view_box;
extern CCrsrHandle			dragcur,resizecur;
extern WindowRef			mainwind;

extern map_type				map;

/* =======================================================

      Click in Portals
      
======================================================= */

int site_path_view_select_portal(d3pnt *pt)
{
	register int	i;
	int				x,z,ex,ez;
	Rect			box;
	
		// find portal clicked on
	
	for (i=0;i!=map.nportal;i++) {
		site_path_view_get_portal(i,&x,&z,&ex,&ez);
		SetRect(&box,x,z,(ex+1),(ez+1));
		
		if (main_wind_click_check_box(pt,&box)) return(i);
	}
    
	return(-1);
}

/* =======================================================

      Portal Scrolling
      
======================================================= */

void site_path_view_mouse_move(d3pnt *pt)
{
	int						x,y,sx,sy;
	d3pnt					old_pt;
	Point					uipt;
	MouseTrackingResult		track;

	memmove(&old_pt,pt,sizeof(d3pnt));
	
	sx=sy=-1;
	
	do {
		TrackMouseLocation(NULL,&uipt,&track);
		pt->x=uipt.h;
		pt->y=uipt.v;

		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		x=old_pt.x-pt->x;
		y=old_pt.y-pt->y;

		memmove(&old_pt,pt,sizeof(d3pnt));
		
		cx+=(x*map_enlarge);
		cz+=(y*map_enlarge);
		
         main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
}

/* =======================================================

      Sight Path Utilities
      
======================================================= */

int site_path_view_find_open_sight(void)
{
	register int	n;
	portal_type		*portal;
	
	portal=&map.portals[cr];
	
	for (n=0;n!=max_sight_list;n++) {
		if (portal->sight[n].rn==nopath) return(n);
	}
	
	return(-1);
}

int site_path_view_find_site_for_portal(int rn)
{
	register int	n;
	portal_type		*portal;
	
	portal=&map.portals[cr];
	
	for (n=0;n!=max_sight_list;n++) {
		if (portal->sight[n].rn==rn) return(n);
	}
	
	return(-1);
}

bool site_path_view_path_duplicate(int rn)
{
	register int	n;
	int				r;
	portal_type		*portal;
	
	if (rn==cr) return(TRUE);
	
	portal=&map.portals[cr];
	
	for (n=0;n!=max_sight_list;n++) {
		r=portal->sight[n].rn;
		if ((r!=nopath) && (r==rn)) return(TRUE);
	}
	
	return(FALSE);
}

/* =======================================================

      Cut Sight Path
      
======================================================= */

bool site_path_view_cut_sight(d3pnt *pt)
{
	int				rn,site_idx;
	
		// find sight clicked on
	
	rn=site_path_view_select_portal(pt);
	if (rn==-1) return(FALSE);
	
	site_idx=site_path_view_find_site_for_portal(rn);
	if (site_idx==-1) return(FALSE);
	
        // cut path
	
	map_portal_sight_prune(&map,cr,site_idx);
	
    main_wind_draw();
    return(TRUE);
}

/* =======================================================

      Drag Sight Path
      
======================================================= */

int site_path_view_sight_drag(int start_rn,d3pnt *pt)
{
	int						rn;
	d3pnt					old_pt;
	Point					uipt;
	MouseTrackingResult		track;
	
	memmove(&old_pt,pt,sizeof(d3pnt));
	
	site_path_view_drag_on=TRUE;
	site_path_view_drag_hilite_rn=start_rn;
	memmove(&site_path_view_drag_start_pt.x,pt,sizeof(d3pnt));
	memmove(&site_path_view_drag_end_pt.x,pt,sizeof(d3pnt));
	main_wind_draw();
	
	do {
		TrackMouseLocation(NULL,&uipt,&track);
		pt->x=uipt.h;
		pt->y=uipt.v;
	
		rn=site_path_view_select_portal(pt);
		
			// is room already in sight path
			
		if (rn!=-1) {
			if (site_path_view_path_duplicate(rn)) rn=-1;
		}

		if ((pt->x==old_pt.x) && (pt->y==old_pt.y)) continue;
		
		site_path_view_drag_hilite_rn=rn;
		memmove(&site_path_view_drag_end_pt.x,pt,sizeof(d3pnt));
		
		main_wind_draw();

		memmove(&old_pt,pt,sizeof(d3pnt));
		
	} while (track!=kMouseTrackingMouseReleased);

	site_path_view_drag_on=FALSE;
	site_path_view_drag_hilite_rn=-1;
	
	main_wind_draw();

	return(rn);	
}

/* =======================================================

      Click in Sight Path
      
======================================================= */

bool site_path_view_click_sight(d3pnt *pt)
{
	register int	n;
	int				t,i,k,start_rn,rn;
	d3pnt			dpt;
	Rect			box;
	portal_type		*portal;
	
	portal=&map.portals[cr];
	
		// click in portal
		
	start_rn=site_path_view_select_portal(pt);
	
	if (start_rn!=cr) {
		if (site_path_view_find_site_for_portal(start_rn)==-1) return(FALSE);		// this portal not in site path
	}
		
		// run the drag
		
	site_path_view_get_site_path_box(start_rn,&box);
	
	dpt.x=(box.left+box.right)/2;
	dpt.y=(box.top+box.bottom)/2;
	
	rn=site_path_view_sight_drag(start_rn,&dpt);

		// path dropping
		
	if (rn!=-1) {
		n=site_path_view_find_open_sight();
		if (n==-1) {
			SysBeep(2);
			return(TRUE);
        }
		
		for (t=0;t!=max_sight_link;t++) {
			portal->sight[n].link[t]=nopath;
		}
            
            // new root path
            
		if (start_rn==cr) {
            portal->sight[n].root=TRUE;
            portal->sight[n].rn=rn;
            main_wind_draw();
            return(TRUE);
        }
        
            // new link in list
		
		k=site_path_view_find_site_for_portal(start_rn);
		
        for (i=0;i!=max_sight_link;i++) {
            if (portal->sight[k].link[i]==nopath) {
                portal->sight[k].link[i]=n;
				portal->sight[n].root=FALSE;
				portal->sight[n].rn=rn;
				main_wind_draw();
                return(TRUE);
            }
        }
		
		SysBeep(2);				// ran out of sight links
	}

	return(TRUE);
}

