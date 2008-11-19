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

extern int				cy,txt_palette_high,obscure_mesh_idx;
extern float			walk_view_fov,walk_view_y_angle,walk_view_x_angle;
extern bool				dp_liquid,dp_object,dp_lightsoundparticle,dp_node,dp_textured;

extern WindowRef		mainwind;
extern AGLContext		ctx;

extern map_type			map;
extern setup_type		setup;
extern bitmap_type		spot_bitmap,scenery_bitmap,node_bitmap,
						light_bitmap,sound_bitmap,particle_bitmap;

extern bool obscure_mesh_view_bit_get(unsigned char *visibility_flag,int idx);

/* =======================================================

      Walk View Sprites Drawing
      
======================================================= */

void walk_view_draw_sprite(d3pnt *cpt,d3pnt *pnt,float ang_y,unsigned long gl_id)
{
    int			x,y,z,px[4],pz[4],wid,high;
	
    x=pnt->x-cpt->x;
    y=pnt->y-cpt->y;
    z=pnt->z-cpt->z;
    
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

void walk_view_draw_circle(d3pnt *cpt,d3pnt *pnt,d3col *col,int dist)
{
    int				n,x,y,z,kx,kz;
	
    x=pnt->x-cpt->x;
    y=pnt->y-cpt->y;
    z=pnt->z-cpt->z;
	
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

      Walk View Mesh Drawing
      
======================================================= */

void walk_view_draw_meshes_texture(d3pnt *cpt,int clip_y,bool opaque)
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
		
		// supergumba -- need view clipping here
		
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
			
			// obscure testing
				
		if (obscure_mesh_idx!=-1) {
			if (!obscure_mesh_view_bit_get(map.mesh.meshes[obscure_mesh_idx].obscure.visibility_flag,n)) {
				mesh++;
				continue;
			}
		}
		
			// draw polys
	
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
				glTexCoord2f(mesh_poly->gx[t],mesh_poly->gy[t]);
				glVertex3i((pt->x-cpt->x),(pt->y-cpt->y),(cpt->z-pt->z));
			}
			
			glEnd();
		}
	
		mesh++;
	}
	
	if (!opaque) glDepthMask(GL_TRUE);
}

void walk_view_draw_meshes_line(d3pnt *cpt,bool opaque)
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
	
			// obscure testing
				
		if (obscure_mesh_idx!=-1) {
			if (!obscure_mesh_view_bit_get(map.mesh.meshes[obscure_mesh_idx].obscure.visibility_flag,n)) {
				mesh++;
				continue;
			}
		}
		
			// draw polys
	
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
				glVertex3i((pt->x-cpt->x),(pt->y-cpt->y),(cpt->z-pt->z));
			}
			
			glEnd();
		}
	
		mesh++;
	}
}

/* =======================================================

      Walk View Liquid Drawing
      
======================================================= */

void walk_view_draw_liquids(d3pnt *cpt,bool opaque)
{
	int					n,nliquid,x,y,z,y2;
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
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	old_gl_id=-1;

		// run through the liquids
		
	nliquid=map.liquid.nliquid;
	liquid=map.liquid.liquids;
	
	glEnable(GL_TEXTURE_2D);
	
	for (n=0;n!=nliquid;n++) {
	
			// textures
			
		glColor4f(1.0f,1.0f,1.0f,1.0f);

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

		x=liquid->lft-cpt->x;
		z=cpt->z-liquid->top;
		glTexCoord2f(liquid->x_txtoff,liquid->y_txtoff);
		glVertex3i(x,y,z);

		x=liquid->rgt-cpt->x;
		z=cpt->z-liquid->top;
		glTexCoord2f((liquid->x_txtoff+liquid->x_txtfact),liquid->y_txtoff);
		glVertex3i(x,y,z);
		
		x=liquid->rgt-cpt->x;
		z=cpt->z-liquid->bot;
		glTexCoord2f((liquid->x_txtoff+liquid->x_txtfact),(liquid->y_txtoff+liquid->y_txtfact));
		glVertex3i(x,y,z);
		
		x=liquid->lft-cpt->x;
		z=cpt->z-liquid->bot;
		glTexCoord2f(liquid->x_txtoff,(liquid->y_txtoff+liquid->y_txtfact));
		glVertex3i(x,y,z);

		glEnd();
		
			// depth lines
			
		glColor4f(setup.col.mesh_line.r,setup.col.mesh_line.g,setup.col.mesh_line.b,1.0f);
		
		y2=(liquid->y+liquid->depth)-cpt->y;
		
		glBegin(GL_LINES);
		
		x=liquid->lft-cpt->x;
		z=cpt->z-liquid->top;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		x=liquid->rgt-cpt->x;
		z=cpt->z-liquid->top;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		x=liquid->lft-cpt->x;
		z=cpt->z-liquid->bot;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		x=liquid->rgt-cpt->x;
		z=cpt->z-liquid->bot;
		glVertex3i(x,y,z);
		glVertex3i(x,y2,z);

		glEnd();
		
		glBegin(GL_LINE_LOOP);
		
		x=liquid->lft-cpt->x;
		z=cpt->z-liquid->top;
		glVertex3i(x,y2,z);

		x=liquid->rgt-cpt->x;
		z=cpt->z-liquid->top;
		glVertex3i(x,y2,z);

		x=liquid->rgt-cpt->x;
		z=cpt->z-liquid->bot;
		glVertex3i(x,y2,z);

		x=liquid->lft-cpt->x;
		z=cpt->z-liquid->bot;
		glVertex3i(x,y2,z);
		
		glEnd();
		
		liquid++;
	}
	
	if (!opaque) glDepthMask(GL_TRUE);
	
	glDisable(GL_ALPHA_TEST);
	if (opaque) glEnable(GL_BLEND);
	
	glDisable(GL_TEXTURE_2D);
}

/* =======================================================

      Walk View Nodes, Scenery, etc Drawing
      
======================================================= */

bool walk_view_draw_pnt_obscure(d3pnt *pnt)
{
	int			mesh_idx;
	
	if (obscure_mesh_idx==-1) return(FALSE);
	
	mesh_idx=map_mesh_find(&map,pnt);
	if (mesh_idx==-1) return(FALSE);
	
	return(obscure_mesh_view_bit_get(map.mesh.meshes[obscure_mesh_idx].obscure.visibility_flag,mesh_idx)==0x0);
}

void walk_view_draw_nodes(d3pnt *cpt)
{
	int			n,k;
	node_type	*node,*lnode;

	if (!dp_node) return;
	
		// connections

	glLineWidth(3.0f);
	glColor4f(0.0f,1.0f,0.0f,1.0f);
	
	glBegin(GL_LINES);
		
	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
	
		if (walk_view_draw_pnt_obscure(&node->pnt)) {
			node++;
			continue;
		}
			
		for (k=0;k!=max_node_link;k++) {
		
			if (node->link[k]!=-1) {
				glVertex3i((node->pnt.x-cpt->x),((node->pnt.y-(map_enlarge*2))-cpt->y),(cpt->z-node->pnt.z));
				lnode=&map.nodes[node->link[k]];
				glVertex3i((lnode->pnt.x-cpt->x),((lnode->pnt.y-(map_enlarge*2))-cpt->y),(cpt->z-lnode->pnt.z));
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
		if (!walk_view_draw_pnt_obscure(&node->pnt)) walk_view_draw_sprite(cpt,&node->pnt,0.0f,node_bitmap.gl_id);
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
	
		if (!walk_view_draw_pnt_obscure(&spot->pnt)) {
			if (!walk_view_model_draw(cpt,&spot->pnt,&spot->ang,spot->display_model,NULL,0)) {
				walk_view_draw_sprite(cpt,&spot->pnt,spot->ang.y,spot_bitmap.gl_id);
			}
		}
		
		spot++;
	}		
    
	scenery=map.sceneries;
	
	for (n=0;n!=map.nscenery;n++) {
	
		if (!walk_view_draw_pnt_obscure(&scenery->pnt)) {
			if (!walk_view_model_draw(cpt,&scenery->pnt,&scenery->ang,scenery->model_name,scenery->texture_frame,max_map_scenery_model_texture_frame)) {
				walk_view_draw_sprite(cpt,&scenery->pnt,scenery->ang.y,scenery_bitmap.gl_id);
			}
		}
		
		scenery++;
	}		
}

void walk_view_draw_lights_sounds_particles(d3pnt *cpt,bool draw_light_circle)
{
	int				n;
	
	if (!dp_lightsoundparticle) return;
	
	for (n=0;n!=map.nlight;n++) {
		if (!walk_view_draw_pnt_obscure(&map.lights[n].pnt)) {
			if (draw_light_circle) walk_view_draw_circle(cpt,&map.lights[n].pnt,&map.lights[n].col,map.lights[n].intensity);
			walk_view_draw_sprite(cpt,&map.lights[n].pnt,0.0f,light_bitmap.gl_id);
		}
	}
	
	for (n=0;n!=map.nsound;n++) {
		if (!walk_view_draw_pnt_obscure(&map.sounds[n].pnt)) walk_view_draw_sprite(cpt,&map.sounds[n].pnt,0.0f,sound_bitmap.gl_id);
	}
	
	for (n=0;n!=map.nparticle;n++) {
		if (!walk_view_draw_pnt_obscure(&map.particles[n].pnt)) walk_view_draw_sprite(cpt,&map.particles[n].pnt,0.0f,particle_bitmap.gl_id);
	}
}

/* =======================================================

      Walk View Drawing Setup
      
======================================================= */

void walk_view_gl_setup(editor_3D_view_setup *view_setup)
{
	main_wind_set_viewport(&view_setup->box,TRUE,TRUE);
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
	int			clip_y;
	
       // 3D view
        
	main_wind_set_viewport(&view_setup->box,TRUE,TRUE);
	main_wind_set_3D_projection(&view_setup->box,&view_setup->ang,view_setup->fov,walk_view_near_z,walk_view_far_z,walk_view_near_offset);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
		// y cipping
		
	clip_y=-1;
	if (view_setup->clip_on) clip_y=view_setup->clip_y;

        // draw opaque parts of portals in sight path
        
	if (!view_setup->mesh_only) walk_view_draw_meshes_texture(&view_setup->cpt,clip_y,TRUE);
	walk_view_draw_nodes(&view_setup->cpt);
	walk_view_draw_spots_scenery(&view_setup->cpt);
	walk_view_draw_lights_sounds_particles(&view_setup->cpt,view_setup->draw_light_circle);
	walk_view_draw_liquids(&view_setup->cpt,TRUE);
	
		// draw opaque mesh lines
		// push view forward to better z-buffer lines
		
	main_wind_set_3D_projection(&view_setup->box,&view_setup->ang,view_setup->fov,(walk_view_near_z+10),(walk_view_far_z-10),walk_view_near_offset);
	walk_view_draw_meshes_line(&view_setup->cpt,TRUE);
	
        // draw transparent parts of portals in sight path
        
	main_wind_set_3D_projection(&view_setup->box,&view_setup->ang,view_setup->fov,walk_view_near_z,walk_view_far_z,walk_view_near_offset);

	if (!view_setup->mesh_only) walk_view_draw_meshes_texture(&view_setup->cpt,clip_y,FALSE);
	walk_view_draw_liquids(&view_setup->cpt,FALSE);
 	
        // draw transparent mesh lines
		// push view forward to better z-buffer lines
        
	main_wind_set_3D_projection(&view_setup->box,&view_setup->ang,view_setup->fov,(walk_view_near_z+10),(walk_view_far_z-10),walk_view_near_offset);
	walk_view_draw_meshes_line(&view_setup->cpt,TRUE);
		
		// draw selection
		
	walk_view_draw_select(&view_setup->cpt);

		// position
		
	main_wind_set_3D_projection(&view_setup->box,&view_setup->ang,view_setup->fov,walk_view_near_z,walk_view_far_z,walk_view_near_offset);

	if (draw_position) walk_view_draw_position(&view_setup->box,&view_setup->cpt);
}

