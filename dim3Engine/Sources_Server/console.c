/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Console Routines

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

int								console_mode,console_count;
console_line_type				console_line[max_console_line];

extern setup_type				setup;
extern server_type				server;

/* =======================================================

      Clear  Console
      
======================================================= */

void console_initialize(void)
{
	console_count=0;
}

/* =======================================================

      Console Lines
      
======================================================= */

void console_remove_line(void)
{
	if (console_count==0) return;
	
	memmove(&console_line[0],&console_line[1],(sizeof(console_line_type)*(max_console_line-1)));
	console_line[max_console_line-1].txt[0]=0x0;
	
	console_count--;
}

void console_add_line(char *txt,d3col *col)
{
	char				c_str[256];
	char				*c,*c2;
	bool				first_line;
	console_line_type	*cline;

	c=txt;
	first_line=TRUE;

	while (TRUE) {

			// get line if more than one

		strncpy(c_str,c,256);
		c_str[255]=0x0;

		c2=strchr(c_str,'\n');
		if (c2!=NULL) *c2=0x0;
		
		if (c_str[0]==0x0) {
			c=strchr(c,'\n');
			if (c==NULL) break;

			c++;
			continue;
		}
	
			// need to remove line?

		if (console_count==max_console_line) console_remove_line();
		
			// add to console

		cline=&console_line[console_count];
		
		memmove(&cline->color,col,sizeof(d3col));
		
		if (first_line) {
			strncpy(cline->txt,c_str,max_console_txt_sz);
		}
		else {
			cline->txt[0]=' ';
			strncpy(&cline->txt[1],c_str,(max_console_txt_sz-1));
		}
		
		if (strlen(c_str)>=max_console_txt_sz) {
			cline->txt[max_console_txt_sz-3]=cline->txt[max_console_txt_sz-2]='.';
			cline->txt[max_console_txt_sz-1]=0x0;
		}

			// if debug on, output

		if (setup.debug_console) {
			if (!first_line) fprintf(stdout," ");
			fprintf(stdout,c_str);
			fprintf(stdout,"\n");
		}

			// next line
		
		console_count++;
		first_line=FALSE;

		c=strchr(c,'\n');
		if (c==NULL) break;

		c++;
	}
}

void console_add(char *txt)
{
	d3col			col;
	
	col.r=col.g=0;
	col.b=1;
	console_add_line(txt,&col);
}

void console_add_system(char *txt)
{
	d3col			col;
	
	col.r=col.b=0;
	col.g=1;
	console_add_line(txt,&col);
}

void console_add_error(char *txt)
{
	d3col			col;
	
	col.g=col.b=0;
	col.r=1;
	console_add_line(txt,&col);
}

