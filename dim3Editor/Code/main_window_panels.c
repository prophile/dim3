/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Window Panel Setup

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

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern int				cx,cz,cy,vertex_mode,magnify_factor,
						txt_palette_y,txt_palette_high;
extern float			walk_view_y_angle,walk_view_x_angle;
extern bool				map_opened,swap_panel_forward,swap_panel_side,swap_panel_top,
						dp_auto_texture,
						dp_object,dp_lightsoundparticle,dp_node,dp_textured,dp_y_hide;

extern map_type			map;
extern setup_type		setup;

extern WindowRef		mainwind;

/* =======================================================

      Forward Panels
      
======================================================= */

void main_wind_setup_panel_forward(editor_3D_view_setup *view_setup)
{
	Rect				wbox;
	
	GetWindowPortBounds(mainwind,&wbox);

	view_setup->box.top=wbox.top+(toolbar_high+2);
	view_setup->box.bottom=(wbox.top+toolbar_high)+(((wbox.bottom-(txt_palette_high+info_high))-(wbox.top+toolbar_high))/2);
	view_setup->box.left=wbox.left+2;
	view_setup->box.right=(wbox.right-piece_wid)-(((wbox.right-piece_wid)-wbox.left)/2);
	
	view_setup->cpt.x=cx;
	view_setup->cpt.y=cy;
	view_setup->cpt.z=cz;

	view_setup->ang.x=0.0f;
	view_setup->ang.y=swap_panel_forward?180.0f:0.0f;
	view_setup->ang.z=0.0f;
	
	view_setup->fov=walk_view_forward_fov;
	
	view_setup->mesh_only=FALSE;
	view_setup->draw_portal=FALSE;
	view_setup->draw_light_circle=FALSE;
	
	view_setup->swap_on=swap_panel_forward;
	view_setup->clip_on=FALSE;
}

void main_wind_setup_panel_forward_frame(editor_3D_view_setup *view_setup)
{
	Rect				wbox;
	
	GetWindowPortBounds(mainwind,&wbox);

	view_setup->box.top=wbox.top+(toolbar_high+2);
	view_setup->box.bottom=(wbox.top+toolbar_high)+(((wbox.bottom-(txt_palette_high+info_high))-(wbox.top+toolbar_high))/2);
	view_setup->box.left=wbox.left+2;
	view_setup->box.right=(wbox.right-piece_wid)-(((wbox.right-piece_wid)-wbox.left)/2);
	
	view_setup->cpt.x=cx;
	view_setup->cpt.y=cy;
	view_setup->cpt.z=cz;

	view_setup->ang.x=0.0f;
	view_setup->ang.y=swap_panel_forward?180.0f:0.0f;
	view_setup->ang.z=0.0f;
	
	view_setup->fov=walk_view_forward_fov;
	
	view_setup->mesh_only=TRUE;
	view_setup->draw_portal=FALSE;
	view_setup->draw_light_circle=FALSE;

	view_setup->swap_on=swap_panel_forward;
	view_setup->clip_on=FALSE;
}

void main_wind_setup_panel_forward_full(editor_3D_view_setup *view_setup)
{
	Rect				wbox;
	
	GetWindowPortBounds(mainwind,&wbox);

	memmove(&view_setup->box,&wbox,sizeof(Rect));

	view_setup->box.top+=toolbar_high;
	view_setup->box.bottom-=(txt_palette_high+info_high);
	view_setup->box.right-=piece_wid;

	view_setup->cpt.x=cx;
	view_setup->cpt.y=cy;
	view_setup->cpt.z=cz;

	view_setup->ang.x=walk_view_x_angle;
	view_setup->ang.y=walk_view_y_angle;
	view_setup->ang.z=0.0f;
	
	view_setup->fov=walk_view_forward_fov;
	
	view_setup->mesh_only=FALSE;
	view_setup->draw_portal=FALSE;
	view_setup->draw_light_circle=FALSE;

	view_setup->swap_on=FALSE;
	view_setup->clip_on=FALSE;
}

/* =======================================================

      Side Panels
      
======================================================= */

void main_wind_setup_panel_side(editor_3D_view_setup *view_setup)
{
	Rect				wbox;
	
	GetWindowPortBounds(mainwind,&wbox);

	view_setup->box.top=wbox.top+(toolbar_high+2);
	view_setup->box.bottom=(wbox.top+toolbar_high)+(((wbox.bottom-(txt_palette_high+info_high))-(wbox.top+toolbar_high))/2);
	view_setup->box.left=((wbox.right-piece_wid)-(((wbox.right-piece_wid)-wbox.left)/2))+2;
	view_setup->box.right=wbox.right-(piece_wid+2);
	
	view_setup->cpt.x=cx;
	view_setup->cpt.y=cy;
	view_setup->cpt.z=cz;

	view_setup->ang.x=0.0f;
	view_setup->ang.y=swap_panel_side?90.0f:270.0f;
	view_setup->ang.z=0.0f;
	
	view_setup->fov=walk_view_side_fov;
	
	view_setup->mesh_only=FALSE;
	view_setup->draw_portal=FALSE;
	view_setup->draw_light_circle=FALSE;

	view_setup->swap_on=swap_panel_side;
	view_setup->clip_on=FALSE;
}

void main_wind_setup_panel_side_frame(editor_3D_view_setup *view_setup)
{
	Rect				wbox;
	
	GetWindowPortBounds(mainwind,&wbox);

	view_setup->box.top=wbox.top+(toolbar_high+2);
	view_setup->box.bottom=(wbox.top+toolbar_high)+(((wbox.bottom-(txt_palette_high+info_high))-(wbox.top+toolbar_high))/2);
	view_setup->box.left=((wbox.right-piece_wid)-(((wbox.right-piece_wid)-wbox.left)/2))+2;
	view_setup->box.right=wbox.right-(piece_wid+2);
	
	view_setup->cpt.x=cx;
	view_setup->cpt.y=cy;
	view_setup->cpt.z=cz;

	view_setup->ang.x=0.0f;
	view_setup->ang.y=swap_panel_side?90.0f:270.0f;
	view_setup->ang.z=0.0f;
	
	view_setup->fov=walk_view_side_fov;
	
	view_setup->mesh_only=TRUE;
	view_setup->draw_portal=FALSE;
	view_setup->draw_light_circle=FALSE;
	
	view_setup->swap_on=swap_panel_side;
	view_setup->clip_on=FALSE;
}

/* =======================================================

      Top Panels
      
======================================================= */

void main_wind_setup_panel_top(editor_3D_view_setup *view_setup)
{
	int					mag_sz;
	Rect				wbox;
	
	GetWindowPortBounds(mainwind,&wbox);

	view_setup->box.top=(wbox.top+toolbar_high)+((((wbox.bottom-(txt_palette_high+info_high))-(wbox.top+toolbar_high))/2)+2);
	view_setup->box.bottom=wbox.bottom-((txt_palette_high+info_high)+2);
	view_setup->box.left=wbox.left+2;
	view_setup->box.right=wbox.right-(piece_wid+2);
	
	mag_sz=(magnify_factor_max-magnify_factor)*(map_enlarge<<1);
	
	view_setup->cpt.x=cx;
	view_setup->cpt.y=cy-mag_sz;
	view_setup->cpt.z=cz;
	
	view_setup->clip_y=cy;
	view_setup->portal_y=cy;
	
	view_setup->ang.x=swap_panel_top?(90.0f):(-90.0f);
	view_setup->ang.y=0.0f;
	view_setup->ang.z=0.0f;
	
	view_setup->fov=top_view_fov;
	
	view_setup->mesh_only=FALSE;
	view_setup->draw_portal=TRUE;
	view_setup->draw_light_circle=TRUE;
	
	view_setup->swap_on=swap_panel_top;
	view_setup->clip_on=TRUE;
}

void main_wind_setup_panel_top_frame(editor_3D_view_setup *view_setup)
{
	int					mag_sz;
	Rect				wbox;
	
	GetWindowPortBounds(mainwind,&wbox);

	view_setup->box.top=(wbox.top+toolbar_high)+((((wbox.bottom-(txt_palette_high+info_high))-(wbox.top+toolbar_high))/2)+2);
	view_setup->box.bottom=wbox.bottom-((txt_palette_high+info_high)+2);
	view_setup->box.left=wbox.left+2;
	view_setup->box.right=(wbox.right-piece_wid)-(((wbox.right-piece_wid)-wbox.left)/2);
	
	mag_sz=(magnify_factor_max-magnify_factor)*(map_enlarge<<1);
	
	view_setup->cpt.x=cx;
	view_setup->cpt.y=cy-mag_sz;
	view_setup->cpt.z=cz;
	
	view_setup->clip_y=cy;
	view_setup->portal_y=cy;

	view_setup->ang.x=-90.0f;
	view_setup->ang.y=0.0f;
	view_setup->ang.z=0.0f;
	
	view_setup->fov=top_view_fov;
	
	view_setup->mesh_only=TRUE;
	view_setup->draw_portal=TRUE;
	view_setup->draw_light_circle=TRUE;
	
	view_setup->swap_on=swap_panel_top;
	view_setup->clip_on=FALSE;
}

void main_wind_setup_panel_top_full(editor_3D_view_setup *view_setup)
{
	int					mag_sz;
	Rect				wbox;
	
	GetWindowPortBounds(mainwind,&wbox);

	memmove(&view_setup->box,&wbox,sizeof(Rect));
	
	view_setup->box.top+=toolbar_high;
	view_setup->box.bottom-=(txt_palette_high+info_high);
	view_setup->box.right-=piece_wid;

	mag_sz=(magnify_factor_max-magnify_factor)*(map_enlarge<<1);

	view_setup->cpt.x=cx;
	view_setup->cpt.y=cy-mag_sz;
	view_setup->cpt.z=cz;
	
	view_setup->clip_y=cy;
	view_setup->portal_y=cy;

	view_setup->ang.x=-90.0f;
	view_setup->ang.y=0.0f;
	view_setup->ang.z=0.0f;
	
	view_setup->fov=top_view_fov;
	
	view_setup->mesh_only=FALSE;
	view_setup->draw_portal=TRUE;
	view_setup->draw_light_circle=TRUE;
	
	view_setup->swap_on=FALSE;
	view_setup->clip_on=TRUE;
}

/* =======================================================

      Walk Panels
      
======================================================= */

void main_wind_setup_panel_walk(editor_3D_view_setup *view_setup)
{
	Rect				wbox;
	
	GetWindowPortBounds(mainwind,&wbox);

	view_setup->box.top=(wbox.top+toolbar_high)+((((wbox.bottom-(txt_palette_high+info_high))-(wbox.top+toolbar_high))/2)+2);
	view_setup->box.bottom=wbox.bottom-((txt_palette_high+info_high)+2);
	view_setup->box.left=((wbox.right-piece_wid)-(((wbox.right-piece_wid)-wbox.left)/2))+2;
	view_setup->box.right=wbox.right-(piece_wid+2);
	
	view_setup->cpt.x=cx;
	view_setup->cpt.y=cy;
	view_setup->cpt.z=cz;

	view_setup->ang.x=walk_view_x_angle;
	view_setup->ang.y=walk_view_y_angle;
	view_setup->ang.z=0.0f;
	
	view_setup->fov=walk_view_forward_fov;
	
	view_setup->mesh_only=FALSE;
	view_setup->draw_portal=FALSE;
	view_setup->draw_light_circle=FALSE;
	
	view_setup->swap_on=FALSE;
	view_setup->clip_on=FALSE;
}

/* =======================================================

      Full Screen
      
======================================================= */

void main_wind_setup_full_screen_box(Rect *box)
{
	GetWindowPortBounds(mainwind,box);
	
	box->top+=toolbar_high;
	box->bottom-=(txt_palette_high+info_high);
	box->right-=piece_wid;
}

