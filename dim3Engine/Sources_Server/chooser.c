/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Chooser

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

extern server_type			server;
extern hud_type				hud;
extern setup_type			setup;

/* =======================================================

      Add Chooser and Items
      
======================================================= */

int chooser_add(char *name)
{
	chooser_type		*chooser;
	
	if (hud.count.chooser>=max_chooser) return(-1);
	
	chooser=&hud.choosers[hud.count.chooser];
	hud.count.chooser++;
	
	strcpy(chooser->name,name);
	chooser->nitem=0;
	
	return(hud.count.chooser-1);
}

void chooser_add_text(int chooser_idx,int text_id,char *data,int x,int y,bool large,int just,bool clickable)
{
	chooser_type		*chooser;
	chooser_text_type	*text;
	
	chooser=&hud.choosers[chooser_idx];
	
	if (chooser->ntext>=max_chooser_text) return;
	
	text=&chooser->texts[chooser->ntext];
	chooser->ntext++;
	
	text->text_id=text_id;
	strcpy(text->data,data);
	text->x=x;
	text->y=y;
	text->large=large;
	text->just=just;
	text->clickable=clickable;
}


void chooser_add_item(int chooser_idx,int item_id,char *file,int x,int y,int wid,int high,bool clickable)
{
	chooser_type		*chooser;
	chooser_item_type	*item;
	
	chooser=&hud.choosers[chooser_idx];
	
	if (chooser->nitem>=max_chooser_item) return;
	
	item=&chooser->items[chooser->nitem];
	chooser->nitem++;
	
	item->item_id=item_id;
	strcpy(item->file,file);
	item->x=x;
	item->y=y;
	item->wid=wid;
	item->high=high;
	item->clickable=clickable;
}

/* =======================================================

      Find Chooser
      
======================================================= */

int chooser_find(char *name)
{
	int				n;
	chooser_type	*chooser;
	
	chooser=hud.choosers;
	
	for (n=0;n!=hud.count.chooser;n++) {
		if (strcasecmp(chooser->name,name)==0) return(n);
		chooser++;
	}
	
	return(-1);
}

