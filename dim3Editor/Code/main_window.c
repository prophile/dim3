/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Main Map Window

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
extern bool				map_opened;

extern map_type			map;
extern setup_type		setup;

int						main_wind_view,main_wind_panel_focus,main_wind_perspective,
						vertex_mode,drag_mode,grid_mode,obscure_mesh_idx;
bool					select_toggle_mode,dp_auto_texture,dp_liquid,
						dp_object,dp_lightsoundparticle,dp_node,dp_textured,dp_y_hide,
						swap_panel_forward,swap_panel_side,swap_panel_top;
Rect					main_wind_box;

WindowRef				mainwind;
EventHandlerRef			main_wind_event;
EventHandlerUPP			main_wind_upp;
ControlRef				tool_ctrl[tool_count],piece_ctrl[piece_count],
						group_combo,magnify_slider;
ControlActionUPP		magnify_proc;
IconSuiteRef			tool_icon[tool_count],piece_icon[piece_count];
MenuRef					group_menu;
Rect					group_box;

AGLContext				ctx;

char					tool_tooltip_str[tool_count][64]=
								{
										"Move Non-Mesh Points Freely\nSwith Mode with Q",
										"Move Non-Mesh Points Together\nSwith Mode with Q",
										"Snap Non-Mesh Points\nSwith Mode with Q",
										"Select Toggle Mode",
										"Edit Entire Mesh\nSwitch Mode with W or Middle Button",
										"Edit Polygon Only\nSwitch Mode with W or Middle Button",
										"Edit Vertex Only\nSwitch Mode with W or Middle Button",
										"No Grid\nSwitch Mode with E",
										"Small Grid\nSwitch Mode with E",
										"Medium Grid\nSwitch Mode with E",
										"Large Grid\nSwitch Mode with E",
										"Combine Meshes","Split Meshes","Tesselate Mesh","Auto-Texture Meshes",
										"Show/Hide Liquids","Show/Hide Script Spots/Scenery",
										"Show/Hide Light/Sound/Particles","Show/Hide Nodes",
										"Test Mesh Obscuring","Edit Map Script","Run Map In Engine",
									};
									
char					piece_tooltip_str[piece_count][64]=
									{
										"Add Script Spot","Add Light","Add Sound",
										"Add Particle","Add Scenery","Add Node",
										"Add OBJ Mesh","Add Height Map Mesh","Add Grid Mesh",
										"Add Single Polygon Mesh","Add Liquid Volume"
									};

/* =======================================================

      Tool Window Events
      
======================================================= */

void main_wind_control_tool(int tool_idx)
{
	undo_clear();
	
	switch (tool_idx) {
	
			// vertex mode
			
		case 0:
			vertex_mode=vertex_mode_none;
			SetControlValue(tool_ctrl[0],1);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],0);
			break;
		case 1:
			vertex_mode=vertex_mode_lock;
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],1);
			SetControlValue(tool_ctrl[2],0);
			break;
		case 2:
			vertex_mode=vertex_mode_snap;
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],1);
			break;
			
			// select toggle mode
			
		case 3:
			select_toggle_mode=!select_toggle_mode;
			break;
			
			// drag mode buttons
			
		case 4:
			drag_mode=drag_mode_mesh;
			SetControlValue(tool_ctrl[4],1);
			SetControlValue(tool_ctrl[5],0);
			SetControlValue(tool_ctrl[6],0);
			break;
			
		case 5:
			drag_mode=drag_mode_polygon;
			SetControlValue(tool_ctrl[4],0);
			SetControlValue(tool_ctrl[5],1);
			SetControlValue(tool_ctrl[6],0);
			break;
			
		case 6:
			drag_mode=drag_mode_vertex;
			SetControlValue(tool_ctrl[4],0);
			SetControlValue(tool_ctrl[5],0);
			SetControlValue(tool_ctrl[6],1);
			break;
			
			// grid mode buttons
			
		case 7:
			grid_mode=grid_mode_none;
			SetControlValue(tool_ctrl[7],1);
			SetControlValue(tool_ctrl[8],0);
			SetControlValue(tool_ctrl[9],0);
			SetControlValue(tool_ctrl[10],0);
			break;
			
		case 8:
			grid_mode=grid_mode_small;
			SetControlValue(tool_ctrl[7],0);
			SetControlValue(tool_ctrl[8],1);
			SetControlValue(tool_ctrl[9],0);
			SetControlValue(tool_ctrl[10],0);
			break;
			
		case 9:
			grid_mode=grid_mode_medium;
			SetControlValue(tool_ctrl[7],0);
			SetControlValue(tool_ctrl[8],0);
			SetControlValue(tool_ctrl[9],1);
			SetControlValue(tool_ctrl[10],0);
			break;

		case 10:
			grid_mode=grid_mode_large;
			SetControlValue(tool_ctrl[7],0);
			SetControlValue(tool_ctrl[8],0);
			SetControlValue(tool_ctrl[9],0);
			SetControlValue(tool_ctrl[10],1);
			break;
			
			// tesselate and auto-texture
			
		case 11:
			SetControlValue(tool_ctrl[tool_idx],0);
			piece_combine_mesh();
			break;
			
		case 12:
			SetControlValue(tool_ctrl[tool_idx],0);
			piece_split_mesh();
			break;

		case 13:
			SetControlValue(tool_ctrl[tool_idx],0);
			piece_tesselate();
			break;
			
		case 14:
			dp_auto_texture=!dp_auto_texture;
			break;
			
			// show/hide
			
		case 15:
			select_clear();
			dp_liquid=!dp_liquid;
			break;
			
		case 16:
			select_clear();
			dp_object=!dp_object;
			break;
			
		case 17:
			select_clear();
			dp_lightsoundparticle=!dp_lightsoundparticle;
			break;
			
		case 18:
			select_clear();
			dp_node=!dp_node;
			break;
						
			// obscure, script and run buttons
			
		case 19:
			if (obscure_test()) {
				SetControlValue(tool_ctrl[tool_idx],1);
			}
			else {
				SetControlValue(tool_ctrl[tool_idx],0);
			}
			break;
	
		case 20:
			SetControlValue(tool_ctrl[tool_idx],0);
			launch_map_script_editor();
			break;
			
		case 21:
			SetControlValue(tool_ctrl[tool_idx],0);
			launch_engine();
			break;
			
	}
	
	menu_fix_enable();
	main_wind_draw();
}

void main_wind_control_piece(int piece_idx)
{
	switch (piece_idx) {

		case 0:
			piece_create_spot();
			break;
			
		case 1:
			piece_create_light();
			break;
			
		case 2:
			piece_create_sound();
			break;
			
		case 3:
			piece_create_particle();
			break;
			
		case 4:
			piece_create_scenery();
			break;
			
		case 5:
			piece_create_node();
			break;
			
		case 6:
			piece_add_obj_mesh();
			break;
			
		case 7:
			piece_add_height_map_mesh();
			break;
			
		case 8:
			piece_add_grid_mesh();
			break;
			
		case 9:
			piece_add_polygon_mesh();
			break;
			
		case 10:
			piece_create_liquid();
			break;
			
	}
	
	SetControlValue(piece_ctrl[piece_idx],0);
}

void main_wind_control(ControlRef ctrl)
{
	int				i,idx,tool_idx,piece_idx;
	
		// group combo
		
	if (ctrl==group_combo) {
		idx=GetControl32BitValue(ctrl);
		
		if (idx==1) {
			group_set(-1);
		}
		else {
			group_set(idx-3);
		}
		
		main_wind_tool_fill_group_combo();
		return;
	}
		
		// find button
		
	tool_idx=-1;
	piece_idx=-1;
	
	for (i=0;i!=tool_count;i++) {
		if (ctrl==tool_ctrl[i]) {
			tool_idx=i;
			break;
		}
	}
	
	if (tool_idx==-1) {
		
		for (i=0;i!=piece_count;i++) {
			if (ctrl==piece_ctrl[i]) {
				piece_idx=i;
				break;
			}
		}
	}
	
	if (tool_idx!=-1) {
		main_wind_control_tool(tool_idx);
		return;
	}
	
	if (piece_idx!=-1) {
		main_wind_control_piece(piece_idx);
		return;
	}
}

/* =======================================================

      Magnify Slider Action
      
======================================================= */

void main_wind_magnify_action(ControlRef ctrl,ControlPartCode code)
{
	int				s;
	
	s=GetControlValue(ctrl);
	if (s==magnify_factor) return;
	
	magnify_factor=s;
    main_wind_draw();
}

/* =======================================================

      World Window Events
      
======================================================= */

OSStatus main_wind_event_callback(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	unsigned long		modifiers,nclick;
	unsigned short		btn;
	long				delta;
	char				ch;
	Point				pt;
	Rect				wbox;
	EventMouseWheelAxis	axis;
	ControlRef			ctrl;
	d3pnt				dpt;
	
	switch (GetEventClass(event)) {
	
		case kEventClassWindow:
		
			switch (GetEventKind(event)) {
			
				case kEventWindowDrawContent:
					main_wind_draw();
					DrawControls(mainwind);
					return(noErr);
					
                case kEventWindowCursorChange:
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);
                    GlobalToLocal(&pt);
 					dpt.x=pt.h;
					dpt.y=pt.v;
					main_wind_cursor(&dpt);
                    return(noErr);

				case kEventWindowClickContentRgn:
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);

                    SetPort(GetWindowPort(mainwind));
                    GlobalToLocal(&pt);
					
					GetWindowPortBounds(mainwind,&wbox);
					
						// click in toolbars

					if ((pt.v<toolbar_high) || (pt.h>(wbox.right-piece_wid))) return(eventNotHandledErr);

						// middle button vertex change mode
						
					GetEventParameter(event,kEventParamMouseButton,typeMouseButton,NULL,sizeof(unsigned short),NULL,&btn);
					
					if (btn==kEventMouseButtonTertiary) {
						main_wind_tool_switch_drag_mode();
						return(noErr);
					}
	
						// click in texture palette
						
					GetEventParameter(event,kEventParamClickCount,typeUInt32,NULL,sizeof(unsigned long),NULL,&nclick);
					
					if ((pt.v>=txt_palette_y) && (pt.v<=(txt_palette_y+txt_palette_high))) {
						texture_palette_click(pt,(nclick!=1));
						return(noErr);
					}
					
						// click in main window
						
					dpt.x=pt.h;
					dpt.y=pt.v;
					
                    if (main_wind_click(&dpt,(nclick!=1))) return(noErr);
                    
                    return(eventNotHandledErr);
					
				case kEventWindowBoundsChanged:
				case kEventWindowResizeCompleted:
					main_wind_resize();
					return(noErr);
				
				case kEventWindowClose:
					if (map_opened) {
						file_close_map();
					}
					return(noErr);
					
			}
			break;
			
		case kEventClassKeyboard:
		
			switch (GetEventKind(event)) {
			
				case kEventRawKeyDown:
				case kEventRawKeyRepeat:
					GetEventParameter(event,kEventParamKeyMacCharCodes,typeChar,NULL,sizeof(char),NULL,&ch);
					main_wind_key_down(ch);
					main_wind_key_cursor();
					return(noErr);
				
				case kEventRawKeyUp:
					GetEventParameter(event,kEventParamKeyMacCharCodes,typeChar,NULL,sizeof(char),NULL,&ch);
					main_wind_key_cursor();
					return(noErr);
                   
                case kEventRawKeyModifiersChanged:
 					GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(unsigned long),NULL,&modifiers);
					main_wind_key_cursor();
                    return(noErr);
					
			}
			break;
			
		case kEventClassMouse:
		
			switch (GetEventKind(event)) {
			
				case kEventMouseWheelMoved:
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);
					SetPort(GetWindowPort(mainwind));
                    GlobalToLocal(&pt);
					
					dpt.x=pt.h;
					dpt.y=pt.v;
					
					GetEventParameter(event,kEventParamMouseWheelAxis,typeMouseWheelAxis,NULL,sizeof(EventMouseWheelAxis),NULL,&axis);
					if (axis==kEventMouseWheelAxisY) {
						GetEventParameter(event,kEventParamMouseWheelDelta,typeLongInteger,NULL,sizeof(long),NULL,&delta);
						main_wind_scroll_wheel(&dpt,delta);
					}
					return(noErr);
					
			}
			break;
			
		case kEventClassControl:
			GetEventParameter(event,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&ctrl);
			main_wind_control(ctrl);
			return(noErr);
			
	}

	return(eventNotHandledErr);
}

/* =======================================================

      Main Window Magnify
      
======================================================= */

void main_wind_magnify_scroll(int delta)
{
	magnify_factor-=delta;
	if (magnify_factor<magnify_factor_min) magnify_factor=magnify_factor_min;
	if (magnify_factor>magnify_factor_max) magnify_factor=magnify_factor_max;
	
	SetControlValue(magnify_slider,magnify_factor);
	
    main_wind_draw();
}

/* =======================================================

      Main Window Viewport Coordinates
      
======================================================= */

void main_wind_setup(void)
{
	Rect			wbox;
	GLint			rect[4];
	
		// setup view box
		
	GetWindowPortBounds(mainwind,&wbox);

	main_wind_box.left=0;
	main_wind_box.right=wbox.right-piece_wid;
	main_wind_box.top=toolbar_high;
	main_wind_box.bottom=wbox.bottom-info_high;
	
		// buffer rect clipping
		
	rect[0]=0;
	rect[1]=info_high;
	rect[2]=(wbox.right-wbox.left)-piece_wid;
	rect[3]=(wbox.bottom-wbox.top)-(toolbar_high+info_high);
	
	aglSetInteger(ctx,AGL_BUFFER_RECT,rect);
	aglEnable(ctx,AGL_BUFFER_RECT);
}

/* =======================================================

      Open & Close Main Window
      
======================================================= */

void main_wind_open(void)
{
	int							n;
	Rect						wbox,box;
	GLint						attrib[]={AGL_NO_RECOVERY,AGL_RGBA,AGL_DOUBLEBUFFER,AGL_ACCELERATED,AGL_PIXEL_SIZE,24,AGL_ALPHA_SIZE,8,AGL_DEPTH_SIZE,16,AGL_STENCIL_SIZE,8,AGL_NONE};
	GDHandle					gdevice;
	AGLPixelFormat				pf;
	IconFamilyHandle			iconfamily;
	ControlButtonContentInfo	icon_info;
	HMHelpContentRec			tag;
	EventTypeSpec				wind_events[]={	{kEventClassWindow,kEventWindowDrawContent},
												{kEventClassWindow,kEventWindowCursorChange},
												{kEventClassWindow,kEventWindowClickContentRgn},
												{kEventClassWindow,kEventWindowBoundsChanged},
												{kEventClassWindow,kEventWindowResizeCompleted},
												{kEventClassWindow,kEventWindowClose},
												{kEventClassKeyboard,kEventRawKeyDown},
												{kEventClassKeyboard,kEventRawKeyUp},
												{kEventClassKeyboard,kEventRawKeyRepeat},
												{kEventClassKeyboard,kEventRawKeyModifiersChanged},
												{kEventClassMouse,kEventMouseWheelMoved},
												{kEventClassControl,kEventControlHit}};
												
        // open window
        
    GetAvailableWindowPositioningBounds(GetMainDevice(),&wbox);

	SetRect(&wbox,(wbox.left+3),(wbox.top+25),(wbox.right-3),(wbox.bottom-5));
	CreateNewWindow(kDocumentWindowClass,kWindowCloseBoxAttribute|kWindowCollapseBoxAttribute|kWindowFullZoomAttribute|kWindowResizableAttribute|kWindowLiveResizeAttribute|kWindowStandardHandlerAttribute|kWindowInWindowMenuAttribute,&wbox,&mainwind);
	
		// toolbar controls
		
	SetRect(&box,0,0,tool_button_size,tool_button_size);
    
	for (n=0;n!=tool_count;n++) {
	
			// create button
			
		iconfamily=(IconFamilyHandle)GetResource('icns',(500+n));
		IconFamilyToIconSuite(iconfamily,kSelectorAllAvailableData,&tool_icon[n]);
		
		icon_info.contentType=kControlContentIconSuiteHandle;
		icon_info.u.iconSuite=tool_icon[n];
		
		CreateBevelButtonControl(mainwind,&box,NULL,kControlBevelButtonSmallBevel,kControlBehaviorToggles,&icon_info,0,0,0,&tool_ctrl[n]);
		SetBevelButtonGraphicAlignment(tool_ctrl[n],kControlBevelButtonAlignCenter,0,0);
		
			// create tooltip
			
		tag.version=kMacHelpVersion;
		tag.tagSide=kHMDefaultSide;
		SetRect(&tag.absHotRect,0,0,0,0);
		tag.content[kHMMinimumContentIndex].contentType=kHMCFStringContent;
		tag.content[kHMMinimumContentIndex].u.tagCFString=CFStringCreateWithCString(NULL,tool_tooltip_str[n],kCFStringEncodingMacRoman);
		tag.content[kHMMaximumContentIndex].contentType=kHMNoContent;
		
		HMSetControlHelpContent(tool_ctrl[n],&tag);

			// next button position
			
		OffsetRect(&box,tool_button_size,0);
		if ((n==2) || (n==3) || (n==6) || (n==10) || (n==14) || (n==18) || (n==19)) OffsetRect(&box,3,0);
	}
	
		// group combo
		
	CreateNewMenu(tool_group_menu_id,kMenuAttrExcludesMarkColumn,&group_menu);
	InsertMenu(group_menu,kInsertHierarchicalMenu);
	
	group_box.top=3;
	group_box.bottom=tool_button_size-3;
	group_box.left=wbox.right-205;
	group_box.right=wbox.right-5;
	
	CreatePopupButtonControl(mainwind,&group_box,NULL,tool_group_menu_id,FALSE,0,0,0,&group_combo);
	
	tag.version=kMacHelpVersion;
	tag.tagSide=kHMDefaultSide;
	SetRect(&tag.absHotRect,0,0,0,0);
	tag.content[kHMMinimumContentIndex].contentType=kHMCFStringContent;
	tag.content[kHMMinimumContentIndex].u.tagCFString=CFStringCreateWithCString(NULL,"Mesh Groups",kCFStringEncodingMacRoman);
	tag.content[kHMMaximumContentIndex].contentType=kHMNoContent;
		
	HMSetControlHelpContent(group_combo,&tag);
	
		// piece buttons
		
	box.left=wbox.right-(piece_button_size+3);
	box.right=box.left+piece_button_size;
	box.top=toolbar_high;
	box.bottom=box.top+piece_button_size;
	
	for (n=0;n!=piece_count;n++) {
			
			// create button
			
		iconfamily=(IconFamilyHandle)GetResource('icns',(600+n));
		IconFamilyToIconSuite(iconfamily,kSelectorAllAvailableData,&piece_icon[n]);
		
		icon_info.contentType=kControlContentIconSuiteHandle;
		icon_info.u.iconSuite=piece_icon[n];
		
		CreateBevelButtonControl(mainwind,&box,NULL,kControlBevelButtonSmallBevel,kControlBehaviorToggles,&icon_info,0,0,0,&piece_ctrl[n]);
		SetBevelButtonGraphicAlignment(piece_ctrl[n],kControlBevelButtonAlignCenter,0,0);

			// create tooltip
			
		tag.version=kMacHelpVersion;
		tag.tagSide=kHMOutsideLeftCenterAligned;
		SetRect(&tag.absHotRect,0,0,0,0);
		tag.content[kHMMinimumContentIndex].contentType=kHMCFStringContent;
		tag.content[kHMMinimumContentIndex].u.tagCFString=CFStringCreateWithCString(NULL,piece_tooltip_str[n],kCFStringEncodingMacRoman);
		tag.content[kHMMaximumContentIndex].contentType=kHMNoContent;
		
		HMSetControlHelpContent(piece_ctrl[n],&tag);
		
		OffsetRect(&box,0,piece_button_size);
    }
	
		// magnify slider
		// this needs to be fixed, a texture size is hard coded

	box.left=wbox.right-(piece_button_size+3);
	box.right=box.left+piece_button_size;
	box.left+=8;
	box.right-=8;
	box.top=(toolbar_high+5)+(piece_button_size*piece_count);
	box.bottom=wbox.bottom-(115+info_high);
	
	magnify_proc=NewControlActionUPP(main_wind_magnify_action);
	CreateSliderControl(mainwind,&box,magnify_factor_default,magnify_factor_min,magnify_factor_max,kControlSliderDoesNotPoint,0,TRUE,magnify_proc,&magnify_slider);

		// show window before additional setup
		
	ShowWindow(mainwind);
   
	SetPort(GetWindowPort(mainwind));
		
	TextFont(FMGetFontFamilyFromName("\pMonaco"));
	TextSize(10);
	
		// opengl setup
		
	gdevice=GetMainDevice();

	pf=aglChoosePixelFormat(&gdevice,1,attrib);
	ctx=aglCreateContext(pf,NULL);
	aglSetCurrentContext(ctx);
	aglDestroyPixelFormat(pf);

	glDisable(GL_SMOOTH);
	glDisable(GL_DITHER);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
	
	aglSetDrawable(ctx,(AGLDrawable)GetWindowPort(mainwind));

		// setup view sizes
		
	texture_palette_setup();
	main_wind_setup();
	walk_view_initialize();
   
        // misc setup
        
	walk_view_y_angle=0.0f;
	walk_view_x_angle=0.0f;
	
	swap_panel_forward=swap_panel_side=swap_panel_top=FALSE;
	
	drag_mode=drag_mode_mesh;
	grid_mode=grid_mode_small;
	select_toggle_mode=FALSE;
	
	obscure_mesh_idx=-1;
	
        // events
    
	main_wind_upp=NewEventHandlerUPP(main_wind_event_callback);
	InstallEventHandler(GetWindowEventTarget(mainwind),main_wind_upp,GetEventTypeCount(wind_events),wind_events,NULL,&main_wind_event);
}

void main_wind_close(void)
{
	int			n;
	
		// gl shutdown
		
	aglSetCurrentContext(NULL);
	aglSetDrawable(ctx,NULL);
	aglDestroyContext(ctx);
	
        // close walkview icons
        
	walk_view_shutdown();
	
		// dispose controls

	for (n=0;n!=tool_count;n++) {
		DisposeControl(tool_ctrl[n]);
		DisposeIconSuite(tool_icon[n],TRUE);
	}
	
	DisposeControl(group_combo);
	DisposeMenu(group_menu);
	
	for (n=0;n!=piece_count;n++) {
		DisposeControl(piece_ctrl[n]);
		DisposeIconSuite(piece_icon[n],TRUE);
	}
	
	DisposeControl(magnify_slider);
	DisposeControlActionUPP(magnify_proc);

        // dispose of events and window
        
	RemoveEventHandler(main_wind_event);
	DisposeEventHandlerUPP(main_wind_upp);
	DisposeWindow(mainwind);
}

void main_wind_set_title(char *file_name)
{
	char			*c,wname[256];
	unsigned char	p_str[256];
	
	strcpy(wname,file_name);
	c=strchr(wname,'.');
	if (c!=NULL) *c=0x0;
	
	CopyCStringToPascal(wname,p_str);
	SetWTitle(mainwind,p_str);
}

/* =======================================================

      Resizing
      
======================================================= */

void main_wind_resize_buttons(void)
{
	int				n,x,y;
	Rect			wbox,box,group_box;
	
	GetWindowPortBounds(mainwind,&wbox);
	
		// group combo
		
	group_box.top=3;
	group_box.bottom=tool_button_size-3;
	group_box.left=wbox.right-205;
	group_box.right=wbox.right-5;
	
	SetControlBounds(group_combo,&group_box);
	
		// pieces
	
	x=wbox.right-piece_button_size;
	y=toolbar_high;
	
	for (n=0;n!=piece_count;n++) {
		MoveControl(piece_ctrl[n],x,y);
		y+=piece_button_size;
	}
	
		// magnify control
		
	box.left=wbox.right-(piece_button_size+3);
	box.right=box.left+piece_button_size;
	box.left+=8;
	box.right-=8;
	box.top=(toolbar_high+5)+(piece_button_size*piece_count);
	box.bottom=wbox.bottom-(txt_palette_high+info_high+5);
	
	SetControlBounds(magnify_slider,&box);
}

void main_wind_resize(void)
{
	Rect			wbox;
	CGrafPtr		saveport;
	
		// erase window
		
	GetPort(&saveport);
	SetPort(GetWindowPort(mainwind));
    
	GetWindowPortBounds(mainwind,&wbox);
	EraseRect(&wbox);
	
	SetPort(saveport);

		// fix all views and palettes

	texture_palette_setup();
	main_wind_setup();
	main_wind_resize_buttons();
	DrawControls(mainwind);
	main_wind_set_view(main_wind_view);
	main_wind_draw();
}

/* =======================================================

      Views
      
======================================================= */

void main_wind_set_view(int view)
{
	main_wind_view=view;
	main_wind_panel_focus=kf_panel_top;
	
	menu_set_view_check(view);
}

void main_wind_set_perspective(int perspective)
{
	main_wind_perspective=perspective;
	menu_set_perspective_check(perspective);
}

/* =======================================================

      Viewport and Projection Setup
      
======================================================= */

void main_wind_set_viewport(Rect *view_box,bool erase,bool use_background)
{
	int				bot_y;
	Rect			wbox;
	
		// set viewport
		
	GetWindowPortBounds(mainwind,&wbox);
	bot_y=wbox.bottom-info_high;

	glEnable(GL_SCISSOR_TEST);
	glScissor(view_box->left,(bot_y-view_box->bottom),(view_box->right-view_box->left),(view_box->bottom-view_box->top));

	glViewport(view_box->left,(bot_y-view_box->bottom),(view_box->right-view_box->left),(view_box->bottom-view_box->top));
	
		// default setup
		
	glDisable(GL_DEPTH_TEST);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)view_box->left,(GLdouble)view_box->right,(GLdouble)view_box->bottom,(GLdouble)view_box->top,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		
		// erase viewport
		
	if (!erase) return;
		
	if (use_background) {
		glColor4f(setup.col.background.r,setup.col.background.g,setup.col.background.b,1.0f);
	}
	else {
		glColor4f(1.0f,1.0f,1.0f,1.0f);
	}
	
	glBegin(GL_QUADS);
	glVertex2i(view_box->left,view_box->top);
	glVertex2i(view_box->right,view_box->top);
	glVertex2i(view_box->right,view_box->bottom);
	glVertex2i(view_box->left,view_box->bottom);
	glEnd();
}

void main_wind_set_2D_projection(Rect *view_box)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho((GLdouble)view_box->left,(GLdouble)view_box->right,(GLdouble)view_box->bottom,(GLdouble)view_box->top,-1.0,1.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void main_wind_set_3D_projection(Rect *view_box,d3ang *ang,float fov,float near_z,float far_z,float near_z_offset)
{
	int				x_sz,y_sz;
	float			ratio;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	if (main_wind_perspective==ps_perspective) {
		ratio=(float)(view_box->right-view_box->left)/(float)(view_box->bottom-view_box->top);
		gluPerspective(fov,ratio,near_z,far_z);
	}
	else {
		x_sz=(view_box->right-view_box->left)*(map_enlarge>>2);
		y_sz=(view_box->bottom-view_box->top)*(map_enlarge>>2);
		glOrtho((GLdouble)-x_sz,(GLdouble)x_sz,(GLdouble)-y_sz,(GLdouble)y_sz,near_z,far_z);
	}
	
	glScalef(-1,-1,1);						// x and y are reversed
	glTranslatef(0,0,near_z_offset);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glRotatef(ang->x,1,0,0);
	glRotatef(angle_add(ang->y,180.0f),0,1,0);
}

/* =======================================================

      Divider Drawing
      
======================================================= */

void main_wind_draw_dividers_single(Rect *box,bool selected)
{
	glColor4f((selected?1.0f:0.0f),0.0f,0.0f,1.0f);
	
	glBegin(GL_LINE_LOOP);
	glVertex2i((box->left-1),(box->top-1));
	glVertex2i((box->right+1),(box->top-1));
	glVertex2i((box->right+1),(box->bottom+1));
	glVertex2i((box->left-1),(box->bottom+1));
	glEnd();
}

void main_wind_draw_3_panel_dividers(void)
{
	editor_3D_view_setup	view_setup;
	
		// use full view port
	
	main_wind_set_viewport(&main_wind_box,FALSE,FALSE);
	
		// draw dividers

	glLineWidth(2.0f);
	
	switch (main_wind_panel_focus) {
	
		case kf_panel_forward:
			main_wind_setup_panel_side(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_top(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_forward(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,TRUE);
			break;
			
		case kf_panel_side:
			main_wind_setup_panel_forward(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_top(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_side(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,TRUE);
			break;
			
		case kf_panel_top:
			main_wind_setup_panel_forward(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_side(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_top(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,TRUE);
			break;
			
	}
	
	glLineWidth(1.0f);
}

void main_wind_draw_4_panel_dividers(void)
{
	editor_3D_view_setup	view_setup;
	
		// use full view port
	
	main_wind_set_viewport(&main_wind_box,FALSE,FALSE);
	
		// draw dividers

	glLineWidth(2.0f);
	
	switch (main_wind_panel_focus) {
	
		case kf_panel_forward:
			main_wind_setup_panel_side_frame(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_top_frame(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_walk(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
		
			main_wind_setup_panel_forward_frame(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,TRUE);
			break;
			
		case kf_panel_side:
			main_wind_setup_panel_forward_frame(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_top_frame(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_walk(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_side_frame(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,TRUE);
			break;
			
		case kf_panel_top:
			main_wind_setup_panel_forward_frame(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_side_frame(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_walk(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_top_frame(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,TRUE);
			break;
			
		case kf_panel_walk:
			main_wind_setup_panel_forward_frame(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_side_frame(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_top_frame(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,FALSE);
			
			main_wind_setup_panel_walk(&view_setup);
			main_wind_draw_dividers_single(&view_setup.box,TRUE);
			break;
			
	}
	
	glLineWidth(1.0f);
}

/* =======================================================

      View Drawing
      
======================================================= */

void main_wind_draw(void)
{
	Rect					wbox;
	editor_3D_view_setup	view_setup;

	GetWindowPortBounds(mainwind,&wbox);
	
		// clear gl buffer
		
	glDisable(GL_SCISSOR_TEST);
	
	glClearColor(1.0f,1.0f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
        // the views
		
	switch (main_wind_view) {
	
		case vw_3_panel:
			main_wind_setup_panel_forward(&view_setup);
			walk_view_draw(&view_setup,FALSE);
			walk_view_swap_draw(&view_setup);
			
			main_wind_setup_panel_side(&view_setup);
			walk_view_draw(&view_setup,FALSE);
			walk_view_swap_draw(&view_setup);
			
			main_wind_setup_panel_top(&view_setup);
			walk_view_draw(&view_setup,TRUE);

			main_wind_draw_3_panel_dividers();
			break;
			
		case vw_4_panel:
			main_wind_setup_panel_forward_frame(&view_setup);
			walk_view_draw(&view_setup,FALSE);
			walk_view_swap_draw(&view_setup);
			
			main_wind_setup_panel_side_frame(&view_setup);
			walk_view_draw(&view_setup,FALSE);
			walk_view_swap_draw(&view_setup);
			
			main_wind_setup_panel_top_frame(&view_setup);
			walk_view_draw(&view_setup,TRUE);
			
			main_wind_setup_panel_walk(&view_setup);
			walk_view_draw(&view_setup,FALSE);
			walk_view_compass_draw(&view_setup);
			
			main_wind_draw_4_panel_dividers();
			break;
			
		case vw_top_only:
			main_wind_setup_panel_top_full(&view_setup);
			walk_view_draw(&view_setup,TRUE);
			break;
			
		case vw_forward_only:
			main_wind_setup_panel_forward_full(&view_setup);
			walk_view_draw(&view_setup,FALSE);
			walk_view_compass_draw(&view_setup);
			break;
	}
	
		// texture window
		
	texture_palette_draw();
	
		// swap GL buffer
		
	aglSwapBuffers(ctx);
	
		// status line
		
	info_status_line_draw();
}

/* =======================================================

      Change Keyboard Focus
	        
======================================================= */

void main_wind_set_focus(int focus)
{
	if (main_wind_panel_focus!=focus) {
		main_wind_panel_focus=focus;
		main_wind_draw();
	}
}

/* =======================================================

      Center Position in Map
	        
======================================================= */

void main_wind_center_position_in_map(void)
{
	int					n;
	d3pnt				pt;
	
		// view angles
		
	walk_view_y_angle=0.0f;
	walk_view_x_angle=0.0f;
	
		// look for player spot first
		
	for (n=0;n!=map.nspot;n++) {
		if ((strcasecmp(map.spots[n].name,"start")==0) || (strcasecmp(map.spots[n].script,"player")==0)) {
			cx=map.spots[n].pnt.x;
			cy=map.spots[n].pnt.y-(map_enlarge*20);
			cz=map.spots[n].pnt.z;
			return;
		}
	}

		// otherwise do first mesh with vertexes
		
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (map.mesh.meshes[n].nvertex!=0) {
			map_mesh_calculate_center(&map,n,&pt);
			cx=pt.x;
			cy=pt.y;
			cz=pt.z;
			return;
		}
	}
	
		// just center in total map size
		
	cx=map_max_size/2;
	cy=map_max_size/2;
	cz=map_max_size/2;
}

/* =======================================================

      Clicking
      
======================================================= */

bool main_wind_click_check_box(d3pnt *pt,Rect *box)
{
	if (pt->x<box->left) return(FALSE);
	if (pt->x>box->right) return(FALSE);
	if (pt->y<box->top) return(FALSE);
	return(pt->y<=box->bottom);
}

bool main_wind_click(d3pnt *pt,bool dblclick)
{
	editor_3D_view_setup	view_setup;

	switch (main_wind_view) {
	
		case vw_3_panel:
			main_wind_setup_panel_forward(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_forward);
				if (walk_view_swap_click(&view_setup,pt,&swap_panel_forward)) return(TRUE);
				walk_view_click(&view_setup,pt,vm_dir_forward,FALSE,dblclick);
				return(TRUE);
			}
			
			main_wind_setup_panel_side(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_side);
				if (walk_view_swap_click(&view_setup,pt,&swap_panel_side)) return(TRUE);
				walk_view_click(&view_setup,pt,vm_dir_side,FALSE,dblclick);
				return(TRUE);
			}
			
			main_wind_setup_panel_top(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_top);
				walk_view_click(&view_setup,pt,vm_dir_top,FALSE,dblclick);
				return(TRUE);
			}
			break;
			
		case vw_4_panel:
			main_wind_setup_panel_forward_frame(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_forward);
				if (walk_view_swap_click(&view_setup,pt,&swap_panel_forward)) return(TRUE);
				walk_view_click(&view_setup,pt,vm_dir_forward,FALSE,dblclick);
				return(TRUE);
			}
			
			main_wind_setup_panel_side_frame(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_side);
				if (walk_view_swap_click(&view_setup,pt,&swap_panel_side)) return(TRUE);
				walk_view_click(&view_setup,pt,vm_dir_side,FALSE,dblclick);
				return(TRUE);
			}
			
			main_wind_setup_panel_top_frame(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_top);
				walk_view_click(&view_setup,pt,vm_dir_top,FALSE,dblclick);
				return(TRUE);
			}
			
			main_wind_setup_panel_walk(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_walk);
				if (!walk_view_compass_click(&view_setup,pt)) {
					walk_view_click(&view_setup,pt,vm_dir_forward,FALSE,dblclick);
				}
				return(TRUE);
			}
			break;
			
		case vw_top_only:
			main_wind_setup_panel_top_full(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				walk_view_click(&view_setup,pt,vm_dir_top,FALSE,dblclick);
				return(TRUE);
			}
			break;
			
		case vw_forward_only:
			main_wind_setup_panel_forward_full(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				if (!walk_view_compass_click(&view_setup,pt)) {
					walk_view_click(&view_setup,pt,vm_dir_forward,TRUE,dblclick);
				}
				return(TRUE);
			}
			break;
			
	}
	    
    return(FALSE);
}

/* =======================================================

      Key Presses
      
======================================================= */

bool main_wind_space_down(void)
{
	KeyMap			map;
	unsigned char	*c;
	
	GetKeys(map);
	c=(unsigned char*)map;
	
	return((c[6]&0x02)!=0);
}

bool main_wind_option_down(void)
{
	return((GetCurrentKeyModifiers()&optionKey)!=0);
}

bool main_wind_control_down(void)
{
	return((GetCurrentKeyModifiers()&controlKey)!=0);
}

bool main_wind_command_down(void)
{
	return((GetCurrentKeyModifiers()&cmdKey)!=0);
}

bool main_wind_shift_down(void)
{
	return((GetCurrentKeyModifiers()&shiftKey)!=0);
}

/* =======================================================

      Cursors
      
======================================================= */

void main_wind_cursor(d3pnt *pt)
{
	editor_3D_view_setup	view_setup;

	switch (main_wind_view) {
	
		case vw_3_panel:
			main_wind_setup_panel_forward(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) walk_view_cursor(FALSE);
			
			main_wind_setup_panel_side(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) walk_view_cursor(FALSE);
			
			main_wind_setup_panel_top(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) walk_view_cursor(FALSE);
			break;
			
		case vw_4_panel:
			main_wind_setup_panel_forward_frame(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) walk_view_cursor(FALSE);
			
			main_wind_setup_panel_side_frame(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) walk_view_cursor(FALSE);
			
			main_wind_setup_panel_top_frame(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) walk_view_cursor(FALSE);
			break;
			
		case vw_top_only:
			main_wind_setup_panel_top_full(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) walk_view_cursor(FALSE);
			break;
			
		case vw_forward_only:
			main_wind_setup_panel_forward_full(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) walk_view_cursor(TRUE);
			break;
	}
}

/* =======================================================

      Key States
      
======================================================= */

void main_wind_key_cursor(void)
{
	Point			pt;
	d3pnt			dpt;
	
	GetMouse(&pt);
	GlobalToLocal(&pt);
	dpt.x=pt.h;
	dpt.y=pt.v;
	
	main_wind_cursor(&dpt);
}

void main_wind_key_down(char ch)
{
	editor_3D_view_setup	view_setup;

		// tab key switches panes
		
	if (ch==0x9) {
	
		switch (main_wind_view) {
			case vw_3_panel:
				main_wind_panel_focus++;
				if (main_wind_panel_focus==3) main_wind_panel_focus=0;
				break;
			case vw_4_panel:
				main_wind_panel_focus++;
				if (main_wind_panel_focus==4) main_wind_panel_focus=0;
				break;
		}
		
		main_wind_draw();
		return;
	}
	
		// esc key deselects
		
	if (ch==0x1B) {
		select_clear();
		main_wind_draw();
		return;
	}
	
		// special tool keys
		
	if ((ch=='q') || (ch=='Q')) {
		main_wind_tool_switch_vertex_mode();
		return;
	}
	
	if ((ch=='w') || (ch=='W')) {
		main_wind_tool_switch_drag_mode();
		return;
	}
	
	if ((ch=='e') || (ch=='E')) {
		main_wind_tool_switch_grid_mode();
		return;
	}
	
		// send keys to proper panel
		
	switch (main_wind_view) {
	
		case vw_3_panel:
		case vw_4_panel:
		
			switch (main_wind_panel_focus) {
			
				case kf_panel_forward:
				case kf_panel_walk:
					main_wind_setup_panel_forward(&view_setup);
					walk_view_key(&view_setup,vm_dir_forward,ch);
					break;
					
				case kf_panel_side:
					main_wind_setup_panel_side(&view_setup);
					walk_view_key(&view_setup,vm_dir_side,ch);
					break;
					
				case kf_panel_top:
					main_wind_setup_panel_top(&view_setup);
					walk_view_key(&view_setup,vm_dir_top,ch);
					break;
			}
			
			break;
			
		case vw_top_only:
			main_wind_setup_panel_top_full(&view_setup);
			walk_view_key(&view_setup,vm_dir_top,ch);
			break;
			
		case vw_forward_only:
			main_wind_setup_panel_forward_full(&view_setup);
			walk_view_key(&view_setup,vm_dir_forward,ch);
			break;
	}
}

/* =======================================================

      Scroll Wheel
      
======================================================= */

void main_wind_scroll_wheel(d3pnt *pt,int delta)
{
	editor_3D_view_setup		view_setup;
	
		// check if over a panel and switch
		// to that panel
		
	switch (main_wind_view) {
	
		case vw_3_panel:
			main_wind_setup_panel_forward(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_forward);
				break;
			}
			
			main_wind_setup_panel_side(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_side);
				break;
			}
			
			main_wind_setup_panel_top(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_top);
				break;
			}
			break;
			
		case vw_4_panel:
			main_wind_setup_panel_forward_frame(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_forward);
				break;
			}
			
			main_wind_setup_panel_side_frame(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_side);
				break;
			}
			
			main_wind_setup_panel_top_frame(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_top);
				break;
			}
			
			main_wind_setup_panel_walk(&view_setup);
			if (main_wind_click_check_box(pt,&view_setup.box)) {
				main_wind_set_focus(kf_panel_walk);
				break;
			}
			break;
			
	}

		// run wheel change
	
	switch (main_wind_view) {
	
		case vw_3_panel:
		case vw_4_panel:
		
			switch (main_wind_panel_focus) {
				case kf_panel_forward:
					main_wind_setup_panel_forward(&view_setup);
					walk_view_scroll_wheel_z_movement(&view_setup,delta,vm_dir_forward);
					break;
					
				case kf_panel_side:
					main_wind_setup_panel_side(&view_setup);
					walk_view_scroll_wheel_z_movement(&view_setup,delta,vm_dir_side);
					break;
					
				case kf_panel_top:
					main_wind_magnify_scroll(delta);
					break;
					
				case kf_panel_walk:
					main_wind_setup_panel_walk(&view_setup);
					walk_view_scroll_wheel_z_movement(&view_setup,delta,vm_dir_forward);
					break;
			}
			
			break;
			
		case vw_top_only:
			main_wind_magnify_scroll(delta);
			break;
			
		case vw_forward_only:
			main_wind_setup_panel_forward_full(&view_setup);
			walk_view_scroll_wheel_z_movement(&view_setup,delta,vm_dir_forward);
			break;
	}
}

/* =======================================================

      Values and States
      
======================================================= */

void main_wind_tool_reset(void)
{
		// vertex mode
		
	SetControlValue(tool_ctrl[0],(vertex_mode==vertex_mode_none)?1:0);
	SetControlValue(tool_ctrl[1],(vertex_mode==vertex_mode_lock)?1:0);
	SetControlValue(tool_ctrl[2],(vertex_mode==vertex_mode_snap)?1:0);
	
		// select toggle mode
		
	SetControlValue(tool_ctrl[3],select_toggle_mode?1:0);
	
		// drag mode
		
	SetControlValue(tool_ctrl[4],(drag_mode==drag_mode_mesh)?1:0);
	SetControlValue(tool_ctrl[5],(drag_mode==drag_mode_polygon)?1:0);
	SetControlValue(tool_ctrl[6],(drag_mode==drag_mode_vertex)?1:0);
	
		// grid mode
		
	SetControlValue(tool_ctrl[7],(grid_mode==grid_mode_none)?1:0);
	SetControlValue(tool_ctrl[8],(grid_mode==grid_mode_small)?1:0);
	SetControlValue(tool_ctrl[9],(grid_mode==grid_mode_medium)?1:0);
	SetControlValue(tool_ctrl[10],(grid_mode==grid_mode_large)?1:0);
	
		// auto-texture
		
	SetControlValue(tool_ctrl[14],dp_auto_texture?1:0);
 
		// show/hide
		
	SetControlValue(tool_ctrl[15],dp_liquid?1:0);
	SetControlValue(tool_ctrl[16],dp_object?1:0);
	SetControlValue(tool_ctrl[17],dp_lightsoundparticle?1:0);
	SetControlValue(tool_ctrl[18],dp_node?1:0);
}

void main_wind_obscure_tool_reset(void)
{
	SetControlValue(tool_ctrl[19],(obscure_mesh_idx!=-1)?1:0);
}

void main_wind_tool_fill_group_combo(void)
{
	int					n,group_idx;
	char				str[256];
	CFStringRef			cf_str;
	HMHelpContentRec	tag;
	
		// old settings
		
	group_idx=GetControl32BitValue(group_combo);
	
		// delete old control and menu
		
	DisposeControl(group_combo);
	
	DeleteMenu(160);
	DisposeMenu(group_menu);
	
		// recreate the menu
		
	CreateNewMenu(tool_group_menu_id,0,&group_menu);
	
	cf_str=CFStringCreateWithCString(kCFAllocatorDefault,"No Group",kCFStringEncodingMacRoman);
	AppendMenuItemTextWithCFString(group_menu,cf_str,0,FOUR_CHAR_CODE('gp01'),NULL);
	CFRelease(cf_str);
	
	AppendMenuItemTextWithCFString(group_menu,NULL,kMenuItemAttrSeparator,0,NULL);
	
	for (n=0;n<map.ngroup;n++) {
		sprintf(str,"%s (%d)",map.groups[n].name,group_count(n));
		cf_str=CFStringCreateWithCString(kCFAllocatorDefault,str,kCFStringEncodingMacRoman);
		AppendMenuItemTextWithCFString(group_menu,cf_str,0,FOUR_CHAR_CODE('gp03'),NULL);
		CFRelease(cf_str);
	}
	
	InsertMenu(group_menu,kInsertHierarchicalMenu);
	
		// recreate the control
		
	CreatePopupButtonControl(mainwind,&group_box,NULL,tool_group_menu_id,FALSE,0,0,0,&group_combo);
	Draw1Control(group_combo);
	
		// build the help
	
	tag.version=kMacHelpVersion;
	tag.tagSide=kHMDefaultSide;
	SetRect(&tag.absHotRect,0,0,0,0);
	tag.content[kHMMinimumContentIndex].contentType=kHMCFStringContent;
	tag.content[kHMMinimumContentIndex].u.tagCFString=CFStringCreateWithCString(NULL,"Mesh Groups",kCFStringEncodingMacRoman);
	tag.content[kHMMaximumContentIndex].contentType=kHMNoContent;
		
	HMSetControlHelpContent(group_combo,&tag);
	
		// reset the control
		
	SetControl32BitValue(group_combo,group_idx);
}

void main_wind_tool_default(void)
{
    vertex_mode=vertex_mode_none;
	drag_mode=drag_mode_mesh;
	grid_mode=grid_mode_small;
	
	dp_auto_texture=setup.auto_texture;
	
	dp_liquid=dp_object=dp_lightsoundparticle=TRUE;
	dp_node=FALSE;
	dp_textured=TRUE;
	dp_y_hide=FALSE;
    
    main_wind_tool_reset();
}

void main_wind_tool_fix_enable(void)
{
	int			group_idx,type,main_idx,sub_idx;
	
		// group combo
		
	group_idx=-1;
	
	if (select_count()==1) {
		select_get(0,&type,&main_idx,&sub_idx);
		
		switch (type) {
			case mesh_piece:
				group_idx=map.mesh.meshes[main_idx].group_idx;
				break;
			case liquid_piece:
				group_idx=map.liquid.liquids[main_idx].group_idx;
				break;
		}
	}
	
	if (group_idx==-1) {
		SetControl32BitValue(group_combo,1);
	}
	else {
		SetControl32BitValue(group_combo,(3+group_idx));
	}
}

void main_wind_tool_switch_vertex_mode(void)
{
	vertex_mode++;
	if (vertex_mode>vertex_mode_snap) vertex_mode=vertex_mode_none;
	
	main_wind_tool_reset();
}

void main_wind_tool_switch_drag_mode(void)
{
	drag_mode++;
	if (drag_mode>drag_mode_vertex) drag_mode=drag_mode_mesh;

	main_wind_tool_reset();
	main_wind_draw();
}

void main_wind_tool_switch_grid_mode(void)
{
	grid_mode++;
	if (grid_mode>grid_mode_large) grid_mode=grid_mode_none;

	main_wind_tool_reset();
}

