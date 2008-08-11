/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Auto Generating and Import Help

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
#include "import.h"
#include "common_view.h"
#include "walk_view.h"

extern map_type		map;

char								import_file_ext[32];

bool								ag_first=TRUE;
auto_generate_settings_type			ag_settings;

/* =======================================================

      Auto-Generate Map
      
======================================================= */

void auto_generate_map(void)
{
		// dialog
		
	if (!dialog_map_auto_generate_setting_run(ag_first)) return;
	ag_first=FALSE;

	select_clear();
	undo_clear();

		// run the auto-generate
		
	SetCursor(*GetCursor(watchCursor));
	map_auto_generate(&map,&ag_settings);
	InitCursor();
	
		// redraw the map
	
	main_wind_tool_fill_group_combo();
		
	main_wind_center_position_in_map();		
	main_wind_draw();
	
	texture_palette_reset();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
	menu_fix_enable();
}

/* =======================================================

      Load a Import File
      
======================================================= */

void import_nav_event_proc(const NavEventCallbackMessage callBackSelector,NavCBRecPtr callBackParms,NavCallBackUserData callBackUD)
{
}

Boolean import_nav_file_filter(AEDesc *theItem,void *info,void *callBackUD,NavFilterModes filterMode)
{
    char					*c,filename[256];
    NavFileOrFolderInfo		*filefolder;
    AEDesc					desc;
	HFSUniStr255			uniname;
    CFStringRef				cfstr;
    FSRef					fref;
    
    if ((theItem->descriptorType!=typeFSS) && (theItem->descriptorType!=typeFSRef)) return(FALSE);

    filefolder=(NavFileOrFolderInfo*)info;
    if (filefolder->isFolder) return(TRUE);

	AECoerceDesc(theItem,typeFSRef,&desc);
	AEGetDescData(&desc,(void*)&fref,sizeof(FSRef));
    AEDisposeDesc(&desc);

    FSGetCatalogInfo(&fref,kFSCatInfoNone,NULL,&uniname,NULL,NULL);
	cfstr=CFStringCreateWithCharacters(kCFAllocatorDefault,uniname.unicode,uniname.length);
	CFStringGetCString(cfstr,filename,256,kCFStringEncodingMacRoman);
    CFRelease(cfstr);
    
    c=strchr(filename,'.');
    if (c==NULL) return(FALSE);
    
    return(strcasecmp((c+1),import_file_ext)==0);
}

bool import_load_file(char *path,char *ext)
{
    NavDialogCreationOptions	navoption;
    NavReplyRecord				navreply;
	NavEventUPP					navevent;
    NavObjectFilterUPP			navfilter;
	AEKeyword					keyword;
	DescType					typecode;
    Size						sz;
    NavDialogRef				diagref;
	FSRef						fsref;
	
	strcpy(import_file_ext,ext);
	
		// get the file
		
	NavGetDefaultDialogCreationOptions(&navoption);
	navoption.optionFlags-=kNavDontAddTranslateItems;
	navoption.optionFlags-=kNavAllowPreviews;

	navevent=NewNavEventUPP(import_nav_event_proc);
	navfilter=NewNavObjectFilterUPP(import_nav_file_filter);
	NavCreateGetFileDialog(&navoption,NULL,navevent,NULL,navfilter,NULL,&diagref);
	NavDialogRun(diagref);
 
 	NavDialogGetReply(diagref,&navreply);
	NavDialogDispose(diagref);
	DisposeNavEventUPP(navevent);
    DisposeNavObjectFilterUPP(navfilter);
    
	if (!navreply.validRecord) {
		NavDisposeReply(&navreply);
        return(FALSE);
    }
    
	AEGetNthPtr(&(navreply.selection),1,typeFSRef,&keyword,&typecode,(void*)&fsref,sizeof(FSRef),&sz);
    NavDisposeReply(&navreply);
	
    FSRefMakePath(&fsref,(unsigned char*)path,1024);
	
	return(TRUE);
}



