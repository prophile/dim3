/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Menus

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

      Add Menus and Items
      
======================================================= */

int menu_add(char *name)
{
	menu_type		*menu;
	
	if (hud.count.menu>=max_menu) return(-1);
	
	menu=&hud.menus[hud.count.menu];
	hud.count.menu++;
	
	strcpy(menu->name,name);
	menu->nitem=0;
	
	return(hud.count.menu-1);
}

void menu_add_item(int menu_idx,int item_id,char *data,char *sub_menu,bool multiplayer_disable,bool quit)
{
	menu_type		*menu;
	menu_item_type  *item;
	
	menu=&hud.menus[menu_idx];
	
	if (menu->nitem>=max_menu_item) return;
	
	item=&menu->items[menu->nitem];
	menu->nitem++;
	
	item->item_id=item_id;
	strcpy(item->data,data);
	strcpy(item->sub_menu,sub_menu);
	item->multiplayer_disable=multiplayer_disable;
	item->quit=quit;
}

/* =======================================================

      Find Menu
      
======================================================= */

int menu_find(char *name)
{
	int			n;
	menu_type	*menu;
	
	menu=hud.menus;
	
	for (n=0;n!=hud.count.menu;n++) {
		if (strcmp(menu->name,name)==0) return(n);
		menu++;
	}
	
	return(-1);
}

