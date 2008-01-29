/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Segment Dialog

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
#include "common_view.h"
#include "dialog.h"

extern map_type				map;

#define kSegmentSettingXOffset						FOUR_CHAR_CODE('xoff')
#define kSegmentSettingXSize						FOUR_CHAR_CODE('xsiz')
#define kSegmentSettingYOffset						FOUR_CHAR_CODE('yoff')
#define kSegmentSettingYSize						FOUR_CHAR_CODE('ysiz')
#define kSegmentSettingAlpha						FOUR_CHAR_CODE('alph')
#define kSegmentSettingDark							FOUR_CHAR_CODE('dark')
#define kSegmentSettingXShift						FOUR_CHAR_CODE('xsft')
#define kSegmentSettingYShift						FOUR_CHAR_CODE('ysft')
#define kSegmentSettingOn							FOUR_CHAR_CODE('fson')
#define kSegmentSettingPassThrough					FOUR_CHAR_CODE('fpth')
#define kSegmentSettingLockTextureSize				FOUR_CHAR_CODE('flts')
#define kSegmentSettingMovable						FOUR_CHAR_CODE('fmov')
#define kSegmentSettingShiftable					FOUR_CHAR_CODE('fsht')
#define kSegmentSettingClimbable					FOUR_CHAR_CODE('fclb')
#define kSegmentSettingTag							FOUR_CHAR_CODE('tagg')
#define kSegmentSettingOrientation					FOUR_CHAR_CODE('orin')

extern bool					dp_primitive;

bool						dialog_segment_setting_cancel;
WindowRef					dialog_segment_setting_wind;

/* =======================================================

      Segment Setting Event Handlers
      
======================================================= */

static pascal OSStatus segment_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandCancel:
					dialog_segment_setting_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_segment_setting_wind);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_segment_setting_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Duplicate Segment Changes
      
======================================================= */

void dialog_segment_setting_dup_changes(segment_type *org_seg,segment_type *seg,segment_type *seg2)
{
	if (seg->x_txtoff!=org_seg->x_txtoff) seg2->x_txtoff=seg->x_txtoff;
	if (seg->x_txtfact!=org_seg->x_txtfact) seg2->x_txtfact=seg->x_txtfact;
	if (seg->y_txtoff!=org_seg->y_txtoff) seg2->y_txtoff=seg->y_txtoff;
	if (seg->y_txtfact!=org_seg->y_txtfact) seg2->y_txtfact=seg->y_txtfact;
	if (seg->x_shift!=org_seg->x_shift) seg2->x_shift=seg->x_shift;
	if (seg->y_shift!=org_seg->y_shift) seg2->y_shift=seg->y_shift;
	if (seg->txt_ang!=org_seg->txt_ang) seg2->txt_ang=seg->txt_ang;
	if (seg->alpha!=org_seg->alpha) seg2->alpha=seg->alpha;
	if (seg->dark_factor!=org_seg->dark_factor) seg2->dark_factor=seg->dark_factor;
	if (seg->on!=org_seg->on) seg2->on=seg->on;
	if (seg->pass_through!=org_seg->pass_through) seg2->pass_through=seg->pass_through;
	if (seg->moveable!=org_seg->moveable) seg2->moveable=seg->moveable;
	if (seg->climbable!=org_seg->climbable) seg2->climbable=seg->climbable;
	if (seg->lock!=org_seg->lock) seg2->lock=seg->lock;
}

/* =======================================================

      Run Segment Setting
      
======================================================= */

bool dialog_segment_setting_run(int seg_idx)
{
	int						n,nsel,type,index,primitive_uid;
	segment_type			*seg,*seg2;
	segment_type			org_seg;
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_segment_setting_wind,"SegmentSetting");

		// set controls
		
	seg=&map.segments[seg_idx];
	
	dialog_set_float(dialog_segment_setting_wind,kSegmentSettingXOffset,0,seg->x_txtoff);
	dialog_set_float(dialog_segment_setting_wind,kSegmentSettingXSize,0,seg->x_txtfact);
	dialog_set_float(dialog_segment_setting_wind,kSegmentSettingYOffset,0,seg->y_txtoff);
	dialog_set_float(dialog_segment_setting_wind,kSegmentSettingYSize,0,seg->y_txtfact);
	dialog_set_float(dialog_segment_setting_wind,kSegmentSettingAlpha,0,seg->alpha);
	dialog_set_float(dialog_segment_setting_wind,kSegmentSettingDark,0,seg->dark_factor);
	dialog_set_float(dialog_segment_setting_wind,kSegmentSettingXShift,0,seg->x_shift);
	dialog_set_float(dialog_segment_setting_wind,kSegmentSettingYShift,0,seg->y_shift);
		
	dialog_set_boolean(dialog_segment_setting_wind,kSegmentSettingOn,0,seg->on);
	dialog_set_boolean(dialog_segment_setting_wind,kSegmentSettingPassThrough,0,seg->pass_through);
	dialog_set_boolean(dialog_segment_setting_wind,kSegmentSettingLockTextureSize,0,seg->lock);
	dialog_set_boolean(dialog_segment_setting_wind,kSegmentSettingMovable,0,seg->moveable);
	dialog_set_boolean(dialog_segment_setting_wind,kSegmentSettingShiftable,0,seg->shiftable);
	dialog_set_boolean(dialog_segment_setting_wind,kSegmentSettingClimbable,0,seg->climbable);

	dialog_set_int(dialog_segment_setting_wind,kSegmentSettingTag,0,seg->tag);
	dialog_set_combo(dialog_segment_setting_wind,kSegmentSettingOrientation,0,seg->txt_ang);
	
		// show window
	
	ShowWindow(dialog_segment_setting_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(segment_setting_event_proc);
	InstallWindowEventHandler(dialog_segment_setting_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_segment_setting_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_segment_setting_wind);
	
		// dialog to data
		
	if (!dialog_segment_setting_cancel) {
	
		memmove(&org_seg,seg,sizeof(segment_type));
	
		seg->x_txtoff=dialog_get_float(dialog_segment_setting_wind,kSegmentSettingXOffset,0);
		seg->x_txtfact=dialog_get_float(dialog_segment_setting_wind,kSegmentSettingXSize,0);
		seg->y_txtoff=dialog_get_float(dialog_segment_setting_wind,kSegmentSettingYOffset,0);
		seg->y_txtfact=dialog_get_float(dialog_segment_setting_wind,kSegmentSettingYSize,0);
		seg->dark_factor=dialog_get_float(dialog_segment_setting_wind,kSegmentSettingDark,0);
		seg->alpha=dialog_get_float(dialog_segment_setting_wind,kSegmentSettingAlpha,0);
		seg->x_shift=dialog_get_float(dialog_segment_setting_wind,kSegmentSettingXShift,0);
		seg->y_shift=dialog_get_float(dialog_segment_setting_wind,kSegmentSettingYShift,0);
			
		seg->on=dialog_get_boolean(dialog_segment_setting_wind,kSegmentSettingOn,0);
		seg->pass_through=dialog_get_boolean(dialog_segment_setting_wind,kSegmentSettingPassThrough,0);
		seg->lock=dialog_get_boolean(dialog_segment_setting_wind,kSegmentSettingLockTextureSize,0);
		seg->moveable=dialog_get_boolean(dialog_segment_setting_wind,kSegmentSettingMovable,0);
		seg->shiftable=dialog_get_boolean(dialog_segment_setting_wind,kSegmentSettingShiftable,0);
		seg->climbable=dialog_get_boolean(dialog_segment_setting_wind,kSegmentSettingClimbable,0);

		seg->tag=dialog_get_int(dialog_segment_setting_wind,kSegmentSettingTag,0);
		seg->txt_ang=dialog_get_combo(dialog_segment_setting_wind,kSegmentSettingOrientation,0);
		
			// need to set flags across primitive

		primitive_uid=seg->primitive_uid[0];
		if ((primitive_uid!=-1) && (dp_primitive)) {

			seg2=map.segments;
	
			for (n=0;n!=map.nsegment;n++) {
				if (seg2->primitive_uid[0]==primitive_uid) dialog_segment_setting_dup_changes(&org_seg,seg,seg2);
				seg2++;
			}
		}
		
			// need to set flag across selection?
		
			/* supergumba	
		nsel=select_count();
		
		for (n=0;n!=nsel;n++) {
			select_get(n,&type,&index);
			if (type!=segment_piece) continue;
			if (index==seg_idx) continue;
			
			seg2=&map.segments[index];
			dialog_segment_setting_dup_changes(&org_seg,seg,seg2);
		}
		
		main_wind_draw();
		*/
	}

		// close window
		
	DisposeWindow(dialog_segment_setting_wind);
	
	return(!dialog_segment_setting_cancel);
}

