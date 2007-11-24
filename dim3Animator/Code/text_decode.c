/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Import Text Decoder

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

int				text_nline,text_data_sz;
char			piece_break=0x9;
char			*text_data;
int				*text_line_starts;

/* =======================================================

      Find Text File Line Starts
      
======================================================= */

int textdecode_count_linestarts(void)
{
	int			i,k,offset;
	char		*c;
	
	k=offset=0;
	c=text_data;
	
	for (i=0;i!=text_data_sz;i++) {
		if ((*c==0xD) || (*c==0xA)) {
			if ((i-offset)>0) k++;
			offset=i+1;
		}
		c++;
	}
	
	return(k);
}

void textdecode_linestarts(void)
{
	int			i,k,offset;
	char		*c;
	
	k=offset=0;
	c=text_data;
	
	for (i=0;i!=text_data_sz;i++) {
		if ((*c==0xD) || (*c==0xA)) {
			*c=0x0;
			if ((i-offset)>0) text_line_starts[k++]=offset;
			offset=i+1;
		}
		c++;
	}
	
	text_line_starts[k]=offset;
}

/* =======================================================

      Open/Close Text File
      
======================================================= */

bool textdecode_open(char *path,char p_piece_break)
{
	FILE			*file;
	struct stat		sb;
	
		// get file size
		
	if (stat(path,&sb)!=0) return(FALSE);
	text_data_sz=sb.st_size;
	
		// open file
		
	file=fopen(path,"r");
	if (file==NULL) return(FALSE);
   
	text_data=valloc(text_data_sz+1);
	if (text_data==NULL) {
		fclose(file);
		return(FALSE);
	}
	
	fread(text_data,1,text_data_sz,file);
	fclose(file);
	
	text_data[text_data_sz]=0x0;
	
		// get line start data
		
	text_nline=textdecode_count_linestarts();
	
	text_line_starts=(int*)valloc((text_nline+1)*sizeof(int));
	if (text_line_starts==NULL) {
		free(text_data);
		return(FALSE);
	}
	
		// decode lines
		
	textdecode_linestarts();
   
    piece_break=p_piece_break;
	
	return(TRUE);
}

void textdecode_close(void)
{
	free(text_line_starts);
	free(text_data);
}

/* =======================================================

      Find a Line
      
======================================================= */

int textdecode_count(void)
{
	return(text_nline);
}

int textdecode_find(int str_line,char *txt)
{
	int			i,sz;
	char		*c;
	
	sz=strlen(txt);
	
	for (i=str_line;i!=text_nline;i++) {
		c=text_data+text_line_starts[i];
		if (strncasecmp(c,txt,sz)==0) return(i);
	}
	
	return(-1);
}

/* =======================================================

      Get Line Data
      
======================================================= */

void textdecode_get_line(int i,char *txt,int len)
{
	strncpy(txt,(text_data+text_line_starts[i]),len);
	txt[len-1]=0x0;
}

void textdecode_get_piece(int i,int k,char *txt)
{
	int			t;
	char		line[1024];
	char		*c;
	
	textdecode_get_line(i,line,1024);
	
	txt[0]=0x0;
	
		// find start of token
		
	c=line;
	
	for (t=0;t!=k;t++) {
		c=strchr(c,piece_break);
		if (c==NULL) return;
		c++;
		
		while (*c==piece_break) c++;
	}
	
		// find end of token
		
	strcpy(txt,c);
		
	c=strchr(txt,piece_break);
	if (c!=NULL) *c=0x0;
}

