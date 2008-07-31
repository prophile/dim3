/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Static Camera

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "cameras.h"
#include "scripts.h"

extern server_type		server;
extern map_type			map;
extern camera_type		camera;
extern js_type			js;

d3pnt					camera_static_pnt;
d3ang					camera_static_walk_ang;

/* =======================================================

      Setup Chase Camera Structure
      
======================================================= */

void camera_static_connect(void)
{
	camera.auto_walk.on=FALSE;
}

/* =======================================================

      Update Static Camera
      
======================================================= */

void camera_static_update(int x,int z,int y)
{
    camera_static_pnt.x=x;
    camera_static_pnt.z=z;
    camera_static_pnt.y=y;
}

/* =======================================================

      Chase Camera Position
      
======================================================= */

void camera_static_get_position(d3pnt *pnt,d3ang *ang)
{
	obj_type		*obj;
    
		// get position

	memmove(pnt,&camera_static_pnt,sizeof(d3pnt));

	if (ang==NULL) return;
	
		// if following an object, point camera at object

    if (camera.static_follow) {
		obj=object_find_uid(camera.obj_uid);
        ang->y=angle_find(camera_static_pnt.x,camera_static_pnt.z,obj->pnt.x,obj->pnt.z);
		ang->x=-(180.0f-angle_find(camera_static_pnt.y,camera_static_pnt.z,obj->pnt.y,obj->pnt.z));
 		ang->z=camera.ang.z;
		return;
	}
 
		// if in walk and not following
		// then point way specified in node

	if (camera.auto_walk.on) {
		ang->x=angle_add(camera_static_walk_ang.x,camera.ang.x);
		ang->y=angle_add(camera_static_walk_ang.y,camera.ang.y);
		ang->z=angle_add(camera_static_walk_ang.z,camera.ang.z);
		return;
	}

		// else just use camera offset

	memmove(ang,&camera.ang,sizeof(d3ang));
}

/* =======================================================

      Setup Camera Auto Moves
      
======================================================= */

bool camera_walk_to_node_setup(char *start_node,char *end_node,int msec,int event_id,bool open_doors,bool in_freeze)
{
	int			from_idx,to_idx,dist;
	float		speed;
	obj_type	*player_obj;

		// only works in static camera

	if (camera.mode!=cv_static) {
		JS_ReportError(js.cx,"Can only walk cameras in static mode");
		return(FALSE);
	}
	
		// get the nodes
		
	from_idx=map_find_node(&map,start_node);
	if (from_idx==-1) {
		JS_ReportError(js.cx,"Named node does not exist: %s",start_node);
		return(FALSE);
	}
	
	to_idx=map_find_node(&map,end_node);
	if (to_idx==-1) {
		JS_ReportError(js.cx,"Named node does not exist: %s",end_node);
		return(FALSE);
	}
	
		// is end node in start node path?
		
	if (map_find_next_node_in_path(&map,from_idx,to_idx)==-1) {
		JS_ReportError(js.cx,"End node '%s' is not in the same path as the start node '%s'",end_node,start_node);
		return(FALSE);
	}

		// get total distance

	dist=map_node_to_node_distance(&map,from_idx,to_idx);
	if (dist==0) {
		JS_ReportError(js.cx,"Camera walk covers no distance");
		return(FALSE);
	}

	speed=(float)dist/(float)(msec/10);
	
		// start at the start node and seek to the next node
		
	memmove(&camera_static_pnt,&map.nodes[from_idx].pnt,sizeof(d3pnt));
	memmove(&camera_static_walk_ang,&map.nodes[from_idx].ang,sizeof(d3ang));

	from_idx=map_find_next_node_in_path(&map,from_idx,to_idx);
	if (from_idx==-1) return(TRUE);
	
		// setup seeking
		
	camera.auto_walk.on=TRUE;
	camera.auto_walk.open_doors=open_doors;
	camera.auto_walk.in_freeze=in_freeze;
	camera.auto_walk.node_seek_idx=from_idx;
	camera.auto_walk.node_dest_idx=to_idx;
	camera.auto_walk.msec=msec;
	camera.auto_walk.speed=speed;
	camera.auto_walk.event_id=event_id;
	
		// player freeze
		
	if (in_freeze) {
		player_obj=object_find_uid(server.player_obj_uid);
		object_input_freeze(player_obj,TRUE);
	}

	return(TRUE);
}

/* =======================================================

      Run Static Camera
      
======================================================= */

void camera_static_run(void)
{
	int			dist,seek_idx,dest_idx;
	float		sx,sy,sz,tot;
	node_type	*node;
	obj_type	*player_obj;

		// auto-walk on?

	if (!camera.auto_walk.on) return;
	
		// get nodes
		
	seek_idx=camera.auto_walk.node_seek_idx;
	dest_idx=camera.auto_walk.node_dest_idx;
		
	node=&map.nodes[seek_idx];

		// move towards node
		// get percentage of each different and use that
		// to adjust the speed

	sx=(float)abs(node->pnt.x-camera_static_pnt.x);
	sy=(float)abs(node->pnt.y-camera_static_pnt.y);
	sz=(float)abs(node->pnt.z-camera_static_pnt.z);
	tot=sx+sy+sz;

	sx=sx/tot;
	sy=sy/tot;
	sz=sz/tot;

	sx*=camera.auto_walk.speed;
	if (node->pnt.x<camera_static_pnt.x) sx=-sx;
	
	sy*=camera.auto_walk.speed;
	if (node->pnt.y<camera_static_pnt.y) sy=-sy;
	
	sz*=camera.auto_walk.speed;
	if (node->pnt.z<camera_static_pnt.z) sz=-sz;

	camera_static_pnt.x+=(int)sx;
	camera_static_pnt.y+=(int)sy;
	camera_static_pnt.z+=(int)sz;

		// distance to seek node

	dist=distance_get(node->pnt.x,node->pnt.y,node->pnt.z,camera_static_pnt.x,camera_static_pnt.y,camera_static_pnt.z);

		// get the look angle if not following

    if (!camera.static_follow) {
		camera_static_walk_ang.x=angle_turn_toward(camera_static_walk_ang.x,node->ang.x,camera.auto_walk.turn_speed);
		camera_static_walk_ang.y=angle_turn_toward(camera_static_walk_ang.y,node->ang.y,camera.auto_walk.turn_speed);
		camera_static_walk_ang.z=angle_turn_toward(camera_static_walk_ang.z,node->ang.z,camera.auto_walk.turn_speed);
	}
	
		// near current seek node?
		
	if (dist>camera.auto_walk.node_slop) return;	
	
		// move on to next node
		
	if (seek_idx!=dest_idx) {
		camera.auto_walk.node_seek_idx=map_find_next_node_in_path(&map,seek_idx,dest_idx);
		scripts_post_event_console(&js.course_attach,sd_event_path,sd_event_path_node,(int)node->user_value);
		return;
	}
	
		// at last node
		
	camera.auto_walk.on=FALSE;
	memmove(&camera_static_walk_ang,&node->ang,sizeof(d3ang));

		// player freeze
		
	if (camera.auto_walk.in_freeze) {
		player_obj=object_find_uid(server.player_obj_uid);
		object_input_freeze(player_obj,FALSE);
	}
	
		// send event
	
	scripts_post_event_console(&js.course_attach,sd_event_path,sd_event_path_done,camera.auto_walk.event_id);
}
