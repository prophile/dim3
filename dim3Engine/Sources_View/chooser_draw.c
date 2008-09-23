/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Chooser

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
#include "interfaces.h"
#include "inputs.h"

extern server_type			server;
extern js_type				js;
extern hud_type				hud;
extern setup_type			setup;

int							chooser_idx;
char						chooser_sub_txt[max_chooser_sub_txt][max_chooser_text_data_sz];
bool						chooser_start_trigger;

extern int chooser_find(char *name);

/* =======================================================

      Chooser Operations
      
======================================================= */

void chooser_open(void)
{
	int					n,idx;
	char				path[1024],path2[1024],fname[256],
						a[32],*c,str[max_chooser_text_data_sz];
	chooser_type		*chooser;
	chooser_text_type	*text;
	chooser_item_type	*item;
	
	gui_initialize(NULL,NULL,TRUE);
	
		// setup elements
		
	chooser=&hud.choosers[chooser_idx];
	gui_set_frame(&chooser->frame);
		
		// text

	text=chooser->texts;

	for (n=0;n<chooser->ntext;n++) {

			// check for substitutions

		strcpy(str,text->data);
		if ((str[0]=='{') && ((str[1]>='0') && (str[1]<='9'))) {
			strncpy(a,(char*)&str[1],32);
			a[32]=0x0;
			c=strchr(a,'}');
			if (c!=NULL) {
				*c=0x0;
				idx=atoi(a);
				if ((idx>=0) && (idx<max_chooser_sub_txt)) {
					strcpy(str,chooser_sub_txt[idx]);
				}
			}
		}

			// add text element

		element_text_add(str,text->text_id,text->x,text->y,text->just,(!text->large),text->clickable,FALSE);
		text++;
	}

		// items

	item=chooser->items;

	for (n=0;n<chooser->nitem;n++) {
	
		file_paths_data(&setup.file_path_setup,path,"Chooser",item->file,"png");
		
		if (item->clickable) {
			sprintf(fname,"%s_selected",item->file);
			file_paths_data(&setup.file_path_setup,path2,"Chooser",fname,"png");
			element_button_add(path,path2,item->item_id,item->x,item->y,item->wid,item->high,element_pos_left,element_pos_top);
		}
		else {
			element_bitmap_add(path,0,item->x,item->y,-1,-1,FALSE);
		}
		
		item++;
	}
	
		// clear raw key
	
	server.state=gs_chooser;
}

void chooser_close(void)
{
	gui_shutdown();
	
	if (server.state==gs_chooser) server.state=gs_running;			// only reset to running if picked chooser item didn't reset to something else
}

/* =======================================================

      Chooser Triggers
      
======================================================= */

void chooser_trigger_clear(void)
{
	chooser_start_trigger=FALSE;
}

void chooser_trigger_check(void)
{
	if (chooser_start_trigger) chooser_open();
}	

void chooser_trigger_set(char *name,char *sub_txt)
{
	int				n;

		// find chooser

	chooser_idx=chooser_find(name);
	if (chooser_idx==-1) return;

		// setup text substitutions

	for (n=0;n!=max_chooser_sub_txt;n++) {
		strcpy(chooser_sub_txt[n],(char*)&sub_txt[max_chooser_text_data_sz*n]);
	}

		// trigger chooser open
	
	chooser_start_trigger=TRUE;
}

/* =======================================================

      Menu Input
      
======================================================= */

void chooser_click(void)
{
	int					id,ch;
	
	id=-1;
	
		// check for ok/cancel keys
		
	ch=input_gui_get_keyboard_key(FALSE);
	if (ch!=0x0) {
		if (ch==0x1B) id=hud.choosers[chooser_idx].key.cancel_id;
		if (ch==0xD) id=hud.choosers[chooser_idx].key.ok_id;
	}
	
	if (id!=-1) {			// wait for key up
		while (input_gui_get_keyboard_key(FALSE)==ch) {}
	}

		// if no key check clicking
		
	if (id==-1) {
		id=gui_click();
		if (id==-1) return;
	}
	
		// run click
		
	hud_click();
	
	chooser_close();
	scripts_post_event_console(&js.game_attach,sd_event_chooser,0,id);
}

/* =======================================================

      Run Chooser
      
======================================================= */

void chooser_run(void)
{
	gui_draw(1.0f,TRUE);
	chooser_click();
}
