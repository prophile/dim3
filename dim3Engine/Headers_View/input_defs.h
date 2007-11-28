/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Input Definitions Header

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

//
// input structures
//

#define input_max_mouse_button			8
#define input_max_keyboard_key			512

#define input_max_keyboard_define		133

#define last_raw_key_count				4

//
// configurable control names
//

#define control_names			{"move_forward","move_backward","turn_left","turn_right", \
								 "sidestep_left","sidestep_right","run", \
								 "jump","duck","up","down","enter-exit", \
								 "next_weapon","previous_weapon", \
								 "fire_0","fire_1","fire_2","fire_3", \
								 "zoom","zoom_increase","zoom_decrease", \
								 "weapon_0","weapon_1","weapon_2","weapon_3","weapon_4", \
								 "weapon_5","weapon_6","weapon_7","weapon_8","weapon_9", \
								 "weapon_10","weapon_11","weapon_12","weapon_13","weapon_14", \
								 "weapon_15","weapon_16","weapon_17","weapon_18","weapon_19", \
								 "game_0","game_1","game_2","game_3","game_4", \
								 "game_5","game_6","game_7","game_8","game_9", \
								 "game_10","game_11","game_12","game_13","game_14", \
								 "game_15","game_16","game_17","game_18","game_19", \
								 "player_0","player_1","player_2","player_3","player_4", \
								 "player_5","player_6","player_7","player_8","player_9", \
								 "player_10","player_11","player_12","player_13","player_14", \
								 "player_15","player_16","player_17","player_18","player_19", \
								 "console","menu","quick_save","save_load","message", \
								 "debug_status","debug_light","debug_screenshot","debug_map", \
								 ""}

//
// configurable controls
//

#define ncontrol				90				// total number of controls

#define nc_move_forward			0
#define nc_move_backward		1
#define nc_turn_left			2
#define nc_turn_right			3

#define nc_sidestep_left		4
#define nc_sidestep_right		5

#define nc_run					6

#define nc_jump					7
#define nc_duck					8

#define nc_up					9
#define nc_down					10

#define nc_enter_exit			11

#define nc_next_weapon			12
#define nc_previous_weapon		13

#define nc_fire_0				14
#define nc_fire_1				15
#define nc_fire_2				16
#define nc_fire_3				17

#define nc_weapon_zoom			18
#define nc_weapon_zoom_increase	19
#define nc_weapon_zoom_decrease	20

#define nc_weapon_0				21
#define nc_weapon_1				22
#define nc_weapon_2				23
#define nc_weapon_3				24
#define nc_weapon_4				25
#define nc_weapon_5				26
#define nc_weapon_6				27
#define nc_weapon_7				28
#define nc_weapon_8				29
#define nc_weapon_9				30
#define nc_weapon_10			31
#define nc_weapon_11			32
#define nc_weapon_12			33
#define nc_weapon_13			34
#define nc_weapon_14			35
#define nc_weapon_15			36
#define nc_weapon_16			37
#define nc_weapon_17			38
#define nc_weapon_18			39
#define nc_weapon_19			40

#define nc_game_0				41
#define nc_game_1				42
#define nc_game_2				43
#define nc_game_3				44
#define nc_game_4				45
#define nc_game_5				46
#define nc_game_6				47
#define nc_game_7				48
#define nc_game_8				49
#define nc_game_9				50
#define nc_game_10				51
#define nc_game_11				52
#define nc_game_12				53
#define nc_game_13				54
#define nc_game_14				55
#define nc_game_15				56
#define nc_game_16				57
#define nc_game_17				58
#define nc_game_18				59
#define nc_game_19				60

#define nc_player_0				61
#define nc_player_1				62
#define nc_player_2				63
#define nc_player_3				64
#define nc_player_4				65
#define nc_player_5				66
#define nc_player_6				67
#define nc_player_7				68
#define nc_player_8				69
#define nc_player_9				70
#define nc_player_10			71
#define nc_player_11			72
#define nc_player_12			73
#define nc_player_13			74
#define nc_player_14			75
#define nc_player_15			76
#define nc_player_16			77
#define nc_player_17			78
#define nc_player_18			79
#define nc_player_19			80

#define nc_console				81
#define nc_menu					82
#define nc_quick_save			83
#define nc_save_load			84

#define nc_message				85

#define nc_debug_status			86
#define nc_debug_light			87
#define nc_debug_screenshot		88
#define nc_debug_map			89

//
// control ranges
//

#define nc_fire_start			14
#define nc_fire_end				17

#define nc_weapon_start			21
#define nc_weapon_end			40

#define nc_game_start			41
#define nc_game_end				60

#define nc_player_start			61
#define nc_player_end			80

//
// SDL key lookups
//

#define key_names						{ \
											"Backspace","Tab","Clear","Return","Pause","Escape","Space", \
											"!","\"","#","$","&","'","(",")","*","+",",","-",".","/", \
											"0","1","2","3","4","5","6","7","8","9", \
											":",";","<","=",">","?","@","[","\\","]","^","_","`", \
											"A","B","C","D","E","F","G","H","I","J","K","L","M", \
											"N","O","P","Q","R","S","T","U","V","W","X","Y","Z", \
											"Delete","KeyPad0","KeyPad1","KeyPad2","KeyPad3","KeyPad4","KeyPad5", \
											"KeyPad6","KeyPad7","KeyPad8","KeyPad9", \
											"KeyPad.","KeyPad/","KeyPad*","KeyPad-","KeyPad+","KeyPadEnter","KeyPad=", \
											"UpArrow","DownArrow","RightArrow","LeftArrow", \
											"Insert","Home","End","PageUp","PageDown", \
											"F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12","F13","F14","F15", \
											"NumLock","CapsLock","ScrollLock", \
											"RightShift","LeftShift","RightControl","LeftControl","RightOption","LeftOption", \
											"RightCommand","LeftCommand","LeftWindow","RightWindow", \
											"ModeShift","Help","PrintScreen","SysRq","Break","Menu","Power","Euro" \
										}
										
#define key_codes						{ \
											SDLK_BACKSPACE,SDLK_TAB,SDLK_CLEAR,SDLK_RETURN,SDLK_PAUSE,SDLK_ESCAPE,SDLK_SPACE, \
											SDLK_EXCLAIM,SDLK_QUOTEDBL,SDLK_HASH,SDLK_DOLLAR,SDLK_AMPERSAND,SDLK_QUOTE,SDLK_LEFTPAREN,SDLK_RIGHTPAREN,SDLK_ASTERISK,SDLK_PLUS,SDLK_COMMA,SDLK_MINUS,SDLK_PERIOD,SDLK_SLASH, \
											SDLK_0,SDLK_1,SDLK_2,SDLK_3,SDLK_4,SDLK_5,SDLK_6,SDLK_7,SDLK_8,SDLK_9, \
											SDLK_COLON,SDLK_SEMICOLON,SDLK_LESS,SDLK_EQUALS,SDLK_GREATER,SDLK_QUESTION,SDLK_AT,SDLK_LEFTBRACKET,SDLK_BACKSLASH,SDLK_RIGHTBRACKET,SDLK_CARET,SDLK_UNDERSCORE,SDLK_BACKQUOTE, \
											SDLK_a,SDLK_b,SDLK_c,SDLK_d,SDLK_e,SDLK_f,SDLK_g,SDLK_h,SDLK_i,SDLK_j,SDLK_k,SDLK_l,SDLK_m, \
											SDLK_n,SDLK_o,SDLK_p,SDLK_q,SDLK_r,SDLK_s,SDLK_t,SDLK_u,SDLK_v,SDLK_w,SDLK_x,SDLK_y,SDLK_z, \
											SDLK_DELETE,SDLK_KP0,SDLK_KP1,SDLK_KP2,SDLK_KP3,SDLK_KP4,SDLK_KP5, \
											SDLK_KP6,SDLK_KP7,SDLK_KP8,SDLK_KP9, \
											SDLK_KP_PERIOD,SDLK_KP_DIVIDE,SDLK_KP_MULTIPLY,SDLK_KP_MINUS,SDLK_KP_PLUS,SDLK_KP_ENTER,SDLK_KP_EQUALS, \
											SDLK_UP,SDLK_DOWN,SDLK_RIGHT,SDLK_LEFT, \
											SDLK_INSERT,SDLK_HOME,SDLK_END,SDLK_PAGEUP,SDLK_PAGEDOWN, \
											SDLK_F1,SDLK_F2,SDLK_F3,SDLK_F4,SDLK_F5,SDLK_F6,SDLK_F7,SDLK_F8,SDLK_F9,SDLK_F10,SDLK_F11,SDLK_F12,SDLK_F13,SDLK_F14,SDLK_F15, \
											SDLK_NUMLOCK,SDLK_CAPSLOCK,SDLK_SCROLLOCK, \
											SDLK_RSHIFT,SDLK_LSHIFT,SDLK_RCTRL,SDLK_LCTRL,SDLK_RALT,SDLK_LALT, \
											SDLK_RMETA,SDLK_LMETA,SDLK_LSUPER,SDLK_RSUPER, \
											SDLK_HELP,SDLK_PRINT,SDLK_SYSREQ,SDLK_BREAK,SDLK_MENU,SDLK_POWER,SDLK_EURO \
										}
										
#define input_max_mouse_button_define		5

#define mouse_button_names					{"MouseLeft","MouseRight","MouseMiddle","MouseWheelUp","MouseWheelDown"}
#define mouse_button_codes					{SDL_BUTTON_LEFT,SDL_BUTTON_RIGHT,SDL_BUTTON_MIDDLE,SDL_BUTTON_WHEELUP,SDL_BUTTON_WHEELDOWN}

#define input_max_joystick_button_define	8

#define joystick_button_names				{"Left","Right","Up","Down","Button 1","Button 2","Button 3","Button 4"}

//
// joystick axis points
//

#define joystick_axis_half_value		16384
#define joystick_axis_max_value			32768

//
// action structures
//

#define input_max_action_item			4

#define input_type_key					0
#define input_type_mouse_button			1
#define input_type_joystick_button		2

#define input_mouse_button_left			0
#define input_mouse_button_right		1
#define input_mouse_button_middle		2
#define input_mouse_button_wheel_up		3
#define input_mouse_button_wheel_down	4

#define input_joystick_button_left		0
#define input_joystick_button_right		1
#define input_joystick_button_up		2
#define input_joystick_button_down		3
#define input_joystick_button_1			4
#define input_joystick_button_2			5
#define input_joystick_button_3			6
#define input_joystick_button_4			7

typedef struct		{
						int						type,index;
					} input_action_item_type;
					
typedef struct		{
						int						nitem;
						input_action_item_type  items[input_max_action_item];
						bool					still_down;
					} input_action_type;

//
// UI display structures
//

typedef struct		{
						char					display_name[name_str_len];
						bool					show;
					} action_display_type;


