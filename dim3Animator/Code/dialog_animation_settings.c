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

extern bool				play_animate;

extern model_type		model;

#define kAnimationPoseTabCount				6
#define kAnimationPoseTab					FOUR_CHAR_CODE('tabb')

#define kAnimationName						FOUR_CHAR_CODE('name')
#define kAnimationLoop						FOUR_CHAR_CODE('loop')

#define kAnimationPoseList					FOUR_CHAR_CODE('psls')
#define kAniamtionPostListNameColumn		FOUR_CHAR_CODE('pnme')

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

#define kAnimationSoundBone					FOUR_CHAR_CODE('sbne')
#define kAnimationSoundName					FOUR_CHAR_CODE('sndn')
#define kAnimationSoundPitch				FOUR_CHAR_CODE('sndp')
#define kAnimationSoundGlobal				FOUR_CHAR_CODE('sglb')

#define kAnimationParticleList				FOUR_CHAR_CODE('part')
#define kAnimationParticleListNameColumn	FOUR_CHAR_CODE('pnme')
#define kAnimationParticleListBoneColumn	FOUR_CHAR_CODE('ebne')
#define kAnimationParticleAdd				FOUR_CHAR_CODE('padd')
#define kAnimationParticleSub				FOUR_CHAR_CODE('psub')

#define kAnimationRingList					FOUR_CHAR_CODE('ring')
#define kAnimationRingListNameColumn		FOUR_CHAR_CODE('pnme')
#define kAnimationRingListBoneColumn		FOUR_CHAR_CODE('ebne')
#define kAnimationRingAdd					FOUR_CHAR_CODE('radd')
#define kAnimationRingSub					FOUR_CHAR_CODE('rsub')

#define kAnimationFlashBone					FOUR_CHAR_CODE('fbne')
#define kAnimationFlashIntensity			FOUR_CHAR_CODE('fint')
#define kAnimationFlashColor				FOUR_CHAR_CODE('fcol')
#define kAnimationFlashColorEdit			FOUR_CHAR_CODE('colb')
#define kAnimationFlashLiveTime				FOUR_CHAR_CODE('flvt')
#define kAnimationFlashFadeTime				FOUR_CHAR_CODE('ffdt')

#define kAnimationShakeDistance				FOUR_CHAR_CODE('skds')
#define kAnimationShakeSize					FOUR_CHAR_CODE('sksz')
#define kAnimationShakeLiveTime				FOUR_CHAR_CODE('sklf')

#define kAnimationMeshName					FOUR_CHAR_CODE('mshn')
#define kAnimationMeshFadeInTime			FOUR_CHAR_CODE('msfi')
#define kAnimationMeshFadeLifeTime			FOUR_CHAR_CODE('msfl')
#define kAnimationMeshFadeOutTime			FOUR_CHAR_CODE('msfo')

WindowRef					dialog_animation_settings_wind;
ControlRef					dialog_pose_move_list,dialog_particle_list,dialog_ring_list;

int							dialog_animate_idx,dialog_pose_move_idx,dialog_particle_idx,dialog_ring_idx;
bool						dialog_pose_move_change_ok,dialog_animation_settings_cancel;

model_animate_type			animate_backup;

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

	dialog_set_bone_combo(dialog_animation_settings_wind,kAnimationSoundBone,0,pose_move->sound.bone_idx);
	dialog_special_combo_fill_sound(dialog_animation_settings_wind,kAnimationSoundName,0,pose_move->sound.name);
	dialog_set_float(dialog_animation_settings_wind,kAnimationSoundPitch,0,pose_move->sound.pitch);
	dialog_set_boolean(dialog_animation_settings_wind,kAnimationSoundGlobal,0,pose_move->sound.no_position);
	
	dialog_particle_idx=-1;
	
	RemoveDataBrowserItems(dialog_particle_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	AddDataBrowserItems(dialog_particle_list,kDataBrowserNoItem,pose_move->particle.count,NULL,kDataBrowserItemNoProperty);

	dialog_ring_idx=-1;
	
	RemoveDataBrowserItems(dialog_ring_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	AddDataBrowserItems(dialog_ring_list,kDataBrowserNoItem,pose_move->ring.count,NULL,kDataBrowserItemNoProperty);
	
	dialog_set_bone_combo(dialog_animation_settings_wind,kAnimationFlashBone,0,pose_move->flash.bone_idx);
	dialog_set_int(dialog_animation_settings_wind,kAnimationFlashIntensity,0,pose_move->flash.intensity);
	dialog_set_int(dialog_animation_settings_wind,kAnimationFlashLiveTime,0,pose_move->flash.flash_msec);
	dialog_set_int(dialog_animation_settings_wind,kAnimationFlashFadeTime,0,pose_move->flash.fade_msec);
	
	dialog_set_color(dialog_animation_settings_wind,kAnimationFlashColor,0,&pose_move->flash.col);
	
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
	dialog_set_int(dialog_animation_settings_wind,kAnimationMeshFadeLifeTime,0,pose_move->mesh_fade.fade_life_msec);
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

	pose_move->sound.bone_idx=dialog_get_bone_combo(dialog_animation_settings_wind,kAnimationSoundBone,0);
	dialog_special_combo_get_sound(dialog_animation_settings_wind,kAnimationSoundName,0,pose_move->sound.name,name_str_len);
	pose_move->sound.pitch=dialog_get_float(dialog_animation_settings_wind,kAnimationSoundPitch,0);
	pose_move->sound.no_position=dialog_get_boolean(dialog_animation_settings_wind,kAnimationSoundGlobal,0);
	
	pose_move->flash.intensity=dialog_get_int(dialog_animation_settings_wind,kAnimationFlashIntensity,0);
	pose_move->flash.flash_msec=dialog_get_int(dialog_animation_settings_wind,kAnimationFlashLiveTime,0);
	pose_move->flash.fade_msec=dialog_get_int(dialog_animation_settings_wind,kAnimationFlashFadeTime,0);
	
	pose_move->flash.bone_idx=dialog_get_bone_combo(dialog_animation_settings_wind,kAnimationFlashBone,0);
	dialog_get_color(dialog_animation_settings_wind,kAnimationFlashColor,0,&pose_move->flash.col);
	
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
	pose_move->mesh_fade.fade_life_msec=dialog_get_int(dialog_animation_settings_wind,kAnimationMeshFadeLifeTime,0);
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

void dialog_particle_list_reset(void)
{
	dialog_particle_idx=-1;
	
	RemoveDataBrowserItems(dialog_particle_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	AddDataBrowserItems(dialog_particle_list,kDataBrowserNoItem,model.animates[dialog_animate_idx].pose_moves[dialog_pose_move_idx].particle.count,NULL,kDataBrowserItemNoProperty);

	Draw1Control(dialog_particle_list);
}

void dialog_ring_list_reset(void)
{
	dialog_ring_idx=-1;
	
	RemoveDataBrowserItems(dialog_ring_list,kDataBrowserNoItem,0,NULL,kDataBrowserItemNoProperty);
	AddDataBrowserItems(dialog_ring_list,kDataBrowserNoItem,model.animates[dialog_animate_idx].pose_moves[dialog_pose_move_idx].ring.count,NULL,kDataBrowserItemNoProperty);

	Draw1Control(dialog_ring_list);
}

static pascal OSStatus animation_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
		
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
					// pose list buttons
					
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

					// dialog buttons
			
				case kAnimationPosePlay:
					model_wind_play(!play_animate,FALSE);
					return(noErr);
					
				case kHICommandCancel:
					dialog_animation_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_animation_settings_wind);
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
	int				idx,event_class,event_kind;
	HICommand		cmd;
	
	event_class=GetEventClass(event);
	event_kind=GetEventKind(event);
	
	if ((event_class==kEventClassCommand) && (event_kind==kEventProcessCommand)) {
			
		GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
		
		switch (cmd.commandID) {
				
				// particle list buttons
				
			case kAnimationParticleAdd:
				idx=model_animate_add_particle(&model,dialog_animate_idx,dialog_pose_move_idx);
				if (idx==-1) return(noErr);
			
				if (!dialog_particle_settings_run(&model.animates[dialog_animate_idx].pose_moves[dialog_pose_move_idx].particle.particles[idx])) {
					model_animate_delete_particle(&model,dialog_animate_idx,dialog_pose_move_idx,idx);
				}
				
				dialog_particle_list_reset();
				return(noErr);
				
			case kAnimationParticleSub:
				if (dialog_particle_idx!=-1) model_animate_delete_particle(&model,dialog_animate_idx,dialog_pose_move_idx,dialog_particle_idx);
				dialog_particle_list_reset();
				return(noErr);
				
				// ring list buttons
				
			case kAnimationRingAdd:
				idx=model_animate_add_ring(&model,dialog_animate_idx,dialog_pose_move_idx);
				if (idx==-1) return(noErr);
			
				if (!dialog_ring_settings_run(&model.animates[dialog_animate_idx].pose_moves[dialog_pose_move_idx].ring.rings[idx])) {
					model_animate_delete_ring(&model,dialog_animate_idx,dialog_pose_move_idx,idx);
				}
				
				dialog_ring_list_reset();
				return(noErr);
				
			case kAnimationRingSub:
				if (dialog_ring_idx!=-1) model_animate_delete_ring(&model,dialog_animate_idx,dialog_pose_move_idx,dialog_ring_idx);
				dialog_ring_list_reset();
				return(noErr);
				
		}
			
		return(eventNotHandledErr);
	}
	
	if ((event_class==kEventClassControl) && (event_kind==kEventControlHit)) {
		dialog_switch_tab(dialog_animation_settings_wind,kAnimationPoseTab,0,kAnimationPoseTabCount);
	}
	
	return(eventNotHandledErr);
}

static pascal OSStatus pose_move_setting_button_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	dialog_click_color(dialog_animation_settings_wind,kAnimationFlashColor,0);
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
	
		case kAniamtionPostListNameColumn:
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

      Particle List Event Handlers
      
======================================================= */

static pascal OSStatus particle_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int						idx;
	CFStringRef				cfstr;
	model_particle_type		*particle;

	idx=itemID-1;
	particle=&model.animates[dialog_animate_idx].pose_moves[dialog_pose_move_idx].particle.particles[idx];

	switch (property) {
	
		case kAnimationParticleListNameColumn:
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,particle->name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kAnimationParticleListBoneColumn:
			if (particle->bone_idx==-1) return(noErr);
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,model.bones[particle->bone_idx].name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);			

	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void particle_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	switch (message) {

		case kDataBrowserItemSelected:
			dialog_particle_idx=itemID-1;
			break;

		case kDataBrowserItemDoubleClicked:
			dialog_particle_idx=itemID-1;
			dialog_particle_settings_run(&model.animates[dialog_animate_idx].pose_moves[dialog_pose_move_idx].particle.particles[dialog_particle_idx]);
			break;

	}
}

/* =======================================================

      Ring List Event Handlers
      
======================================================= */

static pascal OSStatus ring_list_item_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserPropertyID property,DataBrowserItemDataRef itemData,Boolean changeValue)
{
	int						idx;
	CFStringRef				cfstr;
	model_ring_type			*ring;
	
	idx=itemID-1;
	ring=&model.animates[dialog_animate_idx].pose_moves[dialog_pose_move_idx].ring.rings[idx];

	switch (property) {
	
		case kAnimationRingListNameColumn:
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,ring->name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);
			
		case kAnimationRingListBoneColumn:
			if (ring->bone_idx==-1) return(noErr);
			
			cfstr=CFStringCreateWithCString(kCFAllocatorDefault,model.bones[ring->bone_idx].name,kCFStringEncodingMacRoman);
			SetDataBrowserItemDataText(itemData,cfstr);
			CFRelease(cfstr);
			return(noErr);			

	}

	return(errDataBrowserPropertyNotSupported);
}

static pascal void ring_list_notify_proc(ControlRef ctrl,DataBrowserItemID itemID,DataBrowserItemNotification message)
{
	switch (message) {

		case kDataBrowserItemSelected:
			dialog_ring_idx=itemID-1;
			break;

		case kDataBrowserItemDoubleClicked:
			dialog_ring_idx=itemID-1;
			dialog_ring_settings_run(&model.animates[dialog_animate_idx].pose_moves[dialog_pose_move_idx].ring.rings[dialog_ring_idx]);
			break;

	}
}

/* =======================================================

      Run Pose Move Setting
      
======================================================= */

bool dialog_animation_settings_run(int animate_idx)
{
	int								i,pose_move_idx;
	ControlRef						ctrl;
	ControlID						ctrl_id;
	DataBrowserItemID				itemID;
	DataBrowserCallbacks			dbcall;
	DataBrowserItemDataUPP			pose_list_item_upp,particle_list_item_upp,ring_list_item_upp;
	DataBrowserItemNotificationUPP	pose_list_notify_upp,particle_list_notify_upp,ring_list_notify_upp;
	EventHandlerUPP					event_upp,tab_event_upp,button_event_upp;
	EventLoopTimerRef				timer_event;
	EventLoopTimerUPP				timer_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}},
									tab_event_list[]={{kEventClassCommand,kEventProcessCommand},
													  {kEventClassControl,kEventControlHit}},
									button_event_list[]={{kEventClassControl,kEventControlHit}};
	
		// backup animation for cancel
		
	memmove(&animate_backup,&model.animates[animate_idx],sizeof(model_animate_type));
	
		// if there is no pose moves or new animation,
		// then we need to add a default pose
	
	pose_move_idx=0;
			
	if (model.animates[animate_idx].npose_move==0) {
		pose_move_idx=model_animate_pose_insert(&model,animate_idx,-1,0);
	}
	
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
		
	ctrl_id.signature=kAnimationFlashColor;
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
	
	pose_list_item_upp=NewDataBrowserItemDataUPP(&pose_list_item_proc);
	dbcall.u.v1.itemDataCallback=pose_list_item_upp;

	pose_list_notify_upp=NewDataBrowserItemNotificationUPP(&pose_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=pose_list_notify_upp;
	
	SetDataBrowserCallbacks(dialog_pose_move_list,&dbcall);
	
	AddDataBrowserItems(dialog_pose_move_list,kDataBrowserNoItem,model.animates[dialog_animate_idx].npose_move,NULL,kDataBrowserItemNoProperty);
	
	dialog_pose_move_change_ok=FALSE;
	
	itemID=pose_move_idx+1;
	SetDataBrowserSelectedItems(dialog_pose_move_list,1,&itemID,kDataBrowserItemsAssign);
	
	dialog_pose_move_change_ok=TRUE;
	
		// setup particle list

	ctrl_id.signature=kAnimationParticleList;
	ctrl_id.id=0;
	GetControlByID(dialog_animation_settings_wind,&ctrl_id,&dialog_particle_list);
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	particle_list_item_upp=NewDataBrowserItemDataUPP(&particle_list_item_proc);
	dbcall.u.v1.itemDataCallback=particle_list_item_upp;

	particle_list_notify_upp=NewDataBrowserItemNotificationUPP(&particle_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=particle_list_notify_upp;
	
	SetDataBrowserCallbacks(dialog_particle_list,&dbcall);
	
		// setup ring list
		
	ctrl_id.signature=kAnimationRingList;
	ctrl_id.id=0;
	GetControlByID(dialog_animation_settings_wind,&ctrl_id,&dialog_ring_list);
	
	dbcall.version=kDataBrowserLatestCallbacks;
	InitDataBrowserCallbacks(&dbcall);
	
	ring_list_item_upp=NewDataBrowserItemDataUPP(&ring_list_item_proc);
	dbcall.u.v1.itemDataCallback=ring_list_item_upp;

	ring_list_notify_upp=NewDataBrowserItemNotificationUPP(&ring_list_notify_proc);
	dbcall.u.v1.itemNotificationCallback=ring_list_notify_upp;
	
	SetDataBrowserCallbacks(dialog_ring_list,&dbcall);

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
	
	dialog_animation_settings_cancel=FALSE;

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

		// close window
		
	DisposeDataBrowserItemDataUPP(pose_list_item_upp);
	DisposeDataBrowserItemNotificationUPP(pose_list_notify_upp);
	
	DisposeDataBrowserItemDataUPP(particle_list_item_upp);
	DisposeDataBrowserItemNotificationUPP(particle_list_notify_upp);
	
	DisposeDataBrowserItemDataUPP(ring_list_item_upp);
	DisposeDataBrowserItemNotificationUPP(ring_list_notify_upp);
	
	DisposeWindow(dialog_animation_settings_wind);
	
		// if cancel, reset animation
		
	if (dialog_animation_settings_cancel) {
		memmove(&model.animates[animate_idx],&animate_backup,sizeof(model_animate_type));
	}
		
	reset_animate_list();
	
	return(!dialog_animation_settings_cancel);
}

