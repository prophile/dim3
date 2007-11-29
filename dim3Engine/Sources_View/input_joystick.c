/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Joystick Input Routines

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

#include "inputs.h"

extern setup_type			setup;

SDL_Joystick				*input_joystick;

/* =======================================================

      Joystick Initialize
      
======================================================= */

bool input_joystick_initialize(void)
{
		// any joysticks?
		
	if (SDL_NumJoysticks()==0) return(FALSE);

	input_joystick=SDL_JoystickOpen(0);
	if (input_joystick==NULL) return(FALSE);

		// start up events

	SDL_JoystickEventState(SDL_ENABLE);
	return(TRUE);
}

void input_joystick_shutdown(void)
{
	SDL_JoystickEventState(SDL_IGNORE);
	SDL_JoystickClose(input_joystick);
}

/* =======================================================

      Game JoyStick Input
      
======================================================= */

void input_get_joystick_movement(float *x,float *y)
{
	int				kx,ky;
	
		// only consider movement past the quarter point
		
	kx=SDL_JoystickGetAxis(input_joystick,0);
	if (abs(kx)<joystick_axis_quarter_value) kx=0;
	
	ky=SDL_JoystickGetAxis(input_joystick,1);
	if (abs(ky)<joystick_axis_quarter_value) ky=0;
	
		// use the factor to get the joystick movements
		// within the same range as the mouse

	*x=(((float)kx)*0.004f)*setup.joystick_x.speed;
	*y=(((float)ky)*0.004f)*setup.joystick_y.speed;
}

bool input_get_joystick_button(int button_idx)
{
		// regular joystick input buttons
		
	if ((button_idx>=input_joystick_button_1) && (button_idx<=input_joystick_button_16)) {
		return(SDL_JoystickGetButton(input_joystick,(button_idx-input_joystick_button_1))!=0);
	
	}

		// these inputs are hard left/right/up/down on an axis
		// we determine this by a movement greater then halfway
	
	switch (button_idx) {

		case input_joystick_button_left:
			return(SDL_JoystickGetAxis(input_joystick,0)<-joystick_axis_half_value);

		case input_joystick_button_right:
			return(SDL_JoystickGetAxis(input_joystick,0)>joystick_axis_half_value);

		case input_joystick_button_up:
			return(SDL_JoystickGetAxis(input_joystick,1)<-joystick_axis_half_value);

		case input_joystick_button_down:
			return(SDL_JoystickGetAxis(input_joystick,1)>joystick_axis_half_value);

	}
	
	return(FALSE);
}

