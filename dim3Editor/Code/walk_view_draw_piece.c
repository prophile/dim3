/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Walk View Draw Piece Routines

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

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern int				cr,cy,txt_palette_high;
extern float			walk_view_fov,walk_view_y_angle,walk_view_x_angle;
extern bool				dp_liquid,dp_object,dp_lightsoundparticle,dp_node,dp_textured;

extern WindowRef		mainwind;
extern AGLContext		ctx;

extern map_type			map;
extern bitmap_type		spot_bitmap,scenery_bitmap,node_bitmap,
						light_bitmap,sound_bitmap,particle_bitmap;

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

      Walk View Sprites Drawing
      
======================================================= */

void walk_view_draw_sprite(d3pnt *cpt,d3pos *pos,float ang_y,unsigned long gl_id)
{
    int			x,y,z,px[4],pz[4],wid,high;
	portal_type	*portal;
	
	portal=&map.portals[pos->rn];
	
    x=(pos->x+portal->x)-cpt->x;
    y=(pos->y+1)-cpt->y;
    z=(pos->z+portal->z)-cpt->z;
    
    wid=map_enlarge*3;
    high=map_enlarge*4;
	
	px[0]=px[3]=x-wid;
	px[1]=px[2]=x+wid;
	pz[0]=pz[1]=z-wid;
	pz[2]=pz[3]=z+wid;
	
	rotate_2D_polygon(4,px,pz,x,z,ang_y);
    
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,gl_id);
    
	glColor4f(1,1,1,1);
	
	glBegin(GL_QUADS);

    glTexCoord2f(0,0);
	glVertex3i(px[0],(y-high),-pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],(y-high),-pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[1],y,-pz[1]);
    glTexCoord2f(0,1);
	glVertex3i(px[0],y,-pz[0]);

    glTexCoord2f(0,0);
	glVertex3i(px[3],(y-high),-pz[3]);
    glTexCoord2f(1,0);
	glVertex3i(px[2],(y-high),-pz[2]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],y,-pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],y,-pz[3]);

    glTexCoord2f(0,0);
	glVertex3i(px[0],(y-high),-pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[3],(y-high),-pz[3]);
    glTexCoord2f(1,1);
	glVertex3i(px[3],y,-pz[3]);
    glTexCoord2f(0,1);
	glVertex3i(px[0],y,-pz[0]);

    glTexCoord2f(0,0);
	glVertex3i(px[1],(y-high),-pz[1]);
    glTexCoord2f(1,0);
	glVertex3i(px[2],(y-high),-pz[2]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],y,-pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[1],y,-pz[1]);

    glTexCoord2f(0,0);
	glVertex3i(px[0],(y-high),-pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],(y-high),-pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],(y-high),-pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],(y-high),-pz[3]);

    glTexCoord2f(0,0);
	glVertex3i(px[0],y,-pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],y,-pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],y,-pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],y,-pz[3]);
	
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
}

void walk_view_draw_circle(d3pnt *cpt,d3pos *pos,d3col *col,int dist)
{
    int				n,x,y,z,kx,kz;
	portal_type		*portal;
	
	portal=&map.portals[pos->rn];
	
    x=(pos->x+portal->x)-cpt->x;
    y=(pos->y+1)-cpt->y;
    z=(pos->z+portal->z)-cpt->z;
	
	glLineWidth(4.0f);
	glColor4f(col->r,col->g,col->b,0.75f);
	
	glBegin(GL_LINE_LOOP);
	
	for (n=0;n!=360;n+=10) {
		kx=dist;
		kz=0;
		rotate_2D_point_center(&kx,&kz,(float)n);
		glVertex3i((x+kx),y,-(z+kz));
	}

	glEnd();
	
	glLineWidth(1.0f);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
}

/* =======================================================

      Walk View Portal Block
      
======================================================= */

void walk_view_draw_portal_block(int rn,d3pnt *cpt,int y)
{
	int				lx,rx,tz,bz,ty,by;
	portal_type		*portal;
	
		// portal size
		
	portal=&map.portals[rn];
	
	lx=portal->x-cpt->x;
	rx=portal->ex-cpt->x;
	tz=cpt->z-portal->z;
	bz=cpt->z-portal->ez;
	
	map_portal_calculate_y_extent(&map,rn,&ty,&by);

	if ((ty==0) && (by==0)) {
		ty=by=y-cpt->y;			// portal with no Y
	}
	else {
		ty-=cpt->y;
		by-=cpt->y;
	}
	
	glDisable(GL_DEPTH_TEST);
	
		// portal block
		
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	glBegin(GL_QUADS);
	glVertex3i(lx,by,tz);
	glVertex3i(rx,by,tz);
	glVertex3i(rx,by,bz);
	glVertex3i(lx,by,bz);
	glEnd();
	
		// outline
		
	glColor4f(0.5f,0.5f,0.5f,1.0f);
	
	glBegin(GL_LINE_LOOP);
	glVertex3i(lx,by,tz);
	glVertex3i(rx,by,tz);
	glVertex3i(rx,by,bz);
	glVertex3i(lx,by,bz);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3i(lx,ty,tz);
	glVertex3i(rx,ty,tz);
	glVertex3i(rx,ty,bz);
	glVertex3i(lx,ty,bz);
	glEnd();
	
	glBegin(GL_LINES);
	glVertex3i(lx,by,tz);
	glVertex3i(lx,ty,tz);
	glVertex3i(rx,by,tz);
	glVertex3i(rx,ty,tz);
	glVertex3i(lx,by,bz);
	glVertex3i(lx,ty,bz);
	glVertex3i(rx,by,bz);
	glVertex3i(rx,ty,bz);
	glEnd();
	
		// selection
		
	if (rn==cr) {
		glColor4f(1.0f,0.0f,1.0f,0.7f);
		glLineWidth(4.0f);
		
		glBegin(GL_LINE_LOOP);
		glVertex3i(lx,by,tz);
		glVertex3i(rx,by,tz);
		glVertex3i(rx,by,bz);
		glVertex3i(lx,by,bz);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3i(lx,ty,tz);
		glVertex3i(rx,ty,tz);
		glVertex3i(rx,ty,bz);
		glVertex3i(lx,ty,bz);
		glEnd();
		
		glBegin(GL_LINES);
		glVertex3i(lx,by,tz);
		glVertex3i(lx,ty,tz);
		glVertex3i(rx,by,tz);
		glVertex3i(rx,ty,tz);
		glVertex3i(lx,by,bz);
		glVertex3i(lx,ty,bz);
		glVertex3i(rx,by,bz);
		glVertex3i(rx,ty,bz);
		glEnd();
		
		glLineWidth(1.0f);
	}
}

/* =======================================================

      Walk View Portal Drawing
      
======================================================= */

void walk_view_draw_portal_meshes_texture(int rn,d3pnt *cpt,int clip_y,bool opaque)
{
	int					n,k,t,x,y,z;
	unsigned long		old_gl_id;
	float				dark_factor;
	bool				clip_ok;
	d3pnt				*pt;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;
							
		// no depth buffer for transparent segments
		
	glEnable(GL_TEXTURE_2D);

	if (opaque) glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glEnable(GL_DEPTH_TEST);
	if (!opaque) glDepthMask(GL_FALSE);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	old_gl_id=-1;
	dark_factor=1.0f;

		// draw portal meshes
		
	portal=&map.portals[rn];
	
	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		for (k=0;k!=mesh->npoly;k++) {
		
			mesh_poly=&mesh->polys[k];
			texture=&map.textures[mesh_poly->txt_idx];
		
				// opaque or transparent flag
				
			if (opaque) {
				if ((mesh_poly->alpha!=1.0f) || (texture->bitmaps[0].alpha_mode==alpha_mode_transparent)) continue;
			}
			else {
				if ((mesh_poly->alpha==1.0f) && (texture->bitmaps[0].alpha_mode!=alpha_mode_transparent)) continue;
			}
			
				// y clipping
				
			if (clip_y!=-1) {
			
				clip_ok=TRUE;
				
				for (t=0;t!=mesh_poly->ptsz;t++) {
					if (mesh->vertexes[mesh_poly->v[t]].y>=clip_y) {
						clip_ok=FALSE;
						break;
					}
				}
				
				if (clip_ok) continue;
			}
		
				// setup texture
				
			if (texture->bitmaps[0].gl_id!=old_gl_id) {
				old_gl_id=texture->bitmaps[0].gl_id;
				glBindTexture(GL_TEXTURE_2D,old_gl_id);
			}
			
				// darking
				
			if (mesh_poly->dark_factor!=dark_factor) {
				dark_factor=mesh_poly->dark_factor;
				glColor4f(dark_factor,dark_factor,dark_factor,1.0f);
			}
		
				// draw polygon
				
			glBegin(GL_POLYGON);
			
			for (t=0;t!=mesh_poly->ptsz;t++) {
				pt=&mesh->vertexes[mesh_poly->v[t]];
				x=(pt->x+portal->x)-cpt->x;
				y=pt->y-cpt->y;
				z=cpt->z-(pt->z+portal->z);
				glTexCoord2f(mesh_poly->gx[t],mesh_poly->gy[t]);
				glVertex3i(x,y,z);
			}
			
			glEnd();
		}
	
		mesh++;
	}
	
	if (!opaque) glDepthMask(GL_TRUE);
}

void walk_view_draw_portal_meshes_line(int rn,d3pnt *cpt,bool opaque)
{
	int					n,k,t,x,y,z;
	d3pnt				*pt;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;
							
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	
	glColor4f(0.5f,0.5f,1.0f,1.0f);
	
		// draw portal mesh lines
		
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
		
			glBegin(GL_LINE_LOOP);
			
			for (t=0;t!=mesh_poly->ptsz;t++) {
				pt=&mesh->vertexes[mesh_poly->v[t]];
				x=(pt->x+portal->x)-cpt->x;
				y=pt->y-cpt->y;
				z=cpt->z-(pt->z+portal->z);
				glVertex3i(x,y,z);
			}
			
			glEnd();
		}
	
	
		mesh++;
	}
}

void walk_view_draw_portal_liquids(int rn,d3pnt *cpt,bool opaque)
{
	int					n,nliquid,x,y,z;
	unsigned long		old_gl_id;
	portal_type			*portal;
	texture_type		*texture;
	map_liquid_type		*liquid;
	
	if (!dp_liquid) return;
	
		// no depth buffer for transparent segments
		
	glEnable(GL_TEXTURE_2D);

	if (opaque) glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glEnable(GL_DEPTH_TEST);
	if (!opaque) glDepthMask(GL_FALSE);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	old_gl_id=-1;

		// run through the portal liquids
		
	portal=&map.portals[rn];
	
	nliquid=portal->liquid.nliquid;
	liquid=portal->liquid.liquids;
	
	glEnable(GL_TEXTURE_2D);
	
	for (n=0;n!=nliquid;n++) {
		texture=&map.textures[liquid->txt_idx];
	
		if (opaque) {
			if ((liquid->alpha!=1.0f) || (texture->bitmaps[0].alpha_mode==alpha_mode_transparent)) continue;
		}
		else {
			if ((liquid->alpha==1.0f) && (texture->bitmaps[0].alpha_mode!=alpha_mode_transparent)) continue;
		}
		
		if (texture->bitmaps[0].gl_id!=old_gl_id) {
			old_gl_id=texture->bitmaps[0].gl_id;
			glBindTexture(GL_TEXTURE_2D,old_gl_id);
		}
		
		y=liquid->y-cpt->y;

		glBegin(GL_QUADS);

		x=(liquid->lft+portal->x)-cpt->x;
		z=cpt->z-(liquid->top+portal->z);
		glTexCoord2f(liquid->x_txtoff,liquid->y_txtoff);
		glVertex3i(x,y,z);

		x=(liquid->rgt+portal->x)-cpt->x;
		z=cpt->z-(liquid->top+portal->z);
		glTexCoord2f((liquid->x_txtoff+liquid->x_txtfact),liquid->y_txtoff);
		glVertex3i(x,y,z);
		
		x=(liquid->rgt+portal->x)-cpt->x;
		z=cpt->z-(liquid->bot+portal->z);
		glTexCoord2f((liquid->x_txtoff+liquid->x_txtfact),(liquid->y_txtoff+liquid->y_txtfact));
		glVertex3i(x,y,z);
		
		x=(liquid->lft+portal->x)-cpt->x;
		z=cpt->z-(liquid->bot+portal->z);
		glTexCoord2f(liquid->x_txtoff,(liquid->y_txtoff+liquid->y_txtfact));
		glVertex3i(x,y,z);

		glEnd();
		
		liquid++;
	}
	
	if (!opaque) glDepthMask(GL_TRUE);
	
	glDisable(GL_ALPHA_TEST);
	if (opaque) glEnable(GL_BLEND);
	
	glDisable(GL_TEXTURE_2D);
}

void walk_view_draw_portal_nodes(int rn,d3pnt *cpt)
{
	int			n,k;
	portal_type	*portal,*lportal;
	node_type	*node,*lnode;

	if (!dp_node) return;
	
		// connections
		
	portal=&map.portals[rn];
		
	glLineWidth(3.0f);
	glColor4f(0.0f,1.0f,0.0f,1.0f);
	
	glBegin(GL_LINES);
		
	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
	
		if (node->pos.rn==rn) {
			
			for (k=0;k!=max_node_link;k++) {
			
				if (node->link[k]!=-1) {
					glVertex3i(((node->pos.x+portal->x)-cpt->x),((node->pos.y-(map_enlarge*2))-cpt->y),(cpt->z-(node->pos.z+portal->z)));
					lnode=&map.nodes[node->link[k]];
					lportal=&map.portals[lnode->pos.rn];
					glVertex3i(((lnode->pos.x+lportal->x)-cpt->x),((lnode->pos.y-(map_enlarge*2))-cpt->y),(cpt->z-(lnode->pos.z+lportal->z)));
				}
				
			}
		}
		
		node++;
	}
	
	glEnd();
	
	glLineWidth(1.0f);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
		
		// nodes
	
	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
	
		if (node->pos.rn==rn) {
			walk_view_draw_sprite(cpt,&node->pos,0.0f,node_bitmap.gl_id);
		}
		
		node++;
	}
}

void walk_view_draw_portal_spots_scenery(int rn,d3pnt *cpt)
{
	int			n;
	
    if (!dp_object) return;
    
	for (n=0;n!=map.nspot;n++) {
		if (map.spots[n].pos.rn==rn) {
			if (!walk_view_model_draw(cpt,&map.spots[n].pos,&map.spots[n].ang,map.spots[n].display_model)) {
				walk_view_draw_sprite(cpt,&map.spots[n].pos,map.spots[n].ang.y,spot_bitmap.gl_id);
			}
		}
	}		
    
	for (n=0;n!=map.nscenery;n++) {
		if (map.sceneries[n].pos.rn==rn) {
			if (!walk_view_model_draw(cpt,&map.sceneries[n].pos,&map.sceneries[n].ang,map.sceneries[n].model_name)) {
				walk_view_draw_sprite(cpt,&map.sceneries[n].pos,map.sceneries[n].ang.y,scenery_bitmap.gl_id);
			}
		}
	}		
}

void walk_view_draw_portal_lights_sounds_particles(int rn,d3pnt *cpt,bool draw_light_circle)
{
	int				n;
	
	if (!dp_lightsoundparticle) return;
	
	for (n=0;n!=map.nlight;n++) {
		if (map.lights[n].pos.rn==rn) {
			if (draw_light_circle) walk_view_draw_circle(cpt,&map.lights[n].pos,&map.lights[n].col,map.lights[n].intensity);
			walk_view_draw_sprite(cpt,&map.lights[n].pos,0.0f,light_bitmap.gl_id);
		}
	}
	
	for (n=0;n!=map.nsound;n++) {
		if (map.sounds[n].pos.rn==rn) {
			walk_view_draw_sprite(cpt,&map.sounds[n].pos,0.0f,sound_bitmap.gl_id);
		}
	}
	
	for (n=0;n!=map.nparticle;n++) {
		if (map.particles[n].pos.rn==rn) {
			walk_view_draw_sprite(cpt,&map.particles[n].pos,0.0f,particle_bitmap.gl_id);
		}
	}
}

/* =======================================================

      Walk View Drawing Setup
      
======================================================= */

void walk_view_gl_setup(editor_3D_view_setup *view_setup)
{
	main_wind_set_viewport(&view_setup->box,0.75f);
	main_wind_set_3D_projection(&view_setup->box,&view_setup->ang,view_setup->fov,walk_view_near_z,walk_view_far_z,walk_view_near_offset);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

/* =======================================================

      Draw Current Position
      
======================================================= */

void walk_view_draw_position(Rect *box,d3pnt *cpt)
{
    int			x,z,sz;
   
 	main_wind_set_2D_projection(box);

	glDisable(GL_DEPTH_TEST);

	x=box->left+((box->right-box->left)>>1);
	z=box->top+((box->bottom-box->top)>>1);
	
	sz=10;
	
	glColor4f(0.0f,0.0f,0.0f,0.75f);

	glLineWidth(3.0f);
	
	glBegin(GL_LINES);
	glVertex2i(x,(z-sz));
	glVertex2i(x,(z+sz));
	glVertex2i((x-sz),z);
	glVertex2i((x+sz),z);
	glEnd();
	
	glLineWidth(1.0f);
}

/* =======================================================

      Draw Walk View
      
======================================================= */

void walk_view_draw(editor_3D_view_setup *view_setup,bool draw_position)
{
	int			i,rn,clip_y,
				portal_cnt,portal_list[max_portal];
	
 		// always draw from portal that cursor is in
		
	rn=walk_view_find_start_portal();
	walk_view_sight_path_mark(rn);
		
       // 3D view
        
	main_wind_set_viewport(&view_setup->box,0.75f);
	main_wind_set_3D_projection(&view_setup->box,&view_setup->ang,view_setup->fov,walk_view_near_z,walk_view_far_z,walk_view_near_offset);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
		// get the portals to draw
		
	if (view_setup->ignore_site_path) {
	
		for (i=0;i!=map.nportal;i++) {
			portal_list[i]=i;
		}
		
		portal_cnt=map.nportal;
	
	}
	else {
	
		for (i=0;i!=map.nportal;i++) {			// routine depends on mx/mz being set
			map.portals[i].mx=(map.portals[i].x+map.portals[i].ex)/2;
			map.portals[i].mz=(map.portals[i].z+map.portals[i].ez)/2;
			map.portals[i].by=0;
		}
		
		portal_cnt=map_portal_draw_sort(&map,rn,view_setup->cpt.x,view_setup->cpt.y,view_setup->cpt.z,portal_list);
	}
	
		// draw the portal outlines
		
	if (view_setup->draw_portal) {
		for (i=0;i!=portal_cnt;i++) {
			walk_view_draw_portal_block(portal_list[i],&view_setup->cpt,view_setup->portal_y);
		}
	}
	
		// y cipping
		
	clip_y=-1;
	if (view_setup->clip_on) clip_y=view_setup->clip_y;

        // draw opaque parts of portals in sight path
        
    for (i=0;i!=portal_cnt;i++) {
		rn=portal_list[i];
        if (!view_setup->mesh_only) walk_view_draw_portal_meshes_texture(rn,&view_setup->cpt,clip_y,TRUE);
 		walk_view_draw_portal_nodes(rn,&view_setup->cpt);
		walk_view_draw_portal_spots_scenery(rn,&view_setup->cpt);
		walk_view_draw_portal_lights_sounds_particles(rn,&view_setup->cpt,view_setup->draw_light_circle);
		walk_view_draw_portal_liquids(rn,&view_setup->cpt,TRUE);
    }
	
		// draw opaque mesh lines
		// push view forward to better z-buffer lines
		
	main_wind_set_3D_projection(&view_setup->box,&view_setup->ang,view_setup->fov,(walk_view_near_z+10),(walk_view_far_z-10),walk_view_near_offset);
 
	for (i=0;i!=portal_cnt;i++) {
		rn=portal_list[i];
        walk_view_draw_portal_meshes_line(rn,&view_setup->cpt,TRUE);
    }
	
        // draw transparent parts of portals in sight path
        
	main_wind_set_3D_projection(&view_setup->box,&view_setup->ang,view_setup->fov,walk_view_near_z,walk_view_far_z,walk_view_near_offset);

    for (i=0;i!=portal_cnt;i++) {
		rn=portal_list[i];
        if (!view_setup->mesh_only) walk_view_draw_portal_meshes_texture(rn,&view_setup->cpt,clip_y,FALSE);
		walk_view_draw_portal_liquids(rn,&view_setup->cpt,FALSE);
    }
	
        // draw transparent mesh lines
		// push view forward to better z-buffer lines
        
	main_wind_set_3D_projection(&view_setup->box,&view_setup->ang,view_setup->fov,(walk_view_near_z+10),(walk_view_far_z-10),walk_view_near_offset);

    for (i=0;i!=portal_cnt;i++) {
		rn=portal_list[i];
		walk_view_draw_portal_meshes_line(rn,&view_setup->cpt,TRUE);
    }
		
		// draw selection
		
    for (i=0;i!=portal_cnt;i++) {
		rn=portal_list[i];
		walk_view_draw_select_portal(rn,&view_setup->cpt);
    }
	
	main_wind_set_3D_projection(&view_setup->box,&view_setup->ang,view_setup->fov,(walk_view_near_z+20),(walk_view_far_z-20),walk_view_near_offset);

		// position
		
	main_wind_set_3D_projection(&view_setup->box,&view_setup->ang,view_setup->fov,walk_view_near_z,walk_view_far_z,walk_view_near_offset);

	if (draw_position) walk_view_draw_position(&view_setup->box,&view_setup->cpt);
}

