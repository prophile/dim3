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

extern void click_model_wind(Point pt,unsigned long modifiers);

extern void texture_palette_draw(void);
extern void texture_palette_click(Point pt,bool dblclick);

extern void info_palette_draw(void);

extern void add_db_column(ControlRef ctrl,char *name,int idx,int type,int sz,int spot);

extern void start_pose_controls(WindowRef wind,Rect *box);
extern void end_pose_controls(void);
extern void resize_pose_controls(Rect *box);

extern void start_bone_controls(WindowRef wind,Rect *box);
extern void end_bone_controls(void);
extern void resize_bone_controls(Rect *box);

extern void start_animate_controls(WindowRef wind,Rect *bpx);
extern void end_animate_controls(void);
extern void resize_animate_controls(Rect *box);

extern void start_mesh_controls(WindowRef wind,Rect *bpx);
extern void end_mesh_controls(void);
extern void resize_mesh_controls(Rect *box);

extern void start_vertex_controls(WindowRef wind,Rect *box);
extern void end_vertex_controls(void);
extern void resize_vertex_controls(Rect *box);

extern void reset_pose_list(void);
extern void reset_bone_list(void);
extern void reset_animate_list(void);
extern void reset_mesh_list(void);
extern void reset_vertex_tab(void);

extern void hilite_vertex_rows(void);

extern void model_wind_play(bool play,bool blend);


