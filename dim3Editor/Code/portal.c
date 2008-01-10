/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Portal Routines

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
#include "portal_view.h"
#include "top_view.h"
#include "walk_view.h"

int							vertex_mode;
bool						dp_primitive,dp_auto_texture,dp_wall,dp_floor,dp_ceiling,dp_ambient,dp_liquid,
							dp_object,dp_node,dp_lightsoundparticle,dp_textured,dp_y_hide;

extern int					cr,cx,cz,cy,portal_view_x,portal_view_z;

extern setup_type			setup;
extern map_type				map;

/* =======================================================

      Check if Spot is Clear
      
======================================================= */

bool portal_is_spot_ok(int rn,int x,int z,int ex,int ez)
{
	int				n;
	portal_type		*portal;
	
	for (n=0;n!=map.nportal;n++) {
		if (rn==n) continue;
		
		portal=&map.portals[n];
		if (x>=portal->ex) continue;
		if (ex<=portal->x) continue;
		if (z>=portal->ez) continue;
		if (ez<=portal->z) continue;
		
		return(FALSE);
	}

	return(TRUE);
}


/* =======================================================

      Which Direction to Duplicate Piece in Portal
      
======================================================= */

void portal_duplicate_piece_offset(int rn,int x,int z,int *xadd,int *zadd)
{
	int				mx,mz;
	portal_type		*portal;
	
	portal=&map.portals[rn];
	mx=(portal->ex-portal->x)/2;
	mz=(portal->ez-portal->z)/2;
	
	*xadd=setup.duplicate_offset;
	if (x>mx) *xadd=-setup.duplicate_offset;
	
	*zadd=setup.duplicate_offset;
	if (z>mz) *zadd=-setup.duplicate_offset;
}

/* =======================================================

      Start a New Portal
      
======================================================= */

bool portal_new(void)
{
    int				i,x,z,y,xadd,zadd,x_size,z_size,div,high;
    bool			floor_on,ceiling_on,wall_on,spot_ok;
    portal_type		*portal;
	
	if (!segment_create_texture_ok()) return(FALSE);
    
	if (map.nportal==max_portal) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Portal","\pYou've reached the maximum number of portals for a map.",NULL,NULL);
		return(FALSE);
	}
    
	if (!dialog_new_portal_run(&x_size,&z_size,&floor_on,&ceiling_on,&wall_on,&div,&high)) return(FALSE);
    
        // make sure portal divides evenly
        
    if ((floor_on) || (ceiling_on) || (wall_on)) {
        i=x_size/div;
        x_size=div*i;
        i=z_size/div;
        z_size=div*i;
    }
	
		// move portal till it's at a clear spot
		
	x=cx-(x_size/2);
	z=cz-(z_size/2);
	spot_ok=FALSE;
	
	for (i=0;i!=250;i++) {
		spot_ok=portal_is_spot_ok(-1,x,z,(x+x_size),(z+z_size));
		if (spot_ok) break;
		x++;
		z++;
	}
	
	if (!spot_ok) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Portal","\pThere isn't enough free space around the cursor, move to another place on the map and try again.",NULL,NULL);
		return(FALSE);
	}
    
        // create portal
		
	cr=map_portal_create(&map,x,z,(x+x_size),(z+z_size));
    portal=&map.portals[cr];
    
    xadd=x_size/div;
    zadd=z_size/div;
    
        // add segments
    
    y=cy+(high/2);
    if (high>y) y=high;
        
    for ((x=0);(x!=div);x++) {
        for ((z=0);(z!=div);z++) {
            if (floor_on) segment_add_floor_box((x*xadd),(z*zadd),((x*xadd)+xadd),((z*zadd)+zadd),y,-1,FALSE);
            if (ceiling_on) segment_add_ceiling_box((x*xadd),(z*zadd),((x*xadd)+xadd),((z*zadd)+zadd),(y-high),-1,FALSE);
        }
    }
    
    if (wall_on) {
        for ((x=0);(x!=div);x++) {
            segment_add_wall((x*xadd),0,((x+1)*xadd),0,(y-high),(y-1),wc_none,cv_none,-1,FALSE);
            segment_add_wall((x*xadd),(zadd*div),((x+1)*xadd),(zadd*div),(y-high),(y-1),wc_none,cv_none,-1,FALSE);
        }
        for ((z=0);(z!=div);z++) {
            segment_add_wall(0,(z*zadd),0,((z+1)*zadd),(y-high),(y-1),wc_none,cv_none,-1,FALSE);
            segment_add_wall((xadd*div),(z*zadd),(xadd*div),((z+1)*zadd),(y-high),(y-1),wc_none,cv_none,-1,FALSE);
        }
    }
    
    portal_reset_texture_uvs();

	main_wind_draw();
	menu_fix_enable();
	main_wind_tool_fix_enable();
	
	return(TRUE);
}

/* =======================================================

      Delete Portal
      
======================================================= */

void portal_delete(void)
{
	int				n;

		// can't delete if last portal
		
	if (map.nportal==1) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Delete portal","\pYou must have at least one portal in the map.",NULL,NULL);
		return;
	}
	
		// select and delete pieces
		
	select_clear();
	
	for (n=0;n!=map.nnode;n++) {
		if (map.nodes[n].pos.rn==cr) select_add(node_piece,n);
	}
	
	for (n=0;n!=map.nsound;n++) {
		if (map.sounds[n].pos.rn==cr) select_add(sound_piece,n);
	}

	for (n=0;n!=map.nlight;n++) {
		if (map.lights[n].pos.rn==cr) select_add(light_piece,n);
	}

	for (n=0;n!=map.nspot;n++) {
		if (map.spots[n].pos.rn==cr) select_add(spot_piece,n);
	}
	
	for (n=0;n!=map.nscenery;n++) {
		if (map.sceneries[n].pos.rn==cr) select_add(scenery_piece,n);
	}
	
	piece_delete();
	
		// delete
		
	map_portal_delete(&map,cr);
	
	cr=0;
    
	main_wind_center_position_in_map();
	top_view_reset();
	walk_view_portal_view_reset();
	main_wind_draw();
}

/* =======================================================

      Duplicate Portal
      
======================================================= */

void portal_duplicate(void)
{
	int				x,ex,z,ez;
	
	InitCursor();
	
	if (map.nportal==max_portal) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create portal","\pYou've reached the maximum number of portals for a map.",NULL,NULL);
		return;
	}
	
		// position new portal
		
	x=map.portals[cr].ex;
	ex=x+(map.portals[cr].ex-map.portals[cr].x);
	z=map.portals[cr].ez;
	ez=z+(map.portals[cr].ez-map.portals[cr].z);
	
	while (TRUE) {
		if (portal_is_spot_ok(-1,x,z,ex,ez)) break;
		x++;
		ex++;
		z++;
		ez++;
	}

		// duplicate portal
		
	cr=map_portal_duplicate(&map,cr,x,z);
			
	main_wind_draw();
}
	
/* =======================================================

      Move Portal Segments
      
======================================================= */

void portal_move_all_items(int rn,int x,int y,int z)
{
	int				n;
	bool			auto_txt_save;
	
	auto_txt_save=dp_auto_texture;
	dp_auto_texture=FALSE;
	
	for (n=0;n!=map.nsegment;n++) {
		if (map.segments[n].rn==rn) segment_move(&map.segments[n],x,z,y);
	}
	
	dp_auto_texture=auto_txt_save;
		
    for (n=0;n!=map.nspot;n++) {
        if (map.spots[n].pos.rn==rn) {
			map.spots[n].pos.x-=x;
			map.spots[n].pos.y-=y;
			map.spots[n].pos.z-=z;
		}
    }
    
    for (n=0;n!=map.nnode;n++) {
        if (map.nodes[n].pos.rn==rn)  {
			map.nodes[n].pos.x-=x;
			map.nodes[n].pos.y-=y;
			map.nodes[n].pos.z-=z;
		}
    }
	
    for (n=0;n!=map.nsound;n++) {
        if (map.sounds[n].pos.rn==rn)  {
			map.sounds[n].pos.x-=x;
			map.sounds[n].pos.y-=y;
			map.sounds[n].pos.z-=z;
		}
    }
	
    for (n=0;n!=map.nlight;n++) {
        if (map.lights[n].pos.rn==rn)  {
			map.lights[n].pos.x-=x;
			map.lights[n].pos.y-=y;
			map.lights[n].pos.z-=z;
		}
    }
	
    for (n=0;n!=map.nscenery;n++) {
        if (map.sceneries[n].pos.rn==rn)  {
			map.sceneries[n].pos.x-=x;
			map.sceneries[n].pos.y-=y;
			map.sceneries[n].pos.z-=z;
		}
    }
}
