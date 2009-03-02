/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map File Reading (v1 [segment] maps)

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

extern maputility_settings_type		maputility_settings;

extern char				media_type_str[][32],
						sky_type_str[][32],
						gl_fog_type_str[][32],
						liquid_tide_direction_str[][32],
                        light_type_str[][32],
						lighting_mode_str[][32],
						map_bump_mode_str[][32];


char					segment_type_str[][32]={"wall","floor","ceiling","liquid","ambient",""},
                        segment_clip_str[][32]={"none","top","bottom","slant","top_curve","top_arch","bottom_curve","bottom_arch","top_curve_arch","bottom_curve_arch",""},
                        segment_curve_str[][32]={"none","forward","backward",""},
                        segment_push_str[][32]={"none","top","bottom","left","right","up","down",""};

extern int map_prepare_create_fc_curve(int seg_cnt,segment_type *seg_list,segment_type *seg);
extern int map_prepare_create_wall_curve(int seg_cnt,segment_type *seg_list,segment_type *seg);
extern int map_prepare_create_wall_clip(int seg_cnt,segment_type *seg_list,segment_type *seg);

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

	if (seg->pass_through || (seg->type==sg_ambient_wall) || (seg->type==sg_ambient_fc)) map_mesh->flag.pass_through=TRUE;
	if (seg->moveable) map_mesh->flag.moveable=TRUE;
	if (seg->climbable) map_mesh->flag.climbable=TRUE;

	mesh_poly->ptsz=ptsz;
	mesh_poly->txt_idx=seg->fill;

	map_mesh->npoly++;
}

/* =======================================================

      Add Liquid to Portal
      
======================================================= */

void map_convert_liquid(map_type *map,portal_type *portal,segment_type *seg)
{
	int				liq_idx,sz;
	map_liquid_type	*liquid;

	liq_idx=map_liquid_add(map);
	if (liq_idx==-1) return;

	liquid=&map->liquid.liquids[liq_idx];

	liquid->y=(seg->data.liquid.y+1)*map_enlarge;
	liquid->lft=(seg->data.liquid.lft*map_enlarge)+portal->x;
	liquid->rgt=(seg->data.liquid.rgt*map_enlarge)+portal->x;
	liquid->top=(seg->data.liquid.top*map_enlarge)+portal->z;
	liquid->bot=(seg->data.liquid.bot*map_enlarge)+portal->z;
	
	liquid->depth=5000;

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

	sz=liquid->rgt-liquid->lft;
	if ((liquid->bot-liquid->top)>sz) sz=liquid->bot-liquid->top;
	liquid->tide.division=sz/seg->data.liquid.wavesize;
	liquid->tide.rate=seg->data.liquid.tiderate;
	liquid->tide.high=seg->data.liquid.tidesize;
	liquid->tide.direction=seg->data.liquid.tidedirection;
}

/* =======================================================

      Enlarge Map
      
======================================================= */

void map_convert_enlarge(int nportal,portal_type *portals,int seg_cnt,segment_type *seg_list)
{
	int					n,t;
	portal_type			*portal;
	segment_type		*seg;

		// enlarge portals

	portal=portals;

	for (n=0;n!=nportal;n++) {
		portal->x*=map_enlarge;
		portal->z*=map_enlarge;
		portal->ex*=map_enlarge;
		portal->ez*=map_enlarge;

		portal++;
	}

		// enlarge segments
	
	seg=seg_list;

	for (n=0;n!=seg_cnt;n++) {

		portal=&portals[seg->rn];

		switch (seg->type) {

			case sg_wall:
				seg->data.wall.lx=(seg->data.wall.lx*map_enlarge)+portal->x;
				seg->data.wall.rx=(seg->data.wall.rx*map_enlarge)+portal->x;
				seg->data.wall.lz=(seg->data.wall.lz*map_enlarge)+portal->z;
				seg->data.wall.rz=(seg->data.wall.rz*map_enlarge)+portal->z;
				seg->data.wall.ty*=map_enlarge;
				seg->data.wall.by=(seg->data.wall.by+1)*map_enlarge;
 				break;
				
			case sg_floor:
            case sg_ceiling:
				for (t=0;t!=seg->data.fc.ptsz;t++) {
					seg->data.fc.x[t]=(seg->data.fc.x[t]*map_enlarge)+portal->x;
                    seg->data.fc.y[t]*=map_enlarge;
					seg->data.fc.z[t]=(seg->data.fc.z[t]*map_enlarge)+portal->z;
				}
				break;

			case sg_ambient_wall:
				seg->data.ambient_wall.lx=(seg->data.ambient_wall.lx*map_enlarge)+portal->x;
				seg->data.ambient_wall.rx=(seg->data.ambient_wall.rx*map_enlarge)+portal->x;
				seg->data.ambient_wall.lz=(seg->data.ambient_wall.lz*map_enlarge)+portal->z;
				seg->data.ambient_wall.rz=(seg->data.ambient_wall.rz*map_enlarge)+portal->z;
				seg->data.ambient_wall.ty*=map_enlarge;
				seg->data.ambient_wall.by=(seg->data.ambient_wall.by+1)*map_enlarge;
				break;
				
			case sg_ambient_fc:
				for (t=0;t!=seg->data.ambient_fc.ptsz;t++) {
					seg->data.ambient_fc.x[t]=(seg->data.ambient_fc.x[t]*map_enlarge)+portal->x;
                    seg->data.ambient_fc.y[t]*=map_enlarge;
					seg->data.ambient_fc.z[t]=(seg->data.ambient_fc.z[t]*map_enlarge)+portal->z;
				}
				break;

		}

		seg++;
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
			wall->y[2]=wall->by;
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
			wall->y[1]=wall->y[2]=wall->by;
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
			wall->y[2]=wall->by;
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
			wall->y[2]=wall->y[3]=wall->by;
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
	
	switch (ambient_wall->push) {
		case ap_top:
			ambient_wall->lz-=ambient_push_size;
			ambient_wall->rz-=ambient_push_size;
			break;
		case ap_bottom:
			ambient_wall->lz+=ambient_push_size;
			ambient_wall->rz+=ambient_push_size;
			break;
		case ap_left:
			ambient_wall->lx-=ambient_push_size;
			ambient_wall->rx-=ambient_push_size;
			break;
		case ap_right:
			ambient_wall->lx+=ambient_push_size;
			ambient_wall->rx+=ambient_push_size;
			break;
		case ap_up:
			ambient_wall->ty-=ambient_push_size;
			ambient_wall->by-=ambient_push_size;
			break;
		case ap_down:
			ambient_wall->ty+=ambient_push_size;
			ambient_wall->by+=ambient_push_size;
			break;
	}

	ambient_wall->ptsz=4;
	ambient_wall->x[0]=ambient_wall->x[3]=ambient_wall->lx;
	ambient_wall->x[1]=ambient_wall->x[2]=ambient_wall->rx;
	ambient_wall->z[0]=ambient_wall->z[3]=ambient_wall->lz;
	ambient_wall->z[1]=ambient_wall->z[2]=ambient_wall->rz;
	ambient_wall->y[0]=ambient_wall->y[1]=ambient_wall->ty;
	ambient_wall->y[2]=ambient_wall->y[3]=ambient_wall->by;

	draw->gx[0]=draw->gx[3]=seg->x_txtoff+seg->x_txtfact;
	draw->gx[1]=draw->gx[2]=seg->x_txtoff;
	draw->gy[0]=draw->gy[1]=seg->y_txtoff;
	draw->gy[2]=draw->gy[3]=seg->y_txtoff+seg->y_txtfact;
	
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
 	
		// handle pushes
   
    for (n=0;n!=ptsz;n++) {
	
		switch (ambient_fc->push) {
			case ap_top:
				ambient_fc->z[n]-=ambient_push_size;
				break;
			case ap_bottom:
				ambient_fc->z[n]+=ambient_push_size;
				break;
			case ap_left:
				ambient_fc->x[n]-=ambient_push_size;
				break;
			case ap_right:
				ambient_fc->x[n]+=ambient_push_size;
				break;
			case ap_up:
				ambient_fc->y[n]-=ambient_push_size;
				break;
			case ap_down:
				ambient_fc->y[n]+=ambient_push_size;
				break;
		}
    }

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

	map_convert_segment_orient_uv(ptsz,seg->draw.gx,seg->draw.gy,seg->txt_ang);
}

void map_convert_segments(int seg_cnt,segment_type *seg_list)
{
	int				n;
	segment_type	*seg;
	
	seg=seg_list;

	for (n=0;n!=seg_cnt;n++) {

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

int map_convert_tesselate_curves_clips(int seg_cnt,segment_type *seg_list)
{
	int					i,orig_nsegment;
	segment_type		*seg;
 
		// create new segments for curves
		
    seg=seg_list;
    orig_nsegment=seg_cnt;
    
	for (i=0;i!=orig_nsegment;i++) {
    
		switch (seg->type) {
			
			case sg_wall:
				if (seg->curve!=cv_none) {
					seg_cnt=map_prepare_create_wall_curve(seg_cnt,seg_list,seg);
					break;
				}
				if ((seg->clip>=wc_tessel_start) && (seg->clip<=wc_tessel_end)) {
					seg_cnt=map_prepare_create_wall_clip(seg_cnt,seg_list,seg);
					break;
				}
				break;
				
			case sg_floor:
			case sg_ceiling:
				if (seg->curve!=cv_none) seg_cnt=map_prepare_create_fc_curve(seg_cnt,seg_list,seg);
				break;
				
		}
        
        seg++;
	}
	
	return(seg_cnt);
}

/* =======================================================

      Get a List of Segment Primitives and Groups
      
======================================================= */

int map_convert_get_primitive_group_list(int seg_cnt,segment_type *seg_list,int rn,int *plist)
{
	int				n,k,p_idx,p_cnt;
	segment_type	*seg;

		// translate all groups into primitives with high IDs

	seg=seg_list;

	for (n=0;n!=seg_cnt;n++) {
		if ((seg->rn==rn) && (seg->group_idx!=-1)) seg->primitive_uid[0]=seg->group_idx*1000;
		seg++;
	}

		// sort primitives

	p_cnt=0;
	seg=seg_list;

	for (n=0;n!=seg_cnt;n++) {

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

bool map_convert_segment_section_to_mesh(map_type *map,int seg_cnt,segment_type *seg_list,int rn,int primitive_uid,int seg_type,d3pnt *vlist,int vlist_sz)
{
	int					n,nvertex,npoly,mesh_idx;
	segment_type		*seg;
	map_mesh_type		*map_mesh;

		// clear vertex list

	bzero(vlist,vlist_sz);
	
		// count the vertexes and polys

	nvertex=npoly=0;

	for (n=0;n!=seg_cnt;n++) {
		seg=&seg_list[n];
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

	mesh_idx=map_mesh_add(map);
	if (mesh_idx==-1) return(FALSE);

	if (!map_mesh_set_vertex_count(map,mesh_idx,nvertex)) {
		map_mesh_delete(map,mesh_idx);
		return(FALSE);
	}

	if (!map_mesh_set_poly_count(map,mesh_idx,npoly)) {
		map_mesh_delete(map,mesh_idx);
		return(FALSE);
	}

	map_mesh=&map->mesh.meshes[mesh_idx];

		// move over vertexes

	map_mesh->nvertex=nvertex;
	memmove(map_mesh->vertexes,vlist,(sizeof(d3pnt)*nvertex));
		
		// add polys

	map_mesh->npoly=0;

	for (n=0;n!=seg_cnt;n++) {
		seg=&seg_list[n];
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

bool map_convert_v1(map_type *map,int nportal,portal_type *portals,int seg_cnt,segment_type *seg_list)
{
	int					n,i,p,vlist_sz,plist_sz,first_mesh_idx;
	int					*plist;
	d3pnt				*vlist;
	portal_type			*portal;
	segment_type		*seg;
	
		// convert map settings
		
    map->sky.radius*=map_enlarge;
	map->sky.dome_y*=map_enlarge;

	map->sky.dome_mirror=FALSE;
	
	map->fog.outer_radius*=map_enlarge;
	map->fog.inner_radius*=map_enlarge;
	map->fog.high*=map_enlarge;
	map->fog.drop*=map_enlarge;

		// memory for vertex and primitive lists
		// just use enough vertexes to cover most maps,
		// supergumba -- need to do this in a more proper way

	vlist_sz=sizeof(d3pnt)*10000;

	vlist=(d3pnt*)malloc(vlist_sz);
	if (vlist==NULL) return(FALSE);

	plist_sz=sizeof(int)*500;

	plist=(int*)malloc(plist_sz);
	if (plist==NULL) {
		free(vlist);
		return(FALSE);
	}

		// enlarge map and setup vertexes
		// and UVs inside the segments

	map_convert_enlarge(nportal,portals,seg_cnt,seg_list);
	map_convert_segments(seg_cnt,seg_list);

		// tesselate up any curved or clipped segments into
		// multiple segments before converting to a mesh

	seg_cnt=map_convert_tesselate_curves_clips(seg_cnt,seg_list);

		// create a mesh out of all segmentss

	map->mesh.nmesh=0;
	map->mesh.meshes=NULL;

	for (n=0;n!=nportal;n++) {
	
		portal=&portals[n];

		first_mesh_idx=map->mesh.nmesh;

			// create meshes from all primitives
			// and groups

		plist_sz=map_convert_get_primitive_group_list(seg_cnt,seg_list,n,plist);

		for (p=0;p!=plist_sz;p++) {

			if (!map_convert_segment_section_to_mesh(map,seg_cnt,seg_list,n,plist[p],-1,vlist,vlist_sz)) {
				free(vlist);
				free(plist);
				return(FALSE);
			}
		}

			// create meshes for each left over type

		if (!map_convert_segment_section_to_mesh(map,seg_cnt,seg_list,n,-1,sg_wall,vlist,vlist_sz)) {
			free(vlist);
			free(plist);
			return(FALSE);
		}

		if (!map_convert_segment_section_to_mesh(map,seg_cnt,seg_list,n,-1,sg_floor,vlist,vlist_sz)) {
			free(vlist);
			free(plist);
			return(FALSE);
		}

		if (!map_convert_segment_section_to_mesh(map,seg_cnt,seg_list,n,-1,sg_ceiling,vlist,vlist_sz)) {
			free(vlist);
			free(plist);
			return(FALSE);
		}

		if (!map_convert_segment_section_to_mesh(map,seg_cnt,seg_list,n,-1,sg_ambient_wall,vlist,vlist_sz)) {
			free(vlist);
			free(plist);
			return(FALSE);
		}

		if (!map_convert_segment_section_to_mesh(map,seg_cnt,seg_list,n,-1,sg_ambient_fc,vlist,vlist_sz)) {
			free(vlist);
			free(plist);
			return(FALSE);
		}

			// convert messages to first mesh

		if (map->mesh.nmesh!=0) memmove(&map->mesh.meshes[first_mesh_idx].msg,&portal->msg,sizeof(map_mesh_message_type));
	}
	
	free(vlist);
	free(plist);

		// convert liquids

	map->liquid.nliquid=0;
	map->liquid.liquids=NULL;
		
	portal=portals;

	for (n=0;n!=nportal;n++) {

		seg=seg_list;

		for (i=0;i!=seg_cnt;i++) {
			if ((seg->rn==n) && (seg->type==sg_liquid)) map_convert_liquid(map,&portals[seg->rn],seg);
			seg++;
		}
		
		portal++;
	}

	return(TRUE);
}


/* =======================================================

      Read Segment XML
      
======================================================= */

void read_single_segment(int tag,segment_type *seg,int rn,int seg_type)
{
	int				n;
	
	seg->rn=rn;
	seg->type=seg_type;
	seg->group_idx=xml_get_attribute_int_default(tag,"group",-1);
	
	for (n=0;n!=max_primitive_stack;n++) {
		seg->primitive_uid[n]=-1;
	}
	
	xml_get_attribute_short_array(tag,"primitive",seg->primitive_uid,max_primitive_stack);
	
	seg->clip=xml_get_attribute_list(tag,"clip",(char*)segment_clip_str);
	seg->curve=xml_get_attribute_list(tag,"curve",(char*)segment_curve_str);
	seg->pass_through=xml_get_attribute_boolean(tag,"pass");
	seg->moveable=xml_get_attribute_boolean(tag,"moveable");
	seg->shiftable=xml_get_attribute_boolean(tag,"shiftable");
	seg->climbable=xml_get_attribute_boolean(tag,"climbable");
	seg->on=!xml_get_attribute_boolean(tag,"off");
	
	seg->fill=xml_get_attribute_int(tag,"fill");
	seg->txt_ang=xml_get_attribute_int_default(tag,"ang",0)/90;
	xml_get_attribute_2_coord_float(tag,"uv_off",&seg->x_txtoff,&seg->y_txtoff);
	xml_get_attribute_2_coord_float(tag,"uv_fct",&seg->x_txtfact,&seg->y_txtfact);
	xml_get_attribute_2_coord_float_default(tag,"shift",&seg->x_shift,&seg->y_shift,0.0f,0.0f);
	seg->dark_factor=xml_get_attribute_float_default(tag,"dark_fct",1);
	seg->alpha=xml_get_attribute_float_default(tag,"alpha",1);
	seg->lock=xml_get_attribute_boolean(tag,"lock");
	
	seg->tag=xml_get_attribute_int_default(tag,"tag",0);
	
	if (seg_type==sg_liquid) {
		seg->data.liquid.harm=xml_get_attribute_int(tag,"harm");
		seg->data.liquid.drown_harm=xml_get_attribute_int_default(tag,"drown_harm",10);
		seg->data.liquid.drown_tick=xml_get_attribute_int_default(tag,"drown_tick",10000);
		seg->data.liquid.speed_alter=xml_get_attribute_float_default(tag,"speed_alter",0.1f);
		seg->data.liquid.wavesize=xml_get_attribute_int_default(tag,"wave_size",(map_enlarge<<2));
		seg->data.liquid.tiderate=xml_get_attribute_int(tag,"tide_rate");
		seg->data.liquid.tidesize=xml_get_attribute_int(tag,"tide_size");
		seg->data.liquid.tidedirection=xml_get_attribute_list(tag,"tide_direction",(char*)liquid_tide_direction_str);
		xml_get_attribute_color(tag,"rgb",&seg->data.liquid.col);
		seg->data.liquid.tint_alpha=xml_get_attribute_float_default(tag,"tint_alpha",0.5f);
		
		if (seg->data.liquid.wavesize<=(map_enlarge<<2)) seg->data.liquid.wavesize=map_enlarge<<2;
		if (seg->data.liquid.tiderate<0) seg->data.liquid.tiderate=0;
		if (seg->data.liquid.tidesize<0) seg->data.liquid.tidesize=0;
	}
	
	if (seg_type==sg_ambient_wall) {
		seg->data.ambient_wall.push=xml_get_attribute_list(tag,"push",(char*)segment_push_str);
	}

	if (seg_type==sg_ambient_fc) {
		seg->data.ambient_fc.push=xml_get_attribute_list(tag,"push",(char*)segment_push_str);
	}
}

/* =======================================================

      Read Map XML
      
======================================================= */

bool decode_map_v1_xml(map_type *map,int map_head)
{
	int						i,k,j,y,idx,nportal,seg_cnt,
							main_portal_tag,portal_tag,msg_tag,
							main_seg_tag,seg_tag,main_light_tag,light_tag,main_sound_tag,sound_tag,
							main_particle_tag,particle_tag,main_node_tag,node_tag,
							main_obj_tag,obj_tag,tag,cnt;
	bool					convert_ok;
    portal_type				*portals,*portal;
    segment_type			*seg_list,*seg;
    map_light_type			*light;
    map_sound_type			*sound;
	map_particle_type		*particle;
    node_type				*node;
    spot_type				*spot;
	map_scenery_type		*scenery;
	
		// temporary segments and portals for converting from v1 to v3
		
	seg_cnt=0;
	
	seg_list=(segment_type*)malloc(max_segment*sizeof(segment_type));
	if (seg_list==NULL) return(FALSE);

	nportal=0;

	portals=(portal_type*)malloc(max_portal*sizeof(portal_type));
	if (portals==NULL) {
		free(seg_list);
		return(FALSE);
	}
	        
        // portals

    main_portal_tag=xml_findfirstchild("Portals",map_head);
    if (main_portal_tag!=-1) {
    
        nportal=xml_countchildren(main_portal_tag);
		portal_tag=xml_findfirstchild("Portal",main_portal_tag);
		
        portal=portals;
    
        for (i=0;i!=nportal;i++) {
            xml_get_attribute_3_coord_int(portal_tag,"tl_c3",&portal->x,&y,&portal->z);
            xml_get_attribute_3_coord_int(portal_tag,"br_c3",&portal->ex,&y,&portal->ez);
   
            portal->msg.entry_on=portal->msg.exit_on=portal->msg.base_on=portal->msg.map_change_on=FALSE;
            portal->msg.entry_id=portal->msg.exit_id=0;
			portal->msg.map_name[0]=0x0;
			strcpy(portal->msg.map_spot_name,"Start");
			strcpy(portal->msg.map_spot_type,"Player");
            
            msg_tag=xml_findfirstchild("Messages",portal_tag);
            if (msg_tag!=-1) {
                tag=xml_findfirstchild("Entry",msg_tag);
                if (tag!=-1) {
                    portal->msg.entry_on=xml_get_attribute_boolean(tag,"on");
                    portal->msg.entry_id=xml_get_attribute_int(tag,"id");
                }
                tag=xml_findfirstchild("Exit",msg_tag);
                if (tag!=-1) {
                    portal->msg.exit_on=xml_get_attribute_boolean(tag,"on");
                    portal->msg.exit_id=xml_get_attribute_int(tag,"id");
                }
                tag=xml_findfirstchild("Base",msg_tag);
                if (tag!=-1) {
                    portal->msg.base_on=xml_get_attribute_boolean(tag,"on");
                    portal->msg.base_team=xml_get_attribute_int(tag,"team");
                }
                tag=xml_findfirstchild("Map",msg_tag);
                if (tag!=-1) {
                    portal->msg.map_change_on=xml_get_attribute_boolean(tag,"on");
					xml_get_attribute_text(tag,"name", portal->msg.map_name,name_str_len);
					xml_get_attribute_text(tag,"spot_name", portal->msg.map_spot_name,name_str_len);
					xml_get_attribute_text(tag,"spot_type", portal->msg.map_spot_type,name_str_len);
                }
            }

				// walls
				
            main_seg_tag=xml_findfirstchild("Walls",portal_tag);
            if (main_seg_tag!=-1) {
                
                cnt=xml_countchildren(main_seg_tag);
				seg_tag=xml_findfirstchild("Wall",main_seg_tag);
                
                for (k=0;k!=cnt;k++) {
					seg=&seg_list[seg_cnt];
					seg_cnt++;
					
					read_single_segment(seg_tag,seg,i,sg_wall);

					tag=xml_findfirstchild("v",seg_tag);
					xml_get_attribute_3_coord_int(tag,"c3",&seg->data.wall.lx,&seg->data.wall.ty,&seg->data.wall.lz);
					tag=xml_findnextchild(tag);
					xml_get_attribute_3_coord_int(tag,"c3",&seg->data.wall.rx,&seg->data.wall.by,&seg->data.wall.rz);

					seg_tag=xml_findnextchild(seg_tag);
				}
			}

				// floors

            main_seg_tag=xml_findfirstchild("Floors",portal_tag);
            if (main_seg_tag!=-1) {
                
                cnt=xml_countchildren(main_seg_tag);
				seg_tag=xml_findfirstchild("Floor",main_seg_tag);
                
                for (k=0;k!=cnt;k++) {
					seg=&seg_list[seg_cnt];
					seg_cnt++;
					
					read_single_segment(seg_tag,seg,i,sg_floor);
					
					seg->data.fc.ptsz=xml_countchildren(seg_tag);
					tag=xml_findfirstchild("v",seg_tag);
					
					for (j=0;j!=seg->data.fc.ptsz;j++) {
						xml_get_attribute_3_coord_int(tag,"c3",&seg->data.fc.x[j],&seg->data.fc.y[j],&seg->data.fc.z[j]);
						tag=xml_findnextchild(tag);
					}
					
					seg_tag=xml_findnextchild(seg_tag);
				}
			}
			
				// ceilings
				
            main_seg_tag=xml_findfirstchild("Ceilings",portal_tag);
            if (main_seg_tag!=-1) {
                
                cnt=xml_countchildren(main_seg_tag);
				seg_tag=xml_findfirstchild("Ceiling",main_seg_tag);
                
                for (k=0;k!=cnt;k++) {
					seg=&seg_list[seg_cnt];
					seg_cnt++;
					
					read_single_segment(seg_tag,seg,i,sg_ceiling);
					
					seg->data.fc.ptsz=xml_countchildren(seg_tag);
					tag=xml_findfirstchild("v",seg_tag);
					
					for (j=0;j!=seg->data.fc.ptsz;j++) {
						xml_get_attribute_3_coord_int(tag,"c3",&seg->data.fc.x[j],&seg->data.fc.y[j],&seg->data.fc.z[j]);
						tag=xml_findnextchild(tag);
					}
					
					seg_tag=xml_findnextchild(seg_tag);
				}
			}

				// liquids
				
            main_seg_tag=xml_findfirstchild("Liquids",portal_tag);
            if (main_seg_tag!=-1) {
                
                cnt=xml_countchildren(main_seg_tag);
				seg_tag=xml_findfirstchild("Liquid",main_seg_tag);
                
                for (k=0;k!=cnt;k++) {
					seg=&seg_list[seg_cnt];
					seg_cnt++;
					
					read_single_segment(seg_tag,seg,i,sg_liquid);
					
					tag=xml_findfirstchild("v",seg_tag);
					xml_get_attribute_3_coord_int(tag,"c3",&seg->data.liquid.lft,&seg->data.liquid.y,&seg->data.liquid.top);
					tag=xml_findnextchild(tag);
					xml_get_attribute_3_coord_int(tag,"c3",&seg->data.liquid.rgt,&seg->data.liquid.y,&seg->data.liquid.bot);
					
					seg_tag=xml_findnextchild(seg_tag);
				}
			}

				// ambient walls
				
            main_seg_tag=xml_findfirstchild("Ambients",portal_tag);
            if (main_seg_tag!=-1) {
                
                cnt=xml_countchildren(main_seg_tag);
				seg_tag=xml_findfirstchild("Ambient",main_seg_tag);
                
                for (k=0;k!=cnt;k++) {
					seg=&seg_list[seg_cnt];
					seg_cnt++;
					
					read_single_segment(seg_tag,seg,i,sg_ambient_wall);
					
					tag=xml_findfirstchild("v",seg_tag);
					xml_get_attribute_3_coord_int(tag,"c3",&seg->data.ambient_wall.lx,&seg->data.ambient_wall.ty,&seg->data.ambient_wall.lz);
					tag=xml_findnextchild(tag);
					xml_get_attribute_3_coord_int(tag,"c3",&seg->data.ambient_wall.rx,&seg->data.ambient_wall.by,&seg->data.ambient_wall.rz);
					
					seg_tag=xml_findnextchild(seg_tag);
				}
			}
			
				// ambient floor/ceilings

            main_seg_tag=xml_findfirstchild("Ambient_FCs",portal_tag);
            if (main_seg_tag!=-1) {
                
                cnt=xml_countchildren(main_seg_tag);
				seg_tag=xml_findfirstchild("Ambient_FC",main_seg_tag);
                
                for (k=0;k!=cnt;k++) {
					seg=&seg_list[seg_cnt];
					seg_cnt++;
					
					read_single_segment(seg_tag,seg,i,sg_ambient_fc);
					
					seg->data.ambient_fc.ptsz=xml_countchildren(seg_tag);
					tag=xml_findfirstchild("v",seg_tag);
					
					for (j=0;j!=seg->data.ambient_fc.ptsz;j++) {
						xml_get_attribute_3_coord_int(tag,"c3",&seg->data.ambient_fc.x[j],&seg->data.ambient_fc.y[j],&seg->data.ambient_fc.z[j]);
						tag=xml_findnextchild(tag);
					}
					
					seg_tag=xml_findnextchild(seg_tag);
				}
			}

				// scenery
		
			main_obj_tag=xml_findfirstchild("Sceneries",portal_tag);
			if (main_obj_tag!=-1) {
			
				cnt=xml_countchildren(main_obj_tag);
				obj_tag=xml_findfirstchild("Scenery",main_obj_tag);
				
				for (k=0;k!=cnt;k++) {
					if (map->nscenery>=max_map_scenery) break;
					
					scenery=&map->sceneries[map->nscenery];
					map->nscenery++;
					
					xml_get_attribute_3_coord_int(obj_tag,"c3",&scenery->pnt.x,&scenery->pnt.y,&scenery->pnt.z);
					
					xml_get_attribute_text(obj_tag,"model_name",scenery->model_name,name_str_len);
					xml_get_attribute_text(obj_tag,"animation_name",scenery->animation_name,name_str_len);
					xml_get_attribute_3_coord_float(obj_tag,"angle",&scenery->ang.x,&scenery->ang.y,&scenery->ang.z);
					scenery->lighting_mode=xml_get_attribute_list(obj_tag,"lighting_mode",(char*)lighting_mode_str);
					scenery->contact_object_on=xml_get_attribute_boolean(obj_tag,"contact");
					scenery->contact_projectile_on=xml_get_attribute_boolean(obj_tag,"contact_projectile");
					scenery->contact_hit_box=xml_get_attribute_boolean(obj_tag,"contact_hit_box");
					scenery->face_forward=xml_get_attribute_boolean(obj_tag,"face_forward");
					scenery->shadow=xml_get_attribute_boolean(obj_tag,"shadow");
					scenery->shadow_cast_down=xml_get_attribute_boolean(obj_tag,"shadow_cast_down");
					
					scenery->override_size=xml_get_attribute_boolean(obj_tag,"override_size");
					xml_get_attribute_3_coord_int(obj_tag,"size",&scenery->size.x,&scenery->size.y,&scenery->size.z);

					scenery->pnt.x=(scenery->pnt.x*map_enlarge)+(portal->x*map_enlarge);
					scenery->pnt.y=(scenery->pnt.y+1)*map_enlarge;
					scenery->pnt.z=(scenery->pnt.z*map_enlarge)+(portal->z*map_enlarge);
					
					obj_tag=xml_findnextchild(obj_tag);
				}
			}

				// lights
			
			main_light_tag=xml_findfirstchild("Lights",portal_tag);
			if (main_light_tag!=-1) {
			
				cnt=xml_countchildren(main_light_tag);
				light_tag=xml_findfirstchild("Light",main_light_tag);
				
				for (k=0;k!=cnt;k++) {
					if (map->nlight>=max_map_light) break;
					
					light=&map->lights[map->nlight];
					map->nlight++;
					
					light->type=xml_get_attribute_list(light_tag,"type",(char*)light_type_str);
					xml_get_attribute_3_coord_int(light_tag,"c3",&light->pnt.x,&light->pnt.y,&light->pnt.z);
					light->intensity=xml_get_attribute_int(light_tag,"intensity");
					xml_get_attribute_color(light_tag,"rgb",&light->col);
					light->on=!xml_get_attribute_boolean(light_tag,"off");
					
					light->pnt.x=(light->pnt.x*map_enlarge)+(portal->x*map_enlarge);
					light->pnt.y=(light->pnt.y+1)*map_enlarge;
					light->pnt.z=(light->pnt.z*map_enlarge)+(portal->z*map_enlarge);

					light->intensity*=map_enlarge;
					
					light->exponent=1.0f;
				
					light_tag=xml_findnextchild(light_tag);
				}
			}
			
				// sounds
				
			main_sound_tag=xml_findfirstchild("Sounds",portal_tag);
			if (main_sound_tag!=-1) {
			
				cnt=xml_countchildren(main_sound_tag);
				sound_tag=xml_findfirstchild("Sound",main_sound_tag);
				
				for (k=0;k!=cnt;k++) {
					if (map->nsound>=max_map_sound) break;
					
					sound=&map->sounds[map->nsound];
					map->nsound++;
					
					xml_get_attribute_text(sound_tag,"name",sound->name,file_str_len);
					xml_get_attribute_3_coord_int(sound_tag,"c3",&sound->pnt.x,&sound->pnt.y,&sound->pnt.z);
					sound->pitch=xml_get_attribute_float(sound_tag,"pitch");
					sound->on=!xml_get_attribute_boolean(sound_tag,"off");
					
					sound->pnt.x=(sound->pnt.x*map_enlarge)+(portal->x*map_enlarge);
					sound->pnt.y=(sound->pnt.y+1)*map_enlarge;
					sound->pnt.z=(sound->pnt.z*map_enlarge)+(portal->z*map_enlarge);
					
					sound_tag=xml_findnextchild(sound_tag);
				}
			}
			
				// particles
				
			main_particle_tag=xml_findfirstchild("Particles",portal_tag);
			if (main_particle_tag!=-1) {
			
				cnt=xml_countchildren(main_particle_tag);
				particle_tag=xml_findfirstchild("Particle",main_particle_tag);
				
				for (k=0;k!=cnt;k++) {
					if (map->nparticle>=max_map_particle) break;
					
					particle=&map->particles[map->nparticle];
					map->nparticle++;
					
					xml_get_attribute_text(particle_tag,"name",particle->name,file_str_len);
					xml_get_attribute_3_coord_int(particle_tag,"c3",&particle->pnt.x,&particle->pnt.y,&particle->pnt.z);
					particle->spawn_tick=xml_get_attribute_int(particle_tag,"spawn_tick");
					particle->slop_tick=xml_get_attribute_int(particle_tag,"slop_tick");
					particle->on=!xml_get_attribute_boolean(particle_tag,"off");
					
					particle->pnt.x=(particle->pnt.x*map_enlarge)+(portal->x*map_enlarge);
					particle->pnt.y=(particle->pnt.y+1)*map_enlarge;
					particle->pnt.z=(particle->pnt.z*map_enlarge)+(portal->z*map_enlarge);
					
					particle_tag=xml_findnextchild(particle_tag);
				}
			}
        
				// nodes
		
			main_node_tag=xml_findfirstchild("Nodes",portal_tag);
			if (main_node_tag!=-1) {
			
				cnt=xml_countchildren(main_node_tag);
				node_tag=xml_findfirstchild("Node",main_node_tag);
				
				for (k=0;k!=cnt;k++) {
				
					idx=xml_get_attribute_int(node_tag,"id");
					node=&map->nodes[idx];
					if (idx>=map->nnode) map->nnode=idx+1;

					node->idx=idx;

					for (j=0;j!=max_node_link;j++) {
						node->link[j]=-1;
					}
					for (j=0;j!=max_node;j++) {
						node->path_hint[j]=-1;
					}
					
					xml_get_attribute_3_coord_int(node_tag,"c3",&node->pnt.x,&node->pnt.y,&node->pnt.z);

					xml_get_attribute_3_coord_float(node_tag,"angle",&node->ang.x,&node->ang.y,&node->ang.z);

					xml_get_attribute_text(node_tag,"name",node->name,name_str_len);
					node->event_id=xml_get_attribute_int_default(node_tag,"event_id",0);
					
					tag=xml_findfirstchild("Link",node_tag);
					xml_get_attribute_short_array(tag,"node",node->link,max_node_link);
					
					tag=xml_findfirstchild("Hint",node_tag);
					xml_get_attribute_short_array(tag,"node",node->path_hint,map->nnode);

					node->pnt.x=(node->pnt.x*map_enlarge)+(portal->x*map_enlarge);
					node->pnt.y=(node->pnt.y+1)*map_enlarge;
					node->pnt.z=(node->pnt.z*map_enlarge)+(portal->z*map_enlarge);
					
					node_tag=xml_findnextchild(node_tag);
				}
			}

				// spots
		
			main_obj_tag=xml_findfirstchild("Spots",portal_tag);
			if (main_obj_tag!=-1) {
			
				cnt=xml_countchildren(main_obj_tag);
				obj_tag=xml_findfirstchild("Spot",main_obj_tag);
				
				for (k=0;k!=cnt;k++) {
				
					idx=xml_get_attribute_int(obj_tag,"id");
					spot=&map->spots[idx];
					if (idx>=map->nspot) map->nspot=idx+1;
					
					xml_get_attribute_3_coord_int(obj_tag,"c3",&spot->pnt.x,&spot->pnt.y,&spot->pnt.z);
					
					xml_get_attribute_text(obj_tag,"name",spot->name,name_str_len);
					xml_get_attribute_text(obj_tag,"type",spot->type,name_str_len);
					if (!xml_get_attribute_text(obj_tag,"script",spot->script,name_str_len)) {		// supergumba -- remove later -- here to fix XML from older version
						strcpy(spot->script,spot->name);
					}
					xml_get_attribute_text(obj_tag,"display_model",spot->display_model,name_str_len);
					xml_get_attribute_text(obj_tag,"params",spot->params,param_str_len);
					spot->ang.y=xml_get_attribute_float(obj_tag,"angle");
					
					spot->skill=skill_easy;
					spot->spawn=spawn_always;

					spot->pnt.x=(spot->pnt.x*map_enlarge)+(portal->x*map_enlarge);
					spot->pnt.y=(spot->pnt.y+1)*map_enlarge;
					spot->pnt.z=(spot->pnt.z*map_enlarge)+(portal->z*map_enlarge);
					
					obj_tag=xml_findnextchild(obj_tag);
				}
			}
             
            portal++;
			portal_tag=xml_findnextchild(portal_tag);
        }
    }

	convert_ok=map_convert_v1(map,nportal,portals,seg_cnt,seg_list);
	
	free(portals);
	free(seg_list);
	
		// center map
		
	map_center(map);
	
	return(convert_ok);
}
