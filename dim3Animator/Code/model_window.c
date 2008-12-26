/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Main Model Window

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

#include "model.h"
#include "window.h"

WindowRef						model_wind;
EventHandlerRef					model_wind_event;
EventHandlerUPP					model_wind_upp;
EventLoopTimerRef				model_timer_event;
EventLoopTimerUPP				model_timer_upp;
ControlRef						tool_ctrl[tool_count];
IconSuiteRef					tool_icon[tool_count];

char							tool_tooltip_str[tool_count][64]={
									"Show Textured Model","Show Mesh Only","Show Bones Only","Show Textured Model and Bones Only","Show Mesh and Bones Only",
									"Always Show First Mesh","Show Model, Shadow, and Hit Boxes","Show Bump-Mapping","Show Normals",
									"Rotate Bones Mode","Stretch Bones Mode","Play Current Animation"};

int						draw_type,cur_mesh,cur_bone,cur_pose,cur_animate,
                        shift_x,shift_y,magnify_z,drag_bone_mode,gl_view_x_sz,gl_view_y_sz,
						play_animate_tick[max_model_blend_animation],
						play_animate_blend_idx[max_model_blend_animation],
						play_animate_pose_move_idx[max_model_blend_animation];
float					ang_y,ang_x;
bool					play_animate,play_animate_blend,
						model_view_reset,shift_on,rotate_on,size_on,drag_sel_on,vertex_on,
						model_box_on,model_bump_on,model_normal_on,model_bone_drag_on,model_show_first_mesh;
Rect					drag_sel_box;
CCrsrHandle				shift_cursor,rotate_cursor,size_cursor,add_cursor,sub_cursor,bone_drag_cursor;
AGLContext				ctx;

model_type				model;
model_draw_setup		draw_setup;

extern bool				fileopen;
extern ControlRef		tab_list;

/* =======================================================

      Model Window Keyboard Control
      
======================================================= */

void model_wind_key(char ch,bool up)
{
	if (!fileopen) return;
	
		// esc key deselects
		
	if ((ch==0x1B) && (!up)) {
		vertex_clear_sel_mask(cur_mesh);
		draw_model_wind_pose(&model,cur_mesh,cur_pose);
		return;
	}
	
		// space moves around
		
	if (ch==0x20) {
		if (up) {
			shift_on=FALSE;
			InitCursor();
		}
		else {
			shift_on=TRUE;
			rotate_on=size_on=FALSE;
			SetCCursor(shift_cursor);
		}
		return;
	}
}

void key_modifier_model_wind(unsigned long modifiers)
{
	if ((modifiers&cmdKey)!=0) {
		rotate_on=TRUE;
		shift_on=size_on=FALSE;
		SetCCursor(rotate_cursor);
		return;
	}
	else {
		rotate_on=FALSE;
	}
	
	if ((modifiers&optionKey)!=0) {
		size_on=TRUE;
		shift_on=rotate_on=FALSE;
		SetCCursor(size_cursor);
		return;
	}
	else {
		size_on=FALSE;
	}
	
	if ((modifiers&shiftKey)!=0) {
		size_on=shift_on=rotate_on=FALSE;
		SetCCursor(add_cursor);
		return;
	}
	if ((modifiers&controlKey)!=0) {
		size_on=shift_on=rotate_on=FALSE;
		SetCCursor(sub_cursor);
		return;
	}
	
	if (!shift_on) InitCursor();
}

void model_wind_reset_modifiers(void)
{
	key_modifier_model_wind(GetCurrentKeyModifiers());
}

/* =======================================================

      Model Window Cursors
      
======================================================= */

void model_wind_cursor(unsigned long modifiers)
{
	if (shift_on) {
		SetCCursor(shift_cursor);
		return;
	}
	if ((modifiers&cmdKey)!=0) {
		SetCCursor(rotate_cursor);
		return;
	}
	if ((modifiers&optionKey)!=0) {
		SetCCursor(size_cursor);
		return;
	}
	if ((modifiers&shiftKey)!=0) {
		SetCCursor(add_cursor);
		return;
	}
	if ((modifiers&controlKey)!=0) {
		SetCCursor(sub_cursor);
		return;
	}
	
	InitCursor();
}

/* =======================================================

      Window Controls
      
======================================================= */

bool model_wind_control(ControlRef ctrl)
{
	int				i,idx;
	
	idx=-1;
	for (i=0;i!=tool_count;i++) {
		if (ctrl==tool_ctrl[i]) {
			idx=i;
			break;
		}
	}
	
	if (idx==-1) return(FALSE);
	
	switch (idx) {
	
		case 0:
			SetControlValue(tool_ctrl[0],1);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],0);
			SetControlValue(tool_ctrl[3],0);
			SetControlValue(tool_ctrl[4],0);
			draw_type=dt_model;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			break;
			
		case 1:
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],1);
			SetControlValue(tool_ctrl[2],0);
			SetControlValue(tool_ctrl[3],0);
			SetControlValue(tool_ctrl[4],0);
			draw_type=dt_mesh;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			break;
			
		case 2:
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],1);
			SetControlValue(tool_ctrl[3],0);
			SetControlValue(tool_ctrl[4],0);
			draw_type=dt_bones;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
			break;
			
		case 3:
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],0);
			SetControlValue(tool_ctrl[3],1);
			SetControlValue(tool_ctrl[4],0);
			draw_type=dt_model_bones;
			break;
			
		case 4:
			SetControlValue(tool_ctrl[0],0);
			SetControlValue(tool_ctrl[1],0);
			SetControlValue(tool_ctrl[2],0);
			SetControlValue(tool_ctrl[3],0);
			SetControlValue(tool_ctrl[4],1);
			draw_type=dt_mesh_bones;
			break;
			
		case 5:
			model_show_first_mesh=!model_show_first_mesh;
			break;
			
		case 6:
			model_box_on=!model_box_on;
			break;
			
		case 7:
			model_bump_on=!model_bump_on;
			break;
			
		case 8:
			model_normal_on=!model_normal_on;
			break;
			
		case 9:
			drag_bone_mode=drag_bone_mode_rotate;
			SetControlValue(tool_ctrl[9],1);
			SetControlValue(tool_ctrl[10],0);
			break;
			
		case 10:
			drag_bone_mode=drag_bone_mode_stretch;
			SetControlValue(tool_ctrl[9],0);
			SetControlValue(tool_ctrl[10],1);
			break;
			
		case 11:
			model_wind_play(!play_animate,FALSE);
			break;
	}
	
	draw_model_wind_pose(&model,cur_mesh,cur_pose);
	
	return(TRUE);
}

/* =======================================================

      Window Resize
      
======================================================= */

void model_wind_gl_port_setup(void)
{
	long			rect[4];
	Rect			box;

	GetWindowPortBounds(model_wind,&box);
	
 	rect[0]=box.left;
	rect[1]=info_palette_height;
	rect[2]=gl_view_x_sz+mesh_palette_width;
	rect[3]=gl_view_y_sz+texture_palette_height;

	aglSetInteger(ctx,AGL_BUFFER_RECT,rect);
	aglEnable(ctx,AGL_BUFFER_RECT);
	
	glViewport(box.left,info_palette_height,(gl_view_x_sz+mesh_palette_width),(gl_view_y_sz+texture_palette_height));
	
	glEnable(GL_SCISSOR_TEST);
	glScissor(box.left,info_palette_height,(gl_view_x_sz+mesh_palette_width),(gl_view_y_sz+texture_palette_height));
}

void model_wind_resize(void)
{
	Rect			box;
	
		// new model view size
		
	GetWindowPortBounds(model_wind,&box);
	
	gl_view_x_sz=model_view_min_size;
	gl_view_y_sz=(box.bottom-box.top)-(tool_button_size+texture_palette_height+info_palette_height);
	
	if (gl_view_y_sz<model_view_min_size) gl_view_y_sz=model_view_min_size;

		// resize controls

	box.left+=(gl_view_x_sz+mesh_palette_width);
	box.bottom-=info_palette_height;
		
	resize_pose_controls(&box);
	resize_bone_controls(&box);
	resize_animate_controls(&box);
	resize_vertex_controls(&box);
	
		// resize gl view
		
	model_wind_gl_port_setup();
	
		// redraw

	draw_model_wind_pose(&model,cur_mesh,cur_pose);
	texture_palette_draw();
	mesh_palette_draw();
	info_palette_draw();
	
	DrawControls(model_wind);
}

/* =======================================================

      Info Window Events
      
======================================================= */

OSStatus model_wind_event_handler(EventHandlerCallRef eventhandler,EventRef event,void *userdata)
{
	unsigned long		modifiers,nclick;
	unsigned short		btn;
	long				delta;
	char				ch;
	Point				pt;
	EventMouseWheelAxis	axis;
	ControlRef			ctrl;
	
	switch (GetEventClass(event)) {
	
		case kEventClassWindow:
		
			switch (GetEventKind(event)) {
			
				case kEventWindowDrawContent:
					draw_model_wind_pose(&model,cur_mesh,cur_pose);
					DrawControls(model_wind);
					return(noErr);
					
				case kEventWindowBoundsChanged:
					model_wind_resize();
					return(noErr);

				case kEventWindowClickContentRgn:
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);
					
					SetPort(GetWindowPort(model_wind));
					GlobalToLocal(&pt);

						// clicking in toolbar or right side controls

					if (pt.v<=tool_button_size) return(eventNotHandledErr);
					if (pt.h>=(gl_view_x_sz+mesh_palette_width)) return(eventNotHandledErr);
					
						// clicking in palettes
						
					GetEventParameter(event,kEventParamClickCount,typeUInt32,NULL,sizeof(unsigned long),NULL,&nclick);
					 
					if ((pt.h>=0) && (pt.h<=gl_view_x_sz) && (pt.v>=(tool_button_size+gl_view_y_sz)) && (pt.v<=(tool_button_size+gl_view_y_sz+texture_palette_height))) {
						texture_palette_click(pt,(nclick!=1));
						return(noErr);
					}
					
					if ((pt.h>=gl_view_x_sz) && (pt.h<=(gl_view_x_sz+mesh_palette_width)) && (pt.v>tool_button_size) && (pt.v<(tool_button_size+gl_view_y_sz+texture_palette_height))) {
						mesh_palette_click(pt,(nclick!=1));
						return(noErr);
					}
					
						// clicking in model view
						
					model_wind_offset_click(&pt);
					
					GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(unsigned long),NULL,&modifiers);
					GetEventParameter(event,kEventParamMouseButton,typeMouseButton,NULL,sizeof(unsigned short),NULL,&btn);
					
					if (btn==kEventMouseButtonTertiary) {		// shortcut for mesh mode change
						model_wind_switch_mesh_mode();
						draw_model_wind_pose(&model,cur_mesh,cur_pose);
						return(noErr);
					}
					
					click_model_wind(pt,modifiers);
					model_wind_reset_modifiers();
					
					return(noErr);
					
				case kEventWindowClose:
					return(noErr);
					
                case kEventWindowCursorChange:
					GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(unsigned long),NULL,&modifiers);
					model_wind_cursor(modifiers);
                    return(noErr);
					
			}
			break;
			
		case kEventClassKeyboard:
		
			switch (GetEventKind(event)) {
			
				case kEventRawKeyUp:
					GetEventParameter(event,kEventParamKeyMacCharCodes,typeChar,NULL,sizeof(char),NULL,&ch);
					model_wind_key(ch,TRUE);
					return(noErr);
					
				case kEventRawKeyDown:
					GetEventParameter(event,kEventParamKeyMacCharCodes,typeChar,NULL,sizeof(char),NULL,&ch);
					model_wind_key(ch,FALSE);
					return(noErr);
					
				case kEventRawKeyModifiersChanged:
					GetEventParameter(event,kEventParamKeyModifiers,typeUInt32,NULL,sizeof(unsigned long),NULL,&modifiers);
					key_modifier_model_wind(modifiers);
					return(noErr);
					
			}
			break;
			
		case kEventClassMouse:
		
			switch (GetEventKind(event)) {
			
				case kEventMouseWheelMoved:
				
						// are we over model window?
						
					GetEventParameter(event,kEventParamMouseLocation,typeQDPoint,NULL,sizeof(Point),NULL,&pt);
					SetPort(GetWindowPort(model_wind));
					GlobalToLocal(&pt);
					if (pt.v<=tool_button_size) return(eventNotHandledErr);
					if (pt.h>=gl_view_x_sz) return(eventNotHandledErr);
					
						// zoom
						
					GetEventParameter(event,kEventParamMouseWheelAxis,typeMouseWheelAxis,NULL,sizeof(EventMouseWheelAxis),NULL,&axis);
					if (axis!=kEventMouseWheelAxisY) return(noErr);
					GetEventParameter(event,kEventParamMouseWheelDelta,typeLongInteger,NULL,sizeof(long),NULL,&delta);
					magnify_z-=(delta*20);
					draw_model_wind_pose(&model,cur_mesh,cur_pose);
					return(noErr);
					
			}
			break;
			
		case kEventClassControl:
		
			GetEventParameter(event,kEventParamDirectObject,typeControlRef,NULL,sizeof(ControlRef),NULL,&ctrl);
			if (model_wind_control(ctrl)) return(noErr);
			
			return(eventNotHandledErr);
			
	}

	return(eventNotHandledErr);
}

/* =======================================================

      Model Timer
      
======================================================= */

void model_wind_play(bool play,bool blend)
{
	int					n,tick;
	
		// good animation?
		
	if (play) {
		if (cur_animate==-1) return;
		if (model.animates[cur_animate].npose_move==0) return;
	}
	
		// always turn off animation until setup is complete
		// as animation is on a timer
		
	play_animate=FALSE;
	
		// setup animation
		
	play_animate_blend=blend;
	
	time_start();
	tick=time_get();

	for (n=0;n!=max_model_blend_animation;n++) {
		play_animate_pose_move_idx[n]=0;
		play_animate_tick[n]=tick;
	}
	
	model_view_reset=FALSE;
	
		// turn on/off animation
		
	play_animate=play;
	
	model_wind_reset_tools();
}

void model_wind_play_calc_animation(int cur_tick,int animate_idx,int blend_idx,bool non_blend_setup)
{
	int						first_pose,last_pose,tick,nxt_tick,
							pose_move_1_idx,pose_move_2_idx,msec;
	float					pose_factor;
	model_animate_type		*animate;
	model_pose_move_type	*pose_move_1,*pose_move_2;

		// calc animation
		
	animate=&model.animates[animate_idx];
	
	first_pose=0;
	if (animate->loop_start!=-1) first_pose=animate->loop_start;

	last_pose=animate->npose_move-1;
	if (animate->loop_end!=-1) last_pose=animate->loop_end;
	
	tick=play_animate_tick[blend_idx];
	pose_move_1_idx=play_animate_pose_move_idx[blend_idx];
	
	msec=animate->pose_moves[pose_move_1_idx].msec;
	nxt_tick=tick+msec;
	
	while (nxt_tick<cur_tick) {				// skip ahead till we find correct pose
		pose_move_1_idx++;
		if (pose_move_1_idx>last_pose) pose_move_1_idx=first_pose;
		
		msec=animate->pose_moves[pose_move_1_idx].msec;

		tick=nxt_tick;
		nxt_tick=tick+msec;
	}
	
	play_animate_tick[blend_idx]=tick;
	play_animate_pose_move_idx[blend_idx]=pose_move_1_idx;
	
	pose_move_2_idx=pose_move_1_idx+1;
	if (pose_move_2_idx>last_pose) pose_move_2_idx=first_pose;
	
		// get the pose moves
		
	pose_move_1=&animate->pose_moves[pose_move_1_idx];
	pose_move_2=&animate->pose_moves[pose_move_2_idx];
	
		// setup the poses
	
	draw_setup.poses[blend_idx].idx_1=pose_move_1->pose_idx;
	draw_setup.poses[blend_idx].idx_2=pose_move_2->pose_idx;
	
	pose_factor=(float)(nxt_tick-cur_tick)/(float)msec;
	
    draw_setup.poses[blend_idx].factor=pose_factor;
	draw_setup.poses[blend_idx].acceleration=pose_move_1->acceleration;
	
		// non-blended setup
		
	if (non_blend_setup) {
		draw_setup.ang.x=0;
		draw_setup.ang.y=0;
		draw_setup.ang.z=0;
		
		draw_setup.sway.x=pose_move_2->sway.x+((pose_move_1->sway.x-pose_move_2->sway.x)*pose_factor);
		draw_setup.sway.y=pose_move_2->sway.y+((pose_move_1->sway.y-pose_move_2->sway.y)*pose_factor);
		draw_setup.sway.z=pose_move_2->sway.z+((pose_move_1->sway.z-pose_move_2->sway.z)*pose_factor);
		draw_setup.move.x=pose_move_2->mov.x+((pose_move_1->mov.x-pose_move_2->mov.x)*pose_factor);
		draw_setup.move.y=pose_move_2->mov.y+((pose_move_1->mov.y-pose_move_2->mov.y)*pose_factor);
		draw_setup.move.z=pose_move_2->mov.z+((pose_move_1->mov.z-pose_move_2->mov.z)*pose_factor);
	}
}

void model_wind_timer(EventLoopTimerRef inTimer,void *inUserData)
{
	int					n,cur_tick;
	
		// if not playing, then do model view resets
		
	if (!play_animate) {
		if (model_view_reset) {
			model_view_reset=FALSE;
			draw_model_wind_pose(&model,cur_mesh,cur_pose);
		}
		return;
	}
	
		// if no current animation, just do no pose for animated textures
		
	if ((cur_animate==-1) && (!play_animate_blend)) {
		draw_model_wind_pose(&model,cur_mesh,-1);
		return;
	}
	
		// clear the draw setup
		
	model_clear_draw_setup(&model,&draw_setup);
	
		// calc the pose
		
	cur_tick=time_get();
	
	if (!play_animate_blend) {
		model_wind_play_calc_animation(cur_tick,cur_animate,0,TRUE);
	}
	else {
		for (n=0;n!=max_model_blend_animation;n++) {
			if (play_animate_blend_idx[n]!=-1) {
				model_wind_play_calc_animation(cur_tick,play_animate_blend_idx[n],n,(n==0));
			}
		}
	}
		
		// global draw setup
	
	draw_model_wind(&model,cur_mesh,&draw_setup);
}

/* =======================================================

      Model Window
      
======================================================= */

void model_wind_open(void)
{
	int							n;
	Rect						wbox,box;
	GLint						attrib[]={AGL_RGBA,AGL_DOUBLEBUFFER,AGL_DEPTH_SIZE,16,AGL_ALL_RENDERERS,AGL_NONE};
	AGLPixelFormat				pf;
	IconFamilyHandle			iconfamily;
	ControlButtonContentInfo	icon_info;
	HMHelpContentRec			tag;
	EventTypeSpec	wind_events[]={	{kEventClassWindow,kEventWindowDrawContent},
									{kEventClassWindow,kEventWindowBoundsChanged},
									{kEventClassWindow,kEventWindowClickContentRgn},
									{kEventClassWindow,kEventWindowClose},
									{kEventClassWindow,kEventWindowCursorChange},
									{kEventClassKeyboard,kEventRawKeyUp},
									{kEventClassKeyboard,kEventRawKeyDown},
									{kEventClassKeyboard,kEventRawKeyModifiersChanged},
									{kEventClassMouse,kEventMouseWheelMoved},
									{kEventClassControl,kEventControlHit}};

    GetAvailableWindowPositioningBounds(GetMainDevice(),&wbox);
	
	SetRect(&box,(wbox.left+5),(wbox.top+25),(wbox.right-5),(wbox.bottom-5));
	CreateNewWindow(kDocumentWindowClass,kWindowStandardDocumentAttributes|kWindowLiveResizeAttribute|kWindowStandardHandlerAttribute|kWindowInWindowMenuAttribute,&box,&model_wind);
	SetWTitle(model_wind,"\pModel");
	ShowWindow(model_wind);
	
		// set font
		
	SetPort(GetWindowPort(model_wind));
	TextFont(FMGetFontFamilyFromName("\pMonaco"));
	TextSize(10);
	
		// setup and cursors
		
	cur_animate=-1;
	model_wind_play(FALSE,FALSE);
	
	shift_on=FALSE;
	rotate_on=FALSE;
	size_on=FALSE;
	drag_sel_on=FALSE;
	
	shift_cursor=GetCCursor(128);
	rotate_cursor=GetCCursor(129);
	size_cursor=GetCCursor(130);
	add_cursor=GetCCursor(131);
	sub_cursor=GetCCursor(132);
	bone_drag_cursor=GetCCursor(133);
	
		// get gl sizes
		
	GetWindowPortBounds(model_wind,&box);
	
	gl_view_x_sz=model_view_min_size;
	gl_view_y_sz=(box.bottom-box.top)-(tool_button_size+texture_palette_height+info_palette_height);
	
		// toolbar buttons
			
	SetRect(&box,0,0,tool_button_size,tool_button_size);
	
	for (n=0;n!=tool_count;n++) {
	
			// create button
			
		iconfamily=(IconFamilyHandle)GetResource('icns',(500+n));
		IconFamilyToIconSuite(iconfamily,kSelectorAllAvailableData,&tool_icon[n]);
		
		icon_info.contentType=kControlContentIconSuiteHandle;
		icon_info.u.iconSuite=tool_icon[n];
			
		CreateBevelButtonControl(model_wind,&box,NULL,kControlBevelButtonSmallBevel,kControlBehaviorToggles,&icon_info,0,0,0,&tool_ctrl[n]);
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
		if ((n==4) || (n==8) || (n==10)) OffsetRect(&box,5,0);
	}
		
		// dragging for bones window
		
	SetAutomaticControlDragTrackingEnabledForWindow(model_wind,TRUE);
	
		// controls
		
	GetWindowPortBounds(model_wind,&box);

	box.left+=(gl_view_x_sz+mesh_palette_width);
	box.bottom-=info_palette_height;
	
	start_pose_controls(model_wind,&box);
	start_bone_controls(model_wind,&box);
    start_animate_controls(model_wind,&box);
	start_vertex_controls(model_wind,&box);
	
		// start open gl
		
	pf=aglChoosePixelFormat(NULL,0,attrib);
	ctx=aglCreateContext(pf,NULL);
	aglSetDrawable(ctx,(AGLDrawable)GetWindowPort(model_wind));
	aglSetCurrentContext(ctx);
	aglDestroyPixelFormat(pf);
	
	model_wind_gl_port_setup();
	
	glDisable(GL_SMOOTH);
	glDisable(GL_DITHER);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	glEnableClientState(GL_VERTEX_ARRAY);
	
	glClearColor(0.9f,0.9f,0.9f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	aglSwapBuffers(ctx);
	
		// get the controls draw
		
	DrawControls(model_wind);

		// events
		
	model_wind_upp=NewEventHandlerUPP(model_wind_event_handler);
	InstallEventHandler(GetWindowEventTarget(model_wind),model_wind_upp,GetEventTypeCount(wind_events),wind_events,NULL,&model_wind_event);
		
	model_timer_upp=NewEventLoopTimerUPP(model_wind_timer);
	InstallEventLoopTimer(GetCurrentEventLoop(),0,0.01,model_timer_upp,NULL,&model_timer_event);
}

void model_wind_close(void)
{
	int				n;
	
		// remove events
		
	RemoveEventLoopTimer(model_timer_event);
	DisposeEventLoopTimerUPP(model_timer_upp);
	
	RemoveEventHandler(model_wind_event);
	DisposeEventHandlerUPP(model_wind_upp);
	
		// remove controls
		
	end_pose_controls();
	end_bone_controls();
	end_vertex_controls();
    end_animate_controls();
		
	for (n=0;n!=tool_count;n++) {
		DisposeControl(tool_ctrl[n]);
		DisposeIconSuite(tool_icon[n],TRUE);
	}

		// close open gl
		
	aglSetCurrentContext(NULL);
	aglSetDrawable(ctx,NULL);
	aglDestroyContext(ctx);
	
		// close window

	DisposeWindow(model_wind);

	DisposeCCursor(shift_cursor);
	DisposeCCursor(rotate_cursor);
	DisposeCCursor(size_cursor);
	DisposeCCursor(add_cursor);
	DisposeCCursor(sub_cursor);
	DisposeCCursor(bone_drag_cursor);
}

/* =======================================================

      Window Click Offsets
      
======================================================= */

void model_wind_offset_click(Point *pt)
{
	pt->v-=tool_button_size;
}

/* =======================================================

      Model Window Controls
      
======================================================= */

void model_wind_set_title(char *title)
{
	unsigned char		p_str[256];
	
	CopyCStringToPascal(title,p_str);
	SetWTitle(model_wind,p_str);
}

void model_wind_reset_tools(void)
{
	SetControlValue(tool_ctrl[0],(draw_type==dt_model)?1:0);
	SetControlValue(tool_ctrl[1],(draw_type==dt_mesh)?1:0);
	SetControlValue(tool_ctrl[2],(draw_type==dt_bones)?1:0);
	SetControlValue(tool_ctrl[3],(draw_type==dt_model_bones)?1:0);
	SetControlValue(tool_ctrl[4],(draw_type==dt_mesh_bones)?1:0);
    
	SetControlValue(tool_ctrl[5],model_show_first_mesh?1:0);
	SetControlValue(tool_ctrl[6],model_box_on?1:0);
	SetControlValue(tool_ctrl[7],model_bump_on?1:0);
	SetControlValue(tool_ctrl[8],model_normal_on?1:0);
	
	SetControlValue(tool_ctrl[9],(drag_bone_mode==drag_bone_mode_rotate)?1:0);
	SetControlValue(tool_ctrl[10],(drag_bone_mode==drag_bone_mode_stretch)?1:0);

	SetControlValue(tool_ctrl[11],play_animate?1:0);
}

void model_wind_switch_mesh_mode(void)
{
	draw_type++;
	if (draw_type>dt_mesh_bones) draw_type=dt_model;
	
	SetControlValue(tool_ctrl[0],(draw_type==dt_model)?1:0);
	SetControlValue(tool_ctrl[1],(draw_type==dt_mesh)?1:0);
	SetControlValue(tool_ctrl[2],(draw_type==dt_bones)?1:0);
	SetControlValue(tool_ctrl[3],(draw_type==dt_model_bones)?1:0);
	SetControlValue(tool_ctrl[4],(draw_type==dt_mesh_bones)?1:0);
}

