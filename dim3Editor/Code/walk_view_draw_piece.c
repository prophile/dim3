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

#ifdef D3_PCH
	#include "dim3editor.h"
#endif

#include "interface.h"
#include "common_view.h"
#include "walk_view.h"

extern int				cy,main_wind_uv_layer,txt_palette_high;
extern float			walk_view_fov,walk_view_y_angle,walk_view_x_angle;
extern bool				dp_liquid,dp_object,dp_lightsoundparticle,dp_node,dp_area,dp_textured;
extern d3pnt			view_pnt;

extern AGLContext		ctx;

extern map_type			map;
extern setup_type		setup;
extern bitmap_type		spot_bitmap,scenery_bitmap,node_bitmap,node_defined_bitmap,
						light_bitmap,sound_bitmap,particle_bitmap;

extern bool obscure_mesh_view_bit_get(unsigned char *visibility_flag,int idx);

/* =======================================================

      Walk View Sprites Drawing
      
======================================================= */

void walk_view_draw_sprite(d3pnt *pnt,float ang_y,unsigned long gl_id)
{
    int			px[4],pz[4],wid,high;
	
    wid=map_enlarge*3;
    high=map_enlarge*4;
	
	px[0]=px[3]=pnt->x-wid;
	px[1]=px[2]=pnt->x+wid;
	pz[0]=pz[1]=pnt->z-wid;
	pz[2]=pz[3]=pnt->z+wid;
	
	rotate_2D_polygon(4,px,pz,pnt->x,pnt->z,ang_y);
    
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,gl_id);
    
	glColor4f(1,1,1,1);
	
	glBegin(GL_QUADS);

    glTexCoord2f(0,0);
	glVertex3i(px[0],(pnt->y-high),pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],(pnt->y-high),pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[1],pnt->y,pz[1]);
    glTexCoord2f(0,1);
	glVertex3i(px[0],pnt->y,pz[0]);

    glTexCoord2f(0,0);
	glVertex3i(px[3],(pnt->y-high),pz[3]);
    glTexCoord2f(1,0);
	glVertex3i(px[2],(pnt->y-high),pz[2]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],pnt->y,pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],pnt->y,pz[3]);

    glTexCoord2f(0,0);
	glVertex3i(px[0],(pnt->y-high),pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[3],(pnt->y-high),pz[3]);
    glTexCoord2f(1,1);
	glVertex3i(px[3],pnt->y,pz[3]);
    glTexCoord2f(0,1);
	glVertex3i(px[0],pnt->y,pz[0]);

    glTexCoord2f(0,0);
	glVertex3i(px[1],(pnt->y-high),pz[1]);
    glTexCoord2f(1,0);
	glVertex3i(px[2],(pnt->y-high),pz[2]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],pnt->y,pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[1],pnt->y,pz[1]);

    glTexCoord2f(0,0);
	glVertex3i(px[0],(pnt->y-high),pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],(pnt->y-high),pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],(pnt->y-high),pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],(pnt->y-high),pz[3]);

    glTexCoord2f(0,0);
	glVertex3i(px[0],pnt->y,pz[0]);
    glTexCoord2f(1,0);
	glVertex3i(px[1],pnt->y,pz[1]);
    glTexCoord2f(1,1);
	glVertex3i(px[2],pnt->y,pz[2]);
    glTexCoord2f(0,1);
	glVertex3i(px[3],pnt->y,pz[3]);
	
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
}

void walk_view_draw_circle(d3pnt *pnt,d3col *col,int dist)
{
    int				n,kx,kz;
	
	glLineWidth(4.0f);
	glColor4f(col->r,col->g,col->b,0.75f);
	
		// xz line
		
	glBegin(GL_LINE_LOOP);
	
	for (n=0;n!=360;n+=10) {
		kx=dist;
		kz=0;
		rotate_2D_point_center(&kx,&kz,(float)n);
		glVertex3i((pnt->x+kx),pnt->y,(pnt->z+kz));
	}

	glEnd();
	
	glLineWidth(1.0f);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
}

/* =======================================================

      Walk View Mesh Drawing
      
======================================================= */

void walk_view_draw_meshes_texture(int clip_y,bool opaque)
{
	int					n,k,t;
	unsigned long		old_gl_id;
	float				dark_factor;
	bool				clip_ok;
	d3pnt				*pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;
							
		// no depth buffer for transparent segments
		
	glEnable(GL_TEXTURE_2D);

	if (opaque) {
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_BLEND);
	}
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glEnable(GL_DEPTH_TEST);
	if (opaque) {
		glDepthMask(GL_TRUE);
	}
	else {
		glDepthMask(GL_FALSE);
	}
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	old_gl_id=-1;
	dark_factor=1.0f;

		// draw meshes
		
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
	
			// skip any meshes that don't have
			// the correct UV level
			
		if (main_wind_uv_layer>=mesh->nuv) {
			mesh++;
			continue;
		}
		
			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			mesh_poly=&mesh->polys[k];
			
				// get texture.  If in second UV, we use extra
				// texture for display if it exists
				
			texture=&map.textures[mesh_poly->txt_idx];
			if (main_wind_uv_layer!=0) {
				if (mesh->extra_txt_idx!=-1) {
					texture=&map.textures[mesh->extra_txt_idx];
				}
			}
		
				// opaque or transparent flag
				
			if (opaque) {
				if ((mesh_poly->alpha!=1.0f) || (texture->frames[0].bitmap.alpha_mode==alpha_mode_transparent)) continue;
			}
			else {
				if ((mesh_poly->alpha==1.0f) && (texture->frames[0].bitmap.alpha_mode!=alpha_mode_transparent)) continue;
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
				
			if (texture->frames[0].bitmap.gl_id!=old_gl_id) {
				old_gl_id=texture->frames[0].bitmap.gl_id;
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
				glTexCoord2f(mesh_poly->uv[main_wind_uv_layer].x[t],mesh_poly->uv[main_wind_uv_layer].y[t]);
				glVertex3i(pt->x,pt->y,pt->z);
			}
			
			glEnd();
		}
	
		mesh++;
	}
	
	if (!opaque) glDepthMask(GL_TRUE);
	
	glDisable(GL_TEXTURE_2D);
}

void walk_view_draw_meshes_line(bool opaque)
{
	int					n,k,t;
	d3pnt				*pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*mesh_poly;
	texture_type		*texture;
							
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	
	glColor4f(setup.col.mesh_line.r,setup.col.mesh_line.g,setup.col.mesh_line.b,1.0f);
	
		// draw portal mesh lines

	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		
			// draw polys
	
		for (k=0;k!=mesh->npoly;k++) {
		
			mesh_poly=&mesh->polys[k];
			texture=&map.textures[mesh_poly->txt_idx];
		
			if (opaque) {
				if ((mesh_poly->alpha!=1.0f) || (texture->frames[0].bitmap.alpha_mode==alpha_mode_transparent)) continue;
			}
			else {
				if ((mesh_poly->alpha==1.0f) && (texture->frames[0].bitmap.alpha_mode!=alpha_mode_transparent)) continue;
			}
		
			glBegin(GL_LINE_LOOP);
			
			for (t=0;t!=mesh_poly->ptsz;t++) {
				pt=&mesh->vertexes[mesh_poly->v[t]];
				glVertex3i(pt->x,pt->y,pt->z);
			}
			
			glEnd();
		}
	
		mesh++;
	}
}

/* =======================================================

      Walk View Liquid Drawing
      
======================================================= */

void walk_view_draw_liquids(bool opaque)
{
	int					n,nliquid,x,y,z,y2,lx,rx,tz,bz;
	unsigned long		old_gl_id;
	texture_type		*texture;
	map_liquid_type		*liquid;
	
	if (!dp_liquid) return;
	
		// no depth buffer for transparent segments
		
	glEnable(GL_TEXTURE_2D);

	if (opaque) {
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_BLEND);
	}
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glEnable(GL_DEPTH_TEST);
	if (opaque) {
		glDepthMask(GL_TRUE);
	}
	else {
		glDepthMask(GL_FALSE);
	}
	
	old_gl_id=-1;

		// run through the liquids
		
	nliquid=map.liquid.nliquid;
	
	glEnable(GL_TEXTURE_2D);
	
	for (n=0;n!=nliquid;n++) {
		liquid=&map.liquid.liquids[n];
	
			// textures
			
		glColor4f(1.0f,1.0f,1.0f,1.0f);

		texture=&map.textures[liquid->txt_idx];
	
		if (opaque) {
			if ((liquid->alpha!=1.0f) || (texture->frames[0].bitmap.alpha_mode==alpha_mode_transparent)) continue;
		}
		else {
			if ((liquid->alpha==1.0f) && (texture->frames[0].bitmap.alpha_mode!=alpha_mode_transparent)) continue;
		}
		
		if (texture->frames[0].bitmap.gl_id!=old_gl_id) {
			old_gl_id=texture->frames[0].bitmap.gl_id;
			glBindTexture(GL_TEXTURE_2D,old_gl_id);
		}
		
		glColor4f(1.0f,1.0f,1.0f,liquid->alpha);
		
			// dimensions
			
		y=liquid->y;
		y2=liquid->y+liquid->depth;
		
		lx=liquid->lft;
		rx=liquid->rgt;
		tz=liquid->top;
		bz=liquid->bot;
		
		glBegin(GL_QUADS);
		
			// bottom
			
		glTexCoord2f(liquid->x_txtoff,liquid->y_txtoff);
		glVertex3i(lx,y2,tz);
		glTexCoord2f((liquid->x_txtoff+liquid->x_txtfact),liquid->y_txtoff);
		glVertex3i(rx,y2,tz);
		glTexCoord2f((liquid->x_txtoff+liquid->x_txtfact),(liquid->y_txtoff+liquid->y_txtfact));
		glVertex3i(rx,y2,bz);
		glTexCoord2f(liquid->x_txtoff,(liquid->y_txtoff+liquid->y_txtfact));
		glVertex3i(lx,y2,bz);

			// top
			
		glTexCoord2f(liquid->x_txtoff,liquid->y_txtoff);
		glVertex3i(lx,y,tz);
		glTexCoord2f((liquid->x_txtoff+liquid->x_txtfact),liquid->y_txtoff);
		glVertex3i(rx,y,tz);
		glTexCoord2f((liquid->x_txtoff+liquid->x_txtfact),(liquid->y_txtoff+liquid->y_txtfact));
		glVertex3i(rx,y,bz);
		glTexCoord2f(liquid->x_txtoff,(liquid->y_txtoff+liquid->y_txtfact));
		glVertex3i(lx,y,bz);

		glEnd();
		
			// depth lines
	
		glColor4f(setup.col.mesh_line.r,setup.col.mesh_line.g,setup.col.mesh_line.b,1.0f);
		
		glBegin(GL_LINES);
		
		x=liquid->lft;
		z=liquid->top;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		x=liquid->rgt;
		z=liquid->top;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		x=liquid->lft;
		z=liquid->bot;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		x=liquid->rgt;
		z=liquid->bot;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		glEnd();
	}
	
	if (!opaque) glDepthMask(GL_TRUE);
	
	glDisable(GL_ALPHA_TEST);
	if (opaque) glEnable(GL_BLEND);
	
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Walk View Area Drawing
      
======================================================= */

void walk_view_draw_areas(void)
{
	int					n,k,y,lx,rx,tz,bz;
	char				area_flags[max_area];
	map_area_type		*area,*area2;
	
	if (!dp_area) return;
	
		// get a list of all selected areas
		
	bzero(area_flags,max_area);

	for (n=0;n!=map.narea;n++) {
	
		if (!select_check(area_piece,n,-1)) continue;
		
			// area selected
				
		area_flags[n]=0x1;

			// join with any areas with the same color

		area=&map.areas[n];
		area2=map.areas;

		for (k=0;k!=map.narea;k++) {

			if (k!=n) {
				if ((area2->col.r==area->col.r) && (area2->col.g==area->col.g) && (area2->col.b==area->col.b)) area_flags[k]=0x1;
			}

			area2++;
		}
	}
	
		// transparent area draw
		
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);

		// area colors
		
	area=map.areas;
	
	for (n=0;n!=map.narea;n++) {
	
			// only draw color for selection
			
		if (area_flags[n]!=0x0) {
	
				// color
				
			glColor4f(area->col.r,area->col.g,area->col.b,0.5f);
			
				// dimensions
				
			y=view_pnt.y;
			
			lx=area->lft;
			rx=area->rgt;
			tz=area->top;
			bz=area->bot;
			
			glBegin(GL_QUADS);
			glVertex3i(lx,y,tz);
			glVertex3i(rx,y,tz);
			glVertex3i(rx,y,bz);
			glVertex3i(lx,y,bz);
			glEnd();
		}
		
		area++;
	}
	
		// area boxes
		
	glLineWidth(4.0f);

	area=map.areas;
	
	for (n=0;n!=map.narea;n++) {
	
			// color
			
		glColor4f(area->col.r,area->col.g,area->col.b,1.0f);
		
			// dimensions
			
		y=view_pnt.y;
		
		lx=area->lft;
		rx=area->rgt;
		tz=area->top;
		bz=area->bot;
		
		glBegin(GL_LINE_LOOP);
		glVertex3i(lx,y,tz);
		glVertex3i(rx,y,tz);
		glVertex3i(rx,y,bz);
		glVertex3i(lx,y,bz);
		glEnd();
		
		area++;
	}
	
	glLineWidth(1.0f);
	
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
}

/* =======================================================

      Walk View Nodes, Scenery, etc Drawing
      
======================================================= */

void walk_view_draw_nodes(d3pnt *cpt)
{
	int			n,k;
	float		fx,fy,fz;
	node_type	*node,*lnode;
	matrix_type	mat;

	if (!dp_node) return;
	
		// angles
		
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
		
	glLineWidth(3.0f);
	glColor4f(1.0f,0.7f,0.0f,1.0f);
	
	glBegin(GL_LINES);
		
	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
	
		fx=0.0f;
		fy=0.0f;
		fz=-(float)(map_enlarge*8);
	
		matrix_rotate_xyz(&mat,node->ang.x,node->ang.y,node->ang.z);
		matrix_vertex_multiply(&mat,&fx,&fy,&fz);
			
		glVertex3i(node->pnt.x,(node->pnt.y-(map_enlarge*2)),node->pnt.z);
		glVertex3i((node->pnt.x+(int)fx),((node->pnt.y-(map_enlarge*2))+(int)fy),(node->pnt.z+(int)fz));
		
		node++;
	}
	
	glEnd();
	
		// connections
		
	glColor4f(0.0f,1.0f,0.0f,1.0f);
	
	glBegin(GL_LINES);
		
	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
			
		for (k=0;k!=max_node_link;k++) {
		
			if (node->link[k]!=-1) {
				glVertex3i(node->pnt.x,(node->pnt.y-(map_enlarge*2)),node->pnt.z);
				lnode=&map.nodes[node->link[k]];
				glVertex3i(lnode->pnt.x,(lnode->pnt.y-(map_enlarge*2)),lnode->pnt.z);
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
		if (node->name[0]==0x0) {
			walk_view_draw_sprite(&node->pnt,0.0f,node_bitmap.gl_id);
		}
		else {
			walk_view_draw_sprite(&node->pnt,0.0f,node_defined_bitmap.gl_id);
		}
		node++;
	}
}

void walk_view_draw_spots_scenery(d3pnt *cpt)
{
	int					n;
	spot_type			*spot;
	map_scenery_type	*scenery;
	
    if (!dp_object) return;
    
	spot=map.spots;
	
	for (n=0;n!=map.nspot;n++) {
	
		if (!walk_view_model_draw(&spot->pnt,&spot->ang,spot->display_model,NULL,0)) {
			walk_view_draw_sprite(&spot->pnt,spot->ang.y,spot_bitmap.gl_id);
		}
		
		spot++;
	}		
    
	scenery=map.sceneries;
	
	for (n=0;n!=map.nscenery;n++) {
	
		if (!walk_view_model_draw(&scenery->pnt,&scenery->ang,scenery->model_name,scenery->texture_frame,max_map_scenery_model_texture_frame)) {
			walk_view_draw_sprite(&scenery->pnt,scenery->ang.y,scenery_bitmap.gl_id);
		}
		
		scenery++;
	}		
}

void walk_view_draw_lights_sounds_particles(d3pnt *cpt,bool draw_light_circle)
{
	int				n;
	
	if (!dp_lightsoundparticle) return;
	
	for (n=0;n!=map.nlight;n++) {
		if (draw_light_circle) walk_view_draw_circle(&map.lights[n].pnt,&map.lights[n].col,map.lights[n].intensity);
		walk_view_draw_sprite(&map.lights[n].pnt,0.0f,light_bitmap.gl_id);
	}
	
	for (n=0;n!=map.nsound;n++) {
		walk_view_draw_sprite(&map.sounds[n].pnt,0.0f,sound_bitmap.gl_id);
	}
	
	for (n=0;n!=map.nparticle;n++) {
		walk_view_draw_sprite(&map.particles[n].pnt,0.0f,particle_bitmap.gl_id);
	}
}

/* =======================================================

      Walk View Drawing Setup
      
======================================================= */

void walk_view_gl_setup(editor_3D_view_setup *view_setup)
{
	main_wind_set_viewport(&view_setup->box,TRUE,TRUE);
	main_wind_set_3D_projection(view_setup,walk_view_near_z,walk_view_far_z,walk_view_near_offset);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
}

/* =======================================================

      Draw Current Position
      
======================================================= */

void walk_view_draw_position(editor_3D_view_setup *view_setup)
{
    int			x,z,sz;
   
 	main_wind_set_2D_projection(view_setup);

	glDisable(GL_DEPTH_TEST);

	x=view_setup->box.lx+((view_setup->box.rx-view_setup->box.lx)>>1);
	z=view_setup->box.ty+((view_setup->box.by-view_setup->box.ty)>>1);
	
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
	int			clip_y;
	
       // 3D view
        
	main_wind_set_viewport(&view_setup->box,TRUE,TRUE);
	main_wind_set_3D_projection(view_setup,walk_view_near_z,walk_view_far_z,walk_view_near_offset);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
		// y cipping
		
	clip_y=-1;
	if (view_setup->clip_on) clip_y=view_setup->clip_y;

        // draw opaque parts of portals in sight path
        
	if (!view_setup->mesh_only) walk_view_draw_meshes_texture(clip_y,TRUE);
	walk_view_draw_nodes(&view_setup->cpt);
	walk_view_draw_spots_scenery(&view_setup->cpt);
	walk_view_draw_lights_sounds_particles(&view_setup->cpt,view_setup->draw_light_circle);
	walk_view_draw_liquids(TRUE);
	
		// draw opaque mesh lines
		// push view forward to better z-buffer lines
		
	main_wind_set_3D_projection(view_setup,(walk_view_near_z+10),(walk_view_far_z-10),walk_view_near_offset);
	walk_view_draw_meshes_line(TRUE);

        // draw transparent parts of portals in sight path
        
	main_wind_set_3D_projection(view_setup,walk_view_near_z,walk_view_far_z,walk_view_near_offset);

	if (!view_setup->mesh_only) walk_view_draw_meshes_texture(clip_y,FALSE);
	walk_view_draw_liquids(FALSE);
 	
        // draw transparent mesh lines
		// push view forward to better z-buffer lines
        
	main_wind_set_3D_projection(view_setup,(walk_view_near_z+10),(walk_view_far_z-10),walk_view_near_offset);
	walk_view_draw_meshes_line(TRUE);
	
		// draw areas
		
	if (view_setup->draw_area) walk_view_draw_areas();
		
		// draw selection
		
	walk_view_draw_select(&view_setup->cpt,view_setup->draw_area);

		// position
		
	main_wind_set_3D_projection(view_setup,walk_view_near_z,walk_view_far_z,walk_view_near_offset);

	if (draw_position) walk_view_draw_position(view_setup);
}

