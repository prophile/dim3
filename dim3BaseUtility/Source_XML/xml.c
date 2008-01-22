/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: XML Parser

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

#ifdef D3_PCH
	#include "dim3baseutility.h"
#endif

int					xml_ntag,
                    xml_taglistsz,xml_filesz,xml_buffersz;
char				*xml_dataptr,*xml_taglistptr;

/* =======================================================

      Decode into Tags
            
======================================================= */

bool xml_decode(void)
{
	int					i,k,
						q,noff,spot,tag_offset,listsz,
						tagstack[max_tag_depth];
	char				c,*sptr,
						name[256],*oldptr,*newtaglistptr;
	bool				intag,inname,inquote,backslash,endtag,goodread;
	xmltagtype			*tags,*tag;
	
	tag_offset=0;

		// create initial set of tags

	xml_taglistsz=tag_atom_sz;
	listsz=tag_atom_sz*sizeof(xmltagtype);
	xml_taglistptr=(char*)valloc(listsz);
	if (xml_taglistptr==NULL) return(FALSE);

	tags=(xmltagtype*)xml_taglistptr;

		// grab all the tags

	xml_ntag=0;
	spot=0;

	intag=FALSE;
	inname=FALSE;
	inquote=FALSE;
	backslash=FALSE;
	endtag=FALSE;

	noff=0;
	goodread=TRUE;

	sptr=oldptr=(char*)xml_dataptr;

	for ((i=0);(i<xml_filesz);i++) {
		c=*sptr++;
		if (c==0x0) break;

		if (c=='\"') inquote=!inquote;
		if (inquote) continue;

		if (c=='/') {
			if (intag) {
				if ((inname) && (noff==0)) {
					endtag=TRUE;
				}
			}
			backslash=TRUE;
		}

		if (c=='<') {
			intag=TRUE;
			inname=TRUE;
			noff=0;
			backslash=FALSE;
			tag_offset=i;
			continue;
		}

		if (c=='>') {
			intag=FALSE;
			name[noff]=0x0;

				// end tag?

			if (endtag) {
				if (spot!=0) {
					k=tagstack[spot-1];
					tags[k].nchild=tagstack[spot-1];
					tags[k].end_offset=tag_offset;
					spot--;
				}
			}
			else {

					// add the tag

				tag=&tags[xml_ntag];
				
				strcpy(tag->name,name);
				if (spot==0) {
					tag->parent=-1;	// no parent for this item
				}
				else {
					tag->parent=tagstack[spot-1];
				}
				tag->tag_offset=tag_offset;
				tag->attrib_offset=tag_offset+((int)strlen(name)+2);
				tag->start_offset=i+1;

					// move up stack if this tag a parent

				if (!backslash) {			// not single tag, so move into children
					tagstack[spot]=xml_ntag;
					spot++;
				}
				else {
					tag->end_offset=i+1;
				}

				xml_ntag++;

				if (xml_ntag>=xml_taglistsz) {		// need to add more tags?
					listsz=(xml_taglistsz+tag_atom_sz)*sizeof(xmltagtype);
					
					newtaglistptr=(char*)valloc(listsz);
					if (newtaglistptr!=NULL) {
						memmove(newtaglistptr,xml_taglistptr,(xml_taglistsz*sizeof(xmltagtype)));
						free(xml_taglistptr);
						
						xml_taglistptr=newtaglistptr;
						xml_taglistsz=xml_taglistsz+tag_atom_sz;
					}
					
					tags=(xmltagtype*)xml_taglistptr;
					if (newtaglistptr==NULL) {
						goodread=FALSE;
						break;
					}
				}
			}

			endtag=FALSE;
			backslash=FALSE;
			continue;
		}

		if (inname) {
			if (c==' ') {
				inname=FALSE;
				name[noff]=0x0;
			}
			else {
				if (noff<63) {
					name[noff++]=c;
				}
			}
		}
	}

       // count the children

	tag=tags;

	for ((k=0);(k<xml_ntag);k++) {
		tag->nchild=0;
		tag->first_child=-1;
		tag->last_child=-1;
		tag++;
	}

	tag=tags;

	for ((k=0);(k<xml_ntag);k++) {
		q=tag->parent;
		if (q!=-1) {
			tags[q].nchild++;
			if (tags[q].first_child==-1) tags[q].first_child=k;
			if (tags[q].last_child<k) tags[q].last_child=k;
		}
		tag++;
	}

	return(goodread);
}

/* =======================================================

      XML Files
            
======================================================= */

bool xml_new_file(void)
{
	xml_filesz=0;
	xml_buffersz=buff_atom_sz;
	xml_taglistptr=NULL;
	xml_dataptr=(char*)valloc(xml_buffersz);
	return(xml_dataptr!=NULL);
}

bool xml_open_file(char *path)
{
	int				sz;
	FILE			*file;
	struct stat		sb;
	
		// get file size
		
	if (stat(path,&sb)!=0) return(FALSE);
	sz=sb.st_size;
	
		// open file
		
	file=fopen(path,"rb");
	if (file==NULL) return(FALSE);
    
	xml_dataptr=(char*)valloc(sz);
    if (xml_dataptr==NULL) {
        fclose(file);
        return(FALSE);
    }
	
	fread(xml_dataptr,1,sz,file);
	fclose(file);
    
	xml_filesz=sz;
	xml_buffersz=buff_atom_sz;
	xml_taglistptr=NULL;
    
	return(xml_decode());
}

bool xml_save_file(char *path)
{
	FILE				*file;
	
	file=fopen(path,"w");
	if (file==NULL) return(FALSE);
	
	fwrite(xml_dataptr,1,xml_filesz,file);
	fclose(file);
    
    return(TRUE);
}

void xml_close_file(void)
{
    free(xml_dataptr);
    if (xml_taglistptr!=NULL) free(xml_taglistptr);
}

char* xml_get_stream(int *sz)
{
    *sz=xml_filesz;
    return(xml_dataptr);
}

/* =======================================================

      Find Tags
            
======================================================= */

int xml_findfirstchild(char *name,int parent)
{
	int					n,start_tag,end_tag;
	xmltagtype			*tags,*tag;

	tags=(xmltagtype*)xml_taglistptr;

		// if no children, then just exit

	start_tag=0;
	end_tag=xml_ntag;

	if (parent!=xml_root_tag) {
		if ((tags[parent].nchild==0) || (tags[parent].first_child==-1) || (tags[parent].last_child==-1)) return(-1);
		start_tag=tags[parent].first_child;
		end_tag=tags[parent].last_child+1;
	}

		// search for children from parent

	tag=&tags[start_tag];

	for ((n=start_tag);(n<end_tag);n++) {
		if (tag->parent==parent) {
			if (strcmp(tag->name,name)==0) return(n);
		}
		tag++;
	}

	return(-1);
}

int xml_findrootchild(char *name)
{
	return(xml_findfirstchild(name,xml_root_tag));
}

int xml_findnextchild(int curtag)
{
	int					n,parent;
	xmltagtype			*tags,*tag;

	if (curtag>=xml_ntag) return(-1);

	tags=(xmltagtype*)xml_taglistptr;

	tag=&tags[curtag];
	parent=tag->parent;

	curtag++;
	tag++;

	for ((n=curtag);(n<xml_ntag);n++) {
		if (tag->parent==parent) return(n);
		tag++;
	}

	return(-1);
}

/* =======================================================

      Tag Parents and Children
            
======================================================= */

int xml_getparent(int n)
{
	int			k;
	xmltagtype	*tags;

	tags=(xmltagtype*)xml_taglistptr;
	k=tags[n].parent;

	return(k);	
}

int xml_countchildren(int n)
{
	int			k;
	xmltagtype	*tags;

	tags=(xmltagtype*)xml_taglistptr;
	k=tags[n].nchild;

	return(k);
}

/* =======================================================

      Flat Searches
            
======================================================= */

int xml_findflat(char *name,int offset)
{
	int			n;
	xmltagtype	*tag;

	tag=(xmltagtype*)xml_taglistptr;

	for (n=0;n<xml_ntag;n++) {
		if (strcmp(tag->name,name)==0) {
			offset--;
			if (offset<0) return(n);
		}
		tag++;
	}
	
	return(-1);
}

/* =======================================================

      Hex Translations
            
======================================================= */

int xml_hex_character_to_int(char ch)
{
	if ((ch>='0') && (ch<='9')) return(((int)ch)-'0');
	return((((int)ch)-'A')+10);
}
	
char xml_hex_int_to_character(int k)
{
	char		hex_code[16]="0123456789ABCDEF";

	return(hex_code[(k&0xF)]);
}

/* =======================================================

      Read Attributes
            
======================================================= */

bool xml_get_attribute_raw(int n,char *name,char *value,int valuesz)
{
	int				i,noff,voff,sz;
	char			c,*sptr,attribname[256];
	bool			inquote,inname;
	xmltagtype		*tag;

	value[0]=0x0;
	if ((n<0) || (n>xml_ntag)) return(FALSE);

	sptr=(char*)xml_dataptr;
	tag=(xmltagtype*)(xml_taglistptr+(n*sizeof(xmltagtype)));
	
	sptr=sptr+tag->attrib_offset;
	sz=(tag->start_offset-tag->attrib_offset);
	if (sz<=0) return(FALSE);

	inname=TRUE;
	inquote=FALSE;
	noff=voff=0;

	for ((i=0);(i<sz);i++) {
		c=*sptr++;
		if (c==0x0) break;

		if ((c=='"') || (c=='\'')) {
			inquote=!inquote;
			continue;
		}

		if (!inquote) {
			if ((c==' ') || (c=='/') || (c=='>')) {
				value[voff]=0x0;
				if (strcmp(name,attribname)==0) return(TRUE);
	
				inname=TRUE;
				noff=0;
				continue;
			}
	
			if (c=='=') {
				inname=FALSE;
				attribname[noff]=0x0;
				voff=0;
				continue;
			}
		}
		
		if (inname) {
			if (noff<254) {
				attribname[noff++]=c;
			}
		}
		else {
			if (inquote) {
				if (voff<(valuesz-2)) {
					value[voff++]=c;
				}
			}
		}
	}
	
	value[0]=0x0;
	return(FALSE);
}
	
void xml_parse_attribute(char *value)
{
	int		k,len;
	char	*str,*fstr;

		// parse out &#x00;

	str=value;
	
	while (TRUE) {
		str=strstr(str,"&#x");
		if (str==NULL) break;

		fstr=strchr((str+3),';');
		if (fstr==NULL) {
			str+=3;
			continue;
		}

		len=(int)(fstr-str)-3;
		if (len>0) {
			k=0;
			if (len>1) {
				k=(xml_hex_character_to_int(str[3])*16)+xml_hex_character_to_int(str[4]);
			}
			else {
				k=xml_hex_character_to_int(str[3]);
			}

			*str=(char)k;
			len=(int)strlen(fstr+1)+1;
			memmove((str+1),(fstr+1),len);
			str++;
		}
		else {
			str=fstr+1;
		}
	}
}

bool xml_get_attribute_text(int n,char *name,char *value,int valuesz)
{
	if (!xml_get_attribute_raw(n,name,value,valuesz)) return(FALSE);
	xml_parse_attribute(value);

	return(TRUE);
}

bool xml_get_attribute_text_default_blank(int n,char *name,char *value,int valuesz)
{
	if (!xml_get_attribute_raw(n,name,value,valuesz)) {
		value[0]=0x0;
	}
	else {
		xml_parse_attribute(value);
	}

	return(TRUE);
}

int xml_get_attribute_int(int n,char *name)
{
	char			str[256];

	if (!xml_get_attribute_raw(n,name,str,256)) return(0);
	return(atoi(str));
}

int xml_get_attribute_int_default(int n,char *name,int def_int)
{
	char			str[256];

	if (!xml_get_attribute_raw(n,name,str,256)) return(def_int);
	return(atoi(str));
}

int xml_get_attribute_short_array(int n,char *name,short *value,int count)
{
    int			nvalue;
	char		*c,*c2,str[5120];

	if (!xml_get_attribute_raw(n,name,str,5120)) return(0);
	if (str[0]==0x0) return(0);
    
    nvalue=0;
    c=str;
    
    while (TRUE) {
        c2=strchr(c,',');
        if (c2!=NULL) {
            *c2=0x0;
        }
        value[nvalue++]=(short)atoi(c);
        if (c2==NULL) break;
        c=c2+1;
		
		if (nvalue==count) break;
    }
    
	return(nvalue);
}

float xml_get_attribute_float(int n,char *name)
{
	char			str[256];

	if (!xml_get_attribute_raw(n,name,str,256)) return(0);
	return(strtof(str,NULL));
}

float xml_get_attribute_float_default(int n,char *name,float def_float)
{
	char			str[256];

	if (!xml_get_attribute_raw(n,name,str,256)) return(def_float);
	return(strtof(str,NULL));
}

int xml_get_attribute_float_array(int n,char *name,float *value,int count)
{
    int			nvalue;
	char		*c,*c2,str[5120];

	if (!xml_get_attribute_raw(n,name,str,5120)) return(0);
	if (str[0]==0x0) return(0);
    
    nvalue=0;
    c=str;
    
    while (TRUE) {
        c2=strchr(c,',');
        if (c2!=NULL) {
            *c2=0x0;
        }
        value[nvalue++]=strtof(c,NULL);
        if (c2==NULL) break;
        c=c2+1;
		
		if (nvalue==count) break;
    }
    
	return(nvalue);
}

bool xml_get_attribute_boolean(int n,char *name)
{
	char			str[256];

	if (!xml_get_attribute_raw(n,name,str,256)) return(FALSE);
	return(strcmp(str,"true")==0);
}

int xml_get_attribute_list(int n,char *name,char *list)
{
    int				i;
    char			*c,str[256];
    
	if (!xml_get_attribute_raw(n,name,str,256)) return(0);
    
    i=0;
    c=list;
    
    while (TRUE) {
        if (*c==0x0) break;
        if (strcmp(str,c)==0) return(i);
        c+=32;
        i++;
    }
    
    return(0);
}

bool xml_get_attribute_2_coord_int(int n,char *name,int *x,int *y)
{
	char		*c2,str[256];

	*x=*y=0;

	if (!xml_get_attribute_raw(n,name,str,256)) return(FALSE);
    
	c2=strchr(str,',');
	if (c2==NULL) return(FALSE);
	
	*c2=0x0;
	*x=atoi(str);
	
	*y=atoi(c2+1);
    
	return(TRUE);
}

bool xml_get_attribute_2_coord_float(int n,char *name,float *x,float *y)
{
	char		*c2,str[256];

	*x=*y=0;

	if (!xml_get_attribute_raw(n,name,str,256)) return(FALSE);
    
	c2=strchr(str,',');
	if (c2==NULL) return(FALSE);
	
	*c2=0x0;
	*x=strtof(str,NULL);
	*y=strtof(c2+1,NULL);
    
	return(TRUE);
}

bool xml_get_attribute_2_coord_float_default(int n,char *name,float *x,float *y,float def_x,float def_y)
{
	char		*c2,str[256];

	if (!xml_get_attribute_raw(n,name,str,256)) {
		*x=def_x;
		*y=def_y;
		return(TRUE);
    }

	*x=*y=0;

	c2=strchr(str,',');
	if (c2==NULL) return(FALSE);
	
	*c2=0x0;
	*x=strtof(str,NULL);
	*y=strtof(c2+1,NULL);
    
	return(TRUE);
}

bool xml_get_attribute_3_coord_int(int n,char *name,int *x,int *y,int *z)
{
	char		*c,*c2,str[256];

	*x=*y=*z=0;

	if (!xml_get_attribute_raw(n,name,str,256)) return(FALSE);
    
	c2=strchr(str,',');
	if (c2==NULL) return(FALSE);
	
	*c2=0x0;
	*x=atoi(str);
	
	c=c2+1;
	c2=strchr(c,',');
	if (c2==NULL) return(FALSE);
	
	*c2=0x0;
	*y=atoi(c);
	
	*z=atoi(c2+1);
    
	return(TRUE);
}

bool xml_get_attribute_3_coord_float(int n,char *name,float *x,float *y,float *z)
{
	char		*c,*c2,str[256];

	*x=*y=*z=0;

	if (!xml_get_attribute_raw(n,name,str,256)) return(FALSE);
    
	c2=strchr(str,',');
	if (c2==NULL) return(FALSE);
	
	*c2=0x0;
	*x=strtof(str,NULL);
	
	c=c2+1;
	c2=strchr(c,',');
	if (c2==NULL) return(FALSE);
	
	*c2=0x0;
	*y=strtof(c,NULL);
	
	*z=strtof(c2+1,NULL);
    
	return(TRUE);
}

bool xml_get_attribute_4_coord_float(int n,char *name,float *r,float *g,float *b,float *a)
{
	char		*c,*c2,str[256];

	*r=*g=*b=*a=0;

	if (!xml_get_attribute_raw(n,name,str,256)) return(FALSE);
    
	c2=strchr(str,',');
	if (c2==NULL) return(FALSE);
	
	*c2=0x0;
	*r=strtof(str,NULL);
	
	c=c2+1;
	c2=strchr(c,',');
	if (c2==NULL) return(FALSE);
	
	*c2=0x0;
	*g=strtof(c,NULL);
	
	c=c2+1;
	c2=strchr(c,',');
	if (c2==NULL) return(FALSE);
	
	*c2=0x0;
	*b=strtof(c,NULL);
	
	*a=strtof(c2+1,NULL);
    
	return(TRUE);
}

bool xml_get_attribute_color(int n,char *name,d3col *col)
{
	int			k;
	char		str[8];

	col->r=col->g=col->b=0;

	if (!xml_get_attribute_raw(n,name,str,8)) return(FALSE);
	
	k=(xml_hex_character_to_int(str[0])*16)+xml_hex_character_to_int(str[1]);
	col->r=((float)k)/(float)0xFF;

	k=(xml_hex_character_to_int(str[2])*16)+xml_hex_character_to_int(str[3]);
	col->g=((float)k)/(float)0xFF;

	k=(xml_hex_character_to_int(str[4])*16)+xml_hex_character_to_int(str[5]);
	col->b=((float)k)/(float)0xFF;
	
	return(TRUE);
}

/* =======================================================

      Write Tags to Buffer
            
======================================================= */

bool xml_writebyte(byte b)
{
	char		*newdataptr;

	if ((xml_filesz+1)>=xml_buffersz) {					// need to increase the buffer size?
	
		newdataptr=(char*)valloc(xml_buffersz+buff_atom_sz);
		if (newdataptr==NULL) return(FALSE);
		
		memmove(newdataptr,xml_dataptr,xml_buffersz);
		free(xml_dataptr);

		xml_dataptr=newdataptr;
		xml_buffersz=xml_buffersz+buff_atom_sz;
	}

	*(xml_dataptr+xml_filesz)=b;
	xml_filesz++;

	return(TRUE);
}

char xml_gethexnibble(int i)
{
	if (i<10) return('0'+i);
	return('A'+(i-10));
}

bool xml_writechar(unsigned char ch)
{
	int			i,sz;
	char		*c,txt[32];

	if (ch<=0x7F) return(xml_writebyte(ch));

	strcpy(txt,"&#x");
	txt[3]=xml_gethexnibble((ch>>4)&0xF);
	txt[4]=xml_gethexnibble(ch&0xF);
	txt[5]=';';
	txt[6]=0x0;

	c=txt;
	sz=(int)strlen(txt);
	for ((i=0);(i<sz);i++) {
		if (!xml_writebyte(*c++)) return(FALSE);
	}

	return(TRUE);
}

bool xml_writetext(char *txt)
{
	int				i,sz;
	unsigned char	*c;

	sz=(int)strlen(txt);
	if (sz==0) return(TRUE);

	c=(unsigned char*)txt;
	for ((i=0);(i<sz);i++) {
		if (!xml_writechar(*c++)) return(FALSE);
	}

	return(TRUE);
}

/* =======================================================

      Add Tags
            
======================================================= */

bool xml_add_tagstart(char *name)
{
	if (!xml_writechar('<')) return(FALSE);
	return(xml_writetext(name));
}

bool xml_add_tagend(bool single)
{
	if (single) {
		return(xml_writetext(" />\n"));
	}
	return(xml_writetext(">\n"));
}

bool xml_add_tagclose(char *name)
{
	if (!xml_writetext("</")) return(FALSE);
	if (!xml_writetext(name)) return(FALSE);
	return(xml_writetext(">\n"));
}

/* =======================================================

      Parse Attributes
            
======================================================= */

void xml_parsetagsinvalue(char *value)
{
	int			i,sz;
	char		c,*srce,*dest,str[5120];

	str[0]=0x0;
	srce=value;
	dest=str;

	sz=(int)strlen(value);
	for ((i=0);(i!=sz);i++) {
		c=*srce++;
		switch (c) {
			case '<':
				*dest++='&';
				*dest++='#';
				*dest++='x';
				*dest++='3';
				*dest++='C';
				*dest++=';';
				break;
			case '>':
				*dest++='&';
				*dest++='#';
				*dest++='x';
				*dest++='3';
				*dest++='E';
				*dest++=';';
				break;
			case '&':
				*dest++='&';
				*dest++='#';
				*dest++='x';
				*dest++='2';
				*dest++='6';
				*dest++=';';
				break;
			case '\"':
				*dest++='&';
				*dest++='#';
				*dest++='x';
				*dest++='2';
				*dest++='2';
				*dest++=';';
				break;
			default:
				*dest++=c;
				break;
		}
	}

	*dest=0x0;

	strcpy(value,str);
}

/* =======================================================

      Add Attributes
            
======================================================= */

bool xml_add_attribute_text(char *name,char *value)
{
	char		str[5120];

	if (!xml_writechar(' ')) return(FALSE);
	if (!xml_writetext(name)) return(FALSE);
	if (!xml_writetext("=\"")) return(FALSE);

	strcpy(str,value);
	xml_parsetagsinvalue(str);
	if (!xml_writetext(str)) return(FALSE);

	return(xml_writechar('\"'));
}

bool xml_add_attribute_int(char *name,int value)
{
	char		txt[256];

	sprintf(txt,"%d",value);
	return(xml_add_attribute_text(name,txt));
}

bool xml_add_attribute_short_array(char *name,short *value,int count,bool removenegone)
{
    int			i;
	char		txt[5120],a[256];

    txt[0]=0x0;
    
    for ((i=0);(i!=count);i++) {
        if ((value[i]==-1) && (removenegone)) continue;
        sprintf(a,"%d",value[i]);
        if (txt[0]!=0x0) strcat(txt,",");
        strcat(txt,a);
    }
    
    if (txt[0]==0x0) return(TRUE);
    
	return(xml_add_attribute_text(name,txt));
}

bool xml_add_attribute_float(char *name,float value)
{
	char		txt[256];
    
	string_convert_float(txt,value);
	return(xml_add_attribute_text(name,txt));
}

bool xml_add_attribute_float_array(char *name,float *value,int count)
{
    int			i;
	char		txt[5120],a[256];

    txt[0]=0x0;
    
    for ((i=0);(i!=count);i++) {
		string_convert_float(a,value[i]);
        if (txt[0]!=0x0) strcat(txt,",");
        strcat(txt,a);
    }
    
    if (txt[0]==0x0) return(TRUE);
    
	return(xml_add_attribute_text(name,txt));
}

bool xml_add_attribute_boolean(char *name,bool value)
{
	if (value) {
		return(xml_add_attribute_text(name,"true"));
	}
    return(TRUE);
}

bool xml_add_attribute_list(char *name,char *list,int value)
{
    return(xml_add_attribute_text(name,(char*)&list[value*32]));
}

bool xml_add_attribute_2_coord_int(char *name,int x,int y)
{
	char		txt[256];

	sprintf(txt,"%d,%d",x,y);
	return(xml_add_attribute_text(name,txt));
}

bool xml_add_attribute_2_coord_float(char *name,float x,float y)
{
	char		txt[256],sx[32],sy[32];
	
	string_convert_float(sx,x);
	string_convert_float(sy,y);
	sprintf(txt,"%s,%s",sx,sy);
	return(xml_add_attribute_text(name,txt));
}

bool xml_add_attribute_3_coord_int(char *name,int x,int y,int z)
{
	char		txt[256];

	sprintf(txt,"%d,%d,%d",x,y,z);
	return(xml_add_attribute_text(name,txt));
}

bool xml_add_attribute_3_coord_float(char *name,float x,float y,float z)
{
	char		txt[256],sx[32],sy[32],sz[32];

	string_convert_float(sx,x);
	string_convert_float(sy,y);
	string_convert_float(sz,z);
	sprintf(txt,"%s,%s,%s",sx,sy,sz);
	return(xml_add_attribute_text(name,txt));
}

bool xml_add_attribute_4_coord_float(char *name,float r,float g,float b,float a)
{
	char		txt[256],sr[32],sg[32],sb[32],sa[32];

	string_convert_float(sr,r);
	string_convert_float(sg,g);
	string_convert_float(sb,b);
	string_convert_float(sa,a);
	sprintf(txt,"%s,%s,%s,%s",sr,sg,sb,sa);
	return(xml_add_attribute_text(name,txt));
}

bool xml_add_attribute_color(char *name,d3col *col)
{
	int			ir,ig,ib;
	char		txt[256];
	
	ir=(int)(col->r*0xFF);
	ig=(int)(col->g*0xFF);
	ib=(int)(col->b*0xFF);
	
	txt[0]=xml_hex_int_to_character(ir>>4);
	txt[1]=xml_hex_int_to_character(ir);
	txt[2]=xml_hex_int_to_character(ig>>4);
	txt[3]=xml_hex_int_to_character(ig);
	txt[4]=xml_hex_int_to_character(ib>>4);
	txt[5]=xml_hex_int_to_character(ib);
	txt[6]=0x0;

	return(xml_add_attribute_text(name,txt));
}



