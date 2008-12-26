/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Menus

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "model.h"
#include "window.h"
#include "menu.h"
#include "dialog.h"
#include "import.h"

char							filename[256];

extern int						draw_type,cur_mesh,cur_bone,cur_pose,cur_animate,
								shift_x,shift_y,magnify_z,drag_bone_mode,
								play_animate_blend_idx[max_model_blend_animation];
extern float					ang_y,ang_x;
extern bool						done,fileopen,model_box_on,model_bump_on,model_normal_on,
								model_bone_drag_on,play_animate,play_animate_blend,model_show_first_mesh;
							
extern model_type				model;
extern model_draw_setup			draw_setup;

extern WindowRef				model_wind;

extern file_path_setup_type		file_path_setup;

/* =======================================================

      Setup Menus
      
======================================================= */

void menu_start(void)
{
	IBNibRef				nib;
	CFStringRef				cf_str;
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"dim3 Animator",kCFStringEncodingMacRoman);
	CreateNibReference(cf_str,&nib);
	CFRelease(cf_str);
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"MenuBar",kCFStringEncodingMacRoman);
	SetMenuBarFromNib(nib,cf_str);
	CFRelease(cf_str);
	
	DisposeNibReference(nib);
	
	draw_type=dt_model;
	model_show_first_mesh=FALSE;
    model_box_on=TRUE;
	model_bump_on=TRUE;
	model_normal_on=FALSE;
	model_bone_drag_on=FALSE;
	drag_bone_mode=drag_bone_mode_rotate;
	
	fix_menus();
}

/* =======================================================

      App Windows
      
======================================================= */

void windows_start(void)
{
	int				n;
	
	cur_mesh=0;
	cur_bone=-1;
	cur_pose=-1;
	
	play_animate=FALSE;
	play_animate_blend=FALSE;
	
	for (n=0;n!=max_model_blend_animation;n++) {
		play_animate_blend_idx[n]=-1;
	}
	
	model_wind_open();
	model_wind_reset_tools();

	SelectWindow(model_wind);
}

void windows_end(void)
{
	model_wind_close();
}

/* =======================================================

      XML Creation/Saving
      
======================================================= */

bool create_binary(void)
{
	char			base_path[1024],path[1024];
	
	SetCursor(*GetCursor(watchCursor));
	
		// base path
		
	file_paths_data_default(&file_path_setup,base_path,"Models",NULL,NULL);
    
        // create the new folders
		
	strcat(base_path,"/");
	strcat(base_path,filename);
	
	mkdir(base_path,S_IRWXU|S_IRWXG|S_IRWXO);
	
	sprintf(path,"%s/Bitmaps",base_path);
	mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
	
	sprintf(path,"%s/Bitmaps/Textures",base_path);
	mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
	
	sprintf(path,"%s/Bitmaps/Textures_dot3",base_path);
	mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
	
	sprintf(path,"%s/Bitmaps/Textures_Height",base_path);
	mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
	
	sprintf(path,"%s/Bitmaps/Textures_Specular",base_path);
	mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
	
	sprintf(path,"%s/Bitmaps/Textures_Glow",base_path);
	mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);

	sprintf(path,"%s/Shaders",base_path);
	mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);

        // write the binary
	
	model_save(&model);
	
	InitCursor();
    
	return(TRUE);
}

bool save_binary(void)
{
	bool				ok;
	
	SetCursor(*GetCursor(watchCursor));
	ok=model_save(&model);
	InitCursor();
	
	if (!ok) StandardAlert(kAlertCautionAlert,"\pdim3 Animator could not save model.","\pThe disk might be locked or a folder might be missing.\n\nIf you are running dim3 directly from the DMG file, then you need to move the files to your harddrive (DMGs are read-only).",NULL,NULL);
	
	return(ok);
}

/* =======================================================

      Open Model XML
      
======================================================= */

void reset_model_open(void)
{
    ang_y=180;
	ang_x=0;
	shift_x=shift_y=0;
	magnify_z=0;
	
	vertex_clear_sel_mask(cur_mesh);
	vertex_clear_hide_mask(cur_mesh);
	
	cur_mesh=0;
	cur_bone=-1;
	
	cur_pose=-1;
	if (model.npose!=0) cur_pose=0;
	
	cur_animate=-1;
	if (model.nanimate!=0) cur_animate=0;
	
    draw_model_wind_pose(&model,cur_mesh,cur_pose);
    texture_palette_draw();
	mesh_palette_draw();
    info_palette_draw();
	reset_vertex_tab();
    reset_pose_list();
	reset_bone_list();
    reset_animate_list();
	
	undo_clear();
	
	fix_menus();
}

void new_model_xml(void)
{
	char		fname[256];
	
		// get name
	
	strcpy(fname,"NewModel");
	
	if (!dialog_file_new_run("New Model",fname)) return;
	
	strcpy(filename,fname);
	
		// create model

	SetCursor(*GetCursor(watchCursor));
		
	model_setup(&file_path_setup,anisotropic_mode_none,mipmap_mode_none,FALSE);
	model_new(&model,filename);
	
	model.nmesh=1;
	strcpy(model.meshes[0].name,"Default");
	
	model.view_box.size.x=model.view_box.size.y=model.view_box.size.z=100;
	model.shadow_box.size.x=model.shadow_box.size.y=model.shadow_box.size.z=100;

	windows_start();
	
	if (!create_binary()) {
		windows_end();
		InitCursor();
		return;
	}
	
	
	InitCursor();
	
		// finish
		
	fileopen=TRUE;
	
	reset_model_open();
}

void open_model_xml(void)
{
	char		file_name[256];
	
	InitCursor();

    if (!dialog_file_open_run("FileOpen","Models",NULL,file_name)) return;
	
		// open model
		
	SetCursor(*GetCursor(watchCursor));

	windows_start();
    
	model_setup(&file_path_setup,anisotropic_mode_none,mipmap_mode_none,FALSE);
	model_open(&model,file_name,TRUE,FALSE);
    	
	InitCursor();
	
		// finish
		
	fileopen=TRUE;
	
	reset_model_open();
}

void close_model_xml(void)
{
	int				n;
	
	model_close(&model);
	
	for (n=0;n!=max_model_mesh;n++) {
		model.meshes[n].nvertex=0;
		model.meshes[n].ntrig=0;
	}
	
	model.nbone=0;
	model.npose=0;
	model.nanimate=0;
	
	for (n=0;n!=max_model_light;n++) {
		model.tags.light_bone_tag[n]=FOUR_CHAR_CODE('\?\?\?\?');
	}
	
	for (n=0;n!=max_model_halo;n++) {
		model.tags.halo_bone_tag[n]=FOUR_CHAR_CODE('\?\?\?\?');
	}
	
	model.tags.name_bone_tag=FOUR_CHAR_CODE('\?\?\?\?');
	
	windows_end();
	
	fileopen=FALSE;
}

/* =======================================================

      Import Meshes
      
======================================================= */

void import_mesh_obj(void)
{
	float			scale;
	char			path[1024],err_str[256];
	unsigned char	p_err_str[256];
	bool			found_normals;
    
	InitCursor();
	if (!nav_open_file("obj",path)) return;
	
	if (cur_mesh==-1) cur_mesh=0;
	model.meshes[cur_mesh].nvertex=model.meshes[cur_mesh].ntrig=0;
	
	if (!import_obj(path,&found_normals,err_str)) {
		CopyCStringToPascal(err_str,p_err_str);
		StandardAlert(0,"\pCould not import .OBJ file",p_err_str,NULL,NULL);
		return;
	}

	vertex_delete_unused_vertexes(cur_mesh);
	
	dialog_import_finish_run(&model,&scale);
	model_scale(&model,cur_mesh,scale,scale,scale);

	model_center_xz(&model,cur_mesh);
	model_floor(&model,cur_mesh);
	model_recalc_boxes(&model);
    if (!found_normals) model_recalc_normals(&model,cur_mesh);
	
	reset_vertex_tab();
	reset_pose_list();
	reset_bone_list();
	
    draw_model_wind_pose(&model,cur_mesh,cur_pose);
    texture_palette_draw();
	mesh_palette_draw();
    info_palette_draw();
}

void import_mesh_lightwave(void)
{
	float			scale;
	char			path[1024],err_str[256];
	unsigned char	p_err_str[256];
    
	InitCursor();
	if (!nav_open_file("lwo",path)) return;
	
	if (cur_mesh==-1) cur_mesh=0;
	model.meshes[cur_mesh].nvertex=model.meshes[cur_mesh].ntrig=0;
	
	if (!import_lightwave(path,err_str)) {
		CopyCStringToPascal(err_str,p_err_str);
		StandardAlert(0,"\pCould not import .LWO file",p_err_str,NULL,NULL);
		return;
	}
	
	vertex_delete_unused_vertexes(cur_mesh);
	
	dialog_import_finish_run(&model,&scale);
	model_scale(&model,cur_mesh,scale,scale,scale);
	
    model_center_xz(&model,cur_mesh);
    model_floor(&model,cur_mesh);
    model_recalc_boxes(&model);
    model_recalc_normals(&model,cur_mesh);
	
	reset_vertex_tab();
	reset_pose_list();
	reset_bone_list();
	
    draw_model_wind_pose(&model,cur_mesh,cur_pose);
    texture_palette_draw();
	mesh_palette_draw();
    info_palette_draw();
}

void import_mesh_c4d_xml(void)
{
	char			path[1024],err_str[256];
	unsigned char	p_err_str[256];
	float			scale;
    
	InitCursor();
	if (!nav_open_file("xml",path)) return;
	
	if (cur_mesh==-1) cur_mesh=0;
	model.meshes[cur_mesh].nvertex=model.meshes[cur_mesh].ntrig=0;
	
	if (!import_c4d_xml(path,err_str)) {
		CopyCStringToPascal(err_str,p_err_str);
		StandardAlert(0,"\pCould not import .XML file",p_err_str,NULL,NULL);
		return;
	}
	
	vertex_delete_unused_vertexes(cur_mesh);
	
	dialog_import_finish_run(&model,&scale);
	model_scale_all(&model,scale,scale,scale);

    model_center_xz_all(&model);
    model_floor_all(&model);
    model_recalc_boxes(&model);
    model_recalc_normals(&model,cur_mesh);
	
	reset_vertex_tab();
	reset_pose_list();
	reset_bone_list();
	
    draw_model_wind_pose(&model,cur_mesh,cur_pose);
    texture_palette_draw();
	mesh_palette_draw();
    info_palette_draw();
}

/* =======================================================

      Insert Meshes
      
======================================================= */

void insert_mesh_dim3_model(void)
{
	char			file_name[256];
	
	InitCursor();

    if (!dialog_file_open_run("FileOpen","Models",NULL,file_name)) return;
	
	if (cur_mesh==-1) cur_mesh=0;
	
	SetCursor(*GetCursor(watchCursor));
	
	insert_model(file_name);
	
    model_calculate_parents(&model);
    model_center_xz(&model,cur_mesh);
    model_floor(&model,cur_mesh);
    model_recalc_boxes(&model);
    model_recalc_normals(&model,cur_mesh);
	
	InitCursor();
	
	reset_vertex_tab();
	reset_pose_list();
	reset_bone_list();
	reset_animate_list();
    draw_model_wind_pose(&model,cur_mesh,cur_pose);
    texture_palette_draw();
	mesh_palette_draw();
    info_palette_draw();
}

/* =======================================================

      Menu Enable/Disable
      
======================================================= */

void fix_menus(void)
{
	if (!fileopen) {
		EnableMenuItem(GetMenuHandle(filemenu),1);
		EnableMenuItem(GetMenuHandle(filemenu),2);
		DisableMenuItem(GetMenuHandle(filemenu),3);
		DisableMenuItem(GetMenuHandle(filemenu),5);
    
		DisableMenuItem(GetMenuHandle(editmenu),0);
		DisableMenuItem(GetMenuHandle(modelmenu),0);
		DisableMenuItem(GetMenuHandle(viewmenu),0);
		DisableMenuItem(GetMenuHandle(meshmenu),0);
		DisableMenuItem(GetMenuHandle(vertexmenu),0);
		DisableMenuItem(GetMenuHandle(bonemenu),0);
		DisableMenuItem(GetMenuHandle(posemenu),0);
		DisableMenuItem(GetMenuHandle(animatemenu),0);
		DisableMenuItem(GetMenuHandle(hitboxmenu),0);
	}
	else {
		DisableMenuItem(GetMenuRef(filemenu),1);
		DisableMenuItem(GetMenuRef(filemenu),2);
		EnableMenuItem(GetMenuRef(filemenu),3);
		EnableMenuItem(GetMenuRef(filemenu),5);
	
		EnableMenuItem(GetMenuRef(editmenu),0);
		EnableMenuItem(GetMenuRef(modelmenu),0);
		EnableMenuItem(GetMenuRef(viewmenu),0);
		EnableMenuItem(GetMenuRef(meshmenu),0);
		EnableMenuItem(GetMenuRef(vertexmenu),0);
		EnableMenuItem(GetMenuRef(bonemenu),0);
		EnableMenuItem(GetMenuRef(posemenu),0);
		EnableMenuItem(GetMenuRef(animatemenu),0);
		EnableMenuItem(GetMenuRef(hitboxmenu),0);
	}
	
	DrawMenuBar();
}

/* =======================================================

      Misc Menu Routines
      
======================================================= */

void prepare_model(void)
{
    model_calculate_parents(&model);
    model_center_xz_all(&model);
    model_floor_all(&model);
    model_recalc_boxes(&model);
}

void redraw_model(void)
{
    draw_model_wind_pose(&model,cur_mesh,cur_pose);
    info_palette_draw();
	reset_vertex_tab();
	reset_pose_list();
	reset_bone_list();
    reset_animate_list();
}

/* =======================================================

      Save Changes
      
======================================================= */

void save_changes(void)
{
	short					hit;
	AlertStdAlertParamRec	alert_param;
	
	memset(&alert_param,0x0,sizeof(AlertStdAlertParamRec));
	alert_param.defaultText="\pYes";
	alert_param.cancelText="\pNo";
	alert_param.defaultButton=kAlertStdAlertOKButton;
	alert_param.position=kWindowDefaultPosition;

	StandardAlert(0,"\pSave Changes?","\pDo you want to save the changes to this model?",&alert_param,&hit);
	
	if (hit==kAlertStdAlertOKButton) save_binary();
}

/* =======================================================

      Menu Events
      
======================================================= */

OSStatus app_event_menu(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	int				x,y,z,idx,major_bone_idx,minor_bone_idx,parent_idx;
	float			fx,fy,fz,bone_factor;
	bool			nudge_children,nudge_vertexes;
	HICommand		cmd;
	
	GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
	
	switch (cmd.commandID) {
	
 			// --------------------------------
			//
			// apple menu
			//
			// --------------------------------
   
        case kCommandAbout:
			dialog_about_run();
            return(noErr);
		
		case kHICommandQuit:
			if (fileopen) {
				save_changes();
				close_model_xml();
			}
			QuitApplicationEventLoop();
			return(noErr);
	
			// --------------------------------
			//
			// file menu
			//
			// --------------------------------

		case kCommandNew:
			model_wind_play(FALSE,FALSE);
			new_model_xml();
			return(noErr);

		case kCommandOpen:
			model_wind_play(FALSE,FALSE);
			open_model_xml();
			return(noErr);
			
		case kCommandClose:
			save_changes();
			close_model_xml();
			model_wind_play(FALSE,FALSE);
			fix_menus();
			return(noErr);
			
		case kCommandSave:
			save_binary();
			return(noErr);
			
			// --------------------------------
			//
			// edit menu
			//
			// --------------------------------

		case kCommandUndo:
			undo_run();
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			texture_palette_draw();
			mesh_palette_draw();
			info_palette_draw();
			return(noErr);
			
			// --------------------------------
			//
			// model menu
			//
			// --------------------------------
        
 		case kCommandSettings:
			dialog_model_settings_run();
            redraw_model();
			return(noErr);
			
 		case kCommandSpecialBones:
			dialog_special_bones_settings_run();
            redraw_model();
			return(noErr);
			
		case kCommandHitBoxes:
			dialog_hit_box_settings_run();
            redraw_model();
			return(noErr);
		
		case kCommandPrepareModel:
            prepare_model();
            redraw_model();
			return(noErr);
			
		case kCommandCalcBoxes:
            model_recalc_boxes(&model);
            redraw_model();
			return(noErr);
            
		case kCommandCalcNormals:
            model_recalc_normals(&model,cur_mesh);
            redraw_model();
			return(noErr);
            
		case kCommandScaleAll:
			if (!dialog_scale_run(&model,&fx,&fz,&fy)) return(noErr);
			model_scale_all(&model,fx,fy,fz);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);
			
		case kCommandFlipXAll:
			model_flip_all(&model,TRUE,FALSE,FALSE);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);

		case kCommandFlipZAll:
			model_flip_all(&model,FALSE,FALSE,TRUE);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);
			
		case kCommandFlipYAll:
			model_flip_all(&model,FALSE,TRUE,FALSE);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);
			
		case kCommandSwapXZAll:
			model_swap_xz_all(&model);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);
			
		case kCommandSwapYZAll:
			model_swap_yz_all(&model);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);
            
		case kCommandCenterXZAll:
			model_center_xz_all(&model);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);
            
		case kCommandFloorYAll:
			model_floor_all(&model);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);
						
			// --------------------------------
			//
			// view menu
			//
			// --------------------------------
					
		case kCommandFront:
			ang_y=180;
			ang_x=0;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandLeft:
			ang_y=270;
			ang_x=0;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandRight:
			ang_y=90;
			ang_x=0;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandBack:
			ang_y=0;
			ang_x=0;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandTop:
			ang_y=0;
			ang_x=270;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandBottom:
			ang_y=0;
			ang_x=90;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
						
			// --------------------------------
			//
			// mesh menu
			//
			// --------------------------------
			
		case kCommandNewMesh:
			idx=model_mesh_add(&model);
			if (idx==-1) return(noErr);
			
			if (!dialog_mesh_info_run(&model.meshes[idx])) {
				model_mesh_delete(&model,idx);
				return(noErr);
			}
			
			cur_mesh=idx;

			mesh_palette_draw();
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandDuplicateMesh:
			idx=model_mesh_duplicate(&model,cur_mesh);
			if (idx==-1) return(noErr);
			
			if (!dialog_mesh_info_run(&model.meshes[idx])) {
				model_mesh_delete(&model,idx);
				return(noErr);
			}
			
			cur_mesh=idx;

			mesh_palette_draw();
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandCopyMesh:
			idx=dialog_copy_mesh_run();
			if (idx==-1) return(noErr);
			if (idx!=cur_mesh) model_mesh_copy(&model,idx,cur_mesh);
			texture_palette_draw();
			mesh_palette_draw();
			info_palette_draw();
			redraw_model();
			reset_vertex_tab();
			return(noErr);
			
		case kCommandDeleteMesh:
			model_mesh_delete(&model,cur_mesh);
			cur_mesh=0;
			mesh_palette_draw();
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandImportOBJ:
			model_wind_play(FALSE,FALSE);
			import_mesh_obj();
			return(noErr);
			
		case kCommandImportLWO:
			model_wind_play(FALSE,FALSE);
			import_mesh_lightwave();
			return(noErr);
			
		case kCommandImportC4DXML:
			model_wind_play(FALSE,FALSE);
			import_mesh_c4d_xml();
			return(noErr);
			
		case kCommandInsertXML:
			model_wind_play(FALSE,FALSE);
			insert_mesh_dim3_model();
			return(noErr);
			
		case kCommandScale:
			if (!dialog_scale_run(&model,&fx,&fz,&fy)) return(noErr);
			model_scale(&model,cur_mesh,fx,fy,fz);
            redraw_model();
			return(noErr);
			
		case kCommandFlipX:
			model_flip(&model,cur_mesh,TRUE,FALSE,FALSE);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);

		case kCommandFlipZ:
			model_flip(&model,cur_mesh,FALSE,FALSE,TRUE);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);
			
		case kCommandFlipY:
			model_flip(&model,cur_mesh,FALSE,TRUE,FALSE);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);
			
		case kCommandFlipU:
			model_flip_uv(&model,cur_mesh,TRUE,FALSE);
            redraw_model();
			return(noErr);
			
		case kCommandFlipV:
			model_flip_uv(&model,cur_mesh,FALSE,TRUE);
            redraw_model();
			return(noErr);
			
		case kCommandSwapXZ:
			model_swap_xz(&model,cur_mesh);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);
			
		case kCommandSwapYZ:
			model_swap_yz(&model,cur_mesh);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);
            
		case kCommandCenterXZ:
			model_center_xz(&model,cur_mesh);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);
            
		case kCommandFloorY:
			model_floor(&model,cur_mesh);
            model_calculate_parents(&model);
            redraw_model();
			return(noErr);
			
			// --------------------------------
			//
			// vertex menu
			//
			// --------------------------------
			
		case kCommandVertexSelectAll:
			vertex_set_sel_mask_all(cur_mesh);
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			hilite_vertex_rows();
			return(noErr);
			
		case kCommandVertexSelectNotAttached:
			vertex_set_sel_mask_no_bone(cur_mesh);
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			hilite_vertex_rows();
			return(noErr);
			
		case kCommandVertexNudge:
			if (!dialog_nudge_rotate_run(&x,&z,&y,"NudgePick",0)) return(noErr);
			vertex_move_sel_vertexes(cur_mesh,x,y,z);
			model_calculate_parents(&model);
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandVertexScale:
			if (!dialog_scale_run(&model,&fx,&fz,&fy)) return(noErr);
			vertex_scale_sel_vertexes(cur_mesh,fx,fy,fz);
			model_calculate_parents(&model);
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandVertexRotate:
			if (!dialog_nudge_rotate_run(&x,&z,&y,"RotatePick",0)) return(noErr);
			vertex_rotate_sel_vertexes(cur_mesh,(float)x,(float)y,(float)z);
			model_calculate_parents(&model);
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandVertexInvertNormals:
			vertex_invert_normal_sel_vertexes(cur_mesh);
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandVertexHideSelected:
			vertex_hide_mask_set_sel_vertexes(cur_mesh);
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandVertexHideNonSelected:
			vertex_hide_mask_set_non_sel_vertexes(cur_mesh);
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandVertexShowAll:
			vertex_hide_mask_show_all_vertexes(cur_mesh);
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandVertexDelete:
			vertex_delete_sel_vertex(cur_mesh);
			model_calculate_parents(&model);
			texture_palette_draw();
			mesh_palette_draw();
			info_palette_draw();
			redraw_model();
			reset_vertex_tab();
			return(noErr);
			
			
			// --------------------------------
			//
			// bone menu
			//
			// --------------------------------
			
		case kCommandNewBone:
 			vertex_find_center_sel_vertexes(cur_mesh,&x,&y,&z);
			
			idx=model_bone_add(&model,x,y,z);
			if (idx==-1) return(noErr);
			
			if (!dialog_bone_settings_run(&model.bones[idx])) {
				model_bone_delete(&model,idx);
				return(noErr);
			}
			
			vertex_set_sel_vertex_to_bone(cur_mesh,idx,-1,1.0f);
			cur_bone=idx;

			reset_pose_list();
			reset_bone_list();
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandSetBone:
			if (!dialog_set_vertex_bone_run(&major_bone_idx,&minor_bone_idx,&bone_factor)) return(noErr);
			vertex_set_sel_vertex_to_bone(cur_mesh,major_bone_idx,minor_bone_idx,bone_factor);
			reset_vertex_tab();
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandNudgeBone:
			if (cur_bone==-1) return(noErr);
			
			if (!dialog_bone_nudge_run(&x,&z,&y,&nudge_children,&nudge_vertexes)) return(noErr);
			model_bone_move(&model,cur_bone,x,y,z,nudge_children,nudge_vertexes);
			model_calculate_parents(&model);
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandDeleteBone:
			if (cur_bone==-1) return(noErr);
			
 			model_bone_delete(&model,cur_bone);
			cur_bone=-1;
			reset_vertex_tab();
			reset_pose_list();
			reset_bone_list();
			return(noErr);
			
		case kCommandSelectVertexNearBone:
			if (cur_bone!=-1) return(noErr);
			vertex_set_sel_mask_near_bone(cur_mesh,cur_bone,0.10);
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
			// --------------------------------
			//
			// pose menu
			//
			// --------------------------------
			
		case kCommandNewPose:
 			idx=model_pose_add(&model);
			if (idx==-1) return(noErr);
			
			if (!dialog_pose_settings_run(&model.poses[idx])) {
				model_pose_delete(&model,idx);
				return(noErr);
			}
			
			cur_pose=idx;
			cur_bone=-1;

			reset_pose_list();
			reset_bone_list();
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandDupPose:
			if (cur_pose==-1) return(noErr);

			idx=model_pose_duplicate(&model,cur_pose);
			if (idx==-1) return(noErr);
			
			if (!dialog_pose_settings_run(&model.poses[idx])) {
				model_pose_delete(&model,idx);
				return(noErr);
			}
			
			cur_pose=idx;
			cur_bone=-1;

			reset_pose_list();
			reset_bone_list();
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandPreviousPose:
			if (cur_pose>0) cur_pose--;
			reset_pose_list();
			reset_bone_list();
			return(noErr);
			
		case kCommandNextPose:
			if (cur_pose<(model.npose-1)) cur_pose++;
			reset_pose_list();
			reset_bone_list();
			return(noErr);
			
		case kCommandClearPose:
			if (cur_pose==-1) return(noErr);

			model_pose_clear(&model,cur_pose);
			reset_pose_list();
			reset_bone_list();
			return(noErr);
			
        case kCommandDeletePose:
			if (cur_pose==-1) return(noErr);

			if (model_check_pose_in_animation(&model,cur_pose)) {
				StandardAlert(kAlertCautionAlert,"\pCan't Delete Pose","\pThis pose is being used in an animation.",NULL,NULL);
				return(noErr);
			}
			
            model_pose_delete(&model,cur_pose);
			
			cur_pose=-1;
			cur_bone=-1;
			
			reset_pose_list();
			reset_bone_list();
            return(noErr);
			
		case kCommandGoToBoneMoveParent:
			if ((cur_pose==-1) || (cur_bone==-1)) return(noErr);
			
			parent_idx=model.bones[cur_bone].parent_idx;
			if (parent_idx!=-1) {
				cur_bone=parent_idx;
				reset_pose_list();
				reset_bone_list();
			}

			return(noErr);
			
			// --------------------------------
			//
			// blending menu
			//
			// --------------------------------
			
		case kCommandBlendSetSkipAll:
			if (cur_pose==-1) return(noErr);
			
			model_pose_blend_set_all(&model,cur_pose,TRUE);
			reset_pose_list();
			reset_bone_list();
			return(noErr);
			
		case kCommandBlendSetSkipNone:
			if (cur_pose==-1) return(noErr);
			
			model_pose_blend_set_all(&model,cur_pose,FALSE);
			reset_pose_list();
			reset_bone_list();
			return(noErr);
			
		case kCommandBlendFlipSkip:
 			if ((cur_pose==-1) || (cur_bone==-1)) return(noErr);
			
			model.poses[cur_pose].bone_moves[cur_bone].skip_blended=!model.poses[cur_pose].bone_moves[cur_bone].skip_blended;
			reset_pose_list();
			reset_bone_list();
			return(noErr);
						
			// --------------------------------
			//
			// animation menu
			//
			// --------------------------------
			
		case kCommandNewAnimate:
			if (model.npose==0) {
				StandardAlert(0,"\pCan't Create Animation","\pYou need to have at least one pose before creating an animation.",NULL,NULL);
				return(noErr);
			}
			
			model_wind_play(FALSE,FALSE);
			
			idx=model_animate_add(&model);
			if (idx==-1) return(noErr);
			
			if (!dialog_animation_settings_run(idx)) {
				model_animate_delete(&model,idx);
				return(noErr);
			}
			
			cur_animate=idx;

			reset_animate_list();
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandDupAnimate:
			if (cur_animate==-1) return(noErr);
			model_wind_play(FALSE,FALSE);
 
			idx=model_animate_duplicate(&model,cur_animate);
			if (idx==-1) return(noErr);
			
			if (!dialog_animation_settings_run(idx)) {
				model_animate_delete(&model,idx);
				return(noErr);
			}

			cur_animate=idx;

			reset_animate_list();
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
            
		case kCommandDeleteAnimate:
			if (cur_animate==-1) return(noErr);
			model_wind_play(FALSE,FALSE);

			model_animate_delete(&model,cur_animate);
			if (model.nanimate==0) {
				cur_animate=-1;
			}
			else {
				cur_animate=0;
			}
			reset_animate_list();
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			return(noErr);
			
		case kCommandResetTimeAnimate:
			if (cur_animate==-1) return(noErr);
			model_wind_play(FALSE,FALSE);

			dialog_animation_reset_time_run(cur_animate);
			reset_animate_list();
			return(noErr);
			
		case kCommandPlayAnimate:
			model_wind_play(!play_animate,FALSE);
			return(noErr);
			
		case kCommandPlayBlendAnimate:
			if (dialog_play_blend_animation_run()) {
				model_wind_play(TRUE,TRUE);
			}
			return(noErr);
						
	}
	
	return(eventNotHandledErr);
}


