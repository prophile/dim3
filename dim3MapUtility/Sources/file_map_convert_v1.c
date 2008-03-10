/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map File Reading

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

extern maputility_settings_type		maputility_settings;

/* =======================================================

      Add Vertex to Mesh
      
======================================================= */

int map_convert_segment_to_mesh_find_point_vlist(int nvertex,d3pnt *vlist,int x,int y,int z)
{
	int				n;
	d3pnt			*vptr;

	vptr=vlist;

	for (n=0;n!=nvertex;n++) {
		if ((vptr->x==x) && (vptr->y==y) && (vptr->z==z)) return(n);
		vptr++;
	}

	return(-1);
}

int map_convert_segment_to_mesh_add_point_vlist(int nvertex,d3pnt *vlist,int x,int y,int z)
{
	d3pnt			*vptr;

		// already in list?

	if (map_convert_segment_to_mesh_find_point_vlist(nvertex,vlist,x,y,z)!=-1) return(nvertex);

		// add to list

	vptr=vlist+nvertex;

	vptr->x=x;
	vptr->y=y;
	vptr->z=z;

	return(nvertex+1);
}

int map_convert_segment_to_mesh_add_poly_vlist(int nvertex,d3pnt *vlist,int ptsz,int *x,int *y,int *z)
{
	int				n;

	for (n=0;n!=ptsz;n++) {
		nvertex=map_convert_segment_to_mesh_add_point_vlist(nvertex,vlist,x[n],y[n],z[n]);
	}

	return(nvertex);
}

/* =======================================================

      Add Polygon to Mesh
      
======================================================= */

void map_convert_segment_to_mesh_add_mesh_poly(map_mesh_type *map_mesh,int ptsz,int *x,int *y,int *z,float *gx,float *gy,segment_type *seg)
{
	int					n;
	map_mesh_poly_type	*mesh_poly;

	mesh_poly=map_mesh->polys+map_mesh->npoly;

		// get the vertexes

	for (n=0;n!=ptsz;n++) {
		mesh_poly->v[n]=(short)map_convert_segment_to_mesh_find_point_vlist(map_mesh->nvertex,map_mesh->vertexes,x[n],y[n],z[n]);
	}
	
	memmove(mesh_poly->gx,gx,(sizeof(float)*ptsz));
	memmove(mesh_poly->gy,gy,(sizeof(float)*ptsz));
	
	mesh_poly->x_shift=seg->x_shift;
	mesh_poly->y_shift=seg->y_shift;
	mesh_poly->dark_factor=seg->dark_factor;
	mesh_poly->alpha=seg->alpha;

	mesh_poly->flag.on=TRUE;
	mesh_poly->flag.pass_through=seg->pass_through || (seg->type==sg_ambient_wall) || (seg->type==sg_ambient_fc);
	mesh_poly->flag.moveable=seg->moveable;
	mesh_poly->flag.climbable=seg->climbable;
	mesh_poly->flag.touched=FALSE;

	mesh_poly->ptsz=ptsz;
	mesh_poly->txt_idx=seg->fill;

	map_mesh->npoly++;
}

/* =======================================================

      Add Liquid to Portal
      
======================================================= */

void map_convert_liquid(map_type *map,int rn,segment_type *seg)
{
	portal_type		*portal;
	map_liquid_type	*liquid;

	if (!map_portal_liquid_add(map,rn,1)) return;

	portal=&map->portals[rn];
	liquid=&portal->liquid.liquids[portal->liquid.nliquid-1];

	liquid->y=(seg->data.liquid.y+1)*map_enlarge;
	liquid->lft=seg->data.liquid.lft*map_enlarge;
	liquid->rgt=seg->data.liquid.rgt*map_enlarge;
	liquid->top=seg->data.liquid.top*map_enlarge;
	liquid->bot=seg->data.liquid.bot*map_enlarge;

	liquid->alpha=seg->alpha;
	liquid->speed_alter=seg->data.liquid.speed_alter;
	liquid->tint_alpha=seg->data.liquid.tint_alpha;
	
	liquid->x_txtfact=seg->x_txtfact;
	liquid->y_txtfact=seg->y_txtfact;
	liquid->x_txtoff=seg->x_txtoff;
	liquid->y_txtoff=seg->y_txtoff;
	liquid->x_shift=seg->x_shift;
	liquid->y_shift=seg->y_shift;
	
	liquid->txt_idx=seg->fill;
	liquid->group_idx=seg->group_idx;

	memmove(&liquid->col,&seg->data.liquid.col,sizeof(d3col));

	liquid->harm.in_harm=seg->data.liquid.harm;
	liquid->harm.drown_harm=seg->data.liquid.drown_harm;
	liquid->harm.drown_tick=seg->data.liquid.drown_tick;
	
	liquid->tide.rate=seg->data.liquid.tiderate;
	liquid->tide.high=seg->data.liquid.tidesize;
	liquid->tide.direction=seg->data.liquid.tidedirection;
	liquid->tide.split=seg->data.liquid.wavesize;
}

/* =======================================================

      Enlarge Map
      
======================================================= */

void map_convert_enlarge(map_type *map)
{
	int					n,t;
	portal_type			*portal;
	segment_type		*seg;
	map_scenery_type	*scenery;
	map_light_type		*light;
	map_sound_type		*sound;
	map_particle_type	*particle;
	node_type			*node;
	spot_type			*spot;

		// enlarge portals

	portal=map->portals;

	for (n=0;n!=map->nportal;n++) {
		portal->x*=map_enlarge;
		portal->z*=map_enlarge;
		portal->ex*=map_enlarge;
		portal->ez*=map_enlarge;

		portal++;
	}

		// enlarge segments
	
	seg=map->segments;

	for (n=0;n!=map->nsegment;n++) {

		switch (seg->type) {

			case sg_wall:
				seg->data.wall.lx*=map_enlarge;
				seg->data.wall.rx*=map_enlarge;
				seg->data.wall.lz*=map_enlarge;
				seg->data.wall.rz*=map_enlarge;
				seg->data.wall.ty*=map_enlarge;
				seg->data.wall.by=(seg->data.wall.by+1)*map_enlarge;
 				break;
				
			case sg_floor:
            case sg_ceiling:
				for (t=0;t!=seg->data.fc.ptsz;t++) {
					seg->data.fc.x[t]*=map_enlarge;
                    seg->data.fc.y[t]*=map_enlarge;
					seg->data.fc.z[t]*=map_enlarge;
				}
				break;

			case sg_ambient_wall:
				seg->data.ambient_wall.lx*=map_enlarge;
				seg->data.ambient_wall.rx*=map_enlarge;
				seg->data.ambient_wall.lz*=map_enlarge;
				seg->data.ambient_wall.rz*=map_enlarge;
				seg->data.ambient_wall.ty*=map_enlarge;
				seg->data.ambient_wall.by=(seg->data.ambient_wall.by+1)*map_enlarge;
				break;
				
			case sg_ambient_fc:
				for (t=0;t!=seg->data.ambient_fc.ptsz;t++) {
					seg->data.ambient_fc.x[t]*=map_enlarge;
                    seg->data.ambient_fc.y[t]*=map_enlarge;
					seg->data.ambient_fc.z[t]*=map_enlarge;
				}
				break;

		}

		seg++;
	}

		// scenery

	scenery=map->sceneries;
	
	for (n=0;n!=map->nscenery;n++) {
		portal=&map->portals[scenery->pos.rn];
		scenery->pos.x=(scenery->pos.x*map_enlarge);
		scenery->pos.y=(scenery->pos.y+1)*map_enlarge;
		scenery->pos.z=(scenery->pos.z*map_enlarge);
		scenery++;
	}
		
		// map lights
	
	light=map->lights;
	
	for (n=0;n!=map->nlight;n++) {
		portal=&map->portals[light->pos.rn];
		light->intensity*=map_enlarge;
		light->pos.x=(light->pos.x*map_enlarge);
		light->pos.y=(light->pos.y+1)*map_enlarge;
		light->pos.z=(light->pos.z*map_enlarge);
		light++;
	}
	
		// map sounds
	
	sound=map->sounds;
	
	for (n=0;n!=map->nsound;n++) {
		portal=&map->portals[sound->pos.rn];
		sound->pos.x=(sound->pos.x*map_enlarge);
		sound->pos.y=(sound->pos.y+1)*map_enlarge;
		sound->pos.z=(sound->pos.z*map_enlarge);
		
		sound++;
	}
	
		// map particles
	
	particle=map->particles;
	
	for (n=0;n!=map->nparticle;n++) {
		portal=&map->portals[particle->pos.rn];
		particle->pos.x=(particle->pos.x*map_enlarge);
		particle->pos.y=(particle->pos.y+1)*map_enlarge;
		particle->pos.z=(particle->pos.z*map_enlarge);
		
		particle++;
	}
	
		// nodes

	node=map->nodes;
	
	for (n=0;n!=map->nnode;n++) {
        node->idx=n;
		portal=&map->portals[node->pos.rn];
		node->pos.x=(node->pos.x*map_enlarge);
		node->pos.y=(node->pos.y+1)*map_enlarge;
		node->pos.z=(node->pos.z*map_enlarge);
		node++;
	}
    
		// object starts
		
	spot=map->spots;

	for (n=0;n!=map->nspot;n++) {
		portal=&map->portals[spot->pos.rn];
		spot->pos.x=(spot->pos.x*map_enlarge);
		spot->pos.y=(spot->pos.y+1)*map_enlarge;
		spot->pos.z=(spot->pos.z*map_enlarge);
		spot++;
	}
}

/* =======================================================

      Turn Segments into Polygons
      
======================================================= */

void map_convert_segment_orient_uv(int ptsz,float *gx,float *gy,int txt_ang)
{
	int				n;
	float			g;
	
	if (txt_ang==ta_0) return;
	
	for (n=0;n!=ptsz;n++) {
	
		switch (txt_ang) {
		
			case ta_90:
				g=gx[n];
				gx[n]=gy[n];
				gy[n]=-g;
				break;
				
			case ta_180:
				gx[n]=-gx[n];
				gy[n]=-gy[n];
				break;
				
			case ta_270:
				g=gx[n];
				gx[n]=-gy[n];
				gy[n]=g;
				break;
		
		}
	}
}

void map_convert_segments_wall(segment_type *seg)
{
	int					ptsz,my;
    wall_segment_data	*wall;

	wall=&seg->data.wall;
	
	switch (seg->clip) {
	
		case wc_bottom:
			ptsz=3;
			wall->x[0]=wall->lx;
			wall->x[1]=wall->x[2]=wall->rx;
			wall->z[0]=wall->lz;
			wall->z[1]=wall->z[2]=wall->rz;
			wall->y[0]=wall->y[1]=wall->ty;
			wall->y[2]=wall->by+1;
			seg->draw.gx[0]=seg->x_txtoff;
			seg->draw.gx[1]=seg->draw.gx[2]=seg->x_txtoff+seg->x_txtfact;
			seg->draw.gy[0]=seg->draw.gy[1]=seg->y_txtoff;
			seg->draw.gy[2]=seg->y_txtoff+seg->y_txtfact;
			break;
			
		case wc_top:
			ptsz=3;
			wall->x[0]=wall->x[2]=wall->lx;
			wall->x[1]=wall->rx;
			wall->z[0]=wall->z[2]=wall->lz;
			wall->z[1]=wall->rz;
			wall->y[0]=wall->ty;
			wall->y[1]=wall->y[2]=wall->by+1;
			seg->draw.gx[0]=seg->draw.gx[2]=seg->x_txtoff;
			seg->draw.gx[1]=seg->x_txtoff+seg->x_txtfact;
			seg->draw.gy[0]=seg->y_txtoff;
			seg->draw.gy[1]=seg->draw.gy[2]=seg->y_txtoff+seg->y_txtfact;
			break;
			
		case wc_slant:
			ptsz=4;
			my=(wall->by+wall->ty)>>1;
			wall->x[0]=wall->x[3]=wall->lx;
			wall->x[1]=wall->x[2]=wall->rx;
			wall->z[0]=wall->z[3]=wall->lz;
			wall->z[1]=wall->z[2]=wall->rz;
			wall->y[0]=wall->ty;
			wall->y[1]=wall->y[3]=(wall->by+wall->ty)>>1;
			wall->y[2]=wall->by+1;
			seg->draw.gx[0]=seg->draw.gx[3]=seg->x_txtoff;
			seg->draw.gx[1]=seg->draw.gx[2]=seg->x_txtoff+seg->x_txtfact;
			seg->draw.gy[0]=seg->y_txtoff;
			seg->draw.gy[1]=seg->draw.gy[3]=seg->y_txtoff+(seg->y_txtfact/2);
			seg->draw.gy[2]=seg->y_txtoff+seg->y_txtfact;
			break;

        default:
			ptsz=4;
			wall->x[0]=wall->x[3]=wall->lx;
			wall->x[1]=wall->x[2]=wall->rx;
			wall->z[0]=wall->z[3]=wall->lz;
			wall->z[1]=wall->z[2]=wall->rz;
			wall->y[0]=wall->y[1]=wall->ty;
			wall->y[2]=wall->y[3]=wall->by+1;
            seg->draw.gx[0]=seg->draw.gx[3]=seg->x_txtoff;
            seg->draw.gx[1]=seg->draw.gx[2]=seg->x_txtoff+seg->x_txtfact;
            seg->draw.gy[0]=seg->draw.gy[1]=seg->y_txtoff;
            seg->draw.gy[2]=seg->draw.gy[3]=seg->y_txtoff+seg->y_txtfact;
            break;
	}
	
	map_convert_segment_orient_uv(ptsz,seg->draw.gx,seg->draw.gy,seg->txt_ang);

	wall->ptsz=ptsz;
}

void map_convert_segments_fc(segment_type *seg)
{
	int					n,ptsz,lft,rgt,top,bot;
    float				xsize,ysize,x_txtoff,y_txtoff,x_txtfact,y_txtfact;
    fc_segment_data		*fc;
    
	fc=&seg->data.fc;
	
    ptsz=fc->ptsz;

		// get extents
		
	lft=rgt=fc->x[0];
	top=bot=fc->z[0];

	for (n=0;n!=ptsz;n++) {
		if (fc->x[n]<lft) lft=fc->x[n];
		if (fc->x[n]>rgt) rgt=fc->x[n];
		
		if (fc->z[n]<top) top=fc->z[n];
		if (fc->z[n]>bot) bot=fc->z[n];
	}
	
		// get texture coordinates
		
	xsize=(float)(rgt-lft);
	ysize=(float)(bot-top);
 
	x_txtoff=seg->x_txtoff;
	x_txtfact=seg->x_txtfact;
	y_txtoff=seg->y_txtoff;
	y_txtfact=seg->y_txtfact;
   
    for (n=0;n!=ptsz;n++) {
		seg->draw.gx[n]=x_txtoff+((x_txtfact*(float)(fc->x[n]-lft))/xsize);
		seg->draw.gy[n]=y_txtoff+((y_txtfact*(float)(fc->z[n]-top))/ysize);
    }
	
	map_convert_segment_orient_uv(ptsz,seg->draw.gx,seg->draw.gy,seg->txt_ang);
}

void map_convert_segments_ambient_wall(segment_type *seg)
{
	ambient_wall_segment_data	*ambient_wall;
	segment_draw_type			*draw;
    
	ambient_wall=&seg->data.ambient_wall;
	draw=&seg->draw;

	ambient_wall->ptsz=4;
	ambient_wall->x[0]=ambient_wall->x[3]=ambient_wall->lx;
	ambient_wall->x[1]=ambient_wall->x[2]=ambient_wall->rx;
	ambient_wall->z[0]=ambient_wall->z[3]=ambient_wall->lz;
	ambient_wall->z[1]=ambient_wall->z[2]=ambient_wall->rz;
	ambient_wall->y[0]=ambient_wall->y[1]=ambient_wall->ty;
	ambient_wall->y[2]=ambient_wall->y[3]=ambient_wall->by+1;

	draw->gx[0]=draw->gx[3]=seg->x_txtoff+seg->x_txtfact;
	draw->gx[1]=draw->gx[2]=seg->x_txtoff;
	draw->gy[0]=draw->gy[1]=seg->y_txtoff;
	draw->gy[2]=draw->gy[3]=seg->y_txtoff+seg->y_txtfact;
	
	map_prepare_create_push_ambient_wall_segment_polygon(seg);
	map_convert_segment_orient_uv(4,seg->draw.gx,seg->draw.gy,seg->txt_ang);
}

void map_convert_segments_ambient_fc(segment_type *seg)
{
	int						n,ptsz,lft,rgt,top,bot;
    float					xsize,ysize,x_txtoff,y_txtoff,x_txtfact,y_txtfact;
    ambient_fc_segment_data	*ambient_fc;
	segment_draw_type		*draw;
    
	ambient_fc=&seg->data.ambient_fc;
 	draw=&seg->draw;
 
    ptsz=ambient_fc->ptsz;
 
		// get extents
		
	lft=rgt=ambient_fc->x[0];
	top=bot=ambient_fc->z[0];

	for (n=0;n!=ptsz;n++) {
		if (ambient_fc->x[n]<lft) lft=ambient_fc->x[n];
		if (ambient_fc->x[n]>rgt) rgt=ambient_fc->x[n];
		
		if (ambient_fc->z[n]<top) top=ambient_fc->z[n];
		if (ambient_fc->z[n]>bot) bot=ambient_fc->z[n];
	}

		// create uvs

	xsize=(float)(rgt-lft);
	ysize=(float)(bot-top);
 
	x_txtoff=seg->x_txtoff;
	x_txtfact=seg->x_txtfact;
	y_txtoff=seg->y_txtoff;
	y_txtfact=seg->y_txtfact;
   
    for (n=0;n!=ptsz;n++) {
		draw->gx[n]=x_txtoff+((x_txtfact*(float)(ambient_fc->x[n]-lft))/xsize);
		draw->gy[n]=y_txtoff+((y_txtfact*(float)(ambient_fc->z[n]-top))/ysize);
    }

	map_prepare_push_ambient_fc_segment_polygon(seg);
	map_convert_segment_orient_uv(ptsz,seg->draw.gx,seg->draw.gy,seg->txt_ang);
}

void map_convert_segments(map_type *map)
{
	int				n;
	segment_type	*seg;
	
	seg=map->segments;

	for (n=0;n!=map->nsegment;n++) {

		switch (seg->type) {

			case sg_wall:
				map_convert_segments_wall(seg);
				break;

			case sg_floor:
			case sg_ceiling:
				map_convert_segments_fc(seg);
				break;

			case sg_ambient_wall:
				map_convert_segments_ambient_wall(seg);
				break;
				
			case sg_ambient_fc:
				map_convert_segments_ambient_fc(seg);
				break;


		}

		seg++;
	}
}

/* =======================================================

      Create Curved and Clipped Segments
      
======================================================= */

void map_convert_tesselate_curves_clips(map_type *map)
{
	int					i,orig_nsegment;
	segment_type		*seg;
	        
	map_prepare_setup_curve_constant(2);		// supergumba -- hard coded to high
 
		// create new segments for curves
		
    seg=map->segments;
    orig_nsegment=map->nsegment;
    
	for (i=0;i!=orig_nsegment;i++) {
    
		switch (seg->type) {
			
			case sg_wall:
				if (seg->curve!=cv_none) {
					map_prepare_create_wall_curve(map,seg);
					break;
				}
				if ((seg->clip>=wc_tessel_start) && (seg->clip<=wc_tessel_end)) {
					map_prepare_create_wall_clip(map,seg);
					break;
				}
				break;
				
			case sg_floor:
			case sg_ceiling:
				if (seg->curve!=cv_none) map_prepare_create_fc_curve(map,seg);
				break;
				
		}
        
        seg++;
	}
}

/* =======================================================

      Get a List of Segment Primitives and Groups
      
======================================================= */

int map_convert_get_primitive_group_list(map_type *map,int rn,int *plist)
{
	int				n,k,p_idx,p_cnt;
	segment_type	*seg;

		// translate all groups into primitives with high IDs

	seg=map->segments;

	for (n=0;n!=map->nsegment;n++) {
		if ((seg->rn==rn) && (seg->group_idx!=-1)) seg->primitive_uid[0]=seg->group_idx*1000;
		seg++;
	}

		// sort primitives

	p_cnt=0;
	seg=map->segments;

	for (n=0;n!=map->nsegment;n++) {

		if ((seg->rn==rn) && (seg->primitive_uid[0]!=-1)) {
			
			p_idx=-1;

			for (k=0;k!=p_cnt;k++) {
				if (seg->primitive_uid[0]==plist[k]) {
					p_idx=k;
					break;
				}
			}

			if (p_idx==-1) {
				plist[p_cnt]=seg->primitive_uid[0];
				p_cnt++;
			}
		}

		seg++;
	}

	return(p_cnt);
}

/* =======================================================

      Convert One Section of Segments to Mesh
      
======================================================= */

bool map_convert_segment_section_to_mesh(map_type *map,int rn,int primitive_uid,int seg_type,d3pnt *vlist,int vlist_sz)
{
	int					n,nvertex,npoly,mesh_idx;
	portal_type			*portal;
	segment_type		*seg;
	map_mesh_type		*map_mesh;

	portal=&map->portals[rn];

		// clear vertex list

	bzero(vlist,vlist_sz);
	
		// count the vertexes and polys

	nvertex=npoly=0;

	for (n=0;n!=map->nsegment;n++) {
		seg=&map->segments[n];
		if (seg->rn!=rn) continue;
		if (seg->primitive_uid[0]!=primitive_uid) continue;
		if ((seg_type!=-1) && (seg->type!=seg_type)) continue;

		switch (seg->type) {

			case sg_wall:
				nvertex=map_convert_segment_to_mesh_add_poly_vlist(nvertex,vlist,seg->data.wall.ptsz,seg->data.wall.x,seg->data.wall.y,seg->data.wall.z);
				npoly++;
				break;

			case sg_floor:
			case sg_ceiling:
				nvertex=map_convert_segment_to_mesh_add_poly_vlist(nvertex,vlist,seg->data.fc.ptsz,seg->data.fc.x,seg->data.fc.y,seg->data.fc.z);
				npoly++;
				break;

			case sg_ambient_wall:
				nvertex=map_convert_segment_to_mesh_add_poly_vlist(nvertex,vlist,seg->data.ambient_wall.ptsz,seg->data.ambient_wall.x,seg->data.ambient_wall.y,seg->data.ambient_wall.z);
				npoly++;
				break;

			case sg_ambient_fc:
				nvertex=map_convert_segment_to_mesh_add_poly_vlist(nvertex,vlist,seg->data.ambient_fc.ptsz,seg->data.ambient_fc.x,seg->data.ambient_fc.y,seg->data.ambient_fc.z);
				npoly++;
				break;

		}

	}

	if ((nvertex==0) || (npoly==0)) return(TRUE);

		// create new mesh

	mesh_idx=portal->mesh.nmesh;

	if (!map_portal_mesh_add(map,rn,1)) return(FALSE);

	if (!map_portal_mesh_set_vertex_count(map,rn,mesh_idx,nvertex)) {
		map_portal_mesh_delete(map,rn,mesh_idx);
		return(FALSE);
	}

	if (!map_portal_mesh_set_poly_count(map,rn,mesh_idx,npoly)) {
		map_portal_mesh_delete(map,rn,mesh_idx);
		return(FALSE);
	}

	map_mesh=&portal->mesh.meshes[mesh_idx];

		// move over vertexes

	map_mesh->nvertex=nvertex;
	memmove(map_mesh->vertexes,vlist,(sizeof(d3pnt)*nvertex));
		
		// add polys

	map_mesh->npoly=0;

	for (n=0;n!=map->nsegment;n++) {
		seg=&map->segments[n];
		if (seg->rn!=rn) continue;
		if (seg->primitive_uid[0]!=primitive_uid) continue;
		if ((seg_type!=-1) && (seg->type!=seg_type)) continue;

		switch (seg->type) {

			case sg_wall:
				map_convert_segment_to_mesh_add_mesh_poly(map_mesh,seg->data.wall.ptsz,seg->data.wall.x,seg->data.wall.y,seg->data.wall.z,seg->draw.gx,seg->draw.gy,seg);
				break;

			case sg_floor:
			case sg_ceiling:
				map_convert_segment_to_mesh_add_mesh_poly(map_mesh,seg->data.fc.ptsz,seg->data.fc.x,seg->data.fc.y,seg->data.fc.z,seg->draw.gx,seg->draw.gy,seg);
				break;

			case sg_ambient_wall:
				map_convert_segment_to_mesh_add_mesh_poly(map_mesh,seg->data.ambient_wall.ptsz,seg->data.ambient_wall.x,seg->data.ambient_wall.y,seg->data.ambient_wall.z,seg->draw.gx,seg->draw.gy,seg);
				break;

			case sg_ambient_fc:
				map_convert_segment_to_mesh_add_mesh_poly(map_mesh,seg->data.ambient_fc.ptsz,seg->data.ambient_fc.x,seg->data.ambient_fc.y,seg->data.ambient_fc.z,seg->draw.gx,seg->draw.gy,seg);
				break;

		}

			// groups are sorted together into meshes
			// so tranfer any groups to mesh

		map_mesh->group_idx=seg->group_idx;
	}

	return(TRUE);
}

/* =======================================================

      Convert Entire Map
      
======================================================= */

bool map_convert_v1(map_type *map)
{
	int					n,i,p,vlist_sz,plist_sz;
	int					*plist;
	d3pnt				*vlist;
	portal_type			*portal;
	segment_type		*seg;

		// memory for vertex and primitive lists
		// just use enough vertexes to cover most maps,
		// supergumba -- need to do this in a more proper way

	vlist_sz=sizeof(d3pnt)*10000;

	vlist=(d3pnt*)valloc(vlist_sz);
	if (vlist==NULL) return(FALSE);

	plist_sz=sizeof(int)*500;

	plist=(int*)valloc(plist_sz);
	if (plist==NULL) {
		free(vlist);
		return(FALSE);
	}

		// enlarge map and setup vertexes
		// and UVs inside the segments

	map_convert_enlarge(map);
	map_convert_segments(map);

		// tesselate up any curved or clipped segments into
		// multiple segments before converting to a mesh

	map_convert_tesselate_curves_clips(map);

		// create a mesh out of all segments in a portal

	for (n=0;n!=map->nportal;n++) {
	
		portal=&map->portals[n];
	
		portal->mesh.nmesh=0;

			// create meshes from all primitives
			// and groups

		plist_sz=map_convert_get_primitive_group_list(map,n,plist);

		for (p=0;p!=plist_sz;p++) {

			if (!map_convert_segment_section_to_mesh(map,n,plist[p],-1,vlist,vlist_sz)) {
				free(vlist);
				free(plist);
				return(FALSE);
			}
		}

			// create meshes for each left over type

		if (!map_convert_segment_section_to_mesh(map,n,-1,sg_wall,vlist,vlist_sz)) {
			free(vlist);
			free(plist);
			return(FALSE);
		}

		if (!map_convert_segment_section_to_mesh(map,n,-1,sg_floor,vlist,vlist_sz)) {
			free(vlist);
			free(plist);
			return(FALSE);
		}

		if (!map_convert_segment_section_to_mesh(map,n,-1,sg_ceiling,vlist,vlist_sz)) {
			free(vlist);
			free(plist);
			return(FALSE);
		}

		if (!map_convert_segment_section_to_mesh(map,n,-1,sg_ambient_wall,vlist,vlist_sz)) {
			free(vlist);
			free(plist);
			return(FALSE);
		}

		if (!map_convert_segment_section_to_mesh(map,n,-1,sg_ambient_fc,vlist,vlist_sz)) {
			free(vlist);
			free(plist);
			return(FALSE);
		}

	}
	
	free(vlist);
	free(plist);

		// convert liquids

	for (n=0;n!=map->nportal;n++) {
	
		portal=&map->portals[n];
	
		portal->liquid.nliquid=0;

		seg=map->segments;

		for (i=0;i!=map->nsegment;i++) {
			if ((seg->rn==n) && (seg->type==sg_liquid)) map_convert_liquid(map,n,seg);
			seg++;
		}
	}

		// turn off all segments

	map->nsegment=0;

	return(TRUE);
}



