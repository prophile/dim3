/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Ring XML

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

#include "xmls.h"

extern server_type			server;
extern setup_type			setup;

/* =======================================================

      Read Ring XML
      
======================================================= */

void read_settings_ring(void)
{
	int					nring,ring_head_tag,ring_tag,tag;
	char				path[1024];
	ring_type			*ring;

		// no rings yet

	server.rings=NULL;
	server.count.ring=0;

		// read in rings from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Rings","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
       
    ring_head_tag=xml_findrootchild("Rings");
    if (ring_head_tag==-1) {
		xml_close_file();
		return;
	}

	nring=xml_countchildren(ring_head_tag);

	if (nring==0) {
		xml_close_file();
		return;
	}

	server.rings=(ring_type*)malloc(sizeof(ring_type)*nring);
	if (server.rings==NULL) {
		xml_close_file();
		return;
	}

		// read in the rings

	ring_tag=xml_findfirstchild("Ring",ring_head_tag);
    
	while (ring_tag!=-1) {
	
			// create a new ring
			
		ring=&server.rings[server.count.ring];
		
		xml_get_attribute_text(ring_tag,"name",ring->name,name_str_len);
		
		ring->life_msec=1000;
		ring->blend_add=FALSE;

        tag=xml_findfirstchild("Settings",ring_tag);
        if (tag!=-1) {
            ring->life_msec=xml_get_attribute_int(tag,"time");
            ring->blend_add=xml_get_attribute_boolean(tag,"additive");
        }
		
		ring->animate.image_count=1;
		ring->animate.image_per_row=1;
		ring->animate.msec=200;
		ring->animate.loop=TRUE;
		ring->animate.loop_back=FALSE;
		
        tag=xml_findfirstchild("Image",ring_tag);
        if (tag!=-1) {
            xml_get_attribute_text(tag,"file",ring->bitmap_name,file_str_len);
            ring->animate.image_count=xml_get_attribute_int(tag,"count");
			ring->animate.image_per_row=(int)sqrt((float)ring->animate.image_count);
			ring->animate.msec=xml_get_attribute_int(tag,"time");
            ring->animate.loop=xml_get_attribute_boolean(tag,"loop");
            ring->animate.loop_back=xml_get_attribute_boolean(tag,"loop_back");
		}
		
        tag=xml_findfirstchild("Outer",ring_tag);
        if (tag!=-1) {
            ring->start_outer_size=xml_get_attribute_int(tag,"start");
            ring->end_outer_size=xml_get_attribute_int(tag,"end");
        }

        tag=xml_findfirstchild("Inner",ring_tag);
        if (tag!=-1) {
            ring->start_inner_size=xml_get_attribute_int(tag,"start");
            ring->end_inner_size=xml_get_attribute_int(tag,"end");
        }

        tag=xml_findfirstchild("Rotate",ring_tag);
        if (tag!=-1) {
            ring->ang.x=xml_get_attribute_float(tag,"x");
            ring->ang.y=xml_get_attribute_float(tag,"y");
            ring->ang.z=xml_get_attribute_float(tag,"z");
            ring->rot.x=xml_get_attribute_float_default(tag,"x_add",0.0f);
            ring->rot.y=xml_get_attribute_float_default(tag,"y_add",0.0f);
            ring->rot.z=xml_get_attribute_float_default(tag,"z_add",0.0f);
            ring->rot_accel.x=xml_get_attribute_float_default(tag,"x_accel",0.0f);
            ring->rot_accel.y=xml_get_attribute_float_default(tag,"y_accel",0.0f);
            ring->rot_accel.z=xml_get_attribute_float_default(tag,"z_accel",0.0f);
		}

        tag=xml_findfirstchild("Move",ring_tag);
        if (tag!=-1) {
            ring->vct.x=xml_get_attribute_float(tag,"x");
            ring->vct.y=xml_get_attribute_float(tag,"y");
            ring->vct.z=xml_get_attribute_float(tag,"z");
        }
		
		ring->start_color.r=ring->start_color.g=ring->start_color.b=1.0f;
		ring->end_color.r=ring->end_color.g=ring->end_color.b=1.0f;
		ring->team_tint=FALSE;

        tag=xml_findfirstchild("Color",ring_tag);
        if (tag!=-1) {
			xml_get_attribute_color(tag,"start",&ring->start_color);
			xml_get_attribute_color(tag,"end",&ring->end_color);
			ring->team_tint=xml_get_attribute_boolean(tag,"team");
       }
		
        tag=xml_findfirstchild("Alpha",ring_tag);
        if (tag!=-1) {
            ring->start_alpha=xml_get_attribute_float(tag,"start");
            ring->end_alpha=xml_get_attribute_float(tag,"end");
        }
	
			// move on to next ring
			
		server.count.ring++;
		
		ring_tag=xml_findnextchild(ring_tag);
	}
	
	xml_close_file();
}

