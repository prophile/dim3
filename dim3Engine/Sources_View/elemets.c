/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: GUI Control Elements

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

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "video.h"
#include "inputs.h"

extern hud_type				hud;
extern setup_type			setup;

int							nelement,element_click_down_id,
							element_open_text_field_id,element_open_combo_id;
bitmap_type					element_textbox,element_textbox_selected,
							element_combo_close,element_combo_close_selected,
							element_combo_open_top,element_combo_open_top_selected,
							element_combo_open_middle,element_combo_open_middle_selected,
							element_combo_open_bottom,element_combo_open_bottom_selected,
							element_checkbox_true,element_checkbox_false,
							element_checkbox_true_selected,element_checkbox_false_selected,
							element_slider,element_slider_selected,element_slider_fill,
							element_scroll_up,element_scroll_down,element_busy,element_tab_border;
element_type				elements[max_element];

pthread_mutex_t				element_thread_lock;

/* =======================================================

      Initialize/Shutdown Elements
      
======================================================= */

void element_reset_controls(void)
{
	nelement=0;

	element_click_down_id=-1;
	element_open_text_field_id=-1;
	element_open_combo_id=-1;
}

void element_open_global_bitmaps(void)
{
	char			path[1024];

		// text boxes

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","textbox","png");
	bitmap_open(&element_textbox,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","textbox_selected","png");
	bitmap_open(&element_textbox_selected,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

		// combos

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","combo_close","png");
	bitmap_open(&element_combo_close,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","combo_close_selected","png");
	bitmap_open(&element_combo_close_selected,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","combo_open_top","png");
	bitmap_open(&element_combo_open_top,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","combo_open_top_selected","png");
	bitmap_open(&element_combo_open_top_selected,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","combo_open_middle","png");
	bitmap_open(&element_combo_open_middle,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","combo_open_middle_selected","png");
	bitmap_open(&element_combo_open_middle_selected,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","combo_open_bottom","png");
	bitmap_open(&element_combo_open_bottom,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","combo_open_bottom_selected","png");
	bitmap_open(&element_combo_open_bottom_selected,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

		// checkboxes

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","checkbox_true","png");
	bitmap_open(&element_checkbox_true,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","checkbox_true_selected","png");
	bitmap_open(&element_checkbox_true_selected,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","checkbox_false","png");
	bitmap_open(&element_checkbox_false,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","checkbox_false_selected","png");
	bitmap_open(&element_checkbox_false_selected,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

		// sliders

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","slider","png");
	bitmap_open(&element_slider,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","slider_selected","png");
	bitmap_open(&element_slider_selected,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","slider_fill","png");
	bitmap_open(&element_slider_fill,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

		// tables
		
	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","scroll_up","png");
	bitmap_open(&element_scroll_up,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","scroll_down","png");
	bitmap_open(&element_scroll_down,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","busy","png");
	bitmap_open(&element_busy,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);
		
		// tabs

	file_paths_data(&setup.file_path_setup,path,"Bitmaps/UI_Elements","tab_border","png");
	bitmap_open(&element_tab_border,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);
}

void element_close_global_bitmaps(void)
{
	bitmap_close(&element_textbox);
	bitmap_close(&element_textbox_selected);

	bitmap_close(&element_combo_close);
	bitmap_close(&element_combo_close_selected);
	bitmap_close(&element_combo_open_top);
	bitmap_close(&element_combo_open_top_selected);
	bitmap_close(&element_combo_open_middle);
	bitmap_close(&element_combo_open_middle_selected);
	bitmap_close(&element_combo_open_bottom);
	bitmap_close(&element_combo_open_bottom_selected);

	bitmap_close(&element_checkbox_true);
	bitmap_close(&element_checkbox_true_selected);

	bitmap_close(&element_checkbox_false);
	bitmap_close(&element_checkbox_false_selected);

	bitmap_close(&element_slider);
	bitmap_close(&element_slider_selected);
	bitmap_close(&element_slider_fill);

	bitmap_close(&element_scroll_up);
	bitmap_close(&element_scroll_down);
	bitmap_close(&element_busy);
	
	bitmap_close(&element_tab_border);
}

void element_release_control_memory(void)
{
	int				n,k;
	element_type	*element;
	
	element=elements;
	
	for (n=0;n<nelement;n++) {
		
		switch (element->type) {
		
			case element_type_button:
				bitmap_close(&element->setup.button.bitmap);
				bitmap_close(&element->setup.button.bitmap_select);
				break;
				
			case element_type_bitmap:
				bitmap_close(&element->setup.button.bitmap);
				break;
				
			case element_type_table:
				if (element->data!=NULL) free(element->data);
				if (element->setup.table.bitmap!=NULL) {
					for (k=0;k!=element->setup.table.nbitmap;k++) {
						bitmap_close(&element->setup.table.bitmap[k]);
					}
					free(element->setup.table.bitmap);
				}
				break;
		
			case element_type_tab:
				for (k=0;k!=element->setup.tab.ntab;k++) {
					bitmap_close(&element->setup.tab.bitmap[k]);
					bitmap_close(&element->setup.tab.bitmap_select[k]);
				}
				break;
				
		}
		
		element++;
	}
}

void element_initialize(void)
{
	element_reset_controls();
	element_open_global_bitmaps();

	pthread_mutex_init(&element_thread_lock,NULL);		// used to make elements thread safe for certain UIs
}

void element_shutdown(void)
{
	element_release_control_memory();
	element_close_global_bitmaps();

	pthread_mutex_destroy(&element_thread_lock);
}

/* =======================================================

      Clear Elements
      
======================================================= */

void element_clear(void)
{
	pthread_mutex_lock(&element_thread_lock);

	element_release_control_memory();
	element_reset_controls();

	pthread_mutex_unlock(&element_thread_lock);
}

/* =======================================================

      Element Info
      
======================================================= */

inline int element_get_control_high(void)
{
	return((int)(((float)gl_text_get_char_height(TRUE))*element_control_high_factor));
}

inline int element_get_separator_high(void)
{
	return((int)(((float)gl_text_get_char_height(TRUE))*element_control_separator_factor));
}

inline int element_get_padding(void)
{
	return((int)(((float)hud.scale_x)*element_control_padding_factor));
}

/* =======================================================

      Add Elements
      
======================================================= */

void element_button_add(char *path,char *path2,int id,int x,int y,int wid,int high,int x_pos,int y_pos)
{
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);
	
	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_button;
	
	element->x=x;
	element->y=y;
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
		// skip button if graphic is missing to avoid crash

	if (!bitmap_open(&element->setup.button.bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE)) {
		nelement--;
		pthread_mutex_unlock(&element_thread_lock);
		return;
	}

	bitmap_open(&element->setup.button.bitmap_select,path2,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	if ((wid!=-1) && (high!=-1)) {
		element->wid=wid;
		element->high=high;
	}
	else {
		element->wid=element->setup.button.bitmap.wid;
		element->high=element->setup.button.bitmap.high;
	}
	
	switch (x_pos) {
		case element_pos_right:
			element->x-=element->wid;
			break;
		case element_pos_center:
			element->x-=(element->wid>>1);
			break;
	}

	switch (y_pos) {
		case element_pos_bottom:
			element->y-=element->high;
			break;
		case element_pos_center:
			element->y-=(element->high>>1);
			break;
	}

	pthread_mutex_unlock(&element_thread_lock);
}

void element_bitmap_add(char *path,int id,int x,int y,int wid,int high,bool framed)
{
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);
	
	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_bitmap;
	
	if (path==NULL) {
		element->setup.button.bitmap.gl_id=-1;
	}
	else {
		bitmap_open(&element->setup.button.bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);
	}
	
	element->x=x;
	element->y=y;
	
	if ((wid==-1) || (high==-1)) {
		if (element->setup.button.bitmap.gl_id!=-1) {
			element->wid=element->setup.button.bitmap.wid;
			element->high=element->setup.button.bitmap.high;
		}
		else {
			element->wid=element->high=0;
		}
	}
	else {
		element->wid=wid;
		element->high=high;
	}
	
	element->selectable=FALSE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	element->framed=framed;

	pthread_mutex_unlock(&element_thread_lock);
}

void element_text_add(char *str,int id,int x,int y,int just,bool small_text,bool selectable,bool alert)
{
	int				wid,high;
	char			*c,*c2;
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);
	
	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_text;
	
	element->x=x;
	element->y=y;
	
	element->setup.text.just=just;
	element->setup.text.small_text=small_text;
	element->setup.text.alert=alert;

	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	strcpy(element->str,str);

		// need to calculate size through returns

	high=gl_text_get_char_height(small_text);

	element->wid=0;
	element->high=high;

	c=element->str;
	
	while (*c!=0x0) {
		c2=strstr(c,"{r}");
		if (c2!=NULL) *c2=0x0;
		
		wid=gl_text_get_string_width(c,small_text);
		if (wid>element->wid) element->wid=wid;
		
		if (c2==NULL) break;

		element->high+=high;
		
		*c2='{';
		
		c=c2+3;
	}

	pthread_mutex_unlock(&element_thread_lock);
}

void element_text_field_add(char *str,char *value_str,int max_value_str_sz,int id,int x,int y,bool selectable)
{
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_text_field;
	
	element->x=x;
	element->y=y;

	element->wid=(int)(((float)hud.scale_x)*element_control_draw_long_width);
	element->high=(int)(((float)hud.scale_x)*element_control_draw_height);
	
	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	strcpy(element->str,str);
	strncpy(element->value_str,value_str,max_value_str_sz);
	element->value_str[max_value_str_sz-1]=0x0;

	element->max_value_str_sz=max_value_str_sz;

	pthread_mutex_unlock(&element_thread_lock);
}

void element_checkbox_add(char *str,int value,int id,int x,int y,bool selectable)
{
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_checkbox;
	
	element->x=x;
	element->y=y;
	
	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->wid=(int)(((float)hud.scale_x)*element_control_draw_height);
	element->high=(int)(((float)hud.scale_x)*element_control_draw_height);
	
	strcpy(element->str,str);
	
	element->value=value;

	pthread_mutex_unlock(&element_thread_lock);
}

void element_combo_add(char *str,char *combo_data,int value,int id,int x,int y,bool selectable)
{
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_combo;
	
	element->x=x;
	element->y=y;
	
	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->wid=(int)(((float)hud.scale_x)*element_control_draw_long_width);
	element->high=(int)(((float)hud.scale_x)*element_control_draw_height);
	
	strcpy(element->str,str);
	
	element->value=value;
	element->data=combo_data;

	pthread_mutex_unlock(&element_thread_lock);
}

void element_slider_add(char *str,float value,float value_min,float value_max,int id,int x,int y,bool selectable)
{
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_slider;
	
	element->x=x;
	element->y=y;
	
	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;

	element->wid=(int)(((float)hud.scale_x)*element_control_draw_long_width);
	element->high=(int)(((float)hud.scale_x)*element_control_draw_height);

	strcpy(element->str,str);
	
	element->setup.slider.min=value_min;
	element->setup.slider.max=value_max;
	element->setup.slider.value=(value-value_min)/(value_max-value_min);

	pthread_mutex_unlock(&element_thread_lock);
}

int element_table_add_get_data_size(char *row_data)
{
	int			count;
	char		*c;
	
	count=0;
	c=row_data;
	
	while (TRUE) {
		if (*c==0x0) break;
		c+=128;
		count++;
	}

	return((count+1)*128);
}

void element_table_add(element_column_type* cols,char *row_data,int id,int ncolumn,int x,int y,int wid,int high,int bitmap_mode)
{
	int				sz;
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_table;
	
	element->setup.table.ncolumn=ncolumn;
	element->setup.table.busy=FALSE;
	
	element->x=x;
	element->y=y;
	element->wid=wid;
	element->high=high;
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	memmove(element->setup.table.cols,cols,(sizeof(element_column_type)*ncolumn));
	
	element->value=-1;
	element->offset=0;
	
	if (row_data==NULL) {
		element->data=NULL;
	}
	else {
		sz=element_table_add_get_data_size(row_data);
		element->data=valloc(sz);
		memmove(element->data,row_data,sz);
	}

	element->setup.table.bitmap_mode=bitmap_mode;
	element->setup.table.nbitmap=0;
	element->setup.table.bitmap=NULL;
	
	pthread_mutex_unlock(&element_thread_lock);
}

void element_tab_add(char *path_list,char *path2_list,int value,int id,int ntab,int x,int y,int wid,int high,int list_wid,int ext_high)
{
	int				n;
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);

	element=&elements[nelement];
	nelement++;
	
	element->id=id;
	element->type=element_type_tab;
	
	element->x=x;
	element->y=y;
	element->wid=wid;
	element->high=high;
	element->setup.tab.list_wid=list_wid;
	element->setup.tab.ext_high=ext_high;
	
	element->selectable=TRUE;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	element->value=value;
	element->offset=0;

	element->setup.tab.ntab=ntab;
	
	for (n=0;n!=ntab;n++) {
		bitmap_open(&element->setup.tab.bitmap[n],(char*)&path_list[n*1024],anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);
		bitmap_open(&element->setup.tab.bitmap_select[n],(char*)&path2_list[n*1024],anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);
	}

	pthread_mutex_unlock(&element_thread_lock);
}

/* =======================================================

      Change Elements
      
======================================================= */

element_type* element_find(int id)
{
	int					n;
	element_type		*element;
		
	element=elements;
	
	for (n=0;n<nelement;n++) {
		if (element->id==id) return(element);
		element++;
	}
	
	return(NULL);
}

void element_enable(int id,bool enabled)
{
	element_type		*element;
	
	element=element_find(id);
	if (element!=NULL) element->enabled=enabled;
}

void element_hide(int id,bool hide)
{
	element_type		*element;
	
	element=element_find(id);
	if (element!=NULL) element->hidden=hide;
}

void element_table_busy(int id,bool busy)
{
	element_type		*element;
	
	element=element_find(id);
	if (element!=NULL) element->setup.table.busy=busy;
}

void element_text_change(int id,char *str)
{
	element_type		*element;
	
	element=element_find(id);
	if (element!=NULL) strcpy(element->str,str);
}

/* =======================================================

      Select Element
      
======================================================= */

void element_get_box(element_type *element,int *lft,int *rgt,int *top,int *bot)
{
	switch (element->type) {
	
		case element_type_button:
			*lft=element->x;
			*rgt=(*lft)+element->wid;
			*top=element->y;
			*bot=(*top)+element->high;
			return;
			
		case element_type_bitmap:
		case element_type_table:
		case element_type_tab:
			*lft=element->x;
			*rgt=(*lft)+element->wid;
			*top=element->y;
			*bot=(*top)+element->high;
			return;
			
		case element_type_text:
			switch (element->setup.text.just) {
				case tx_center:
					*lft=element->x-(element->wid>>1);
					*rgt=element->x+(element->wid>>1);
					break;
				case tx_right:
					*lft=element->x-element->wid;
					*rgt=element->x;
					break;
				default:
					*lft=element->x;
					*rgt=element->x+element->wid;
					break;
			}
			*top=element->y-element->high;
			*bot=element->y;
			return;

		case element_type_text_field:
			*lft=element->x;
			*rgt=(element->x+10)+element->wid;
			*top=(element->y-element->high)-1;
			*bot=element->y+1;
			return;
			
		case element_type_checkbox:
			*lft=element->x+5;
			*rgt=(element->x+5)+element->wid;
			*top=element->y-element->high;
			*bot=element->y;
			return;
			
		case element_type_combo:
			*lft=element->x;
			*rgt=element->x+element->wid;
			*top=(element->y-element->high)-1;
			*bot=element->y+1;
			return;
			
		case element_type_slider:
			*lft=element->x;
			*rgt=element->x+element->wid;
			*top=element->y-element->high;
			*bot=element->y;
			return;
			
	}
	
	*lft=*rgt=*top=*bot=-1;
}

int element_find_for_xy(int x,int y)
{
	int					n,lft,rgt,top,bot;
	element_type		*element;
	
	element=elements;
	
	for (n=0;n<nelement;n++) {
		if ((element->selectable) && (!element->hidden)) {
			element_get_box(element,&lft,&rgt,&top,&bot);
			if ((x>=lft) && (x<=rgt) && (y>=top) && (y<=bot)) return(element->id);
		}
		element++;
	}
	
	return(-1);
}

/* =======================================================

      Button Element
      
======================================================= */

void element_draw_button(element_type *element,int sel_id)
{
	int				lft,rgt,top,bot;
	
	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	if (element->enabled) {
		if (element->id==sel_id) {
			gl_texture_simple_set(element->setup.button.bitmap_select.gl_id,TRUE,1.0f,1.0f,1.0f,1.0f);
		}
		else {
			gl_texture_simple_set(element->setup.button.bitmap.gl_id,TRUE,1.0f,1.0f,1.0f,1.0f);
		}
	}
	else {
		gl_texture_simple_set(element->setup.button.bitmap.gl_id,TRUE,1.0f,1.0f,1.0f,0.5f);
	}
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(lft,top);
	glTexCoord2f(1,0);
	glVertex2i(rgt,top);
	glTexCoord2f(1,1);
	glVertex2i(rgt,bot);
	glTexCoord2f(0,1);
	glVertex2i(lft,bot);
	glEnd();
	
	gl_texture_simple_end();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      Bitmap Element
      
======================================================= */

void element_draw_bitmap(element_type *element)
{
	int				lft,rgt,top,bot;
	
	element_get_box(element,&lft,&rgt,&top,&bot);

		// the picture
		
	if (element->setup.button.bitmap.gl_id!=-1) {
		gl_texture_simple_start();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL,0);

		glDisable(GL_DEPTH_TEST);

		gl_texture_simple_set(element->setup.button.bitmap.gl_id,TRUE,1,1,1,1);
		
		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2i(lft,top);
		glTexCoord2f(1,0);
		glVertex2i(rgt,top);
		glTexCoord2f(1,1);
		glVertex2i(rgt,bot);
		glTexCoord2f(0,1);
		glVertex2i(lft,bot);
		glEnd();
		
		gl_texture_simple_end();

		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
	}
	
		// the frame
		
	if (element->framed) {
		glColor4f(hud.color.base.r,hud.color.base.g,hud.color.base.b,1);
		
		glBegin(GL_LINE_LOOP);
		glVertex2i(lft,top);
		glVertex2i(rgt,top);
		glVertex2i(rgt,bot);
		glVertex2i(lft,bot);
		glEnd();
	}
}

/* =======================================================

      Text Element
      
======================================================= */

void element_draw_text(element_type *element,int sel_id)
{
	int				y,high;
	char			*c,*c2;
	d3col			col;

		// get text color

	if (element->setup.text.alert) {
		col.r=1.0f;
		col.g=col.b=0.0f;
	}
	else {
		if (!element->enabled) {
			memmove(&col,&hud.color.disabled,sizeof(d3col));
		}
		else {
			if ((element->id!=-1) && (element->id==sel_id)) {
				memmove(&col,&hud.color.mouse_over,sizeof(d3col));
			}
			else {
				memmove(&col,&hud.color.base,sizeof(d3col));
			}
		}
	}

		// get height

	y=element->y;
	high=gl_text_get_char_height(element->setup.text.small_text);

		// draw text

	gl_text_start(element->setup.text.small_text);

	c=element->str;
	
	while (*c!=0x0) {
		c2=strstr(c,"{r}");
		if (c2!=NULL) *c2=0x0;
		
		gl_text_draw(element->x,y,c,element->setup.text.just,FALSE,&col,1.0f);
		
		if (c2==NULL) break;
		
		*c2='{';
		
		c=c2+3;
		y+=high;
	}

	gl_text_end();
}

/* =======================================================

      Text Field Element
      
======================================================= */

void element_click_text_field(element_type *element)
{
	element_open_text_field_id=element->id;
}

bool element_click_text_field_open(element_type *element,int x,int y)
{
	int				high,lft,rgt,top,bot;
	
	high=gl_text_get_char_height(TRUE);
		
		// control box
		
	lft=element->x+3;
	rgt=lft+252;
	top=(element->y-high)-1;
	bot=element->y;
	
		// in box?
		
	if ((x<lft) || (x>rgt) || (y<top) || (y>bot)) return(FALSE);

	return(TRUE);
}

void element_draw_text_field(element_type *element,int sel_id)
{
	int				x,y,ky,lft,rgt,top,bot;
	float			alpha;
	char			txt[256];
	
	x=element->x;
	y=element->y;
	
		// label

	ky=y-(element->high>>1);
		
	gl_text_start(TRUE);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.base,1.0f);
	gl_text_draw(x,ky,":",tx_center,TRUE,&hud.color.base,1.0f);
	gl_text_end();
		
		// control box
		
	lft=x+5;
	rgt=lft+element->wid;
	top=(y-element->high)-1;
	bot=y+1;
	
	alpha=(element->enabled?1.0f:0.5f);

	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	if (element->id==sel_id) {
		gl_texture_simple_set(element_textbox_selected.gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
	}
	else {
		gl_texture_simple_set(element_textbox.gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
	}

	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(lft,top);
	glTexCoord2f(1,0);
	glVertex2i(rgt,top);
	glTexCoord2f(1,1);
	glVertex2i(rgt,bot);
	glTexCoord2f(0,1);
	glVertex2i(lft,bot);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

	gl_texture_simple_end();

		// control text

	strcpy(txt,element->value_str);
	if (element_open_text_field_id==element->id) strcat(txt,"_");

	gl_text_start(TRUE);
		
	if (element->enabled) {
		if (element->id==element_open_text_field_id) {
			gl_text_draw((x+10),ky,txt,tx_left,TRUE,&hud.color.mouse_over,1.0f);
		}
		else {
			gl_text_draw((x+10),ky,txt,tx_left,TRUE,&hud.color.base,1.0f);
		}
	}
	else {
		gl_text_draw((x+10),ky,txt,tx_left,TRUE,&hud.color.disabled,1.0f);
	}
	
	gl_text_end();
}

/* =======================================================

      Checkbox Element
      
======================================================= */

void element_click_checkbox(element_type *element)
{
	element->value=element->value^0x1;
}

void element_draw_checkbox(element_type *element,int sel_id)
{
	int				x,y,ky,lft,rgt,top,bot;
	float			alpha;
	
	x=element->x;
	y=element->y;
	
		// label

	ky=y-(element->high>>1);

	gl_text_start(TRUE);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.base,1.0f);
	gl_text_draw(x,ky,":",tx_center,TRUE,&hud.color.base,1.0f);
	gl_text_end();
	
		// checkbox
		
	lft=x+5;
	rgt=lft+(element->wid-1);
	top=(y-element->high)+1;
	bot=y-1;
	
	alpha=(element->enabled?1.0f:0.5f);

	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	if (element->value==0) {
		if ((element->id==sel_id) && (element->enabled)) {
			gl_texture_simple_set(element_checkbox_false_selected.gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
		}
		else {
			gl_texture_simple_set(element_checkbox_false.gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
		}
	}
	else {
		if ((element->id==sel_id) && (element->enabled)) {
			gl_texture_simple_set(element_checkbox_true_selected.gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
		}
		else {
			gl_texture_simple_set(element_checkbox_true.gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
		}
	}

	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(lft,top);
	glTexCoord2f(1,0);
	glVertex2i(rgt,top);
	glTexCoord2f(1,1);
	glVertex2i(rgt,bot);
	glTexCoord2f(0,1);
	glVertex2i(lft,bot);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

	gl_texture_simple_end();
}

/* =======================================================

      Combo Element
      
======================================================= */

int element_get_combo_list_count(element_type *element)
{
	int			count;
	char		*c;
	
	count=0;
	c=element->data;
	
	while (TRUE) {
		if (*c==0x0) return(count);
		c+=32;
		count++;
	}

	return(0);
}

void element_click_combo(element_type *element)
{
	element_open_combo_id=element->id;
}

void element_box_combo_open(element_type *element,int *lft,int *rgt,int *top,int *bot)
{
	int				cnt;

	cnt=element_get_combo_list_count(element);
		
	*lft=element->x+5;
	*rgt=*lft+150;
	*top=(element->y-element->high)-1;
	*bot=*top+((cnt*element->high)+1);
}

bool element_in_combo_open(element_type *element,int x,int y)
{
	int				lft,rgt,top,bot;
	
	element_box_combo_open(element,&lft,&rgt,&top,&bot);
	if ((x<lft) || (x>rgt) || (y<top) || (y>bot)) return(FALSE);
	
	return(TRUE);
}

bool element_click_combo_open(element_type *element,int x,int y)
{
	int				lft,rgt,top,bot;

		// in box?
		
	element_box_combo_open(element,&lft,&rgt,&top,&bot);
	if ((x<lft) || (x>rgt) || (y<top) || (y>bot)) return(FALSE);
	
		// change clicked item and close
		
	element->value=(y-top)/element->high;
	
		// close open combo
		
	element_open_combo_id=-1;
	
	return(TRUE);
}

void element_draw_combo(element_type *element,int sel_id)
{
	int				x,y,ky,lft,rgt,top,bot;
	char			str[256];
	float			alpha;
	
	x=element->x;
	y=element->y;
	
		// label
	
	ky=y-(element->high>>1);

	gl_text_start(TRUE);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.base,1.0f);
	gl_text_draw(x,ky,":",tx_center,TRUE,&hud.color.base,1.0f);
	gl_text_end();
		
		// combo box
		
	lft=x+5;
	rgt=lft+element->wid;
	top=(y-element->high)-1;
	bot=y+1;

	alpha=(element->enabled?1.0f:0.5f);

	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

		// text box

	if ((element->id==sel_id) && (element->enabled)) {
		gl_texture_simple_set(element_combo_close_selected.gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
	}
	else {
		gl_texture_simple_set(element_combo_close.gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
	}

	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(lft,top);
	glTexCoord2f(1,0);
	glVertex2i(rgt,top);
	glTexCoord2f(1,1);
	glVertex2i(rgt,bot);
	glTexCoord2f(0,1);
	glVertex2i(lft,bot);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

	gl_texture_simple_end();

		// control text

	strcpy(str,(element->data+(element->value*32)));
		
	gl_text_start(TRUE);
		
	if (element->enabled) {
		gl_text_draw((x+10),ky,str,tx_left,TRUE,&hud.color.base,1.0f);
	}
	else {
		gl_text_draw((x+10),ky,str,tx_left,TRUE,&hud.color.disabled,1.0f);
	}
	
	gl_text_end();
}

void element_draw_combo_open(element_type *element)
{
	int				x,y,n,cnt,lft,rgt,top,bot;
	char			str[256];
	
	x=element->x;
	y=element->y;
	
	cnt=element_get_combo_list_count(element);

		// combo items

	for (n=0;n!=cnt;n++) {

			// graphic

		lft=x+5;
		rgt=lft+element->wid;
		top=(y-element->high)-1;
		bot=y+1;

		gl_texture_simple_start();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL,0);

		glDisable(GL_DEPTH_TEST);

		if (n==0) {
			if (element->value==n) {
				gl_texture_simple_set(element_combo_open_top_selected.gl_id,TRUE,1.0f,1.0f,1.0f,1.0f);
			}
			else {
				gl_texture_simple_set(element_combo_open_top.gl_id,TRUE,1.0f,1.0f,1.0f,1.0f);
			}
		}
		else {
			if (n==(cnt-1)) {
				if (element->value==n) {
					gl_texture_simple_set(element_combo_open_bottom_selected.gl_id,TRUE,1.0f,1.0f,1.0f,1.0f);
				}
				else {
					gl_texture_simple_set(element_combo_open_bottom.gl_id,TRUE,1.0f,1.0f,1.0f,1.0f);
				}
			}
			else {
				if (element->value==n) {
					gl_texture_simple_set(element_combo_open_middle_selected.gl_id,TRUE,1.0f,1.0f,1.0f,1.0f);
				}
				else {
					gl_texture_simple_set(element_combo_open_middle.gl_id,TRUE,1.0f,1.0f,1.0f,1.0f);
				}
			}
		}

		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2i(lft,top);
		glTexCoord2f(1,0);
		glVertex2i(rgt,top);
		glTexCoord2f(1,1);
		glVertex2i(rgt,bot);
		glTexCoord2f(0,1);
		glVertex2i(lft,bot);
		glEnd();

		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);

		gl_texture_simple_end();

			// text

		gl_text_start(TRUE);
		strcpy(str,(element->data+(n*32)));
		gl_text_draw((x+10),(y-(element->high>>1)),str,tx_left,TRUE,&hud.color.base,1.0f);
		gl_text_end();

		y+=element->high;
	}
}

/* =======================================================

      Slider Element
      
======================================================= */

void element_click_slider(element_type *element,int x,int y)
{
	x=x-(element->x+5);
	if (x<0) x=0;
	if (x>element->wid) x=element->wid;
	
	element->setup.slider.value=((float)x)/((float)element->wid);
}

void element_move_slider(element_type *element,int x,int y)
{
	int				id;
	
	id=element_find_for_xy(x,y);
	if (id==element->id) element_click_slider(element,x,y);
}

void element_draw_slider(element_type *element,int sel_id)
{
	int				x,y,ky,lft,rgt,top,bot,mid;
	float			alpha;
	
	x=element->x;
	y=element->y;
	
		// label
	
	ky=y-(element->high>>1);

	gl_text_start(TRUE);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.base,1.0f);
	gl_text_draw(x,ky,":",tx_center,TRUE,&hud.color.base,1.0f);
	gl_text_end();
	
		// slider size
		
	lft=x+5;
	rgt=lft+element->wid;
	top=(y-element->high)+1;
	bot=y-1;
	
	mid=lft+(int)(((float)element->wid)*element->setup.slider.value);

	alpha=(element->enabled?1.0f:0.5f);

		// draw slider

	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

		// slider box

	if (element->id==sel_id) {
		gl_texture_simple_set(element_textbox_selected.gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
	}
	else {
		gl_texture_simple_set(element_textbox.gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
	}

	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(lft,top);
	glTexCoord2f(1,0);
	glVertex2i(rgt,top);
	glTexCoord2f(1,1);
	glVertex2i(rgt,bot);
	glTexCoord2f(0,1);
	glVertex2i(lft,bot);
	glEnd();
	
		// slider value
		
	if (element->enabled) {
		gl_texture_simple_set(element_slider_fill.gl_id,FALSE,1.0f,1.0f,1.0f,alpha);

		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2i((lft+1),(top+1));
		glTexCoord2f(1,0);
		glVertex2i(mid,(top+1));
		glTexCoord2f(1,1);
		glVertex2i(mid,(bot-1));
		glTexCoord2f(0,1);
		glVertex2i((lft+1),(bot-1));
		glEnd();
	}

		// slider drag

	if (element->id==sel_id) {
		gl_texture_simple_set(element_slider_selected.gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
	}
	else {
		gl_texture_simple_set(element_slider.gl_id,TRUE,1.0f,1.0f,1.0f,alpha);
	}
	
	if ((mid+16)>rgt) mid=rgt-16;

	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(mid,top);
	glTexCoord2f(1,0);
	glVertex2i((mid+16),top);
	glTexCoord2f(1,1);
	glVertex2i((mid+16),bot);
	glTexCoord2f(0,1);
	glVertex2i(mid,bot);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

	gl_texture_simple_end();
}

/* =======================================================

      Table Element
      
======================================================= */

int element_get_table_row_count(element_type *element)
{
	int			count;
	char		*c;
	
	c=element->data;
	if (c==NULL) return(0);
	
	count=0;
	
	while (TRUE) {
		if (*c==0x0) return(count);
		c+=128;
		count++;
	}

	return(0);
}

inline int element_get_table_row_high(element_type *element)
{
	if (element->setup.table.bitmap_mode==element_table_bitmap_none) return(gl_text_get_char_height(TRUE)+2);

	return(element_table_bitmap_size+2);
}

bool element_click_table(element_type *element,int x,int y)
{
	int				high,row_high,row_cnt,cnt;
	bool			up_ok,down_ok;
	
		// get text sizes
		
	high=gl_text_get_char_height(TRUE)+2;
	row_high=element_get_table_row_high(element);
	
	row_cnt=element_get_table_row_count(element);
	
		// check for clicking in scroll bar
		
	if (x>((element->x+element->wid)-24)) {
		element->value=-1;
		
			// get scrolling sizes
			
		cnt=((element->high-(high+4))/row_high)-1;
		up_ok=(element->offset!=0);
		down_ok=((element->offset+cnt)<row_cnt);
		
		y-=element->y;
		
		if ((y>=(high+4)) && (y<=(high+28)) && (up_ok)) {
			element->offset-=cnt;
			return(TRUE);
		}
		
		if ((y>=(element->high-24)) && (down_ok)) {
			element->offset+=cnt;
			return(TRUE);
		}
	
		return(FALSE);
	}
	
		// select cliked on element
		
	y-=((element->y+4)+(high+2));
	element->value=element->offset+(y/element_get_table_row_high(element));
	
	if ((element->value<0) || (element->value>=row_cnt)) {
		element->value=-1;
		return(FALSE);
	}
	
	return(TRUE);
}

void element_draw_table_background_fills(element_type *element,int row_high)
{
	int				lft,rgt,top,bot,x,y;
	
		// header
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	
	bot=(top+row_high)+4;
	y=(top+bot)>>1;
		
	glBegin(GL_QUADS);
	
	glColor4f(hud.color.header.r,hud.color.header.g,hud.color.header.b,1.0f);
	glVertex2i(lft,top);
	glVertex2i(rgt,top);
	glColor4f((hud.color.header.r/2.0f),(hud.color.header.g/2.0f),(hud.color.header.b/2.0f),1.0f);
	glVertex2i(rgt,y);
	glVertex2i(lft,y);
	
	glColor4f((hud.color.header.r/2.0f),(hud.color.header.g/2.0f),(hud.color.header.b/2.0f),1.0f);
	glVertex2i(lft,y);
	glVertex2i(rgt,y);
	glColor4f(hud.color.header.r,hud.color.header.g,hud.color.header.b,1.0f);
	glVertex2i(rgt,bot);
	glVertex2i(lft,bot);
	
	glEnd();
	
		// scroll bar
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	
	lft=rgt-24;
	top+=(row_high+4);
	
	x=(lft+rgt)>>1;
		
	glBegin(GL_QUADS);
	
	glColor4f(0.5f,0.5f,0.5f,1.0f);
	glVertex2i(lft,bot);
	glVertex2i(lft,top);
	glColor4f(0.3f,0.3f,0.3f,1.0f);
	glVertex2i(x,top);
	glVertex2i(x,bot);
	
	glColor4f(0.3f,0.3f,0.3f,1.0f);
	glVertex2i(x,bot);
	glVertex2i(x,top);
	glColor4f(0.5f,0.5f,0.5f,1.0f);
	glVertex2i(rgt,top);
	glVertex2i(rgt,bot);
	
	glEnd();
}

void element_draw_table_line_lines(element_type *element,int x,int y,int wid,int row_high,d3col *line_col)
{
	int			n,cx,lx,ty,by;
	
	glColor4f(line_col->r,line_col->g,line_col->b,1.0f);

	cx=x;
	
	for (n=1;n<element->setup.table.ncolumn;n++) {
		cx+=(int)(element->setup.table.cols[n-1].percent_size*(float)wid);
	
		lx=cx;
		ty=y;
		by=y+row_high;
		
		glBegin(GL_LINES);
		glVertex2i(lx,ty);
		glVertex2i(lx,by);
		glEnd();
	}
}

void element_draw_table_line_header(element_type *element,int x,int y,int wid,int row_high)
{
	int			n;
	d3col		col;
	
	col.r=col.g=col.b=0.0f;

	y+=(row_high>>1);
	
	for (n=0;n!=element->setup.table.ncolumn;n++) {
		gl_text_start(TRUE);
		gl_text_draw((x+4),y,element->setup.table.cols[n].name,tx_left,TRUE,&col,1.0f);
		gl_text_end();
		
		x+=(int)(element->setup.table.cols[n].percent_size*(float)wid);
	}
}

void element_draw_table_line_data(element_type *element,int x,int y,int row,int wid,int row_high,d3col *txt_col,char *data)
{
	int				n,dx,dy;
	unsigned long	gl_id;
	char			*c,*c2,txt[256];
	d3col			col;

	dy=y+(row_high>>1);
	
	c=data;
	
	for (n=0;n!=element->setup.table.ncolumn;n++) {
	
			// get current data
			
		strncpy(txt,c,255);
		txt[255]=0x0;
		c2=strchr(txt,'\t');
		if (c2!=NULL) *c2=0x0;

			// draw any bitmaps

		dx=x+4;

		if ((element->setup.table.bitmap_mode!=element_table_bitmap_none) && (n==0)) {

				// draw bitmap

			gl_id=element->setup.table.bitmap[row+n].gl_id;

			if (gl_id!=-1) {

				gl_texture_simple_start();
				gl_texture_simple_set(gl_id,TRUE,1.0f,1.0f,1.0f,1.0f);

				glDisable(GL_BLEND);
				glDisable(GL_ALPHA_TEST);
				glDisable(GL_DEPTH_TEST);
		
				glBegin(GL_QUADS);
				glTexCoord2f(0,0);
				glVertex2i(dx,(y+1));
				glTexCoord2f(1,0);
				glVertex2i((dx+element_table_bitmap_size),(y+1));
				glTexCoord2f(1,1);
				glVertex2i((dx+element_table_bitmap_size),((y+1)+element_table_bitmap_size));
				glTexCoord2f(0,1);
				glVertex2i(dx,((y+1)+element_table_bitmap_size));
				glEnd();

				gl_texture_simple_end();
				
				glColor4f(1.0f,1.0f,1.0f,1.0f);
				
				glBegin(GL_LINE_LOOP);
				glVertex2i(dx,(y+1));
				glVertex2i((dx+element_table_bitmap_size),(y+1));
				glVertex2i((dx+element_table_bitmap_size),((y+1)+element_table_bitmap_size));
				glVertex2i(dx,((y+1)+element_table_bitmap_size));
				glEnd();
			}

				// missing graphic

			else {

				glBegin(GL_QUADS);
				glColor4f(0.6f,0.6f,0.6f,1.0f);
				glVertex2i((dx+2),(y+4));
				glVertex2i(((dx-4)+element_table_bitmap_size),(y+4));
				glColor4f(0.4f,0.4f,0.4f,1.0f);
				glVertex2i(((dx-4)+element_table_bitmap_size),((y-2)+element_table_bitmap_size));
				glVertex2i((dx+2),((y-2)+element_table_bitmap_size));
				glEnd();
			
				glColor4f(1.0f,1.0f,1.0f,1.0f);
				glLineWidth(2.0f);
				
				glBegin(GL_LINE_LOOP);
				glVertex2i((dx+2),(y+4));
				glVertex2i(((dx-4)+element_table_bitmap_size),(y+4));
				glVertex2i(((dx-4)+element_table_bitmap_size),((y-2)+element_table_bitmap_size));
				glVertex2i((dx+2),((y-2)+element_table_bitmap_size));
				glEnd();
				
				glLineWidth(1.0f);

				col.r=col.g=col.b=1.0f;

				gl_text_start(FALSE);
				gl_text_draw((dx+(element_table_bitmap_size>>1)),(y+(element_table_bitmap_size>>1)),"?",tx_center,TRUE,&col,1.0f);
				gl_text_end();
			}

				// get to correct text

			c2=strchr(txt,';');
			if (c2!=NULL) {
				c2=strchr((c2+1),';');
				if (c2!=NULL) {
					strcpy(txt,(c2+1));
				}
			}

			dx+=(element_table_bitmap_size+4);
		}
		
			// draw text
			
		gl_text_start(TRUE);
		gl_text_draw(dx,dy,txt,tx_left,TRUE,txt_col,1.0f);
		gl_text_end();
		
			// get next data
			
		c=strchr(c,'\t');
		if (c==NULL) break;
		c++;
		
		x+=(int)(element->setup.table.cols[n].percent_size*(float)wid);
	}
}

void element_table_load_row_bitmaps(element_type *element)
{
	int				n,nrow;
	char			*c,*c2,subdir[256],filename[256],path[1024];

	if (element->setup.table.bitmap_mode==element_table_bitmap_none) return;
	if (element->setup.table.bitmap!=NULL) return;

	nrow=element_get_table_row_count(element);
	element->setup.table.nbitmap=nrow;

		// memory for bitmaps

	element->setup.table.bitmap=(bitmap_type*)valloc(nrow*sizeof(bitmap_type));
	if (element->setup.table.bitmap==NULL) {
		element->setup.table.bitmap_mode=element_table_bitmap_none;
		return;
	}

		// load bitmaps

	for (n=0;n!=nrow;n++) {
		c=element->data+(n*128);
	
		strcpy(subdir,c);
		c2=strchr(subdir,';');
		if (c2==NULL) continue;

		*c2=0x0;
		strcpy(filename,(c2+1));

		c2=strchr(filename,';');
		if (c2==NULL) continue;

		*c2=0x0;

		if (element->setup.table.bitmap_mode==element_table_bitmap_data) {
			file_paths_data(&setup.file_path_setup,path,subdir,filename,"png");
		}
		else {
			file_paths_documents(&setup.file_path_setup,path,subdir,filename,"png");
		}

		bitmap_open(&element->setup.table.bitmap[n],path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);
	}
}

void element_draw_table(element_type *element,int sel_id)
{
	int				n,k,x,y,wid,high,cnt,lft,rgt,top,bot,row_high;
	float			gx,gy;
	char			*c;
	bool			up_ok,down_ok;
	d3col			line_col;

		// load row bitmaps if needed

	element_table_load_row_bitmaps(element);

		// sizes
	
	wid=element->wid-30;
	high=gl_text_get_char_height(TRUE)+2;
	row_high=element_get_table_row_high(element);
	
		// get element counts
		
	cnt=((element->high-(high+4))/row_high)-1;
	up_ok=(element->offset!=0);
	down_ok=((element->offset+cnt)<element_get_table_row_count(element));
	
		// header and scroll bar fill
		
	element_draw_table_background_fills(element,high);
		
		// outline
		
	glColor4f(hud.color.base.r,hud.color.base.g,hud.color.base.b,1);
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	
	glBegin(GL_LINE_LOOP);
	glVertex2i(lft,top);
	glVertex2i(rgt,top);
	glVertex2i(rgt,bot);
	glVertex2i(lft,bot);
	glEnd();
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	top+=(high+4);
	
	glBegin(GL_LINES);
	glVertex2i(lft,top);
	glVertex2i(rgt,top);
	glEnd();
	
		// scrolling lines
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	lft=rgt-24;
	
	glBegin(GL_LINES);
	glVertex2i(lft,top);
	glVertex2i(lft,bot);
	glEnd();
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	lft=rgt-24;
	top+=(high+28);
	
	glBegin(GL_LINES);
	glVertex2i(lft,top);
	glVertex2i(rgt,top);
	glEnd();
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	lft=rgt-24;
	bot-=24;
	
	glBegin(GL_LINES);
	glVertex2i(lft,bot);
	glVertex2i(rgt,bot);
	glEnd();
	
		// busy icon scrolling arrows
		
	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

		// busy
		
	if (element->setup.table.busy) {
		element_get_box(element,&lft,&rgt,&top,&bot);
		rgt-=2;
		lft=rgt-20;
		top+=((high-20)/2);
		bot=top+20;
		
		gl_texture_simple_set(element_busy.gl_id,TRUE,1,1,1,1.0f);
		
		k=(time_get()>>8)&0x3;
		gx=((float)(k%2))*0.5f;
		gy=((float)(k/2))*0.5f;
		
		glBegin(GL_QUADS);
		glTexCoord2f(gx,gy);
		glVertex2i(lft,top);
		glTexCoord2f((gx+0.5f),gy);
		glVertex2i(rgt,top);
		glTexCoord2f((gx+0.5f),(gy+0.5f));
		glVertex2i(rgt,bot);
		glTexCoord2f(gx,(gy+0.5f));
		glVertex2i(lft,bot);
		glEnd();
	}

		// scroll up

	element_get_box(element,&lft,&rgt,&top,&bot);
	lft=rgt-20;
	rgt=lft+16;
	top+=(high+8);
	bot=top+16;
	
	gl_texture_simple_set(element_scroll_up.gl_id,TRUE,1,1,1,(up_ok?1.0f:0.1f));
	
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(lft,top);
	glTexCoord2f(1,0);
	glVertex2i(rgt,top);
	glTexCoord2f(1,1);
	glVertex2i(rgt,bot);
	glTexCoord2f(0,1);
	glVertex2i(lft,bot);
	glEnd();

		// scroll down
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	lft=rgt-20;
	rgt=lft+16;
	bot-=4;
	top=bot-16;
	
	gl_texture_simple_set(element_scroll_down.gl_id,TRUE,1,1,1,(down_ok?1.0f:0.1f));
	
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(lft,top);
	glTexCoord2f(1,0);
	glVertex2i(rgt,top);
	glTexCoord2f(1,1);
	glVertex2i(rgt,bot);
	glTexCoord2f(0,1);
	glVertex2i(lft,bot);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

	gl_texture_simple_end();
	
		// text positions
		
	x=element->x;
	y=element->y+3;
	
		// header
		
	line_col.r=line_col.g=line_col.b=0.8f;
	element_draw_table_line_lines(element,x,y,wid,high,&line_col);
	element_draw_table_line_header(element,x,y,wid,high);
	
		// items
		
	if (element->data==NULL) return;
	
	y=(element->y+4)+(high+2);
	
	c=element->data+(element->offset*128);
	
	for (n=0;n<=cnt;n++) {
		if (*c==0x0) break;
		
			// selection or background
			
		if ((n+element->offset)==element->value) {
			glColor4f(hud.color.hilite.r,hud.color.hilite.g,hud.color.hilite.b,1.0f);
			line_col.r=line_col.g=line_col.b=0.0f;
		}
		else {
			if (((n+element->offset)&0x1)==0) {
				glColor4f(0.0f,0.0f,0.0f,1.0f);
				line_col.r=line_col.g=line_col.b=0.25f;
			}
			else {
				glColor4f(0.15f,0.15f,0.15f,1.0f);
				line_col.r=line_col.g=line_col.b=0.0f;
			}
		}
			
		element_get_box(element,&lft,&rgt,&top,&bot);
		
		lft+=2;
		rgt-=26;
		top=y;
		bot=y+row_high;
		
		glBegin(GL_QUADS);
		glVertex2i(lft,top);
		glVertex2i(rgt,top);
		glVertex2i(rgt,bot);
		glVertex2i(lft,bot);
		glEnd();
		
			// table line
			
		element_draw_table_line_lines(element,x,y,wid,row_high,&line_col);
		element_draw_table_line_data(element,x,y,(element->offset+n),wid,row_high,&hud.color.base,c);
		
		c+=128;
		y+=row_high;
	}
}

/* =======================================================

      Tab Element
      
======================================================= */

int element_mouse_over_tab(element_type *element,int x)
{
	int				lft,rgt,max_sz,xadd;
	
		// within tab box?
		
	lft=element->x+(int)(((float)hud.scale_x)*0.02f);
	rgt=lft+element->setup.tab.list_wid;
	
	if ((x<lft) || (x>rgt)) return(-1);
	
		// select value
		
	xadd=element->setup.tab.list_wid/element->setup.tab.ntab;

	max_sz=(int)(((float)hud.scale_x)*0.2f);
	if (xadd>max_sz) xadd=max_sz;

	return((x-lft)/xadd);
}

bool element_click_tab(element_type *element,int x)
{
	int				value;
	
	value=element_mouse_over_tab(element,x);
	if (value==-1) return(FALSE);
	
	element->value=value;
	
	return(TRUE);
}

void element_draw_tab(element_type *element,int sel_id,int x)
{
	int				n,ky,xstart,xadd,high,max_sz,
					lft,rgt,top,bot,lx,rx,ty,by,
					klft,krgt,ktop;
	
	high=gl_text_get_char_height(TRUE);
		
		// sizes
	
	element_get_box(element,&lft,&rgt,&top,&bot);

	xadd=element->setup.tab.list_wid/element->setup.tab.ntab;

	max_sz=(int)(((float)hud.scale_x)*0.2f);
	if (xadd>max_sz) xadd=max_sz;

		// bitmap drawing
		
	gl_texture_simple_start();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

		// top and bottom borders

	lx=lft;
	rx=rgt;

	klft=0;
	krgt=xadd;
	ktop=top;
	ky=bot;

	gl_texture_simple_set(element_tab_border.gl_id,FALSE,1,1,1,1);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(lx,ky);
	glTexCoord2f(1,0);
	glVertex2i(rx,ky);
	glTexCoord2f(1,1);
	glVertex2i(rx,(ky+4));
	glTexCoord2f(0,1);
	glVertex2i(lx,(ky+4));
	glEnd();

	ky=top+element->setup.tab.ext_high;

	glBegin(GL_QUADS);
	glTexCoord2f(0,0);
	glVertex2i(lx,(ky-4));
	glTexCoord2f(1,0);
	glVertex2i(rx,(ky-4));
	glTexCoord2f(1,1);
	glVertex2i(rx,ky);
	glTexCoord2f(0,1);
	glVertex2i(lx,ky);
	glEnd();

		// tabs
		
	xstart=lft+(int)(((float)hud.scale_x)*0.02f);
		
	for (n=0;n!=element->setup.tab.ntab;n++) {
		lx=xstart+(xadd*n);
		rx=lx+xadd;
		ty=top;
		by=bot+1;
		
		if (element->value==n) {
			gl_texture_simple_set(element->setup.tab.bitmap_select[n].gl_id,TRUE,1,1,1,1);
		}
		else {
			gl_texture_simple_set(element->setup.tab.bitmap[n].gl_id,TRUE,1,1,1,1);
		}

		glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex2i(lx,ty);
		glTexCoord2f(1,0);
		glVertex2i(rx,ty);
		glTexCoord2f(1,1);
		glVertex2i(rx,by);
		glTexCoord2f(0,1);
		glVertex2i(lx,by);
		glEnd();
	}

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

	gl_texture_simple_end();
}

/* =======================================================

      Draw Elements
      
======================================================= */

void element_draw_lock(bool cursor_hilite)
{
	int					n,id,x,y;
	element_type		*element;
	
		// get element under cursor

	id=-1;

	if (cursor_hilite) {

			// find hilited item

		if (element_open_text_field_id!=-1) {
			id=element_open_text_field_id;
		}
		else {
			if (element_open_combo_id!=-1) {
				id=element_open_combo_id;
			}
			else {
				input_gui_get_mouse_position(&x,&y);
				id=element_find_for_xy(x,y);
			}
		}

			// if clicking and holding, then we
			// can only hilite clicked item

		if (element_click_down_id!=-1) {
			if (id!=element_click_down_id) id=-1;
		}
	}

		// setup draw

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_LINE_SMOOTH);

	glLineWidth(1.0f);
	
		// draw elements
				
	element=elements;
	
	for (n=0;n<nelement;n++) {

		if (element->hidden) {
			element++;
			continue;
		}
		
			// draw element
	
		switch (element->type) {
		
			case element_type_button:
				element_draw_button(element,id);
				break;
			case element_type_bitmap:
				element_draw_bitmap(element);
				break;
			case element_type_text:
				element_draw_text(element,id);
				break;
			case element_type_text_field:
				element_draw_text_field(element,id);
				break;
			case element_type_checkbox:
				element_draw_checkbox(element,id);
				break;
			case element_type_combo:
				element_draw_combo(element,id);
				break;
			case element_type_slider:
				element_draw_slider(element,id);
				break;
			case element_type_table:
				element_draw_table(element,id);
				break;
			case element_type_tab:
				element_draw_tab(element,id,x);
				break;
				
		}
		
		element++;
	}
	
		// draw open combo
		
	if (element_open_combo_id!=-1) {
		element_draw_combo_open(element_find(element_open_combo_id));
	}

		// reset line smoothing

	glEnable(GL_LINE_SMOOTH);
}

void element_draw(bool cursor_hilite)
{
	pthread_mutex_lock(&element_thread_lock);
	element_draw_lock(cursor_hilite);
	pthread_mutex_unlock(&element_thread_lock);
}

/* =======================================================

      Click Elements
      
======================================================= */

void element_click_down_lock(int x,int y)
{
	element_type		*element;

		// already in a down click, if so,
		// check if we are in a slider

	if (element_click_down_id!=-1) {
		element=element_find(element_click_down_id);
		if (element->type==element_type_slider) {
			element_move_slider(element,x,y);
		}
		return;
	}

		// in open combo?

	if (element_open_combo_id!=-1) {
	
		element=element_find(element_open_combo_id);
		if (element_in_combo_open(element,x,y)) {
			element_click_down_id=element->id;
			return;
		}

		element_open_combo_id=-1;
	}
	
		// get click

	element_click_down_id=element_find_for_xy(x,y);

		// if open text, outside down clicks close

	if (element_open_text_field_id!=-1) {
		if (element_click_down_id!=element_open_text_field_id) element_open_text_field_id=-1;
	}
}

int element_click_up_lock(int x,int y)
{
	int					id;
	element_type		*element;
	
		// no click down ID
		
	if (element_click_down_id==-1) return(-1);
	
		// in open combo?
		
	if (element_open_combo_id!=-1) {

		element=element_find(element_open_combo_id);
		if (element_click_combo_open(element,x,y)) {
			element_click_down_id=-1;
			return(element->id);
		}

		element_open_combo_id=-1;
	}

		// find element

	id=element_find_for_xy(x,y);
	if (id!=element_click_down_id) {
		element_click_down_id=-1;
		return(-1);
	}
	
	element=element_find(id);

		// ignore disabled elements

	if (!element->enabled) {
		element_click_down_id=-1;
		return(-1);
	}
	
		// click it
		
	switch (element->type) {
		
		case element_type_text_field:
			element_click_text_field(element);
			break;
		case element_type_checkbox:
			element_click_checkbox(element);
			break;
		case element_type_combo:
			element_click_combo(element);
			break;
		case element_type_slider:
			element_click_slider(element,x,y);
			break;
		case element_type_table:
			element_click_table(element,x,y);
			break;
		case element_type_tab:
			element_click_tab(element,x);
			break;
			
	}
	
		// turn off down click
		
	element_click_down_id=-1;
	
	return(id);
}

void element_click_down(int x,int y)
{
	pthread_mutex_lock(&element_thread_lock);
	element_click_down_lock(x,y);
	pthread_mutex_unlock(&element_thread_lock);
}

int element_click_up(int x,int y)
{
	int			id;

	pthread_mutex_lock(&element_thread_lock);
	id=element_click_up_lock(x,y);
	pthread_mutex_unlock(&element_thread_lock);

	return(id);
}

/* =======================================================

      Key Elements
      
======================================================= */

int element_key_lock(char ch)
{
	int					len;
	element_type		*element;

		// text field open?

	if (element_open_text_field_id==-1) return(-1);

	element=element_find(element_open_text_field_id);

		// returns or tabs close box

	if ((ch==0xD) || (ch==0x9)) {
		element_open_text_field_id=-1;
		return(element->id);
	}

		// get text field

	len=strlen(element->value_str);

		// backspace?

	if (ch==0x8) {
		if (len==0) return(-1);
		element->value_str[len-1]=0x0;
		return(-1);
	}

		// regular text

	if (len>=(element->max_value_str_sz-1)) return(-1);

	element->value_str[len]=ch;
	element->value_str[len+1]=0x0;

	return(-1);
}

int element_key(char ch)
{
	int			rtn;

	pthread_mutex_lock(&element_thread_lock);
	rtn=element_key_lock(ch);
	pthread_mutex_unlock(&element_thread_lock);

	return(rtn);
}

/* =======================================================

      Find Hilite
      
======================================================= */

int element_get_selected(void)
{
	int			x,y;

	if (element_open_text_field_id!=-1) return(element_open_text_field_id);
	if (element_open_combo_id!=-1) return(element_open_combo_id);

	input_gui_get_mouse_position(&x,&y);
				
	return(element_find_for_xy(x,y));
}

/* =======================================================

      Values
      
======================================================= */

int element_get_value(int id)
{
	int				rtn;
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);

	element=element_find(id);
	if (element==NULL) {
		rtn=-1;
	}
	else {
		rtn=element->value;
	}

	pthread_mutex_unlock(&element_thread_lock);

	return(rtn);
}

float element_get_slider_value(int id)
{
	float			rtn;
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);

	element=element_find(id);
	if (element==NULL) {
		rtn=0.0f;
	}
	else {
		rtn=element->setup.slider.min+((element->setup.slider.max-element->setup.slider.min)*element->setup.slider.value);
	}

	pthread_mutex_unlock(&element_thread_lock);

	return(rtn);
}

void element_get_value_string(int id,char *str)
{
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);

	str[0]=0x0;

	element=element_find(id);
	if (element!=NULL) strcpy(str,element->value_str);

	pthread_mutex_unlock(&element_thread_lock);
}

void element_set_value(int id,int value)
{
	element_type	*element;
	
	pthread_mutex_lock(&element_thread_lock);

	element=element_find(id);
	if (element!=NULL) element->value=value;

	pthread_mutex_unlock(&element_thread_lock);
}

void element_set_slider_value(int id,float value)
{
	element_type	*element;
	
	pthread_mutex_lock(&element_thread_lock);

	element=element_find(id);
	if (element!=NULL) element->setup.slider.value=(value-element->setup.slider.min)/(element->setup.slider.max-element->setup.slider.min);

	pthread_mutex_unlock(&element_thread_lock);
}

void element_set_value_string(int id,char *str)
{
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);

	element=element_find(id);
	if (element!=NULL) strcpy(element->value_str,str);

	pthread_mutex_unlock(&element_thread_lock);
}

void element_set_table_data(int id,char *row_data)
{
	int				idx,sz;
	element_type	*element;
	
	idx=element_get_value(id);

	pthread_mutex_lock(&element_thread_lock);

	element=element_find(id);
	if (element!=NULL) {
		if (element->data!=NULL) free(element->data);

		if (row_data==NULL) {
			element->data=NULL;
		}
		else {
			sz=element_table_add_get_data_size(row_data);
			element->data=valloc(sz);
			memmove(element->data,row_data,sz);
		}
	}
	
	pthread_mutex_unlock(&element_thread_lock);
	
	element_set_value(id,idx);
}

void element_set_bitmap(int id,char *path)
{
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);

	element=element_find(id);
	if (element!=NULL) {
		if (element->setup.button.bitmap.gl_id!=-1) bitmap_close(&element->setup.button.bitmap);
		
		if (path==NULL) {
			element->setup.button.bitmap.gl_id=-1;
		}
		else {
			bitmap_open(&element->setup.button.bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);
		}
	}

	pthread_mutex_unlock(&element_thread_lock);
}

int element_get_scroll_position(int id)
{
	int				rtn;
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);

	element=element_find(id);
	if (element==NULL) {
		rtn=0;
	}
	else {
		rtn=element->offset;
	}

	pthread_mutex_unlock(&element_thread_lock);

	return(rtn);
}

void element_set_scroll_position(int id,int pos)
{
	element_type	*element;

	pthread_mutex_lock(&element_thread_lock);

	element=element_find(id);
	if (element!=NULL) element->offset=pos;

	pthread_mutex_unlock(&element_thread_lock);
}

int element_get_x_position(int id)
{
	int				x;
	element_type	*element;

	x=0;

	pthread_mutex_lock(&element_thread_lock);

	element=element_find(id);
	if (element!=NULL) x=element->x;

	pthread_mutex_unlock(&element_thread_lock);

	return(x);
}
