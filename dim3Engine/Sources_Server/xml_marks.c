/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Marks XML

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

      Read Marks XML
      
======================================================= */

void read_settings_mark(void)
{
	int					nmark,marks_head_tag,mark_tag,tag;
	char				path[1024];
	mark_type			*mark;

		// no marks yet

	server.marks=NULL;
	server.count.mark=0;
	
		// read in interface from setting files
		
	file_paths_data(&setup.file_path_setup,path,"Settings","Marks","xml");
	if (!xml_open_file(path)) return;
	
		// get counts
		
    marks_head_tag=xml_findrootchild("Marks");
    if (marks_head_tag==-1) {
		xml_close_file();
		return;
	}

	nmark=xml_countchildren(marks_head_tag);

	if (nmark==0) {
		xml_close_file();
		return;
	}

	server.marks=(mark_type*)malloc(sizeof(mark_type)*nmark);
	if (server.marks==NULL) {
		xml_close_file();
		return;
	}

		// read the marks
	
	mark_tag=xml_findfirstchild("Mark",marks_head_tag);
	
	while (mark_tag!=-1) {
	
			// start a new mark
			
		mark=&server.marks[server.count.mark];
		
		xml_get_attribute_text(mark_tag,"name",mark->name,name_str_len);
		
		tag=xml_findfirstchild("Setting",mark_tag);
		if (tag!=-1) {
			mark->life_msec=xml_get_attribute_int(tag,"time");
			mark->fade_in_msec=xml_get_attribute_int(tag,"fade_in");
			mark->fade_out_msec=xml_get_attribute_int(tag,"fade_out");
			mark->no_rotate=xml_get_attribute_boolean(tag,"no_rotate");
			mark->no_transparent=xml_get_attribute_boolean(tag,"no_transparent");
			mark->no_opaque=xml_get_attribute_boolean(tag,"no_opaque");
            mark->hilite=xml_get_attribute_boolean(tag,"highlight");
            mark->blend_add=xml_get_attribute_boolean(tag,"additive");
		}
		else {
			mark->life_msec=1000;
			mark->fade_in_msec=mark->fade_out_msec=500;
			mark->no_rotate=FALSE;
			mark->no_transparent=FALSE;
			mark->no_opaque=FALSE;
			mark->hilite=FALSE;
			mark->blend_add=FALSE;
		}
		
		mark->total_msec=mark->life_msec+mark->fade_in_msec+mark->fade_out_msec;
		
		mark->animate.image_count=1;
		mark->animate.image_per_row=1;
		mark->animate.msec=200;
		mark->animate.loop=TRUE;
		mark->animate.loop_back=FALSE;
	
		tag=xml_findfirstchild("Image",mark_tag);
		if (tag!=-1) {
			xml_get_attribute_text(tag,"file",mark->bitmap_name,file_str_len);
			mark->animate.image_count=xml_get_attribute_int(tag,"count");
			mark->animate.image_per_row=(int)sqrt((float)mark->animate.image_count);
			mark->animate.msec=xml_get_attribute_int(tag,"time");
			mark->animate.loop=xml_get_attribute_boolean(tag,"loop");
			mark->animate.loop_back=xml_get_attribute_boolean(tag,"loop_back");
		}
		
			// move on to next mark
			
		server.count.mark++;
		
		mark_tag=xml_findnextchild(mark_tag);
	}
	
	xml_close_file();
}
