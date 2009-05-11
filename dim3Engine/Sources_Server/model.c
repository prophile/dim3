/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Main Model

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

#include "projectiles.h"
#include "models.h"
#include "consoles.h"
#include "video.h"

extern server_type		server;
extern setup_type		setup;

/* =======================================================

      Start Model Data
      
======================================================= */

void model_initialize(void)
{
	server.models=NULL;
	server.count.model=0;
	server.uid.model=0;
}

/* =======================================================

      Find Models
      
======================================================= */

int model_find_uid_index(int uid)
{
	int				n;
	model_type		*mdl;

	mdl=server.models;
	
	for (n=0;n!=server.count.model;n++) {
		if (mdl->uid==uid) return(n);
		mdl++;
	}
	
	return(-1);
}

model_type* model_find_uid(int uid)
{
	int				n;
	model_type		*mdl;

	mdl=server.models;
	
	for (n=0;n!=server.count.model;n++) {
		if (mdl->uid==uid) return(mdl);
		mdl++;
	}
	
	return(NULL);
}

model_type* model_find(char *name)
{
	int				n;
	model_type		*mdl;

	mdl=server.models;
	
	for (n=0;n!=server.count.model;n++) {
		if (strcasecmp(mdl->name,name)==0) return(mdl);
		mdl++;
	}
	
	return(NULL);
}

/* =======================================================

      Open Models
      
======================================================= */

model_type* model_load_single(char *name)
{
	model_type		*mdl,*ptr;
	
		// has model been already loaded?
		// if so, return model and increment reference count
	
	mdl=model_find(name);
	if (mdl!=NULL) {
		mdl->reference_count++;
		return(mdl);
	}

		// only allow a maximum number of models

	if (server.count.model>=max_model) return(NULL);

		// create memory for new model

	ptr=(model_type*)malloc(sizeof(model_type)*(server.count.model+1));
	if (ptr==NULL) return(NULL);

	if (server.models!=NULL) {
		memmove(ptr,server.models,(sizeof(model_type)*server.count.model));
		free(server.models);
	}

	server.models=ptr;

	mdl=&server.models[server.count.model];
	server.count.model++;

		// load model

	model_setup(&setup.file_path_setup,setup.anisotropic_mode,setup.mipmap_mode,setup.texture_compression);
	if (!model_open(mdl,name,TRUE)) {
		server.count.model--;		// error loading, leave memory as is an fix next model load
		return(NULL);
	}

	gl_shader_attach_model(mdl);

		// setup some animation indexes to avoid name lookups

	model_animation_effect_setup(mdl);

		// start reference count at 1

	mdl->reference_count=1;

		// new unique ID
		
	mdl->uid=server.uid.model;
	server.uid.model++;
	
	return(mdl);
}

void model_load_and_init(model_draw *draw)
{
	int					n;
	char				err_str[256];
	model_type			*mdl;

	draw->uid=-1;
	draw->center.x=draw->center.y=draw->center.z=0;
	draw->size.x=draw->size.y=draw->size.z=0;

	draw->script_animation_idx=0;
	draw->script_halo_idx=0;
	draw->script_light_idx=0;
	
	mdl=NULL;
	
	if (draw->name[0]!=0x0) {
		mdl=model_load_single(draw->name);
		if (mdl!=NULL) {
			draw->uid=mdl->uid;
			model_get_size(mdl,&draw->size.x,&draw->size.y,&draw->size.z);
			memmove(&draw->center,&mdl->center,sizeof(d3pnt));
		}
		else {
			draw->on=FALSE;
			draw->uid=-1;
			sprintf(err_str,"Unable to load model named %s",draw->name);
			console_add_error(err_str);
		}
	}
	else {
		draw->on=FALSE;
	}
	
		// initialize draw memory
		
	if (mdl!=NULL) model_draw_setup_initialize(mdl,&draw->setup,TRUE);

		// stop all animations and fades

	for (n=0;n!=max_model_blend_animation;n++) {
		draw->script_animation_idx=n;
		model_stop_animation(draw);
	}

	draw->script_animation_idx=0;

	model_fade_clear(draw);
}

/* =======================================================

      Dispose Models
      
======================================================= */

void models_dispose(model_draw *draw)
{
	int					idx;
	model_type			*mdl,*ptr;

		// was model loaded?
		
	if (draw->uid==-1) return;

		// find model

	idx=model_find_uid_index(draw->uid);
	if (idx==-1) return;
	
	mdl=&server.models[idx];
	
		// clear draw memory
		
	model_draw_setup_shutdown(mdl,&draw->setup);

		// decrement reference count

	mdl->reference_count--;
	if (mdl->reference_count>0) return;

		// dispose model

	model_close(mdl);

		// is the list completely empty?

	if (server.count.model==1) {
		free(server.models);
		server.models=NULL;
		server.count.model=0;
		return;
	}

		// if for some reason we can't create new
		// memory, just shuffle the list and wait
		// until next time

	ptr=(model_type*)malloc(sizeof(model_type)*(server.count.model-1));

	if (ptr==NULL) {
		if (idx<(server.count.model-1)) {
			memmove(&server.models[idx],&server.models[idx+1],(sizeof(model_type)*((server.count.model-idx)-1)));
		}
	}
	else {

		if (idx>0) {
			memmove(ptr,server.models,(sizeof(model_type)*idx));
		}
		if (idx<(server.count.model-1)) {
			memmove(&ptr[idx],&server.models[idx+1],(sizeof(model_type)*((server.count.model-idx)-1)));
		}

		free(server.models);
		server.models=ptr;
	}
	
	server.count.model--;
}

/* =======================================================

      Reset Model UIDs after Load
      
======================================================= */

void models_reset_uid_single(model_draw *draw)
{
	model_type			*mdl;

	mdl=model_find(draw->name);
	if (mdl==NULL) {
		draw->on=FALSE;
		return;
	}

	draw->uid=mdl->uid;
}

void models_reset_uid(void)
{
	int					i;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	proj_type			*proj;
	
	obj=server.objs;
	
	for (i=0;i!=server.count.obj;i++) {
		models_reset_uid_single(&obj->draw);
		obj++;
	}

	weap=server.weapons;
	
	for (i=0;i!=server.count.weapon;i++) {
		models_reset_uid_single(&weap->draw);
		weap++;
	}

	proj_setup=server.proj_setups;
	
    for (i=0;i!=server.count.proj_setup;i++) {
		models_reset_uid_single(&proj_setup->draw);
		proj_setup++;
	}

	proj=server.projs;
	
    for (i=0;i!=server.count.proj;i++) {
		models_reset_uid_single(&proj->draw);
		proj++;
	}
}

