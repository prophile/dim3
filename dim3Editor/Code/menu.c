/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Menus

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
#include "dialog.h"
#include "common_view.h"
#include "portal_view.h"
#include "walk_view.h"
#include "import.h"

extern int				cr;
extern bool				done,map_opened;

extern map_type			map;

/* =======================================================

      Misc. Dialog Routines
      
======================================================= */

void menu_start(void)
{
	IBNibRef				nib;
	CFStringRef				cf_str;
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"dim3 Editor",kCFStringEncodingMacRoman);
	CreateNibReference(cf_str,&nib);
	CFRelease(cf_str);
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"MenuBar",kCFStringEncodingMacRoman);
	SetMenuBarFromNib(nib,cf_str);
	CFRelease(cf_str);

	DisposeNibReference(nib);

	menu_fix_enable();
	undo_clear();
}

/* =======================================================

      Mainline Menu Enable/Disable
      
======================================================= */

void menu_fix_enable(void)
{
	if (!map_opened) {
		EnableMenuItem(GetMenuHandle(app_menu_file),1);
		EnableMenuItem(GetMenuHandle(app_menu_file),2);
		DisableMenuItem(GetMenuHandle(app_menu_file),3);
		DisableMenuItem(GetMenuHandle(app_menu_file),5);
		DisableMenuItem(GetMenuHandle(app_menu_file),6);
		DisableMenuItem(GetMenuHandle(app_menu_file),7);
		DisableMenuItem(GetMenuHandle(app_menu_file),9);
	
		DisableMenuItem(GetMenuHandle(app_menu_edit),0);
		DisableMenuItem(GetMenuHandle(app_menu_view),0);
		DisableMenuItem(GetMenuHandle(app_menu_map),0);
		DisableMenuItem(GetMenuHandle(app_menu_portal),0);
		DisableMenuItem(GetMenuHandle(app_menu_pieces),0);
		DisableMenuItem(GetMenuHandle(app_menu_groups),0);
	}
	else {
	
			// file menu
			
		DisableMenuItem(GetMenuHandle(app_menu_file),1);
		DisableMenuItem(GetMenuHandle(app_menu_file),2);
		EnableMenuItem(GetMenuHandle(app_menu_file),3);
		EnableMenuItem(GetMenuHandle(app_menu_file),5);
		EnableMenuItem(GetMenuHandle(app_menu_file),6);
		EnableMenuItem(GetMenuHandle(app_menu_file),7);
		EnableMenuItem(GetMenuHandle(app_menu_file),9);
	
			// other menus
			
		EnableMenuItem(GetMenuHandle(app_menu_edit),0);
		EnableMenuItem(GetMenuHandle(app_menu_view),0);
		EnableMenuItem(GetMenuHandle(app_menu_map),0);
		EnableMenuItem(GetMenuHandle(app_menu_portal),0);
		EnableMenuItem(GetMenuHandle(app_menu_groups),0);
        
            // pieces menu
        
		EnableMenuItem(GetMenuHandle(app_menu_pieces),0);
		
		if (select_has_type(mesh_piece)) {
			EnableMenuItem(GetMenuHandle(app_menu_pieces),1);
			EnableMenuItem(GetMenuHandle(app_menu_pieces),2);
			EnableMenuItem(GetMenuHandle(app_menu_pieces),3);
			EnableMenuItem(GetMenuHandle(app_menu_pieces),5);
		}
		else {
			DisableMenuItem(GetMenuHandle(app_menu_pieces),1);
			DisableMenuItem(GetMenuHandle(app_menu_pieces),2);
			DisableMenuItem(GetMenuHandle(app_menu_pieces),3);
			DisableMenuItem(GetMenuHandle(app_menu_pieces),5);
		}
		
        if (select_count()!=0) {
			EnableMenuItem(GetMenuHandle(app_menu_pieces),7);
			EnableMenuItem(GetMenuHandle(app_menu_pieces),8);
			EnableMenuItem(GetMenuHandle(app_menu_pieces),10);
			EnableMenuItem(GetMenuHandle(app_menu_pieces),11);
        }
        else {
			DisableMenuItem(GetMenuHandle(app_menu_pieces),7);
			DisableMenuItem(GetMenuHandle(app_menu_pieces),8);
			DisableMenuItem(GetMenuHandle(app_menu_pieces),10);
			DisableMenuItem(GetMenuHandle(app_menu_pieces),11);
        }
	}
	
	DrawMenuBar();
}

/* =======================================================

      Set View Check
      
======================================================= */

void menu_set_view_check(int view)
{
	switch (view) {
		case vw_3_panel:
			CheckMenuItem(GetMenuHandle(app_menu_view),1,TRUE);
			CheckMenuItem(GetMenuHandle(app_menu_view),2,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),3,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),4,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),5,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),6,FALSE);
			break;
		case vw_4_panel:
			CheckMenuItem(GetMenuHandle(app_menu_view),1,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),2,TRUE);
			CheckMenuItem(GetMenuHandle(app_menu_view),3,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),4,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),5,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),6,FALSE);
			break;
		case vw_portal_only:
			CheckMenuItem(GetMenuHandle(app_menu_view),1,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),2,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),3,TRUE);
			CheckMenuItem(GetMenuHandle(app_menu_view),4,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),5,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),6,FALSE);
			break;
		case vw_site_path_only:
			CheckMenuItem(GetMenuHandle(app_menu_view),1,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),2,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),3,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),4,TRUE);
			CheckMenuItem(GetMenuHandle(app_menu_view),5,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),6,FALSE);
			break;
		case vw_top_only:
			CheckMenuItem(GetMenuHandle(app_menu_view),1,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),2,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),3,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),4,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),5,TRUE);
			CheckMenuItem(GetMenuHandle(app_menu_view),6,FALSE);
			break;
		case vw_forward_only:
			CheckMenuItem(GetMenuHandle(app_menu_view),1,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),2,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),3,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),4,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),5,FALSE);
			CheckMenuItem(GetMenuHandle(app_menu_view),6,TRUE);
			break;
	}
}

void menu_set_perspective_check(int perspective)
{
	if (perspective==ps_perspective) {
		CheckMenuItem(GetMenuHandle(app_menu_view),8,TRUE);
		CheckMenuItem(GetMenuHandle(app_menu_view),9,FALSE);
	}
	else {
		CheckMenuItem(GetMenuHandle(app_menu_view),8,FALSE);
		CheckMenuItem(GetMenuHandle(app_menu_view),9,TRUE);
	}
}

/* =======================================================

      Alert
      
======================================================= */

void menu_save_changes_dialog(void)
{
	short					hit;
	AlertStdAlertParamRec	alert_param;
	
	memset(&alert_param,0x0,sizeof(AlertStdAlertParamRec));
	alert_param.defaultText="\pYes";
	alert_param.cancelText="\pNo";
	alert_param.defaultButton=kAlertStdAlertOKButton;
	alert_param.position=kWindowDefaultPosition;

	StandardAlert(0,"\pSave Changes?","\pDo you want to save the changes to this map?",&alert_param,&hit);
	
	if (hit==kAlertStdAlertOKButton) file_save_map();
}

void menu_auto_generate_paths_dialog(void)
{
	short					hit;
	AlertStdAlertParamRec	alert_param;
	
	memset(&alert_param,0x0,sizeof(AlertStdAlertParamRec));
	alert_param.defaultText="\pYes";
	alert_param.cancelText="\pNo";
	alert_param.defaultButton=kAlertStdAlertOKButton;
	alert_param.position=kWindowDefaultPosition;

	StandardAlert(0,"\pAuto-Generate Sight Paths?","\pThis will replace all portal sight paths with auto-generated ones.  These paths will probably be incomplete and not-optimal and should be adjusted by hand.",&alert_param,&hit);
	
	if (hit==kAlertStdAlertOKButton) map_portal_sight_generate_paths(&map,FALSE);
}

bool menu_delete_portal_dialog(void)
{
	short					hit;
	AlertStdAlertParamRec	alert_param;
	
	memset(&alert_param,0x0,sizeof(AlertStdAlertParamRec));
	alert_param.defaultText="\pYes";
	alert_param.cancelText="\pNo";
	alert_param.defaultButton=kAlertStdAlertOKButton;
	alert_param.position=kWindowDefaultPosition;

	StandardAlert(0,"\pDelete Portal?","\pAre you sure you want to delete this portal?",&alert_param,&hit);
	
	return(hit==kAlertStdAlertOKButton);
}

/* =======================================================

      Menu Events
      
======================================================= */

OSStatus menu_event_callback(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	int				index;
	HICommand		cmd;
	
	GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
	
	switch (cmd.commandID) {
		
        case kCommandAbout:
            dialog_about_run();
            return(noErr);
            
		case kHICommandQuit:
			if (map_opened) {
				menu_save_changes_dialog();
				file_close_map();
			}
			QuitApplicationEventLoop();
			return(noErr);
	
			// file menu
			
		case kCommandNew:
			file_new_map(TRUE);
			undo_clear();
			return(noErr);
			
		case kCommandOpen:
			file_open_map();
			undo_clear();
			return(noErr);
			
		case kCommandClose:
			if (map_opened) {
				menu_save_changes_dialog();
				file_close_map();
			}
			undo_clear();
			return(noErr);
			
		case kCommandImportOBJ:				// supergumba
		//	select_clear();
		//	import_obj();
		//	undo_clear();
			return(noErr);
			
		case kCommandImportHeightMap:		// supergumba
		//	select_clear();
		//	import_height_map();
		//	undo_clear();
			return(noErr);
			
		case kCommandAutoGenerateMap:
			select_clear();
			import_auto_generate();
			main_wind_tool_fill_group_combo();
			undo_clear();
			return(noErr);
			
		case kCommandSave:
			file_save_map();
			return(noErr);
			
			// edit menu
			
		case kCommandUndo:
			undo_run();
			return(noErr);
			
			// view menu
			
		case kCommandView3Panel:
			main_wind_set_view(vw_3_panel);
			main_wind_draw();
			return(noErr);
			
		case kCommandView4Panel:
			main_wind_set_view(vw_4_panel);
			main_wind_draw();
			return(noErr);
			
		case kCommandViewPortal:
			main_wind_set_view(vw_portal_only);
			main_wind_draw();
			return(noErr);
		
		case kCommandViewSitePath:
			main_wind_set_view(vw_site_path_only);
			main_wind_draw();
			return(noErr);
			
		case kCommandViewTopOnly:
			main_wind_set_view(vw_top_only);
			main_wind_draw();
			return(noErr);
			
		case kCommandViewForwardOnly:
			main_wind_set_view(vw_forward_only);
			main_wind_draw();
			return(noErr);

		case kCommandViewPerspective:
			main_wind_set_perspective(ps_perspective);
			main_wind_draw();
			return(noErr);
			
		case kCommandViewOrtho:
			main_wind_set_perspective(ps_ortho);
			main_wind_draw();
			return(noErr);
			
			// map menu

		case kCommandMapSettings:
			dialog_map_settings_run();
			return(noErr);
			
		case kCommandMapCounts:
			dialog_map_counts_run();
			return(noErr);
			
		case kCommandMapOptimizations:
			dialog_map_optimizations_run();
			return(noErr);
			
		case kCommandMapMedia:
			dialog_map_media_run();
			return(noErr);
			
		case kCommandAmbientLightSoundSettings:
			dialog_map_ambient_light_sound_settings_run();
			return(noErr);
            
		case kCommandSkySettings:
			dialog_background_sky_setting_run();
			return(noErr);
			
		case kCommandFogSettings:
			dialog_map_fog_settings_run();
			return(noErr);
			
		case kCommandRainSettings:
			dialog_map_rain_settings_run();
			return(noErr);
			
		case kCommandAutoGenerateSightPaths:
			menu_auto_generate_paths_dialog();
			return(noErr);
			
		case kCommandMapRaiseY:
			portal_all_y_change(-(map_enlarge*10));
			main_wind_draw();
			return(noErr);
			
		case kCommandMapLowerY:
			portal_all_y_change(map_enlarge*10);
			main_wind_draw();
			return(noErr);
			
		case kCommandRun:
			launch_engine();
			return(noErr);
			
			// portals menu
			
		case kCommandPortalSettings:
			dialog_portal_settings_run(&map.portals[cr]);
			return(noErr);
            
        case kCommandPortalDuplicate:
            portal_duplicate();
            return(noErr);
            
        case kCommandPortalDelete:
			if (menu_delete_portal_dialog()) portal_delete();
            return(noErr);
            
		case kCommandPortalGoToTop:
			walk_view_portal_go_to_top();
			return(noErr);
			
		case kCommandPortalGoToBottom:
			walk_view_portal_go_to_bottom();
			return(noErr);
			
		case kCommandPortalGoToSelection:
			walk_view_portal_go_to_selection();
			return(noErr);
			
		case kCommandClearPath:
            map_portal_sight_clear(&map,cr);
            main_wind_draw();
			return(noErr);
            
			// piece menu

		case kCommandPieceCombineMeshes:
			piece_combine_mesh(cr);
			main_wind_draw();
			return(noErr);
			
		case kCommandPieceTesselatePolygon:
			piece_tesselate();
			return(noErr);
			
		case kCommandPieceAddLibrary:
		//	primitive_save();
			return(noErr);
			
		case kCommandPieceDuplicate:
			piece_duplicate();
			return(noErr);
            
		case kCommandPieceDelete:
			piece_delete();
			return(noErr);
			
		case kCommandPieceMove:
			index=dialog_piece_move_to_portal_run();
			if (index==-1) return(noErr);
			piece_move_to_portal(index);
			return(noErr);
			
		case kCommandPieceDuplicateMove:
			index=dialog_piece_move_to_portal_run();
			if (index==-1) return(noErr);
			piece_duplicate();
			piece_move_to_portal(index);
			return(noErr);
			
			// group menu
			
		case kCommandGroupAdd:
			group_add();
			return(noErr);
			
		case kCommandGroupDelete:
			group_delete();
			return(noErr);
			
		case kCommandGroupClear:
			group_clear();
			return(noErr);
			
		case kCommandGroupMovements:
			dialog_map_movements_run();
			return(noErr);
			
			// portal transform menu

        case kCommandPortalResize:
            portal_resize();
			undo_clear();
            return(noErr);

		case kCommandPortalFlipHorizontal:
			portal_flip_horizontal();
			undo_clear();
			return(noErr);
			
		case kCommandPortalFlipVertical:
			portal_flip_vertical();
			undo_clear();
			return(noErr);
			
		case kCommandPortalRotate:
			portal_rotate();
			undo_clear();
			return(noErr);
            
        case kCommandPortalRaiseY:
            portal_y_change(-map_enlarge);
			main_wind_draw();
			undo_clear();
            return(noErr);
            
        case kCommandPortalLowerY:
            portal_y_change(map_enlarge);
			main_wind_draw();
			undo_clear();
            return(noErr);
			
        case kCommandPortalSplitHorizontal:
            portal_split(cr,FALSE);
			main_wind_draw();
			undo_clear();
            return(noErr);
			
        case kCommandPortalSplitVertical:
            portal_split(cr,TRUE);
			main_wind_draw();
 			undo_clear();
			return(noErr);
			            
			// piece transform menu

		case kCommandPieceFlipX:
			piece_flip(TRUE,FALSE,TRUE);
			return(noErr);
			
		case kCommandPieceFlipY:
			piece_flip(FALSE,TRUE,FALSE);
			return(noErr);
			
		case kCommandPieceFlipZ:
			piece_flip(FALSE,FALSE,TRUE);
			return(noErr);
			
		case kCommandPieceRotateX:
			piece_rotate(90.0f,0.0f,0.0f);
			return(noErr);
			
		case kCommandPieceRotateY:
			piece_rotate(0.0f,90.0f,0.0f);
			return(noErr);
			
		case kCommandPieceRotateZ:
			piece_rotate(0.0f,0.0f,90.0f);
			return(noErr);

		case kCommandPieceFreeRotate:
			piece_free_rotate();
			return(noErr);

		case kCommandPieceRaiseY:
			select_move(cr,0,0,map_enlarge);
			main_wind_draw();
			return(noErr);
            
		case kCommandPieceLowerY:
			select_move(cr,0,0,-map_enlarge);
            main_wind_draw();
 			return(noErr);
			
			// face menu
			
		case kCommandFaceDelete:
			return(noErr);
			
		case kCommandFaceHole1:
			return(noErr);

	}
	
	return(eventNotHandledErr);
}

