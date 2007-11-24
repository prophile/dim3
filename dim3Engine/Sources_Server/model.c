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

#include "models.h"
#include "consoles.h"

extern server_type		server;
extern setup_type		setup;

/* =======================================================

      Start Model Data
      
======================================================= */

void model_initialize(void)
{
	server.count.model=0;
	server.uid.model=0;
}

/* =======================================================

      Find Models
      
======================================================= */

model_type* model_find_uid(int uid)
{
	int				i;
	model_type		*mdl;

	mdl=server.models;
	
	for (i=0;i!=server.count.model;i++) {
		if (mdl->uid==uid) return(mdl);
		mdl++;
	}
	
	return(NULL);
}

model_type* model_find(char *name)
{
	int				i;
	model_type		*mdl;

	mdl=server.models;
	
	for (i=0;i!=server.count.model;i++) {
		if (strcmp(mdl->name,name)==0) return(mdl);
		mdl++;
	}
	
	return(NULL);
}

/* =======================================================

      Write Model Shader Errors to Console
      
======================================================= */

void model_shader_errors_write_console(model_type *model)
{
	int				n;
	texture_type	*texture;
	
	texture=model->textures;
	
	for (n=0;n!=max_model_texture;n++) {
		if ((texture->shader.on) && (texture->shader.init_error[0]!=0x0)) console_add_error(texture->shader.init_error);
		texture++;
	}
}

/* =======================================================

      Open Models
      
======================================================= */

model_type* model_load_single(char *name,int bind,bool load_shaders)
{
	model_type			*mdl;
	
		// has model been already loaded?
	
	mdl=model_find(name);
	if (mdl!=NULL) {
		if (mdl->bind==bind) return(mdl);
	}
	
		// new model
		
	if (server.count.model>=max_model) return(NULL);
	
	mdl=&server.models[server.count.model];
	
		// load model
	
	model_setup(&setup.file_path_setup,setup.anisotropic_mode,setup.texture_quality_mode,setup.mipmap_mode,setup.mipmap_card_generated,setup.texture_compression);
	if (!model_open(mdl,name,bind,TRUE,TRUE,load_shaders)) return(NULL);

	model_shader_errors_write_console(mdl);

		// setup some animation indexes to avoid name lookups

	model_animation_effect_setup(mdl);
	
		// new unique ID
		
	mdl->uid=server.uid.model;
	server.uid.model++;
	
	server.count.model++;
	
	return(mdl);
}

void model_load_and_init_single(model_draw *draw,int bind,bool load_shaders)
{
	char				err_str[256];
	model_type			*mdl;
	
	draw->uid=-1;
	draw->center.x=draw->center.y=draw->center.z=0;
	draw->size.x=draw->size.y=draw->size.z=0;
	
	if (draw->name[0]!=0x0) {
		mdl=model_load_single(draw->name,bind,load_shaders);
		if (mdl!=NULL) {
			draw->uid=mdl->uid;
			model_get_size(mdl,&draw->size.x,&draw->size.y,&draw->size.z);
			memmove(&draw->center,&mdl->center,sizeof(d3pnt));
		}
		else {
			draw->on=FALSE;
			sprintf(err_str,"Unable to load model named %s",draw->name);
			console_add_error(err_str);
		}
	}
	else {
		draw->on=FALSE;
	}
	
	model_stop_animation(draw);
	model_fade_clear(draw);
}

void models_load(int bind,bool load_shaders)
{
	int					i;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	
	obj=server.objs;
	
	for (i=0;i!=server.count.obj;i++) {
		if (obj->bind==bind) model_load_and_init_single(&obj->draw,bind,load_shaders);
		obj++;
	}

	weap=server.weapons;
	
	for (i=0;i!=server.count.weapon;i++) {
		if (weap->bind==bind) model_load_and_init_single(&weap->draw,bind,load_shaders);
		weap++;
	}

	proj_setup=server.proj_setups;
	
    for (i=0;i!=server.count.proj_setup;i++) {
		if (proj_setup->bind==bind) model_load_and_init_single(&proj_setup->draw,bind,load_shaders);
		proj_setup++;
	}
}

/* =======================================================

      Dispose Models
      
======================================================= */

void models_dispose(int bind)
{
	int					i;
	model_type			*model;
	
	i=0;
	
	while (i<server.count.model) {
		model=&server.models[i];
		if (model->bind!=bind) {
			i++;
			continue;
		}
		
		model_close(model);
	
		if (i<(server.count.model-1)) {
			memmove(&server.models[i],&server.models[i+1],(sizeof(model_type)*((server.count.model-i)-1)));
		}
		
		server.count.model--;
		if (server.count.model==0) break;
	}
}

