/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Particle Setting Dialog

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "window.h"
#include "dialog.h"
#include "model.h"

#define kAnimationParticleName				FOUR_CHAR_CODE('prtn')
#define kAnimationParticleBone				FOUR_CHAR_CODE('pbne')
#define kAnimationParticleMotion			FOUR_CHAR_CODE('psim')
#define kAnimationParticleMotionFactor		FOUR_CHAR_CODE('psmf')
#define kAnimationParticleRotation			FOUR_CHAR_CODE('psir')
#define kAnimationParticleStick				FOUR_CHAR_CODE('pssm')
#define kAnimationParticleSlopX				FOUR_CHAR_CODE('pslx')
#define kAnimationParticleSlopY				FOUR_CHAR_CODE('psly')
#define kAnimationParticleSlopZ				FOUR_CHAR_CODE('pslz')

bool						dialog_particle_settings_cancel;
WindowRef					dialog_particle_settings_wind;

/* =======================================================

      Particle Setting Event Handlers
      
======================================================= */

static pascal OSStatus particle_settings_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kHICommandCancel:
					dialog_particle_settings_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_particle_settings_wind);
					return(noErr);

				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_particle_settings_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Particle Setting
      
======================================================= */

bool dialog_particle_settings_run(model_particle_type *particle)
{
	EventHandlerUPP					event_upp;
	EventTypeSpec					event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_particle_settings_wind,"ParticleSettings");
	
		// set controls

	dialog_special_combo_fill_particle(dialog_particle_settings_wind,kAnimationParticleName,0,particle->name);
	dialog_set_bone_combo(dialog_particle_settings_wind,kAnimationParticleBone,0,particle->bone_idx);
	dialog_set_boolean(dialog_particle_settings_wind,kAnimationParticleMotion,0,particle->motion);
	dialog_set_float(dialog_particle_settings_wind,kAnimationParticleMotionFactor,0,particle->motion_factor);
	dialog_set_boolean(dialog_particle_settings_wind,kAnimationParticleRotation,0,particle->rotate);
	dialog_set_boolean(dialog_particle_settings_wind,kAnimationParticleStick,0,particle->stick);
	dialog_set_int(dialog_particle_settings_wind,kAnimationParticleSlopX,0,particle->slop.x);
	dialog_set_int(dialog_particle_settings_wind,kAnimationParticleSlopY,0,particle->slop.y);
	dialog_set_int(dialog_particle_settings_wind,kAnimationParticleSlopZ,0,particle->slop.z);

		// show window
	
	ShowWindow(dialog_particle_settings_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(particle_settings_event_proc);
	InstallWindowEventHandler(dialog_particle_settings_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_particle_settings_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_particle_settings_wind);
	
		// dialog to data
		
	if (!dialog_particle_settings_cancel) {
		dialog_special_combo_get_particle(dialog_particle_settings_wind,kAnimationParticleName,0,particle->name,name_str_len);
		particle->bone_idx=dialog_get_bone_combo(dialog_particle_settings_wind,kAnimationParticleBone,0);
		particle->motion=dialog_get_boolean(dialog_particle_settings_wind,kAnimationParticleMotion,0);
		particle->motion_factor=dialog_get_float(dialog_particle_settings_wind,kAnimationParticleMotionFactor,0);
		particle->rotate=dialog_get_boolean(dialog_particle_settings_wind,kAnimationParticleRotation,0);
		particle->stick=dialog_get_boolean(dialog_particle_settings_wind,kAnimationParticleStick,0);
		particle->slop.x=dialog_get_int(dialog_particle_settings_wind,kAnimationParticleSlopX,0);
		particle->slop.y=dialog_get_int(dialog_particle_settings_wind,kAnimationParticleSlopY,0);
		particle->slop.z=dialog_get_int(dialog_particle_settings_wind,kAnimationParticleSlopZ,0);
	}
	
		// close window
		
	DisposeWindow(dialog_particle_settings_wind);
	
	return(!dialog_particle_settings_cancel);
}

