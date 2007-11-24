/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Object Grows

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

#include "objects.h"

extern server_type				server;
extern map_type					map;

extern int game_time_get(void);

/* =======================================================

      Start/Clear Object Grows
      
======================================================= */

void object_grow_clear(obj_type *obj)
{
	obj->grow.on=FALSE;
}

void object_grow_start(obj_type *obj,int grow_msec,float end_resize,d3pnt *end_size,d3pnt *end_offset)
{
	float			f_msec;
	obj_grow		*grow;
	
	grow=&obj->grow;

	grow->count=grow_msec/10;
	f_msec=(float)grow->count;

	grow->resize_end=end_resize;
	grow->resize_add=(end_resize-obj->draw.resize)/f_msec;

	memmove(&grow->size_end,end_size,sizeof(d3pnt));
	
	grow->size_add.x=(float)(end_size->x-obj->size.x)/f_msec;
	grow->size_accum.x=0.0f;

	grow->size_add.y=(float)(end_size->y-obj->size.y)/f_msec;
	grow->size_accum.y=0.0f;

	grow->size_add.z=(float)(end_size->z-obj->size.z)/f_msec;
	grow->size_accum.z=0.0f;
	
	memmove(&grow->offset_end,end_offset,sizeof(d3pnt));

	grow->offset_add.x=(float)(end_offset->x-obj->draw.offset.x)/f_msec;
	grow->offset_accum.x=0.0f;

	grow->offset_add.y=(float)(end_offset->y-obj->draw.offset.y)/f_msec;
	grow->offset_accum.y=0.0f;

	grow->offset_add.z=(float)(end_offset->z-obj->draw.offset.z)/f_msec;
	grow->offset_accum.z=0.0f;

	grow->on=TRUE;
}

/* =======================================================

      Run Object Grows
      
======================================================= */

void object_grow_run(obj_type *obj)
{
	int					n,xmove,ymove,zmove;
	float				ypush;
	obj_grow			*grow;
	obj_type			*test_obj;

	grow=&obj->grow;
	if (!grow->on) return;

		// finished?
		// make sure final numbers are hit

	grow->count--;
	if (grow->count==0) {
	
		grow->on=FALSE;
		
		obj->draw.resize=grow->resize_end;
		
		obj->size.x=grow->size_end.x;
		obj->size.y=grow->size_end.y;
		obj->size.z=grow->size_end.z;
		
		obj->draw.offset.x=grow->offset_end.x;
		obj->draw.offset.y=grow->offset_end.y;
		obj->draw.offset.z=grow->offset_end.z;

		return;
	}

		// calculate model resize

	obj->draw.resize+=grow->resize_add;

		// calculate collision size
		
	grow->size_accum.x+=grow->size_add.x;
	xmove=(int)grow->size_accum.x;
	grow->size_accum.x-=(float)xmove;

	obj->size.x+=xmove;

	grow->size_accum.y+=grow->size_add.y;
	ymove=(int)grow->size_accum.y;
	grow->size_accum.y-=(float)ymove;

	obj->size.y+=ymove;

	grow->size_accum.z+=grow->size_add.z;
	zmove=(int)grow->size_accum.z;
	grow->size_accum.z-=(float)zmove;

	obj->size.z+=zmove;

		// calculate model offset
		
	grow->offset_accum.x+=grow->offset_add.x;
	xmove=(int)grow->offset_accum.x;
	grow->offset_accum.x-=(float)xmove;

	obj->draw.offset.x+=xmove;

	grow->offset_accum.y+=grow->offset_add.y;
	ymove=(int)grow->offset_accum.y;
	grow->offset_accum.y-=(float)ymove;

	obj->draw.offset.y+=ymove;

	grow->offset_accum.z+=grow->offset_add.z;
	zmove=(int)grow->offset_accum.z;
	grow->offset_accum.z-=(float)zmove;

	obj->draw.offset.z+=zmove;

		// move any objects standing on this one
		// if object is growing upwards

	if (ymove>0) {
	
		ypush=-(((float)ymove)/10.0f);

		test_obj=server.objs;

		for (n=0;n!=server.count.obj;n++) {
			if (test_obj->stand_obj_uid==obj->uid) {
				object_move_y(test_obj,-ymove);
				if (test_obj->force.vct.y>ypush) test_obj->force.vct.y+=ypush;
			}
			test_obj++;
		}
	}
}
