/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: proj.hit object

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

#include "scripts.h"
#include "objects.h"
#include "projectiles.h"

extern map_type			map;
extern server_type		server;
extern js_type			js;

JSBool js_get_proj_hit_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp);

JSClass			proj_hit_class={"proj_hit_class",0,
							script_add_property,JS_PropertyStub,
							js_get_proj_hit_property,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSPropertySpec	proj_hit_props[]={
							{"type",				proj_hit_prop_type,					JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"name",				proj_hit_prop_name,					JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"isPlayer",			proj_hit_prop_is_player,			JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"startTick",			proj_hit_prop_start_tick,			JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{"materialName",		proj_hit_prop_material_name,		JSPROP_READONLY|JSPROP_PERMANENT|JSPROP_SHARED},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_proj_hit_object(JSObject *parent_obj)
{
    JSObject		*j_obj;

	j_obj=JS_DefineObject(js.cx,parent_obj,"hit",&proj_hit_class,NULL,0);
	JS_DefineProperties(js.cx,j_obj,proj_hit_props);
}

/* =======================================================

      Properties
      
======================================================= */

int js_get_proj_hit_type(proj_type *proj)
{
	int					portal_idx;

		// check auto hits
		
	if (proj->action.hit_tick!=0) {
		if ((proj->start_tick+proj->action.hit_tick)<=js.time.current_tick) return(sd_proj_hit_type_auto);
	}

		// object or projectile hits

    if (proj->contact.obj_uid!=-1) return(sd_proj_hit_type_object);
	if (proj->contact.proj_uid!=-1) return(sd_proj_hit_type_projectile);
	if (proj->contact.melee) return(sd_proj_hit_type_melee);

		// map hits
		// we could polygons with common x/z as
		// wall type hits.  We determine floor/ceiling
		// hits by proximity to projectile Y

	portal_idx=proj->contact.hit_poly.portal_idx;
	if (portal_idx==-1) return(sd_proj_hit_type_none);

	return(sd_proj_hit_type_mesh);
}

void js_get_proj_hit_name(proj_type *proj,int hit_type,char *name)
{
	obj_type			*hit_obj;
	proj_type			*hit_proj;
	proj_setup_type		*hit_proj_setup;
	
	switch (hit_type) {
	
		case sd_proj_hit_type_object:
			hit_obj=object_find_uid(proj->contact.obj_uid);
			strcpy(name,hit_obj->name);
			return;
		
		case sd_proj_hit_type_projectile:
			hit_proj=projectile_find_uid(proj->contact.proj_uid);
			hit_proj_setup=proj_setups_find_uid(hit_proj->proj_setup_uid);
			strcpy(name,hit_proj_setup->name);
			return;
		
		default:
			name[0]=0x0;
			return;
			
	}
}

void js_get_proj_hit_material_name(proj_type *proj,int hit_type,char *name)
{
	int					portal_idx,mesh_idx,poly_idx;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;

		// get hit poly

	portal_idx=proj->contact.hit_poly.portal_idx;
	if (portal_idx==-1) {
		name[0]=0x0;
		return;
	}

	mesh_idx=proj->contact.hit_poly.mesh_idx;
	poly_idx=proj->contact.hit_poly.poly_idx;

	mesh_poly=&map.portals[portal_idx].mesh.meshes[mesh_idx].polys[poly_idx];
	
		// get material name

	texture=&map.textures[mesh_poly->txt_idx];
	strcpy(name,texture->material_name);
}

JSBool js_get_proj_hit_property(JSContext *cx,JSObject *j_obj,jsval id,jsval *vp)
{
	int					hit_type;
	char				hit_name[name_str_len];
	proj_type			*proj;

	if (!JSVAL_IS_INT(id)) return(JS_TRUE);

	proj=proj_get_attach();
	if (proj==NULL) return(JS_TRUE);

	switch (JSVAL_TO_INT(id)) {
	
		case proj_hit_prop_type:
			*vp=INT_TO_JSVAL(js_get_proj_hit_type(proj));
			break;
		case proj_hit_prop_name:
			hit_type=js_get_proj_hit_type(proj);
			js_get_proj_hit_name(proj,hit_type,hit_name);
			*vp=script_string_to_value(hit_name);
			break;
		case proj_hit_prop_is_player:
			*vp=BOOLEAN_TO_JSVAL(proj->contact.obj_uid==server.player_obj_uid);
			break;
		case proj_hit_prop_start_tick:
			*vp=INT_TO_JSVAL(proj->start_tick);
			break;
		case proj_hit_prop_material_name:
			hit_type=js_get_proj_hit_type(proj);
			js_get_proj_hit_material_name(proj,hit_type,hit_name);
			*vp=script_string_to_value(hit_name);
			break;
	}
	
	return(JS_TRUE);
}

