/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Window Functions

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

extern void model_wind_open(void);
extern void model_wind_close(void);
extern void model_wind_offset_click(Point *pt);
extern void model_wind_set_title(char *title);
extern void model_wind_reset_tools(void);
extern void model_wind_switch_mesh_mode(void);
extern void model_wind_reset_modifiers(void);

extern void tabs_start(void);
extern void tabs_end(void);
extern void tabs_resize(void);

extern void click_model_wind(Point pt,unsigned long modifiers);

extern void texture_palette_draw(void);
extern void texture_palette_click(Point pt,bool dblclick);

extern void mesh_palette_draw(void);
extern void mesh_palette_click(Point pt,bool dblclick);

extern void info_palette_draw(void);

extern void model_wind_play(bool play,bool blend);


