/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Hand

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "weapons.h"
#include "models.h"
#include "consoles.h"
#include "interfaces.h"
#include "video.h"

extern map_type				map;
extern camera_type			camera;
extern server_type			server;
extern view_type			view;

extern void model_render(int tick,model_draw *draw);

/* =======================================================

      Draw Weapon in Hand
      
======================================================= */

void draw_weapon_hand(int tick,obj_type *obj,weapon_type *weap)
{
    model_draw		*draw;
	model_type		*mdl;
	
		// weapons hidden or zoom on?
		
	if (obj->hide_all_weapons) return;
	if ((weap->zoom.on) && (weap->zoom.active)) return;
	
		// weapon model draw
		
	draw=&weap->draw;
	if ((draw->uid==-1) || (!draw->on)) return;
	
	mdl=model_find_uid(draw->uid);
	if (mdl==NULL) return;
	
		// always draw weapons over view
		// without rotation
		
	gl_3D_view();
	gl_setup_project();
	
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);

		// regular weapon model
	
	model_clear_draw_setup(mdl,&draw->setup);
	model_draw_setup_weapon(tick,obj,weap,FALSE,FALSE);

	model_calc_animation(draw);
	model_calc_draw_bones(draw);
	
	model_render(tick,draw);

		// dual wielded weapons

	if ((weap->dual.on) && (weap->dual.active)) {
		draw=&weap->draw_dual;
		
		model_clear_draw_setup(mdl,&draw->setup);
		model_draw_setup_weapon(tick,obj,weap,FALSE,TRUE);

		model_calc_animation(draw);
		model_calc_draw_bones(draw);
		
		model_render(tick,draw);
	}
}

