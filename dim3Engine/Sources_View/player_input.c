/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Player Routines

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "client.h"
#include "scripts.h"
#include "objects.h"
#include "remotes.h"
#include "weapons.h"
#include "cameras.h"
#include "inputs.h"

extern map_type				map;
extern view_type			view;
extern server_type			server;
extern camera_type			camera;
extern hud_type				hud;
extern js_type				js;
extern setup_type			setup;
extern network_setup_type	net_setup;

int							mouse_last_read_tick;
bool						weapon_change_key_down,weapon_zoom_key_down,enter_exit_key_down,toggle_run_state,
							fire_key_down[4],command_key_down[20],player_key_down[20];

extern void chat_add_message(int tick,char *name,char *str,d3col *col);

/* =======================================================

      Clear Input
      
======================================================= */

void player_clear_input(void)
{
	int			i;
	
	mouse_last_read_tick=-1;

	weapon_change_key_down=FALSE;
	weapon_zoom_key_down=FALSE;
	enter_exit_key_down=FALSE;
	toggle_run_state=FALSE;
	
	for (i=0;i!=4;i++) {
		fire_key_down[i]=FALSE;
	}

    for (i=0;i!=20;i++) {
        command_key_down[i]=FALSE;
        player_key_down[i]=FALSE;
    }
	
	input_clear();

	hud.chat.type_on=FALSE;
}

/* =======================================================

      Player Command Keys
      
======================================================= */

void player_command_input(obj_type *obj)
{
	int			i,k;

        // game keys
        
	for (i=nc_game_start;i<=nc_game_end;i++) {
    
        k=i-nc_game_start;
        
        if (input_action_get_state(i)) {
            if (!command_key_down[k]) {
                command_key_down[k]=TRUE;
				scripts_post_event_console(&js.game_attach,sd_event_message,sd_event_message_from_key_down,k);
            }
        }
        else {
            if (command_key_down[k]) {
                command_key_down[k]=FALSE;
				scripts_post_event_console(&js.game_attach,sd_event_message,sd_event_message_from_key_up,k);
            }
        }
    }
    
        // player keys
        
	for (i=nc_player_start;i<=nc_player_end;i++) {
    
        k=i-nc_player_start;
        
        if (input_action_get_state(i)) {
            if (!player_key_down[k]) {
                player_key_down[k]=TRUE;
				scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_key_down,k);
            }
        }
        else {
            if (player_key_down[k]) {
                player_key_down[k]=FALSE;
				scripts_post_event_console(&obj->attach,sd_event_message,sd_event_message_from_key_up,k);
            }
        }
    }
}

/* =======================================================

      Player Movement Input
      
======================================================= */

float player_mouse_smooth(float mouse_ang,float turn_ang)
{
	if (!setup.mouse_smooth) return(mouse_ang);
	
	if (mouse_ang>0) {
		if ((mouse_ang<turn_ang) && (turn_ang>=0)) {
			return(turn_ang/2.0f);
		}
		
		return(mouse_ang);
	}

	if (mouse_ang<0) {
		if ((mouse_ang>turn_ang) && (turn_ang<=0)) {
			return(turn_ang/2.0f);
		}
		
		return(mouse_ang);
	}
	
	return(mouse_ang);
}

void player_turn_mouse_input(obj_type *obj,float mouse_x)
{
		// only turn in fpp and no mouse ignores
		
	if (((obj->input_mode!=im_fpp) && (obj->input_mode!=im_fly)) || (obj->turn.ignore_mouse)) {
		obj->turn.ang_add.y=0;
		return;
	}

		// set the turning

	obj->turn.ang_add.y=player_mouse_smooth(mouse_x,obj->turn.ang_add.y);
}

void player_turn_key_input(obj_type *obj)
{
	if ((obj->input_mode!=im_fpp) && (obj->input_mode!=im_fly)) return;
	
	if (input_action_get_state(nc_turn_left)) obj->turn.ang_add.y=-ky_turn_speed;
	if (input_action_get_state(nc_turn_right)) obj->turn.ang_add.y=ky_turn_speed;
}

void player_look_mouse_input(obj_type *obj,float mouse_y)
{
	if (obj->turn.ignore_mouse) {
		obj->look.ang_add=0;
		return;
	}
	
	obj->look.ang_add=player_mouse_smooth(mouse_y,obj->look.ang_add);
}

void player_movement_fpp_xz_input(obj_type *obj)
{
	bool			key_forward,key_backward;

		// forward or backwards movement keys

	key_backward=FALSE;
	key_forward=input_action_get_state(nc_move_forward);
	if (!key_forward) {
		key_backward=input_action_get_state(nc_move_backward);
	}

		// no quick reverse locks

	if (!obj->quick_reverse) {
		if (key_forward) {
			if ((obj->forward_move.reverse) && (obj->forward_move.speed!=0)) key_forward=FALSE;
		}

		if (key_backward) {
			if ((!obj->forward_move.reverse) && (obj->forward_move.speed!=0)) key_backward=FALSE;
		}
	}

		// forward movement

	if (key_forward) {
		obj->forward_move.moving=TRUE;
		if (obj->forward_move.reverse) {
			obj->forward_move.speed=0;
		}
		obj->forward_move.reverse=FALSE;
	}

	if (key_backward) {
		obj->forward_move.moving=TRUE;
		if (!obj->forward_move.reverse) {
			obj->forward_move.speed=0;
		}
		obj->forward_move.reverse=TRUE;
	}

		// run, walk
	
	if (obj->single_speed) {
		obj->forward_move.running=FALSE;
	}
	else {
	
		if (!setup.toggle_run) {
			toggle_run_state=input_action_get_state(nc_run);
		}
		else {
			if (input_action_get_state_single(nc_run)) toggle_run_state=!toggle_run_state;
		}
		
		if (setup.always_run) {
			obj->forward_move.running=!toggle_run_state;
		}
		else {
			obj->forward_move.running=toggle_run_state;
		}
	}

		// side movement
	
	if (input_action_get_state(nc_sidestep_left)) {
		obj->side_move.moving=TRUE;
		if (obj->side_move.reverse) {
			obj->side_move.speed=0;
		}
		obj->side_move.reverse=FALSE;
	}
	else {
		if (input_action_get_state(nc_sidestep_right)) {
			obj->side_move.moving=TRUE;
			if (!obj->side_move.reverse) {
				obj->side_move.speed=0;
			}
			obj->side_move.reverse=TRUE;
		}
	}
}
	
void player_movement_fpp_y_input(obj_type *obj)
{
    if (input_action_get_state(nc_down)) {
        obj->vert_move.moving=TRUE;
        if (obj->vert_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=FALSE;
		return;
    }

	if (input_action_get_state(nc_up)) {
		obj->vert_move.moving=TRUE;
        if (!obj->vert_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=TRUE;
		return;
    }
}

void player_movement_fly_swim_y_input(obj_type *obj)
{
	if (!obj->forward_move.moving) return;

    if (obj->view_ang.x<-obj->vert_move.slop) {
        obj->vert_move.moving=TRUE;
        if (obj->vert_move.reverse==obj->forward_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=!obj->forward_move.reverse;
		return;
    }
    
	if (obj->view_ang.x>obj->vert_move.slop) {
		obj->vert_move.moving=TRUE;
        if (!obj->vert_move.reverse==obj->forward_move.reverse) obj->vert_move.speed=0;
        obj->vert_move.reverse=obj->forward_move.reverse;
		return;
    }
}

void player_movement_side_scroll_input(obj_type *obj)
{
		// left-right movement
		
	if (input_action_get_state(nc_turn_left)) {
		object_player_turn_direct(obj,270);
	}
	else {
		if (input_action_get_state(nc_turn_right)) {
			object_player_turn_direct(obj,90);
		}
	}
	
		// up-down movement
	
    if (input_action_get_state(nc_down)) {
        obj->vert_move.moving=TRUE;
        if (obj->vert_move.reverse) {
            obj->vert_move.speed=0;
        }
        obj->vert_move.reverse=FALSE;
    }
    else {
        if (input_action_get_state(nc_up)) {
            obj->vert_move.moving=TRUE;
            if (!obj->vert_move.reverse) {
                obj->vert_move.speed=0;
            }
            obj->vert_move.reverse=TRUE;
        }
    }
}

void player_movement_top_down_input(obj_type *obj)
{
	bool				left,right,up,down;
	
	left=input_action_get_state(nc_turn_left);
	right=input_action_get_state(nc_turn_right);
	up=input_action_get_state(nc_move_forward);
	down=input_action_get_state(nc_move_backward);
	
		// all the movements
		
	if (left) {
		if (up) {
			object_player_turn_direct(obj,315);
			return;
		}
		if (down) {
			object_player_turn_direct(obj,225);
			return;
		}
		object_player_turn_direct(obj,270);
		return;
	}
	
	if (right) {
		if (up) {
			object_player_turn_direct(obj,45);
			return;
		}
		if (down) {
			object_player_turn_direct(obj,135);
			return;
		}
		object_player_turn_direct(obj,90);
		return;
	}
	
	if (up) {
		object_player_turn_direct(obj,0);
		return;
	}
	
	if (down) {
		object_player_turn_direct(obj,180);
		return;
	}
}

void player_movement_input(obj_type *obj)
{
		// start with no movement
		
    obj->forward_move.moving=FALSE;
	obj->side_move.moving=FALSE;
	obj->vert_move.moving=FALSE;
	
		// don't move if ducked and crawling disabled
		
	if ((!obj->crawl) && (obj->duck.mode!=dm_stand)) return;
	
		// get proper movement
	
	switch (obj->input_mode) {
	
		case im_fpp:
			player_movement_fpp_xz_input(obj);
			if ((!obj->fly) && (obj->liquid_mode!=lm_under)) {
				player_movement_fpp_y_input(obj);
			}
			else {
				player_movement_fly_swim_y_input(obj);
			}
			break;
			
		case im_side_scroll:
			player_movement_side_scroll_input(obj);
			break;
			
		case im_top_down:
			player_movement_top_down_input(obj);
			break;

		case im_fly:
			player_movement_fpp_xz_input(obj);
			player_movement_fly_swim_y_input(obj);
			break;
	}
}

void player_jump_duck_input(obj_type *obj)
{
	if (input_action_get_state(nc_jump)) object_start_jump(obj);
    
	if (input_action_get_state(nc_duck)) {
        object_start_duck(obj);
    }
    else {
        object_start_stand(obj);
    }
}

/* =======================================================

      Player Weapon Input
      
======================================================= */

void player_weapon_zoom_input(obj_type *obj,weapon_type *weap)
{
	bool				zoom_key,zoom_increase_key,zoom_decrease_key;
 	
		// zooms allowed?
		
	if (!weap->zoom.on) return;
	
		// any zoom keys down?
		
	zoom_key=input_action_get_state(nc_weapon_zoom);
	zoom_increase_key=input_action_get_state(nc_weapon_zoom_increase);
	zoom_decrease_key=input_action_get_state(nc_weapon_zoom_decrease);
	
	if ((!zoom_key) && (!zoom_increase_key) && (!zoom_decrease_key)) {
		weapon_zoom_key_down=FALSE;
		return;
	}
	
		// key held down, ignore
		
	if (weapon_zoom_key_down) return;
	
		// check zoom activate/deactive key
		
	if (zoom_key) {

		if (!weap->zoom.active) {
			weapon_zoom_on(obj,weap);
		}
		else {
			weapon_zoom_off(obj,weap);
		}
		
		weapon_zoom_key_down=TRUE;
		return;
	}
		
		// if zoom active, check increase/decrease keys
		
	if (!weap->zoom.active) return;
	
	if ((zoom_increase_key) && (weap->zoom.current_step<(weap->zoom.step_count-1))) {
		weap->zoom.current_step++;
		weapon_zoom_key_down=TRUE;
		return;
	}

	if ((zoom_decrease_key) && (weap->zoom.current_step>0)) {
		weap->zoom.current_step--;
		weapon_zoom_key_down=TRUE;
		return;
	}
}

void player_weapon_change_input(int tick,obj_type *obj,weapon_type *weap)
{
	int				i,k;
	bool			down_key,weapon_key[(nc_weapon_end-nc_weapon_start)+1],
					next_weapon_key,previous_weapon_key;
	
		// special check if weapon change key is same as zoom key
		// if zoom on, don't change weapon
		
	next_weapon_key=input_action_get_state(nc_next_weapon);
	previous_weapon_key=input_action_get_state(nc_previous_weapon);
		
	if ((weap->zoom.on) && (weap->zoom.active)) {
		if (input_check_action_same_attachment(nc_next_weapon,nc_weapon_zoom_increase)) {
			next_weapon_key=FALSE;
		}
		if (input_check_action_same_attachment(nc_next_weapon,nc_weapon_zoom_decrease)) {
			next_weapon_key=FALSE;
		}
		if (input_check_action_same_attachment(nc_previous_weapon,nc_weapon_zoom_increase)) {
			previous_weapon_key=FALSE;
		}
		if (input_check_action_same_attachment(nc_previous_weapon,nc_weapon_zoom_decrease)) {
			previous_weapon_key=FALSE;
		}
	}
	
		// any weapon change keys down?
		
	down_key=next_weapon_key||previous_weapon_key;

	for (i=nc_weapon_start;i<=nc_weapon_end;i++) {
		k=i-nc_weapon_start;
		weapon_key[k]=input_action_get_state(i);
		down_key=down_key||weapon_key[k];
	}
	
	if (!down_key) {
		weapon_change_key_down=FALSE;
		return;
	}
	
		// key held down, ignore
		
	if (weapon_change_key_down) return;
	
		// change weapons

	weapon_zoom_off(obj,weap);
		
	if (next_weapon_key) {
		weapon_switch(tick,obj,1);
		weapon_change_key_down=TRUE;
		return;
	}

	if (previous_weapon_key) {
		weapon_switch(tick,obj,-1);
		weapon_change_key_down=TRUE;
		return;
	}
	
	for ((i=nc_weapon_start);(i<=nc_weapon_end);i++) {
		k=i-nc_weapon_start;
		if (weapon_key[k]) {
			weapon_pick(tick,obj,k);
			weapon_change_key_down=TRUE;
			return;
		}
	}
}

void player_weapon_fire_input(int tick,obj_type *obj,weapon_type *weap)
{
	int					i,method;
	
		// no firing if no weapon is being held
		
	if (obj->held_weapon.mode!=wm_held) return;
		
		// check keys

	for (i=nc_fire_start;i<=nc_fire_end;i++) {
	
		method=i-nc_fire_start;
		
		if (input_action_get_state(i)) {
			if (!fire_key_down[method]) {
				fire_key_down[method]=TRUE;
				weapon_player_fire_down(tick,obj,weap,method);
			}
			else {
				weapon_player_fire_repeat(tick,obj,weap);
			}
		}
		else {
			if (fire_key_down[method]) {
				fire_key_down[method]=FALSE;
				weapon_player_fire_up(obj,weap,method);
			}
		}
	}
}

void player_weapon_input(int tick,obj_type *obj)
{
	weapon_type			*weap;
	
		// get held weapon
		
	weap=weapon_find_uid(obj->held_weapon.current_uid);
	if (weap==NULL) return;

		// weapon keys
		
	if ((obj->held_weapon.mode==wm_held) && (weap!=NULL)) {
		player_weapon_zoom_input(obj,weap);
		player_weapon_fire_input(tick,obj,weap);
	}

	player_weapon_change_input(tick,obj,weap);
}

/* =======================================================

      Player Enter-Exit
      
======================================================= */

void player_enter_exit_input(obj_type *obj)
{
	if (!input_action_get_state(nc_enter_exit)) {
		enter_exit_key_down=FALSE;
		return;
	}
	
	if (enter_exit_key_down) return;			// only check on key downs
	
	enter_exit_key_down=TRUE;
	
		// exit a vehicle?
		
	if (obj->vehicle.attach_obj_uid!=-1) {
		object_exit_vehicle(obj,FALSE,NULL);
		return;
	}
		
		// enter a vehicle?
	
	object_enter_vehicle(obj,NULL);
}

/* =======================================================

      Player Messages
      
======================================================= */

bool player_message_input(int tick,obj_type *obj)
{
	int				len;
	char			ch,nstr[chat_str_len];
	char			*c,*msg;
	d3col			col;
	
	if (!net_setup.client.joined) return(TRUE);

	if (hud.chat.type_on) {
	
			// get last raw key as character
			
		ch=input_get_last_raw_key();
		input_clear_last_raw_key();
		
		if (ch==0x0) return(FALSE);
		
			// get current message len
			
		msg=hud.chat.type_str;
		len=strlen(msg);
		
			// escape cancels
			
		if (ch==0x1B) {
			hud.chat.type_on=FALSE;
			return(FALSE);
		}
		
			// return ends entry
			
		if (ch==0xD) {
			hud.chat.type_on=FALSE;
			if (len>1) {
				msg[len-1]=0x0;
				network_client_send_message(net_setup.client.remote_uid,msg);
				player_get_ui_color(&col);
				chat_add_message(tick,obj->name,msg,&col);
			}
			return(FALSE);
		}
		
			// delete
			
		if (ch==0x8) {
			len-=2;
			if (len<0) len=0;

			msg[len++]='_';
			msg[len]=0x0;

			return(FALSE);
		}
		
			// eliminate certain characters
			
		if (((ch<'!') || (ch>'z')) && (ch!=0x20)) return(FALSE);
		
			// force start a new line if too many characters
			
		if (len>=(chat_str_len-1)) {
			nstr[0]=0x0;
			
			c=strrchr(msg,' ');
			if (c!=NULL) {
				if ((int)(c-msg)>5) {
					strcpy(nstr,(c+1));
					*c=0x0;
				}
			}
			
			msg[len-1]=0x0;
			network_client_send_message(net_setup.client.remote_uid,msg);
			player_get_ui_color(&col);
			chat_add_message(tick,obj->name,msg,&col);
			
			strcpy(msg,nstr);
			len=strlen(msg);
		}
		
			// add new character
			
		if (len>0) len--;
		
		msg[len++]=ch;
		msg[len++]='_';
		msg[len]=0x0;
	
		return(FALSE);
	}
	
	if (input_action_get_state(nc_message)) {
		hud.chat.type_on=TRUE;
		
		input_clear_all_last_raw_key();
		
		hud.chat.type_str[0]='_';
		hud.chat.type_str[1]=0x0;
		
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Get Player Input
      
======================================================= */

void player_get_input(int tick)
{
	float			mouse_x,mouse_y;
	obj_type		*obj;
	
	obj=object_find_uid(server.player_obj_uid);
	
		// message inputs
		
	if (!player_message_input(tick,obj)) return;
		
		// commands (game and player)
		
	player_command_input(obj);
	
        // check for no-movement states
        
	if ((obj->hidden) || (obj->input_freeze)) return;
	
		// get turning movement
		
	if (!setup.joystick_turn) {
		input_get_mouse_movement(tick,&mouse_x,&mouse_y);
	}
	else {
		input_get_joystick_movement(&mouse_x,&mouse_y);
	}

		// turning and movement
		
	player_turn_mouse_input(obj,mouse_x);
	player_turn_key_input(obj);
	player_look_mouse_input(obj,mouse_y);
	player_movement_input(obj);
    	
        // jump and duck
		
	player_jump_duck_input(obj);
	
        // weapons
    
	player_weapon_input(tick,obj);
	
		// enter and exit
		
	player_enter_exit_input(obj);
	
		// clear mouse wheel state (since it's single state instead of held, like buttons)
		
	input_clear_mouse_wheel_state();
}

