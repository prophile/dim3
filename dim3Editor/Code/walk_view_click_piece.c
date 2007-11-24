/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Piece Click Routines

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
#include "dialog.h"
#include "common_view.h"
#include "walk_view.h"

extern int					cr,cx,cy,cz;
extern bool					dp_wall,dp_floor,dp_ceiling,dp_liquid,dp_ambient,dp_object,dp_lightsoundparticle,dp_node;
extern Rect					walk_view_box;

extern CCrsrHandle			towardcur,dragcur;

extern map_type				map;

/* =======================================================

      Draw Map Pieces for Feeback Click Selection
      
======================================================= */

void walk_view_draw_feedback_poly(int idx,int ptsz,int *x,int *y,int *z)
{
	int			i;
		
	glBegin(GL_POLYGON);

	for (i=0;i<ptsz;i++) {
		glVertex3i((x[i]-cx),(y[i]-cy),(cz-z[i]));
	}
	
	glEnd();
	
	glPassThrough((float)segment_piece);
	glPassThrough((float)idx);
}

void walk_view_draw_feedback_sprite(int type,int idx,d3pos *pos,d3ang *ang,char *display_model)
{
    int			px[4],pz[4],ty,by;
  
	walk_view_model_click_select_size(display_model,pos,ang,px,pz,&ty,&by);
     
	glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
	glVertex3i(px[0],ty,pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],ty,pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[1],by,pz[1]);
    glTexCoord2f(0,1);
	glVertex3i(px[0],by,pz[0]);
	glEnd();
	
	glPassThrough((float)type);
	glPassThrough((float)idx);

	glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
	glVertex3i(px[3],ty,pz[3]);
    glTexCoord2f(1,0);
	glVertex3i(px[2],ty,pz[2]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],by,pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],by,pz[3]);
	glEnd();
	
	glPassThrough((float)type);
	glPassThrough((float)idx);
	
	glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
	glVertex3i(px[3],ty,pz[3]);
    glTexCoord2f(1,0);
	glVertex3i(px[0],ty,pz[0]);
    glTexCoord2f(1,1);
	glVertex3i(px[0],by,pz[0]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],by,pz[3]);
	glEnd();
	
	glPassThrough((float)type);
	glPassThrough((float)idx);
	
	glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
	glVertex3i(px[2],ty,pz[2]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],ty,pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[1],by,pz[1]);
    glTexCoord2f(0,1);
	glVertex3i(px[2],by,pz[2]);
	glEnd();
	
	glPassThrough((float)type);
	glPassThrough((float)idx);
	
	glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
	glVertex3i(px[0],ty,pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],ty,pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],ty,pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],ty,pz[3]);
	glEnd();
	
	glPassThrough((float)type);
	glPassThrough((float)idx);
	
	glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
	glVertex3i(px[0],by,pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],by,pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],by,pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],by,pz[3]);
	glEnd();
	
	glPassThrough((float)type);
	glPassThrough((float)idx);
}

/* =======================================================

      Draw Map For Feedback Click Selection
      
======================================================= */

void walk_view_draw_feedback_map(int rn,bool sel_only)
{
	register int				n,t;
	int							xadd,zadd,ptsz,x[8],y[8],z[8];
	register node_type			*node;
	register spot_type			*spot;
	register map_scenery_type	*scenery;
	register map_light_type		*light;
	register map_sound_type		*sound;
	register map_particle_type	*particle;
	register segment_type		*seg;
    
    xadd=map.portals[rn].x*map_enlarge;
    zadd=map.portals[rn].z*map_enlarge;
	
		// segments
	
	for (n=0;n!=map.nsegment;n++) {
		seg=&map.segments[n];
		if (seg->rn!=rn) continue;
		
		if (sel_only) {
			if (!select_check_segment(n)) continue;
		}
    
		switch (seg->type) {
		
		   case sg_wall:
				if (!dp_wall) break;
				ptsz=walk_view_get_wall_poly(seg,x,z,y,NULL,NULL,xadd,zadd);
				walk_view_draw_feedback_poly(n,ptsz,x,y,z);
				break;
				
			case sg_floor:
			case sg_ceiling:
				if ((seg->type==sg_floor) && (!dp_floor)) break;
				if ((seg->type==sg_ceiling) && (!dp_ceiling)) break;
				
				ptsz=seg->data.fc.ptsz;
				for ((t=0);(t<ptsz);t++) {
					x[t]=(seg->data.fc.x[t]*map_enlarge)+xadd;
					y[t]=seg->data.fc.y[t]*map_enlarge;
					z[t]=(seg->data.fc.z[t]*map_enlarge)+zadd;
				}
				walk_view_draw_feedback_poly(n,ptsz,x,y,z);
				break;
				
			case sg_liquid:
				if (!dp_liquid) break;
				
				x[0]=x[3]=(seg->data.liquid.lft*map_enlarge)+xadd;
				x[1]=x[2]=(seg->data.liquid.rgt*map_enlarge)+xadd;
				z[0]=z[1]=(seg->data.liquid.top*map_enlarge)+zadd;
				z[2]=z[3]=(seg->data.liquid.bot*map_enlarge)+zadd;
				y[0]=y[1]=y[2]=y[3]=(seg->data.liquid.y+1)*map_enlarge;
				
				walk_view_draw_feedback_poly(n,4,x,y,z);
				break;
								
			case sg_ambient_wall:
				if (!dp_ambient) break;
				walk_view_get_ambient_wall_poly(seg,x,z,y,xadd,zadd);
				walk_view_draw_feedback_poly(n,4,x,y,z);
				break;
				
			case sg_ambient_fc:
				if (!dp_ambient) break;
				walk_view_get_ambient_fc_poly(seg,x,z,y,NULL,NULL,xadd,zadd);
				walk_view_draw_feedback_poly(n,seg->data.ambient_fc.ptsz,x,y,z);
				break;
				
		}
	}
            
		// nodes
		
	if (dp_node) {
	
		for (n=0;n<map.nnode;n++) {
			node=&map.nodes[n];
			if (node->pos.rn!=rn) continue;
			
			if (sel_only) {
				if (!select_check(node_piece,n)) continue;
			}
			
			walk_view_draw_feedback_sprite(node_piece,n,&node->pos,NULL,NULL);
		}
		
    }
	
		// spots and scenery
	
    if (dp_object) {
	
		for (n=0;n<map.nspot;n++) {
			spot=&map.spots[n];
			if (spot->pos.rn!=rn) continue;
			
			if (sel_only) {
				if (!select_check(spot_piece,n)) continue;
			}
			
			walk_view_draw_feedback_sprite(spot_piece,n,&spot->pos,NULL,spot->display_model);
		}		
    
		for (n=0;n<map.nscenery;n++) {
			scenery=&map.sceneries[n];
			if (scenery->pos.rn!=rn) continue;
			
			if (sel_only) {
				if (!select_check(scenery_piece,n)) continue;
			}
			
			walk_view_draw_feedback_sprite(scenery_piece,n,&scenery->pos,&scenery->ang,scenery->model_name);
		}		
	}
	
		// lights, sounds, particles
		
	if (dp_lightsoundparticle) {
		
		for (n=0;n<map.nlight;n++) {
			light=&map.lights[n];
			if (light->pos.rn!=rn) continue;
			
			if (sel_only) {
				if (!select_check(light_piece,n)) continue;
			}
			
			walk_view_draw_feedback_sprite(light_piece,n,&light->pos,NULL,NULL);
		}
		
		for (n=0;n<map.nsound;n++) {
			sound=&map.sounds[n];
			if (sound->pos.rn!=rn) continue;
			
			if (sel_only) {
				if (!select_check(sound_piece,n)) continue;
			}
			
			walk_view_draw_feedback_sprite(sound_piece,n,&sound->pos,NULL,NULL);
		}
		
		for (n=0;n<map.nparticle;n++) {
			particle=&map.particles[n];
			if (particle->pos.rn!=rn) continue;
			
			if (sel_only) {
				if (!select_check(particle_piece,n)) continue;
			}
			
			walk_view_draw_feedback_sprite(particle_piece,n,&particle->pos,NULL,NULL);
		}
    }
}

/* =======================================================

      Piece Dragging
      
======================================================= */

bool walk_view_piece_drag(Point pt)
{
	int						x,y,xadd,zadd,yadd,dv,maxx,maxz;
	Point					oldpt;
	bool					first_drag,ctrl_key;
	MouseTrackingResult		track;
	
    if (!Button()) return(FALSE);
	
	undo_set_segment_move();
	
	maxx=map.portals[cr].ex-map.portals[cr].x;
	maxz=map.portals[cr].ez-map.portals[cr].z;
   
	first_drag=TRUE;
	ctrl_key=main_wind_control_down();
	
	oldpt=pt;
	dv=ctrl_key?2:5;
	
	do {
		TrackMouseLocation(NULL,&pt,&track);
		
		if ((pt.h==oldpt.h) && (pt.v==oldpt.v)) continue;
		
		x=(oldpt.h-pt.h)/dv;
		y=(oldpt.v-pt.v)/dv;
		if ((x==0) && (y==0)) continue;
		
		oldpt=pt;
		
			// turn on drag cursor
			
		if (first_drag) {
			SetCCursor(ctrl_key?towardcur:dragcur);
			first_drag=FALSE;
		}
		
			// move item

		walk_view_get_piece_movement(ctrl_key,x,y,&xadd,&zadd,&yadd);
		select_move(cr,xadd,zadd,yadd);

        main_wind_draw();
		
	} while (track!=kMouseTrackingMouseReleased);
	
	InitCursor();
	
	return(!first_drag);
}

/* =======================================================

      View Handle Clicking
      
======================================================= */

bool walk_view_handle_click(int xorg,int yorg,Point pt)
{
    int			k,type,index,idx,n,nitem,x,y,ysz;
    Rect		box;
    GLfloat		*f,buf[128];
	
    if (select_count()!=1) return(FALSE);
	
	select_get(0,&type,&index);
	if ((type!=segment_piece) && (type!=primitive_piece)) return(FALSE);
	
		// draw into feedback buffer
		
	glFeedbackBuffer(128,GL_3D,buf);
	glRenderMode(GL_FEEDBACK);
	
	walk_view_gl_setup();
	walk_view_draw_segment_handles();

	nitem=glRenderMode(GL_RENDER);
	
		// search the buffer for hits
		
    k=-1;

	n=0;
	f=buf;
	
	ysz=walk_view_box.bottom-walk_view_box.top;
	
	while (n<nitem) {
	
			// point
			
		if (*f==GL_POINT_TOKEN) {
			x=((int)*(f+1))+xorg;
			y=(ysz-((int)*(f+2)))+yorg;
			idx=(int)*(f+5);
			f+=6;
			n+=6;
			
			SetRect(&box,(x-5),(y-5),(x+5),(y+5));
			if (!PtInRect(pt,&box)) continue;
			
			k=idx;
			break;
		}
		
			// just a pass through (point was culled)
			
		f+=2;
		n+=2;
	}
	
    if (k==-1) return(FALSE);
    
		// draw the handle
		
	if (type==primitive_piece) {
		walk_view_drag_primitive_handle(pt,index,k);
	}
	else {
		walk_view_drag_segment_handle(pt,index,k);
	}
 
    return(TRUE);
}

/* =======================================================

      View Polygon Clicking
      
======================================================= */

void walk_view_polygon_click_index(int xorg,int yorg,Point pt,int *p_index,int *p_type,bool sel_only)
{
    int			n,i,k,nitem,type,index,pt_type,pt_index,px,py,ysz,
				ptsz,x[8],y[8];
	float		z,cur_z;
    GLfloat		*f,*pnt,*buf;
	
	*p_index=-1;
	
		// line up point with buffer
		
	px=pt.h-xorg;
	py=pt.v-yorg;
	
		// draw into feedback buffer
		
	buf=valloc(102400*sizeof(GLfloat));
	if (buf==NULL) return;
	
	glFeedbackBuffer(100240,GL_3D,buf);
	glRenderMode(GL_FEEDBACK);
	
	walk_view_gl_setup();
	
    for (n=0;n!=map.nportal;n++) {
        if (!map.portals[n].in_path) continue;
        walk_view_draw_feedback_map(n,sel_only);
    }

	nitem=glRenderMode(GL_RENDER);
	
		// search the buffer for hits
		
    k=-1;
	cur_z=1000;

	n=0;
	f=buf;
	
	type=-1;
	index=-1;
	
	ysz=walk_view_box.bottom-walk_view_box.top;
	
	while (n<nitem) {
	
			// polygon
			
		if (*f==GL_POLYGON_TOKEN) {
			ptsz=(int)*(f+1);
			
			z=0;
			pnt=f+2;
			
			for (i=0;i<ptsz;i++) {
				x[i]=(int)*pnt;
				y[i]=ysz-((int)*(pnt+1));
				z+=(float)*(pnt+2);
				pnt+=3;
			}
			
			z=z/(float)ptsz;
			
			pt_type=(int)*(pnt+1);
			pt_index=(int)*(pnt+3);
			
			if (polygon_2D_point_inside(ptsz,x,y,px,py)) {
			
				if (z<cur_z) {
					cur_z=z;
					type=pt_type;
					index=pt_index;
				}
				
			}
			
			f+=((ptsz*3)+6);
			n+=((ptsz*3)+6);
			
			continue;
		}
		
			// just a pass through (polygon was culled)
			
		f+=4;
		n+=4;
	}
	
		// free the buffer
		
	free(buf);
	
	*p_index=index;
	*p_type=type;
}

bool walk_view_polygon_click_select(int xorg,int yorg,Point pt)
{
	int				index,type;
	
		// run through selected items first so we
		// can click and drag selections even when hidden
		
	if (select_count()==0) return(FALSE);
	
		// anything clicked?
		
	walk_view_polygon_click_index(xorg,yorg,pt,&index,&type,TRUE);
	if (index==-1) return(FALSE);
	
		// if not shift down and a primitive clicked,
		// then possibly change selected piece in primitive
		
	if ((!main_wind_shift_down()) && (type==segment_piece)) {
	
		if (map.segments[index].primitive_uid[0]!=-1) {
			if (!select_check(primitive_piece,index)) {
				select_clear();	
				select_add(primitive_piece,index);
				
				menu_fix_enable();
				main_wind_tool_fix_enable();

				main_wind_draw();
				texture_palette_reset();
			}
		}
		
	}
		
		// run drag
	
	return(walk_view_piece_drag(pt));
}
	
void walk_view_polygon_click_normal(int xorg,int yorg,Point pt,bool dblclick)
{
	int				index,primitive_index,type;
	
		// anything clicked?
		
	walk_view_polygon_click_index(xorg,yorg,pt,&index,&type,FALSE);
	
		// if no select, then can still drag previous selections
		
	if (index==-1) {
		if (select_count()!=0) {
			if (!walk_view_piece_drag(pt)) {
				select_clear();					// if no drag, clear selection
				menu_fix_enable();
				main_wind_tool_fix_enable();
				main_wind_draw();
				texture_palette_reset();
			}
		}
		return;
	}
	
		// switch primitive segments to primitives
		
	if (type==segment_piece) {
		if (map.segments[index].primitive_uid[0]!=-1) type=primitive_piece;
	}
	
		// if a selection, make sure in right portal
		
	switch (type) {
		case segment_piece:
		case primitive_piece:
			cr=map.segments[index].rn;
			break;
		case node_piece:
			cr=map.nodes[index].pos.rn;
			break;
		case spot_piece:
			cr=map.spots[index].pos.rn;
			break;
		case scenery_piece:
			cr=map.sceneries[index].pos.rn;
			break;
		case light_piece:
			cr=map.lights[index].pos.rn;
			break;
		case sound_piece:
			cr=map.sounds[index].pos.rn;
			break;
		case particle_piece:
			cr=map.particles[index].pos.rn;
			break;
	}
	
		// add to selection
		
	if (!main_wind_shift_down()) {
		if (!select_check(type,index)) {			// keep selection if selecting an already selected piece
			select_clear();	
			select_add(type,index);
		}
	}
	else {
		if (type==primitive_piece) {				// if it's a primitive, then let's find the original index to flip
			primitive_index=select_check_primitive_find_index(map.segments[index].primitive_uid[0]);
			if (primitive_index!=-1) index=primitive_index;
		}

		select_flip(type,index);
	}
	
		// redraw
		
	menu_fix_enable();
	main_wind_tool_fix_enable();
	
	main_wind_draw();
	texture_palette_reset();
	
		// dragging clicks
		
	if (!dblclick) {
		walk_view_piece_drag(pt);
		return;
	}
	
		// double clicks
		
	switch (type) {
	
		case segment_piece:
		case primitive_piece:
			dialog_segment_setting_run(index);
			break;
			
		case node_piece:
			dialog_node_settings_run(&map.nodes[index]);
			break;
		
		case spot_piece:
			dialog_spot_setting_run(&map.spots[index]);
			break;
			
		case scenery_piece:
			dialog_scenery_setting_run(&map.sceneries[index]);
			break;
		
		case light_piece:
			dialog_map_light_settings_run(&map.lights[index]);
			break;
		
		case sound_piece:
			dialog_map_sound_settings_run(&map.sounds[index]);
			break;
			
		case particle_piece:
			dialog_map_particle_settings_run(&map.particles[index]);
			break;
	}
}

/* =======================================================

      View Map Clicking
      
======================================================= */

void walk_view_click_piece(int xorg,int yorg,Point pt,bool dblclick)
{
	if (walk_view_handle_click(xorg,yorg,pt)) return;
	
	if (!dblclick) {
		if (walk_view_polygon_click_select(xorg,yorg,pt)) return;
	}
	
	walk_view_polygon_click_normal(xorg,yorg,pt,dblclick);
}



