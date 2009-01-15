/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: External Editor Launching

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

#include "common_view.h"
#include "interface.h"

extern d3pnt					view_pnt;
extern float					walk_view_y_angle;

extern file_path_setup_type		file_path_setup;
extern setup_type				setup;
extern map_type					map;

/* =======================================================

      Launch dim3 Engine
      
======================================================= */

void launch_engine(void)
{
	short					hit;
	char					path[1024];
	unsigned char			uc_len;
	d3pnt					pt;
	d3ang					ang;
	AlertStdAlertParamRec	alert_param;
	CFURLRef				cf_url;
	FSRef					fsref;
	FILE					*file;
	
		// ok to save?
	
	memset(&alert_param,0x0,sizeof(AlertStdAlertParamRec));
	alert_param.defaultText="\pYes";
	alert_param.cancelText="\pNo";
	alert_param.defaultButton=kAlertStdAlertOKButton;
	alert_param.position=kWindowDefaultPosition;

	StandardAlert(0,"\pSave Changes and Launch Engine?","\pYou need to save changes to this map before you can launch the engine.  Click Yes to save changes and launch the map in the engine.",&alert_param,&hit);
	
	if (hit!=kAlertStdAlertOKButton) return;

		// save the map
		
	file_save_map();
	
		// the link file
		
	file_paths_base(&file_path_setup,path,"editor_link","tmp");

	file=fopen(path,"w");
	if (file==NULL) return;
	
		// map name
		
	uc_len=(unsigned char)strlen(map.info.name);
   	fwrite(&uc_len,1,1,file);
   	fwrite(map.info.name,1,strlen(map.info.name),file);
	
		// map position
		
	pt.x=view_pnt.x;
	pt.y=view_pnt.y;
	pt.z=view_pnt.z;
	ang.x=0.0f;
	ang.y=walk_view_y_angle;
	ang.z=0.0f;
		
	fwrite(&pt,1,sizeof(d3pnt),file);
	fwrite(&ang,1,sizeof(d3ang),file);
	
	fclose(file);
	
		// run engine
		
	file_paths_base(&file_path_setup,path,setup.engine_name,"app");

	FSPathMakeRef((unsigned char*)path,&fsref,NULL);
	
	cf_url=CFURLCreateFromFSRef(kCFAllocatorDefault,&fsref);
	LSOpenCFURLRef(cf_url,NULL);
	CFRelease(cf_url);
}

/* =======================================================

      Launch Map Script Editor
      
======================================================= */

void launch_map_script_editor(void)
{
	char					path[1024];
	CFURLRef				cf_url;
	FSRef					fsref;
		
	file_paths_data(&file_path_setup,path,"Scripts/Courses",map.info.name,"js");
	FSPathMakeRef((unsigned char*)path,&fsref,NULL);
	
	cf_url=CFURLCreateFromFSRef(kCFAllocatorDefault,&fsref);
	if (cf_url==NULL) {
		StandardAlert(0,"\pCould not locate script file","\pNo script file with this name exists.",NULL,NULL);
		return;
	}

	LSOpenCFURLRef(cf_url,NULL);
	CFRelease(cf_url);
}

/* =======================================================

      Launch Spot Script Editor
      
======================================================= */

void launch_spot_script_editor(char *script_name)
{
	char					path[1024];
	CFURLRef				cf_url;
	FSRef					fsref;
	
	if (script_name[0]==0x0) return;
		
	file_paths_data(&file_path_setup,path,"Scripts/Objects",script_name,"js");
	FSPathMakeRef((unsigned char*)path,&fsref,NULL);
	
	cf_url=CFURLCreateFromFSRef(kCFAllocatorDefault,&fsref);
	if (cf_url==NULL) {
		StandardAlert(0,"\pCould not locate script file","\pNo script file with this name exists.",NULL,NULL);
		return;
	}
	
	LSOpenCFURLRef(cf_url,NULL);
	CFRelease(cf_url);
}
