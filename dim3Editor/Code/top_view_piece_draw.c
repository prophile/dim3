/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Top View Piece Drawing Routines

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

#include "common_view.h"
#include "top_view.h"

extern int					cr,cy;
extern bool					dp_wall,dp_floor,dp_ceiling,dp_liquid,dp_ambient,dp_object,dp_node,dp_lightsoundparticle,dp_y_hide;
extern CIconHandle			maplight_icon,mapsound_icon,mapparticle_icon;

extern map_type				map;

unsigned short				effect,effectmask;
int							txtfill,txtrbfill;

extern void primitive_get_extend(int primitive_uid,int *p_minx,int *p_maxx,int *p_minz,int *p_maxz,int *p_miny,int *p_maxy);
extern void primitive_get_center(int primitive_uid,int *x,int *z,int *y);

/* =======================================================

      Draw Pieces
      
======================================================= */

void top_view_piece_draw_floor_ceiling(int rn,fc_segment_data *fc,bool light_color)
{
	int				n,x,z;
	portal_type		*portal;
	
		// Y hiding
		
	if (top_view_hide_y(fc)) return;
	
		// color
		
	if (light_color) {
		glColor4f(0.75f,0.75f,0.75f,1.0f);
	}
	else {
		glColor4f(0.5f,0.5f,0.5f,1.0f);
	}
		
		// polygon
		
	portal=&map.portals[rn];
	
	glBegin(GL_POLYGON);
	
	for (n=0;n!=fc->ptsz;n++) {
		x=fc->x[n]+portal->x;
		z=fc->z[n]+portal->z;
		top_view_map_to_pane(&x,&z);
		glVertex2i(x,z);
	}
	
	glEnd();
	
		// outline
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	
	glBegin(GL_LINE_LOOP);
	
	for (n=0;n!=fc->ptsz;n++) {
		x=fc->x[n]+portal->x;
		z=fc->z[n]+portal->z;
		top_view_map_to_pane(&x,&z);
		glVertex2i(x,z);
	}
	
	glEnd();
}

void top_view_piece_draw_liquid(int rn,liquid_segment_data *liq)
{
	Rect		box;
	RGBColor	blackcolor={0x0,0x0,0x0},greencolor={0x6666,0xFFFF,0x0};
	
	top_view_make_rect(rn,liq->lft,liq->rgt,liq->top,liq->bot,&box);
	RGBForeColor(&greencolor);
	PaintRect(&box);
	RGBForeColor(&blackcolor);
	FrameRect(&box);
}

void top_view_piece_draw_grade(int lx,int lz,int rx,int rz)
{
	int			i,x,z,gadd;
	float		dx,dz,r;
	
	gadd=90/10;
	dx=(float)rx-lx;
	dz=(float)lz-rz;
	
	x=z=0;
	
	glBegin(GL_LINES);
	
	for (i=0;i<=90;i=i+gadd) {
		glVertex2i((lx+x),(rz+z));
		
		r=(float)i*.0174533;
		x=(int)(cos(r)*dx);
		z=(int)(sin(r)*dz);

		glVertex2i((lx+x),(rz+z));
	}
	
	glEnd();
}

void top_view_piece_draw_wall(int rn,wall_segment_data *wall,int curve,int clip)
{
	int			lx,rx,lz,rz;
	portal_type	*portal;
	
	portal=&map.portals[rn];

	lx=wall->lx+portal->x;
	lz=wall->lz+portal->z;
	top_view_map_to_pane(&lx,&lz);
	
	rx=wall->rx+portal->x;
	rz=wall->rz+portal->z;
	top_view_map_to_pane(&rx,&rz);

	glColor4f(0.0f,0.0f,1.0f,1.0f);

	if (curve==cv_forward) {
		top_view_piece_draw_grade(lx,lz,rx,rz);
		glColor4f(1.0f,0.0f,0.0f,1.0f);
	}
	if (curve==cv_backward) {
		top_view_piece_draw_grade(rx,rz,lx,lz);
		glColor4f(1.0f,0.0f,0.0f,1.0f);
	}
	
	glBegin(GL_LINES);
	glVertex2i(lx,lz);
	glVertex2i(rx,rz);
	glEnd();

	if (clip!=0) {
		glColor4f(0.0f,0.0f,0.0f,1.0f);
		
		glBegin(GL_LINE_LOOP);
		glVertex2i((lx-2),(lz-2));
		glVertex2i((lx+3),(lz-2));
		glVertex2i((lx+3),(lz+3));
		glVertex2i((lx-2),(lz+3));
		glEnd();
	}
}

void top_view_piece_draw_ambient_wall(int rn,ambient_wall_segment_data *ambient_wall,Rect *clipbox)
{
	int			lx,rx,lz,rz;
	portal_type	*portal;
	RGBColor	purplecolor={0x9999,0x0,0x6666};
	
	portal=&map.portals[rn];

	lx=ambient_wall->lx+portal->x;
	lz=ambient_wall->lz+portal->z;
	top_view_map_to_pane(&lx,&lz);
	
	rx=ambient_wall->rx+portal->x;
	rz=ambient_wall->rz+portal->z;
	top_view_map_to_pane(&rx,&rz);
	
	if ((lx<clipbox->left) && (rx<clipbox->left)) return;
	if ((lx>clipbox->right) && (rx>clipbox->right)) return;
	if ((lz<clipbox->top) && (rz<clipbox->top)) return;
	if ((lz>clipbox->bottom) && (rz>clipbox->bottom)) return;
	
	RGBForeColor(&purplecolor);
	MoveTo(lx,lz);	
	LineTo(rx,rz);
}

void top_view_piece_draw_ambient_fc(int rn,ambient_fc_segment_data *ambient_fc)
{
	PolyHandle	tply;
	RGBColor	blackcolor={0x0,0x0,0x0},purplecolor={0x9999,0x0,0x6666};
	
	top_view_make_poly(rn,ambient_fc->ptsz,ambient_fc->x,ambient_fc->z,&tply);
	
	RGBForeColor(&purplecolor);
	PaintPoly(tply);
	RGBForeColor(&blackcolor);
	FramePoly(tply);
	KillPoly(tply);
}

void top_view_piece_draw_icon(d3pos *pos,CIconHandle icn,RGBColor *col,Rect *clipbox)
{
	Rect			box;
	
	if (col!=NULL) {
		top_view_make_rect_by_pos(pos,10,&box);
		if ((box.right<clipbox->left) || (box.left>clipbox->right) || (box.bottom<clipbox->top) || (box.top>clipbox->bottom)) return;
		
		RGBForeColor(col);
		PaintOval(&box);
	}
	
	top_view_make_rect_by_pos(pos,8,&box);
	if ((box.right<clipbox->left) || (box.left>clipbox->right) || (box.bottom<clipbox->top) || (box.top>clipbox->bottom)) return;

	PlotCIcon(&box,icn);
}

void top_view_piece_draw_arrow(Rect *box,char *name,int fang,Rect *clipbox,RGBColor *col)
{
	int				t,mx,my,x[3],y[3],xsz,len;
	float			d,d2,dx,dy,r,cs,sn;
	PolyHandle		ph;
	RGBColor		blackcolor={0x0,0x0,0x0};
	
	if (((box->right+25)<clipbox->left) || ((box->left-25)>clipbox->right) || ((box->bottom+10)<clipbox->top) || (box->top>clipbox->bottom)) return;
	
	mx=(box->left+box->right)/2;
	my=(box->top+box->bottom)/2;
	x[0]=mx;
	x[1]=box->right;
	x[2]=box->left;
	y[0]=box->bottom;
	y[1]=y[2]=box->top;
	
	r=(float)fang*.0174533;
	cs=cos(r);
	sn=sin(r);
	for ((t=0);(t!=3);t++) {
		d=(float)(x[t]-mx);
		d2=(float)(y[t]-my);
		dx=(d*cs)+(d2*sn);
		x[t]=(int)dx+mx;
		dy=(d*sn)-(d2*cs);
		y[t]=(int)dy+my;
	}
	
	ph=OpenPoly();
	MoveTo(x[0],y[0]);
	LineTo(x[1],y[1]);
	LineTo(x[2],y[2]);
	LineTo(x[0],y[0]);
	ClosePoly();
	
	RGBForeColor(col);
	PaintPoly(ph);
	RGBForeColor(&blackcolor);
	FramePoly(ph);
	KillPoly(ph);
	
		// start name
		
	TextSize(9);
	len=strlen(name);
	xsz=TextWidth(name,0,len)/2;
	MoveTo((mx-xsz),(box->bottom+10));
	DrawText(name,0,len);
	TextSize(12);
}

/* =======================================================

      Draw Portal Pieces
      
======================================================= */

// supergumba -- delete me!
void top_view_piece_draw2(int rn)
{
	register int			i,k,t;
	register segment_type	*seg;
	int						x1,x2,y1,y2;
	Rect					box;
	RGBColor				col;
	RGBColor				blackcolor={0x0,0x0,0x0},ltgraycolor={0xAAAA,0xAAAA,0xAAAA},graycolor={0x7FFF,0x7FFF,0x7FFF},
							orangecolor={0xF300,0x7FFF,0x1F00},redcolor={0xFFFF,0x0,0x0},greencolor={0x0,0xFFFF,0x0},yellowcolor={0xFFFF,0xFFFF,0x0};

		// floors
		
	if (dp_floor) {
	
		seg=map.segments;
		
		for (i=0;i!=map.nsegment;i++) {
			if ((seg->type==sg_floor) && (seg->rn==rn)) {
				top_view_piece_draw_floor_ceiling(seg->rn,&seg->data.fc,FALSE);
			}
			seg++;
		}
		
	}
	
		// ceilings
		
	if (dp_ceiling) {
	
		seg=map.segments;
	
		for (i=0;i!=map.nsegment;i++) {
			if ((seg->type==sg_ceiling) && (seg->rn==rn)) {
				top_view_piece_draw_floor_ceiling(seg->rn,&seg->data.fc,TRUE);
			}
			seg++;
		}
		
	}
/*	
		// liquids
		
	if (dp_liquid) {
	
		seg=map.segments;
		
		for (i=0;i!=map.nsegment;i++) {
			if ((seg->type==sg_liquid) && (seg->rn==rn)) {
				top_view_piece_draw_liquid(seg->rn,&seg->data.liquid);
			}
			seg++;
		}
	}
	*/
		// walls
		
	if (dp_wall) {
	
		seg=map.segments;
		
		for (i=0;i!=map.nsegment;i++) {
			if ((seg->type==sg_wall) && (seg->rn==rn)) {
				top_view_piece_draw_wall(seg->rn,&seg->data.wall,seg->curve,seg->clip);
			}
			seg++;
		}
	}
	/*
		// ambients
		
	if (dp_ambient) {
	
		seg=map.segments;
	
		for (i=0;i!=map.nsegment;i++) {
			if (seg->rn==rn) {
				if (seg->type==sg_ambient_fc) top_view_piece_draw_ambient_fc(seg->rn,&seg->data.ambient_fc);
				if (seg->type==sg_ambient_wall) top_view_piece_draw_ambient_wall(seg->rn,&seg->data.ambient_wall,clipbox);
			}
			
			seg++;
		}
	}
	
		// nodes
		
	if (dp_node) {
	
			// connections
        
		RGBForeColor(&redcolor);
    
		for (i=0;i!=map.nnode;i++) {
			top_view_make_rect_by_pos(&map.nodes[i].pos,5,&box);
			x1=(box.left+box.right)/2;
			y1=(box.top+box.bottom)/2;
			
			for (k=0;k!=max_node_link;k++) {
				t=map.nodes[i].link[k];
				if (t==-1) continue;
			
				if ((map.nodes[i].pos.rn!=rn) && (map.nodes[t].pos.rn!=rn)) continue;
				
				top_view_make_rect_by_pos(&map.nodes[t].pos,5,&box);
				x2=(box.left+box.right)/2;
				y2=(box.top+box.bottom)/2;
				
				MoveTo(x1,y1);
				LineTo(x2,y2);
			}
		}
		
			// nodes
			
		for (i=0;i!=map.nnode;i++) {
			if (map.nodes[i].pos.rn!=rn) continue;
			
			top_view_make_rect_by_pos(&map.nodes[i].pos,5,&box);
			RGBForeColor(&greencolor);
			PaintRect(&box);
			RGBForeColor(&blackcolor);
			FrameRect(&box);
		}
	
	}
	
		// lights and sounds
		
	if (dp_lightsoundparticle) {
	
		for (i=0;i!=map.nparticle;i++) {
			if (map.particles[i].pos.rn==rn) top_view_piece_draw_icon(&map.particles[i].pos,mapparticle_icon,NULL,clipbox);
		}

		for (i=0;i!=map.nsound;i++) {
			if (map.sounds[i].pos.rn==rn) top_view_piece_draw_icon(&map.sounds[i].pos,mapsound_icon,NULL,clipbox);
		}

		for (i=0;i!=map.nlight;i++) {
			if (map.lights[i].pos.rn!=rn) continue;
			
			col.red=(unsigned short)(map.lights[i].col.r*(float)0xFFFF);
			col.green=(unsigned short)(map.lights[i].col.g*(float)0xFFFF);
			col.blue=(unsigned short)(map.lights[i].col.b*(float)0xFFFF);

			top_view_piece_draw_icon(&map.lights[i].pos,maplight_icon,&col,clipbox);
			
			top_view_make_rect_by_pos(&map.lights[i].pos,map.lights[i].intensity,&box);
			if ((box.right<clipbox->left) || (box.left>clipbox->right) || (box.bottom<clipbox->top) || (box.top>clipbox->bottom)) continue;

			RGBForeColor(&yellowcolor);
			FrameOval(&box);
			RGBForeColor(&blackcolor);
		}
	
	}
	
		// spots
		
	if (dp_object) {
	
			// script spots
			
		for (i=0;i!=map.nspot;i++) {
			if (map.spots[i].pos.rn!=rn) continue;
			
			top_view_make_rect_by_pos(&map.spots[i].pos,5,&box);
			top_view_piece_draw_arrow(&box,map.spots[i].name,map.spots[i].ang.y,clipbox,&orangecolor);
		}
	
			// scenery
		
		for (i=0;i!=map.nscenery;i++) {
			if (map.sceneries[i].pos.rn!=rn) continue;
			
			top_view_make_rect_by_pos(&map.sceneries[i].pos,5,&box);
			top_view_piece_draw_arrow(&box,map.sceneries[i].model_name,map.sceneries[i].ang.y,clipbox,&yellowcolor);
		}
		
	}
	
	RGBForeColor(&blackcolor);
	*/
}







// supergumba -- delete a bunch of this


void top_view_piece_draw(int rn)
{
	int					n,k,t,x,z;
	d3pnt				*pt;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	
		// meshes
		
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	
	portal=&map.portals[rn];
	
	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			glBindTexture(GL_TEXTURE_2D,map.textures[mesh_poly->txt_idx].bitmaps[0].gl_id);
		
			glBegin(GL_POLYGON);
			
			for (t=0;t!=mesh_poly->ptsz;t++) {
				pt=&mesh->vertexes[mesh_poly->v[t]];
				x=pt->x+portal->x;
				z=pt->z+portal->z;
				top_view_map_to_pane(&x,&z);
				glTexCoord2f(mesh_poly->gx[t],mesh_poly->gy[t]);
				glVertex2i(x,z);
			}
			
			glEnd();
		
			mesh_poly++;
		}
	
	
		mesh++;
	}

	glDisable(GL_TEXTURE_2D);
	
		// spots
		
	if (dp_object) {
	
			// script spots
			
		for (n=0;n!=map.nspot;n++) {
			if (map.spots[n].pos.rn!=rn) continue;
			
		//	top_view_make_rect_by_pos(&map.spots[n].pos,5,&box);
		//	top_view_piece_draw_arrow(&box,map.spots[n].name,map.spots[i].ang.y,clipbox,&orangecolor);
		}
	
			// scenery
		
		for (n=0;n!=map.nscenery;n++) {
			if (map.sceneries[n].pos.rn!=rn) continue;
			
		//	top_view_make_rect_by_pos(&map.sceneries[n].pos,5,&box);
		//	top_view_piece_draw_arrow(&box,map.sceneries[n].model_name,map.sceneries[n].ang.y,clipbox,&yellowcolor);
		}
		
	}


}

/* =======================================================

      Draw Selected Pieces Utility
      
======================================================= */

void top_view_piece_selection_draw_line(int rn,int lx,int rx,int lz,int rz)
{
	Rect		box;
	portal_type	*portal;
	
	portal=&map.portals[rn];
	
	lx+=portal->x;
	lz+=portal->z;
	top_view_map_to_pane(&lx,&lz);
	
	rx+=portal->x;
	rz+=portal->z;
	top_view_map_to_pane(&rx,&rz);
	
	SetRect(&box,(lx-2),(lz-2),(lx+3),(lz+3));
	PaintRect(&box);
	SetRect(&box,(rx-2),(rz-2),(rx+3),(rz+3));
	PaintRect(&box);
}

void top_view_piece_selection_draw_rect(int rn,int lft,int rgt,int top,int bot)
{
	int			lx,lz,rx,rz;
	Rect		box;
	portal_type	*portal;
	
	portal=&map.portals[rn];
	
	lx=lft+portal->x;
	lz=top+portal->z;
	top_view_map_to_pane(&lx,&lz);
	
	rx=rgt+portal->x;
	rz=bot+portal->z;
	top_view_map_to_pane(&rx,&rz);
	
	SetRect(&box,(lx-2),(lz-2),(lx+3),(lz+3));
	PaintRect(&box);
	SetRect(&box,(rx-2),(lz-2),(rx+3),(lz+3));
	PaintRect(&box);
	SetRect(&box,(lx-2),(rz-2),(lx+3),(rz+3));
	PaintRect(&box);
	SetRect(&box,(rx-2),(rz-2),(rx+3),(rz+3));
	PaintRect(&box);
}

void top_view_piece_selection_draw_rect_pos(d3pos *pos,int sz)
{
	int			lx,lz,rx,rz;
	Rect		box;
	portal_type	*portal;
	
	portal=&map.portals[pos->rn];
	
	lx=portal->x+pos->x-sz;
	lz=portal->z+pos->z-sz;
	top_view_map_to_pane(&lx,&lz);
	
	rx=portal->x+pos->x+sz;
	rz=portal->z+pos->z+sz;
	top_view_map_to_pane(&rx,&rz);
	
	SetRect(&box,(lx-2),(lz-2),(lx+3),(lz+3));
	PaintRect(&box);
	SetRect(&box,(rx-2),(lz-2),(rx+3),(lz+3));
	PaintRect(&box);
	SetRect(&box,(lx-2),(rz-2),(lx+3),(rz+3));
	PaintRect(&box);
	SetRect(&box,(rx-2),(rz-2),(rx+3),(rz+3));
	PaintRect(&box);
}

void top_view_piece_selection_draw_poly(int rn,int ptsz,int *x,int *z)
{
	register int	i;
	int				lx,lz;
	Rect			box;
	portal_type		*portal;
	
	portal=&map.portals[rn];
	
	for (i=0;i<ptsz;i++) {
		lx=x[i]+portal->x;
		lz=z[i]+portal->z;
		top_view_map_to_pane(&lx,&lz);
		
		SetRect(&box,(lx-2),(lz-2),(lx+3),(lz+3));
		PaintRect(&box);
	}
}

/* =======================================================

      Draw Selected Pieces
      
======================================================= */

void top_view_piece_selection_draw_segment(int idx)
{
	segment_type	*seg;
	
	seg=&map.segments[idx];
	
	switch (seg->type) {
		case sg_wall:
			top_view_piece_selection_draw_line(seg->rn,seg->data.wall.lx,seg->data.wall.rx,seg->data.wall.lz,seg->data.wall.rz);
			break;
		case sg_floor:
		case sg_ceiling:			
			top_view_piece_selection_draw_poly(seg->rn,seg->data.fc.ptsz,seg->data.fc.x,seg->data.fc.z);
			break;
		case sg_liquid:
			top_view_piece_selection_draw_rect(seg->rn,seg->data.liquid.lft,seg->data.liquid.rgt,seg->data.liquid.top,seg->data.liquid.bot);
			break;
		case sg_ambient_wall:
			top_view_piece_selection_draw_line(seg->rn,seg->data.ambient_wall.lx,seg->data.ambient_wall.rx,seg->data.ambient_wall.lz,seg->data.ambient_wall.rz);
			break;
		case sg_ambient_fc:			
			top_view_piece_selection_draw_poly(seg->rn,seg->data.ambient_fc.ptsz,seg->data.ambient_fc.x,seg->data.ambient_fc.z);
			break;
	}
}

void top_view_piece_selection_draw_primitive(int primitive_uid)
{
	int				x,z,cx,cz,cy,minx,maxx,minz,maxz,miny,maxy;
	Rect			box;
	portal_type		*portal;
	
	portal=&map.portals[cr];

	primitive_get_center(primitive_uid,&cx,&cz,&cy);
	primitive_get_extend(primitive_uid,&minx,&maxx,&minz,&maxz,&miny,&maxy);
	
	x=minx+portal->x;
	z=cz+portal->z;
	top_view_map_to_pane(&x,&z);
	SetRect(&box,(x-2),(z-2),(x+3),(z+3));
	PaintRect(&box);

	x=maxx+portal->x;
	z=cz+portal->z;
	top_view_map_to_pane(&x,&z);
	SetRect(&box,(x-2),(z-2),(x+3),(z+3));
	PaintRect(&box);

	x=cx+portal->x;
	z=minz+portal->z;
	top_view_map_to_pane(&x,&z);
	SetRect(&box,(x-2),(z-2),(x+3),(z+3));
	PaintRect(&box);
	
	x=cx+portal->x;
	z=maxz+portal->z;
	top_view_map_to_pane(&x,&z);
	SetRect(&box,(x-2),(z-2),(x+3),(z+3));
	PaintRect(&box);
}

void top_view_piece_selection_draw(void)
{
	int				n,sel_count,type,index;
	RGBColor		curcolor;
	RGBColor		blackcolor={0x0,0x0,0x0},dkgraycolor={0x5555,0x5555,0x5555};
	
	sel_count=select_count();
	sel_count--;
	
		// get handle color
		
	if (sel_count==0) {
		RGBForeColor(&blackcolor);
	}
	else {
		RGBForeColor(&dkgraycolor);
	}
	
		// draw the selection
		
	for (n=sel_count;n>=0;n--) {
	
		select_get(n,&type,&index);
		
			// draw selection
			
		switch (type) {
		
			case segment_piece:
				top_view_piece_selection_draw_segment(index);
				break;
				
			case primitive_piece:
				GetForeColor(&curcolor);
				RGBForeColor(&dkgraycolor);
				top_view_piece_selection_draw_segment(index);
				RGBForeColor(&curcolor);
				top_view_piece_selection_draw_primitive(map.segments[index].primitive_uid[0]);
				break;
				
			case node_piece:
				top_view_piece_selection_draw_rect_pos(&map.nodes[index].pos,5);
				break;
				
			case spot_piece:
				top_view_piece_selection_draw_rect_pos(&map.spots[index].pos,5);
				break;
				
			case scenery_piece:
				top_view_piece_selection_draw_rect_pos(&map.sceneries[index].pos,5);
				break;
				
			case light_piece:
				top_view_piece_selection_draw_rect_pos(&map.lights[index].pos,8);
				break;
				
			case sound_piece:
				top_view_piece_selection_draw_rect_pos(&map.sounds[index].pos,8);
				break;
				
			case particle_piece:
				top_view_piece_selection_draw_rect_pos(&map.particles[index].pos,8);
				break;
		}
	}
}
			
