/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Texture Setting Dialogs

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

#define kTextureSettingFrame						FOUR_CHAR_CODE('frme')
#define kTextureSettingFrameBitmap					FOUR_CHAR_CODE('bitm')
#define kTextureSettingFrameBitmapEdit				FOUR_CHAR_CODE('edbt')
#define kTextureSettingFrameBumpmap					FOUR_CHAR_CODE('bmpm')
#define kTextureSettingFrameBumpmapEdit				FOUR_CHAR_CODE('edbm')
#define kTextureSettingFrameBumpmapClear			FOUR_CHAR_CODE('clbm')
#define kTextureSettingFrameSpecularmap				FOUR_CHAR_CODE('spmp')
#define kTextureSettingFrameSpecularmapEdit			FOUR_CHAR_CODE('edsp')
#define kTextureSettingFrameSpecularmapClear		FOUR_CHAR_CODE('clsp')
#define kTextureSettingFrameGlowmap					FOUR_CHAR_CODE('gwmp')
#define kTextureSettingFrameGlowmapEdit				FOUR_CHAR_CODE('edgw')
#define kTextureSettingFrameGlowmapClear			FOUR_CHAR_CODE('clgw')
#define kTextureSettingFrameWait					FOUR_CHAR_CODE('watm')
#define kTextureSettingColor						FOUR_CHAR_CODE('colr')
#define kTextureSettingBumpMode						FOUR_CHAR_CODE('bmmd')
#define kTextureSettingAnimate						FOUR_CHAR_CODE('anmt')
#define kTextureSettingAlphaAdditive				FOUR_CHAR_CODE('addv')
#define kTextureSettingGlowRate						FOUR_CHAR_CODE('grtm')
#define kTextureSettingGlowMin						FOUR_CHAR_CODE('grmn')
#define kTextureSettingGlowMax						FOUR_CHAR_CODE('grmx')
#define kTextureSettingMaterialName					FOUR_CHAR_CODE('mtrl')
#define kTextureSettingShaderUse					FOUR_CHAR_CODE('tsus')
#define kTextureSettingScaleX						FOUR_CHAR_CODE('tsxf')
#define kTextureSettingScaleY						FOUR_CHAR_CODE('tsyf')
#define kTextureSettingScaleOn						FOUR_CHAR_CODE('tson')
#define kTextureSettingScaleLockOffset				FOUR_CHAR_CODE('tslo')

#define kTextureSettingButtonAddFrame				FOUR_CHAR_CODE('addt')
#define kTextureSettingButtonSubFrame				FOUR_CHAR_CODE('subt')
#define kTextureSettingShaderSettings				FOUR_CHAR_CODE('shad')

#define kTextureSettingButtonColor					FOUR_CHAR_CODE('colh')

extern file_path_setup_type		file_path_setup;
extern map_type					map;

int								dialog_texture_wind_current_txt,dialog_texture_wind_current_frame;
texture_type					frame_texture;
WindowRef						dialog_texture_wind;
RGBColor						dialog_texture_color;





/* =======================================================

      Texture Drawing
      
======================================================= */

void texture_setting_bitmap_draw(bitmap_type *bitmap,CGrafPtr dport,Rect *dbox)
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

      Texture Frames
      
======================================================= */

void texture_setting_frame_build_combo(bool first)
{
	int						n,nframe;
	char					str[32];
	texture_type			*texture;
	
		// set combo
		
	dialog_hide(dialog_texture_wind,kTextureSettingFrame,0,FALSE);
		
	dialog_clear_combo(dialog_texture_wind,kTextureSettingFrame,0);
	
	nframe=map_count_texture_frames(&map,dialog_texture_wind_current_txt);
	if (nframe<=0) nframe=1;
	
	for (n=0;n!=nframe;n++) {
		sprintf(str,"Frame %d",n);
		dialog_add_combo_item(dialog_texture_wind,kTextureSettingFrame,0,str,0);
	}
	
	if (first) {
		dialog_texture_wind_current_frame=0;
		dialog_set_combo(dialog_texture_wind,kTextureSettingFrame,0,0);
	}
	else {
		dialog_texture_wind_current_frame=nframe-1;
		dialog_set_combo(dialog_texture_wind,kTextureSettingFrame,0,(nframe-1));
	}
	
	dialog_hide(dialog_texture_wind,kTextureSettingFrame,0,TRUE);
	
		// set wait (so next bitmap draw doesn't save wrong wait)
		
	texture=&map.textures[dialog_texture_wind_current_txt];
	dialog_set_int(dialog_texture_wind,kTextureSettingFrameWait,0,texture->animate.wait[dialog_texture_wind_current_frame]);
}

void texture_setting_frame_save(void)
{
	texture_type			*texture;
	
	texture=&map.textures[dialog_texture_wind_current_txt];
	texture->animate.wait[dialog_texture_wind_current_frame]=dialog_get_int(dialog_texture_wind,kTextureSettingFrameWait,0);
}

void texture_setting_frame_reset(void)
{
    int						cframe,bump_mode;
	bool					bump_enable,bump_show;
    Rect					box;
	ControlRef				ctrl;
	ControlID				ctrl_id;
    RGBColor				ltgraycolor={0xCCCC,0xCCCC,0xCCCC},blackcolor={0x0,0x0,0x0};
	texture_type			*texture;
	
		// save wait from current frame
		
	texture_setting_frame_save();
	
		// enable/disable flags
		
	bump_mode=dialog_get_value(dialog_texture_wind,kTextureSettingBumpMode,0)-1;
	bump_enable=((bump_mode!=bump_mode_none) && (bump_mode!=bump_mode_auto_generate));
	bump_show=(bump_mode!=bump_mode_none);
	
		// get current frame
		
	texture=&map.textures[dialog_texture_wind_current_txt];
	cframe=dialog_get_combo(dialog_texture_wind,kTextureSettingFrame,0);
	dialog_texture_wind_current_frame=cframe;
		
		// draw bitmap
	
	SetPort(GetWindowPort(dialog_texture_wind));
	
	ctrl_id.signature=kTextureSettingFrameBitmap;
	ctrl_id.id=0;
	GetControlByID(dialog_texture_wind,&ctrl_id,&ctrl);
	
	GetControlBounds(ctrl,&box);
		
	if (texture->bitmaps[cframe].data==NULL) {
		RGBForeColor(&ltgraycolor);
		PaintRect(&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	else {
		texture_setting_bitmap_draw(&texture->bitmaps[cframe],GetWindowPort(dialog_texture_wind),&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	
		// draw bumpmap
		
	ctrl_id.signature=kTextureSettingFrameBumpmap;
	ctrl_id.id=0;
	GetControlByID(dialog_texture_wind,&ctrl_id,&ctrl);
	
	GetControlBounds(ctrl,&box);
	
	if ((texture->bumpmaps[cframe].data==NULL) || (!bump_show)) {
		RGBForeColor(&ltgraycolor);
		PaintRect(&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	else {
		texture_setting_bitmap_draw(&texture->bumpmaps[cframe],GetWindowPort(dialog_texture_wind),&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	
		// draw specularmap
		
	ctrl_id.signature=kTextureSettingFrameSpecularmap;
	ctrl_id.id=0;
	GetControlByID(dialog_texture_wind,&ctrl_id,&ctrl);
	
	GetControlBounds(ctrl,&box);
	
	if (texture->specularmaps[cframe].data==NULL) {
		RGBForeColor(&ltgraycolor);
		PaintRect(&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	else {
		texture_setting_bitmap_draw(&texture->specularmaps[cframe],GetWindowPort(dialog_texture_wind),&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	
		// draw glowmap
		
	ctrl_id.signature=kTextureSettingFrameGlowmap;
	ctrl_id.id=0;
	GetControlByID(dialog_texture_wind,&ctrl_id,&ctrl);
	
	GetControlBounds(ctrl,&box);
	
	if (texture->glowmaps[cframe].data==NULL) {
		RGBForeColor(&ltgraycolor);
		PaintRect(&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	else {
		texture_setting_bitmap_draw(&texture->glowmaps[cframe],GetWindowPort(dialog_texture_wind),&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	
		// set wait
		
	dialog_set_int(dialog_texture_wind,kTextureSettingFrameWait,0,texture->animate.wait[cframe]);
	
		// disable/enable buttons
		
	dialog_enable(dialog_texture_wind,kTextureSettingFrameBumpmapEdit,0,bump_enable);
	dialog_enable(dialog_texture_wind,kTextureSettingFrameBumpmapClear,0,bump_enable);
}

/* =======================================================

      Edit Textures
      
======================================================= */

bool texture_setting_bitmap_open(char *bitmap_name)
{
    char				err_str[256],path[1024];
	unsigned char		p_err_str[256];
	
		// get bitmap
				
	if (!dialog_file_open_run("BitmapOpen","Bitmaps/Textures","png",bitmap_name)) return(FALSE);
	
		// check bitmap
		
	file_paths_data(&file_path_setup,path,"Bitmaps/Textures",bitmap_name,"png");
	if (!bitmap_check(path,err_str)) {
		CopyCStringToPascal(err_str,p_err_str);
		StandardAlert(0,"\pTexture Error",p_err_str,NULL,NULL);
		return(FALSE);
	}
	
	return(TRUE);
}

bool texture_setting_bumpmap_open(char *bumpmap_name)
{
	int					bump_mode;
    char				err_str[256],path[1024];
	unsigned char		p_err_str[256];
	
		// get current bump mode
		
	bump_mode=dialog_get_value(dialog_texture_wind,kTextureSettingBumpMode,0)-1;
	
		// get bump or height map
		
	bumpmap_name[0]=0x0;
		
	switch (bump_mode) {
	
		case bump_mode_height_map:
		
			if (!dialog_file_open_run("HeightmapOpen","Bitmaps/Textures_Height","png",bumpmap_name)) {
				bumpmap_name[0]=0x0;
				break;
			}
			
			file_paths_data(&file_path_setup,path,"Bitmaps/Textures_Height",bumpmap_name,"png");
			if (!bitmap_check(path,err_str)) {
				CopyCStringToPascal(err_str,p_err_str);
				StandardAlert(0,"\pTexture Error",p_err_str,NULL,NULL);
				return(FALSE);
			}
			break;
			
		case bump_mode_normal_map:
		
			if (!dialog_file_open_run("BumpmapOpen","Bitmaps/Textures_dot3","png",bumpmap_name)) {
				bumpmap_name[0]=0x0;
				break;
			}
			
			file_paths_data(&file_path_setup,path,"Bitmaps/Textures_dot3",bumpmap_name,"png");
			if (!bitmap_check(path,err_str)) {
				CopyCStringToPascal(err_str,p_err_str);
				StandardAlert(0,"\pTexture Error",p_err_str,NULL,NULL);
				return(FALSE);
			}
			break;
			
	}
	
	return(TRUE);
}

bool texture_setting_specularmap_open(char *specularmap_name)
{
    char				err_str[256],path[1024];
	unsigned char		p_err_str[256];
	
		// get specularmap
				
	if (!dialog_file_open_run("SpecularmapOpen","Bitmaps/Textures_Specular","png",specularmap_name)) return(FALSE);
	
		// check specularmap
		
	file_paths_data(&file_path_setup,path,"Bitmaps/Textures_Specular",specularmap_name,"png");
	if (!bitmap_check(path,err_str)) {
		CopyCStringToPascal(err_str,p_err_str);
		StandardAlert(0,"\pTexture Error",p_err_str,NULL,NULL);
		return(FALSE);
	}
	
	return(TRUE);
}

bool texture_setting_glowmap_open(char *glowmap_name)
{
    char				err_str[256],path[1024];
	unsigned char		p_err_str[256];
	
		// get glowmap
				
	if (!dialog_file_open_run("GlowmapOpen","Bitmaps/Textures_Glow","png",glowmap_name)) return(FALSE);
	
		// check glowmap
		
	file_paths_data(&file_path_setup,path,"Bitmaps/Textures_Glow",glowmap_name,"png");
	if (!bitmap_check(path,err_str)) {
		CopyCStringToPascal(err_str,p_err_str);
		StandardAlert(0,"\pTexture Error",p_err_str,NULL,NULL);
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Texture Setting Event Handlers
      
======================================================= */

static pascal OSStatus texture_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	char			bitmap_name[file_str_len],bumpmap_name[file_str_len],
					specularmap_name[file_str_len],glowmap_name[file_str_len];
	Point			pt;
	RGBColor		color;
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kTextureSettingFrame:
					texture_setting_frame_reset();
					return(noErr);
			
				case kTextureSettingButtonAddFrame:
					texture_setting_frame_save();
					if (texture_setting_bitmap_open(bitmap_name)) {
						SetCursor(*GetCursor(watchCursor));
						map_add_texture_frame(&map,dialog_texture_wind_current_txt,bitmap_name,"","","");
						texture_setting_frame_build_combo(FALSE);
						texture_setting_frame_reset();
						InitCursor();
					}
					return(noErr);
					
				case kTextureSettingButtonSubFrame:
					texture_setting_frame_save();
					SetCursor(*GetCursor(watchCursor));
					map_delete_texture_frame(&map,dialog_texture_wind_current_txt);
					texture_setting_frame_build_combo(FALSE);
					texture_setting_frame_reset();
					InitCursor();
					return(noErr);
					
				case kTextureSettingFrameBitmapEdit:
					if (texture_setting_bitmap_open(bitmap_name)) {
						strcpy(map.textures[dialog_texture_wind_current_txt].bitmaps[dialog_texture_wind_current_frame].name,bitmap_name);
						SetCursor(*GetCursor(watchCursor));
						map_refresh_textures(&map);
						texture_setting_frame_reset();
						InitCursor();
					}
					return(noErr);
					
				case kTextureSettingFrameBumpmapEdit:
					if (texture_setting_bumpmap_open(bumpmap_name)) {
						strcpy(map.textures[dialog_texture_wind_current_txt].bumpmaps[dialog_texture_wind_current_frame].name,bumpmap_name);
						SetCursor(*GetCursor(watchCursor));
						map_refresh_textures(&map);
						texture_setting_frame_reset();
						InitCursor();
					}
					return(noErr);
					
				case kTextureSettingFrameBumpmapClear:
					map.textures[dialog_texture_wind_current_txt].bumpmaps[dialog_texture_wind_current_frame].name[0]=0x0;
					SetCursor(*GetCursor(watchCursor));
					map_refresh_textures(&map);
					texture_setting_frame_reset();
					InitCursor();
					return(noErr);
					
				case kTextureSettingFrameSpecularmapEdit:
					if (texture_setting_specularmap_open(specularmap_name)) {
						strcpy(map.textures[dialog_texture_wind_current_txt].specularmaps[dialog_texture_wind_current_frame].name,specularmap_name);
						SetCursor(*GetCursor(watchCursor));
						map_refresh_textures(&map);
						texture_setting_frame_reset();
						InitCursor();
					}
					return(noErr);
					
				case kTextureSettingFrameSpecularmapClear:
					map.textures[dialog_texture_wind_current_txt].specularmaps[dialog_texture_wind_current_frame].name[0]=0x0;
					SetCursor(*GetCursor(watchCursor));
					map_refresh_textures(&map);
					texture_setting_frame_reset();
					InitCursor();
					return(noErr);

				case kTextureSettingFrameGlowmapEdit:
					if (texture_setting_glowmap_open(glowmap_name)) {
						strcpy(map.textures[dialog_texture_wind_current_txt].glowmaps[dialog_texture_wind_current_frame].name,glowmap_name);
						SetCursor(*GetCursor(watchCursor));
						map_refresh_textures(&map);
						texture_setting_frame_reset();
						InitCursor();
					}
					return(noErr);
					
				case kTextureSettingFrameGlowmapClear:
					map.textures[dialog_texture_wind_current_txt].glowmaps[dialog_texture_wind_current_frame].name[0]=0x0;
					SetCursor(*GetCursor(watchCursor));
					map_refresh_textures(&map);
					texture_setting_frame_reset();
					InitCursor();
					return(noErr);
					
				case kTextureSettingBumpMode:
					SetCursor(*GetCursor(watchCursor));
					map.textures[dialog_texture_wind_current_txt].bump_mode=dialog_get_value(dialog_texture_wind,kTextureSettingBumpMode,0)-1;
					map_refresh_textures(&map);
					texture_setting_frame_reset();
					InitCursor();
					return(noErr);
										
				case kTextureSettingButtonColor:
					pt.h=pt.v=-1;
					if (GetColor(pt,"\pChoose the Light Color:",&dialog_texture_color,&color)) {
						dialog_texture_color=color;
					}
					dialog_draw_color(dialog_texture_wind,kTextureSettingColor,0,&dialog_texture_color);
					return(noErr);
					
				case kTextureSettingShaderSettings:
					dialog_shader_setting_run(dialog_texture_wind_current_txt);
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_texture_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Texture Settings
      
======================================================= */

void dialog_texture_setting_run(int txt)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	texture_type			*texture;

		// the texture
		
	texture=&map.textures[txt];

		// open the dialog
		
	dialog_open(&dialog_texture_wind,"TextureSetting");
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(texture_setting_event_proc);
	InstallWindowEventHandler(dialog_texture_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);

		// frames
		
	dialog_texture_wind_current_txt=txt;
	texture_setting_frame_build_combo(TRUE);
	
		// other dialog controls
		
	dialog_texture_color.red=(int)(texture->col.r*(float)0xFFFF);
	dialog_texture_color.green=(int)(texture->col.g*(float)0xFFFF);
	dialog_texture_color.blue=(int)(texture->col.b*(float)0xFFFF);
		
	dialog_set_value(dialog_texture_wind,kTextureSettingBumpMode,0,(texture->bump_mode+1));
	dialog_set_boolean(dialog_texture_wind,kTextureSettingAnimate,0,texture->animate.on);
	dialog_set_boolean(dialog_texture_wind,kTextureSettingAlphaAdditive,0,texture->additive);
	dialog_set_int(dialog_texture_wind,kTextureSettingGlowRate,0,texture->glow.rate);
	dialog_set_float(dialog_texture_wind,kTextureSettingGlowMin,0,texture->glow.min);
	dialog_set_float(dialog_texture_wind,kTextureSettingGlowMax,0,texture->glow.max);
	dialog_set_text(dialog_texture_wind,kTextureSettingMaterialName,0,texture->material_name);
	dialog_set_boolean(dialog_texture_wind,kTextureSettingShaderUse,0,texture->shader.on);
	dialog_set_float(dialog_texture_wind,kTextureSettingScaleX,0,texture->scale.x);
	dialog_set_float(dialog_texture_wind,kTextureSettingScaleY,0,texture->scale.y);
	dialog_set_boolean(dialog_texture_wind,kTextureSettingScaleOn,0,texture->scale.on);
	dialog_set_boolean(dialog_texture_wind,kTextureSettingScaleLockOffset,0,texture->scale.lock_offset);

		// show window
	
	ShowWindow(dialog_texture_wind);
	
		// redraw bitmaps
	
	texture_setting_frame_reset();
	dialog_draw_color(dialog_texture_wind,kTextureSettingColor,0,&dialog_texture_color);
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_texture_wind);
	
		// dialog to data

	texture_setting_frame_save();
	
	texture->col.r=((float)dialog_texture_color.red/(float)0xFFFF);
	texture->col.g=((float)dialog_texture_color.green/(float)0xFFFF);
	texture->col.b=((float)dialog_texture_color.blue/(float)0xFFFF);
		
	texture->bump_mode=dialog_get_value(dialog_texture_wind,kTextureSettingBumpMode,0)-1;
	texture->animate.on=dialog_get_boolean(dialog_texture_wind,kTextureSettingAnimate,0);
	texture->additive=dialog_get_boolean(dialog_texture_wind,kTextureSettingAlphaAdditive,0);
	texture->glow.rate=dialog_get_int(dialog_texture_wind,kTextureSettingGlowRate,0);
	texture->glow.min=dialog_get_float(dialog_texture_wind,kTextureSettingGlowMin,0);
	texture->glow.max=dialog_get_float(dialog_texture_wind,kTextureSettingGlowMax,0);
	dialog_get_text(dialog_texture_wind,kTextureSettingMaterialName,0,texture->material_name,name_str_len);
	texture->shader.on=dialog_get_boolean(dialog_texture_wind,kTextureSettingShaderUse,0);
	texture->scale.x=dialog_get_float(dialog_texture_wind,kTextureSettingScaleX,0);
	texture->scale.y=dialog_get_float(dialog_texture_wind,kTextureSettingScaleY,0);
	texture->scale.on=dialog_get_boolean(dialog_texture_wind,kTextureSettingScaleOn,0);
	texture->scale.lock_offset=dialog_get_boolean(dialog_texture_wind,kTextureSettingScaleLockOffset,0);
	
		// close window
		
	DisposeWindow(dialog_texture_wind);
}

