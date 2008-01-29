/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Segment Routines

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

extern int					cr;
extern bool					dp_auto_texture,dp_wall,dp_floor,dp_ceiling,dp_ambient,dp_liquid;

extern map_type				map;

/* =======================================================

      Polygon Bounds
      
======================================================= */

void poly_find_bound_rect(int ptsz,int *x,int *z,int *lft,int *rgt,int *top,int *bot)
{
	int			i,lx,rx,ty,by;
	
	lx=rx=x[0];
	ty=by=z[0];
	for (i=1;i<ptsz;i++) {
		if (x[i]<lx) lx=x[i];
		if (x[i]>rx) rx=x[i];
		if (z[i]<ty) ty=z[i];
		if (z[i]>by) by=z[i];
	}
	*lft=lx;
	*rgt=rx;
	*top=ty;
	*bot=by;
}

void poly_find_bound_high(int ptsz,int *y,int *top,int *bot)
{
	int			i,ty,by;
	
	ty=by=y[0];

	for (i=1;i<ptsz;i++) {
		if (y[i]<ty) ty=y[i];
		if (y[i]>by) by=y[i];
	}
    
	*top=ty;
	*bot=by;
}

/* =======================================================

      Check for Square Portals
      
======================================================= */

bool segment_is_floor_ceiling_square(int rn,fc_segment_data *fc,Rect *box)
{
	int			lft,rgt,top,bot;
	portal_type	*portal;
	
	if (fc->ptsz!=4) return(FALSE);
	
		// two ways to be square
	
	if ((fc->x[0]==fc->x[1]) && (fc->x[2]==fc->x[3]) && (fc->z[0]==fc->z[2]) && (fc->z[1]==fc->z[3])) {
		lft=fc->x[0];
		rgt=fc->x[2];
		top=fc->z[0];
		bot=fc->z[1];
	}
	else {
		if ((fc->x[0]==fc->x[3]) && (fc->x[1]==fc->x[2]) && (fc->z[0]==fc->z[1]) && (fc->z[2]==fc->z[3])) {
			lft=fc->x[0];
			rgt=fc->x[1];
			top=fc->z[0];
			bot=fc->z[2];
		}
		else {
			return(FALSE);
		}
	}
	
	portal=&map.portals[rn];
	
	lft+=portal->x;
	top+=portal->z;
	top_view_map_to_pane(&lft,&top);
	
	rgt+=portal->x;
	bot+=portal->z;
	top_view_map_to_pane(&rgt,&bot);
	
	if (lft<rgt) {
		box->left=lft;
		box->right=rgt;
	}
	else {
		box->right=lft;
		box->left=rgt;
	}
	
	if (top<bot) {
		box->top=top;
		box->bottom=bot;
	}
	else {
		box->bottom=top;
		box->top=bot;
	}
	
	return(TRUE);
}

/* =======================================================

      Enough Textures to Create Piece?
      
======================================================= */

bool segment_create_texture_ok(void)
{
	if (map.textures[0].bitmaps[0].data!=NULL) return(TRUE);

	StandardAlert(kAlertCautionAlert,"\pYou need at least one texture","\pYou need a default texture in the first spot to add pieces with.  Please add a texture (by double clicking the first open texture) before adding pieces to the map.",NULL,NULL);
	return(FALSE);
}

/* =======================================================

      Clear Segment
      
======================================================= */

void segment_clear(segment_type *seg,int type,int primitive_uid)
{
	int		n;
	
    seg->type=type;
	seg->rn=cr;
	seg->fill=0;
    seg->clip=wc_none;
    seg->curve=cv_none;
	seg->x_txtoff=seg->y_txtoff=0;
	seg->x_txtfact=seg->y_txtfact=1;
    seg->x_shift=seg->y_shift=0;
	seg->txt_ang=0;
	seg->dark_factor=1.0f;
    seg->alpha=1.0f;
	seg->group_idx=-1;
    seg->pass_through=FALSE;
	seg->moveable=FALSE;
	seg->climbable=FALSE;
	seg->lock=FALSE;
	seg->on=TRUE;
	
	seg->primitive_uid[0]=primitive_uid;
	for (n=1;n!=max_primitive_stack;n++) {
		seg->primitive_uid[n]=-1;
	}
}

/* =======================================================

      Add Wall Segment
      
======================================================= */

void segment_add_wall(int lx,int lz,int rx,int rz,int ty,int by,int clip,int curve,int primitive_uid,bool redraw)
{
	segment_type	*seg;
	
	if (!segment_create_texture_ok()) return;
	
	if (map.nsegment==max_segment) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Wall","\pYou've reached the maximum number of segments.",NULL,NULL);
		return;
	}
	
	seg=&map.segments[map.nsegment];
    segment_clear(seg,sg_wall,primitive_uid);
	
	seg->clip=clip;
	seg->curve=curve;
	
	seg->data.wall.lx=lx;
	seg->data.wall.rx=rx;
	seg->data.wall.lz=lz;
	seg->data.wall.rz=rz;
	seg->data.wall.ty=ty;
	seg->data.wall.by=by;

	map.nsegment++;
    
    if (!redraw) return;
    
	dp_wall=TRUE;
	select_clear();
//	select_add(segment_piece,(map.nsegment-1));
	
	main_wind_draw();
	texture_palette_reset();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
	menu_fix_enable();
}

/* =======================================================

      Add Floor/Ceiling Segmenet
      
======================================================= */

void segment_add_floor_quad(int x1,int z1,int y1,int x2,int z2,int y2,int x3,int z3,int y3,int x4,int z4,int y4,int primitive_uid,bool redraw)
{
	segment_type	*seg;
	
	if (!segment_create_texture_ok()) return;

	if (map.nsegment==max_segment) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Floor","\pYou've reached the maximum number of segments.",NULL,NULL);
		return;
	}
	
	seg=&map.segments[map.nsegment];
    segment_clear(seg,sg_floor,primitive_uid);

	seg->data.fc.ptsz=4;
	seg->data.fc.x[0]=x1;
	seg->data.fc.x[1]=x2;
	seg->data.fc.x[2]=x3;
	seg->data.fc.x[3]=x4;
	seg->data.fc.z[0]=z1;
	seg->data.fc.z[1]=z2;
	seg->data.fc.z[2]=z3;
	seg->data.fc.z[3]=z4;
	seg->data.fc.y[0]=y1;
	seg->data.fc.y[1]=y2;
	seg->data.fc.y[2]=y3;
	seg->data.fc.y[3]=y4;
    
	map.nsegment++;
    
    if (!redraw) return;
    
	dp_floor=TRUE;
	select_clear();
//	select_add(segment_piece,(map.nsegment-1));

	main_wind_draw();
	texture_palette_reset();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
	menu_fix_enable();
}

void segment_add_floor_trig(int x1,int z1,int y1,int x2,int z2,int y2,int x3,int z3,int y3,int primitive_uid,bool redraw)
{
	segment_type	*seg;
	
	if (!segment_create_texture_ok()) return;

	if (map.nsegment==max_segment) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Floor","\pYou've reached the maximum number of segments.",NULL,NULL);
		return;
	}
	
	seg=&map.segments[map.nsegment];
    segment_clear(seg,sg_floor,primitive_uid);

	seg->data.fc.ptsz=3;
	seg->data.fc.x[0]=x1;
	seg->data.fc.x[1]=x2;
	seg->data.fc.x[2]=x3;
	seg->data.fc.z[0]=z1;
	seg->data.fc.z[1]=z2;
	seg->data.fc.z[2]=z3;
	seg->data.fc.y[0]=y1;
	seg->data.fc.y[1]=y2;
	seg->data.fc.y[2]=y3;
    
	map.nsegment++;
    
    if (!redraw) return;
    
	dp_floor=TRUE;
	select_clear();
//	select_add(segment_piece,(map.nsegment-1));

	main_wind_draw();
	texture_palette_reset();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
	menu_fix_enable();
}

void segment_add_floor_box(int lx,int lz,int rx,int rz,int y,int primitive_uid,bool redraw)
{
	segment_add_floor_quad(lx,lz,y,rx,lz,y,rx,rz,y,lx,rz,y,primitive_uid,redraw);
}

void segment_add_ceiling_quad(int x1,int z1,int y1,int x2,int z2,int y2,int x3,int z3,int y3,int x4,int z4,int y4,int primitive_uid,bool redraw)
{
	segment_type	*seg;

	if (!segment_create_texture_ok()) return;
	
	if (map.nsegment==max_segment) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Ceiling","\pYou've reached the maximum number of segments.",NULL,NULL);
		return;
	}
	
	seg=&map.segments[map.nsegment];
    segment_clear(seg,sg_ceiling,primitive_uid);

	seg->data.fc.ptsz=4;
	seg->data.fc.x[0]=x1;
	seg->data.fc.x[1]=x2;
	seg->data.fc.x[2]=x3;
	seg->data.fc.x[3]=x4;
	seg->data.fc.z[0]=z1;
	seg->data.fc.z[1]=z2;
	seg->data.fc.z[2]=z3;
	seg->data.fc.z[3]=z4;
	seg->data.fc.y[0]=y1;
	seg->data.fc.y[1]=y2;
	seg->data.fc.y[2]=y3;
	seg->data.fc.y[3]=y4;

	map.nsegment++;
    
    if (!redraw) return;

	dp_ceiling=TRUE;
	select_clear();
//	select_add(segment_piece,(map.nsegment-1));

	main_wind_draw();
	texture_palette_reset();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
	menu_fix_enable();
}

void segment_add_ceiling_box(int lx,int lz,int rx,int rz,int y,int primitive_uid,bool redraw)
{
	segment_add_ceiling_quad(lx,lz,y,rx,lz,y,rx,rz,y,lx,rz,y,primitive_uid,redraw);
}

/* =======================================================

      Add Liquid Segment
	        
======================================================= */

void segment_add_liquid(bool redraw)
{
	int				x,y,z;
	segment_type	*seg;

	if (!segment_create_texture_ok()) return;
	
	if (map.nsegment==max_segment) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Liquid","\pYou've reached the maximum number of segments.",NULL,NULL);
		return;
	}
	
	seg=&map.segments[map.nsegment];
    segment_clear(seg,sg_liquid,-1);
	
	map_portal_calculate_center(&map,cr,&x,&y,&z);

	seg->data.liquid.lft=x-8;
	seg->data.liquid.rgt=x+8;
	seg->data.liquid.top=z-8;
	seg->data.liquid.bot=z+8;
	seg->data.liquid.y=y;
	seg->data.liquid.speed_alter=0.1f;
	seg->data.liquid.wavesize=600;
	seg->data.liquid.tidesize=100;
	seg->data.liquid.tiderate=1000;
	seg->data.liquid.tidedirection=ld_horizontal;
	seg->data.liquid.harm=0;
	seg->data.liquid.drown_tick=10000;
	seg->data.liquid.drown_harm=10;
	seg->data.liquid.col.r=seg->data.liquid.col.g=seg->data.liquid.col.b=1;

	map.nsegment++;
	
	if (!redraw) return;

	dp_liquid=TRUE;
	select_clear();
//	select_add(segment_piece,(map.nsegment-1));

	main_wind_draw();
	texture_palette_reset();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
	menu_fix_enable();
}

/* =======================================================

      Add Ambient Segment
      
======================================================= */

void segment_add_ambient_wall(int lx,int rx,int lz,int rz,int ty,int by,bool redraw)
{
	segment_type	*seg;

	if (!segment_create_texture_ok()) return;
	
	if (map.nsegment==max_segment) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Ambient","\pYou've reached the maximum number of segments.",NULL,NULL);
		return;
	}
	
	seg=&map.segments[map.nsegment];
    segment_clear(seg,sg_ambient_wall,-1);
	
	seg->data.ambient_wall.push=ap_none;
	
	seg->data.ambient_wall.lx=lx;
	seg->data.ambient_wall.rx=rx;
	seg->data.ambient_wall.lz=lz;
	seg->data.ambient_wall.rz=rz;
	seg->data.ambient_wall.ty=ty;
	seg->data.ambient_wall.by=by;
	
	seg->x_txtfact=seg->y_txtfact=0.98;
	seg->lock=TRUE;

	map.nsegment++;
	
	if (!redraw) return;

	dp_ambient=TRUE;
	select_clear();
//	select_add(segment_piece,(map.nsegment-1));

	main_wind_draw();
	texture_palette_reset();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
	menu_fix_enable();
}

void segment_add_ambient_fc(int x1,int z1,int x2,int z2,int y,bool redraw)
{
	segment_type	*seg;
	
	if (!segment_create_texture_ok()) return;

	if (map.nsegment==max_segment) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Create Ambient","\pYou've reached the maximum number of segments.",NULL,NULL);
		return;
	}
	
	seg=&map.segments[map.nsegment];
    segment_clear(seg,sg_ambient_fc,-1);

	seg->data.ambient_fc.ptsz=4;
	seg->data.ambient_fc.x[0]=x1;
	seg->data.ambient_fc.x[1]=x2;
	seg->data.ambient_fc.x[2]=x2;
	seg->data.ambient_fc.x[3]=x1;
	seg->data.ambient_fc.z[0]=z1;
	seg->data.ambient_fc.z[1]=z1;
	seg->data.ambient_fc.z[2]=z2;
	seg->data.ambient_fc.z[3]=z2;
	seg->data.ambient_fc.y[0]=y;
	seg->data.ambient_fc.y[1]=y;
	seg->data.ambient_fc.y[2]=y;
	seg->data.ambient_fc.y[3]=y;

	seg->x_txtfact=seg->y_txtfact=0.98;
	seg->lock=TRUE;
    
	map.nsegment++;
    
    if (!redraw) return;
    
	dp_ambient=TRUE;
	select_clear();
//	select_add(segment_piece,(map.nsegment-1));

	main_wind_draw();
	texture_palette_reset();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
	menu_fix_enable();
}

/* =======================================================

      Segment Moving
      
======================================================= */

void segment_move(segment_type *seg,int xchg,int zchg,int ychg)
{
	int				i;
	
	switch (seg->type) {
		case sg_wall:
			seg->data.wall.lx-=xchg;
			seg->data.wall.lz-=zchg;
			seg->data.wall.rx-=xchg;
			seg->data.wall.rz-=zchg;
			seg->data.wall.ty-=ychg;
			seg->data.wall.by-=ychg;
			break;
			
		case sg_floor:
		case sg_ceiling:
			for (i=0;i!=seg->data.fc.ptsz;i++) {
				seg->data.fc.x[i]-=xchg;
				seg->data.fc.z[i]-=zchg;
				seg->data.fc.y[i]-=ychg;
			}
			break;
			
		case sg_liquid:
			seg->data.liquid.lft-=xchg;
			seg->data.liquid.rgt-=xchg;
			seg->data.liquid.top-=zchg;
			seg->data.liquid.bot-=zchg;
			seg->data.liquid.y-=ychg;
			break;
			
		case sg_ambient_wall:
			seg->data.ambient_wall.lx-=xchg;
			seg->data.ambient_wall.lz-=zchg;
			seg->data.ambient_wall.rx-=xchg;
			seg->data.ambient_wall.rz-=zchg;
			seg->data.ambient_wall.ty-=ychg;
			seg->data.ambient_wall.by-=ychg;
			break;
			
		case sg_ambient_fc:
			for (i=0;i!=seg->data.ambient_fc.ptsz;i++) {
				seg->data.ambient_fc.x[i]-=xchg;
				seg->data.ambient_fc.z[i]-=zchg;
				seg->data.ambient_fc.y[i]-=ychg;
			}
			break;
			
	}
	
	if (dp_auto_texture) map_segment_reset_texture_uvs(&map,seg);
}

/* =======================================================

      Duplicate and Delete Segments
      
======================================================= */

int segment_duplicate(int idx,int primitive_uid,int xadd,int zadd)
{
	int				i,ptsz;
	segment_type	*seg;

	if (map.nsegment==max_segment) {
		StandardAlert(kAlertCautionAlert,"\pCan Not Duplicate","\pYou've reached the maximum number of segments.",NULL,NULL);
		return(-1);
	}
		
	seg=&map.segments[map.nsegment];
	memmove(seg,&map.segments[idx],sizeof(segment_type));
	
	switch (seg->type) {
		case sg_wall:
			seg->data.wall.lx+=xadd;
			seg->data.wall.rx+=xadd;
			seg->data.wall.lz+=zadd;
			seg->data.wall.rz+=zadd;
			break;
		case sg_floor:
		case sg_ceiling:
			ptsz=seg->data.fc.ptsz;
			for (i=0;i!=ptsz;i++) {
				seg->data.fc.x[i]+=xadd;
				seg->data.fc.z[i]+=zadd;
			}
			break;				
		case sg_ambient_wall:
			seg->data.ambient_wall.lx+=xadd;
			seg->data.ambient_wall.rx+=xadd;
			seg->data.ambient_wall.lz+=zadd;
			seg->data.ambient_wall.rz+=zadd;
			break;
		case sg_ambient_fc:
			ptsz=seg->data.ambient_fc.ptsz;
			for (i=0;i!=ptsz;i++) {
				seg->data.ambient_fc.x[i]+=xadd;
				seg->data.ambient_fc.z[i]+=zadd;
			}
			break;				
	}
	
	seg->primitive_uid[0]=primitive_uid;

	map.nsegment++;
	
	return(map.nsegment-1);
}

void segment_delete(int idx)
{
	int			i;
	
	for (i=idx;i<map.nsegment;i++) {
		map.segments[i]=map.segments[i+1];
	}
	
	map.nsegment--;
}

/* =======================================================

      Segment Points
      
======================================================= */

void segment_add_point(void)
{
/* supergumba
	int					n,sel_count,type,index,p;
	segment_type		*seg;
	fc_segment_data		*fc;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);
		if (type!=segment_piece) continue;
		
		seg=&map.segments[index];
		if ((seg->type!=sg_floor) && (seg->type!=sg_ceiling)) continue;

		fc=&seg->data.fc;
		
		if (fc->ptsz>=8) continue;
		
		p=fc->ptsz;
		
		fc->ptsz++;
		fc->x[p]=(fc->x[p-1]+fc->x[0])/2;
		fc->z[p]=(fc->z[p-1]+fc->z[0])/2;
		fc->y[p]=(fc->y[p-1]+fc->y[0])/2;
	}
	*/
}

void segment_sub_point(void)
{
/* supergumba
	int					n,sel_count,type,index;
	segment_type		*seg;
	fc_segment_data		*fc;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);
		if (type!=segment_piece) continue;
		
		seg=&map.segments[index];
		if ((seg->type!=sg_floor) && (seg->type!=sg_ceiling)) continue;

		fc=&seg->data.fc;
		if (fc->ptsz<=3) continue;
		
		fc->ptsz--;
	}
	*/
}

/* =======================================================

      Tesselate Floor/Ceilings
      
======================================================= */

void segment_tesselate(void)
{
/* supergumba
	int					n,k,i,sel_count,type,index;
	segment_type		*seg,*seg2;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);
		if (type!=segment_piece) continue;
		
		seg=&map.segments[index];
		if ((seg->type!=sg_floor) && (seg->type!=sg_ceiling)) continue;
		
		if (seg->data.fc.ptsz==3) continue;

		if (map.nsegment==max_segment) return;
		
		for (k=0;k<(seg->data.fc.ptsz-3);k++) {
			seg2=&map.segments[map.nsegment];
			memmove(seg2,seg,sizeof(segment_type));

			i=k+2;
			
			seg2->data.fc.x[1]=seg->data.fc.x[i];
			seg2->data.fc.y[1]=seg->data.fc.y[i];
			seg2->data.fc.z[1]=seg->data.fc.z[i];
				
			i++;
			if (i>=seg->data.fc.ptsz) i=0;
			
			seg2->data.fc.x[2]=seg->data.fc.x[i];
			seg2->data.fc.y[2]=seg->data.fc.y[i];
			seg2->data.fc.z[2]=seg->data.fc.z[i];
			
			seg2->data.fc.ptsz=3;
			
		//	select_add(segment_piece,map.nsegment);
			
			map.nsegment++;
		}
		
		seg->data.fc.ptsz=3;
	}
	*/
}

/* =======================================================

      Switch Floor/Ceilings
      
======================================================= */

void segment_switch_floor_ceiling(void)
{
/* supergumba
	int					n,sel_count,type,index;
	segment_type		*seg;
	
	sel_count=select_count();
	
	for (n=0;n!=sel_count;n++) {
		select_get(n,&type,&index);
		if (type!=segment_piece) continue;
		
		seg=&map.segments[index];
	
		if (seg->type==sg_floor) {
			seg->type=sg_ceiling;
			continue;
		}
	
		if (seg->type==sg_ceiling) {
			seg->type=sg_floor;
			continue;
		}
	}
	*/
}

/* =======================================================

      Select Alls
      
======================================================= */

void segment_select_all(void)
{
	int				n;
	segment_type	*seg;
	
	select_clear();
	
	seg=map.segments;
	
	for (n=0;n!=map.nsegment;n++) {
	//	if (seg->rn==cr) select_add(segment_piece,n);
		seg++;
	}
}

void segment_select_all_wall(void)
{
	int				n;
	segment_type	*seg;
	
	select_clear();
	
	seg=map.segments;
	
	for (n=0;n!=map.nsegment;n++) {
	//	if ((seg->rn==cr) && (seg->type==sg_wall) && (seg->primitive_uid[0]==-1)) select_add(segment_piece,n);
		seg++;
	}
}

void segment_select_all_floor(void)
{
	int				n;
	segment_type	*seg;
	
	select_clear();
	
	seg=map.segments;
	
	for (n=0;n!=map.nsegment;n++) {
	//	if ((seg->rn==cr) && (seg->type==sg_floor) && (seg->primitive_uid[0]==-1)) select_add(segment_piece,n);
		seg++;
	}
}

void segment_select_all_ceiling(void)
{
	int				n;
	segment_type	*seg;
	
	select_clear();
	
	seg=map.segments;
	
	for (n=0;n!=map.nsegment;n++) {
	//	if ((seg->rn==cr) && (seg->type==sg_ceiling) && (seg->primitive_uid[0]==-1)) select_add(segment_piece,n);
		seg++;
	}
}

void segment_select_all_texture(void)
{
	int				n,fill;
	segment_type	*seg;

	fill=texture_palette_get_selected_fill();
	
	select_clear();
	
	seg=map.segments;
	
	for (n=0;n!=map.nsegment;n++) {
	//	if ((seg->rn==cr) && (seg->fill==fill) && (seg->primitive_uid[0]==-1)) select_add(segment_piece,n);
		seg++;
	}
}

/* =======================================================

      Segment Transforms
      
======================================================= */

void segment_flip_horizontal(segment_type *seg,int min,int max)
{
	int				t;
	
	max-=min;
	
	switch (seg->type) {
		case sg_wall:
			seg->data.wall.lx=(max-(seg->data.wall.lx-min))+min;
			seg->data.wall.rx=(max-(seg->data.wall.rx-min))+min;
			break;
		case sg_floor:
		case sg_ceiling:
			for (t=0;t!=seg->data.fc.ptsz;t++) {
				seg->data.fc.x[t]=(max-(seg->data.fc.x[t]-min))+min;
			}
			break;
		case sg_liquid:
			seg->data.liquid.lft=(max-(seg->data.liquid.lft-min))+min;
			seg->data.liquid.rgt=(max-(seg->data.liquid.rgt-min))+min;
			if (seg->data.liquid.lft>seg->data.liquid.rgt) {
				t=seg->data.liquid.lft;
				seg->data.liquid.lft=seg->data.liquid.rgt;
				seg->data.liquid.rgt=t;
			}
			break;
		case sg_ambient_wall:
			seg->data.ambient_wall.lx=(max-(seg->data.ambient_wall.lx-min))+min;
			seg->data.ambient_wall.rx=(max-(seg->data.ambient_wall.rx-min))+min;
			break;
		case sg_ambient_fc:
			for (t=0;t!=seg->data.ambient_fc.ptsz;t++) {
				seg->data.ambient_fc.x[t]=(max-(seg->data.ambient_fc.x[t]-min))+min;
			}
			break;
	}
}

void segment_flip_vertical(segment_type *seg,int min,int max)
{
	int				t;
	
	max-=min;
	
	switch (seg->type) {
		case sg_wall:
			seg->data.wall.lz=(max-(seg->data.wall.lz-min))+min;
			seg->data.wall.rz=(max-(seg->data.wall.rz-min))+min;
			break;
		case sg_floor:
		case sg_ceiling:
			for (t=0;t!=seg->data.fc.ptsz;t++) {
				seg->data.fc.z[t]=(max-(seg->data.fc.z[t]-min))+min;
			}
			break;
		case sg_liquid:
			seg->data.liquid.top=(max-(seg->data.liquid.top-min))+min;
			seg->data.liquid.bot=(max-(seg->data.liquid.bot-min))+min;
			if (seg->data.liquid.top>seg->data.liquid.bot) {
				t=seg->data.liquid.top;
				seg->data.liquid.top=seg->data.liquid.bot;
				seg->data.liquid.bot=t;
			}
			break;
		case sg_ambient_wall:
			seg->data.ambient_wall.lz=(max-(seg->data.ambient_wall.lz-min))+min;
			seg->data.ambient_wall.rz=(max-(seg->data.ambient_wall.rz-min))+min;
			break;
		case sg_ambient_fc:
			for (t=0;t!=seg->data.ambient_fc.ptsz;t++) {
				seg->data.ambient_fc.z[t]=(max-(seg->data.ambient_fc.z[t]-min))+min;
			}
			break;
	}
}

void segment_rotate(segment_type *seg,int cx,int cz)
{
	int						k,t;
	float					f;
	
	switch (seg->type) {
		case sg_wall:
			k=seg->data.wall.lx-cx;
			seg->data.wall.lx=(seg->data.wall.lz-cz)+cx;
			seg->data.wall.lz=k+cz;
			k=seg->data.wall.rx-cx;
			seg->data.wall.rx=(seg->data.wall.rz-cz)+cx;
			seg->data.wall.rz=k+cz;
			break;
		case sg_floor:
		case sg_ceiling:
			for (t=0;t!=seg->data.fc.ptsz;t++) {
				k=seg->data.fc.x[t]-cx;
				seg->data.fc.x[t]=(seg->data.fc.z[t]-cz)+cx;
				seg->data.fc.z[t]=k+cz;
			}
			
			f=seg->x_txtfact;
			seg->x_txtfact=seg->y_txtfact;
			seg->y_txtfact=f;
			f=seg->x_txtoff;
			seg->x_txtoff=seg->y_txtoff;
			seg->y_txtoff=f;
			break;
		case sg_liquid:
			k=seg->data.liquid.lft-cx;
			seg->data.liquid.lft=(seg->data.liquid.top-cz)+cx;
			seg->data.liquid.top=k+cz;
			k=seg->data.liquid.rgt-cx;
			seg->data.liquid.rgt=(seg->data.liquid.bot-cz)+cx;
			seg->data.liquid.bot=k+cz;
			
			f=seg->x_txtfact;
			seg->x_txtfact=seg->y_txtfact;
			seg->y_txtfact=f;
			f=seg->x_txtoff;
			seg->x_txtoff=seg->y_txtoff;
			seg->y_txtoff=f;
			break;
		case sg_ambient_wall:
			k=seg->data.ambient_wall.lx-cx;
			seg->data.ambient_wall.lx=(seg->data.ambient_wall.lz-cz)+cx;
			seg->data.ambient_wall.lz=k+cz;
			k=seg->data.ambient_wall.rx-cx;
			seg->data.ambient_wall.rx=(seg->data.ambient_wall.rz-cz)+cx;
			seg->data.ambient_wall.rz=k+cz;
			break;
		case sg_ambient_fc:
			for (t=0;t!=seg->data.ambient_fc.ptsz;t++) {
				k=seg->data.ambient_fc.x[t]-cx;
				seg->data.ambient_fc.x[t]=(seg->data.ambient_fc.z[t]-cz)+cx;
				seg->data.ambient_fc.z[t]=k+cz;
			}
			
			f=seg->x_txtfact;
			seg->x_txtfact=seg->y_txtfact;
			seg->y_txtfact=f;
			f=seg->x_txtoff;
			seg->x_txtoff=seg->y_txtoff;
			seg->y_txtoff=f;
			break;
	}
}

void segment_rotate_free(segment_type *seg,float ang,int cx,int cz)
{
	int					t;
	
	cx*=100;
	cz*=100;
	
	switch (seg->type) {
		case sg_wall:
			seg->data.wall.lx*=100;
			seg->data.wall.rx*=100;
			seg->data.wall.lz*=100;
			seg->data.wall.rz*=100;
			rotate_2D_point(&seg->data.wall.lx,&seg->data.wall.lz,cx,cz,ang);
			rotate_2D_point(&seg->data.wall.rx,&seg->data.wall.rz,cx,cz,ang);
			seg->data.wall.lx/=100;
			seg->data.wall.rx/=100;
			seg->data.wall.lz/=100;
			seg->data.wall.rz/=100;
			break;
		case sg_floor:
		case sg_ceiling:
			for (t=0;t!=seg->data.fc.ptsz;t++) {
				seg->data.fc.x[t]*=100;
				seg->data.fc.z[t]*=100;
			}
			rotate_2D_polygon(seg->data.fc.ptsz,seg->data.fc.x,seg->data.fc.z,cx,cz,ang);
			for (t=0;t!=seg->data.fc.ptsz;t++) {
				seg->data.fc.x[t]/=100;
				seg->data.fc.z[t]/=100;
			}
			break;
		case sg_ambient_wall:
			seg->data.ambient_wall.lx*=100;
			seg->data.ambient_wall.rx*=100;
			seg->data.ambient_wall.lz*=100;
			seg->data.ambient_wall.rz*=100;
			rotate_2D_point(&seg->data.ambient_wall.lx,&seg->data.ambient_wall.lz,cx,cz,ang);
			rotate_2D_point(&seg->data.ambient_wall.rx,&seg->data.ambient_wall.rz,cx,cz,ang);
			seg->data.ambient_wall.lx/=100;
			seg->data.ambient_wall.rx/=100;
			seg->data.ambient_wall.lz/=100;
			seg->data.ambient_wall.rz/=100;
			break;
	}
}

