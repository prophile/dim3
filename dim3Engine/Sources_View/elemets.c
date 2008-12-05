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
				for (k=0;k!=element_table_max_image;k++) {
					if (element->setup.table.images[k].path[0]!=0x0) bitmap_close(&element->setup.table.images[k].bitmap);
				}
				break;
		
		}
		
		element++;
	}
}

void element_initialize(void)
{
	element_reset_controls();

	pthread_mutex_init(&element_thread_lock,NULL);		// used to make elements thread safe for certain UIs
}

void element_shutdown(void)
{
	element_release_control_memory();

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
	return((int)(((float)gl_text_get_char_height(hud.font.text_size_small))*element_control_high_factor));
}

inline int element_get_separator_high(void)
{
	return((int)(((float)gl_text_get_char_height(hud.font.text_size_small))*element_control_separator_factor));
}

inline int element_get_padding(void)
{
	return((int)(((float)hud.scale_x)*element_control_padding_factor));
}

/* =======================================================

      Add Elements
      
======================================================= */

void element_button_text_add(char *name,int id,int x,int y,int wid,int high,int x_pos,int y_pos)
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

	element->setup.button.text_only=TRUE;
	strcpy(element->setup.button.name,name);
	
	element->wid=wid;
	element->high=high;
	
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

void element_button_bitmap_add(char *path,char *path2,int id,int x,int y,int wid,int high,int x_pos,int y_pos)
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

	element->setup.button.text_only=FALSE;
	
		// skip button if graphic is missing to avoid crash

	if (!bitmap_open(&element->setup.button.bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE)) {
		nelement--;
		pthread_mutex_unlock(&element_thread_lock);
		return;
	}

		// if no selected button, then just use the regular one

	if (!bitmap_open(&element->setup.button.bitmap_select,path2,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE)) {
		bitmap_open(&element->setup.button.bitmap_select,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);
	}

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

void element_text_add(char *str,int id,int x,int y,int size,int just,bool selectable,bool alert)
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
	
	element->setup.text.size=size;
	element->setup.text.just=just;
	element->setup.text.alert=alert;

	element->selectable=selectable;
	element->enabled=TRUE;
	element->hidden=FALSE;
	
	strcpy(element->str,str);

		// need to calculate size through returns

	high=gl_text_get_char_height(size);

	element->wid=0;
	element->high=high;

	c=element->str;
	
	while (*c!=0x0) {
		c2=strstr(c,"{r}");
		if (c2!=NULL) *c2=0x0;
		
		wid=gl_text_get_string_width(size,c);
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
	int				n,sz;
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
		element->data=malloc(sz);
		memmove(element->data,row_data,sz);
	}

	element->setup.table.bitmap_mode=bitmap_mode;
	
	for (n=0;n!=element_table_max_image;n++) {
		element->setup.table.images[n].path[0]=0x0;
	}
	
	element->setup.table.next_image_idx=0;
	
	pthread_mutex_unlock(&element_thread_lock);
}

void element_tab_add(char *tab_list,int value,int id,int ntab,int x,int y,int wid,int high,int list_wid,int ext_high)
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
		strcpy(element->setup.tab.name[n],(char*)&tab_list[n*name_str_len]);
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

void element_draw_button_text(element_type *element,int sel_id)
{
	int				lft,rgt,top,bot,slant_add;
	float			alpha;
	d3col			txt_col,outline_col;
	
	if (element->enabled) {
		alpha=1.0f;
		if (element->id==sel_id) {
			memmove(&outline_col,&hud.color.mouse_over,sizeof(d3col));
			memmove(&txt_col,&hud.color.mouse_over,sizeof(d3col));
		}
		else {
			outline_col.r=outline_col.g=outline_col.b=0.2f;
			memmove(&txt_col,&hud.color.gradient_text,sizeof(d3col));
		}
	}
	else {
		alpha=0.3f;
		outline_col.r=outline_col.g=outline_col.b=0.0f;
		memmove(&txt_col,&hud.color.gradient_text,sizeof(d3col));
	}
	
	element_get_box(element,&lft,&rgt,&top,&bot);
	slant_add=(int)(((float)hud.scale_x)*0.008f);
	
		// button background

	view_draw_next_vertex_object_2D_color_quad(lft,bot,&hud.color.gradient_end,lft,top,&hud.color.gradient_end,(lft+slant_add),(top+slant_add),&hud.color.gradient_start,(lft+slant_add),(bot-slant_add),&hud.color.gradient_start,alpha);
	view_draw_next_vertex_object_2D_color_quad(rgt,bot,&hud.color.gradient_end,rgt,top,&hud.color.gradient_end,(rgt-slant_add),(top+slant_add),&hud.color.gradient_start,(rgt-slant_add),(bot-slant_add),&hud.color.gradient_start,alpha);
	view_draw_next_vertex_object_2D_color_quad(lft,top,&hud.color.gradient_end,rgt,top,&hud.color.gradient_end,(rgt-slant_add),(top+slant_add),&hud.color.gradient_start,(lft+slant_add),(top+slant_add),&hud.color.gradient_start,alpha);
	view_draw_next_vertex_object_2D_color_quad(lft,bot,&hud.color.gradient_end,rgt,bot,&hud.color.gradient_end,(rgt-slant_add),(bot-slant_add),&hud.color.gradient_start,(lft+slant_add),(bot-slant_add),&hud.color.gradient_start,alpha);
	view_draw_next_vertex_object_2D_color_quad((lft+slant_add),(top+slant_add),&hud.color.gradient_start,(rgt-slant_add),(top+slant_add),&hud.color.gradient_start,(rgt-slant_add),(bot-slant_add),&hud.color.gradient_start,(lft+slant_add),(bot-slant_add),&hud.color.gradient_start,alpha);

		// button outline

	glColor4f(0.0f,0.0f,0.0f,alpha);
	view_draw_next_vertex_object_2D_line_quad(lft,rgt,top,bot);
	
		// button text

	gl_text_start(hud.font.text_size_medium);
	gl_text_draw(((lft+rgt)>>1),((top+bot)>>1),element->setup.button.name,tx_center,TRUE,&txt_col,1.0f);
	gl_text_end();
}

void element_draw_button_bitmap(element_type *element,int sel_id)
{
	int				lft,rgt,top,bot;
	
	gl_texture_simple_start();

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

	view_draw_next_vertex_object_2D_texture_quad(lft,rgt,top,bot);

	gl_texture_simple_end();
}

void element_draw_button(element_type *element,int sel_id)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

	if (element->setup.button.text_only) {
		element_draw_button_text(element,sel_id);
	}
	else {
		element_draw_button_bitmap(element,sel_id);
	}

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
		
		view_draw_next_vertex_object_2D_texture_quad(lft,rgt,top,bot);
		
		gl_texture_simple_end();

		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
	}
	
		// the frame
		
	if (element->framed) {
		glColor4f(hud.color.base.r,hud.color.base.g,hud.color.base.b,1);
		view_draw_next_vertex_object_2D_line_quad(lft,rgt,top,bot);
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
	high=gl_text_get_char_height(element->setup.text.size);

		// draw text

	gl_text_start(element->setup.text.size);

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
	
	high=gl_text_get_char_height(hud.font.text_size_small);
		
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
		
	gl_text_start(hud.font.text_size_small);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.base,1.0f);
	gl_text_draw(x,(ky-1),":",tx_center,TRUE,&hud.color.base,1.0f);
	gl_text_end();
		
		// control box
		
	lft=x+10;
	rgt=lft+element->wid;
	top=ky-(element->high>>1);
	bot=top+element->high;
	
	alpha=(element->enabled?1.0f:0.3f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

		// outline

	glColor4f(0.0f,0.0f,0.0f,alpha);
	view_draw_next_vertex_object_2D_line_quad(lft,rgt,top,bot);

	if ((element->id==sel_id) && (element->enabled)) {
		glColor4f(hud.color.mouse_over.r,hud.color.mouse_over.g,hud.color.mouse_over.b,alpha);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,alpha);
	}

	view_draw_next_vertex_object_2D_line_quad((lft+1),(rgt-1),(top+1),(bot-1));

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

		// control text

	strcpy(txt,element->value_str);
	if (element_open_text_field_id==element->id) strcat(txt,"_");

	gl_text_start(hud.font.text_size_small);
		
	if (element->enabled) {
		if (element->id==element_open_text_field_id) {
			gl_text_draw((x+15),(ky-1),txt,tx_left,TRUE,&hud.color.mouse_over,1.0f);
		}
		else {
			gl_text_draw((x+15),(ky-1),txt,tx_left,TRUE,&hud.color.base,1.0f);
		}
	}
	else {
		gl_text_draw((x+15),(ky-1),txt,tx_left,TRUE,&hud.color.disabled,1.0f);
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

	gl_text_start(hud.font.text_size_small);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.base,1.0f);
	gl_text_draw(x,(ky-1),":",tx_center,TRUE,&hud.color.base,1.0f);
	gl_text_end();
	
		// checkbox
		
	lft=x+10;
	rgt=lft+element->high;
	top=ky-(element->high>>1);
	bot=top+element->high;

	alpha=(element->enabled?1.0f:0.3f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

		// background

	view_draw_next_vertex_object_2D_color_quad(lft,top,&hud.color.gradient_start,rgt,top,&hud.color.gradient_start,rgt,bot,&hud.color.gradient_end,lft,bot,&hud.color.gradient_end,alpha);

		// check

	if (element->value!=0) {
		
		glLineWidth(3.0f);
		glColor4f(hud.color.hilite.r,hud.color.hilite.g,hud.color.hilite.b,alpha);

		glBegin(GL_LINES);
		glVertex2i((lft+3),(top+3));
		glVertex2i((rgt-3),(bot-3));
		glVertex2i((rgt-3),(top+3));
		glVertex2i((lft+3),(bot-3));
		glEnd();

		glLineWidth(1.0f);
	}

		// outline

	glColor4f(0.0f,0.0f,0.0f,alpha);
	view_draw_next_vertex_object_2D_line_quad(lft,rgt,top,bot);

	if ((element->id==sel_id) && (element->enabled)) {
		glColor4f(hud.color.mouse_over.r,hud.color.mouse_over.g,hud.color.mouse_over.b,alpha);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,alpha);
	}

	view_draw_next_vertex_object_2D_line_quad((lft+1),(rgt-1),(top+1),(bot-1));

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
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
		
	*lft=element->x+10;
	*rgt=(*lft+element->wid)-element->high;
	*top=element->y-element->high;
	*bot=*top+(cnt*element->high);
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
	int				x,y,sz,kx,ky,lft,rgt,top,bot;
	char			str[256];
	float			alpha;
	
	x=element->x;
	y=element->y;
	
		// label
	
	ky=y-(element->high>>1);

	gl_text_start(hud.font.text_size_small);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.base,1.0f);
	gl_text_draw(x,(ky-1),":",tx_center,TRUE,&hud.color.base,1.0f);
	gl_text_end();
		
		// combo box
		
	lft=x+10;
	rgt=lft+element->wid;
	top=ky-(element->high>>1);
	bot=top+element->high;

	alpha=(element->enabled?1.0f:0.3f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

		// background

	view_draw_next_vertex_object_2D_color_quad(lft,top,&hud.color.gradient_start,rgt,top,&hud.color.gradient_start,rgt,bot,&hud.color.gradient_end,lft,bot,&hud.color.gradient_end,alpha);

		// outline

	sz=(bot-top)-8;
	kx=(rgt-8)-sz;

	glColor4f(0.0f,0.0f,0.0f,alpha);
	view_draw_next_vertex_object_2D_line_quad(lft,rgt,top,bot);

	glBegin(GL_LINES);
	glVertex2i(kx,top);
	glVertex2i(kx,bot);
	glEnd();

	glColor4f(1.0f,1.0f,1.0f,alpha);
	view_draw_next_vertex_object_2D_line_quad(kx,(rgt-1),(top+1),(bot-1));

	if ((element->id==sel_id) && (element->enabled)) {
		glColor4f(hud.color.mouse_over.r,hud.color.mouse_over.g,hud.color.mouse_over.b,alpha);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,alpha);
	}

	view_draw_next_vertex_object_2D_line_quad((lft+1),(kx-1),(top+1),(bot-1));

		// arrow

	if ((element->id==element_open_combo_id) || ((element->id==sel_id) && (element->enabled))) {
		glColor4f(hud.color.mouse_over.r,hud.color.mouse_over.g,hud.color.mouse_over.b,alpha);
	}
	else {
		glColor4f(hud.color.hilite.r,hud.color.hilite.g,hud.color.hilite.b,alpha);
	}

	glBegin(GL_TRIANGLES);
	glVertex2i(((rgt-4)-sz),(top+4));
	glVertex2i((rgt-4),(top+4));
	glVertex2i(((rgt-4)-(sz/2)),(bot-4));
	glEnd();

	glColor4f(0.0f,0.0f,0.0f,alpha);

	glBegin(GL_LINE_LOOP);
	glVertex2i(((rgt-4)-sz),(top+4));
	glVertex2i((rgt-4),(top+4));
	glVertex2i(((rgt-4)-(sz/2)),(bot-4));
	glEnd();

		// control text

	strcpy(str,(element->data+(element->value*32)));

	gl_text_start(hud.font.text_size_small);
	gl_text_draw((x+15),(ky-1),str,tx_left,TRUE,&hud.color.gradient_text,alpha);
	gl_text_end();
}

void element_draw_combo_open(element_type *element)
{
	int				x,y,n,cnt,lft,rgt,top,bot,sel_item_idx;
	char			str[256];
	
		// combo count

	cnt=element_get_combo_list_count(element);

		// get selected item

	input_gui_get_mouse_position(&x,&y);

	sel_item_idx=-1;

	element_box_combo_open(element,&lft,&rgt,&top,&bot);
	if ((x>=lft) && (x<=rgt) && (y>=top) && (y<=bot)) {
		sel_item_idx=(y-top)/element->high;
	}

		// drawing sizes

	x=element->x;
	y=element->y;
	
	lft=x+10;
	rgt=(lft+element->wid)-element->high;
	top=y-element->high;

		// draw items

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

		// background

	bot=top+(cnt*element->high);

	view_draw_next_vertex_object_2D_color_quad(lft,top,&hud.color.gradient_start,rgt,top,&hud.color.gradient_start,rgt,bot,&hud.color.gradient_end,lft,bot,&hud.color.gradient_end,1.0f);

		// combo items

	for (n=0;n!=cnt;n++) {

		bot=top+element->high;

			// selection

		if (element->value==n) {
			view_draw_next_vertex_object_2D_color_quad(lft,top,&hud.color.hilite,rgt,top,&hud.color.hilite,rgt,bot,&hud.color.hilite,lft,bot,&hud.color.hilite,1.0f);
		}

			// text

		gl_text_start(hud.font.text_size_small);
		strcpy(str,(element->data+(n*32)));

		if (sel_item_idx==n) {
			gl_text_draw((x+10),((top+bot)>>1),str,tx_left,TRUE,&hud.color.mouse_over,1.0f);
		}
		else {
			gl_text_draw((x+10),((top+bot)>>1),str,tx_left,TRUE,&hud.color.gradient_text,1.0f);
		}

		gl_text_end();

		top+=element->high;
	}

		// outline

	top=y-element->high;
	bot=top+(cnt*element->high);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	view_draw_next_vertex_object_2D_line_quad(lft,rgt,top,bot);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

/* =======================================================

      Slider Element
      
======================================================= */

void element_click_slider(element_type *element,int x,int y)
{
	x-=(element->x+10);
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

	gl_text_start(hud.font.text_size_small);
	gl_text_draw((x-5),ky,element->str,tx_right,TRUE,&hud.color.base,1.0f);
	gl_text_draw(x,(ky-1),":",tx_center,TRUE,&hud.color.base,1.0f);
	gl_text_end();
	
		// slider size
		
	lft=x+10;
	rgt=lft+element->wid;
	top=ky-(element->high>>1);
	bot=top+element->high;
	
	mid=lft+(int)(((float)element->wid)*element->setup.slider.value);

	alpha=(element->enabled?1.0f:0.3f);

		// draw slider

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);
	
		// slider value
		
	if (element->enabled) {

		glBegin(GL_QUADS);

		glColor4f(hud.color.hilite.r,hud.color.hilite.g,hud.color.hilite.b,alpha);
		glVertex2i((lft+1),(top+1));
		glVertex2i(mid,(top+1));

		glColor4f((hud.color.hilite.r*0.7f),(hud.color.hilite.g*0.7f),(hud.color.hilite.b*0.7f),alpha);
		glVertex2i(mid,(bot-1));
		glVertex2i((lft+1),(bot-1));

		glEnd();
	}

		// outline

	glColor4f(0.0f,0.0f,0.0f,alpha);
	view_draw_next_vertex_object_2D_line_quad(lft,rgt,top,bot);

	if ((element->id==sel_id) && (element->enabled)) {
		glColor4f(hud.color.mouse_over.r,hud.color.mouse_over.g,hud.color.mouse_over.b,alpha);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,alpha);
	}

	view_draw_next_vertex_object_2D_line_quad((lft+1),(rgt-1),(top+1),(bot-1));

		// slider drag

	if ((mid+16)>rgt) mid=rgt-16;

	glBegin(GL_QUADS);

	glColor4f(0.6f,0.6f,0.6f,alpha);
	glVertex2i(mid,top);
	glVertex2i((mid+16),top);

	glColor4f(0.3f,0.3f,0.3f,alpha);
	glVertex2i((mid+16),bot);
	glVertex2i(mid,bot);

	glEnd();

	glColor4f(1.0f,1.0f,1.0f,alpha);
	view_draw_next_vertex_object_2D_line_quad(mid,(mid+16),top,bot);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
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
	if (element->setup.table.bitmap_mode==element_table_bitmap_none) return(gl_text_get_char_height(hud.font.text_size_small)+2);

	return(element_table_bitmap_size+2);
}

bool element_click_table(element_type *element,int x,int y)
{
	int				high,row_high,row_cnt,cnt;
	bool			up_ok,down_ok;
	
		// get text sizes
		
	high=gl_text_get_char_height(hud.font.text_size_small)+2;
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
		gl_text_start(hud.font.text_size_small);
		gl_text_draw((x+4),y,element->setup.table.cols[n].name,tx_left,TRUE,&col,1.0f);
		gl_text_end();
		
		x+=(int)(element->setup.table.cols[n].percent_size*(float)wid);
	}
}

unsigned long element_draw_table_get_image_gl_id(element_type *element,int row_idx)
{
	int				n,idx;
	char			*c,*c2,subdir[256],filename[256],path[1024];
	
		// get path
		
	c=element->data+(row_idx*128);
	
	strcpy(subdir,c);
	c2=strchr(subdir,';');
	if (c2==NULL) return(-1);

	*c2=0x0;
	strcpy(filename,(c2+1));

	c2=strchr(filename,';');
	if (c2==NULL) return(-1);

	*c2=0x0;

	if (element->setup.table.bitmap_mode==element_table_bitmap_data) {
		file_paths_data(&setup.file_path_setup,path,subdir,filename,"png");
	}
	else {
		file_paths_documents(&setup.file_path_setup,path,subdir,filename,"png");
	}
	
		// is it already loaded?
		
	for (n=0;n!=element_table_max_image;n++) {
		if (strcmp(element->setup.table.images[n].path,path)==0) return(element->setup.table.images[n].bitmap.gl_id);
	}
	
		// need to load, find open bitmap
		
	idx=-1;
	
	for (n=0;n!=element_table_max_image;n++) {
		if (element->setup.table.images[n].path[0]==0x0) {
			idx=n;
			break;
		}
	}
	
		// no spots left, replace a image
		
	if (idx==-1) {
		idx=element->setup.table.next_image_idx;
		element->setup.table.next_image_idx++;
		if (element->setup.table.next_image_idx==element_table_max_image) element->setup.table.next_image_idx=0;
	}
	
		// open
		
	strcpy(element->setup.table.images[idx].path,path);
	bitmap_open(&element->setup.table.images[idx].bitmap,path,anisotropic_mode_none,mipmap_mode_none,FALSE,FALSE,FALSE);

	return(element->setup.table.images[idx].bitmap.gl_id);
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

			gl_id=element_draw_table_get_image_gl_id(element,row);

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
				view_draw_next_vertex_object_2D_line_quad(dx,(dx+element_table_bitmap_size),(y+1),((y+1)+element_table_bitmap_size));
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
				view_draw_next_vertex_object_2D_line_quad((dx+2),((dx-4)+element_table_bitmap_size),(y+4),((y-2)+element_table_bitmap_size));
				
				glLineWidth(1.0f);

				col.r=col.g=col.b=1.0f;

				gl_text_start(hud.font.text_size_large);
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
			
		gl_text_start(hud.font.text_size_small);
		gl_text_draw(dx,dy,txt,tx_left,TRUE,txt_col,1.0f);
		gl_text_end();
		
			// get next data
			
		c=strchr(c,'\t');
		if (c==NULL) break;
		c++;
		
		x+=(int)(element->setup.table.cols[n].percent_size*(float)wid);
	}
}

void element_draw_table(element_type *element,int sel_id)
{
	int				n,x,y,wid,high,cnt,lft,rgt,top,bot,mx,my,row_high;
	char			*c;
	bool			up_ok,down_ok;
	d3col			line_col;

		// sizes
	
	wid=element->wid-30;
	high=gl_text_get_char_height(hud.font.text_size_small)+2;
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
	view_draw_next_vertex_object_2D_line_quad(lft,rgt,top,bot);
	
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
	
		// scrolling arrows
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glDisable(GL_DEPTH_TEST);

		// scroll up

	element_get_box(element,&lft,&rgt,&top,&bot);
	lft=rgt-20;
	rgt=lft+16;
	top+=(high+8);
	bot=top+16;

	glColor4f(hud.color.hilite.r,hud.color.hilite.g,hud.color.hilite.b,(up_ok?1.0f:0.1f));

	glBegin(GL_TRIANGLES);
	glVertex2i(lft,bot);
	glVertex2i(rgt,bot);
	glVertex2i(((lft+rgt)>>1),top);
	glEnd();

	glColor4f(0.0f,0.0f,0.0f,(up_ok?1.0f:0.3f));

	glBegin(GL_LINE_LOOP);
	glVertex2i(lft,bot);
	glVertex2i(rgt,bot);
	glVertex2i(((lft+rgt)>>1),top);
	glEnd();

		// scroll down
		
	element_get_box(element,&lft,&rgt,&top,&bot);
	lft=rgt-20;
	rgt=lft+16;
	bot-=4;
	top=bot-16;

	glColor4f(hud.color.hilite.r,hud.color.hilite.g,hud.color.hilite.b,(down_ok?1.0f:0.1f));
	
	glBegin(GL_TRIANGLES);
	glVertex2i(lft,top);
	glVertex2i(rgt,top);
	glVertex2i(((lft+rgt)>>1),bot);
	glEnd();

	glColor4f(0.0f,0.0f,0.0f,(up_ok?1.0f:0.3f));

	glBegin(GL_LINE_LOOP);
	glVertex2i(lft,top);
	glVertex2i(rgt,top);
	glVertex2i(((lft+rgt)>>1),bot);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

		// busy
		
	if (element->setup.table.busy) {
		element_get_box(element,&lft,&rgt,&top,&bot);
		
		rgt--;
		lft=rgt-23;
		top++;
		bot=top+29;

		mx=(lft+rgt)>>1;
		my=(top+bot)>>1;
		
		glBegin(GL_TRIANGLES);

		glColor4f((hud.color.hilite.r*0.5f),(hud.color.hilite.g*0.5f),(hud.color.hilite.b*0.5f),1.0f);
		
		switch ((time_get()>>7)&0x3) {
			case 0:
				glVertex2i(lft,top);
				glVertex2i(rgt,top);
				break;
			case 1:
				glVertex2i(rgt,top);
				glVertex2i(rgt,bot);
				break;
			case 2:
				glVertex2i(lft,bot);
				glVertex2i(rgt,bot);
				break;
			case 3:
				glVertex2i(lft,top);
				glVertex2i(lft,bot);
				break;
		}
		
		glColor4f((hud.color.hilite.r*0.8f),(hud.color.hilite.g*0.8f),(hud.color.hilite.b*0.8f),1.0f);
		glVertex2i(mx,my);

		glEnd();
	}
	
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
	int				lft,rgt,max_sz,xadd,idx;
	
		// within tab box?
		
	lft=element->x+(int)(((float)hud.scale_x)*0.02f);
	rgt=lft+element->setup.tab.list_wid;
	
	if ((x<lft) || (x>rgt)) return(-1);
	
		// select value
		
	xadd=element->setup.tab.list_wid/element->setup.tab.ntab;

	max_sz=(int)(((float)hud.scale_x)*0.2f);
	if (xadd>max_sz) xadd=max_sz;

	idx=(x-lft)/xadd;
	if ((idx<0) || (idx>=element->setup.tab.ntab)) return(-1);

	return(idx);
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
	int				n,ky,xstart,xadd,high,max_sz,slant_add,mouse_idx,
					lft,rgt,top,bot,lx,rx,ty,by,
					klft,krgt,ktop;
	float			col_base;
	d3col			txt_col;
	
	high=gl_text_get_char_height(hud.font.text_size_small);
		
		// sizes
	
	element_get_box(element,&lft,&rgt,&top,&bot);

	xadd=element->setup.tab.list_wid/element->setup.tab.ntab;

	max_sz=(int)(((float)hud.scale_x)*0.2f);
	if (xadd>max_sz) xadd=max_sz;

		// bitmap drawing
		
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
	view_draw_next_vertex_object_2D_color_quad(lx,(ky-3),&hud.color.gradient_start,rx,(ky-3),&hud.color.gradient_start,rx,ky,&hud.color.gradient_end,lx,ky,&hud.color.gradient_end,1.0f);
	view_draw_next_vertex_object_2D_color_quad(lx,ky,&hud.color.gradient_end,rx,ky,&hud.color.gradient_end,rx,(ky+3),&hud.color.gradient_start,lx,(ky+3),&hud.color.gradient_start,1.0f);

	ky=top+element->setup.tab.ext_high;
	view_draw_next_vertex_object_2D_color_quad(lx,(ky-3),&hud.color.gradient_start,rx,(ky-3),&hud.color.gradient_start,rx,ky,&hud.color.gradient_end,lx,ky,&hud.color.gradient_end,1.0f);
	view_draw_next_vertex_object_2D_color_quad(lx,ky,&hud.color.gradient_end,rx,ky,&hud.color.gradient_end,rx,(ky+3),&hud.color.gradient_start,lx,(ky+3),&hud.color.gradient_start,1.0f);

	glColor4f(0.0f,0.0f,0.0f,1.0f);

	glBegin(GL_LINES);

	ky=bot;

	glVertex2i(lx,(ky-4));
	glVertex2i(rx,(ky-4));
	glVertex2i(lx,(ky+4));
	glVertex2i(rx,(ky+4));

	ky=top+element->setup.tab.ext_high;

	glVertex2i(lx,(ky-4));
	glVertex2i(rx,(ky-4));
	glVertex2i(lx,(ky+4));
	glVertex2i(rx,(ky+4));

	glEnd();
	
		// background
		
	glBegin(GL_QUADS);
	
	glColor4f(hud.color.gradient_start.r,hud.color.gradient_start.g,hud.color.gradient_start.b,0.2f);
	glVertex2i(lx,(bot+4));
	glVertex2i(rx,(bot+4));
	
	ky=top+(element->setup.tab.ext_high>>1);

	glColor4f(hud.color.gradient_end.r,hud.color.gradient_end.g,hud.color.gradient_end.b,0.2f);
	glVertex2i(rx,ky);
	glVertex2i(lx,ky);
	
	glVertex2i(lx,ky);
	glVertex2i(rx,ky);

	ky=top+element->setup.tab.ext_high;

	glColor4f(hud.color.gradient_start.r,hud.color.gradient_start.g,hud.color.gradient_start.b,0.2f);
	glVertex2i(rx,(ky-4));
	glVertex2i(lx,(ky-4));
	
	glEnd();
	
		// get mouse over element
	
	mouse_idx=-1;
	
	if (element->id==sel_id) {
		mouse_idx=element_mouse_over_tab(element,x);
	}

		// tabs
		
	xstart=lft+(int)(((float)hud.scale_x)*0.02f);
	slant_add=(int)(((float)hud.scale_x)*0.01f);

	for (n=0;n!=element->setup.tab.ntab;n++) {

		lx=xstart+(xadd*n);
		rx=lx+xadd;
		
		if (element->value!=n) {
			ty=top+(int)(((float)slant_add)*1.5f);
			by=bot-4;
			col_base=0.25f;
		}
		else {
			ty=top;
			by=bot+3;
			col_base=0.35f;
		}
		
		memmove(&txt_col,&hud.color.gradient_text,sizeof(d3col));
		
		if (element->value==n) {
			memmove(&txt_col,&hud.color.hilite,sizeof(d3col));
		}
		
		if (mouse_idx==n) {
			memmove(&txt_col,&hud.color.mouse_over,sizeof(d3col));
		}

		glBegin(GL_QUADS);

		glColor4f(hud.color.gradient_start.r,hud.color.gradient_start.g,hud.color.gradient_start.b,1.0f);
		glVertex2i((lx+slant_add),ty);
		glVertex2i((rx-slant_add),ty);
		glColor4f(hud.color.gradient_end.r,hud.color.gradient_end.g,hud.color.gradient_end.b,1.0f);
		glVertex2i(rx,(ty+slant_add));
		glVertex2i(lx,(ty+slant_add));

		glVertex2i(lx,(ty+slant_add));
		glVertex2i(rx,(ty+slant_add));
		glColor4f(hud.color.gradient_start.r,hud.color.gradient_start.g,hud.color.gradient_start.b,1.0f);
		glVertex2i(rx,(ty+(slant_add<<1)));
		glVertex2i(lx,(ty+(slant_add<<1)));

		glVertex2i(lx,(ty+(slant_add<<1)));
		glVertex2i(rx,(ty+(slant_add<<1)));
		glVertex2i(rx,by);
		glVertex2i(lx,by);
		
		glEnd();
		
		glColor4f(0.0f,0.0f,0.0f,1.0f);

		glBegin(GL_LINE_LOOP);
		glVertex2i(lx,by);
		glVertex2i(lx,(ty+slant_add));
		glVertex2i((lx+slant_add),ty);
		glVertex2i((rx-slant_add),ty);
		glVertex2i(rx,(ty+slant_add));
		glVertex2i(rx,by);
		glEnd();
		
		gl_text_start(hud.font.text_size_medium);
		gl_text_draw(((lx+rx)>>1),((ty+by)>>1),element->setup.tab.name[n],tx_center,TRUE,&txt_col,1.0f);
		gl_text_end();
	}

		// mouse over element
		
	if (mouse_idx!=-1) {
		lx=xstart+(xadd*mouse_idx);
		rx=lx+xadd;
		
		if (element->value!=mouse_idx) {
			ty=top+(int)(((float)slant_add)*1.5f);
			by=bot-4;
		}
		else {
			ty=top;
			by=bot+3;
		}

		glColor4f(hud.color.mouse_over.r,hud.color.mouse_over.g,hud.color.mouse_over.b,1.0f);

		glBegin(GL_LINE_LOOP);
		glVertex2i(lx,by);
		glVertex2i(lx,(ty+slant_add));
		glVertex2i((lx+slant_add),ty);
		glVertex2i((rx-slant_add),ty);
		glVertex2i(rx,(ty+slant_add));
		glVertex2i(rx,by);
		glEnd();
	}
	
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
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
			if (!element_click_tab(element,x)) {
				element_click_down_id=-1;
				return(-1);
			}
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
		return(element->id);
	}

		// regular text

	if (len>=(element->max_value_str_sz-1)) return(-1);

	element->value_str[len]=ch;
	element->value_str[len+1]=0x0;

	return(element->id);
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
			element->data=malloc(sz);
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
