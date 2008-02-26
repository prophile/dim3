/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: File Menu Routines

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
#include "walk_view.h"

extern WindowRef				mainwind;

extern int						cr,cx,cz,cy,magnify_factor;
extern bool						map_opened,primitive_reform_ok;

extern file_path_setup_type		file_path_setup;
extern setup_type				setup;

map_type						map;

/* =======================================================

      Open Windows
            
======================================================= */

void open_windows(void)
{
	main_wind_open();
}

void redraw_windows(void)
{
    cr=0;
	
        // starting room
    
    if (map.nportal==0) {
		cr=map_portal_create(&map,(map_x_size/2),(map_z_size/2),40,40);
	}
    
        // center in first room
        
	select_clear();
	main_wind_center_position_in_map();

        // some defaults
        
	magnify_factor=magnify_factor_min+((magnify_factor_max-magnify_factor_min)/3);
	main_wind_set_view(vw_3_panel);
	main_wind_set_perspective(ps_perspective);
	
		// redraw
	
    main_wind_tool_default();
	
	menu_fix_enable();
	main_wind_tool_fix_enable();
	
    SelectWindow(mainwind);
    
	main_wind_draw();
	texture_palette_reset();
}

void close_windows(void)
{
    main_wind_close();
	menu_fix_enable();
}

/* =======================================================

      New Map utilities
      
======================================================= */

bool create_course_script(char *file_name)
{
	int				sz;
	char			*data,path[1024];
	FILE			*file;
	struct stat		sb;
	
		// read the file
	
	file_paths_app(&file_path_setup,path,"Contents/Resources/Defaults","Course","js");
		
	if (stat(path,&sb)!=0) return(FALSE);
	sz=sb.st_size;
		
	file=fopen(path,"r");
	if (file==NULL) return(FALSE);
    
	data=(char*)valloc(sz);
    if (data==NULL) {
        fclose(file);
        return(FALSE);
    }
	
	fread(data,1,sz,file);
	fclose(file);
	
		// write the file
		
	file_paths_data_default(&file_path_setup,path,"Scripts/Courses",file_name,"js");
		
	file=fopen(path,"w");
	if (file==NULL) {
		free(data);
		return(FALSE);
	}
	
	fwrite(data,1,sz,file);
	fclose(file);
	
	return(TRUE);
}

bool create_first_portal(void)
{
	int				x,y,z;
	char			bitmap_name[256],path[1024],err_str[256];
	unsigned char	p_err_str[256];
	spot_type		*spot;
	map_light_type	*lit;

		// get first texture
		
	if (!dialog_file_open_run("BitmapOpen","Bitmaps/Textures","png",bitmap_name)) return(FALSE);
		
	file_paths_data(&file_path_setup,path,"Bitmaps/Textures",bitmap_name,"png");
	if (!bitmap_check(path,err_str)) {
		CopyCStringToPascal(err_str,p_err_str);
		StandardAlert(0,"\pTexture Error",p_err_str,NULL,NULL);
		return(FALSE);
	}

	map_add_texture_frame(&map,0,bitmap_name,"","","");
	
		// get first portal
		
	cx=map_x_size>>1;
	cz=map_z_size>>1;
	cy=200*map_enlarge;
	
	map.nportal=0;
	if (!portal_new()) return(FALSE);

	map_portal_calculate_center_floor(&map,0,&x,&y,&z);
	if (y==0) y=200;
	
		// put player spot in first portal
		
	spot=&map.spots[0];
	map.nspot=1;
    
	spot->pos.rn=0;
	spot->pos.x=x;
	spot->pos.z=z;
	spot->pos.y=y;
	spot->ang.y=0;
	strcpy(spot->name,"Start");
	strcpy(spot->type,"Player");
	strcpy(spot->script,"Player");
	spot->display_model[0]=0x0;
	spot->params[0]=0x0;
	
		// and create a single light
		
	lit=&map.lights[0];
	map.nlight=1;
	
	lit->pos.rn=0;
	lit->pos.x=x;
	lit->pos.z=z+4;
	lit->pos.y=y-10;
	lit->type=lt_normal;
	lit->col.r=lit->col.g=lit->col.b=1;
	lit->intensity=100;
	lit->on=TRUE;
	
	return(TRUE);
}

/* =======================================================

      New Map
      
======================================================= */

bool file_new_map(bool create_initial_portal)
{
	char		file_name[256];
	
		// get the map name
		
	strcpy(file_name,"NewMap");
	if (!dialog_file_new_run("Create a New Map",file_name)) return(FALSE);
	
		// set the map paths
		
	map_setup(&file_path_setup,setup.anisotropic_mode,setup.texture_quality_mode,setup.mipmap_mode,setup.mipmap_card_generated,setup.texture_compression);
	
		// copy the course script
		
	if (!create_course_script(file_name)) {
		StandardAlert(kAlertCautionAlert,"\pdim3 Editor could not create map script","\pThe disk might be locked or a folder might be missing.\n\nIf you are running dim3 directly from the DMG file, then you need to move the files to your harddrive (DMGs are read-only).",NULL,NULL);
		return(FALSE);
	}
	
		// start the map
		
    map_new(&map,file_name);
	
	map_opened=TRUE;
    
    open_windows();
	main_wind_set_title(file_name);
	
		// create first portal
		
	if (!create_initial_portal) return(TRUE);
	
	if (!create_first_portal()) {
		map_opened=FALSE;
		close_windows();
		map_close(&map);
		return(FALSE);
	}
	
		// start models
		
	walk_view_models_start();
		
		// redraw the window
    
	redraw_windows();
	main_wind_tool_fill_group_combo();

	primitive_reform_ok=FALSE;
	
	return(TRUE);
}

/* =======================================================

      Open Map
      
======================================================= */

bool file_open_map(void)
{
	char			file_name[256];
	bool			ok;
	
		// open the map
		
	if (!dialog_file_open_run("MapOpen","Maps","xml",file_name)) return(FALSE);
	
		// open the map
		
	SetCursor(*GetCursor(watchCursor));
	
	open_windows();
	main_wind_set_title(file_name);
	
	map_setup(&file_path_setup,setup.anisotropic_mode,setup.texture_quality_mode,setup.mipmap_mode,setup.mipmap_card_generated,setup.texture_compression);
	
	ok=map_open(&map,file_name,TRUE,FALSE,FALSE);
 
    InitCursor();
	
	if (!ok) {
		close_windows();
		return(FALSE);
	}
	
		// clear selection
		
	select_clear();
	clipboard_clear();
	
		// fill in the groups
		
	main_wind_tool_fill_group_combo();
	
		// start models
		
	walk_view_models_start();
	walk_view_models_reset();
	
		// set flags as opened
		
	map_opened=TRUE;
	primitive_reform_ok=FALSE;
	
	redraw_windows();
	
	return(TRUE);
}

/* =======================================================

      Save Map
      
======================================================= */

void file_save_map(void)
{
	bool			ok;
	
    SetCursor(*GetCursor(watchCursor)); 
	
	node_path_rebuild();		// force rebuild on node paths
	ok=map_save(&map);			// save map
	
	InitCursor();
	
	if (!ok) StandardAlert(kAlertCautionAlert,"\pdim3 Editor could not save map","\pThe disk might be locked or a folder might be missing.\n\nIf you are running dim3 directly from the DMG file, then you need to move the files to your harddrive (DMGs are read-only).",NULL,NULL);
}

/* =======================================================

      Close Map
      
======================================================= */

void file_close_map(void)
{
	if (!map_opened) return;
	
	InitCursor();
	map_close(&map);
	
	map_opened=FALSE;
	
	walk_view_models_close();
	
    close_windows();
}

