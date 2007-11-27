/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Animation Setting Dialog

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

#include "window.h"
#include "dialog.h"
#include "model.h"
#include "tab.h"

extern bool				play_animate;

extern model_type		model;

#define kAnimationPoseTabCount				6

#define kAnimationName						FOUR_CHAR_CODE('name')
#define kAnimationLoop						FOUR_CHAR_CODE('loop')

#define kAnimationPoseTab					FOUR_CHAR_CODE('tabb')

#define kAnimationPoseList					FOUR_CHAR_CODE('psls')
#define kPoseListNameColumn					FOUR_CHAR_CODE('pnme')

#define kAnimationPoseAdd					FOUR_CHAR_CODE('addp')
#define kAnimationPoseSub					FOUR_CHAR_CODE('subp')
#define kAnimationPoseLoopStart				FOUR_CHAR_CODE('lpst')
#define kAnimationPoseLoopEnd				FOUR_CHAR_CODE('lped')
#define kAnimationPosePlay					FOUR_CHAR_CODE('play')

#define kAnimationPosePose					FOUR_CHAR_CODE('pose')
#define kAnimationPoseTime					FOUR_CHAR_CODE('time')
#define kAnimationPoseAccel					FOUR_CHAR_CODE('accl')
#define kAnimationPoseSwayX					FOUR_CHAR_CODE('swyx')
#define kAnimationPoseSwayY					FOUR_CHAR_CODE('swyy')
#define kAnimationPoseSwayZ					FOUR_CHAR_CODE('swyz')
#define kAnimationPoseMoveX					FOUR_CHAR_CODE('movx')
#define kAnimationPoseMoveY					FOUR_CHAR_CODE('movy')
#define kAnimationPoseMoveZ					FOUR_CHAR_CODE('movz')
#define kAnimationEffectBone				FOUR_CHAR_CODE('eftb')

#define kAnimationSoundName					FOUR_CHAR_CODE('sndn')
#define kAnimationSoundPitch				FOUR_CHAR_CODE('sndp')

#define kAnimationParticleName				FOUR_CHAR_CODE('prtn')
#define kAnimationParticleMotion			FOUR_CHAR_CODE('psim')
#define kAnimationParticleMotionFactor		FOUR_CHAR_CODE('psmf')
#define kAnimationParticleRotation			FOUR_CHAR_CODE('psir')
#define kAnimationParticleStick				FOUR_CHAR_CODE('pssm')
#define kAnimationParticleSlopX				FOUR_CHAR_CODE('pslx')
#define kAnimationParticleSlopY				FOUR_CHAR_CODE('psly')
#define kAnimationParticleSlopZ				FOUR_CHAR_CODE('pslz')

#define kAnimationRingName					FOUR_CHAR_CODE('rign')
#define kAnimationRingAngle					FOUR_CHAR_CODE('riga')
#define kAnimationRingSlopX					FOUR_CHAR_CODE('rslx')
#define kAnimationRingSlopY					FOUR_CHAR_CODE('rsly')
#define kAnimationRingSlopZ					FOUR_CHAR_CODE('rslz')

#define kAnimationFlashIntensity			FOUR_CHAR_CODE('fint')
#define kAnimationFlashColor				FOUR_CHAR_CODE('fcol')
#define kAnimationFlashColorEdit			FOUR_CHAR_CODE('colb')
#define kAnimationFlashLiveTime				FOUR_CHAR_CODE('flvt')
#define kAnimationFlashFadeTime				FOUR_CHAR_CODE('ffdt')
#define kAnimationFlashColorButton			FOUR_CHAR_CODE('colh')

#define kAnimationShakeDistance				FOUR_CHAR_CODE('skds')
#define kAnimationShakeSize					FOUR_CHAR_CODE('sksz')
#define kAnimationShakeLiveTime				FOUR_CHAR_CODE('sklf')

#define kAnimationMeshName					FOUR_CHAR_CODE('mshn')
#define kAnimationMeshFadeInTime			FOUR_CHAR_CODE('msfi')
#define kAnimationMeshFadeOutTime			FOUR_CHAR_CODE('msfo')

WindowRef					dialog_animation_settings_wind;
ControlRef					dialog_pose_move_list;
RGBColor					dialog_pose_move_settings_color;

int							dialog_animate_idx,dialog_pose_move_idx;
bool						dialog_pose_move_change_ok;

/* =======================================================

      Get and Save Data
      
======================================================= */

void dialog_pose_move_settings_load(void)
{
	int							mesh_idx;
	model_pose_move_type		*pose_move;
	
	pose_move=&model.animates[dialog_animate_idx].pose_moves[dialog_pose_move_idx];
		
		// set controls

	dialog_set_combo(dialog_animation_settings_wind,kAnimationPosePose,0,pose_move->pose_idx);
	
	dialog_set_int(dialog_animation_settings_wind,kAnimationPoseTime,0,pose_move->msec);
	dialog_set_value(dialog_animation_settings_wind,kAnimationPoseAccel,0,(int)(pose_move->acceleration*100.0f));

	dialog_set_float(dialog_animation_settings_wind,kAnimationPoseSwayX,0,pose_move->sway.x);
	dialog_set_float(dialog_animation_settings_wind,kAnimationPoseSwayY,0,pose_move->sway.y);
	dialog_set_float(dialog_animation_settings_wind,kAnimationPoseSwayZ,0,pose_move->sway.z);
	dialog_set_float(dialog_animation_settings_wind,kAnimationPoseMoveX,0,pose_move->mov.x);
	dialog_set_float(dialog_animation_settings_wind,kAnimationPoseMoveY,0,pose_move->mov.y);
	dialog_set_float(dialog_animation_settings_wind,kAnimationPoseMoveZ,0,pose_move->mov.z);
	
	dialog_set_bone_combo(dialog_animation_settings_wind,kAnimationEffectBone,0,pose_move->effect_bone_idx);

	dialog_special_combo_fill_sound(dialog_animation_settings_wind,kAnimationSoundName,0,pose_move->sound.name);
	dialog_set_float(dialog_animation_settings_wind,kAnimationSoundPitch,0,pose_move->sound.pitch);
	
	dialog_special_combo_fill_particle(dialog_animation_settings_wind,kAnimationParticleName,0,pose_move->particle.name);
	dialog_set_boolean(dialog_animation_settings_wind,kAnimationParticleMotion,0,pose_move->particle.motion);
	dialog_set_float(dialog_animation_settings_wind,kAnimationParticleMotionFactor,0,pose_move->particle.motion_factor);
	dialog_set_boolean(dialog_animation_settings_wind,kAnimationParticleRotation,0,pose_move->particle.rotate);
	dialog_set_boolean(dialog_animation_settings_wind,kAnimationParticleStick,0,pose_move->particle.stick);
	dialog_set_int(dialog_animation_settings_wind,kAnimationParticleSlopX,0,pose_move->particle.slop.x);
	dialog_set_int(dialog_animation_settings_wind,kAnimationParticleSlopY,0,pose_move->particle.slop.y);
	dialog_set_int(dialog_animation_settings_wind,kAnimationParticleSlopZ,0,pose_move->particle.slop.z);

	dialog_special_combo_fill_ring(dialog_animation_settings_wind,kAnimationRingName,0,pose_move->ring.name);
	dialog_set_boolean(dialog_animation_settings_wind,kAnimationRingAngle,0,pose_move->ring.angle);
	dialog_set_int(dialog_animation_settings_wind,kAnimationRingSlopX,0,pose_move->ring.slop.x);
	dialog_set_int(dialog_animation_settings_wind,kAnimationRingSlopY,0,pose_move->ring.slop.y);
	dialog_set_int(dialog_animation_settings_wind,kAnimationRingSlopZ,0,pose_move->ring.slop.z);
	
	dialog_set_int(dialog_animation_settings_wind,kAnimationFlashIntensity,0,pose_move->flash.intensity);
	dialog_set_int(dialog_animation_settings_wind,kAnimationFlashLiveTime,0,pose_move->flash.flash_msec);
	dialog_set_int(dialog_animation_settings_wind,kAnimationFlashFadeTime,0,pose_move->flash.fade_msec);
	
	dialog_pose_move_settings_color.red=(int)(pose_move->flash.col.r*(float)0xFFFF);
	dialog_pose_move_settings_color.green=(int)(pose_move->flash.col.g*(float)0xFFFF);
	dialog_pose_move_settings_color.blue=(int)(pose_move->flash.col.b*(float)0xFFFF);
	
	dialog_set_int(dialog_animation_settings_wind,kAnimationShakeDistance,0,pose_move->shake.distance);
	dialog_set_int(dialog_animation_settings_wind,kAnimationShakeSize,0,pose_move->shake.size);
	dialog_set_int(dialog_animation_settings_wind,kAnimationShakeLiveTime,0,pose_move->shake.life_msec);
	
	mesh_idx=model_find_mesh(&model,pose_move->mesh_fade.name);
	if (mesh_idx==-1) {
		dialog_set_combo(dialog_animation_settings_wind,kAnimationMeshName,0,0);
	}
	else {
		dialog_set_combo(dialog_animation_settings_wind,kAnimationMeshName,0,(mesh_idx+2));
	}
	dialog_set_int(dialog_animation_settings_wind,kAnimationMeshFadeInTime,0,pose_move->mesh_fade.fade_in_msec);
	dialog_set_int(dialog_animation_settings_wind,kAnimationMeshFadeOutTime,0,pose_move->mesh_fade.fade_out_msec);
	
	DrawControls(dialog_animation_settings_wind);
}

void dialog_pose_move_settings_save(void)
{
	int							mesh_idx;
	model_pose_move_type		*pose_move;
	
	pose_move=&model.animates[dialog_animate_idx].pose_moves[dialog_pose_move_idx];

	pose_move->pose_idx=dialog_get_combo(dialog_animation_settings_wind,kAnimationPosePose,0);
	
	pose_move->msec=dialog_get_int(dialog_animation_settings_wind,kAnimationPoseTime,0);
	if (pose_move->msec<=0) pose_move->msec=1;

	pose_move->sway.x=dialog_get_float(dialog_animation_settings_wind,kAnimationPoseSwayX,0);
	pose_move->sway.y=dialog_get_float(dialog_animation_settings_wind,kAnimationPoseSwayY,0);
	pose_move->sway.z=dialog_get_float(dialog_animation_settings_wind,kAnimationPoseSwayZ,0);
	pose_move->mov.x=dialog_get_float(dialog_animation_settings_wind,kAnimationPoseMoveX,0);
	pose_move->mov.y=dialog_get_float(dialog_animation_settings_wind,kAnimationPoseMoveY,0);
	pose_move->mov.z=dialog_get_float(dialog_animation_settings_wind,kAnimationPoseMoveZ,0);
	pose_move->acceleration=((float)dialog_get_value(dialog_animation_settings_wind,kAnimationPoseAccel,0))/100.0f;
	
	pose_move->effect_bone_idx=dialog_get_bone_combo(dialog_animation_settings_wind,kAnimationEffectBone,0);

	dialog_special_combo_get_sound(dialog_animation_settings_wind,kAnimationSoundName,0,pose_move->sound.name,name_str_len);
	pose_move->sound.pitch=dialog_get_float(dialog_animation_settings_wind,kAnimationSoundPitch,0);

	dialog_special_combo_get_particle(dialog_animation_settings_wind,kAnimationParticleName,0,pose_move->particle.name,name_str_len);
	pose_move->particle.motion=dialog_get_boolean(dialog_animation_settings_wind,kAnimationParticleMotion,0);
	pose_move->particle.motion_factor=dialog_get_float(dialog_animation_settings_wind,kAnimationParticleMotionFactor,0);
	pose_move->particle.rotate=dialog_get_boolean(dialog_animation_settings_wind,kAnimationParticleRotation,0);
	pose_move->particle.stick=dialog_get_boolean(dialog_animation_settings_wind,kAnimationParticleStick,0);
	pose_move->particle.slop.x=dialog_get_int(dialog_animation_settings_wind,kAnimationParticleSlopX,0);
	pose_move->particle.slop.y=dialog_get_int(dialog_animation_settings_wind,kAnimationParticleSlopY,0);
	pose_move->particle.slop.z=dialog_get_int(dialog_animation_settings_wind,kAnimationParticleSlopZ,0);

	dialog_special_combo_get_ring(dialog_animation_settings_wind,kAnimationRingName,0,pose_move->ring.name,name_str_len);
	pose_move->ring.angle=dialog_get_boolean(dialog_animation_settings_wind,kAnimationRingAngle,0);
	pose_move->ring.slop.x=dialog_get_int(dialog_animation_settings_wind,kAnimationRingSlopX,0);
	pose_move->ring.slop.y=dialog_get_int(dialog_animation_settings_wind,kAnimationRingSlopY,0);
	pose_move->ring.slop.z=dialog_get_int(dialog_animation_settings_wind,kAnimationRingSlopZ,0);
	
	pose_move->flash.intensity=dialog_get_int(dialog_animation_settings_wind,kAnimationFlashIntensity,0);
	pose_move->flash.flash_msec=dialog_get_int(dialog_animation_settings_wind,kAnimationFlashLiveTime,0);
	pose_move->flash.fade_msec=dialog_get_int(dialog_animation_settings_wind,kAnimationFlashFadeTime,0);
	
	pose_move->flash.col.r=((float)dialog_pose_move_settings_color.red/(float)0xFFFF);
	pose_move->flash.col.g=((float)dialog_pose_move_settings_color.green/(float)0xFFFF);
	pose_move->flash.col.b=((float)dialog_pose_move_settings_color.blue/(float)0xFFFF);
	
	pose_move->shake.distance=dialog_get_int(dialog_animation_settings_wind,kAnimationShakeDistance,0);
	pose_move->shake.size=dialog_get_int(dialog_animation_settings_wind,kAnimationShakeSize,0);
	pose_move->shake.life_msec=dialog_get_int(dialog_animation_settings_wind,kAnimationShakeLiveTime,0);
	
	mesh_idx=dialog_get_combo(dialog_animation_settings_wind,kAnimationMeshName,0);
	if (mesh_idx==0) {
		pose_move->mesh_fade.name[0]=0x0;
	}
	else {
		strcpy(pose_move->mesh_fade.name,model.meshes[mesh_idx-2].name);
	}
	pose_move->mesh_fade.fade_in_msec=dialog_get_int(dialog_animation_settings_wind,kAnimationMeshFadeInTime,0);
	pose_move->mesh_fade.fade_out_msec=dialog_get_int(dialog_animation_settings_wind,kAnimationMeshFadeOutTime,0);
}

/* =======================================================

      Pose Move Setting Event Handlers
      
======================================================= */

void dialog_pose_list_reset(void)
{
	DataBrowserItemID				itemID;

	RemoveDataBrowserItems(dialog_pose_move_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	AddDataBrowserItems(dialog_pose_move_list,kDataBrowserNoItem,model.animates[dialog_animate_idx].npose_move,NULL,kDataBrowserItemNoProperty);

	itemID=dialog_pose_move_idx+1;
	SetDataBrowserSelectedItems(dialog_pose_move_list,1,&itemID,kDataBrowserItemsAssign);

	Draw1Control(dialog_pose_move_list);
}

static pascal OSStatus animation_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kAnimationPoseAdd:
					dialog_pose_move_idx=model_animate_pose_insert(&model,dialog_animate_idx,dialog_pose_move_idx,0);
					dialog_pose_move_settings_load();
					dialog_pose_list_reset();
					return(noErr);
					
				case kAnimationPoseSub:
					if (model.animates[dialog_animate_idx].npose_move<=1) return(noErr);
					model_animate_pose_delete(&model,dialog_animate_idx,dialog_pose_move_idx);
					if (dialog_pose_move_idx>=model.animates[dialog_animate_idx].npose_move) dialog_pose_move_idx=0;
					dialog_pose_move_settings_load();
					dialog_pose_list_reset();
					return(noErr);
					
				case kAnimationPoseLoopStart:
					model_animate_set_loop_start(&model,dialog_animate_idx,dialog_pose_move_idx);
					dialog_pose_list_reset();
					return(noErr);
					
				case kAnimationPoseLoopEnd:
					model_animate_set_loop_end(&model,dialog_animate_idx,dialog_pose_move_idx);
					dialog_pose_list_reset();
					return(noErr);
			
				case kAnimationPosePlay:
					model_wind_play(!play_animate,FALSE);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_animation_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
			
	}
	
	return(eventNotHandledErr);
}

static pascal OSStatus pose_move_setting_tab_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	dialog_switch_tab(dialog_animation_settings_wind,kAnimationPoseTab,0,kAnimationPoseTabCount);

		// draw color

	if (dialog_get_value(dialog_animation_settings_wind,kAnimationPoseTab,0)==5) {
		dialog_draw_color(dialog_animation_settings_wind,kAnimationFlashColor,0,&dialog_pose_move_settings_color);
	}
	
	return(noErr);
}

static pascal OSStatus pose_move_setting_button_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	Point			pt;
	RGBColor		color;

	pt.h=pt.v=-1;
	if (GetColor(pt,"\pChoose the Light Color:",&dialog_pose_move_settings_color,&color)) {
		dialog_pose_move_settings_color=color;
	}
	dialog_draw_color(dialog_animation_settings_wind,kAnimationFlashColor,0,&dialog_pose_move_settings_color);
	
	return(noErr);
}

/* =======================================================

      Timer
      
======================================================= */

void pose_move_timer(EventLoopTimerRef inTimer,void *inUserData)
{
	model_pose_move_type	pose_move;
	
		// any data changed?
		
	memmove(&pose_move,&model.animates[dialog_animate_idx].pose_moves[dialog_pose_move_idx],sizeof(model_pose_move_type));
	dialog_pose_move_settings_save();
	if (memcmp(&pose_move,&model.animates[dialog_animate_idx].pose_moves[dialog_pose_move_idx],sizeof(model_pose_move_type))==0) return;
	
		// reset other control
	
	dialog_pose_list_reset();
}

/* =======================================================

      Pose List Event Handlers
      
======================================================= */

static pascal OSStatus pose_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int				idx,pose_idx,loop_start_idx,loop_end_idx;
	char			str[256];
	CFStringRef		cfstr;
	
	switch (property) {
	
		case kPoseListNameColumn:
			idx=itemID-1;
			
			pose_idx=model.animates[dialog_animate_idx].pose_moves[idx].pose_idx;
			
			loop_start_idx=model.animates[dialog_animate_idx].loop_start;
			loop_end_idx=model.animates[dialog_animate_idx].loop_end;
			
			sprintf(str,"  %s (%d)",model.poses[pose_idx].name,model.animates[dialog_animate_idx].pose_moves[idx].msec);
			if ((idx>=loop_start_idx) && (idx<=loop_end_idx)) str[0]='¥';
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);

	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void pose_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	switch (message) {

		case kDataBrowserItemSelected:
			if ((!dialog_pose_move_change_ok) || (itemID==0)) return;
			
			dialog_pose_move_settings_save();
			dialog_pose_move_idx=itemID-1;
			dialog_pose_move_settings_load();
			
			break;

	}
}

/* =======================================================

      Run Pose Move Setting
      
======================================================= */

void dialog_animation_settings_run(int animate_idx,int pose_move_idx)
{
	int								i;
	ControlRef						ctrl;
	ControlID						ctrl_id;
	DataBrowserItemID				itemID;
	DataBrowserCallbacks			dbcall;
	DataBrowserItemDataUPP			list_item_upp;
	DataBrowserItemNotificationUPP	list_notify_upp;
	EventHandlerUPP					event_upp,tab_event_upp,button_event_upp;
	EventLoopTimerRef				timer_event;
	EventLoopTimerUPP				timer_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}},
									tab_event_list[]={{kEventClassControl,kEventControlHit}},
									button_event_list[]={{kEventClassControl,kEventControlHit}};
	
		// if there is no pose moves or new animation,
		// then we need to add a default pose
		
	if (model.animates[animate_idx].npose_move==0) {
		pose_move_idx=model_animate_pose_insert(&model,animate_idx,-1,0);
	}
	
	if (pose_move_idx==-1) pose_move_idx=0;
	
		// open the dialog
		
	dialog_open(&dialog_animation_settings_wind,"AnimationSettings");
	
		// tab
		
	dialog_set_tab(dialog_animation_settings_wind,kAnimationPoseTab,0,0,kAnimationPoseTabCount);
	
	ctrl_id.signature=kAnimationPoseTab;
	ctrl_id.id=0;
	GetControlByID(dialog_animation_settings_wind,&ctrl_id,&ctrl);
	
	tab_event_upp=NewEventHandlerUPP(pose_move_setting_tab_proc);
	InstallControlEventHandler(ctrl,tab_event_upp,GetEventTypeCount(tab_event_list),tab_event_list,dialog_animation_settings_wind,NULL);

		// color button
		// have to do this because window doesn't get commands for buttons in tabs
		
	ctrl_id.signature=kAnimationFlashColorEdit;
	ctrl_id.id=0;
	GetControlByID(dialog_animation_settings_wind,&ctrl_id,&ctrl);
		
	button_event_upp=NewEventHandlerUPP(pose_move_setting_button_proc);
	InstallControlEventHandler(ctrl,button_event_upp,GetEventTypeCount(button_event_list),button_event_list,dialog_animation_settings_wind,NULL);

		// set pose list
		
	dialog_animate_idx=animate_idx;
	dialog_pose_move_idx=pose_move_idx;
		
	ctrl_id.signature=kAnimationPoseList;
	ctrl_id.id=0;
	GetControlByID(dialog_animation_settings_wind,&ctrl_id,&dialog_pose_move_list);
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	list_item_upp=NewDataBrowserItemDataUPP(&pose_list_item_proc);
	dbcall.u.v1.itemDataCallback=list_item_upp;

	list_notify_upp=NewDataBrowserItemNotificationUPP(&pose_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=list_notify_upp;
	
	SetDataBrowserCallbacks(dialog_pose_move_list,&dbcall);
	
	AddDataBrowserItems(dialog_pose_move_list,kDataBrowserNoItem,model.animates[dialog_animate_idx].npose_move,NULL,kDataBrowserItemNoProperty);
	
	dialog_pose_move_change_ok=FALSE;
	
	itemID=pose_move_idx+1;
	SetDataBrowserSelectedItems(dialog_pose_move_list,1,&itemID,kDataBrowserItemsAssign);
	
	dialog_pose_move_change_ok=TRUE;

		// fill pose combo

	dialog_clear_combo(dialog_animation_settings_wind,kAnimationPosePose,0);
	
	for (i=0;i!=model.npose;i++) {
		dialog_add_combo_item(dialog_animation_settings_wind,kAnimationPosePose,0,model.poses[i].name,FOUR_CHAR_CODE('\?\?\?\?'));
	}
	
		// fill mesh combo (leave none in list)

	for (i=0;i!=model.nmesh;i++) {
		dialog_add_combo_item(dialog_animation_settings_wind,kAnimationMeshName,0,model.meshes[i].name,FOUR_CHAR_CODE('\?\?\?\?'));
	}
	
		// load setting data
		
	dialog_set_text(dialog_animation_settings_wind,kAnimationName,0,model.animates[animate_idx].name);
	dialog_set_boolean(dialog_animation_settings_wind,kAnimationLoop,0,model.animates[animate_idx].loop);
	dialog_set_focus(dialog_animation_settings_wind,kAnimationName,0);
	
		// load pose move data
		
	dialog_pose_move_settings_load();

		// show window
	
	ShowWindow(dialog_animation_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(animation_settings_event_proc);
	InstallWindowEventHandler(dialog_animation_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window and timer

	timer_upp=NewEventLoopTimerUPP(pose_move_timer);
	InstallEventLoopTimer(GetCurrentEventLoop(),0,0.01,timer_upp,NULL,&timer_event);
		
	RunAppModalLoopForWindow(dialog_animation_settings_wind);

	RemoveEventLoopTimer(timer_event);
	DisposeEventLoopTimerUPP(timer_upp);
	
		// dialog to data
		
	dialog_get_text(dialog_animation_settings_wind,kAnimationName,0,model.animates[animate_idx].name,name_str_len);
	model.animates[animate_idx].loop=dialog_get_boolean(dialog_animation_settings_wind,kAnimationLoop,0);
	
	dialog_pose_move_settings_save();
	
	reset_animate_tab(dialog_animate_idx,dialog_pose_move_idx);

		// close window
		
	DisposeDataBrowserItemDataUPP(list_item_upp);
	DisposeDataBrowserItemNotificationUPP(list_notify_upp);
	DisposeWindow(dialog_animation_settings_wind);
}

