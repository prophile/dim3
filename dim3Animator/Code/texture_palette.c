/****************************** File *********************************

Module: dim3 Animator
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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "window.h"
#include "tab.h"
#include "dialog.h"
#include "model.h"

extern int						cur_mesh,cur_pose,gl_view_x_sz,gl_view_y_sz;
extern bool						fileopen;
extern model_type				model;

extern WindowRef				model_wind;

/* =======================================================

      Texture Drawing
      
======================================================= */

void texture_palette_bitmap_draw(bitmap_type *bitmap,CGrafPtr dport,Rect *dbox)
{
	register int		x,y,xsz,ysz,row_add;
	register ptr		sptr,dptr;
	int					xbyte,k,gray;
	unsigned char		r,g,b;
	float				alpha,fr,fg,fb;
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

      Texture Palette Drawing
      
======================================================= */

void texture_palette_draw(void)
{
	int						i,row_texture_count;
	Rect					wbox,box;
    RGBColor				ltgraycolor={0xCCCC,0xCCCC,0xCCCC},blackcolor={0x0,0x0,0x0};
	texture_type			*texture;
    GrafPtr					saveport;
	
 	if (!fileopen) return;

		// drawing area
		
	GetPort(&saveport);
	SetPort(GetWindowPort(model_wind));
	
	GetWindowPortBounds(model_wind,&wbox);
	
	wbox.top+=tool_button_size+gl_view_y_sz;
	wbox.bottom=wbox.top+texture_palette_height;
	
		// draw textures

	row_texture_count=max_model_texture/texture_palette_row_count;
	
	box=wbox;
	box.right=box.left+texture_palette_texture_size;
	box.bottom=box.top+texture_palette_texture_size;
	
	texture=model.textures;

	for (i=0;i!=max_model_texture;i++) {
			
			// the texture
			
		if (texture->bitmaps[0].data!=NULL) {
			texture_palette_bitmap_draw(&texture->bitmaps[0],GetWindowPort(model_wind),&box);
        }
		else {
			RGBForeColor(&ltgraycolor);
			PaintRect(&box);
		}

			// the frame
			
        RGBForeColor(&blackcolor);
		FrameRect(&box);
		
		if (((i+1)%row_texture_count)==0) {
			OffsetRect(&box,0,texture_palette_texture_size);
			box.left=0;
			box.right=texture_palette_texture_size;
		}
		else {
			OffsetRect(&box,texture_palette_texture_size,0);
		}
		
		texture++;
	}

	SetPort(saveport);
}

/* =======================================================

      Texture Palette Clicking
      
======================================================= */

void texture_palette_click(Point pt,bool dblclick)
{
	int					x,y,nsel,row_texture_count;
	Rect				wbox;
	
    if (!fileopen) return;
 
		// drawing area
		
	GetWindowPortBounds(model_wind,&wbox);
	
	wbox.top+=tool_button_size+gl_view_y_sz;
	wbox.bottom=wbox.top+texture_palette_height;

		// find clicked texture
	
	row_texture_count=max_model_texture/texture_palette_row_count;
	
	x=(pt.h-wbox.left)/texture_palette_texture_size;
	y=(pt.v-wbox.top)/texture_palette_texture_size;
	nsel=(y*row_texture_count)+x;
	
	if ((nsel<0) || (nsel>(max_model_texture-1))) return;
		
		// handle click
	
	if (dblclick) {
		dialog_texture_setting_run(nsel);
	}
	else {
		model_set_sel_mask_material(&model,cur_mesh,nsel);
		hilite_vertex_rows();
	}
	
    draw_model_wind_pose(&model,cur_mesh,cur_pose);
    texture_palette_draw();
}

