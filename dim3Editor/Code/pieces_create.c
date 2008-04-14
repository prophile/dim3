/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Piece Creation

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

extern int						cr,cy,drag_mode;
extern bool						dp_object,dp_node,dp_lightsoundparticle,dp_liquid;

extern file_path_setup_type		file_path_setup;
extern map_type					map;

/* =======================================================

      Check if Piece Creation OK
      
======================================================= */

bool piece_create_texture_ok(void)
{
	if (map.textures[0].bitmaps[0].data!=NULL) return(TRUE);

	StandardAlert(kAlertCautionAlert,"\pYou need at least one texture","\pYou need a default texture in the first spot to add pieces with.  Please add a texture (by double clicking the first open texture) before adding pieces to the map.",NULL,NULL);
	return(FALSE);
}

/* =======================================================

      Find Create Spot
      
======================================================= */

int piece_create_get_spot(int *x,int *y,int *z,int x_wid,int z_wid,int high)
{
	d3pnt			min,max;
	portal_type		*portal;
	
		// does this portal have any meshes?
		
	portal=&map.portals[cr];
	
	if (portal->mesh.nmesh==0) {
		*x=((portal->x+portal->ex)/2)-portal->x;
		*z=((portal->z+portal->ez)/2)-portal->z;
		*y=cy;
	}
	else {
	
			// find place in selection
			
		if (select_count()!=0) {
			select_get_extent(&min,&max);
			*x=(min.x+max.x)>>1;
			*z=(min.z+max.z)>>1;
			*y=((min.y+max.y)>>1)-1;
		}
		
			// no selection, find portal center

		else {
			map_portal_calculate_center(&map,cr,x,y,z);
		}
	}
	
		// size adjustments
		
	if ((*x)<x_wid) *x=x_wid;
	if ((*z)<z_wid) *z=z_wid;
	if ((*y)<high) *y=high;
	
	return(cr);
}

/* =======================================================

      Create Spot
      
======================================================= */

void piece_create_spot(void)
{
	int				rn,x,z,y,index;
	spot_type		*spot;
	
	if (map.nspot==max_spot) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Spot","\pYou've reached the maximum number of spots for this map.",NULL,NULL);
	}
			
		// create spot
		
	index=map.nspot;
	
	spot=&map.spots[index];
 	map.nspot++;
	
	rn=piece_create_get_spot(&x,&y,&z,0,0,0);
    
	spot->pos.rn=rn;
	spot->pos.x=x;
	spot->pos.z=z;
	spot->pos.y=y;
	spot->ang.y=0;
	spot->name[0]=0x0;
	strcpy(spot->type,"Object");
	spot->script[0]=0x0;
	spot->display_model[0]=0x0;
	spot->params[0]=0x0;
    
		// select spot
		
    dp_object=TRUE;
	
	select_clear();
	select_add(spot_piece,rn,index,-1);
	
	main_wind_draw();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
    
	dialog_spot_setting_run(spot);
}

/* =======================================================

      Create Scenery
      
======================================================= */

void piece_create_scenery(void)
{
	int					rn,x,z,y,index;
	map_scenery_type	*scenery;
	
	if (map.nscenery==max_map_scenery) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Scenery","\pYou've reached the maximum number of sceneries for this map.",NULL,NULL);
	}
			
		// create spot
		
	index=map.nscenery;
	
	scenery=&map.sceneries[index];
 	map.nscenery++;
	
	rn=piece_create_get_spot(&x,&y,&z,0,0,0);
    
	scenery->pos.rn=rn;
	scenery->pos.x=x;
	scenery->pos.z=z;
	scenery->pos.y=y;
	scenery->ang.y=0;
	scenery->model_name[0]=0x0;
	scenery->animation_name[0]=0x0;
	scenery->lighting_mode=3;			// this is vertex lighting
	scenery->contact=TRUE;
	scenery->face_forward=FALSE;
	scenery->shadow=FALSE;
	scenery->override_size=FALSE;
	scenery->size.x=scenery->size.y=scenery->size.z=0;
    
		// select spot
		
    dp_object=TRUE;
	
	select_clear();
	select_add(scenery_piece,rn,index,-1);
	
	main_wind_draw();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
    
	dialog_scenery_setting_run(scenery);
}

/* =======================================================

      Create Light
      
======================================================= */

void piece_create_light(void)
{
	int					rn,x,z,y,index;
	map_light_type		*lit;
	
	if (map.nlight==max_map_light) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Light","\pYou've reached the maximum number of lights for this map.",NULL,NULL);
        return;
    }
	
		// create light
    
	index=map.nlight;
	
	lit=&map.lights[index];
	map.nlight++;
	
	rn=piece_create_get_spot(&x,&y,&z,0,0,0);
	
	lit->pos.rn=rn;
	lit->pos.x=x;
	lit->pos.z=z;
	lit->pos.y=y;
	lit->type=lt_normal;
	lit->col.r=lit->col.g=lit->col.b=1;
	lit->intensity=100;
	lit->on=TRUE;
	
		// select light
		
	dp_lightsoundparticle=TRUE;
	
	select_clear();
	select_add(light_piece,rn,index,-1);
	
	main_wind_draw();
	dialog_map_light_settings_run(lit);
}

/* =======================================================

      Create Sound
      
======================================================= */

void piece_create_sound(void)
{
	int					rn,x,z,y,index;
	map_sound_type		*snd;
	
	if (map.nsound==max_map_sound) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Sound","\pYou've reached the maximum number of sounds for this map.",NULL,NULL);
        return;
    }
	
		// create sound
	
	index=map.nsound;
	
	snd=&map.sounds[index];
	map.nsound++;
	
	rn=piece_create_get_spot(&x,&y,&z,0,0,0);
    
	snd->pos.rn=rn;
	snd->pos.x=x;
	snd->pos.z=z;
	snd->pos.y=y;
	snd->pitch=1;
	snd->name[0]=0x0;
	snd->on=TRUE;
	
		// select sound
		
    dp_lightsoundparticle=TRUE;
	
	select_clear();
	select_add(sound_piece,rn,index,-1);
	
	main_wind_draw();
	dialog_map_sound_settings_run(snd);
}

/* =======================================================

      Create Particle
      
======================================================= */

void piece_create_particle(void)
{
	int					rn,x,z,y,index;
	map_particle_type	*prt;
	
	if (map.nparticle==max_map_particle) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Particle","\pYou've reached the maximum number of particles for this map.",NULL,NULL);
        return;
    }
	
		// create particle
	
	index=map.nparticle;
	
	prt=&map.particles[index];
	map.nparticle++;
	
	rn=piece_create_get_spot(&x,&y,&z,0,0,0);
    
	prt->pos.rn=rn;
	prt->pos.x=x;
	prt->pos.z=z;
	prt->pos.y=y;
	prt->spawn_tick=5000;
	prt->slop_tick=0;
	prt->name[0]=0x0;
	prt->on=TRUE;
	
		// select particle
		
    dp_lightsoundparticle=TRUE;
	
	select_clear();
	select_add(particle_piece,rn,index,-1);
	
	main_wind_draw();
	dialog_map_particle_settings_run(prt);
}

/* =======================================================

      Create Node
      
======================================================= */

void piece_create_node(void)
{
	int				i,rn,x,z,y,index;
	node_type		*node;
			
	if (map.nnode==max_node) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Node","\pYou've reached the maximum number of nodes for a map.",NULL,NULL);
	}
	
		// create node
	
	index=map.nnode;
	
	node=&map.nodes[index];
	map.nnode++;
	
	rn=piece_create_get_spot(&x,&y,&z,0,0,0);
	
	node->pos.rn=rn;
	node->pos.x=x;
	node->pos.z=z;
	node->pos.y=y;
	node->ang.x=0.0f;
	node->ang.y=0.0f;
	node->ang.z=0.0f;
	node->name[0]=0x0;
	node->user_value=0;

	for (i=0;i!=max_node_link;i++) {
		node->link[i]=-1;
	}

		// select sound
		
	dp_node=TRUE;
	
	select_clear();
	select_add(node_piece,rn,index,-1);
	
	main_wind_draw();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
}

/* =======================================================

      Create Liquid
	        
======================================================= */

void piece_create_liquid(void)
{
	int				rn,x,y,z,index,sz;
	map_liquid_type	*liq;

	if (!piece_create_texture_ok()) return;

	sz=map_enlarge*4;
	rn=piece_create_get_spot(&x,&y,&z,sz,sz,0);
	
		// create the liquid
		
	index=map_portal_liquid_add(&map,rn);
	if (index==-1) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Liquid","\pNot enough memory.",NULL,NULL);
		return;
	}
	
	liq=&map.portals[rn].liquid.liquids[index];
	
	liq->lft=x-sz;
	liq->rgt=x+sz;
	liq->top=z-sz;
	liq->bot=z+sz;
	liq->y=y;
	
	liq->txt_idx=0;
	liq->group_idx=-1;
	
	liq->alpha=1.0f;
	liq->tint_alpha=0.5f;
	liq->speed_alter=1.0f;
	
	liq->col.r=liq->col.g=liq->col.b=1.0f;
	
	liq->x_txtoff=liq->y_txtoff=0.0f;
	liq->x_txtfact=liq->y_txtfact=1.0f;
    liq->x_shift=liq->y_shift=0.0f;
	
	liq->tide.rate=1000;
	liq->tide.high=100;
	liq->tide.split=600;
	liq->tide.direction=ld_horizontal;
	
	liq->harm.in_harm=0;
	liq->harm.drown_tick=10000;
	liq->harm.drown_harm=10;

		// select the liquid
		
	dp_liquid=TRUE;
	select_clear();
	select_add(liquid_piece,rn,index,-1);

	main_wind_draw();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
}
