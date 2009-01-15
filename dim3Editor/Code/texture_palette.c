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

extern int				drag_mode;

extern map_type			map;

int						txt_page,txt_offset,
						txt_palette_y,txt_palette_high,txt_pixel_sz;
d3rect					txt_palette_box;

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
	
	txt_palette_box.lx=wbox.left;
	txt_palette_box.rx=wbox.right-piece_wid;
	txt_palette_box.ty=txt_palette_y;
	txt_palette_box.by=txt_palette_box.ty+txt_palette_high;
}

/* =======================================================

      Get and Put Selection Texture
      
======================================================= */

int texture_palette_get_selected_texture(void)
{
	int				type,main_idx,poly_idx;
	
	if (select_count()!=1) return(-1);
	
	select_get(0,&type,&main_idx,&poly_idx);
	
	if (type==liquid_piece) return(map.liquid.liquids[main_idx].txt_idx);
	if (type==mesh_piece) return(map.mesh.meshes[main_idx].polys[poly_idx].txt_idx);
	
	return(-1);
}

void texture_palette_put_selected_texture(int txt_idx)
{
	int					n,k,sel_count,type,main_idx,poly_idx;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
	
		select_get(n,&type,&main_idx,&poly_idx);
		
			// liquids
			
		if (type==liquid_piece) {
			map.liquid.liquids[main_idx].txt_idx=txt_idx;
			continue;
		}
		
			// meshes
			
		if (type!=mesh_piece) continue;
		
		mesh=&map.mesh.meshes[main_idx];
		
			// only set polygon
			
		if (drag_mode==drag_mode_polygon) {
			mesh->polys[poly_idx].txt_idx=txt_idx;
		}
		
			// set all mesh
			
		else {
			
			poly=mesh->polys;
			
			for (k=0;k!=mesh->npoly;k++) {
				poly->txt_idx=txt_idx;
				poly++;
			}
		
		}		
	}
}

/* =======================================================

      Texture Palette Drawing
      
======================================================= */

void texture_palette_draw(void)
{
	int					i,x,y,yadd,sel,k,idx,page,page_cnt;
	texture_type		*texture;
	
		// viewport setup
		
	main_wind_set_viewport(&txt_palette_box,TRUE,FALSE);
	
		// texture page switch
		
	page_cnt=max_map_texture/txt_wind_per_page;
	yadd=(txt_wind_row_count*txt_pixel_sz)/page_cnt;

	for (i=0;i!=page_cnt;i++) {
		y=txt_palette_y+(i*yadd);
		
		glColor4f(0.5f,0.5f,0.5f,1.0f);
		
		glBegin(GL_QUADS);
		glVertex2i(0,y);
		glVertex2i(16,y);
		glVertex2i(16,(y+yadd));
		glVertex2i(0,(y+yadd));
		glEnd();
		
		glColor4f(0.0f,0.0f,0.0f,1.0f);
		
		glBegin(GL_LINE_LOOP);
		glVertex2i(0,y);
		glVertex2i(16,y);
		glVertex2i(16,(y+yadd));
		glVertex2i(0,(y+yadd));
		glEnd();
		
		if (txt_page==i) {
			glColor4f(0.0f,0.0f,1.0f,1.0f);
			
			glBegin(GL_TRIANGLES);
			glVertex2i(1,(y+1));
			glVertex2i(15,(y+(yadd/2)));
			glVertex2i(1,((y+yadd)-1));
			glEnd();
		}
	}

		// textures
		
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
		
	for (i=0;i!=max_map_texture;i++) {
		texture=&map.textures[i];
	
		page=i/txt_wind_per_page;
		if (page!=txt_page) continue;
		
		idx=i-(txt_page*txt_wind_per_page);
	
		k=idx/txt_wind_column_count;
		x=((idx-(k*txt_wind_column_count))*txt_pixel_sz)+16;
		y=txt_palette_y+(k*txt_pixel_sz);
		
			// the textures
			
		if (texture->bitmaps[0].gl_id!=-1) {
			glBindTexture(GL_TEXTURE_2D,texture->bitmaps[0].gl_id);

			glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex2i(x,y);
			glTexCoord2f(1,0);
			glVertex2i((x+txt_pixel_sz),y);
			glTexCoord2f(1,1);
			glVertex2i((x+txt_pixel_sz),(y+txt_pixel_sz));
			glTexCoord2f(0,1);
			glVertex2i(x,(y+txt_pixel_sz));
			glEnd();
		}
	}
	
	glDisable(GL_TEXTURE_2D);
	
	glDisable(GL_ALPHA_TEST);
	
		// lines
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
		
	for (i=0;i!=max_map_texture;i++) {
		page=i/txt_wind_per_page;
		if (page!=txt_page) continue;
		
		idx=i-(txt_page*txt_wind_per_page);
	
		k=idx/txt_wind_column_count;
		x=((idx-(k*txt_wind_column_count))*txt_pixel_sz)+16;
		y=txt_palette_y+(k*txt_pixel_sz);
		
		glBegin(GL_LINE_LOOP);
		glVertex2i(x,(y+1));
		glVertex2i((x+txt_pixel_sz),(y+1));
		glVertex2i((x+txt_pixel_sz),(y+txt_pixel_sz));
		glVertex2i(x,(y+txt_pixel_sz));
		glEnd();
	}
	
		// selection
		
	sel=texture_palette_get_selected_texture();
	if (sel==-1) return;
	
	page=sel/txt_wind_per_page;
	if (page!=txt_page) return;
	
	idx=sel-(txt_page*txt_wind_per_page);

	k=idx/txt_wind_column_count;
	x=((idx-(k*txt_wind_column_count))*txt_pixel_sz)+16;
	y=txt_palette_y+(k*txt_pixel_sz);
	
	glColor4f(1.0f,0.0f,0.0f,1.0f);
	
	glBegin(GL_LINE_LOOP);
	glVertex2i(x,(y+1));
	glVertex2i((x+txt_pixel_sz),(y+1));
	glVertex2i((x+txt_pixel_sz),(y+txt_pixel_sz));
	glVertex2i(x,(y+txt_pixel_sz));
	glEnd();
}

/* =======================================================

      Reset Texture Palette
      
======================================================= */

void texture_palette_reset(void)
{
	int			sel;
	
		// reset texture set if necessary
		
	sel=texture_palette_get_selected_texture();
	if (sel!=-1) txt_offset=sel/8;
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
			main_wind_draw();
		}
		return;
	}
	
		// find clicked texture
	
	nsel=((pt.v/txt_pixel_sz)*txt_wind_column_count)+((pt.h-16)/txt_pixel_sz);
	nsel+=(txt_page*txt_wind_per_page);
		
		// double clicks
		
	if (dblclick) {
		dialog_texture_setting_run(nsel);
		main_wind_draw();
		return;
	}
    
		// single clicks
		
	if (map.textures[nsel].bitmaps[0].gl_id==-1) return;
	
	texture_palette_put_selected_texture(nsel);
	texture_palette_reset();
	main_wind_draw();
}

