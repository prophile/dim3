/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Piece Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
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
#include "top_view.h"
#include "portal_view.h"

extern CCrsrHandle			dragcur;

extern unsigned short		effect,effectmask;
extern int					cr;
extern bool					dp_primitive,primitive_reform_ok;

extern map_type				map;

/* =======================================================

      Get Piece Extent
      
======================================================= */

void piece_get_extent(int type,int index,int *min_x,int *min_z,int *max_x,int *max_z,int *min_y,int *max_y)
{
	switch (type) {
	
		case segment_piece:
			map_segment_calculate_extent(&map,index,min_x,min_z,min_y,max_x,max_z,max_y);
			break;
			
		case primitive_piece:
			primitive_get_extend(map.segments[index].primitive_uid[0],min_x,max_x,min_z,max_z,min_y,max_y);
			break;
			
		case node_piece:
			*min_x=*max_x=map.nodes[index].pos.x;
			*min_z=*max_z=map.nodes[index].pos.z;
			*min_y=*max_y=map.nodes[index].pos.y;
			break;
			
		case spot_piece:
			*min_x=*max_x=map.spots[index].pos.x;
			*min_z=*max_z=map.spots[index].pos.z;
			*min_y=*max_y=map.spots[index].pos.y;
			break;
			
		case scenery_piece:
			*min_x=*max_x=map.sceneries[index].pos.x;
			*min_z=*max_z=map.sceneries[index].pos.z;
			*min_y=*max_y=map.sceneries[index].pos.y;
			break;
			
		case light_piece:
			*min_x=*max_x=map.lights[index].pos.x;
			*min_z=*max_z=map.lights[index].pos.z;
			*min_y=*max_y=map.lights[index].pos.y;
			break;
			
		case sound_piece:
			*min_x=*max_x=map.sounds[index].pos.x;
			*min_z=*max_z=map.sounds[index].pos.z;
			*min_y=*max_y=map.sounds[index].pos.y;
			break;
			
		case particle_piece:
			*min_x=*max_x=map.particles[index].pos.x;
			*min_z=*max_z=map.particles[index].pos.z;
			*min_y=*max_y=map.particles[index].pos.y;
			break;
			
	}
}

/* =======================================================

      Move Pieces
      
======================================================= */

void piece_move_to_portal(int rn)
{
	int			n,nsel_count,
				type,index,min_x,min_z,min_y,max_x,max_z,max_y;
			
	undo_clear();
	
	select_get_extent(&min_x,&min_z,&min_y,&max_x,&max_z,&max_y);
	
	nsel_count=select_count();
	
	for (n=0;n!=nsel_count;n++) {
		select_get(n,&type,&index);
		
		switch (type) {
		
			case segment_piece:
				map.segments[index].rn=rn;
				break;
				
			case primitive_piece:
				primitive_change_portal(map.segments[index].primitive_uid[0],rn);
				break;
				
			case spot_piece:
				map.spots[index].pos.rn=rn;
				break;
				
			case scenery_piece:
				map.sceneries[index].pos.rn=rn;
				break;
				
			case node_piece:
				map.nodes[index].pos.rn=rn;
				break;
				
			case light_piece:
				map.lights[index].pos.rn=rn;
				break;
				
			case sound_piece:
				map.sounds[index].pos.rn=rn;
				break;
				
			case particle_piece:
				map.particles[index].pos.rn=rn;
				break;
				
		}
    }
	
		// shift selection to top-left
		
	cr=rn;
	select_move(rn,min_x,min_z,0);
	
	main_wind_draw();
}

/* =======================================================

      Duplicate and Delete Piece
      
======================================================= */

void piece_duplicate(void)
{
	int				n,i,nsel_count,
					type,index,xadd,zadd,dup_index,
					cx,cy,cz;

	undo_clear();
	
		// duplicate selection
		
	select_duplicate_clear();
	
		// duplicate pieces
		
	nsel_count=select_count();
	
	for (n=0;n!=nsel_count;n++) {
		select_get(n,&type,&index);
		
		switch (type) {
		
			case segment_piece:
				map_segment_calculate_center(&map,index,&cx,&cy,&cz);
				portal_duplicate_piece_offset(map.segments[index].rn,cx,cz,&xadd,&zadd);
	
				dup_index=segment_duplicate(index,-1,xadd,zadd);
				if (dup_index!=-1) select_duplicate_add(segment_piece,dup_index);
				break;
				
			case primitive_piece:
				map_segment_calculate_center(&map,index,&cx,&cy,&cz);
				portal_duplicate_piece_offset(map.segments[index].rn,cx,cz,&xadd,&zadd);
				
				dup_index=primitive_duplicate(map.segments[index].primitive_uid[0],xadd,zadd);
				if (dup_index!=-1) select_duplicate_add(primitive_piece,dup_index);
				break;
				
			case spot_piece:
				if (map.nspot==max_spot) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Spot","\pYou've reached the maximum number of spots for a map.",NULL,NULL);
					return;
				}
				
				portal_duplicate_piece_offset(map.spots[index].pos.rn,map.spots[index].pos.x,map.spots[index].pos.z,&xadd,&zadd);

				map.spots[map.nspot]=map.spots[index];
				map.spots[map.nspot].pos.x+=xadd;
				map.spots[map.nspot].pos.z+=zadd;
				select_duplicate_add(spot_piece,map.nspot);
				map.nspot++;
				break;
				
			case scenery_piece:
				if (map.nscenery==max_map_scenery) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Scenery","\pYou've reached the maximum number of scenery for a map.",NULL,NULL);
					return;
				}
				
				portal_duplicate_piece_offset(map.sceneries[index].pos.rn,map.sceneries[index].pos.x,map.sceneries[index].pos.z,&xadd,&zadd);

				map.sceneries[map.nscenery]=map.sceneries[index];
				map.sceneries[map.nscenery].pos.x+=xadd;
				map.sceneries[map.nscenery].pos.z+=zadd;
				select_duplicate_add(scenery_piece,map.nscenery);
				map.nscenery++;
				break;
				
			case node_piece:
				if (map.nnode==max_node) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Node","\pYou've reached the maximum number of nodes for a map.",NULL,NULL);
					return;
				}

				portal_duplicate_piece_offset(map.nodes[index].pos.rn,map.nodes[index].pos.x,map.nodes[index].pos.z,&xadd,&zadd);

				map.nodes[map.nnode]=map.nodes[index];
				map.nodes[map.nnode].pos.x+=xadd;
				map.nodes[map.nnode].pos.z+=zadd;
				for (i=0;i!=max_node_link;i++) {
					map.nodes[map.nnode].link[i]=-1;
				}
				select_duplicate_add(node_piece,map.nnode);
				map.nnode++;
				break;
				
			case light_piece:
				if (map.nlight==max_map_light) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Light","\pYou've reached the maximum number of lights for a map.",NULL,NULL);
					return;
				}

				portal_duplicate_piece_offset(map.lights[index].pos.rn,map.lights[index].pos.x,map.lights[index].pos.z,&xadd,&zadd);

				map.lights[map.nlight]=map.lights[index];
				map.lights[map.nlight].pos.x+=xadd;
				map.lights[map.nlight].pos.z+=zadd;
				select_duplicate_add(light_piece,map.nlight);
				map.nlight++;
				break;
				
			case sound_piece:
				if (map.nsound==max_map_sound) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Sound","\pYou've reached the maximum number of sounds for a map.",NULL,NULL);
					return;
				}

				portal_duplicate_piece_offset(map.sounds[index].pos.rn,map.sounds[index].pos.x,map.sounds[index].pos.z,&xadd,&zadd);

				map.sounds[map.nsound]=map.sounds[index];
				map.sounds[map.nsound].pos.x+=xadd;
				map.sounds[map.nsound].pos.z+=zadd;
				select_duplicate_add(sound_piece,map.nsound);
				map.nsound++;
				break;
				
			case particle_piece:
				if (map.nparticle==max_map_particle) {
					StandardAlert(kAlertCautionAlert,"\pCan Not Create Particle","\pYou've reached the maximum number of particles for a map.",NULL,NULL);
					return;
				}

				portal_duplicate_piece_offset(map.particles[index].pos.rn,map.particles[index].pos.x,map.particles[index].pos.z,&xadd,&zadd);

				map.particles[map.nparticle]=map.particles[index];
				map.particles[map.nparticle].pos.x+=xadd;
				map.particles[map.nparticle].pos.z+=zadd;
				select_duplicate_add(particle_piece,map.nparticle);
				map.nparticle++;
				break;
				
		}
    }
	
		// switch selections
		
	select_duplicate_copy();
	
	main_wind_draw();
}

void piece_delete(void)
{
	int				n,i,k,nsel_count,
					type,index;
	
	undo_clear();
	
		// sort segment so higher indexes are deleted first
		
	select_sort();
	
		// delete selection
	
	nsel_count=select_count();
	
	for (n=0;n!=nsel_count;n++) {
		select_get(n,&type,&index);
	
		switch (type) {
			
			case segment_piece:
				segment_delete(index);
				break;
				
			case primitive_piece:
				if (!dp_primitive) {
					primitive_reform_reset_for_delete(index);
					segment_delete(index);
				}
				else {
					primitive_delete(map.segments[index].primitive_uid[0]);
					primitive_reform_ok=FALSE;
				}
				break;
				
			case spot_piece:
				for (i=index;i<map.nspot;i++) {
					map.spots[i]=map.spots[i+1];
				}
				map.nspot--;
				break;
				
			case scenery_piece:
				for (i=index;i<map.nscenery;i++) {
					map.sceneries[i]=map.sceneries[i+1];
				}
				map.nscenery--;
				break;
				
			case node_piece:
				for (i=0;i!=map.nnode;i++) {			// clear all linkage
					if (i==index) continue;

					for (k=0;k!=max_node_link;k++) {
						if (map.nodes[i].link[k]==index) map.nodes[i].link[k]=-1;
						if (map.nodes[i].link[k]>index) map.nodes[i].link[k]--;
					}
				}

				for (i=index;i<map.nnode;i++) {
					map.nodes[i]=map.nodes[i+1];
				}
				
				map.nnode--;
				break;
				
			case light_piece:
				for (i=index;i<map.nlight;i++) {
					map.lights[i]=map.lights[i+1];
				}
				map.nlight--;
				break;
				
			case sound_piece:
				for (i=index;i<map.nsound;i++) {
					map.sounds[i]=map.sounds[i+1];
				}
				map.nsound--;
				break;
				
			case particle_piece:
				for (i=index;i<map.nparticle;i++) {
					map.particles[i]=map.particles[i+1];
				}
				map.nparticle--;
				break;
				
		}
	}
	
	select_clear();
    
	main_wind_draw();
}

/* =======================================================

      Piece Transforms
      
======================================================= */

void piece_flip_horizontal(void)
{
	int					n,k,sel_count,type,index,
						primitive_uid,min_x,max_x,min_z,max_z,min_y,max_y;
	segment_type		*seg;
	
	select_get_extent(&min_x,&min_z,&min_y,&max_x,&max_z,&max_y);
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);
		
		switch (type) {
		
			case segment_piece:
				segment_flip_horizontal(&map.segments[index],min_x,max_x);
				break;
				
			case primitive_piece:
				primitive_uid=map.segments[index].primitive_uid[0];
				
				seg=map.segments;
					
				for (k=0;k!=map.nsegment;k++) {
					if (seg->primitive_uid[0]==primitive_uid) segment_flip_horizontal(seg,min_x,max_x);
					seg++;
				}
				break;
		}
	}
	
	main_wind_draw();
}

void piece_flip_vertical(void)
{
	int					n,k,sel_count,type,index,
						primitive_uid,min_x,max_x,min_z,max_z,min_y,max_y;
	segment_type		*seg;
	
	select_get_extent(&min_x,&min_z,&min_y,&max_x,&max_z,&max_y);
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);
		
		switch (type) {
		
			case segment_piece:
				segment_flip_vertical(&map.segments[index],min_z,max_z);
				break;
				
			case primitive_piece:
				primitive_uid=map.segments[index].primitive_uid[0];

				seg=map.segments;
					
				for (k=0;k!=map.nsegment;k++) {
					if (seg->primitive_uid[0]==primitive_uid) segment_flip_vertical(seg,min_z,max_z);
					seg++;
				}
				break;
		}
	}
	
	main_wind_draw();
}

void piece_rotate(void)
{
	int					n,k,sel_count,type,index,cx,cz,
						primitive_uid,min_x,max_x,min_z,max_z,min_y,max_y;
	segment_type		*seg;

	select_get_extent(&min_x,&min_z,&min_y,&max_x,&max_z,&max_y);
	cx=(min_x+max_x)/2;
	cz=(min_z+max_z)/2;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);
		
		switch (type) {
		
			case segment_piece:
				segment_rotate(&map.segments[index],cx,cz);
				break;
				
			case primitive_piece:
				primitive_uid=map.segments[index].primitive_uid[0];

				seg=map.segments;
					
				for (k=0;k!=map.nsegment;k++) {
					if (seg->primitive_uid[0]==primitive_uid) segment_rotate(seg,cx,cz);
					seg++;
				}
				break;
		}
	}
	
	main_wind_draw();
}

void piece_free_rotate(float ang)
{
	int					n,k,sel_count,type,index,primitive_uid,
						cx,cz,min_x,max_x,min_z,max_z,min_y,max_y;;
	segment_type		*seg;
	
	if (ang==0) return;
	
	select_get_extent(&min_x,&min_z,&min_y,&max_x,&max_z,&max_y);
	cx=(min_x+max_x)/2;
	cz=(min_z+max_z)/2;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);
		
		switch (type) {
		
			case segment_piece:
				segment_rotate_free(&map.segments[index],ang,cx,cz);
				break;
				
			case primitive_piece:
				primitive_uid=map.segments[index].primitive_uid[0];

				seg=map.segments;
					
				for (k=0;k!=map.nsegment;k++) {
					if (seg->primitive_uid[0]==primitive_uid) segment_rotate_free(seg,ang,cx,cz);
					seg++;
				}
				break;
		}
	}
	
	main_wind_draw();
}

/* =======================================================

      Piece Keys
      
======================================================= */

void piece_key(int rn,char ch,bool walk)
{
	int				mv;
	
	mv=main_wind_shift_down()?10:1;
	
	switch (ch) {
		case 0x1C:
			select_move(rn,mv,0,0);
			main_wind_draw();
			break;
		case 0x1D:
			select_move(rn,-mv,0,0);
			main_wind_draw();
			break;
		case 0x1E:
			if (walk) {
				if (main_wind_control_down()) {
					select_move(rn,0,mv,0);
				}
				else {
					select_move(rn,0,0,mv);
				}
			}
			else {
				select_move(rn,0,mv,0);
			}
			main_wind_draw();
			break;
		case 0x1F:
			if (walk) {
				if (main_wind_control_down()) {
					select_move(rn,0,-mv,0);
				}
				else {
					select_move(rn,0,0,-mv);
				}
			}
			else {
				select_move(rn,0,-mv,0);
			}
			main_wind_draw();
			break;
		case 8:
			piece_delete();
			main_wind_draw();
			menu_fix_enable();
			main_wind_tool_fix_enable();
			break;
	}
}

/* =======================================================

      Piece Switchs
      
======================================================= */

void piece_switch_spot_scenery(void)
{
	int					i,type,index;
	spot_type			*spot;
	map_scenery_type	*scenery;
	
	select_get(0,&type,&index);
		
		// spot into scenery
		
	if (type==spot_piece) {
		if (map.nscenery==max_map_scenery) return;

		scenery=&map.sceneries[map.nscenery];
		map.nscenery++;
		
		spot=&map.spots[index];
		
		memmove(&scenery->pos,&spot->pos,sizeof(d3pos));
		strcpy(scenery->model_name,spot->display_model);
		memmove(&scenery->ang,&spot->ang,sizeof(d3ang));
		scenery->animation_name[0]=0x0;

		for (i=index;i<map.nspot;i++) {
			map.spots[i]=map.spots[i+1];
		}
		map.nspot--;
	}
	
	if (type==scenery_piece) {
		if (map.nspot==max_spot) return;

		spot=&map.spots[map.nspot];
		map.nspot++;
		
		scenery=&map.sceneries[index];
		
		memmove(&spot->pos,&scenery->pos,sizeof(d3pos));
		strcpy(spot->name,scenery->model_name);
		strcpy(spot->script,scenery->model_name);
		strcpy(spot->display_model,scenery->model_name);
		memmove(&spot->ang,&scenery->ang,sizeof(d3ang));
		strcpy(spot->type,"Object");
		spot->params[0]=0x0;

		for (i=index;i<map.nscenery;i++) {
			map.sceneries[i]=map.sceneries[i+1];
		}
		map.nscenery--;
	}
	
	select_clear();
	
	main_wind_draw();
}

