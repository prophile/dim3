/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Action Routines

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

#include "inputs.h"

int							key_define_code[input_max_keyboard_define]=key_codes,
							button_define_code[input_max_button_define]=button_codes;
char						key_define_str[input_max_keyboard_define][32]=key_names,
							button_define_str[input_max_button_define][32]=button_names,
							control_names_str[][32]=control_names;
bool						input_key_set_skip_flag[input_max_keyboard_define];
												
input_action_type			input_actions[256];

extern bool					game_loop_quit;
extern setup_type			setup;

/* =======================================================

      Input Start and Stop
      
======================================================= */

void input_initialize(bool in_window)
{
	SDL_EventState(SDL_ACTIVEEVENT,SDL_IGNORE);
	SDL_EventState(SDL_KEYUP,SDL_IGNORE);
	SDL_EventState(SDL_MOUSEBUTTONUP,SDL_IGNORE);
	SDL_EventState(SDL_JOYAXISMOTION,SDL_IGNORE);
	SDL_EventState(SDL_JOYBALLMOTION,SDL_IGNORE);
	SDL_EventState(SDL_JOYHATMOTION,SDL_IGNORE);
	SDL_EventState(SDL_JOYBUTTONDOWN,SDL_IGNORE);
	SDL_EventState(SDL_JOYBUTTONUP,SDL_IGNORE);
	SDL_EventState(SDL_SYSWMEVENT,SDL_IGNORE);
	SDL_EventState(SDL_VIDEORESIZE,SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT,SDL_IGNORE);

	if (!in_window) {
		SDL_EventState(SDL_QUIT,SDL_IGNORE);
	}

	input_mouse_initialize();
}

void input_shutdown(void)
{
	input_mouse_shutdown();
}

/* =======================================================

      Input Setup
      
======================================================= */

void input_clear(void)
{
		// pump out rest of events
		
	input_event_pump();

		// clear keyboard and mouse
		
	input_clear_keyboard();
	input_clear_mouse();
}

/* =======================================================

      Clear Actions
      
======================================================= */

void input_action_clear(void)
{
    int					i;
	input_action_type	*action;
	
	action=input_actions;
    
	for (i=0;i!=256;i++) {
		action->nitem=0;
		action->still_down=FALSE;
		action++;
	}
}

/* =======================================================

      Setup Utilities
      
======================================================= */

int setup_find_control_for_name(char *name)
{
	int			n;
	
	for (n=0;n!=ncontrol;n++) {
		if (strcasecmp(control_names_str[n],name)==0) return(n);
	}
	
	return(-1);
}

int setup_find_action_in_setup(int action_idx)
{
	int			n;
	
	for (n=0;n!=setup.action_list.naction;n++) {
		if (strcasecmp(control_names_str[action_idx],setup.action_list.actions[n].name)==0) return(n);
	}
	
	return(-1);
}

void setup_to_input(void)
{
	int					n,k,control_idx;
	setup_action_type	*action;
	
	input_action_clear();
	
		// actions
		
	action=setup.action_list.actions;
	
	for (n=0;n!=setup.action_list.naction;n++) {
		control_idx=setup_find_control_for_name(action->name);
		if (control_idx!=-1) {
			for (k=0;k!=max_setup_action_attach;k++) {
				if (action->attach[k][0]!=0x0) input_action_attach(action->attach[k],control_idx);
			}
		}
		action++;
	}
}

/* =======================================================

      Wait For Key/Button And Return
      
======================================================= */

void input_set_key_start(void)
{
	int				i;

		// get a list of keys to ignore if they
		// are started out down (works around problems
		// with capsLock, numLock, scrollLock, etc

	for (i=0;i!=input_max_keyboard_define;i++) {
		input_key_set_skip_flag[i]=input_get_keyboard_key(key_define_code[i]);
	}

}

bool input_set_key_wait(char *name,bool *no_key_up)
{
	int				i;
	
	name[0]=0x0;
	*no_key_up=FALSE;

		// any keys pressed?
		
	for (i=0;i!=input_max_keyboard_define;i++) {
		if (!input_key_set_skip_flag[i]) {
			if (input_get_keyboard_key(key_define_code[i])) {
				strcpy(name,key_define_str[i]);
				*no_key_up=((key_define_code[i]==SDLK_NUMLOCK) || (key_define_code[i]==SDLK_CAPSLOCK) || (key_define_code[i]==SDLK_SCROLLOCK));
				return(TRUE);
			}
		}
	}
	
		// any mouse buttons
		
	for (i=0;i!=input_max_button_define;i++) {
		if (input_get_mouse_button(button_define_code[i])) {
			strcpy(name,button_define_str[i]);
			*no_key_up=((button_define_code[i]==SDL_BUTTON_WHEELUP) || (button_define_code[i]==SDL_BUTTON_WHEELDOWN));
			return(TRUE);
		}
	}
	
	return(FALSE);
}

/* =======================================================

      App Active State
      
======================================================= */

inline bool input_app_active(void)
{
	return((SDL_GetAppState()&SDL_APPINPUTFOCUS)!=0);
}

/* =======================================================

      Event Pumping
      
======================================================= */

void input_event_pump(void)
{
	SDL_Event			event;
	
	SDL_PumpEvents();
	
	while (SDL_PollEvent(&event)!=0) {
	
		switch (event.type) {

			case SDL_KEYDOWN:
				input_event_keydown(event.key.keysym.sym);
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				input_event_button_down(event.button.button);
				break;

			case SDL_MOUSEMOTION:
				input_event_mouse_motion(event.motion.xrel,event.motion.yrel);
				break;

			case SDL_QUIT:
				game_loop_quit=TRUE;
				break;
				
		}
	}
}

/* =======================================================

      Attach Actions
      
======================================================= */

void input_action_attach(char *attach_name,int action_index)
{
    int						i,index,type,nitem;
	input_action_type		*action;
	input_action_item_type  *item;
    
		// is it a key?
		
	index=-1;
	type=0;
	
    for (i=0;i!=input_max_keyboard_define;i++) {
        if (strcasecmp(attach_name,key_define_str[i])==0) {
            index=key_define_code[i];
			type=input_type_key;
            break;
        }
    }
	
		// is it a mouse button?
	
	if (index==-1) {
	
		for (i=0;i!=input_max_button_define;i++) {
			if (strcasecmp(attach_name,button_define_str[i])==0) {
				index=button_define_code[i];
				type=input_type_mouse_button;
				break;
			}
		}
	
	}
	
		// any attachment found?
		
	if (index==-1) return;
	
		// attach to action
		
	action=&input_actions[action_index];
	
	nitem=action->nitem;
	if (nitem==input_max_action_item) return;
	
	item=&action->items[nitem];
	action->nitem++;
	
	item->type=type;
	item->index=index;
}

/* =======================================================

      See If States Are Attached to Same Key/Button
      
======================================================= */

bool input_check_action_same_attachment(int action_1_index,int action_2_index)
{
	int						n1,n2,nitem_1,nitem_2;
	input_action_type		*action_1,*action_2;
	input_action_item_type	*item_1,*item_2;
	
	action_1=&input_actions[action_1_index];
	action_2=&input_actions[action_2_index];
	
	nitem_1=action_1->nitem;
	item_1=action_1->items;
	
	for (n1=0;n1<nitem_1;n1++) {
	
		nitem_2=action_2->nitem;
		item_2=action_2->items;
		
		for (n2=0;n2<nitem_2;n2++) {
			if ((item_1->type==item_2->type) && (item_1->index==item_2->index)) return(TRUE);
			item_2++;
		}
		
		item_1++;
	}
	
	return(FALSE);
}

/* =======================================================

      Button States
      
======================================================= */

bool input_action_get_state(int action_index)
{
	int						n,nitem;
	input_action_type		*action;
	input_action_item_type  *item;
	
	action=&input_actions[action_index];
	
	nitem=action->nitem;
	item=action->items;
	
	for (n=0;n<nitem;n++) {
		
		switch (item->type) {
		
				// keys
				
			case input_type_key:
				if (input_get_keyboard_key(item->index)) return(TRUE);
				break;
				
				// mouse buttons
				
			case input_type_mouse_button:
				if (input_get_mouse_button(item->index)) return(TRUE);
				break;
		}
		
		item++;
	}
	
	return(FALSE);
}

bool input_action_get_state_single(int action_index)
{
	input_action_type		*action;

	action=&input_actions[action_index];

	if (!input_action_get_state(action_index)) {
		action->still_down=FALSE;
		return(FALSE);
	}
	
	if (action->still_down) return(FALSE);			// only return TRUE on first trigger
	
	action->still_down=TRUE;
	return(TRUE);
}

