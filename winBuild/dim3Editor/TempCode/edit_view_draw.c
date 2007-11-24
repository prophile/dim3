/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Edit View Main Draw Routines

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

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#define edit_gl_fov				45.0f
#define edit_gl_near_z			(6*map_enlarge)
#define edit_gl_far_z			(2000*map_enlarge)
#define edit_gl_near_offset		(3*map_enlarge)

extern map_type			map;




int						edit_view_last_portal=0;
unsigned long			edit_view_cur_gl_id;

bool					dp_wall=TRUE,
						dp_floor=TRUE,
						dp_ceiling=TRUE,
						dp_liquid=TRUE,
						dp_ambient=TRUE,
						dp_node=TRUE,
						dp_object=TRUE,
						dp_lightsoundparticle=TRUE;

d3pnt					edit_view_pt;

bitmap_type				node_bitmap,spot_bitmap,scenery_bitmap,
						light_bitmap,sound_bitmap,particle_bitmap;

short					edit_seg_list[max_segment];

extern void edit_view_sight_path_mark(int rn);
extern void edit_view_draw_fc_curve(int ptsz,int *x,int *z,int *y,float *gx,float *gy,int curve_type,int fill,int txt_ang,float r,float g,float b,float dark_factor,float alpha);
extern void edit_view_draw_wall_curve(segment_type *seg,int xadd,int zadd,int fill,int txt_ang,float dark_factor,float alpha);
extern void edit_view_draw_wall_clip_curve(segment_type *seg,int xadd,int zadd,int fill,int txt_ang,float dark_factor,float alpha);
extern void edit_view_poly_find_bound_rect(int ptsz,int *x,int *z,int *lft,int *rgt,int *top,int *bot);
extern int edit_view_poly_for_wall(segment_type *seg,int *x,int *y,int *z,float *gx,float *gy,int xadd,int zadd);
extern void edit_view_poly_for_fc(segment_type *seg,int *x,int *y,int *z,float *gx,float *gy,int xadd,int zadd);
extern void edit_view_poly_for_poly(segment_type *seg,int *x,int *y,int *z,float *gx,float *gy,int xadd,int zadd);
extern void edit_view_poly_for_ambient_wall(segment_type *seg,int *x,int *y,int *z,int xadd,int zadd);
extern void edit_view_poly_for_ambient_fc(segment_type *seg,int *x,int *y,int *z,float *gx,float *gy,int xadd,int zadd);
extern int edit_view_draw_portal_segments_sort(int cnt,short *seg_list,d3pnt *camera_pt);

//supergumba

bool edit_view_model_draw(d3pos *pos,d3ang *ang,char *name)
{
	return(FALSE);
}

/* =======================================================

      Draw Segments and Sprites
      
======================================================= */

void edit_view_draw_segment_poly(int ptsz,int *x,int *y,int *z,float *gx,float *gy,int txt,int txt_ang,float darken,float alpha)
{
	int				n;
	unsigned long	gl_id;
	texture_type	*texture;

	texture=&map.textures[txt];
	
	gl_id=texture->bitmaps[0].gl_id;
	
	if (edit_view_cur_gl_id!=gl_id) {
    	glBindTexture(GL_TEXTURE_2D,gl_id);
		edit_view_cur_gl_id=gl_id;
	}

	if (texture->additive) {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	}
	else {
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

    glColor4f(darken,darken,darken,alpha);
	glBegin(GL_POLYGON);

	for (n=0;n<ptsz;n++) {
	
		switch (txt_ang) {
			case ta_0:
				glTexCoord2f(gx[n],gy[n]);
				break;
			case ta_90:
				glTexCoord2f(gy[n],-gx[n]);
				break;
			case ta_180:
				glTexCoord2f(-gx[n],-gy[n]);
				break;
			case ta_270:
				glTexCoord2f(-gy[n],gx[n]);
				break;
		}
				
		glVertex3i((x[n]-edit_view_pt.x),(y[n]-edit_view_pt.y),(edit_view_pt.z-z[n]));
	}
	
	glEnd();
}

void edit_view_draw_sprite(d3pos *pos,int xadd,int zadd,int txt)
{
    int			x,y,z,wid,high;
  
    x=((pos->x*map_enlarge)+xadd)-edit_view_pt.x;
    y=((pos->y+1)*map_enlarge)-edit_view_pt.y;
    z=edit_view_pt.z-((pos->z*map_enlarge)+zadd);
    
    wid=map_enlarge*3;
    high=map_enlarge*4;

 	glEnable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

 	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
  
    glBindTexture(GL_TEXTURE_2D,txt);
    
	glColor4f(1,1,1,1);
	
	glBegin(GL_QUADS);

    glTexCoord2f(0,0);
	glVertex3i((x-wid),(y-high),(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x+wid),(y-high),(z-wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),y,(z-wid));
    glTexCoord2f(0,1);
	glVertex3i((x-wid),y,(z-wid));

    glTexCoord2f(0,0);
	glVertex3i((x-wid),(y-high),(z+wid));
    glTexCoord2f(1,0);
	glVertex3i((x+wid),(y-high),(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),y,(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x-wid),y,(z+wid));

    glTexCoord2f(0,0);
	glVertex3i((x-wid),(y-high),(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x-wid),(y-high),(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x-wid),y,(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x-wid),y,(z-wid));

    glTexCoord2f(0,0);
	glVertex3i((x+wid),(y-high),(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x+wid),(y-high),(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),y,(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x+wid),y,(z-wid));

    glTexCoord2f(0,0);
	glVertex3i((x-wid),(y-high),(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x-wid),(y-high),(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),(y-high),(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x+wid),(y-high),(z-wid));

    glTexCoord2f(0,0);
	glVertex3i((x-wid),y,(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x-wid),y,(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),y,(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x+wid),y,(z-wid));

	glEnd();
	
	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Edit View Portal Segment Drawing
      
======================================================= */

void edit_view_draw_portal_segments_list(int cnt,short *sptr,int rn,bool opaque)
{
	int				n,t;
	segment_type	*seg;
	int				xadd,zadd,ptsz,x[8],z[8],y[8],lft,rgt,top,bot;
	float			gx[8],gy[8],
					x_txtoff,z_txtoff,x_txtfact,z_txtfact;
							
	glEnable(GL_TEXTURE_2D);
	
	edit_view_cur_gl_id=-1;
	
	xadd=map.portals[rn].x*map_enlarge;
	zadd=map.portals[rn].z*map_enlarge;

		// no depth buffer for transparent segments
		
	if (opaque) {
		glDisable(GL_BLEND);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL,0);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
	}
	else {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL,0);
	
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);
	}
	
		// draw segments
		
	for (n=0;n!=cnt;n++) {
		seg=&map.segments[*sptr++];
		
		switch (seg->type) {
		
			case sg_wall:
				if (!dp_wall) break;
                
                if ((seg->clip==wc_top_curve) || (seg->clip==wc_top_arch) || (seg->clip==wc_bottom_curve) || (seg->clip==wc_bottom_arch) || (seg->clip==wc_top_curve_arch) || (seg->clip==wc_bottom_curve_arch)) {
					edit_view_draw_wall_clip_curve(seg,xadd,zadd,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
                    break;
                }
                
                if (seg->curve!=cv_none) {
					edit_view_draw_wall_curve(seg,xadd,zadd,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
                    break;
				}
                
				ptsz=edit_view_poly_for_wall(seg,x,y,z,gx,gy,xadd,zadd);
				edit_view_draw_segment_poly(ptsz,x,y,z,gx,gy,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
				break;
				
			case sg_floor:
			case sg_ceiling:
				if ((seg->type==sg_floor) && (!dp_floor)) break;
				if ((seg->type==sg_ceiling) && (!dp_ceiling)) break;

				edit_view_poly_for_fc(seg,x,y,z,gx,gy,xadd,zadd);

				ptsz=seg->data.fc.ptsz;
			
                if (seg->curve!=cv_none) {
					if (seg->type==sg_ceiling) {
						edit_view_draw_fc_curve(ptsz,x,z,y,gx,gy,seg->curve,seg->fill,seg->txt_ang,0.8f,0.8f,0.8f,seg->dark_factor,seg->alpha);
					}
					else {
						edit_view_draw_fc_curve(ptsz,x,z,y,gx,gy,seg->curve,seg->fill,seg->txt_ang,0.3f,0.3f,0.3f,seg->dark_factor,seg->alpha);
					}
					break;
                }
                
				edit_view_draw_segment_poly(ptsz,x,y,z,gx,gy,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
				break;

			case sg_poly:
				edit_view_poly_for_poly(seg,x,y,z,gx,gy,xadd,zadd);

				ptsz=seg->data.fc.ptsz;
			
                if (seg->curve!=cv_none) {
					if (seg->type==sg_ceiling) {
						edit_view_draw_fc_curve(ptsz,x,z,y,gx,gy,seg->curve,seg->fill,seg->txt_ang,0.8f,0.8f,0.8f,seg->dark_factor,seg->alpha);
					}
					else {
						edit_view_draw_fc_curve(ptsz,x,z,y,gx,gy,seg->curve,seg->fill,seg->txt_ang,0.3f,0.3f,0.3f,seg->dark_factor,seg->alpha);
					}
					break;
                }
                
				edit_view_draw_segment_poly(ptsz,x,y,z,gx,gy,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
				break;
				
			case sg_liquid:
				if (!dp_liquid) break;
				
				lft=(seg->data.liquid.lft*map_enlarge)+xadd;
				rgt=(seg->data.liquid.rgt*map_enlarge)+xadd;
				top=(seg->data.liquid.top*map_enlarge)+zadd;
				bot=(seg->data.liquid.bot*map_enlarge)+zadd;

				x_txtoff=seg->x_txtoff;
				x_txtfact=seg->x_txtfact;
				z_txtoff=seg->y_txtoff;
				z_txtfact=seg->y_txtfact;

				x[0]=x[3]=lft;
				x[1]=x[2]=rgt;
				z[0]=z[1]=top;
				z[2]=z[3]=bot;
				y[0]=y[1]=y[2]=y[3]=(seg->data.liquid.y+1)*map_enlarge;
				
				for (t=0;t!=4;t++) {
					gx[t]=x_txtoff+(((x[t]-lft)*x_txtfact)/(rgt-lft));
					gy[t]=z_txtoff+(((z[t]-top)*z_txtfact)/(bot-top));
				}
				
				edit_view_draw_segment_poly(4,x,y,z,gx,gy,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
				break;
					
			case sg_ambient_wall:
				if (!dp_ambient) break;
				
				edit_view_poly_for_ambient_wall(seg,x,y,z,xadd,zadd);
					
				gx[0]=gx[1]=seg->x_txtoff+seg->x_txtfact;
				gx[2]=gx[3]=seg->x_txtoff;
				gy[0]=gy[3]=seg->y_txtoff+seg->y_txtfact;
				gy[1]=gy[2]=seg->y_txtoff;
				
				edit_view_draw_segment_poly(4,x,y,z,gx,gy,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
				break;
				
			case sg_ambient_fc:
				if (!dp_ambient) break;
				
				edit_view_poly_for_ambient_fc(seg,x,y,z,gx,gy,xadd,zadd);
				edit_view_draw_segment_poly(seg->data.ambient_fc.ptsz,x,y,z,gx,gy,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
				break;
				
		}
		
	}

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

	glDepthMask(GL_TRUE);

	glDisable(GL_TEXTURE_2D);
}

void edit_view_draw_portal_segments(int rn,bool opaque)
{
	int				n,cnt;
	short			*sptr;
	segment_type	*seg;

		// compile segments into a list

	cnt=0;
	sptr=edit_seg_list;

	for (n=0;n!=map.nsegment;n++) {
		seg=&map.segments[n];
		if (seg->rn!=rn) continue;
		
		if (opaque) {
			if ((seg->alpha!=1.0f) || (map.textures[seg->fill].bitmaps[0].alpha_mode==alpha_mode_transparent)) continue;
		}
		else {
			if ((seg->alpha==1.0f) && (map.textures[seg->fill].bitmaps[0].alpha_mode!=alpha_mode_transparent)) continue;
		}

		*sptr++=(short)n;
		cnt++;
	}

	if (cnt==0) return;

		// if transparent, then sort list

	if (!opaque) cnt=edit_view_draw_portal_segments_sort(cnt,edit_seg_list,&edit_view_pt);

		// draw list

	edit_view_draw_portal_segments_list(cnt,edit_seg_list,rn,opaque);
}

/* =======================================================

      Edit View Portal Piece Drawing
      
======================================================= */

void edit_view_draw_portal_pieces(int rn)
{
	int				n,xadd,zadd;

	xadd=map.portals[rn].x*map_enlarge;
	zadd=map.portals[rn].z*map_enlarge;
	
		// draw the nodes
	
	if (dp_node) {
		for (n=0;n<map.nnode;n++) {
			if (map.nodes[n].pos.rn==rn) {
				edit_view_draw_sprite(&map.nodes[n].pos,xadd,zadd,node_bitmap.gl_id);
			}
		}
    }
	
		// draw the spots
	
    if (dp_object) {
    
		for (n=0;n<map.nspot;n++) {
			if (map.spots[n].pos.rn==rn) {
				if (!edit_view_model_draw(&map.spots[n].pos,&map.spots[n].ang,map.spots[n].display_model)) {
					edit_view_draw_sprite(&map.spots[n].pos,xadd,zadd,spot_bitmap.gl_id);
				}
			}
		}		
	}
	
		// draw the scenery
	
    if (dp_object) {
    
		for (n=0;n<map.nscenery;n++) {
			if (map.sceneries[n].pos.rn==rn) {
				if (!edit_view_model_draw(&map.sceneries[n].pos,&map.sceneries[n].ang,map.sceneries[n].model_name)) {
					edit_view_draw_sprite(&map.sceneries[n].pos,xadd,zadd,scenery_bitmap.gl_id);
				}
			}
		}		
	}
	
		// draw the lights, sounds, particles
		
	if (dp_lightsoundparticle) {
	
		for (n=0;n<map.nlight;n++) {
			if (map.lights[n].pos.rn==rn) {
				edit_view_draw_sprite(&map.lights[n].pos,xadd,zadd,light_bitmap.gl_id);
			}
		}
		
		for (n=0;n<map.nsound;n++) {
			if (map.sounds[n].pos.rn==rn) {
				edit_view_draw_sprite(&map.sounds[n].pos,xadd,zadd,sound_bitmap.gl_id);
			}
		}
		
		for (n=0;n<map.nparticle;n++) {
			if (map.particles[n].pos.rn==rn) {
				edit_view_draw_sprite(&map.particles[n].pos,xadd,zadd,particle_bitmap.gl_id);
			}
		}
    }
}

/* =======================================================

      Walk View Drawing
      
======================================================= */

void walk_view_gl_setup(d3pnt *pt,d3ang *ang,d3rect *box,int wnd_high)
{
	int				y,wid,high;
	float			ratio;
	
		// translate and remember draw point

	memmove(&edit_view_pt,pt,sizeof(d3pnt));

	edit_view_pt.x*=map_enlarge;
	edit_view_pt.y*=map_enlarge;
	edit_view_pt.z*=map_enlarge;

		// setup the view port

	wid=box->rx-box->lx;
	high=box->by-box->ty;

	y=(wnd_high-box->by);

	glEnable(GL_SCISSOR_TEST);
	glScissor(box->lx,y,wid,high);

	glViewport(box->lx,y,wid,high);

		// setup projection

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	ratio=(float)wid/(float)high;
	
	gluPerspective(edit_gl_fov,ratio,edit_gl_near_z,edit_gl_far_z);
	glScalef(-1.0f,-1.0f,1.0f);						// x and y are reversed
	glTranslatef(0.0f,0.0f,edit_gl_near_offset);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(ang->x,1.0f,0.0f,0.0f);
	glRotatef(((360.0f-ang->y)+180.0f),0.0f,1.0f,0.0f);
}

void edit_view_draw(d3pnt *pt,d3ang *ang,d3rect *box,int wnd_high,bool focus)
{
	int			n,rn,portal_cnt,portal_list[max_portal];
	
        // find draw portal

	rn=map_find_portal(&map,pt->x,pt->y,pt->z);
	if (rn==-1) rn=edit_view_last_portal;

	edit_view_last_portal=rn;
	
		// sort the portals to draw
		// this routine depends on mx/mz being set (this is cached in
		// the engine and could change with edits in the editor) so
		// we need to set it here
		
	for (n=0;n!=map.nportal;n++) {
		map.portals[n].mx=(map.portals[n].x+map.portals[n].ex)/2;
		map.portals[n].mz=(map.portals[n].z+map.portals[n].ez)/2;
		map.portals[n].by=0;
	}
		
	portal_cnt=map_portal_draw_sort(&map,rn,pt->x,pt->y,pt->z,portal_list);

		// setup drawing

	walk_view_gl_setup(pt,ang,box,wnd_high);
	edit_view_sight_path_mark(rn);

        // draw opaque parts of portals in sight path
        
    for (n=0;n!=portal_cnt;n++) {
		edit_view_draw_portal_segments(portal_list[n],TRUE);
		edit_view_draw_portal_pieces(portal_list[n]);
    }
	
        // draw transparent parts of portals in sight path
        
    for (n=0;n!=portal_cnt;n++) {
		edit_view_draw_portal_segments(portal_list[n],FALSE);
    }
	
		// draw selection
		
    for (n=0;n!=portal_cnt;n++) {
//		walk_view_draw_select_portal(portal_list[n]);
    }
	
//	walk_view_draw_segment_handles();

		// key focus

	if (focus) {
		glDisable(GL_BLEND);
		glDisable(GL_ALPHA_TEST);
		glDisable(GL_DEPTH_TEST);

		gluOrtho2D(box->lx,box->rx,box->by,box->ty);

		glLineWidth(4);
		glColor4f(1.0f,0.0f,0.0f,1.0f);

		glBegin(GL_LINE_LOOP);
		glVertex2i(box->lx,box->ty);
		glVertex2i(box->rx,box->ty);
		glVertex2i(box->rx,box->by);
		glVertex2i(box->lx,box->by);
		glEnd();

		glLineWidth(1);
	}

}

