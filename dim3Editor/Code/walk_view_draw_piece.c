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

void walk_view_draw_sprite(d3pos *pos,unsigned long gl_id)
{
    int			x,y,z,wid,high;
	portal_type	*portal;
	
	portal=&map.portals[pos->rn];
	
    x=(pos->x+portal->x)-cx;
    y=(pos->y+1)-cy;
    z=(pos->z+portal->z)-cz;
    
    wid=map_enlarge*3;
    high=map_enlarge*4;
    
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,gl_id);
    
	glColor4f(1,1,1,1);
	
	glBegin(GL_QUADS);
	
    glTexCoord2f(0,0);
	glVertex3i((x-wid),(y-high),-(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x+wid),(y-high),-(z-wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),y,-(z-wid));
    glTexCoord2f(0,1);
	glVertex3i((x-wid),y,-(z-wid));

    glTexCoord2f(0,0);
	glVertex3i((x-wid),(y-high),-(z+wid));
    glTexCoord2f(1,0);
	glVertex3i((x+wid),(y-high),-(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),y,-(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x-wid),y,-(z+wid));

    glTexCoord2f(0,0);
	glVertex3i((x-wid),(y-high),-(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x-wid),(y-high),-(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x-wid),y,-(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x-wid),y,-(z-wid));

    glTexCoord2f(0,0);
	glVertex3i((x+wid),(y-high),-(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x+wid),(y-high),-(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),y,-(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x+wid),y,-(z-wid));

    glTexCoord2f(0,0);
	glVertex3i((x-wid),(y-high),-(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x-wid),(y-high),-(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),(y-high),-(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x+wid),(y-high),-(z-wid));

    glTexCoord2f(0,0);
	glVertex3i((x-wid),y,-(z-wid));
    glTexCoord2f(1,0);
	glVertex3i((x-wid),y,-(z+wid));
    glTexCoord2f(1,1);
	glVertex3i((x+wid),y,-(z+wid));
    glTexCoord2f(0,1);
	glVertex3i((x+wid),y,-(z-wid));
	
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Walk View Portal Drawing
      
======================================================= */

void walk_view_draw_portal_meshes(int rn,bool opaque)
{
	int					n,k,t,x,y,z;
	unsigned long		old_gl_id;
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

		// draw portal meshes
		
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
		
			if (texture->bitmaps[0].gl_id!=old_gl_id) {
				old_gl_id=texture->bitmaps[0].gl_id;
				glBindTexture(GL_TEXTURE_2D,old_gl_id);
			}
		
			glBegin(GL_POLYGON);
			
			for (t=0;t!=mesh_poly->ptsz;t++) {
				pt=&mesh->vertexes[mesh_poly->v[t]];
				x=(pt->x+portal->x)-cx;
				y=pt->y-cy;
				z=cz-(pt->z+portal->z);
				glTexCoord2f(mesh_poly->gx[t],mesh_poly->gy[t]);
				glVertex3i(x,y,z);
			}
			
			glEnd();
		}
	
	
		mesh++;
	}
	
	if (!opaque) glDepthMask(GL_TRUE);
	
	glDisable(GL_ALPHA_TEST);
	if (opaque) glEnable(GL_BLEND);
	
	glDisable(GL_TEXTURE_2D);
	
		// draw portal mesh lines
		
	glColor4f(0.5f,0.5f,1.0f,1.0f);
	glLineWidth(2.0f);
	
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
				x=(pt->x+portal->x)-cx;
				y=pt->y-cy;
				z=cz-(pt->z+portal->z);
				glVertex3i(x,y,z);
			}
			
			glEnd();
		}
	
	
		mesh++;
	}
	
	glLineWidth(1.0f);
}

void walk_view_draw_portal_liquids(int rn,bool opaque)
{
	int					n,x,y,z;
	unsigned long		old_gl_id;
	portal_type			*portal;
	texture_type		*texture;
	portal_liquid_type	*portal_liquid;
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
	portal_liquid=&portal->liquid;
	liquid=portal_liquid->liquids;
	
	glEnable(GL_TEXTURE_2D);
	
	for (n=0;n!=portal_liquid->nliquid;n++) {
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
		
		y=liquid->y-cy;

		glBegin(GL_QUADS);

		x=(liquid->lft+portal->x)-cx;
		z=cz-(liquid->top+portal->z);
		glTexCoord2f(liquid->x_txtoff,liquid->y_txtoff);
		glVertex3i(x,y,z);

		x=(liquid->rgt+portal->x)-cx;
		z=cz-(liquid->top+portal->z);
		glTexCoord2f((liquid->x_txtoff+liquid->x_txtfact),liquid->y_txtoff);
		glVertex3i(x,y,z);
		
		x=(liquid->rgt+portal->x)-cx;
		z=cz-(liquid->bot+portal->z);
		glTexCoord2f((liquid->x_txtoff+liquid->x_txtfact),(liquid->y_txtoff+liquid->y_txtfact));
		glVertex3i(x,y,z);
		
		x=(liquid->lft+portal->x)-cx;
		z=cz-(liquid->bot+portal->z);
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

void walk_view_draw_portal_nodes(int rn)
{
	int			n;

	if (!dp_node) return;
	
	for (n=0;n!=map.nnode;n++) {
		if (map.nodes[n].pos.rn==rn) {
			walk_view_draw_sprite(&map.nodes[n].pos,node_bitmap.gl_id);
		}
	}
}

void walk_view_draw_portal_spots_scenery(int rn)
{
	int			n;
	
    if (!dp_object) return;
    
	for (n=0;n!=map.nspot;n++) {
		if (map.spots[n].pos.rn==rn) {
			if (!walk_view_model_draw(&map.spots[n].pos,&map.spots[n].ang,map.spots[n].display_model)) {
				walk_view_draw_sprite(&map.spots[n].pos,spot_bitmap.gl_id);
			}
		}
	}		
    
	for (n=0;n!=map.nscenery;n++) {
		if (map.sceneries[n].pos.rn==rn) {
			if (!walk_view_model_draw(&map.sceneries[n].pos,&map.sceneries[n].ang,map.sceneries[n].model_name)) {
				walk_view_draw_sprite(&map.sceneries[n].pos,scenery_bitmap.gl_id);
			}
		}
	}		
}

void walk_view_draw_portal_lights_sounds_particles(int rn)
{
	int				n;
	
	if (!dp_lightsoundparticle) return;
	
	for (n=0;n!=map.nlight;n++) {
		if (map.lights[n].pos.rn==rn) {
			walk_view_draw_sprite(&map.lights[n].pos,light_bitmap.gl_id);
		}
	}
	
	for (n=0;n!=map.nsound;n++) {
		if (map.sounds[n].pos.rn==rn) {
			walk_view_draw_sprite(&map.sounds[n].pos,sound_bitmap.gl_id);
		}
	}
	
	for (n=0;n!=map.nparticle;n++) {
		if (map.particles[n].pos.rn==rn) {
			walk_view_draw_sprite(&map.particles[n].pos,particle_bitmap.gl_id);
		}
	}
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
		main_wind_set_viewport(&walk_view_forward_box,0.75f);
		main_wind_set_3D_projection(&walk_view_forward_box,walk_view_x_angle,ang_y,walk_view_fov,walk_view_near_z,walk_view_far_z,walk_view_near_offset);
	}
	else {
		main_wind_set_viewport(&walk_view_side_box,0.75f);
		main_wind_set_3D_projection(&walk_view_side_box,walk_view_x_angle,ang_y,walk_view_fov,walk_view_near_z,walk_view_far_z,walk_view_near_offset);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

void walk_view_draw(bool on_side)
{
	int			i,rn,
				portal_cnt,portal_list[max_portal];
	
 		// always draw from portal that cursor is in
		
	rn=walk_view_find_start_portal();
	walk_view_sight_path_mark(rn);
		
       // 3D view
        
	walk_view_gl_setup(on_side);
	
		// get the portals to draw
		
	for (i=0;i!=map.nportal;i++) {			// routine depends on mx/mz being set
		map.portals[i].mx=(map.portals[i].x+map.portals[i].ex)/2;
		map.portals[i].mz=(map.portals[i].z+map.portals[i].ez)/2;
		map.portals[i].by=0;
	}
	
	portal_cnt=map_portal_draw_sort(&map,rn,cx,cy,cz,portal_list);

        // draw opaque parts of portals in sight path
        
    for (i=0;i!=portal_cnt;i++) {
		rn=portal_list[i];
        walk_view_draw_portal_meshes(rn,TRUE);
		walk_view_draw_portal_nodes(rn);
		walk_view_draw_portal_spots_scenery(rn);
		walk_view_draw_portal_lights_sounds_particles(rn);
		walk_view_draw_portal_liquids(rn,TRUE);
    }
	
        // draw transparent parts of portals in sight path
        
    for (i=0;i!=portal_cnt;i++) {
		rn=portal_list[i];
        walk_view_draw_portal_meshes(rn,FALSE);
		walk_view_draw_portal_liquids(rn,FALSE);
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

