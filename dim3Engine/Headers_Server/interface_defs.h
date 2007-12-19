/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: HUD Definitions Header

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
// GUI Defines
//

#define max_element					128
#define max_element_str_len			64
#define max_element_value_str_len	64
#define max_element_column			32
#define max_element_tab				8

//
// GUI elements
//

#define element_type_button			0
#define element_type_bitmap			1
#define element_type_text			2
#define element_type_text_field		3
#define element_type_checkbox		4
#define element_type_combo			5
#define element_type_slider			6
#define element_type_table			7
#define element_type_tab			8

//
// GUI position mode
//

#define element_pos_top				0
#define element_pos_bottom			1
#define element_pos_center			2

#define element_pos_left			0
#define element_pos_right			1

//
// GUI element structures
//

typedef struct		{
						bitmap_type				bitmap,bitmap_select;
					} element_button_type;

typedef struct		{
						int						just;
						bool					small_text,alert;
					} element_text_type;

typedef struct		{
						float					value,min,max;
					} element_slider_type;

typedef struct		{
						float					percent_size;
						char					name[name_str_len];
					} element_column_type;

typedef struct		{
						int						ncolumn;
						bool					busy;
						element_column_type		cols[max_element_column];
					} element_table_type;

typedef struct		{
						int						ntab;
						bitmap_type				bitmap[max_element_tab],bitmap_select[max_element_tab];
					} element_tab_type;

typedef union		{
						element_button_type		button;
						element_text_type		text;
						element_slider_type		slider;
						element_table_type		table;
						element_tab_type		tab;
					} element_setup_type;

typedef struct		{
						int						id,type,x,y,wid,high,ext_high,
												value,offset,max_value_str_sz;
						bool					selectable,enabled,hidden,framed;
						char					str[max_element_str_len],
												value_str[max_element_value_str_len],
												*data;
						element_setup_type		setup;
					} element_type;

//
// HUD defines
//

#define max_hud_image				300	

#define max_hud_bitmap				256
#define max_hud_text				256
#define max_hud_bar					32

#define max_radar_icon				16

#define max_hud_text_str_sz			256

#define max_menu					16
#define max_menu_item				24
#define max_menu_item_data_sz		64

#define max_chooser					32
#define max_chooser_item			32

#define max_story_page				16

//
// chat
//

#define max_chat_lines				15
#define chat_str_len				64

//
// console
//

#define max_console_line			8
#define max_console_txt_sz			80

#define console_mode_close			0
#define console_mode_fps			1
#define console_mode_open			2

typedef struct		{
						int						style;
						char					txt[max_console_txt_sz];
						d3col					color;
					} console_line_type;

//
// color settings
//

typedef struct		{
						d3col					base,header,disabled,mouse_over,hilite;
					} hud_color_type;

//
// bitmaps, text, bars
//

typedef struct		{
						int						start_tick,
												fade_in_tick,life_tick,fade_out_tick;
						bool					on;
					} hud_item_fade_type;
					
typedef struct		{
						int						x_add,y_add,count;
						bool					on;
					} hud_bitmap_repeat_type;
					
typedef struct		{
						int						x,y,x_size,y_size,image_idx,show_tick;
						float					alpha,rot;
						char					name[name_str_len],filename[file_str_len];
						bool					show,flash,flip_horz,flip_vert;
						hud_bitmap_repeat_type	repeat;
						image_animation_type	animate;
						hud_item_fade_type		fade;
					} hud_bitmap_type;
					
typedef struct		{
						int						x,y,just;
						float					alpha;
						char					name[name_str_len],data[max_hud_text_str_sz];
						bool					show,large,fps,has_return;
						d3col					color;
						hud_item_fade_type		fade;
					} hud_text_type;
					
typedef struct		{
						int						x,y,x_size,y_size;
						float					fill_alpha,outline_alpha,value;
						char					name[name_str_len];
						bool					outline,vert,show;
						d3col					fill_start_color,fill_end_color,outline_color;
					} hud_bar_type;

//
// radar
//

typedef struct		{
						int						size,image_idx;
						char					name[name_str_len],bitmap_name[name_str_len];
					} hud_radar_icon_type;

typedef struct		{
						int						x,y,display_radius,view_radius,
												background_image_idx,nicon;
						bool					on;
						char					background_bitmap_name[name_str_len];
						hud_radar_icon_type		icons[max_radar_icon];
					} hud_radar_type;
			
//
// menus
//

typedef struct		{
						int						item_id;
						char					data[max_menu_item_data_sz],sub_menu[name_str_len];
						bool					multiplayer_disable,quit;
					} menu_item_type;
					
typedef struct		{
						int						nitem;
						char					name[name_str_len];
						menu_item_type			items[max_menu_item];
					} menu_type;
					
//
// chooser
//

typedef struct		{
						int						x,y,item_id;
						char					file[file_str_len];
						bool					clickable;
					} chooser_item_type;
					
typedef struct		{
						int						nitem;
						char					name[name_str_len];
						chooser_item_type		items[max_chooser_item];
					} chooser_type;
					
//
// progress
//

typedef struct		{
						int						lx,rx,ty,by;
						d3col					base_color_start,base_color_end,
												hilite_color_start,hilite_color_end,
												text_color;
					} hud_progress_type;
					
//
// intro
//

typedef struct		{
						int						x,y,wid,high;
						bool					on;
					} hud_intro_button_type;
					
typedef struct		{
						hud_intro_button_type	button_new,button_load,button_join,
												button_credit,button_setup,button_quit;
					} hud_intro_type;

//
// hud fades
//

typedef struct		{
						int						title_msec,
												map_msec;
					} hud_fade_type;

//
// chat structures
//

typedef struct		{
						char					name[name_str_len],str[chat_str_len];
						d3col					col;
					} chat_line_type;

typedef struct		{
						int						nline,x,y,
												last_add_life_sec,next_life_sec,
												remove_tick;
						char					type_str[chat_str_len];
						bool					type_on;
						chat_line_type			*lines;
					} chat_type;

//
// HUD counts
//

typedef struct		{
						int						bitmap,text,bar,menu,chooser;
					} hud_count_type;
					
//
// main HUD structure
//
 
typedef struct		{
						int						scale_x,scale_y;
						char					click_sound[name_str_len],
												intro_music[name_str_len];
						hud_count_type			count;
						hud_color_type			color;
						hud_progress_type		progress;
						hud_intro_type			intro;
						hud_fade_type			fade;
						hud_bitmap_type			*bitmaps;
						hud_text_type			*texts;
						hud_bar_type			*bars;
						menu_type				*menus;
						chooser_type			*choosers;
						hud_radar_type			radar;
						chat_type				chat;
					} hud_type;
					

