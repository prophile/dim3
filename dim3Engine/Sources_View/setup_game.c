/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Setup

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

#include "interfaces.h"
#include "xmls.h"
#include "video.h"
#include "sounds.h"
#include "inputs.h"

#define setup_pane_display					0
#define setup_pane_graphics					1
#define setup_pane_audio					2
#define setup_pane_mouse					3
#define setup_pane_action					4
#define setup_pane_network					5

#define ctrl_screen_size_id					0
#define ctrl_fsaa_id						1
#define ctrl_lock_fps_refresh_id			2
#define ctrl_anisotropic_id					3
#define ctrl_texture_quality_id				4
#define ctrl_mipmap_id						5
#define ctrl_mipmap_card_id					6
#define ctrl_compression_id					7

#define ctrl_high_quality_lighting_id		10
#define ctrl_bumpmapping_id					11
#define ctrl_gamma_id						12
#define ctrl_diffuselighting_id				13
#define ctrl_raytracelighting_id			14
#define ctrl_specularmapping_id				15
#define ctrl_glowmapping_id					16
#define ctrl_halo_id						17
#define ctrl_fog_id							18
#define ctrl_mark_id						19
#define ctrl_shadow_id						20

#define ctrl_sound_volume_id				30
#define ctrl_music_on_id					31
#define ctrl_music_volume_id				32

#define ctrl_always_run_id					40
#define ctrl_toggle_run_id					41
#define ctrl_invert_look_id					42
#define ctrl_mouse_x_speed_id				43
#define ctrl_mouse_x_accel_id				44
#define ctrl_mouse_y_speed_id				45
#define ctrl_mouse_y_accel_id				46
#define ctrl_mouse_smooth_id				47
#define ctrl_joystick_mode_id				48
#define ctrl_joystick_x_speed_id			49
#define ctrl_joystick_y_speed_id			50

#define ctrl_action_id						60

#define ctrl_tab_id							70

#define setup_action_set_button				80
#define setup_action_clear_button			81

#define setup_game_ok_button				100
#define setup_game_cancel_button			101
#define setup_game_default_button			102

extern int view_search_screen_size_list(int wid,int high);
extern bool view_reset_display(char *err_str);
extern int setup_find_action_in_setup(int action_idx);
extern void intro_open(void);
extern bool setup_xml_reset(void);

extern char					control_names_str[][32];
extern bool					game_loop_quit;

extern render_info_type		render_info;
extern server_type			server;
extern setup_type			setup;
extern hud_type				hud;

int							setup_tab_value,setup_action_scroll_pos,
							setup_key_control_to_action_index_list[ncontrol];

char						setup_screen_size_list[max_screen_size][32],
							setup_anisotropic_mode_list[][32]=anisotropic_mode_setup_list_def,
							setup_texture_quality_mode_list[][32]=texture_quality_mode_setup_list_def,
							setup_mipmap_mode_list[][32]=mipmap_mode_setup_list_def,
							setup_fsaa_mode_list[][32]=setup_fsaa_mode_list_def,
							setup_shadow_mode_list[][32]=setup_shadow_mode_list_def,
							setup_joystick_mode_list[][32]=setup_joystick_mode_list_def,
							setup_control_names[][32]=control_names,
							setup_action_list[ncontrol+1][128];
							
bool						setup_game_start_trigger,setup_action_set_flag,setup_action_set_last_click,
							setup_in_game;
setup_type					setup_backup;

action_display_type			action_display[ncontrol];

/* =======================================================

      Setup Panes
      
======================================================= */

void setup_game_display_pane(void)
{
	int			n,idx,wid,high,
				x,y,control_y_add,separate_y_add,control_y_sz;
	
	control_y_add=element_get_control_high();
	separate_y_add=element_get_separator_high();
	control_y_sz=(control_y_add*7)+separate_y_add;
	
	x=(int)(((float)hud.scale_x)*0.4f);
	y=(hud.scale_y>>1)-(control_y_sz>>1);
	
		// setup screen size list
		
	for (n=0;n!=render_info.nscreen_size;n++) {
		wid=render_info.screen_sizes[n].wid;
		high=render_info.screen_sizes[n].high;
		if (gl_is_size_widescreen(wid,high)) {
			sprintf(setup_screen_size_list[n],"%dx%d Widescreen",wid,high);
		}
		else {
			sprintf(setup_screen_size_list[n],"%dx%d",wid,high);
		}
	}
	idx=view_search_screen_size_list(setup.screen_wid,setup.screen_high);
	if (idx==-1) idx=0;
	
	element_combo_add("Screen Size",(char*)setup_screen_size_list,idx,ctrl_screen_size_id,x,y,TRUE);
	y+=control_y_add;
	element_combo_add("Full-Screen Anti-Aliasing",(char*)setup_fsaa_mode_list,setup.fsaa_mode,ctrl_fsaa_id,x,y,TRUE);
	element_enable(ctrl_fsaa_id,gl_check_fsaa_ok());
	y+=control_y_add;
	element_checkbox_add("Lock FPS to Refresh Rate",setup.lock_fps_refresh,ctrl_lock_fps_refresh_id,x,y,TRUE);
	y+=control_y_add+separate_y_add;
	
	element_combo_add("Anisotropic Filtering",(char*)setup_anisotropic_mode_list,setup.anisotropic_mode,ctrl_anisotropic_id,x,y,TRUE);
	element_enable(ctrl_anisotropic_id,gl_check_texture_anisotropic_filter_ok());
	y+=control_y_add;
	element_combo_add("Texture Quality",(char*)setup_texture_quality_mode_list,setup.texture_quality_mode,ctrl_texture_quality_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Texture Compression",setup.texture_compression,ctrl_compression_id,x,y,TRUE);
	element_enable(ctrl_compression_id,gl_check_texture_compress_ok());
	y+=control_y_add;
	element_combo_add("MipMap Filtering",(char*)setup_mipmap_mode_list,setup.mipmap_mode,ctrl_mipmap_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("MipMapping On Card",setup.mipmap_card_generated,ctrl_mipmap_card_id,x,y,TRUE);
	element_enable(ctrl_mipmap_card_id,gl_check_texture_generate_mipmaps_ok());
}

void setup_game_graphics_pane(void)
{
	int			x,y,control_y_add,separate_y_add,control_y_sz;
	
	control_y_add=element_get_control_high();
	separate_y_add=element_get_separator_high();
	control_y_sz=(control_y_add*10)+(2*separate_y_add);
	
	x=(int)(((float)hud.scale_x)*0.4f);
	y=(hud.scale_y>>1)-(control_y_sz>>1);
	
	element_checkbox_add("High Quality Lighting",setup.high_quality_lighting,ctrl_high_quality_lighting_id,x,y,TRUE);
	element_enable(ctrl_high_quality_lighting_id,(!setup_in_game));
	y+=control_y_add;
	element_checkbox_add("Diffuse Lighting",setup.diffuse_lighting,ctrl_diffuselighting_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Ray-Trace Lighting",setup.ray_trace_lighting,ctrl_raytracelighting_id,x,y,TRUE);
	y+=control_y_add;
	element_slider_add("Gamma",setup.gamma,-0.5f,0.5f,ctrl_gamma_id,x,y,TRUE);
	y+=control_y_add+separate_y_add;

	element_checkbox_add("Bump Mapping",setup.bump_mapping,ctrl_bumpmapping_id,x,y,TRUE);
	element_enable(ctrl_bumpmapping_id,gl_check_bump_ok());
	y+=control_y_add;
	element_checkbox_add("Specular Mapping",setup.specular_mapping,ctrl_specularmapping_id,x,y,TRUE);
	element_enable(ctrl_specularmapping_id,gl_check_specular_ok());
	y+=control_y_add;
	element_checkbox_add("Glow Mapping",setup.glow_mapping,ctrl_glowmapping_id,x,y,TRUE);
	element_enable(ctrl_glowmapping_id,gl_check_glow_ok());
	y+=control_y_add;
	element_checkbox_add("Halos",setup.halo,ctrl_halo_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Decals",setup.mark,ctrl_mark_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Fog",setup.fog,ctrl_fog_id,x,y,TRUE);
	y+=control_y_add+separate_y_add;

	element_combo_add("Shadows",(char*)setup_shadow_mode_list,setup.shadow_mode,ctrl_shadow_id,x,y,TRUE);
	element_enable(ctrl_shadow_id,gl_check_shadow_ok());
}

void setup_game_audio_pane(void)
{
	int			x,y,control_y_add,separate_y_add,control_y_sz;
	
	control_y_add=element_get_control_high();
	separate_y_add=element_get_separator_high();
	control_y_sz=(control_y_add*2)+separate_y_add;
	
	x=(int)(((float)hud.scale_x)*0.4f);
	y=(hud.scale_y>>1)-(control_y_sz>>1);
	
	element_slider_add("Sound Volume",setup.sound_volume,0.0f,1.0f,ctrl_sound_volume_id,x,y,TRUE);
	y+=control_y_add+separate_y_add;

	element_checkbox_add("Music",setup.music_on,ctrl_music_on_id,x,y,TRUE);
	y+=control_y_add;
	element_slider_add("Music Volume",setup.music_volume,0.0f,1.0f,ctrl_music_volume_id,x,y,TRUE);
}

void setup_game_mouse_pane(void)
{
	int			x,y,control_y_add,separate_y_add,control_y_sz;
	
	control_y_add=element_get_control_high();
	separate_y_add=element_get_separator_high();
	control_y_sz=(10*control_y_add)+(3*separate_y_add);
	
	x=(int)(((float)hud.scale_x)*0.4f);
	y=(hud.scale_y>>1)-(control_y_sz>>1);
	
	element_checkbox_add("Always Run",setup.always_run,ctrl_always_run_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Toggle Run",setup.toggle_run,ctrl_toggle_run_id,x,y,TRUE);
	y+=control_y_add+separate_y_add;
	
	element_checkbox_add("Invert Look",setup.invert_look,ctrl_invert_look_id,x,y,TRUE);
	y+=control_y_add;
	element_checkbox_add("Mouse Smoothing",setup.mouse_smooth,ctrl_mouse_smooth_id,x,y,TRUE);
	y+=control_y_add+separate_y_add;

	element_slider_add("Mouse X Speed",setup.mouse_x.speed,setup.mouse_x.speed_min,setup.mouse_x.speed_max,ctrl_mouse_x_speed_id,x,y,TRUE);
	y+=control_y_add;
	element_slider_add("Mouse X Acceleration",setup.mouse_x.acceleration,setup.mouse_x.acceleration_min,setup.mouse_x.acceleration_max,ctrl_mouse_x_accel_id,x,y,TRUE);
	y+=control_y_add;
	element_slider_add("Mouse Y Speed",setup.mouse_y.speed,setup.mouse_y.speed_min,setup.mouse_y.speed_max,ctrl_mouse_y_speed_id,x,y,TRUE);
	y+=control_y_add;
	element_slider_add("Mouse Y Acceleration",setup.mouse_y.acceleration,setup.mouse_y.acceleration_min,setup.mouse_y.acceleration_max,ctrl_mouse_y_accel_id,x,y,TRUE);
	y+=control_y_add+separate_y_add;

	element_combo_add("Joystick Mode",(char*)setup_joystick_mode_list,setup.joystick_mode,ctrl_joystick_mode_id,x,y,TRUE);
	element_enable(ctrl_joystick_mode_id,input_check_joystick_ok());
	y+=control_y_add;
	element_slider_add("Joystick X Speed",setup.joystick_x.speed,setup.joystick_x.speed_min,setup.joystick_x.speed_max,ctrl_joystick_x_speed_id,x,y,TRUE);
	y+=control_y_add;
	element_slider_add("Joystick Y Speed",setup.joystick_y.speed,setup.joystick_y.speed_min,setup.joystick_y.speed_max,ctrl_joystick_y_speed_id,x,y,TRUE);
}

void setup_game_action_pane(void)
{
	int					n,k,list_cnt,idx,x,y,wid,high,padding;
	char				path[1024],path2[1024];
	element_column_type	cols[2];
	
	x=(int)(((float)hud.scale_x)*0.03f);
	y=(int)(((float)hud.scale_y)*0.15f);

	wid=hud.scale_x-(x*2);
	high=(int)(((float)hud.scale_y)*0.75f)-y;
	
		// setup action list
		
	list_cnt=0;
	
	for (n=0;n!=ncontrol;n++) {
	
			// is item showing
			
		if (!action_display[n].show) continue;
		
			// hook action index to control index
			
		setup_key_control_to_action_index_list[list_cnt]=n;
	
			// action
			
		strcpy(setup_action_list[list_cnt],action_display[n].display_name);
		strcat(setup_action_list[list_cnt],"\t");
		
			// attachments
			
		idx=setup_find_action_in_setup(n);
		if (idx!=-1) {
			for (k=0;k!=max_setup_action_attach;k++) {
				if (setup.action_list.actions[idx].attach[k][0]!=0x0) {
					strcat(setup_action_list[list_cnt],setup.action_list.actions[idx].attach[k]);
					strcat(setup_action_list[list_cnt],"  ");
				}
			}
		}
		
		list_cnt++;
	}
	
	setup_action_list[list_cnt][0]=0x0;

	strcpy(cols[0].name,"Action");
	cols[0].percent_size=0.50f;
	strcpy(cols[1].name,"Keys");
	cols[1].percent_size=0.50f;

	element_table_add(cols,(char*)setup_action_list,ctrl_action_id,2,x,y,wid,high,element_table_bitmap_none);
	
		// action buttons
		
	padding=element_get_padding();

	x=hud.scale_x>>1;
	y+=high;
	
	wid=(int)(((float)hud.scale_x)*0.2f);
	high=(int)(((float)hud.scale_x)*0.05f);
	
	y+=(padding+(high/2));
	
	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_action_set","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_action_set_selected","png");
	element_button_add(path,path2,setup_action_set_button,(x-(padding/2)),y,wid,high,element_pos_right,element_pos_center);
	element_enable(setup_action_set_button,FALSE);
	
	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_action_clear","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_action_clear_selected","png");
	element_button_add(path,path2,setup_action_clear_button,(x+(padding/2)),y,wid,high,element_pos_left,element_pos_center);
	element_enable(setup_action_clear_button,FALSE);
}

void setup_game_create_pane(void)
{
	int			x,y,n,wid,high,padding,
				tab_list_wid,tab_pane_high,ntab,stab,pane;
	char		path[1024],path2[1024],
				tab_path[6][1024],tab_path2[6][1024];
	char		tab_list[][32]={"tab_display","tab_graphics","tab_audio","tab_mouse","tab_actions"},
				tab_selected_list[][32]={"tab_display_selected","tab_graphics_selected","tab_audio_selected","tab_mouse_selected","tab_actions_selected"};
							
	element_clear();
	
		// tabs
		
	if (!setup_in_game) {
		stab=0;
		ntab=5;
	}
	else {
		stab=1;
		ntab=4;
	}
	
	for (n=0;n!=ntab;n++) {
		file_paths_data(&setup.file_path_setup,tab_path[n],"Bitmaps/UI_Elements",tab_list[n+stab],"png");
		file_paths_data(&setup.file_path_setup,tab_path2[n],"Bitmaps/UI_Elements",tab_selected_list[n+stab],"png");
	}
	
	padding=element_get_padding();;
	
	wid=hud.scale_x;
	high=(int)(((float)hud.scale_y)*0.075f);
	tab_list_wid=(int)(((float)hud.scale_x)*0.85f);
	tab_pane_high=(int)(((float)hud.scale_y)*0.84f);
	
	element_tab_add((char*)tab_path,(char*)tab_path2,setup_tab_value,ctrl_tab_id,ntab,0,padding,wid,high,tab_list_wid,tab_pane_high);
	
		// buttons
		
	wid=(int)(((float)hud.scale_x)*0.2f);
	high=(int)(((float)hud.scale_x)*0.05f);
	
	x=padding;
	y=hud.scale_y-padding;

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_default","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_default_selected","png");
	element_button_add(path,path2,setup_game_default_button,x,y,wid,high,element_pos_left,element_pos_bottom);

	wid=(int)(((float)hud.scale_x)*0.1f);
	x=hud.scale_x-padding;

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_ok","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_ok_selected","png");
	element_button_add(path,path2,setup_game_ok_button,x,y,wid,high,element_pos_right,element_pos_bottom);

	x=element_get_x_position(setup_game_ok_button)-padding;

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","button_cancel","png");
	file_paths_data(&setup.file_path_setup,path2,"Bitmaps/UI_Elements","button_cancel_selected","png");
	element_button_add(path,path2,setup_game_cancel_button,x,y,wid,high,element_pos_right,element_pos_bottom);
	
		// specific pane controls
		
	pane=element_get_value(ctrl_tab_id);
	if (setup_in_game) pane++;
		
	switch (pane) {
		case setup_pane_display:
			setup_game_display_pane();
			break;
		case setup_pane_graphics:
			setup_game_graphics_pane();
			break;
		case setup_pane_audio:
			setup_game_audio_pane();
			break;
		case setup_pane_mouse:
			setup_game_mouse_pane();
			break;
		case setup_pane_action:
			setup_game_action_pane();
			break;
	}
}

/* =======================================================

      Setup Actions
      
======================================================= */

int setup_game_action_find_free_attach(setup_action_type *action)
{
	int					n;
	
	for (n=0;n!=max_setup_action_attach;n++) {
		if (action->attach[n][0]==0x0) return(n);
	}
	
	return(-1);
}
	
void setup_game_action_enable_buttons(void)
{
	int					ctrl_idx,action_idx;
	setup_action_type	*action;
	
	element_enable(setup_action_set_button,FALSE);
	element_enable(setup_action_clear_button,FALSE);
	
		// get current action
		
	ctrl_idx=element_get_value(ctrl_action_id);
	if (ctrl_idx==-1) return;
	
	action_idx=setup_find_action_in_setup(setup_key_control_to_action_index_list[ctrl_idx]);
	
		// clear enabled if selection
		
	element_enable(setup_action_clear_button,TRUE);
	
		// set enabled if empty attachment
		
	action=&setup.action_list.actions[action_idx];
	if (setup_game_action_find_free_attach(action)!=-1) element_enable(setup_action_set_button,TRUE);
}

void setup_game_action_clear(void)
{
	int					n,ctrl_idx,action_idx;
	setup_action_type	*action;
	
		// clear attachments
		
	ctrl_idx=element_get_value(ctrl_action_id);
	if (ctrl_idx==-1) return;
	
	action_idx=setup_find_action_in_setup(setup_key_control_to_action_index_list[ctrl_idx]);
	action=&setup.action_list.actions[action_idx];
	
	for (n=0;n!=max_setup_action_attach;n++) {
		action->attach[n][0]=0x0;
	}
	
		// rebuild list
		
	setup_game_create_pane();
	element_set_value(ctrl_action_id,ctrl_idx);
	setup_game_action_enable_buttons();
}

void setup_game_action_wait_for_input(void)
{
	int					n,ctrl_idx,action_idx;
	char				name[32];
	bool				no_key_up;
	setup_action_type	*action;
	
		// if we already got input, wait for it to clear
		// before releasing
		
	if (setup_action_set_last_click) {
		if (!input_set_key_wait(name,&no_key_up)) setup_action_set_flag=FALSE;
		return;
	}
	
		// any input?
		
	if (!input_set_key_wait(name,&no_key_up)) return;
	
		// set current action
		
	ctrl_idx=element_get_value(ctrl_action_id);
	if (ctrl_idx==-1) return;
	
	action_idx=setup_find_action_in_setup(setup_key_control_to_action_index_list[ctrl_idx]);
	
	if (action_idx==-1) {					// action doesn't exit, so create new one
		action_idx=setup.action_list.naction;
		strcpy(setup.action_list.actions[action_idx].name,control_names_str[setup_key_control_to_action_index_list[ctrl_idx]]);
		setup.action_list.naction++;
	}
	
	action=&setup.action_list.actions[action_idx];
	
	n=setup_game_action_find_free_attach(action);
	if (n!=-1) strcpy(action->attach[n],name);
	
		// rebuild list
		
	setup_game_create_pane();
	element_set_value(ctrl_action_id,ctrl_idx);
	element_set_scroll_position(ctrl_action_id,setup_action_scroll_pos);
	setup_game_action_enable_buttons();
	
		// got action, if key up needed, wait for it to be released
		
	if (no_key_up) {
		setup_action_set_last_click=FALSE;
		setup_action_set_flag=FALSE;
		return;
	}

	setup_action_set_last_click=TRUE;
}

/* =======================================================

      Setup Game Operations
      
======================================================= */

void setup_game_open(bool in_game)
{
	setup_in_game=in_game;
	
		// setup gui
		
	gui_initialize("Bitmaps/Backgrounds","setup",FALSE);
	
		// waiting for an action flag
		
	setup_action_set_flag=FALSE;
	
		// start with first tab
		
	setup_tab_value=0;
	setup_game_create_pane();
	
		// save setup
		
	memmove(&setup_backup,&setup,sizeof(setup_type));
	
		// server in setup
		
	server.state=gs_setup_game;
}

void setup_game_cancel_close(void)
{
		// restore old settings

	memmove(&setup,&setup_backup,sizeof(setup_type));

		// shutdown and continue

	gui_shutdown();
	
	if (setup_in_game) {
		server.state=gs_running;
		return;
	}
	
	intro_open();
}

void setup_game_save_close(void)
{
	char			err_str[256];
	bool			display_reset;

		// shutdown GUI so display reset works properly

	gui_shutdown();
	
		// fix control and sound changes
		
	setup_to_input();
	
	al_set_volume(setup.sound_volume);
	al_music_set_volume(setup.music_volume);
	al_music_set_state(setup.music_on);
	
		// need shadow reset?
		
	if (setup_in_game) {
		if (setup_backup.shadow_mode!=setup.shadow_mode) {
			gl_shadow_shutdown();
			gl_shadow_initialize(setup.shadow_mode,err_str);
		}
	}
	
		// need to reset SDL?
		// can't do it in game (not possible, anyway, but just in case)
		
	if (!setup_in_game) {
		display_reset=(setup_backup.screen_wid!=setup.screen_wid);
		display_reset=display_reset || (setup_backup.screen_high!=setup.screen_high);
		display_reset=display_reset || (setup_backup.fsaa_mode!=setup.fsaa_mode);
		display_reset=display_reset || (setup_backup.texture_compression!=setup.texture_compression);
		display_reset=display_reset || (setup_backup.anisotropic_mode!=setup.anisotropic_mode);
		display_reset=display_reset || (setup_backup.mipmap_mode!=setup.mipmap_mode);
		display_reset=display_reset || (setup_backup.mipmap_card_generated!=setup.mipmap_card_generated);
	
		if (display_reset) {
			if (!view_reset_display(err_str)) {
				game_loop_quit=TRUE;			// fatal error resetting display
				return;
			}

		}
	}
	
		// write setup
		
	setup_xml_write();

		// continue with app

	if (setup_in_game) {
		server.state=gs_running;
		return;
	}
	
	intro_open();
}

void setup_game_default(void)
{
	setup_xml_reset();
	setup_game_create_pane();
}

/* =======================================================

      Setup Game Triggers
      
======================================================= */

void setup_game_trigger_clear(void)
{
	setup_game_start_trigger=FALSE;
}

void setup_game_trigger_check(void)
{
	if (setup_game_start_trigger) setup_game_open(TRUE);			// trigger only checked when game running
}	

void setup_game_trigger_set(void)
{
	setup_game_start_trigger=TRUE;
}

/* =======================================================

      Setup Game Input
      
======================================================= */

void setup_game_handle_click(int id)
{
	int			idx;
	
	switch (id) {
	
			// tab
			
		case ctrl_tab_id:
			setup_tab_value=element_get_value(ctrl_tab_id);
			setup_game_create_pane();
			return;
			
			// buttons
			
		case setup_game_ok_button:
			setup_game_save_close();
			return;
			
		case setup_game_cancel_button:
			setup_game_cancel_close();
			return;

		case setup_game_default_button:
			setup_game_default();
			return;
			
		case setup_action_set_button:
			setup_action_scroll_pos=element_get_scroll_position(ctrl_action_id);
			setup_action_set_last_click=FALSE;
			setup_action_set_flag=TRUE;
			input_clear();
			input_set_key_start();
			return;
			
		case setup_action_clear_button:
			setup_action_scroll_pos=element_get_scroll_position(ctrl_action_id);
			setup_game_action_clear();
			element_set_scroll_position(ctrl_action_id,setup_action_scroll_pos);
			return;
	
			// graphics pane
			
		case ctrl_screen_size_id:
			idx=element_get_value(ctrl_screen_size_id);
			setup.screen_wid=render_info.screen_sizes[idx].wid;
			setup.screen_high=render_info.screen_sizes[idx].high;
			break;
			
		case ctrl_lock_fps_refresh_id:
			setup.lock_fps_refresh=element_get_value(ctrl_lock_fps_refresh_id);
			break;
			
		case ctrl_compression_id:
			setup.texture_compression=element_get_value(ctrl_compression_id);
			break;
			
		case ctrl_anisotropic_id:
			setup.anisotropic_mode=element_get_value(ctrl_anisotropic_id);
			break;
			
		case ctrl_texture_quality_id:
			setup.texture_quality_mode=element_get_value(ctrl_texture_quality_id);
			break;

		case ctrl_mipmap_id:
			setup.mipmap_mode=element_get_value(ctrl_mipmap_id);
			break;

		case ctrl_mipmap_card_id:
			setup.mipmap_card_generated=element_get_value(ctrl_mipmap_card_id);
			break;

		case ctrl_fsaa_id:
			setup.fsaa_mode=element_get_value(ctrl_fsaa_id);
			break;

		case ctrl_high_quality_lighting_id:
			setup.high_quality_lighting=element_get_value(ctrl_high_quality_lighting_id);
			break;
			
		case ctrl_bumpmapping_id:
			setup.bump_mapping=element_get_value(ctrl_bumpmapping_id);
			break;
			
		case ctrl_gamma_id:
			setup.gamma=element_get_slider_value(ctrl_gamma_id);
			break;

		case ctrl_diffuselighting_id:
			setup.diffuse_lighting=element_get_value(ctrl_diffuselighting_id);
			break;
			
		case ctrl_raytracelighting_id:
			setup.ray_trace_lighting=element_get_value(ctrl_raytracelighting_id);
			break;
			
		case ctrl_specularmapping_id:
			setup.specular_mapping=element_get_value(ctrl_specularmapping_id);
			break;

		case ctrl_glowmapping_id:
			setup.glow_mapping=element_get_value(ctrl_glowmapping_id);
			break;
			
		case ctrl_halo_id:
			setup.halo=element_get_value(ctrl_halo_id);
			break;
			
		case ctrl_fog_id:
			setup.fog=element_get_value(ctrl_fog_id);
			break;
			
		case ctrl_mark_id:
			setup.mark=element_get_value(ctrl_mark_id);
			break;
			
		case ctrl_shadow_id:
			setup.shadow_mode=element_get_value(ctrl_shadow_id);
			break;
			
			// audio pane
			
		case ctrl_sound_volume_id:
			setup.sound_volume=element_get_slider_value(ctrl_sound_volume_id);
			break;

		case ctrl_music_on_id:
			setup.music_on=element_get_value(ctrl_music_on_id);
			break;
			
		case ctrl_music_volume_id:
			setup.music_volume=element_get_slider_value(ctrl_music_volume_id);
			break;
			
			// mouse pane
			
		case ctrl_always_run_id:
			setup.always_run=element_get_value(ctrl_always_run_id);
			break;
			
		case ctrl_toggle_run_id:
			setup.toggle_run=element_get_value(ctrl_toggle_run_id);
			break;
			
		case ctrl_invert_look_id:
			setup.invert_look=element_get_value(ctrl_invert_look_id);
			break;
			
		case ctrl_mouse_smooth_id:
			setup.mouse_smooth=element_get_value(ctrl_mouse_smooth_id);
			break;
			
		case ctrl_mouse_x_speed_id:
			setup.mouse_x.speed=element_get_slider_value(ctrl_mouse_x_speed_id);
			break;
			
		case ctrl_mouse_x_accel_id:
			setup.mouse_x.acceleration=element_get_slider_value(ctrl_mouse_x_accel_id);
			break;
			
		case ctrl_mouse_y_speed_id:
			setup.mouse_y.speed=element_get_slider_value(ctrl_mouse_y_speed_id);
			break;

		case ctrl_mouse_y_accel_id:
			setup.mouse_y.acceleration=element_get_slider_value(ctrl_mouse_y_accel_id);
			break;

		case ctrl_joystick_mode_id:
			setup.joystick_mode=element_get_value(ctrl_joystick_mode_id);
			break;

		case ctrl_joystick_x_speed_id:
			setup.joystick_x.speed=element_get_slider_value(ctrl_joystick_x_speed_id);
			break;

		case ctrl_joystick_y_speed_id:
			setup.joystick_y.speed=element_get_slider_value(ctrl_joystick_y_speed_id);
			break;
		
			// action pane
			
		case ctrl_action_id:
			setup_game_action_enable_buttons();
			break;
			
	}
}

void setup_game_keyboard(void)
{
	int			id;

	id=gui_keyboard();
	if (id==-1) return;

	hud_click();

	setup_game_handle_click(id);
}

void setup_game_click(void)
{
	int			id;
	
	id=gui_click();
	if (id==-1) return;
	
	hud_click();

	setup_game_handle_click(id);
}
	
/* =======================================================

      Run Game Setup
      
======================================================= */

void setup_game_run(void)
{
		// if input wait is on
		
	if (setup_action_set_flag) {
		gui_draw_message("Press a Key or Click a Button");
		input_event_pump();
		setup_game_action_wait_for_input();
		return;
	}
	
		// draw UI
		
	gui_draw(1.0f,TRUE);

		// keyboard and clicking

	setup_game_keyboard();
	setup_game_click();
}

