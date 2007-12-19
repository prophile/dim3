/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Remote Functions Header

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

extern bool remote_add(int remote_uid,network_request_remote_add *add);
extern void remote_remove(int remote_uid);

extern void remote_get_tint(obj_type *obj,d3col *tint);
extern void remote_get_ui_color(obj_type *obj,d3col *col);

extern bool remote_timed_out(obj_type *obj);

extern void remote_predict_move(obj_type *obj);

extern bool remote_network_get_updates(int tick);
extern void remote_network_send_updates(int tick);
extern void remote_network_send_latency_ping(int tick);

extern void remote_draw_status(obj_type *obj);

extern void remote_draw_names_setup(void);
extern void remote_draw_names_render(void);
