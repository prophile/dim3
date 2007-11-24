/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Vertex Setting Dialog

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

#include "dialog.h"

extern model_type		model;

#define kVertexX							FOUR_CHAR_CODE('pntx')
#define kVertexY							FOUR_CHAR_CODE('pnty')
#define kVertexZ							FOUR_CHAR_CODE('pntz')
#define kVertexNormalX						FOUR_CHAR_CODE('nmlx')
#define kVertexNormalY						FOUR_CHAR_CODE('nmly')
#define kVertexNormalZ						FOUR_CHAR_CODE('nmlz')
#define kVertexMajorBone					FOUR_CHAR_CODE('majb')
#define kVertexMinorBone					FOUR_CHAR_CODE('minb')
#define kVertexPercentage					FOUR_CHAR_CODE('perc')

bool						dialog_vertex_settings_cancel;
WindowRef					dialog_vertex_settings_wind;

/* =======================================================

      Vertex Setting Event Handlers
      
======================================================= */

static pascal OSStatus vertex_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int				major_bone_idx,minor_bone_idx,bone_factor;
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_vertex_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_vertex_settings_wind);
					return(noErr);
					
				case kHICommandOK:
				
						// check factor
					
					bone_factor=((float)dialog_get_int(dialog_vertex_settings_wind,kVertexPercentage,0))/100.0;
						
					if (bone_factor!=1) {
						major_bone_idx=dialog_get_bone_combo(dialog_vertex_settings_wind,kVertexMajorBone,0);
						minor_bone_idx=dialog_get_bone_combo(dialog_vertex_settings_wind,kVertexMinorBone,0);
			
						if ((major_bone_idx==-1) || (minor_bone_idx==-1)) {
							StandardAlert(0,"\pBone Factors","\pYou need both a major and minor bone to set the factor to anything but 100%",NULL,NULL);
							return(noErr);
						}
					}
					
						// ok to end

					QuitAppModalLoopForWindow(dialog_vertex_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Fill Bone Combo
      
======================================================= */

void dialog_set_bone_combo(WindowRef wind,unsigned long sig,int id,int sel_idx)
{
	int				n;
	char			txt[128];
	
	dialog_clear_combo(wind,sig,id);

	dialog_add_combo_item(wind,sig,id,"None",0);
	dialog_add_combo_item(wind,sig,id,"-",0);
	
	for (n=0;n!=model.nbone;n++) {
	
		memmove(txt,&model.bones[n].tag,4);
		txt[4]=0x0;
		
		if (model.bones[n].name[0]!=0x0) {
			strcat(txt," (");
			strcat(txt,model.bones[n].name);
			strcat(txt,")");
		}
		
		dialog_add_combo_item(wind,sig,id,txt,0);
	}
	
	if (sel_idx==-1) {
		dialog_set_combo(wind,sig,id,0);
	}
	else {
		dialog_set_combo(wind,sig,id,(sel_idx+2));
	}
}

int dialog_get_bone_combo(WindowRef wind,unsigned long sig,int id)
{
	int				sel_idx;
	
	sel_idx=dialog_get_combo(wind,sig,id);
	if (sel_idx==0) return(-1);
	
	return(sel_idx-2);
}

/* =======================================================

      Vertex Setting
      
======================================================= */

bool dialog_vertex_settings_run(model_vertex_type *vertex)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_vertex_settings_wind,"VertexSettings");

		// set controls
		
	dialog_set_int(dialog_vertex_settings_wind,kVertexX,0,vertex->pnt.x);
	dialog_set_int(dialog_vertex_settings_wind,kVertexY,0,vertex->pnt.y);
	dialog_set_int(dialog_vertex_settings_wind,kVertexZ,0,vertex->pnt.z);
	dialog_set_int(dialog_vertex_settings_wind,kVertexNormalX,0,vertex->normal.x);
	dialog_set_int(dialog_vertex_settings_wind,kVertexNormalY,0,vertex->normal.y);
	dialog_set_int(dialog_vertex_settings_wind,kVertexNormalZ,0,vertex->normal.z);
	dialog_set_bone_combo(dialog_vertex_settings_wind,kVertexMajorBone,0,vertex->major_bone_idx);
	dialog_set_bone_combo(dialog_vertex_settings_wind,kVertexMinorBone,0,vertex->minor_bone_idx);
	dialog_set_int(dialog_vertex_settings_wind,kVertexPercentage,0,(int)(vertex->bone_factor*100.0));
	
	dialog_set_focus(dialog_vertex_settings_wind,kVertexX,0);
	
		// show window
	
	ShowWindow(dialog_vertex_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(vertex_settings_event_proc);
	InstallWindowEventHandler(dialog_vertex_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_vertex_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_vertex_settings_wind);
	
		// dialog to data
		
	if (!dialog_vertex_settings_cancel) {
		vertex->pnt.x=dialog_get_int(dialog_vertex_settings_wind,kVertexX,0);
		vertex->pnt.y=dialog_get_int(dialog_vertex_settings_wind,kVertexY,0);
		vertex->pnt.z=dialog_get_int(dialog_vertex_settings_wind,kVertexZ,0);
		vertex->normal.x=dialog_get_int(dialog_vertex_settings_wind,kVertexNormalX,0);
		vertex->normal.y=dialog_get_int(dialog_vertex_settings_wind,kVertexNormalY,0);
		vertex->normal.z=dialog_get_int(dialog_vertex_settings_wind,kVertexNormalZ,0);
		vertex->major_bone_idx=dialog_get_bone_combo(dialog_vertex_settings_wind,kVertexMajorBone,0);
		vertex->minor_bone_idx=dialog_get_bone_combo(dialog_vertex_settings_wind,kVertexMinorBone,0);
		vertex->bone_factor=((float)dialog_get_int(dialog_vertex_settings_wind,kVertexPercentage,0))/100.0;
	}

		// close window
		
	DisposeWindow(dialog_vertex_settings_wind);
	
	return(!dialog_vertex_settings_cancel);
}

