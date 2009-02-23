/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: XML File Functions Header

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

extern void setup_xml_default(void);
extern int setup_find_control_for_name(char *name);
extern int setup_find_action_in_setup(int action_idx);
extern void setup_to_input(void);
extern void setup_xml_read_key_int(int setup_tag,char *name,int *value);
extern void setup_xml_read_key_float(int setup_tag,char *name,float *value);
extern void setup_xml_read_key_boolean(int setup_tag,char *name,bool *value);
extern bool setup_xml_read(void);
extern void setup_xml_write_key_int(char *name,int value);
extern void setup_xml_write_key_float(char *name,float value);
extern void setup_xml_write_key_boolean(char *name,bool value);
extern bool setup_xml_write(void);
extern void setup_restore(void);

extern void default_settings_interface(void);
extern void read_settings_interface_bitmap(int bitmap_tag);
extern void read_settings_interface_text(int text_tag);
extern void read_settings_interface_menu(int menu_tag);
extern void read_settings_interface_chooser(int chooser_tag);
extern void read_settings_interface(void);

extern void read_settings_particle(void);
extern void read_settings_ring(void);
extern void read_settings_halo(void);
extern void read_settings_mark(void);
extern void read_settings_crosshair(void);
extern void read_settings_sound(void);
extern void read_settings_action(void);
extern void read_settings_shader(void);


