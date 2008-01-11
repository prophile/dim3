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
#include "top_view.h"
#include "portal_view.h"
#include "site_path_view.h"
#include "walk_view.h"

extern int				cr,cx,cz,cy,vertex_mode,magnify_factor,
						txt_palette_y,txt_palette_high;
extern float			walk_view_y_angle,walk_view_x_angle;
extern bool				map_opened,
						dp_primitive,dp_auto_texture,dp_wall,dp_floor,dp_ceiling,dp_liquid,dp_ambient,
						dp_object,dp_lightsoundparticle,dp_node,dp_textured,dp_y_hide;
extern Rect				walk_view_forward_box,walk_view_side_box,portal_view_box,site_path_view_box,top_view_box;

extern map_type			map;
extern setup_type		setup;

int						main_wind_view,main_wind_focus;
int						wtpt;
bool					main_wind_rot;
Rect					main_wind_box;

WindowRef				mainwind;
EventHandlerRef			main_wind_event;
EventHandlerUPP			main_wind_upp;
ControlRef				tool_ctrl[tool_count],piece_ctrl[piece_count],
						group_combo,magnify_slider;
IconSuiteRef			tool_icon[tool_count],piece_icon[piece_count];
ControlActionUPP		magnify_proc;
MenuRef					group_menu;
Rect					group_box;

AGLContext				ctx;

char					tool_tooltip_str[tool_count][64]=
								{
										"Drag Points Freely","Drag Touching Points Together",
										"Snap To Nearby Points While Dragging",
										"Texture Entire Primitive or Segment Only","Auto-Texture","Rotate Editor Panes",
										"Show/Hide Walls","Show/Hide Floors","Show/Hide Ceilings","Show/Hide Liquids",
										"Show/Hide Ambients","Show/Hide Script Spots/Scenery","Show/Hide Light/Sound/Particles",
										"Show/Hide Nodes","Textured/Simple 3D Drawing","Show Only Segments With Nearby Ys",
										"Subtract Polygon Point","Add Polygon Point","Tesselate Polygon",
										"Edit Map Script","Run Map In Engine",
									};
									
char					piece_tooltip_str[piece_count][64]=
									{
										"Add Portal","Add Script Spot",
										"Add Light","Add Sound",
										"Add Particle","Add Scenery","Add Node",
										"Add Wall","Add Liquid Volume",
										"Add Floor","Add Ceiling",
										"Add Wall Ambient","Add Floor-Ceiling Ambient",
										"Add Primitive","Replace Primitive with New Primitive"
									};

/* =======================================================

      Tool Window Events
      
======================================================= */

void main_wind_control_tool(int tool_idx)
{
	undo_clear();
	
	switch (tool_idx) {
		case 0:
			vertex_mode=vm_none;
			SetControlValue(tool_ctrl[0],1);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],0);
			break;
		case 1:
			vertex_mode=vm_lock;
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],1);
			SetControlValue(tool_ctrl[2],0);
			break;
		case 2:
			vertex_mode=vm_snap;
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],1);
			break;
		case 3:
			dp_primitive=!dp_primitive;
			break;
		case 4:
			dp_auto_texture=!dp_auto_texture;
			break;
		case 5:
			SetControlValue(tool_ctrl[tool_idx],0);			// pop back up
			main_wind_rotate_view();
			main_wind_draw();
			break;
		case 6:
			select_clear();
			dp_wall=!dp_wall;
			break;
		case 7:
			select_clear();
			dp_floor=!dp_floor;
			break;
		case 8:
			select_clear();
			dp_ceiling=!dp_ceiling;
			break;
		case 9:
			select_clear();
			dp_liquid=!dp_liquid;
			break;
		case 10:
			select_clear();
			dp_ambient=!dp_ambient;
			break;
		case 11:
			select_clear();
			dp_object=!dp_object;
			break;
		case 12:
			select_clear();
			dp_lightsoundparticle=!dp_lightsoundparticle;
			break;
		case 13:
			select_clear();
			dp_node=!dp_node;
			break;
		case 14:
			dp_textured=!dp_textured;
			break;
		case 15:
			dp_y_hide=!dp_y_hide;
			break;
		case 16:
			SetControlValue(tool_ctrl[tool_idx],0);			// pop back up
			segment_sub_point();
			break;
		case 17:
			SetControlValue(tool_ctrl[tool_idx],0);			// pop back up
			segment_add_point();
			break;
		case 18:
			SetControlValue(tool_ctrl[tool_idx],0);			// pop back up
			segment_tesselate();
			break;
		case 19:
			SetControlValue(tool_ctrl[tool_idx],0);			// pop back up
			launch_map_script_editor();
			break;
		case 20:
			SetControlValue(tool_ctrl[tool_idx],0);			// pop back up
			launch_engine();
			break;
			
	}
	
	menu_fix_enable();
	main_wind_draw();
}

void main_wind_control_piece(int piece_idx)
{
	int				x,y,z;
	
	switch (piece_idx) {
		case 0:
			main_wind_set_view_piece_portal();
			portal_new();
			break;
		case 1:
			main_wind_set_view_piece_segment();
			piece_create_spot();
			break;
		case 2:
			piece_create_light();
			break;
		case 3:
			piece_create_sound();
			break;
		case 4:
			piece_create_particle();
			break;
		case 5:
			main_wind_set_view_piece_segment();
			piece_create_scenery();
			break;
		case 6:
			main_wind_set_view_piece_segment();
			piece_create_node();
			break;
		case 7:
			main_wind_set_view_piece_segment();
			piece_create_get_spot(&x,&y,&z,0,0,8);
			segment_add_wall(x,z,(x+8),z,(y-8),y,wc_none,wc_none,-1,TRUE);
			break;
		case 8:
			main_wind_set_view_piece_segment();
			segment_add_liquid(TRUE);
			break;
		case 9:
			main_wind_set_view_piece_segment();
			piece_create_get_spot(&x,&y,&z,0,0,1);
			segment_add_floor_box(x,z,(x+8),(z+8),(y-1),-1,TRUE);
			break;
		case 10:
			main_wind_set_view_piece_segment();
			piece_create_get_spot(&x,&y,&z,0,0,0);
			segment_add_ceiling_box(x,z,(x+8),(z+8),(y-1),-1,TRUE);
			break;
		case 11:
			main_wind_set_view_piece_segment();
			piece_create_get_spot(&x,&y,&z,0,0,4);
			segment_add_ambient_wall(x,(x+4),z,(z+4),(y-4),y,TRUE);
			break;
		case 12:
			main_wind_set_view_piece_segment();
			piece_create_get_spot(&x,&y,&z,0,0,4);
			segment_add_ambient_fc(x,z,(x+4),(z+4),y,TRUE);
			break;
		case 13:
			main_wind_set_view_piece_segment();
			primitive_add();
			break;
		case 14:
			main_wind_set_view_piece_segment();
			primitive_replace();
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
		
			// no group
			
		if (idx==1) {
			group_set(-1);
			return;
		}
		
			// set a group
			
		group_set(idx-3);
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
                    main_wind_cursor(pt);
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
						main_wind_tool_switch_vertex_mode();
						return(noErr);
					}
	
						// click in texture palette
						
					GetEventParameter(event,kEventParamClickCount,typeUInt32,NULL,sizeof(unsigned long),NULL,&nclick);
					
					if ((pt.v>=txt_palette_y) && (pt.v<=(txt_palette_y+txt_palette_high))) {
						texture_palette_click(pt,(nclick!=1));
						return(noErr);
					}
					
						// click in main window
						
                    if (main_wind_click(pt,(nclick!=1))) return(noErr);
                    
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
					GetEventParameter(event,kEventParamMouseWheelAxis,typeMouseWheelAxis,NULL,sizeof(EventMouseWheelAxis),NULL,&axis);
					if (axis==kEventMouseWheelAxisY) {
						GetEventParameter(event,kEventParamMouseWheelDelta,typeLongInteger,NULL,sizeof(long),NULL,&delta);
						main_wind_scroll_wheel(delta);
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

      Magnify Slider Action
      
======================================================= */

void main_wind_magnify_action(ControlRef ctrl,ControlPartCode code)
{
	int				s;
	
	s=GetControlValue(ctrl);
	if (s==magnify_factor) return;
	
	magnify_factor=s;
	
	top_view_reset();
	portal_view_reset();
	site_path_view_reset();
	
    main_wind_draw();
}

void main_wind_magnify_scroll(int delta)
{
	magnify_factor-=delta;
	if (magnify_factor<magnify_factor_min) magnify_factor=magnify_factor_min;
	if (magnify_factor>magnify_factor_max) magnify_factor=magnify_factor_max;
	
	SetControlValue(magnify_slider,magnify_factor);

	top_view_reset();
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
	main_wind_box.bottom=wbox.bottom-(txt_palette_high+info_high);
	
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
	int							n,rspace;
	Rect						wbox,box;
	GLint						attrib[]={AGL_NO_RECOVERY,AGL_RGBA,AGL_DOUBLEBUFFER,AGL_ACCELERATED,AGL_PIXEL_SIZE,24,AGL_ALPHA_SIZE,8,AGL_DEPTH_SIZE,16,AGL_NONE};
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
		if ((n==2) || (n==5) || (n==13) || (n==15) || (n==18)) OffsetRect(&box,3,0);
	}
	
		// remaining space
	
	rspace=(wbox.right-box.left)-15;
	
		// group combo
		
	CreateNewMenu(tool_group_menu_id,kMenuAttrExcludesMarkColumn,&group_menu);
	InsertMenu(group_menu,kInsertHierarchicalMenu);
	
	group_box=box;
	group_box.top+=3;
	group_box.bottom-=3;
	group_box.left+=5;
	group_box.right=group_box.left+(rspace/2);
	
	CreatePopupButtonControl(mainwind,&group_box,NULL,tool_group_menu_id,FALSE,0,0,0,&group_combo);
	
	tag.version=kMacHelpVersion;
	tag.tagSide=kHMDefaultSide;
	SetRect(&tag.absHotRect,0,0,0,0);
	tag.content[kHMMinimumContentIndex].contentType=kHMCFStringContent;
	tag.content[kHMMinimumContentIndex].u.tagCFString=CFStringCreateWithCString(NULL,"Segment Groups",kCFStringEncodingMacRoman);
	tag.content[kHMMaximumContentIndex].contentType=kHMNoContent;
		
	HMSetControlHelpContent(group_combo,&tag);
	
		// magnify slider

	box.left=(wbox.right-5)-(rspace/2);
	box.right=box.left+(rspace/2);
	box.top=box.top+3;
	box.bottom=box.bottom-3;

	magnify_proc=NewControlActionUPP(main_wind_magnify_action);
	CreateSliderControl(mainwind,&box,(magnify_factor-1),5,210,kControlSliderDoesNotPoint,0,TRUE,magnify_proc,&magnify_slider);
 
	tag.version=kMacHelpVersion;
	tag.tagSide=kHMDefaultSide;
	SetRect(&tag.absHotRect,0,0,0,0);
	tag.content[kHMMinimumContentIndex].contentType=kHMCFStringContent;
	tag.content[kHMMinimumContentIndex].u.tagCFString=CFStringCreateWithCString(NULL,"Magnify",kCFStringEncodingMacRoman);
	tag.content[kHMMaximumContentIndex].contentType=kHMNoContent;
		
	HMSetControlHelpContent(magnify_slider,&tag);

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

	glEnable(GL_SMOOTH);
	glDisable(GL_DITHER);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
	
	aglSetDrawable(ctx,(AGLDrawable)GetWindowPort(mainwind));

		// setup view sizes
		
	texture_palette_setup();
	main_wind_setup();
	walk_view_initialize();
		
	main_wind_rot=setup.view_flip;
	
	walk_view_setup(TRUE,FALSE);
	portal_view_setup(TRUE,FALSE);
	portal_view_center();
	site_path_view_setup(TRUE,FALSE);
	site_path_view_center();
	top_view_setup(TRUE,FALSE);
	top_view_center();
    
        // misc setup
        
	walk_view_y_angle=0.0f;
	walk_view_x_angle=0.0f;
	
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
	
	DisposeControl(magnify_slider);
	DisposeControlActionUPP(magnify_proc);
	
	DisposeControl(group_combo);
	DisposeMenu(group_menu);
	
	for (n=0;n!=piece_count;n++) {
		DisposeControl(piece_ctrl[n]);
		DisposeIconSuite(piece_icon[n],TRUE);
	}

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
	int				n,x,y,rspace;
	Rect			wbox,box;
	
	GetWindowPortBounds(mainwind,&wbox);
	
		// tools
	
	x=wbox.left+(tool_count*tool_button_size)+(5*3);
	rspace=(wbox.right-x)-15;

		// group combo
		
	GetControlBounds(group_combo,&group_box);
	
	group_box.right=group_box.left+(rspace/2);
	
	SizeControl(group_combo,(group_box.right-group_box.left),(group_box.bottom-group_box.top));
	
		// magnify slider

	GetControlBounds(group_combo,&box);

	box.left=(wbox.right-5)-(rspace/2);
	box.right=box.left+(rspace/2);
	
	MoveControl(magnify_slider,box.left,box.top);
	SizeControl(magnify_slider,(box.right-box.left),(box.bottom-box.top));
		
		// pieces
	
	x=wbox.right-piece_button_size;
	y=toolbar_high;
	
	for (n=0;n!=piece_count;n++) {
		MoveControl(piece_ctrl[n],x,y);
		y+=piece_button_size;
	}
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
	
	menu_set_view_check(view);
	
	switch (main_wind_view) {
	
		case vw_walk_top:
			walk_view_setup(TRUE,FALSE);
			portal_view_setup(FALSE,FALSE);
			site_path_view_setup(FALSE,FALSE);
			top_view_setup(TRUE,FALSE);
			top_view_reset();
			main_wind_focus=kf_top;
			break;
			
		case vw_portal_only:
			walk_view_setup(FALSE,FALSE);
			portal_view_setup(TRUE,TRUE);
			site_path_view_setup(FALSE,FALSE);
			top_view_setup(FALSE,FALSE);
			portal_view_reset();
			main_wind_focus=kf_portal;
			break;

		case vw_site_path_only:
			walk_view_setup(FALSE,FALSE);
			portal_view_setup(FALSE,FALSE);
			site_path_view_setup(TRUE,TRUE);
			top_view_setup(FALSE,FALSE);
			site_path_view_reset();
			main_wind_focus=kf_site_path;
			break;
			
		case vw_top_only:
			walk_view_setup(FALSE,FALSE);
			portal_view_setup(FALSE,FALSE);
			site_path_view_setup(FALSE,FALSE);
			top_view_setup(TRUE,TRUE);
			top_view_reset();
			main_wind_focus=kf_top;
			break;
			
		case vw_walk_only:
			walk_view_setup(TRUE,TRUE);
			portal_view_setup(FALSE,FALSE);
			site_path_view_setup(FALSE,FALSE);
			top_view_setup(FALSE,FALSE);
			portal_view_reset();
			main_wind_focus=kf_walk_forward;
			break;
	
	}
}

void main_wind_rotate_view(void)
{
	main_wind_rot=!main_wind_rot;
	main_wind_set_view(main_wind_view);
}

void main_wind_set_view_piece_portal(void)
{
	main_wind_set_view(vw_portal_only);
}

void main_wind_set_view_piece_segment(void)
{
	if ((main_wind_view==vw_portal_only) || (main_wind_view==vw_site_path_only)) {
		main_wind_set_view(vw_walk_top);
		return;
	}
}

/* =======================================================

      Viewport and Projection Setup
      
======================================================= */

void main_wind_set_viewport(Rect *view_box,float rgb)
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
		
	if (rgb<0) return;
		
	glColor4f(rgb,rgb,rgb,1.0f);
	
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

void main_wind_set_3D_projection(Rect *view_box,float ang_x,float ang_y,float fov,float near_z,float far_z,float near_z_offset)
{
	float			ratio;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	ratio=(float)(walk_view_forward_box.right-walk_view_forward_box.left)/(float)(walk_view_forward_box.bottom-walk_view_forward_box.top);
	
	gluPerspective(fov,ratio,near_z,far_z);
	glScalef(-1,-1,1);						// x and y are reversed
	glTranslatef(0,0,near_z_offset);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glRotatef(ang_x,1,0,0);
	glRotatef(ang_y,0,1,0);
}

/* =======================================================

      Drawing
      
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

void main_wind_draw_dividers(void)
{
		// use full view port
	
	main_wind_set_viewport(&main_wind_box,-1.0f);
	
		// draw dividers

	glLineWidth(2.0f);
	
	switch (main_wind_focus) {
	
		case kf_walk_forward:
			main_wind_draw_dividers_single(&walk_view_side_box,FALSE);
			main_wind_draw_dividers_single(&top_view_box,FALSE);
			main_wind_draw_dividers_single(&walk_view_forward_box,TRUE);
			break;
			
		case kf_walk_side:
			main_wind_draw_dividers_single(&walk_view_forward_box,FALSE);
			main_wind_draw_dividers_single(&top_view_box,FALSE);
			main_wind_draw_dividers_single(&walk_view_side_box,TRUE);
			break;
			
		case kf_top:
			main_wind_draw_dividers_single(&walk_view_forward_box,FALSE);
			main_wind_draw_dividers_single(&walk_view_side_box,FALSE);
			main_wind_draw_dividers_single(&top_view_box,TRUE);
			break;
			
	}
	
	glLineWidth(1.0f);
}

void main_wind_draw(void)
{
	Rect			wbox;

	GetWindowPortBounds(mainwind,&wbox);
	
		// clear gl buffer
		
	glDisable(GL_SCISSOR_TEST);
	
	glClearColor(1.0f,1.0f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
        // the views
		
	switch (main_wind_view) {
	
		case vw_walk_top:
			walk_view_draw(FALSE);
			walk_view_draw(TRUE);
			top_view_draw();
			main_wind_draw_dividers();
			break;
			
		case vw_portal_only:
			portal_view_draw();
			break;
			
		case vw_site_path_only:
			site_path_view_draw(-1);
			break;
			
		case vw_top_only:
			top_view_draw();
			break;
			
		case vw_walk_only:
			walk_view_draw(FALSE);
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

void main_wind_set_focus(int focus,bool redraw)
{
	bool			focus_change;
	
	focus_change=main_wind_focus!=focus;
	main_wind_focus=focus;
	
	if ((main_wind_view==vw_walk_top) && (focus_change) && (redraw)) main_wind_draw_dividers();
}

/* =======================================================

      Center Position in Map
	        
======================================================= */

void main_wind_center_position_in_map(void)
{
	int				xsz,zsz,ty,by;

	cx=map.portals[cr].x;
	cz=map.portals[cr].z;
	xsz=map.portals[cr].ex-cx;
	zsz=map.portals[cr].ez-cz;
	portal_get_y_size(cr,&ty,&by);
			
	cx=cx+(xsz/2);		// center starting position
	cz=cz+(zsz/2);
	cy=(ty+by)/2;
	
	walk_view_y_angle=0.0f;
	walk_view_x_angle=0.0f;
	
	wtpt=0;
}

/* =======================================================

      Drawing
      
======================================================= */

bool main_wind_click(Point pt,bool dblclick)
{
    if (PtInRect(pt,&walk_view_forward_box)) {
		main_wind_set_focus(kf_walk_forward,TRUE);
        walk_view_click(pt,dblclick,FALSE);
        return(TRUE);
    }
	
    if (PtInRect(pt,&walk_view_side_box)) {
		main_wind_set_focus(kf_walk_side,TRUE);
        walk_view_click(pt,dblclick,TRUE);
        return(TRUE);
    }
	
    if (PtInRect(pt,&top_view_box)) {
		main_wind_set_focus(kf_top,TRUE);
        top_view_click(pt,dblclick);
        return(TRUE);
    }
    
    if (PtInRect(pt,&portal_view_box)) {
		main_wind_set_focus(kf_portal,TRUE);
        portal_view_click(pt,dblclick);
        return(TRUE);
    }
	
    if (PtInRect(pt,&site_path_view_box)) {
		main_wind_set_focus(kf_site_path,TRUE);
        site_path_view_click(pt,dblclick);
        return(TRUE);
    }
    
    return(FALSE);
}

/* =======================================================

      Cursors
      
======================================================= */

void main_wind_cursor(Point pt)
{
    if ((PtInRect(pt,&walk_view_forward_box)) || (PtInRect(pt,&walk_view_side_box))) {
        walk_view_cursor();
        return;
    }
	
    if (PtInRect(pt,&top_view_box)) {
        top_view_cursor();
        return;
    }
    
    if (PtInRect(pt,&portal_view_box)) {
        portal_view_cursor();
        return;
    }
	
    if (PtInRect(pt,&site_path_view_box)) {
        site_path_view_cursor();
        return;
    }
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

      Key States
      
======================================================= */

void main_wind_key_cursor(void)
{
	Point			pt;
	
	GetMouse(&pt);
	GlobalToLocal(&pt);
	main_wind_cursor(pt);
}

void main_wind_key_down(char ch)
{
		// tab key switches panes
		
	if (ch==0x9) {
	
		if (main_wind_view==vw_walk_top) {
			if (main_wind_focus==kf_walk_forward) {
				main_wind_focus=kf_walk_side;
			}
			else {
				if (main_wind_focus==kf_walk_side) {
					main_wind_focus=kf_top;
				}
				else {
					main_wind_focus=kf_walk_forward;
				}
			}
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
	
		// send keys to proper pane
		
	switch (main_wind_focus) {
	
		case kf_walk_forward:
			walk_view_key(ch,FALSE);
			break;
			
		case kf_walk_side:
			walk_view_key(ch,TRUE);
			break;
			
		case kf_portal:
			portal_view_key(ch);
			break;
			
		case kf_site_path:
			site_path_view_key(ch);
			break;
			
		case kf_top:
			top_view_key(ch);
			break;
	}
}

/* =======================================================

      Scroll Wheel
      
======================================================= */

void main_wind_scroll_wheel(int delta)
{
	switch (main_wind_focus) {
	
		case kf_walk_forward:
			walk_view_scroll_wheel_z_movement(delta,FALSE);
			break;
			
		case kf_walk_side:
			walk_view_scroll_wheel_z_movement(delta,TRUE);
			break;
			
		case kf_portal:
			main_wind_magnify_scroll(delta);
			portal_view_reset();
			break;
			
		case kf_site_path:
			main_wind_magnify_scroll(delta);
			site_path_view_reset();
			break;
			
		case kf_top:
			main_wind_magnify_scroll(delta);
			top_view_reset();
			break;
	}

}

/* =======================================================

      Values and States
      
======================================================= */

void main_wind_tool_reset(void)
{
    int			n;
    
	SetControlValue(tool_ctrl[0],(vertex_mode==vm_none)?1:0);
	SetControlValue(tool_ctrl[1],(vertex_mode==vm_lock)?1:0);
	SetControlValue(tool_ctrl[2],(vertex_mode==vm_snap)?1:0);
	
	SetControlValue(tool_ctrl[3],dp_primitive?1:0);
	SetControlValue(tool_ctrl[4],dp_auto_texture?1:0);
    
	SetControlValue(tool_ctrl[6],dp_wall?1:0);
	SetControlValue(tool_ctrl[7],dp_floor?1:0);
	SetControlValue(tool_ctrl[8],dp_ceiling?1:0);
	SetControlValue(tool_ctrl[9],dp_liquid?1:0);
	SetControlValue(tool_ctrl[10],dp_ambient?1:0);
	SetControlValue(tool_ctrl[11],dp_object?1:0);
	SetControlValue(tool_ctrl[12],dp_lightsoundparticle?1:0);
	SetControlValue(tool_ctrl[13],dp_node?1:0);
	
	SetControlValue(tool_ctrl[14],dp_textured?1:0);
	SetControlValue(tool_ctrl[15],dp_y_hide?1:0);
   
	for (n=16;n!=tool_count;n++) {
		SetControlValue(tool_ctrl[n],0);
	}
    
	SetControlValue(magnify_slider,(magnify_factor-1));
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
	tag.content[kHMMinimumContentIndex].u.tagCFString=CFStringCreateWithCString(NULL,"Segment Groups",kCFStringEncodingMacRoman);
	tag.content[kHMMaximumContentIndex].contentType=kHMNoContent;
		
	HMSetControlHelpContent(group_combo,&tag);
	
		// reset the control
		
	SetControl32BitValue(group_combo,group_idx);
}

void main_wind_tool_default(void)
{
    vertex_mode=vm_none;
	
	dp_primitive=TRUE;
	dp_auto_texture=setup.auto_texture;
	
	dp_wall=dp_floor=dp_ceiling=dp_liquid=dp_ambient=dp_object=dp_lightsoundparticle=TRUE;
	dp_node=FALSE;
	dp_textured=TRUE;
	dp_y_hide=FALSE;
    
    main_wind_tool_reset();
}

void main_wind_tool_fix_enable(void)
{
	int			n,nsel,type,index,seg_type,group_idx;
	bool		has_fc_seg;
	
		// add-sub-tesselate vertex buttons
		
	has_fc_seg=FALSE;
	nsel=select_count();
	
	for (n=0;n!=nsel;n++) {
		select_get(0,&type,&index);
		if (type==segment_piece) {
			seg_type=map.segments[index].type;
			if ((seg_type==sg_floor) || (seg_type==sg_ceiling)) {
				has_fc_seg=TRUE;
				break;
			}
		}
	}
	
	if (!has_fc_seg) {
		HiliteControl(tool_ctrl[16],255);
		HiliteControl(tool_ctrl[17],255);
		HiliteControl(tool_ctrl[18],255);
	}
	else {
		HiliteControl(tool_ctrl[16],0);
		HiliteControl(tool_ctrl[17],0);
		HiliteControl(tool_ctrl[18],0);
	}
	
		// group combo
		
	group_idx=-1;
	
	if (select_count()==1) {
		select_get(0,&type,&index);
		if ((type==segment_piece) || (type==primitive_piece)) group_idx=map.segments[index].group_idx;
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
	if (vertex_mode>vm_snap) vertex_mode=vm_none;

	SetControlValue(tool_ctrl[0],(vertex_mode==vm_none)?1:0);
	SetControlValue(tool_ctrl[1],(vertex_mode==vm_lock)?1:0);
	SetControlValue(tool_ctrl[2],(vertex_mode==vm_snap)?1:0);
}


