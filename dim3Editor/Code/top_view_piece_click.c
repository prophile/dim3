/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Top View Piece Click Routines

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
#include "dialog.h"
#include "common_view.h"
#include "top_view.h"

extern CCrsrHandle			dragcur;

extern int					cr,cx,cz,vertex_mode,magnify_factor;
extern unsigned short		effect,effectmask;
extern int					cr,txtfill,txtrbfill;
extern bool					dp_wall,dp_floor,dp_ceiling,dp_liquid,dp_ambient,dp_object,dp_lightsoundparticle,dp_node;

extern map_type				map;

/* =======================================================

      Segment Handle Snapping
      
======================================================= */

void top_view_snap_handle(int rn,int index,int *x,int *z)
{
    int					i,k,crn,cx,cz,nx,nz,ptsz;
    segment_type		*seg;
    wall_segment_data	*wall;
    fc_segment_data		*fc;
    
    nx=(*x)+map.portals[rn].x;
    nz=(*z)+map.portals[rn].z;
    
    for (i=0;i!=map.nsegment;i++) {
        if (i==index) continue;
        
        seg=&map.segments[i];
        crn=seg->rn;
        
        if (seg->type==sg_wall) {
            wall=&seg->data.wall;
            
            cx=wall->lx+map.portals[crn].x;
            cz=wall->lz+map.portals[crn].z;
            
            if ((abs(cx-nx)<=5) && (abs(cz-nz)<=5)) {
                *x=cx-map.portals[rn].x;
                *z=cz-map.portals[rn].z;
                return;
            }
            
            cx=wall->rx+map.portals[crn].x;
            cz=wall->rz+map.portals[crn].z;
            
            if ((abs(cx-nx)<=5) && (abs(cz-nz)<=5)) {
                *x=cx-map.portals[rn].x;
                *z=cz-map.portals[rn].z;
                return;
            }
        }
        
        if ((seg->type==sg_floor) || (seg->type==sg_ceiling)) {
        	fc=&seg->data.fc;
            
            ptsz=fc->ptsz;
            for ((k=0);(k!=ptsz);k++) {
                cx=fc->x[k]+map.portals[crn].x;
                cz=fc->z[k]+map.portals[crn].z;
                
                if ((abs(cx-nx)<=5) && (abs(cz-nz)<=5)) {
                    *x=cx-map.portals[rn].x;
                    *z=cz-map.portals[rn].z;
                    return;
                }
            }
        }
    }
}

/* =======================================================

      Draw Segment Handles
      
======================================================= */

bool top_view_single_segment_handle_drag(int index,int whand)
{
	int						rn,x,z,oldx,oldz,xoff,zoff,portal_maxx,portal_maxz;
	segment_type			*seg;
	Point					pt;
	MouseTrackingResult		track;

	seg=&map.segments[index];
    rn=seg->rn;
    
	xoff=map.portals[rn].x;
	zoff=map.portals[rn].z;
	top_view_map_to_pane(&xoff,&zoff);
	
	portal_maxx=map.portals[rn].ex-map.portals[rn].x;
	portal_maxz=map.portals[rn].ez-map.portals[rn].z;
	
	oldx=oldz=-1;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);

		x=top_view_pane_to_map_factor(pt.h-xoff);
		z=top_view_pane_to_map_factor(pt.v-zoff);
		if (x<0) x=0;
		if (z<0) z=0;
		if (x>portal_maxx) x=portal_maxx;
		if (z>portal_maxz) z=portal_maxz;
		
		if ((oldx==x) && (oldz==z)) continue;
        
		if (vertex_mode==vm_snap) top_view_snap_handle(rn,index,&x,&z);
		if (vertex_mode==vm_lock) vertexes_move_to_2D_similiar_to_segment(seg,whand,x,z);
		vertexes_move_to_2D(seg,whand,x,z);
		
		main_wind_draw();
		oldx=x;
		oldz=z;

	} while (track!=kMouseTrackingMouseReleased);
    
    return(TRUE);
}

bool top_view_primitive_handle_drag(int index,int whand)
{
	int						rn,x,z,oldx,oldz,xoff,zoff,portal_maxx,portal_maxz,
							primitive_uid,
							org_minx,org_maxx,org_minz,org_maxz,org_miny,org_maxy,
							new_minx,new_maxx,new_minz,new_maxz,new_miny,new_maxy;
	segment_type			*seg;
	Point					pt;
	MouseTrackingResult		track;

	seg=&map.segments[index];
    rn=seg->rn;
	
	primitive_uid=seg->primitive_uid[0];
	primitive_get_extend(primitive_uid,&org_minx,&org_maxx,&org_minz,&org_maxz,&org_miny,&org_maxy);
	primitive_backup(primitive_uid);

	xoff=map.portals[rn].x;
	zoff=map.portals[rn].z;
	top_view_map_to_pane(&xoff,&zoff);
	
	portal_maxx=map.portals[rn].ex-map.portals[rn].x;
	portal_maxz=map.portals[rn].ez-map.portals[rn].z;
	
	oldx=oldz=-1;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);

		x=top_view_pane_to_map_factor(pt.h-xoff);
		z=top_view_pane_to_map_factor(pt.v-zoff);
		if (x<0) x=0;
		if (z<0) z=0;
		if (x>portal_maxx) x=portal_maxx;
		if (z>portal_maxz) z=portal_maxz;
		
		if ((oldx==x) && (oldz==z)) continue;
		
		new_minx=org_minx;
		new_maxx=org_maxx;
		new_minz=org_minz;
		new_maxz=org_maxz;
		new_miny=org_miny;
		new_maxy=org_maxy;
		
		switch (whand) {
			case 0:
				new_minx=x;
				break;
			case 1:
				new_maxx=x;
				break;
			case 2:
				new_minz=z;
				break;
			case 3:
				new_maxz=z;
				break;
		}
        
		primitive_restore(primitive_uid);
		primitive_resize(primitive_uid,new_minx,new_maxx,new_minz,new_maxz,new_miny,new_maxy);
		
		main_wind_draw();
		oldx=x;
		oldz=z;

	} while (track!=kMouseTrackingMouseReleased);
    
    return(TRUE);
}

bool top_view_segment_handle_drag(Point pt)
{
	int				i,rn,x,z,lx,rx,lz,rz,mx,mz,my,
					type,portal_idx,main_idx,sub_idx,index,primitive_uid,minx,maxx,minz,maxz,miny,maxy;
	Rect			box;
	portal_type		*portal;
	segment_type	*seg;

		// only works for single selection
		
	if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&portal_idx,&index,&sub_idx);		// supergumba -- fix this!
	if ((type!=segment_piece) && (type!=primitive_piece)) return(FALSE);
	
		// get segment to move
   
	seg=&map.segments[index];
    rn=seg->rn;
	
	portal=&map.portals[rn];
		
		// is it a primitive
		
	if (type==primitive_piece) {
		primitive_uid=seg->primitive_uid[0];
	
		primitive_get_center(primitive_uid,&mx,&mz,&my);
		primitive_get_extend(primitive_uid,&minx,&maxx,&minz,&maxz,&miny,&maxy);
	
		x=minx+portal->x;
		z=mz+portal->z;
		top_view_map_to_pane(&x,&z);
		SetRect(&box,(x-2),(z-2),(x+3),(z+3));
		if (PtInRect(pt,&box)) return(top_view_primitive_handle_drag(index,0));
	
		x=maxx+portal->x;
		z=mz+portal->z;
		top_view_map_to_pane(&x,&z);
		SetRect(&box,(x-2),(z-2),(x+3),(z+3));
		if (PtInRect(pt,&box)) return(top_view_primitive_handle_drag(index,1));

		x=mx+portal->x;
		z=minz+portal->z;
		top_view_map_to_pane(&x,&z);
		SetRect(&box,(x-2),(z-2),(x+3),(z+3));
		if (PtInRect(pt,&box)) return(top_view_primitive_handle_drag(index,2));

		x=mx+portal->x;
		z=maxz+portal->z;
		top_view_map_to_pane(&x,&z);
		SetRect(&box,(x-2),(z-2),(x+3),(z+3));
		if (PtInRect(pt,&box)) return(top_view_primitive_handle_drag(index,3));
	
		return(FALSE);
	}
	
		// regular segments
	
	switch (seg->type) {
	
		case sg_wall:
			x=seg->data.wall.lx+portal->x;
			z=seg->data.wall.lz+portal->z;
			top_view_map_to_pane(&x,&z);
			SetRect(&box,(x-2),(z-2),(x+3),(z+3));
			if (PtInRect(pt,&box)) return(top_view_single_segment_handle_drag(index,0));

			x=seg->data.wall.rx+portal->x;
			z=seg->data.wall.rz+portal->z;
			top_view_map_to_pane(&x,&z);
			SetRect(&box,(x-2),(z-2),(x+3),(z+3));
			if (PtInRect(pt,&box)) return(top_view_single_segment_handle_drag(index,1));
			break;
			
		case sg_floor:
		case sg_ceiling:
			for ((i=0);(i!=seg->data.fc.ptsz);i++) {
				x=seg->data.fc.x[i]+portal->x;
				z=seg->data.fc.z[i]+portal->z;
				top_view_map_to_pane(&x,&z);
				SetRect(&box,(x-2),(z-2),(x+3),(z+3));
				if (PtInRect(pt,&box)) return(top_view_single_segment_handle_drag(index,i));
			}
			break;
		
		case sg_liquid:
			lx=seg->data.liquid.lft+portal->x;
			lz=seg->data.liquid.top+portal->z;
			top_view_map_to_pane(&lx,&lz);
			rx=seg->data.liquid.rgt+portal->x;
			rz=seg->data.liquid.bot+portal->z;
			top_view_map_to_pane(&rx,&rz);
			
			SetRect(&box,(lx-2),(lz-2),(lx+3),(lz+3));
			if (PtInRect(pt,&box)) return(top_view_single_segment_handle_drag(index,0));

			SetRect(&box,(lx-2),(rz-2),(lx+3),(rz+3));
			if (PtInRect(pt,&box)) return(top_view_single_segment_handle_drag(index,1));
            
			SetRect(&box,(rx-2),(lz-2),(rx+3),(lz+3));
			if (PtInRect(pt,&box)) return(top_view_single_segment_handle_drag(index,2));
            
			SetRect(&box,(rx-2),(rz-2),(rx+3),(rz+3));
			if (PtInRect(pt,&box)) return(top_view_single_segment_handle_drag(index,3));
			break;
			
		case sg_ambient_wall:
			x=seg->data.ambient_wall.lx+portal->x;
			z=seg->data.ambient_wall.lz+portal->z;
			top_view_map_to_pane(&x,&z);
			SetRect(&box,(x-2),(z-2),(x+3),(z+3));
			if (PtInRect(pt,&box)) return(top_view_single_segment_handle_drag(index,0));

			x=seg->data.ambient_wall.rx+portal->x;
			z=seg->data.ambient_wall.rz+portal->z;
			top_view_map_to_pane(&x,&z);
			SetRect(&box,(x-2),(z-2),(x+3),(z+3));
			if (PtInRect(pt,&box)) return(top_view_single_segment_handle_drag(index,1));
			break;
			
		case sg_ambient_fc:
			for ((i=0);(i!=seg->data.ambient_fc.ptsz);i++) {
				x=seg->data.ambient_fc.x[i]+portal->x;
				z=seg->data.ambient_fc.z[i]+portal->z;
				top_view_map_to_pane(&x,&z);
				SetRect(&box,(x-2),(z-2),(x+3),(z+3));
				if (PtInRect(pt,&box)) return(top_view_single_segment_handle_drag(index,i));
			}
			break;
	}

	return(FALSE);
}

/* =======================================================

      Draw Piece
      
======================================================= */

void top_view_piece_drag(Point pt)
{
	int						x,z,ox,oz,xchg,zchg;
	MouseTrackingResult		track;
	
	undo_set_segment_move();
	
	top_view_drag_pane_to_map(pt.h,pt.v,&ox,&oz);
	
	do {
		TrackMouseLocation(NULL,&pt,&track);

		top_view_drag_pane_to_map(pt.h,pt.v,&x,&z);

		if ((x!=ox) || (z!=oz)) {
			
			xchg=ox-x;
			zchg=oz-z;
			
			select_move(cr,xchg,zchg,0);
								
			SetCCursor(dragcur);
			main_wind_draw();
			ox=x;
			oz=z;
		}
        
	} while (track!=kMouseTrackingMouseReleased);
	
	InitCursor();
}

/* =======================================================

      Find Piece For Point
      
======================================================= */

bool top_view_piece_click_rect_pos(d3pos *pos,int sz,int tx,int tz)
{
	int				x,z,k;
	portal_type		*portal;

	portal=&map.portals[pos->rn];
	x=pos->x+portal->x;
	z=pos->z+portal->z;
	
	top_view_map_to_pane(&x,&z);
	
	k=(sz*magnify_factor)/magnify_size;
		
	if (tx<(x-k)) return(FALSE);
	if (tx>(x+k)) return(FALSE);
	if (tz<(z-k)) return(FALSE);
	if (tz>(z+k)) return(FALSE);
	
	return(TRUE);
}

bool top_view_piece_click_polygon(int rn,int ptsz,int *x,int *z,int tx,int tz)
{
	int				n,px[8],pz[8];
	portal_type		*portal;
	
	portal=&map.portals[rn];
		
	for (n=0;n!=ptsz;n++) {
		px[n]=x[n]+portal->x;
		pz[n]=z[n]+portal->z;
		top_view_map_to_pane(&px[n],&pz[n]);
	}
	
	return(polygon_2D_point_inside(ptsz,px,pz,tx,tz));
}

bool top_view_piece_click(Point pt,int *type,int *portal_idx,int *main_idx,int *sub_idx)
{
	int								n,k,t,px[8],pz[8];
	d3pnt							*pt2;
	portal_type						*portal;
	map_mesh_type					*mesh;
	map_mesh_poly_type				*mesh_poly;
	
	*portal_idx=-1;
	*main_idx=-1;
	*sub_idx=-1;
	
		// lights, sounds, particles
		
	if (dp_lightsoundparticle) {
	
		for (n=0;n!=map.nlight;n++) {
			if (top_view_piece_click_rect_pos(&map.lights[n].pos,700,pt.h,pt.v)) {
				*type=light_piece;
				*portal_idx=map.lights[n].pos.rn;
				*main_idx=n;
				return(TRUE);
			}
		}
		
		for (n=0;n!=map.nsound;n++) {
			if (top_view_piece_click_rect_pos(&map.sounds[n].pos,700,pt.h,pt.v)) {
				*type=sound_piece;
				*portal_idx=map.sounds[n].pos.rn;
				*main_idx=n;
				return(TRUE);
			}
		}
		
		for (n=0;n!=map.nparticle;n++) {
			if (top_view_piece_click_rect_pos(&map.particles[n].pos,700,pt.h,pt.v)) {
				*type=particle_piece;
				*portal_idx=map.particles[n].pos.rn;
				*main_idx=n;
				return(TRUE);
			}
		}
	
	}
	
		// nodes
		
	if (dp_node) {
		for (n=0;n!=map.nnode;n++) {
			if (top_view_piece_click_rect_pos(&map.nodes[n].pos,600,pt.h,pt.v)) {
				*type=node_piece;
				*portal_idx=map.nodes[n].pos.rn;
				*main_idx=n;
				return(TRUE);
			}
		}
	}
	
		// spots and scenery
		
	if (dp_object) {
	
	    for (n=0;n!=map.nspot;n++) {
			if (top_view_piece_click_rect_pos(&map.spots[n].pos,800,pt.h,pt.v)) {
				*type=spot_piece;
				*portal_idx=map.spots[n].pos.rn;
				*main_idx=n;
				return(TRUE);
			}
		}

	    for (n=0;n!=map.nscenery;n++) {
			if (top_view_piece_click_rect_pos(&map.sceneries[n].pos,800,pt.h,pt.v)) {
				*type=scenery_piece;
				*portal_idx=map.sceneries[n].pos.rn;
				*main_idx=n;
				return(TRUE);
			}
		}
    }
	
		// find portal clicked in
	
		// meshes
		
	portal=&map.portals[cr];
	
	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			for (t=0;t!=mesh_poly->ptsz;t++) {
				pt2=&mesh->vertexes[mesh_poly->v[t]];
				px[t]=pt2->x;
				pz[t]=pt2->z;
			}
			
			if (top_view_piece_click_polygon(cr,mesh_poly->ptsz,px,pz,pt.h,pt.v)) {
				*type=mesh_piece;
				*portal_idx=cr;
				*main_idx=n;
				*sub_idx=k;
				return(TRUE);
			}
		
			mesh_poly++;
		}

		mesh++;
	}




		

	
	return(FALSE);
		
		// ambients
/*
	if (dp_ambient) {
	
		for (i=0;i!=map.nsegment;i++) {
			seg=&map.segments[i];
			if ((seg->type!=sg_ambient_wall) || (seg->rn!=cr)) continue;
			
			if (top_view_line_click(seg->rn,seg->data.ambient_wall.lx,seg->data.ambient_wall.lz,seg->data.ambient_wall.rx,seg->data.ambient_wall.rz,pt.h,pt.v)) {
				if (seg->primitive_uid[0]!=-1) {
					*type=primitive_piece;
				}
				else {
					*type=segment_piece;
				}
				*index=i;
				return(TRUE);
			}
		}
		
		for (i=(map.nsegment-1);i>=0;i--) {
			seg=&map.segments[i];
			if ((seg->type!=sg_ambient_fc) || (seg->rn!=cr)) continue;
			
			if (top_view_polygon_click(seg->rn,seg->data.ambient_fc.ptsz,seg->data.ambient_fc.x,seg->data.ambient_fc.z,pt.h,pt.v)) {
				if (seg->primitive_uid[0]!=-1) {
					*type=primitive_piece;
				}
				else {
					*type=segment_piece;
				}
				*index=i;
				return(TRUE);
			}
		}
	}
	
		// walls

	if (dp_wall) {
	
		for (i=0;i!=map.nsegment;i++) {
			seg=&map.segments[i];
			if ((seg->type!=sg_wall) || (seg->rn!=cr)) continue;
			
			if (top_view_line_click(seg->rn,seg->data.wall.lx,seg->data.wall.lz,seg->data.wall.rx,seg->data.wall.rz,pt.h,pt.v)) {
				if (seg->primitive_uid[0]!=-1) {
					*type=primitive_piece;
				}
				else {
					*type=segment_piece;
				}
				*index=i;
				return(TRUE);
			}
		}
	}
	
		// liquids

	if (dp_liquid) {
	
		for (i=(map.nsegment-1);i>=0;i--) {
			seg=&map.segments[i];
			if ((seg->type!=sg_liquid) || (seg->rn!=cr)) continue;

			if (top_view_rect_click(seg->rn,seg->data.liquid.lft,seg->data.liquid.rgt,seg->data.liquid.top,seg->data.liquid.bot,pt.h,pt.v)) {
				if (seg->primitive_uid[0]!=-1) {
					*type=primitive_piece;
				}
				else {
					*type=segment_piece;
				}
				*index=i;
				return(TRUE);
			}
		}
	}
    
		// ceilings
		
	if (dp_ceiling) {
	
		for (i=(map.nsegment-1);i>=0;i--) {
			seg=&map.segments[i];
			if ((seg->type!=sg_ceiling) || (seg->rn!=cr)) continue;
			if (top_view_hide_y(&seg->data.fc)) continue;
			
			if (top_view_polygon_click(seg->rn,seg->data.fc.ptsz,seg->data.fc.x,seg->data.fc.z,pt.h,pt.v)) {
				if (seg->primitive_uid[0]!=-1) {
					*type=primitive_piece;
				}
				else {
					*type=segment_piece;
				}
				*index=i;
				return(TRUE);
			}
		}
	}	
	
		// floors
		
	if (dp_floor) {
	
		for (i=(map.nsegment-1);i>=0;i--) {
			seg=&map.segments[i];
			if ((seg->type!=sg_floor) || (seg->rn!=cr)) continue;
			if (top_view_hide_y(&seg->data.fc)) continue;
			
			if (top_view_polygon_click(seg->rn,seg->data.fc.ptsz,seg->data.fc.x,seg->data.fc.z,pt.h,pt.v)) {
				if (seg->primitive_uid[0]!=-1) {
					*type=primitive_piece;
				}
				else {
					*type=segment_piece;
				}
				*index=i;
				return(TRUE);
			}
		}
	}
*/
		// no hits
		
	return(FALSE);
}

/* =======================================================

      Double-Click Piece
      
======================================================= */

void top_view_piece_double_click(int type,int index)
{
	switch (type) {
	
		case segment_piece:
		case primitive_piece:
			dialog_segment_setting_run(index);
			break;

		case spot_piece:
            dialog_spot_setting_run(&map.spots[index]);
			break;
			
		case scenery_piece:
            dialog_scenery_setting_run(&map.sceneries[index]);
			break;
			
		case node_piece:
            dialog_node_settings_run(&map.nodes[index]);
			break;
			
		case light_piece:
			dialog_map_light_settings_run(&map.lights[index]);
			break;
			
		case sound_piece:
			dialog_map_sound_settings_run(&map.sounds[index]);
			break;
			
		case particle_piece:
			dialog_map_particle_settings_run(&map.particles[index]);
			break;
	}
}

