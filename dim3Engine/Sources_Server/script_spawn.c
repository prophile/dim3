/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Script: spawn object

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "effects.h"
#include "scripts.h"
#include "physics.h"

extern server_type		server;
extern js_type			js;

JSBool js_spawn_particle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_spawn_particle_moving_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_spawn_particle_line_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_spawn_ring_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_spawn_ring_line_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_spawn_flash_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_spawn_lightning_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_spawn_ray_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_spawn_globe_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_spawn_shake_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);
JSBool js_spawn_push_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval);

JSClass			spawn_class={"spawn_class",0,
							script_add_property,JS_PropertyStub,
							JS_PropertyStub,JS_PropertyStub,
							JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub};

JSFunctionSpec	spawn_functions[]={
							{"particle",			js_spawn_particle_func,				4},
							{"particleMoving",		js_spawn_particle_moving_func,		7},
							{"particleLine",		js_spawn_particle_line_func,		8},
							{"ring",				js_spawn_ring_func,					4},
							{"ringLine",			js_spawn_ring_line_func,			8},
							{"flash",				js_spawn_flash_func,				9},
							{"lightning",			js_spawn_lightning_func,			12},
							{"ray",					js_spawn_ray_func,					11},
							{"globe",				js_spawn_globe_func,				11},
							{"shake",				js_spawn_shake_func,				6},
							{"push",				js_spawn_push_func,					5},
							{0}};

/* =======================================================

      Create Object
      
======================================================= */

void script_add_global_spawn_object(JSObject *parent_obj)
{
    JSObject		*j_obj;
    
	j_obj=JS_DefineObject(js.cx,parent_obj,"spawn",&spawn_class,NULL,0);
	JS_DefineFunctions(js.cx,j_obj,spawn_functions);
}

/* =======================================================

      Spawn Particle
      
======================================================= */

JSBool js_spawn_particle_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				idx;
	char			name[name_str_len];
	d3pnt			pt;
	
	pt.x=JSVAL_TO_INT(argv[0]);
	pt.z=JSVAL_TO_INT(argv[1]);
	pt.y=JSVAL_TO_INT(argv[2]);
	
	script_value_to_string(argv[3],name,name_str_len);
	
	idx=particle_find_index(name);
	if (idx==-1) {
		JS_ReportError(js.cx,"Named particle does not exist: %s",name);
		return(JS_FALSE);
	}
	
	if (!particle_spawn(idx,&pt,NULL,NULL)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}
    
	return(JS_TRUE);
}

JSBool js_spawn_particle_moving_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				idx;
	char			name[name_str_len];
	d3pnt			pt;
	particle_motion	motion;
	
	pt.x=JSVAL_TO_INT(argv[0]);
	pt.z=JSVAL_TO_INT(argv[1]);
	pt.y=JSVAL_TO_INT(argv[2]);
	
	motion.bone_idx=-1;
	motion.vct.x=script_value_to_float(argv[3]);
	motion.vct.z=script_value_to_float(argv[4]);
	motion.vct.y=script_value_to_float(argv[5]);

	script_value_to_string(argv[6],name,name_str_len);
	
	idx=particle_find_index(name);
	if (idx==-1) {
		JS_ReportError(js.cx,"Named particle does not exist: %s",name);
		return(JS_FALSE);
	}
	
	if (!particle_spawn(idx,&pt,NULL,&motion)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}
    
	return(JS_TRUE);
}

JSBool js_spawn_particle_line_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				idx,count;
	char			name[name_str_len];
	d3pnt			start_pt,end_pt;
	
	start_pt.x=JSVAL_TO_INT(argv[0]);
	start_pt.z=JSVAL_TO_INT(argv[1]);
	start_pt.y=JSVAL_TO_INT(argv[2]);

	end_pt.x=JSVAL_TO_INT(argv[3]);
	end_pt.z=JSVAL_TO_INT(argv[4]);
	end_pt.y=JSVAL_TO_INT(argv[5]);

	count=JSVAL_TO_INT(argv[6]);

	script_value_to_string(argv[7],name,name_str_len);
	
	idx=particle_find_index(name);
	if (idx==-1) {
		JS_ReportError(js.cx,"Named particle does not exist: %s",name);
		return(JS_FALSE);
	}
	
	if (!particle_line_spawn(idx,&start_pt,&end_pt,count)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}
    
	return(JS_TRUE);
}

/* =======================================================

      Spawn Ring
      
======================================================= */

JSBool js_spawn_ring_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				idx;
	char			name[name_str_len];
	d3pnt			pt;
	
	pt.x=JSVAL_TO_INT(argv[0]);
	pt.z=JSVAL_TO_INT(argv[1]);
	pt.y=JSVAL_TO_INT(argv[2]);
	
	script_value_to_string(argv[3],name,name_str_len);
	
	idx=ring_find_index(name);
	if (idx==-1) {
		JS_ReportError(js.cx,"Named ring does not exist: %s",name);
		return(JS_FALSE);
	}
	
	if (!ring_spawn(idx,&pt,NULL)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}
    
	return(JS_TRUE);
}

JSBool js_spawn_ring_line_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				idx,count;
	char			name[name_str_len];
	d3pnt			start_pt,end_pt;
	
	start_pt.x=JSVAL_TO_INT(argv[0]);
	start_pt.z=JSVAL_TO_INT(argv[1]);
	start_pt.y=JSVAL_TO_INT(argv[2]);

	end_pt.x=JSVAL_TO_INT(argv[3]);
	end_pt.z=JSVAL_TO_INT(argv[4]);
	end_pt.y=JSVAL_TO_INT(argv[5]);

	count=JSVAL_TO_INT(argv[6]);

	script_value_to_string(argv[7],name,name_str_len);
	
	idx=ring_find_index(name);
	if (idx==-1) {
		JS_ReportError(js.cx,"Named ring does not exist: %s",name);
		return(JS_FALSE);
	}
	
	if (!ring_line_spawn(idx,&start_pt,&end_pt,count)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}
    
	return(JS_TRUE);
}

/* =======================================================

      Spawn Flash
      
======================================================= */

JSBool js_spawn_flash_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				intensity,flash_msec,fade_msec;
	d3pnt			pt;
	d3col			col;
	
	pt.x=JSVAL_TO_INT(argv[0]);
	pt.z=JSVAL_TO_INT(argv[1]);
	pt.y=JSVAL_TO_INT(argv[2]);

	col.r=script_value_to_float(argv[3]);
	col.g=script_value_to_float(argv[4]);
	col.b=script_value_to_float(argv[5]);

	intensity=JSVAL_TO_INT(argv[6]);
	flash_msec=JSVAL_TO_INT(argv[7]);
	fade_msec=JSVAL_TO_INT(argv[8]);

	if (!effect_spawn_flash(&pt,&col,intensity,flash_msec,fade_msec)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}
    
	return(JS_TRUE);
}

/* =======================================================

      Spawn Lightning
      
======================================================= */

JSBool js_spawn_lightning_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				wid,life_msec;
	float			varient;
	d3pnt			start_pt,end_pt;
	d3col			col;
	
	start_pt.x=JSVAL_TO_INT(argv[0]);
	start_pt.z=JSVAL_TO_INT(argv[1]);
	start_pt.y=JSVAL_TO_INT(argv[2]);

	end_pt.x=JSVAL_TO_INT(argv[3]);
	end_pt.z=JSVAL_TO_INT(argv[4]);
	end_pt.y=JSVAL_TO_INT(argv[5]);
	
	wid=JSVAL_TO_INT(argv[6]);
	varient=script_value_to_float(argv[7]);

	col.r=script_value_to_float(argv[8]);
	col.g=script_value_to_float(argv[9]);
	col.b=script_value_to_float(argv[10]);

	life_msec=JSVAL_TO_INT(argv[11]);

	if (!effect_spawn_lightning(js.time.current_tick,&start_pt,&end_pt,wid,varient,&col,life_msec)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}
    
	return(JS_TRUE);
}

/* =======================================================

      Spawn Ray
      
======================================================= */

JSBool js_spawn_ray_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				wid,life_msec;
	d3pnt			start_pt,end_pt;
	d3col			col;
	
	start_pt.x=JSVAL_TO_INT(argv[0]);
	start_pt.z=JSVAL_TO_INT(argv[1]);
	start_pt.y=JSVAL_TO_INT(argv[2]);

	end_pt.x=JSVAL_TO_INT(argv[3]);
	end_pt.z=JSVAL_TO_INT(argv[4]);
	end_pt.y=JSVAL_TO_INT(argv[5]);
	
	wid=JSVAL_TO_INT(argv[6]);

	col.r=script_value_to_float(argv[7]);
	col.g=script_value_to_float(argv[8]);
	col.b=script_value_to_float(argv[9]);

	life_msec=JSVAL_TO_INT(argv[10]);

	if (!effect_spawn_ray(&start_pt,&end_pt,wid,&col,life_msec)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}
    
	return(JS_TRUE);
}

/* =======================================================

      Spawn Globe
      
======================================================= */

JSBool js_spawn_globe_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				start_sz,end_sz,life_msec;
	float			start_alpha,end_alpha;
	d3pnt			pt;
	d3col			col;
	
	pt.x=JSVAL_TO_INT(argv[0]);
	pt.z=JSVAL_TO_INT(argv[1]);
	pt.y=JSVAL_TO_INT(argv[2]);

	start_sz=JSVAL_TO_INT(argv[3]);
	end_sz=JSVAL_TO_INT(argv[4]);

	start_alpha=script_value_to_float(argv[5]);
	end_alpha=script_value_to_float(argv[6]);

	col.r=script_value_to_float(argv[7]);
	col.g=script_value_to_float(argv[8]);
	col.b=script_value_to_float(argv[9]);

	life_msec=JSVAL_TO_INT(argv[10]);

	if (!effect_spawn_globe(&pt,start_sz,end_sz,start_alpha,end_alpha,&col,life_msec)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}
    
	return(JS_TRUE);
}

/* =======================================================

      Spawn Shake
      
======================================================= */

JSBool js_spawn_shake_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				dist,sz,life_msec;
	d3pnt			pt;
	
	pt.x=JSVAL_TO_INT(argv[0]);
	pt.z=JSVAL_TO_INT(argv[1]);
	pt.y=JSVAL_TO_INT(argv[2]);
    
    dist=JSVAL_TO_INT(argv[3]);
	
	sz=JSVAL_TO_INT(argv[4]);
	life_msec=JSVAL_TO_INT(argv[5]);
	
	if (!effect_spawn_shake(&pt,dist,sz,life_msec)) {
		*rval=JSVAL_FALSE;
	}
    else {
		*rval=JSVAL_TRUE;
	}
    
	return(JS_TRUE);
}

/* =======================================================

      Spawn Push
      
======================================================= */

JSBool js_spawn_push_func(JSContext *cx,JSObject *j_obj,uintN argc,jsval *argv,jsval *rval)
{
	int				x,z,y,radius,force;
	
	x=JSVAL_TO_INT(argv[0]);
	z=JSVAL_TO_INT(argv[1]);
	y=JSVAL_TO_INT(argv[2]);
	
	radius=JSVAL_TO_INT(argv[3]);
	force=JSVAL_TO_INT(argv[4]);

	collide_push_objects(x,z,y,radius,force);
	*rval=JSVAL_TRUE;
	
	return(JS_TRUE);
}

