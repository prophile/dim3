/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Top View Piece Drawing Routines

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

#include "common_view.h"
#include "interface.h"
#include "top_view.h"

extern int					cr,cx,cy,cz,top_view_x,top_view_z,magnify_factor;
extern bool					dp_wall,dp_floor,dp_ceiling,dp_liquid,dp_ambient,dp_object,dp_node,dp_lightsoundparticle,dp_y_hide;
extern Rect					top_view_box;

extern bitmap_type			light_bitmap,sound_bitmap,particle_bitmap;

extern map_type				map;

unsigned short				effect,effectmask;
int							txtfill,txtrbfill;

extern void primitive_get_extend(int primitive_uid,int *p_minx,int *p_maxx,int *p_minz,int *p_maxz,int *p_miny,int *p_maxy);
extern void primitive_get_center(int primitive_uid,int *x,int *z,int *y);

/* =======================================================

      Handle Draw Size
      
======================================================= */

int top_view_piece_handle_size(void)
{
	int			pixel_sz;
	
	pixel_sz=(200*magnify_factor)/magnify_size;
	if (pixel_sz<3) pixel_sz=3;
	
	return(pixel_sz);
}

/* =======================================================

      Draw Pieces
      
======================================================= */

void top_view_piece_draw_icon(d3pos *pos,unsigned long gl_id)
{
	int				x,z,k;
	portal_type		*portal;
	
		// box position
			
	portal=&map.portals[pos->rn];
	x=pos->x+portal->x;
	z=pos->z+portal->z;
	
	top_view_map_to_pane(&x,&z);
	
	k=(600*magnify_factor)/magnify_size;
	
	glEnable(GL_TEXTURE_2D);
	 
    glBindTexture(GL_TEXTURE_2D,gl_id);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	glBegin(GL_QUADS);
    glTexCoord2f(0,0);
	glVertex2i((x-k),(z-k));
    glTexCoord2f(1,0);
	glVertex2i((x+k),(z-k));
    glTexCoord2f(1,1);
	glVertex2i((x+k),(z+k));
    glTexCoord2f(0,1);
	glVertex2i((x-k),(z+k));
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
}

void top_view_piece_draw_box(d3pos *pos,float r,float g,float b)
{
	int				x,z,k;
	portal_type		*portal;
	
		// box position
			
	portal=&map.portals[pos->rn];
	x=pos->x+portal->x;
	z=pos->z+portal->z;
	
	top_view_map_to_pane(&x,&z);
	
	k=(400*magnify_factor)/magnify_size;
	
		// box fill
		
	glColor4f(r,g,b,1.0f);
	
	glBegin(GL_QUADS);
	glVertex2i((x-k),(z-k));
	glVertex2i((x+k),(z-k));
	glVertex2i((x+k),(z+k));
	glVertex2i((x-k),(z+k));
	glEnd();
	
		// box outline
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	
	glBegin(GL_LINE_LOOP);
	glVertex2i((x-k),(z-k));
	glVertex2i((x+k),(z-k));
	glVertex2i((x+k),(z+k));
	glVertex2i((x-k),(z+k));
	glEnd();
}

void top_view_piece_draw_circle(d3pos *pos,int radius,float r,float g,float b)
{
	int				x,z,n;
	double			rad,d_radius;
	portal_type		*portal;
	
		// box position
			
	portal=&map.portals[pos->rn];
	x=pos->x+portal->x;
	z=pos->z+portal->z;
	
	top_view_map_to_pane(&x,&z);
	
	d_radius=(double)((radius*magnify_factor)/magnify_size);
	
		// circle
		
	glLineWidth(2.0f);
	glColor4f(r,g,b,1.0f);
	
	glBegin(GL_LINE_LOOP);
	
	for (n=0;n!=360;n+=10) {
		rad=((float)n)*ANG_to_RAD;
		glVertex2i((x+(int)(d_radius*sin(rad))),(z-(int)(d_radius*cos(rad))));
	}

	glEnd();
	
	glLineWidth(1.0f);     
}

void top_view_piece_draw_arrow(d3pos *pos,char *name,int ang_y,float r,float g,float b)
{
	int				x,z,k,px[3],pz[3];
	portal_type		*portal;
	
		// arrow position
			
	portal=&map.portals[pos->rn];
	x=pos->x+portal->x;
	z=pos->z+portal->z;
	
	top_view_map_to_pane(&x,&z);
	
	k=(600*magnify_factor)/magnify_size;
	
	px[0]=x-k;
	pz[0]=z+k;
	
	px[1]=x;
	pz[1]=z-k;
	
	px[2]=x+k;
	pz[2]=z+k;
	
	rotate_2D_polygon(3,px,pz,x,z,ang_y);
	
		// arrow fill
		
	glColor4f(r,g,b,1.0f);
	
	glBegin(GL_TRIANGLES);
	glVertex2i(px[0],pz[0]);
	glVertex2i(px[1],pz[1]);
	glVertex2i(px[2],pz[2]);
	glEnd();
	
		// arrow outline
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	
	glBegin(GL_LINE_LOOP);
	glVertex2i(px[0],pz[0]);
	glVertex2i(px[1],pz[1]);
	glVertex2i(px[2],pz[2]);
	glEnd();
	
	
		// start name
	// supergumba -- fix this	
}

/* =======================================================

      Draw Portal Pieces
      
======================================================= */

void top_view_piece_draw_meshes(int rn)
{
	int							n,k,t,x,z,poly_cnt,
								sort_idx,sort_cnt;
	unsigned long				gl_id;
	float						fy;
	d3pnt						*pt;
	portal_type					*portal;
	map_mesh_type				*mesh;
	map_mesh_poly_type			*mesh_poly;
	map_mesh_poly_sort_type		*poly_sort;
	
	portal=&map.portals[rn];
	
		// count polys
		
	poly_cnt=0;
	
	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
		poly_cnt+=mesh->npoly;
		mesh++;
	}
	
	if (poly_cnt==0) return;
	
	poly_sort=(map_mesh_poly_sort_type*)valloc(sizeof(map_mesh_poly_sort_type)*(poly_cnt+1));
	if (poly_sort==NULL) return;
	
		// sort pieces
		
	sort_cnt=0;
		
	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			fy=(float)mesh->vertexes[mesh_poly->v[0]].y;		// rough sort on first vertex

				// find position in sort list

			sort_idx=sort_cnt;

			for (t=0;t!=sort_cnt;t++) {
				if (fy<poly_sort[t].dist) {
					sort_idx=t;
					break;
				}
			}

				// add to sort list

			if (sort_idx<sort_cnt) memmove(&poly_sort[sort_idx+1],&poly_sort[sort_idx],((sort_cnt-sort_idx)*sizeof(map_mesh_poly_sort_type)));

			poly_sort[sort_idx].mesh_idx=n;
			poly_sort[sort_idx].poly_idx=k;
			poly_sort[sort_idx].dist=fy;

			sort_cnt++;

			mesh_poly++;
		}

		mesh++;
	}

		// draw sorted meshes
		
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glEnable(GL_TEXTURE_2D);
	
	gl_id=-1;
	
	for (n=(sort_cnt-1);n>=0;n--) {
	
		mesh=&portal->mesh.meshes[poly_sort[n].mesh_idx];
		mesh_poly=&mesh->polys[poly_sort[n].poly_idx];
		
		if (gl_id!=map.textures[mesh_poly->txt_idx].bitmaps[0].gl_id) {
			gl_id=map.textures[mesh_poly->txt_idx].bitmaps[0].gl_id;
			glBindTexture(GL_TEXTURE_2D,gl_id);
		}
	
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
	}

	glDisable(GL_TEXTURE_2D);
	
		// free sorting memory
		
	free(poly_sort);
	
		// draw mesh outlines
		
	glColor4f(0.5f,0.5f,1.0f,1.0f);
	
	mesh=portal->mesh.meshes;
	
	for (n=0;n!=portal->mesh.nmesh;n++) {
	
		mesh_poly=mesh->polys;
		
		for (k=0;k!=mesh->npoly;k++) {
		
			glBegin(GL_LINE_LOOP);
			
			for (t=0;t!=mesh_poly->ptsz;t++) {
				pt=&mesh->vertexes[mesh_poly->v[t]];
				x=pt->x+portal->x;
				z=pt->z+portal->z;
				top_view_map_to_pane(&x,&z);
				glVertex2i(x,z);
			}
			
			glEnd();
		
			mesh_poly++;
		}

		mesh++;
	}
}

void top_view_piece_draw_nodes(int rn)
{
	int				n,k,node_idx,x1,z1,x2,z2;
	portal_type		*portal;
	node_type		*node,*node2;

	if (!dp_node) return;
	
		// connections

	glColor4f(1.0f,0.0f,0.0f,1.0f);

	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
		portal=&map.portals[node->pos.rn];
		x1=node->pos.x+portal->x;
		z1=node->pos.z+portal->z;
		
		top_view_map_to_pane(&x1,&z1);
	
		for (k=0;k!=max_node_link;k++) {
			node_idx=node->link[k];
			if (node_idx==-1) continue;
			
			node2=&map.nodes[node_idx];
			if (node2->pos.rn!=rn) continue;
			
			portal=&map.portals[node2->pos.rn];
			x2=node2->pos.x+portal->x;
			z2=node2->pos.z+portal->z;
			
			top_view_map_to_pane(&x2,&z2);
			
			glBegin(GL_LINES);
			glVertex2i(x1,z1);
			glVertex2i(x2,z2);
			glEnd();
		}
		
		node++;
	}
	
		// nodes
		
	node=map.nodes;
	
	for (n=0;n!=map.nnode;n++) {
		if (node->pos.rn==rn) top_view_piece_draw_box(&node->pos,0.0f,1.0f,0.0f);
		node++;
	}
}



void top_view_piece_draw_particle_light_sound(int rn)
{
	int				n;
	
	if (!dp_lightsoundparticle) return;
	
	for (n=0;n!=map.nparticle;n++) {
		if (map.particles[n].pos.rn==rn) top_view_piece_draw_icon(&map.particles[n].pos,particle_bitmap.gl_id);
	}

	for (n=0;n!=map.nsound;n++) {
		if (map.sounds[n].pos.rn==rn) top_view_piece_draw_icon(&map.sounds[n].pos,sound_bitmap.gl_id);
	}

	for (n=0;n!=map.nlight;n++) {
		if (map.lights[n].pos.rn==rn) {
			top_view_piece_draw_icon(&map.lights[n].pos,light_bitmap.gl_id);
			top_view_piece_draw_circle(&map.lights[n].pos,map.lights[n].intensity,map.lights[n].col.r,map.lights[n].col.g,map.lights[n].col.b);
		}
	}
}

void top_view_piece_draw_spots_scenery(int rn)
{
	int						n;
	spot_type				*spot;
	map_scenery_type		*scenery;
	
	if (!dp_object) return;
	
		// script spots
		
	spot=map.spots;
	
	for (n=0;n!=map.nspot;n++) {
		if (spot->pos.rn==rn) top_view_piece_draw_arrow(&spot->pos,spot->name,spot->ang.y,1.0f,0.6f,0.0f);
		spot++;
	}

		// scenery
		
	scenery=map.sceneries;
	
	for (n=0;n!=map.nscenery;n++) {
		if (scenery->pos.rn==rn) top_view_piece_draw_arrow(&scenery->pos,scenery->model_name,scenery->ang.y,1.0f,1.0f,0.0f);
		scenery++;
	}
}

void top_view_piece_draw_liquids(int rn)
{
	int					n,x1,z1,x2,z2;
	portal_type			*portal;
	portal_liquid_type	*portal_liquid;
	map_liquid_type		*liquid;
	
	if (!dp_liquid) return;
	
	portal=&map.portals[rn];
	portal_liquid=&portal->liquid;
	liquid=portal_liquid->liquids;
	
	glEnable(GL_TEXTURE_2D);
	
	for (n=0;n!=portal_liquid->nliquid;n++) {
		x1=liquid->lft+portal->x;
		z1=liquid->top+portal->z;
		top_view_map_to_pane(&x1,&z1);
		
		x2=liquid->rgt+portal->x;
		z2=liquid->bot+portal->z;
		top_view_map_to_pane(&x2,&z2);
		
		glColor4f(1.0f,1.0f,1.0f,liquid->alpha);
		glBindTexture(GL_TEXTURE_2D,map.textures[liquid->txt_idx].bitmaps[0].gl_id);
		
		glBegin(GL_QUADS);
		glTexCoord2f(liquid->x_txtoff,liquid->y_txtoff);
		glVertex2i(x1,z1);
		glTexCoord2f((liquid->x_txtoff+liquid->x_txtfact),liquid->y_txtoff);
		glVertex2i(x2,z1);
		glTexCoord2f((liquid->x_txtoff+liquid->x_txtfact),(liquid->y_txtoff+liquid->y_txtfact));
		glVertex2i(x2,z2);
		glTexCoord2f(liquid->x_txtoff,(liquid->y_txtoff+liquid->y_txtfact));
		glVertex2i(x1,z2);
		glEnd();
	
		liquid++;
	}
	
	glDisable(GL_TEXTURE_2D);
}

void top_view_piece_draw(int rn)
{
	top_view_piece_draw_meshes(rn);
	top_view_piece_draw_liquids(rn);
	top_view_piece_draw_nodes(rn);
	top_view_piece_draw_particle_light_sound(rn);
	top_view_piece_draw_spots_scenery(rn);	
}

/* =======================================================

      Draw Selected Pieces Utility
      
======================================================= */

void top_view_piece_selection_draw_mesh(int rn,int mesh_idx,int mesh_poly_idx)
{
	int						n,k,t,x,z,pixel_sz;
	d3pnt					*pt;
	portal_type				*portal;
	map_mesh_type			*mesh;
	map_mesh_poly_type		*mesh_poly;
		
		// the selected mesh
		
	glColor4f(1.0f,1.0f,0.0f,1.0f);
	
	portal=&map.portals[rn];
	mesh=&portal->mesh.meshes[mesh_idx];
	
	mesh_poly=mesh->polys;
		
	for (k=0;k!=mesh->npoly;k++) {
	
		glBegin(GL_LINE_LOOP);
		
		for (t=0;t!=mesh_poly->ptsz;t++) {
			pt=&mesh->vertexes[mesh_poly->v[t]];
			x=pt->x+portal->x;
			z=pt->z+portal->z;
			top_view_map_to_pane(&x,&z);
			glVertex2i(x,z);
		}
		
		glEnd();
			
		mesh_poly++;
	}
	
		// the selected mesh poly
		
	glColor4f(1.0f,0.0f,0.0f,1.0f);
	glLineWidth(2.0f);
		
	mesh_poly=&mesh->polys[mesh_poly_idx];
	
	glBegin(GL_LINE_LOOP);
	
	for (t=0;t!=mesh_poly->ptsz;t++) {
		pt=&mesh->vertexes[mesh_poly->v[t]];
		x=pt->x+portal->x;
		z=pt->z+portal->z;
		top_view_map_to_pane(&x,&z);
		glVertex2i(x,z);
	}
	
	glEnd();
	
	glLineWidth(1.0f);
	
		// the vertexes
		
	pixel_sz=top_view_piece_handle_size();
	
	pt=mesh->vertexes;
	
	glColor4f(0.0f,0.0f,0.0f,1.0f);
	
	glBegin(GL_QUADS);

	for (n=0;n!=mesh->nvertex;n++) {
		x=pt->x+portal->x;
		z=pt->z+portal->z;
		top_view_map_to_pane(&x,&z);

		glVertex2i((x-pixel_sz),(z-pixel_sz));
		glVertex2i((x+pixel_sz),(z-pixel_sz));
		glVertex2i((x+pixel_sz),(z+pixel_sz));
		glVertex2i((x-pixel_sz),(z+pixel_sz));
		
		pt++;
	}

	glEnd();
}

void top_view_piece_selection_draw_liquid(int portal_idx,int liquid_idx)
{
	int				x1,z1,x2,z2,pixel_sz;
	portal_type		*portal;
	map_liquid_type	*liquid;

	portal=&map.portals[portal_idx];
	liquid=&portal->liquid.liquids[liquid_idx];
	
	x1=liquid->lft+portal->x;
	z1=liquid->top+portal->z;
	top_view_map_to_pane(&x1,&z1);
	
	x2=liquid->rgt+portal->x;
	z2=liquid->bot+portal->z;
	top_view_map_to_pane(&x2,&z2);
	
	pixel_sz=top_view_piece_handle_size();
	
	glColor4f(0.0f,0.0f,0.0f,0.7f);
	
	glBegin(GL_QUADS);
	
	glVertex2i((x1-pixel_sz),(z1-pixel_sz));
	glVertex2i((x1+pixel_sz),(z1-pixel_sz));
	glVertex2i((x1+pixel_sz),(z1+pixel_sz));
	glVertex2i((x1-pixel_sz),(z1+pixel_sz));
	
	glVertex2i((x2-pixel_sz),(z1-pixel_sz));
	glVertex2i((x2+pixel_sz),(z1-pixel_sz));
	glVertex2i((x2+pixel_sz),(z1+pixel_sz));
	glVertex2i((x2-pixel_sz),(z1+pixel_sz));

	glVertex2i((x2-pixel_sz),(z2-pixel_sz));
	glVertex2i((x2+pixel_sz),(z2-pixel_sz));
	glVertex2i((x2+pixel_sz),(z2+pixel_sz));
	glVertex2i((x2-pixel_sz),(z2+pixel_sz));

	glVertex2i((x1-pixel_sz),(z2-pixel_sz));
	glVertex2i((x1+pixel_sz),(z2-pixel_sz));
	glVertex2i((x1+pixel_sz),(z2+pixel_sz));
	glVertex2i((x1-pixel_sz),(z2+pixel_sz));
	
	glEnd();
}


// supergumba -- a lot of this can be deleted
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
	int				x,z,k,pixel_sz;
	portal_type		*portal;

	portal=&map.portals[pos->rn];
	x=pos->x+portal->x;
	z=pos->z+portal->z;
	
	top_view_map_to_pane(&x,&z);
	
	k=(sz*magnify_factor)/magnify_size;
	
	pixel_sz=top_view_piece_handle_size();
	
	glColor4f(0.0f,0.0f,0.0f,0.7f);
	
	glBegin(GL_QUADS);
	
	glVertex2i(((x-k)-pixel_sz),((z-k)-pixel_sz));
	glVertex2i(((x-k)+pixel_sz),((z-k)-pixel_sz));
	glVertex2i(((x-k)+pixel_sz),((z-k)+pixel_sz));
	glVertex2i(((x-k)-pixel_sz),((z-k)+pixel_sz));
	
	glVertex2i(((x+k)-pixel_sz),((z-k)-pixel_sz));
	glVertex2i(((x+k)+pixel_sz),((z-k)-pixel_sz));
	glVertex2i(((x+k)+pixel_sz),((z-k)+pixel_sz));
	glVertex2i(((x+k)-pixel_sz),((z-k)+pixel_sz));

	glVertex2i(((x+k)-pixel_sz),((z+k)-pixel_sz));
	glVertex2i(((x+k)+pixel_sz),((z+k)-pixel_sz));
	glVertex2i(((x+k)+pixel_sz),((z+k)+pixel_sz));
	glVertex2i(((x+k)-pixel_sz),((z+k)+pixel_sz));

	glVertex2i(((x-k)-pixel_sz),((z+k)-pixel_sz));
	glVertex2i(((x-k)+pixel_sz),((z+k)-pixel_sz));
	glVertex2i(((x-k)+pixel_sz),((z+k)+pixel_sz));
	glVertex2i(((x-k)-pixel_sz),((z+k)+pixel_sz));
	
	glEnd();
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
	int				n,sel_count,type,portal_idx,main_idx,sub_idx;
	
	sel_count=select_count();
	sel_count--;
	
		// draw the selection
		
	for (n=sel_count;n>=0;n--) {
	
		select_get(n,&type,&portal_idx,&main_idx,&sub_idx);
		
			// draw selection
			
		switch (type) {
		
			case mesh_piece:
				top_view_piece_selection_draw_mesh(portal_idx,main_idx,sub_idx);
				break;
				
			case liquid_piece:
				top_view_piece_selection_draw_liquid(portal_idx,main_idx);
				break;
				
			case node_piece:
				top_view_piece_selection_draw_rect_pos(&map.nodes[main_idx].pos,500);
				break;
				
			case spot_piece:
				top_view_piece_selection_draw_rect_pos(&map.spots[main_idx].pos,700);
				break;
				
			case scenery_piece:
				top_view_piece_selection_draw_rect_pos(&map.sceneries[main_idx].pos,700);
				break;
				
			case light_piece:
				top_view_piece_selection_draw_rect_pos(&map.lights[main_idx].pos,600);
				break;
				
			case sound_piece:
				top_view_piece_selection_draw_rect_pos(&map.sounds[main_idx].pos,600);
				break;
				
			case particle_piece:
				top_view_piece_selection_draw_rect_pos(&map.particles[main_idx].pos,600);
				break;
		}
	}
}
			
