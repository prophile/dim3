/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Console Functions Header

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

extern void console_initialize(void);

extern void console_remove_line(void);
extern void console_add_line(char *txt,d3col *col);
extern void console_add(char *txt);
extern void console_add_system(char *txt);
extern void console_add_error(char *txt);

extern void console_show(void);
extern void console_hide(void);
extern void console_next_mode(void);

extern int console_y_offset(void);

extern void console_input(void);
extern void console_draw(void);
