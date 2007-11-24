/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: HUD Routines

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

extern hud_type				hud;

/* =======================================================

      Initialize and Shutdown HUD Data
      
======================================================= */

void hud_initialize(void)
{
	hud.count.bitmap=0;
	hud.count.text=0;
	hud.count.bar=0;
	hud.count.menu=0;
	hud.count.chooser=0;

	hud.chat.nline=0;
	hud.chat.type_on=FALSE;
	hud.chat.type_str[0]=0x0;
}

/* =======================================================

      Finding
      
======================================================= */

hud_bitmap_type* hud_bitmaps_find(char *name)
{
	int					n;
	hud_bitmap_type		*bitmap;
	
	bitmap=hud.bitmaps;
	
	for (n=0;n!=hud.count.bitmap;n++) {
		if (strcmp(bitmap->name,name)==0) return(bitmap);
		bitmap++;
	}
	
	return(NULL);
}

hud_text_type* hud_texts_find(char *name)
{
	int					n;
	hud_text_type		*text;
	
	text=hud.texts;
	
	for (n=0;n!=hud.count.text;n++) {
		if (strcmp(text->name,name)==0) return(text);
		text++;
	}
	
	return(NULL);
}

hud_bar_type* hud_bars_find(char *name)
{
	int					n;
	hud_bar_type		*bar;
	
	bar=hud.bars;
	
	for (n=0;n!=hud.count.bar;n++) {
		if (strcmp(bar->name,name)==0) return(bar);
		bar++;
	}
	
	return(NULL);
}

/* =======================================================

      HUD Set Text
      
======================================================= */

void hud_text_set(hud_text_type *text,char *data)
{
	strcpy(text->data,data);
	text->has_return=(strstr(data,"{r}")!=NULL);
}

/* =======================================================

      HUD Utilities
      
======================================================= */

void hud_bitmaps_hide_all(void)
{
	int				n;
	hud_bitmap_type	*bitmap;

	bitmap=hud.bitmaps;
	
	for (n=0;n!=hud.count.bitmap;n++) {
		bitmap->show=FALSE;
		bitmap++;
	}
}

void hud_texts_hide_all(void)
{
	int				n;
	hud_text_type	*text;

	text=hud.texts;
	
	for (n=0;n!=hud.count.text;n++) {
		text->show=FALSE;
		text++;
	}
}

void hud_bars_hide_all(void)
{
	int				n;
	hud_bar_type	*bar;

	bar=hud.bars;
	
	for (n=0;n!=hud.count.bar;n++) {
		bar->show=FALSE;
		bar++;
	}
}

/* =======================================================

      Chat
      
======================================================= */

void chat_clear_messages(void)
{
	hud.chat.nline=0;
}

void chat_add_message(int tick,char *name,char *str,d3col *col)
{
	int				idx;
	chat_line_type	*line;
	
		// need to remove a line?
		
	if (hud.chat.nline>=max_chat_lines) {
		memmove(&hud.chat.lines[0],&hud.chat.lines[1],(sizeof(chat_line_type)*(max_chat_lines-1)));
		idx=hud.chat.nline-1;
	}
	else {
		idx=hud.chat.nline;
		hud.chat.nline++;
	}
	
		// set line
		
	line=&hud.chat.lines[idx];
	
	strcpy(line->name,name);
	strcpy(line->str,str);
	memmove(&line->col,col,sizeof(d3col));
	
		// last addition tick
		
	hud.chat.remove_tick=tick+(hud.chat.last_add_life_sec*1000);
}

void chat_time_out(int tick)
{
	if ((hud.chat.nline==0) || (hud.chat.remove_tick>tick)) return;
	
	memmove(&hud.chat.lines[0],&hud.chat.lines[1],(sizeof(chat_line_type)*(max_chat_lines-1)));
	hud.chat.nline--;
	
	hud.chat.remove_tick=tick+(hud.chat.next_life_sec*1000);
}
