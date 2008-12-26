/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Info Palette

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

extern int				cur_mesh;
extern bool				fileopen;
extern model_type		model;

extern WindowRef		model_wind;

/* =======================================================

      Draw Info Palette
      
======================================================= */

void info_palette_draw(void)
{
    int					i,x,y,ntexture;
	char				txt[256];
	Rect				wbox;
    RGBColor			blackcolor={0x0,0x0,0x0};
	GrafPtr				saveport;

	if (!fileopen) return;
	
		// get draw box
		
	GetPort(&saveport);
	SetPort(GetWindowPort(model_wind));

	GetWindowPortBounds(model_wind,&wbox);
	
	wbox.top=wbox.bottom-info_palette_height;

	EraseRect(&wbox);
	
		// draw line
		
	RGBForeColor(&blackcolor);
	MoveTo(wbox.left,wbox.top);
	LineTo(wbox.right,wbox.top);
	
		// draw info
		
	x=wbox.left;
	y=wbox.bottom-3;

	sprintf(txt,"X: %d",model.view_box.size.x);
	MoveTo((x+5),y);
	DrawText(txt,0,strlen(txt));

	sprintf(txt,"Z: %d",model.view_box.size.z);
	MoveTo((x+135),y);
	DrawText(txt,0,strlen(txt));

	sprintf(txt,"Y: %d",model.view_box.size.y);
	MoveTo((x+265),y);
	DrawText(txt,0,strlen(txt));

	sprintf(txt,"Vertexes: %d",model.meshes[cur_mesh].nvertex);
	MoveTo((x+415),y);
	DrawText(txt,0,strlen(txt));
	
	sprintf(txt,"Bones: %d",model.nbone);
	MoveTo((x+545),y);
	DrawText(txt,0,strlen(txt));
	
	sprintf(txt,"Triangles: %d",model.meshes[cur_mesh].ntrig);
	MoveTo((x+675),y);
	DrawText(txt,0,strlen(txt));
	
	ntexture=0;
	for (i=0;i!=max_model_texture;i++) {
		if (model.textures[i].bitmaps[0].gl_id!=-1) ntexture++;
	}

	sprintf(txt,"Materials: %d",(int)ntexture);
	MoveTo((x+845),y);
	DrawText(txt,0,strlen(txt));
	
	SetPort(saveport);
}

