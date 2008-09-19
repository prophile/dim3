/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Importing Routines

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

extern bool import_obj(char *path,bool *found_normals,char *err_str);
extern bool import_lightwave(char *path,char *err_str);
extern bool import_c4d_xml(char *path,char *err_str);
extern void insert_model(char *file_name);

extern int textdecode_count_linestarts(void);
extern void textdecode_linestarts(void);
extern bool textdecode_open(char *path,char p_piece_break);
extern void textdecode_close(void);
extern int textdecode_count(void);
extern int textdecode_find(int str_line,char *txt);
extern void textdecode_get_line(int i,char *txt,int len);
extern void textdecode_get_piece(int i,int k,char *txt);

extern void clear_materials(void);
extern int texture_count(void);
extern int texture_find_free(void);
extern int texture_pick(char *material_name,char *err_str);
extern bool texture_use_single(void);

extern bool nav_open_file(char *filetype,char *path);
