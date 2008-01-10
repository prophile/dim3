/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Draw Piece Routines

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

extern int				cr,cx,cz,cy,txt_palette_high;
extern float			walk_view_fov,walk_view_y_angle,walk_view_x_angle;
extern bool				walk_view_active,main_wind_rot,
						dp_wall,dp_floor,dp_ceiling,dp_liquid,dp_ambient,
						dp_object,dp_lightsoundparticle,dp_node,dp_textured;
extern Rect				walk_view_forward_box,walk_view_side_box;

extern WindowRef		mainwind;
extern AGLContext		ctx;

extern map_type			map;
extern bitmap_type		spot_bitmap,scenery_bitmap,node_bitmap,
						light_bitmap,sound_bitmap,particle_bitmap;

int						old_gl_id;

/* =======================================================

      Mark Portals in Sight Path
      
======================================================= */

void walk_view_sight_path_trace(portal_sight_list_type *sight,int u)
{
	int			i,t,rn;
	
	rn=sight[u].rn;					// add portal to sorting list
    
        // room already in path?
        
    if (map.portals[rn].in_path) return;

	map.portals[rn].in_path=TRUE;

	for (i=0;i!=max_sight_link;i++) {									// continue down the path
		t=(int)sight[u].link[i];
		if (t!=nopath) walk_view_sight_path_trace(sight,t);
	}
}

void walk_view_sight_path_mark(int rn)
{
	int				i;
	portal_sight_list_type		*sight;
	
        // clear all portals from path
        
    for (i=0;i!=map.nportal;i++) {
        map.portals[i].in_path=FALSE;
    }
					
		// start with current room
	
    map.portals[rn].in_path=TRUE;

		// add in portals sighted with linked room list

	sight=map.portals[rn].sight;
	
	for (i=0;i!=max_sight_list;i++) {
        if (sight[i].root) walk_view_sight_path_trace(sight,i);
	}
}


/* =======================================================

      Walk View Piece Drawing
      
======================================================= */

void walk_view_draw_textured_poly(int ptsz,int *x,int *y,int *z,float *gx,float *gy,int txt,int txt_ang,float darken,float alpha)
{
	register int			n;
	int						gl_id;
	
	gl_id=map.textures[txt].bitmaps[0].gl_id;
	
	if (old_gl_id!=gl_id) {
    	glBindTexture(GL_TEXTURE_2D,gl_id);
		old_gl_id=gl_id;
	}
	
	if (map.textures[txt].additive) {
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
				
		glVertex3i((x[n]-cx),(y[n]-cy),(cz-z[n]));
	}
	
	glEnd();
}

void walk_view_draw_colored_poly(int ptsz,int *x,int *y,int *z,float r,float g,float b,float alpha)
{
	register int			n;

    glColor4f(r,g,b,alpha);
	
	glBegin(GL_POLYGON);

	for (n=0;n<ptsz;n++) {
		glVertex3i((x[n]-cx),(y[n]-cy),(cz-z[n]));
	}
	
	glEnd();
	
	glColor4f(0,0,0,alpha);
	glLineWidth(2);
	
	glBegin(GL_LINE_LOOP);
	
	for (n=0;n<ptsz;n++) {
		glVertex3i((x[n]-cx),(y[n]-cy),((cz-z[n])-25));
	}
	
	glEnd();
	
	glLineWidth(1);
}

void walk_view_draw_sprite(d3pos *pos,int xadd,int zadd,int txt)
{
    int			x,y,z,wid,high;
  
    x=(pos->x+xadd)-cx;
    y=(pos->y+1)-cy;
    z=(pos->z+zadd)-cz;
    
    wid=map_enlarge*3;
    high=map_enlarge*4;
    
    glBindTexture(GL_TEXTURE_2D,txt);
    
	glColor4f(1,1,1,1);
	
	glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
	glVertex3i((x-wid),(y-high),-(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x+wid),(y-high),-(z-wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),y,-(z-wid));
    glTexCoord2f(0,1);
	glVertex3i((x-wid),y,-(z-wid));
	glEnd();

	glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
	glVertex3i((x-wid),(y-high),-(z+wid));
    glTexCoord2f(1,0);
	glVertex3i((x+wid),(y-high),-(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),y,-(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x-wid),y,-(z+wid));
	glEnd();
	
	glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
	glVertex3i((x-wid),(y-high),-(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x-wid),(y-high),-(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x-wid),y,-(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x-wid),y,-(z-wid));
	glEnd();
	
	glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
	glVertex3i((x+wid),(y-high),-(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x+wid),(y-high),-(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),y,-(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x+wid),y,-(z-wid));
	glEnd();
	
	glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
	glVertex3i((x-wid),(y-high),-(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x-wid),(y-high),-(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),(y-high),-(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x+wid),(y-high),-(z-wid));
	glEnd();
	
	glBegin(GL_POLYGON);
    glTexCoord2f(0,0);
	glVertex3i((x-wid),y,-(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x-wid),y,-(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),y,-(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x+wid),y,-(z-wid));
	glEnd();
}

/* =======================================================

      Walk View Portal Drawing
      
======================================================= */

void walk_view_draw_portal_segments(int rn,bool opaque)
{
//	register int			i,t;	// supergumba
//	register segment_type	*seg;
//	int						xadd,zadd,ptsz,x[8],z[8],y[8],lft,rgt,top,bot;
//	float					gx[8],gy[8],
//							x_txtoff,z_txtoff,x_txtfact,z_txtfact;
	int					n,k,t,x,y,z,xadd,zadd;
	d3pnt				*pt;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;
							
	if (dp_textured) glEnable(GL_TEXTURE_2D);
	
	old_gl_id=-1;
	
	xadd=map.portals[rn].x;
	zadd=map.portals[rn].z;

		// no depth buffer for transparent segments
		
	if (opaque) glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glEnable(GL_DEPTH_TEST);
	
	if (!opaque) glDepthMask(GL_FALSE);
	
	
	
	
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	
	portal=&map.portals[rn];
	
	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		for (k=0;k!=mesh->npoly;k++) {
		
			mesh_poly=&mesh->polys[k];
			texture=&map.textures[mesh_poly->txt_idx];
		
			if (opaque) {
				if ((mesh_poly->alpha!=1.0f) || (texture->bitmaps[0].alpha_mode==alpha_mode_transparent)) continue;
			}
			else {
				if ((mesh_poly->alpha==1.0f) && (texture->bitmaps[0].alpha_mode!=alpha_mode_transparent)) continue;
			}
		
			glBindTexture(GL_TEXTURE_2D,texture->bitmaps[0].gl_id);
		
			glBegin(GL_POLYGON);
			
			for (t=0;t!=mesh_poly->ptsz;t++) {
				pt=&mesh->vertexes[mesh_poly->v[t]];
				x=(pt->x+xadd)-cx;
				y=pt->y-cy;
				z=cz-(pt->z+zadd);
				glTexCoord2f(mesh_poly->gx[t],mesh_poly->gy[t]);
				glVertex3i(x,y,z);
			}
			
			glEnd();
		}
	
	
		mesh++;
	}

	glDisable(GL_TEXTURE_2D);



/*
	
		// draw segments
		
	for (i=0;i!=map.nsegment;i++) {
		seg=&map.segments[i];
		if (seg->rn!=rn) continue;
		
		if (opaque) {
			if ((seg->alpha!=1.0f) || (map.textures[seg->fill].bitmaps[0].alpha_mode==alpha_mode_transparent)) continue;
		}
		else {
			if ((seg->alpha==1.0f) && (map.textures[seg->fill].bitmaps[0].alpha_mode!=alpha_mode_transparent)) continue;
		}
		
		switch (seg->type) {
		
			case sg_wall:
				if (!dp_wall) break;
                
                if ((seg->clip==wc_top_curve) || (seg->clip==wc_top_arch) || (seg->clip==wc_bottom_curve) || (seg->clip==wc_bottom_arch) || (seg->clip==wc_top_curve_arch) || (seg->clip==wc_bottom_curve_arch)) {
                    walk_view_draw_wall_clip_curve(seg,xadd,zadd,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
                    break;
                }
                
                if (seg->curve!=cv_none) {
                    walk_view_draw_wall_curve(seg,xadd,zadd,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
                    break;
				}
                
				ptsz=walk_view_get_wall_poly(seg,x,z,y,gx,gy,xadd,zadd);
				
				if (dp_textured) {
					walk_view_draw_textured_poly(ptsz,x,y,z,gx,gy,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
					break;
				}

				walk_view_draw_colored_poly(ptsz,x,y,z,0.3f,0.3f,1.0f,seg->alpha);
				break;
				
			case sg_floor:
			case sg_ceiling:
				if ((seg->type==sg_floor) && (!dp_floor)) break;
				if ((seg->type==sg_ceiling) && (!dp_ceiling)) break;
				
				ptsz=seg->data.fc.ptsz;

				poly_find_bound_rect(ptsz,seg->data.fc.x,seg->data.fc.z,&lft,&rgt,&top,&bot);
				lft=lft*map_enlarge;
				rgt=rgt*map_enlarge;
				top=top*map_enlarge;
				bot=bot*map_enlarge;
				
				x_txtoff=seg->x_txtoff;
				x_txtfact=seg->x_txtfact;
				z_txtoff=seg->y_txtoff;
				z_txtfact=seg->y_txtfact;
				
				for (t=0;t<ptsz;t++) {
					x[t]=seg->data.fc.x[t]*map_enlarge;
					z[t]=seg->data.fc.z[t]*map_enlarge;
					y[t]=seg->data.fc.y[t]*map_enlarge;

					gx[t]=x_txtoff+(((float)(x[t]-lft)*x_txtfact)/(float)(rgt-lft));
					gy[t]=z_txtoff+(((float)(z[t]-top)*z_txtfact)/(float)(bot-top));
					
                    x[t]+=xadd;
                    z[t]+=zadd;
				}
			
                if (seg->curve!=cv_none) {
					if (seg->type==sg_ceiling) {
						walk_view_draw_fc_curve(ptsz,x,z,y,gx,gy,seg->curve,seg->fill,seg->txt_ang,0.8f,0.8f,0.8f,seg->dark_factor,seg->alpha);
					}
					else {
						walk_view_draw_fc_curve(ptsz,x,z,y,gx,gy,seg->curve,seg->fill,seg->txt_ang,0.3f,0.3f,0.3f,seg->dark_factor,seg->alpha);
					}
					break;
                }
                
				if (dp_textured) {
					walk_view_draw_textured_poly(ptsz,x,y,z,gx,gy,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
					break;
				}

				if (seg->type==sg_ceiling) {
					walk_view_draw_colored_poly(ptsz,x,y,z,0.8f,0.8f,0.8f,seg->alpha);
				}
				else {
					walk_view_draw_colored_poly(ptsz,x,y,z,0.3f,0.3f,0.3f,seg->alpha);
				}
				
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
				
				if (dp_textured) {
					for (t=0;t!=4;t++) {
						gx[t]=x_txtoff+(((x[t]-lft)*x_txtfact)/(rgt-lft));
						gy[t]=z_txtoff+(((z[t]-top)*z_txtfact)/(bot-top));
					}
					
					walk_view_draw_textured_poly(4,x,y,z,gx,gy,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
					break;
				}
				
				walk_view_draw_colored_poly(4,x,y,z,1.0f,1.0f,0.0f,seg->alpha);
				break;
					
			case sg_ambient_wall:
				if (!dp_ambient) break;
				
				walk_view_get_ambient_wall_poly(seg,x,z,y,xadd,zadd);
					
				if (dp_textured) {
					gx[0]=gx[1]=seg->x_txtoff+seg->x_txtfact;
					gx[2]=gx[3]=seg->x_txtoff;
					gy[0]=gy[3]=seg->y_txtoff+seg->y_txtfact;
					gy[1]=gy[2]=seg->y_txtoff;
					
					walk_view_draw_textured_poly(4,x,y,z,gx,gy,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
					break;
				}
				
				walk_view_draw_colored_poly(4,x,y,z,1.0f,0.0f,1.0f,seg->alpha);
				break;
				
			case sg_ambient_fc:
				if (!dp_ambient) break;
				
				walk_view_get_ambient_fc_poly(seg,x,z,y,gx,gy,xadd,zadd);
				
				if (dp_textured) {
					walk_view_draw_textured_poly(seg->data.ambient_fc.ptsz,x,y,z,gx,gy,seg->fill,seg->txt_ang,seg->dark_factor,seg->alpha);
					break;
				}
				
				walk_view_draw_colored_poly(seg->data.ambient_fc.ptsz,x,y,z,1.0f,0.0f,1.0f,seg->alpha);
				break;
				
		}
		
	}
	*/
	
	if (!opaque) glDepthMask(GL_TRUE);
	
	glDisable(GL_ALPHA_TEST);
	if (opaque) glEnable(GL_BLEND);
	
	if (dp_textured) glDisable(GL_TEXTURE_2D);
}

void walk_view_draw_portal_pieces(int rn)
{
	register int			i;
	int						xadd,zadd;

	glEnable(GL_TEXTURE_2D);
	
	xadd=map.portals[rn].x;
	zadd=map.portals[rn].z;
	
		// draw the nodes
	
	if (dp_node) {
		for (i=0;i<map.nnode;i++) {
			if (map.nodes[i].pos.rn==rn) {
				walk_view_draw_sprite(&map.nodes[i].pos,xadd,zadd,node_bitmap.gl_id);
			}
		}
    }
	
		// draw the spots
	
    if (dp_object) {
    
		for (i=0;i<map.nspot;i++) {
			if (map.spots[i].pos.rn==rn) {
				if (!walk_view_model_draw(&map.spots[i].pos,&map.spots[i].ang,map.spots[i].display_model)) {
					walk_view_draw_sprite(&map.spots[i].pos,xadd,zadd,spot_bitmap.gl_id);
				}
			}
		}		
	}
	
		// draw the scenery
	
    if (dp_object) {
    
		for (i=0;i<map.nscenery;i++) {
			if (map.sceneries[i].pos.rn==rn) {
				if (!walk_view_model_draw(&map.sceneries[i].pos,&map.sceneries[i].ang,map.sceneries[i].model_name)) {
					walk_view_draw_sprite(&map.sceneries[i].pos,xadd,zadd,scenery_bitmap.gl_id);
				}
			}
		}		
	}
	
		// draw the lights, sounds, particles
		
	if (dp_lightsoundparticle) {
	
		for (i=0;i<map.nlight;i++) {
			if (map.lights[i].pos.rn==rn) {
				walk_view_draw_sprite(&map.lights[i].pos,xadd,zadd,light_bitmap.gl_id);
			}
		}
		
		for (i=0;i<map.nsound;i++) {
			if (map.sounds[i].pos.rn==rn) {
				walk_view_draw_sprite(&map.sounds[i].pos,xadd,zadd,sound_bitmap.gl_id);
			}
		}
		
		for (i=0;i<map.nparticle;i++) {
			if (map.particles[i].pos.rn==rn) {
				walk_view_draw_sprite(&map.particles[i].pos,xadd,zadd,particle_bitmap.gl_id);
			}
		}
    }
	
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Walk View Drawing
      
======================================================= */

void walk_view_gl_setup(bool on_side)
{
	float			ang_y;
	
		// y view angle
		
	ang_y=walk_view_y_angle;
	if (on_side) ang_y=angle_add(ang_y,90.0f);
	ang_y=(360.0f-ang_y)+180.0f;
	
		// viewport setup
		
	if (!on_side) {
		main_wind_set_viewport(&walk_view_forward_box);
		main_wind_set_3D_projection(&walk_view_forward_box,walk_view_x_angle,ang_y,walk_view_fov,walk_view_near_z,walk_view_far_z,walk_view_near_offset);
	}
	else {
		main_wind_set_viewport(&walk_view_side_box);
		main_wind_set_3D_projection(&walk_view_forward_box,walk_view_x_angle,ang_y,walk_view_fov,walk_view_near_z,walk_view_far_z,walk_view_near_offset);
	}

	glEnable(GL_DEPTH_TEST);
	
	walk_view_sight_path_mark(cr);
}

void walk_view_draw(bool on_side)
{
	int			i,rn,
				portal_cnt,portal_list[max_portal];
	
        // 3D view
        
	walk_view_gl_setup(on_side);
	
		// get the portals to draw
		
	for (i=0;i!=map.nportal;i++) {			// routine depends on mx/mz being set
		map.portals[i].mx=(map.portals[i].x+map.portals[i].ex)/2;
		map.portals[i].mz=(map.portals[i].z+map.portals[i].ez)/2;
		map.portals[i].by=0;
	}
		
	portal_cnt=map_portal_draw_sort(&map,cr,cx,cy,cz,portal_list);

        // draw opaque parts of portals in sight path
        
    for (i=0;i!=portal_cnt;i++) {
		rn=portal_list[i];
        walk_view_draw_portal_segments(rn,TRUE);
		walk_view_draw_portal_pieces(rn);
    }
	
        // draw transparent parts of portals in sight path
        
    for (i=0;i!=portal_cnt;i++) {
		rn=portal_list[i];
        walk_view_draw_portal_segments(rn,FALSE);
    }
	
		// draw selection
		
    for (i=0;i!=portal_cnt;i++) {
		rn=portal_list[i];
		walk_view_draw_select_portal(rn);
    }
	
	walk_view_draw_segment_handles();
	
		// draw compass
		
	walk_view_compass_draw(on_side);
}

