/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Texture Palette

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

#include "interface.h"
#include "dialog.h"
#include "common_view.h"

#define txt_wind_column_count		32
#define txt_wind_row_count			2
#define txt_wind_per_page			(txt_wind_column_count*txt_wind_row_count)

#define txt_wind_max_pixel_sz		32

extern int				cr;
extern bool				dp_primitive;

extern map_type			map;

int						txt_page,txt_offset,
						txt_palette_y,txt_palette_high,txt_pixel_sz;

extern WindowRef		mainwind;

/* =======================================================

      Texture Palette Setup
      
======================================================= */

void texture_palette_setup(void)
{
	Rect			wbox;
	
	GetWindowPortBounds(mainwind,&wbox);
		
	txt_pixel_sz=((wbox.right-piece_wid)-(wbox.left+16))/txt_wind_column_count;
	if (txt_pixel_sz>txt_wind_max_pixel_sz) txt_pixel_sz=txt_wind_max_pixel_sz;
	
	txt_palette_high=(txt_wind_row_count*txt_pixel_sz);
	txt_palette_y=(wbox.bottom-txt_palette_high)-info_high;
}

/* =======================================================

      Texture Drawing
      
======================================================= */

void texture_palette_bitmap_draw(bitmap_type *bitmap,CGrafPtr dport,Rect *dbox)
{
	int					x,y,xsz,ysz,row_add,
						xbyte,k,gray;
	unsigned char		r,g,b;
	float				alpha,fr,fg,fb;
	ptr					sptr,dptr;
	Rect				box;
	PixMapHandle		texturemap;
	GWorldPtr			gworld;
	
	xsz=bitmap->wid;
	ysz=bitmap->high;
	
		// make gworld for texture
		
	SetRect(&box,0,0,xsz,ysz);
	NewGWorld(&gworld,32,&box,NULL,NULL,0);
    
        // copy RGB for gworld
        
	texturemap=GetGWorldPixMap(gworld);
	
	LockPixels(texturemap);
	sptr=(ptr)GetPixBaseAddr(texturemap);
	xbyte=xsz<<2;
	row_add=GetPixRowBytes(texturemap)-xbyte;

	dptr=bitmap->data;
	
	switch (bitmap->alpha_mode) {
	
		case alpha_mode_none:
		
			for (y=0;y!=ysz;y++) {
				for (x=0;x!=xsz;x++) {
					*sptr++=0xFF;
					*sptr++=*dptr++;
					*sptr++=*dptr++;
					*sptr++=*dptr++;
					dptr++;
				}
				sptr=sptr+row_add;
			}
			break;
			
		case alpha_mode_cut_out:
		
			for (y=0;y!=ysz;y++) {
				for (x=0;x!=xsz;x++) {
					r=*dptr++;
					g=*dptr++;
					b=*dptr++;
					alpha=*dptr++;
					
					*sptr++=0xFF;
					
					if (alpha!=255) {
						*sptr++=0x0;
						*sptr++=0x0;
						*sptr++=0xFF;
					}
					else {
						*sptr++=r;
						*sptr++=g;
						*sptr++=b;
					}
				}
				sptr=sptr+row_add;
			}
			break;
		
		case alpha_mode_transparent:

			for (y=0;y!=ysz;y++) {
				for (x=0;x!=xsz;x++) {
					
					r=*dptr++;
					g=*dptr++;
					b=*dptr++;
					alpha=((float)*dptr++)/255.0f;
				
						// calculate alpha
						
					if (alpha!=1.0f) {
						gray=((((y/5)+(x/5))&0x1)==0x0)?0x00:0x55;
						
						fr=((float)r)/255.0f;
						fr*=alpha;
						k=(int)(fr*255.0f)+gray;
						if (k>255) k=255;
						r=(unsigned char)k;
						
						fg=((float)g)/255.0f;
						fg*=alpha;
						k=(int)(fg*255.0f)+gray;
						if (k>255) k=255;
						g=(unsigned char)k;
						
						fb=((float)b)/255.0f;
						fb*=alpha;
						k=(int)(fb*255.0f)+gray;
						if (k>255) k=255;
						b=(unsigned char)k;
					}
					
					*sptr++=0xFF;
					*sptr++=r;
					*sptr++=g;
					*sptr++=b;
				}
				
				sptr=sptr+row_add;
			}
			break;
	}

	CopyBits((BitMap*)(*texturemap),GetPortBitMapForCopyBits(dport),&box,dbox,srcCopy,NULL);
    
	UnlockPixels(texturemap);
	
		// dispose the gworld
		
	DisposeGWorld(gworld);
}

/* =======================================================

      Get and Put Selection Texture
      
======================================================= */

int texture_palette_get_selected_fill(void)
{
	int				type,index;
	
	if (select_count()!=1) return(-1);
	
	select_get(0,&type,&index);
	if ((type!=segment_piece) && (type!=primitive_piece)) return(-1);

	return(map.segments[index].fill);
}

void texture_palette_put_selected_fill(int fill)
{
	int				n,k,sel_count,type,index,primitive_uid;
	segment_type	*seg;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);
		
		switch (type) {
		
			case segment_piece:
				map.segments[index].fill=fill;
				break;
				
			case primitive_piece:
			
				if (!dp_primitive) {
					map.segments[index].fill=fill;
					break;
				}
				
				primitive_uid=map.segments[index].primitive_uid[0];
				
				seg=map.segments;
				
				for (k=0;k!=map.nsegment;k++) {
					if (seg->primitive_uid[0]==primitive_uid) seg->fill=fill;
					seg++;
				}
				break;
		}
		
	}
}

/* =======================================================

      Texture Palette Drawing
      
======================================================= */

void texture_palette_draw_select(void)
{
	int					i,x,y,k,idx,sel,page;
	Rect				box;
	GrafPtr				saveport;
	RGBColor			blackcolor={0x0,0x0,0x0},redcolor={0xFFFF,0x0,0x0};
	
	GetPort(&saveport);
	SetPort(GetWindowPort(mainwind));
		
	sel=texture_palette_get_selected_fill();

	for (i=0;i!=max_map_texture;i++) {
	
		page=i/txt_wind_per_page;
		if (page!=txt_page) continue;
		
		idx=i-(txt_page*txt_wind_per_page);
	
		k=idx/txt_wind_column_count;
		x=((idx-(k*txt_wind_column_count))*txt_pixel_sz)+16;
		y=txt_palette_y+(k*txt_pixel_sz);
		
		if (i!=sel) {
			RGBForeColor(&blackcolor);
		}
		else {
			RGBForeColor(&redcolor);
		}
		
		SetRect(&box,x,y,(x+txt_pixel_sz),(y+txt_pixel_sz));
		FrameRect(&box);
	}
    
    RGBForeColor(&blackcolor);

	SetPort(saveport);
}

void texture_palette_draw(void)
{
	int					i,x,y,yadd,k,idx,page,page_cnt;
	char				str[8];
	Rect				wbox,box;
	texture_type		*texture;
	GrafPtr				saveport;
	RGBColor			blackcolor={0x0,0x0,0x0},ltltgraycolor={0xCCCC,0xCCCC,0xCCCC},dkgraycolor={0x5555,0x5555,0x5555};
	
	GetPort(&saveport);
	SetPort(GetWindowPort(mainwind));
	
	GetWindowPortBounds(mainwind,&wbox);
	
		// clear the entire bottom
		
	box=wbox;
	box.top=txt_palette_y;
	box.bottom=box.top+txt_palette_high;
	
	EraseRect(&box);
	
		// texture pages
		
	page_cnt=max_map_texture/txt_wind_per_page;
	yadd=(txt_wind_row_count*txt_pixel_sz)/page_cnt;
	
	for (i=0;i!=page_cnt;i++) {
		y=txt_palette_y+(i*yadd);
		SetRect(&box,0,y,16,(y+yadd));
		
		if (txt_page==i) {
			RGBForeColor(&ltltgraycolor);
		}
		else {
			RGBForeColor(&dkgraycolor);
		}
		
		PaintRect(&box);
		
		RGBForeColor(&blackcolor);
		FrameRect(&box);
		
		sprintf(str,"%d",(i+1));
		
		MoveTo(5,((y+(yadd/2))+4));
		DrawText(str,0,strlen(str));
	}
		
		// textures
		
	for (i=0;i!=max_map_texture;i++) {
	
		texture=&map.textures[i];
	
		page=i/txt_wind_per_page;
		if (page!=txt_page) continue;
		
		idx=i-(txt_page*txt_wind_per_page);
	
		k=idx/txt_wind_column_count;
		x=((idx-(k*txt_wind_column_count))*txt_pixel_sz)+16;
		y=txt_palette_y+(k*txt_pixel_sz);
		
		SetRect(&box,x,y,(x+txt_pixel_sz),(y+txt_pixel_sz));
		
			// the textures
			
		if (texture->bitmaps[0].data==NULL) {
			RGBForeColor(&ltltgraycolor);
			PaintRect(&box);
            RGBForeColor(&blackcolor);
		}
		else {
            texture_palette_bitmap_draw(&texture->bitmaps[0],GetWindowPort(mainwind),&box);
		}
	}
    
    RGBForeColor(&blackcolor);

	SetPort(saveport);
	
		// draw selection
		
	texture_palette_draw_select();
}

/* =======================================================

      Texture Palette Resize
      
======================================================= */

void texture_palette_resize(void)
{
	texture_palette_setup();
	texture_palette_draw();
}

/* =======================================================

      Reset Texture Palette
      
======================================================= */

void texture_palette_reset(void)
{
	int			sel;
	
		// reset texture set if necessary
		
	sel=texture_palette_get_selected_fill();
	if (sel!=-1) txt_offset=sel/8;
	
		// redraw window
		
	texture_palette_draw_select();
}

/* =======================================================

      Texture Clicking
      
======================================================= */

void texture_palette_click(Point pt,bool dblclick)
{
	int				nsel,page,page_cnt,yadd;
	
		// move within palette
		
	pt.v-=txt_palette_y;
	
		// texture page change
		
	if (pt.h<16) {
		page_cnt=max_map_texture/txt_wind_per_page;
		yadd=(txt_wind_row_count*txt_pixel_sz)/page_cnt;
		page=pt.v/yadd;
		if (txt_page!=page) {
			txt_page=page;
			texture_palette_draw();
		}
		return;
	}
	
		// find clicked texture
	
	nsel=((pt.v/txt_pixel_sz)*txt_wind_column_count)+((pt.h-16)/txt_pixel_sz);
	nsel+=(txt_page*txt_wind_per_page);
		
		// double clicks
		
	if (dblclick) {
		dialog_texture_setting_run(nsel);
		texture_palette_draw();
		return;
	}
    
		// single clicks
		
	if (map.textures[nsel].bitmaps[0].data==NULL) return;
	
	texture_palette_put_selected_fill(nsel);
	main_wind_draw();
	texture_palette_reset();
}

