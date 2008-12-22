/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Setting Tabs

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

#include "tab.h"

ControlRef						tab_list;

extern int						gl_view_x_sz,gl_view_y_sz;
extern model_type				model;

extern WindowRef				model_wind;

/* =======================================================

      Swith To Tab
      
======================================================= */

void switch_tab_mesh(void)
{
    if (GetControl32BitValue(tab_list)==1) return;
    
    SetControl32BitValue(tab_list,1);
	
	hide_vertex_controls();
	hide_bone_controls();
	hide_pose_controls();
    hide_animate_controls();
 	hide_hit_box_controls();
	
	show_mesh_controls();
}

void switch_tab_vertex(void)
{
    if (GetControl32BitValue(tab_list)==2) return;
    
    SetControl32BitValue(tab_list,2);
	
	hide_mesh_controls();
    hide_bone_controls();
	hide_pose_controls();
    hide_animate_controls();
	hide_hit_box_controls();
	
	show_vertex_controls();
}

void switch_tab_bone(void)
{
    if (GetControl32BitValue(tab_list)==3) return;
    
    SetControl32BitValue(tab_list,3);
	
	hide_mesh_controls();
	hide_vertex_controls();
	hide_pose_controls();
    hide_animate_controls();
	hide_hit_box_controls();
	
    show_bone_controls();
}

void switch_tab_pose(void)
{
    if (GetControl32BitValue(tab_list)==4) return;
    
    SetControl32BitValue(tab_list,4);
	
	hide_mesh_controls();
	hide_vertex_controls();
	hide_bone_controls();
    hide_animate_controls();
	hide_hit_box_controls();
	
    show_pose_controls();
}

void switch_tab_animate(void)
{
    if (GetControl32BitValue(tab_list)==5) return;
    
    SetControl32BitValue(tab_list,5);
	
	hide_mesh_controls();
	hide_vertex_controls();
	hide_bone_controls();
    hide_pose_controls();
	hide_hit_box_controls();
	
    show_animate_controls();
}

void switch_tab_hit_box(void)
{
    if (GetControl32BitValue(tab_list)==6) return;
    
    SetControl32BitValue(tab_list,6);
	
	hide_mesh_controls();
	hide_vertex_controls();
	hide_bone_controls();
    hide_pose_controls();
    hide_animate_controls();
	
	show_hit_box_controls();
}

void switch_tab_control(void)
{
    switch (GetControl32BitValue(tab_list)) {
        case 1:
			hide_vertex_controls();
			hide_bone_controls();
            hide_pose_controls();
            hide_animate_controls();
			hide_hit_box_controls();
			show_mesh_controls();
            break;
        case 2:
			hide_mesh_controls();
 			hide_bone_controls();
			hide_pose_controls();
            hide_animate_controls();
			hide_hit_box_controls();
			show_vertex_controls();
            break;
        case 3:
			hide_mesh_controls();
			hide_vertex_controls();
            hide_pose_controls();
            hide_animate_controls();
			hide_hit_box_controls();
			show_bone_controls();
            break;
        case 4:
			hide_mesh_controls();
			hide_vertex_controls();
			hide_bone_controls();
            hide_animate_controls();
 			hide_hit_box_controls();
			show_pose_controls();
            break;
        case 5:
			hide_mesh_controls();
			hide_vertex_controls();
			hide_bone_controls();
            hide_pose_controls();
			hide_hit_box_controls();
            show_animate_controls();
            break;
        case 6:
			hide_mesh_controls();
			hide_vertex_controls();
			hide_bone_controls();
            hide_pose_controls();
            hide_animate_controls();
			show_hit_box_controls();
            break;
    }
}

/* =======================================================

      Start and Stop Tabs
      
======================================================= */

void tabs_start(void)
{

    int							n;
	char						tab_names[6][32]={"Meshes","Vertexes","Bones","Poses","Animations","Hit Boxes"};
	Rect						box,tabbox;
    ControlTabEntry				tabarray[6];
    
		// get bounds
	
	GetWindowPortBounds(model_wind,&tabbox);

	tabbox.left+=(gl_view_x_sz+10);
	tabbox.right-=4;
	tabbox.top+=6;
	tabbox.bottom-=(info_palette_height+8);
/*    
        // tabs
        
	for (n=0;n!=6;n++) {
		tabarray[n].icon=NULL;
		tabarray[n].name=CFStringCreateWithCString(kCFAllocatorDefault,tab_names[n],kCFStringEncodingMacRoman);
		tabarray[n].enabled=TRUE;
	}
	
	CreateTabsControl(model_wind,&tabbox,kControlTabSizeSmall,kControlTabDirectionNorth,6,tabarray,&tab_list);
	Draw1Control(tab_list);
	
	for (n=0;n!=6;n++) {
		CFRelease(tabarray[n].name);
	}
	*/
		// control area
		
	SetRect(&box,(tabbox.left+4),(tabbox.top+25),(tabbox.right-4),(tabbox.bottom-6));
	
	
	
	GetWindowPortBounds(model_wind,&box);

	box.left+=(gl_view_x_sz+mesh_palette_width);
//	tabbox.right-=4;
//	tabbox.top+=6;
	box.bottom-=info_palette_height;
	
		// mesh controls
		
	start_mesh_controls(model_wind,&box);
	
		// vertex controls
		
	start_vertex_controls(model_wind,&box);
	hide_vertex_controls();
	
        // bone controls
        
	start_bone_controls(model_wind,&box);
	hide_bone_controls();
	
        // pose controls
        
	start_pose_controls(model_wind,&box);
	hide_pose_controls();
    
        // animation controls
        
    start_animate_controls(model_wind,&box);
    hide_animate_controls();
	
        // hit box controls
        
    start_hit_box_controls(model_wind,&box);
    hide_hit_box_controls();
	
		// make sure keyboard focus is on mesh tab
		
	show_mesh_controls();
}

void tabs_end(void)
{
	end_mesh_controls();
	end_vertex_controls();
	end_bone_controls();
	end_pose_controls();
    end_animate_controls();
	end_hit_box_controls();
/*
    DisposeControl(tab_list);
	*/
}

/* =======================================================

      Tabs Resize
      
======================================================= */

void tabs_resize(void)
{
/*
	Rect			tabbox,box;
	
		// get bounds
	
	GetWindowPortBounds(model_wind,&tabbox);

	tabbox.left+=(gl_view_x_sz+10);
	tabbox.right-=4;
	tabbox.top+=6;
	tabbox.bottom-=(info_palette_height+8);
    
        // tabs
        
	SizeControl(tab_list,(tabbox.right-tabbox.left),(tabbox.bottom-tabbox.top));
	
		// other controls

	SetRect(&box,(tabbox.left+4),(tabbox.top+25),(tabbox.right-4),(tabbox.bottom-6));

	resize_mesh_controls(&box);
	resize_vertex_controls(&box);
	resize_bone_controls(&box);
	resize_pose_controls(&box);
	resize_animate_controls(&box);
    resize_hit_box_controls(&box);
	*/
}


