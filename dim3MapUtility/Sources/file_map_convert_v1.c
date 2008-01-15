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
	mesh_poly->flag.pass_through=seg->pass_through;
	mesh_poly->flag.moveable=seg->moveable;
	mesh_poly->flag.climbable=seg->climbable;
	mesh_poly->flag.touched=FALSE;

	mesh_poly->ptsz=ptsz;
	mesh_poly->txt_idx=seg->fill;

	map_mesh->npoly++;
}

void map_convert_segment_to_mesh_orient_uv(int ptsz,float *gx,float *gy,int txt_ang)
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

/* =======================================================

      Convert Wall Segments
      
======================================================= */

int map_convert_segment_to_mesh_wall_to_polygon(segment_type *seg,int *px,int *py,int *pz,float *gx,float *gy)
{
	int					ptsz,my;
    wall_segment_data	*wall;

	wall=&seg->data.wall;

		// any segment that was created by tesselating
		// a clip or a curve will have the simple tessel
		// flag set, just grab the tesseled data

	if (seg->simple_tessel) {
		ptsz=wall->ptsz;

		memmove(px,wall->x,(sizeof(int)*ptsz));
		memmove(py,wall->y,(sizeof(int)*ptsz));
		memmove(pz,wall->z,(sizeof(int)*ptsz));
		memmove(gx,seg->draw.gx,(sizeof(float)*ptsz));
		memmove(gy,seg->draw.gy,(sizeof(float)*ptsz));
		map_convert_segment_to_mesh_orient_uv(ptsz,gx,gy,seg->txt_ang);
		return(ptsz);
	}

		// otherwise convert segment to polygon
	
	ptsz=0;
	
	switch (seg->clip) {
	
		case wc_bottom:
			ptsz=3;
			px[0]=wall->lx;
			px[1]=px[2]=wall->rx;
			pz[0]=wall->lz;
			pz[1]=pz[2]=wall->rz;
			py[0]=py[1]=wall->ty;
			py[2]=wall->by+1;
			gx[0]=seg->x_txtoff;
			gx[1]=gx[2]=seg->x_txtoff+seg->x_txtfact;
			gy[0]=gy[1]=seg->y_txtoff;
			gy[2]=seg->y_txtoff+seg->y_txtfact;
			break;
			
		case wc_top:
			ptsz=3;
			px[0]=px[2]=wall->lx;
			px[1]=wall->rx;
			pz[0]=pz[2]=wall->lz;
			pz[1]=wall->rz;
			py[0]=wall->ty;
			py[1]=py[2]=wall->by+1;
			gx[0]=gx[2]=seg->x_txtoff;
			gx[1]=seg->x_txtoff+seg->x_txtfact;
			gy[0]=seg->y_txtoff;
			gy[1]=gy[2]=seg->y_txtoff+seg->y_txtfact;
			break;
			
		case wc_slant:
			ptsz=4;
			my=(wall->by+wall->ty)>>1;
			px[0]=px[3]=wall->lx;
			px[1]=px[2]=wall->rx;
			pz[0]=pz[3]=wall->lz;
			pz[1]=pz[2]=wall->rz;
			py[0]=wall->ty;
			py[1]=py[3]=(wall->by+wall->ty)>>1;
			py[2]=wall->by+1;
			gx[0]=gx[3]=seg->x_txtoff;
			gx[1]=gx[2]=seg->x_txtoff+seg->x_txtfact;
			gy[0]=seg->y_txtoff;
			gy[1]=gy[3]=seg->y_txtoff+(seg->y_txtfact/2);
			gy[2]=seg->y_txtoff+seg->y_txtfact;
			break;

        default:
			ptsz=4;
			px[0]=px[3]=wall->lx;
			px[1]=px[2]=wall->rx;
			pz[0]=pz[3]=wall->lz;
			pz[1]=pz[2]=wall->rz;
			py[0]=py[1]=wall->ty;
			py[2]=py[3]=wall->by+1;
            gx[0]=gx[3]=seg->x_txtoff;
            gx[1]=gx[2]=seg->x_txtoff+seg->x_txtfact;
            gy[0]=gy[1]=seg->y_txtoff;
            gy[2]=gy[3]=seg->y_txtoff+seg->y_txtfact;
            break;
	}
	
	map_convert_segment_to_mesh_orient_uv(ptsz,gx,gy,seg->txt_ang);
	
	return(ptsz);
}

/* =======================================================

      Convert Floor/Ceiling Segments
      
======================================================= */

int map_convert_segment_to_mesh_fc_to_polygon(segment_type *seg,int *px,int *py,int *pz,float *gx,float *gy)
{
	int					n,ptsz,lft,rgt,top,bot;
    float				xsize,ysize,x_txtoff,y_txtoff,x_txtfact,y_txtfact;
    fc_segment_data		*fc;
    
	fc=&seg->data.fc;
	
    ptsz=fc->ptsz;

		// any segment that was created by tesselating
		// a clip or a curve will have the simple tessel
		// flag set, just grab the tesseled data

	if (seg->simple_tessel) {
		memmove(px,fc->x,(sizeof(int)*ptsz));
		memmove(py,fc->y,(sizeof(int)*ptsz));
		memmove(pz,fc->z,(sizeof(int)*ptsz));
		memmove(gx,seg->draw.gx,(sizeof(float)*ptsz));
		memmove(gy,seg->draw.gy,(sizeof(float)*ptsz));
		map_convert_segment_to_mesh_orient_uv(ptsz,gx,gy,seg->txt_ang);
		return(ptsz);
	}

		// otherwise convert segment to polygon

		// get extents
		
	lft=rgt=fc->x[0];
	top=bot=fc->z[0];

	for (n=0;n!=ptsz;n++) {
		px[n]=fc->x[n];
		if (px[n]<lft) lft=px[n];
		if (px[n]>rgt) rgt=px[n];
		
		pz[n]=fc->z[n];
		if (pz[n]<top) top=pz[n];
		if (pz[n]>bot) bot=pz[n];
    
		py[n]=fc->y[n];
	}
	
		// get texture coordinates
		
	xsize=(float)(rgt-lft);
	ysize=(float)(bot-top);
 
	x_txtoff=seg->x_txtoff;
	x_txtfact=seg->x_txtfact;
	y_txtoff=seg->y_txtoff;
	y_txtfact=seg->y_txtfact;
   
    for (n=0;n!=ptsz;n++) {
		gx[n]=x_txtoff+((x_txtfact*(float)(px[n]-lft))/xsize);
		gy[n]=y_txtoff+((y_txtfact*(float)(pz[n]-top))/ysize);
    }
	
	map_convert_segment_to_mesh_orient_uv(ptsz,gx,gy,seg->txt_ang);
	
	return(ptsz);
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

      Enlarge Map
      
======================================================= */

void map_convert_enlarge(map_type *map)
{
	int				n,t;
	portal_type		*portal;
	segment_type	*seg;

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

			case sg_liquid:
				seg->data.liquid.lft*=map_enlarge;
				seg->data.liquid.rgt*=map_enlarge;
				seg->data.liquid.top*=map_enlarge;
				seg->data.liquid.bot*=map_enlarge;
				seg->data.liquid.y=(seg->data.liquid.y+1)*map_enlarge;
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
}

/* =======================================================

      Convert Entire Map
      
======================================================= */

bool map_convert_v1(map_type *map)
{
	int					n,i,vlist_sz,nvertex,npoly;
	int					ptsz,px[8],py[8],pz[8];
	float				gx[8],gy[8];
	d3pnt				*vlist;
	portal_type			*portal;
	segment_type		*seg;
	map_mesh_type		*map_mesh;
	node_type			*node;
	map_scenery_type	*scenery;
	map_light_type		*light;
	map_sound_type		*sound;
	map_particle_type	*particle;
	spot_type			*spot;

		// memory for vertex lists
		// just use enough vertexes to cover most maps,
		// supergumba -- need to do this in a more proper way

	vlist_sz=sizeof(d3pnt)*10000;

	vlist=(d3pnt*)valloc(vlist_sz);
	if (vlist==NULL) return(FALSE);

		// enlarge map

	map_convert_enlarge(map);

		// tesselate up any curved or clipped segments into
		// multiple segments before converting to a mesh

	map_convert_tesselate_curves_clips(map);

		// create a mesh out of all segments in a portal

	for (n=0;n!=map->nportal;n++) {
	
		portal=&map->portals[n];
	
		portal->mesh.nmesh=0;

			// clear vertex list

		bzero(vlist,vlist_sz);
		
			// count the vertexes and polys

		nvertex=npoly=0;

		for (i=0;i!=map->nsegment;i++) {
			seg=&map->segments[i];
			if (seg->rn!=n) continue;

			switch (seg->type) {

				case sg_wall:
					ptsz=map_convert_segment_to_mesh_wall_to_polygon(seg,px,py,pz,gx,gy);
					nvertex=map_convert_segment_to_mesh_add_poly_vlist(nvertex,vlist,ptsz,px,py,pz);
					npoly++;
					break;

				case sg_floor:
				case sg_ceiling:
					ptsz=map_convert_segment_to_mesh_fc_to_polygon(seg,px,py,pz,gx,gy);
					nvertex=map_convert_segment_to_mesh_add_poly_vlist(nvertex,vlist,ptsz,px,py,pz);
					npoly++;
					break;

			}

		}

		if ((nvertex==0) || (npoly==0)) continue;

		fprintf(stdout,"%d: nvertex = %d\n",n,nvertex);

			// create new mesh

		if (!map_portal_mesh_add(map,n,1)) {
			free(vlist);
			return(FALSE);
		}

		if (!map_portal_mesh_set_vertex_count(map,n,0,nvertex)) {
			map_portal_mesh_delete(map,n,0);
			free(vlist);
			return(FALSE);
		}

		if (!map_portal_mesh_set_poly_count(map,n,0,npoly)) {
			map_portal_mesh_delete(map,n,0);
			free(vlist);
			return(FALSE);
		}

		map_mesh=&portal->mesh.meshes[0];

			// move over vertexes

		map_mesh->nvertex=nvertex;
		memmove(map_mesh->vertexes,vlist,(sizeof(d3pnt)*nvertex));
			
			// add polys

		map_mesh->npoly=0;

		for (i=0;i!=map->nsegment;i++) {
			seg=&map->segments[i];
			if (seg->rn!=n) continue;

			switch (seg->type) {

				case sg_wall:
					ptsz=map_convert_segment_to_mesh_wall_to_polygon(seg,px,py,pz,gx,gy);
					map_convert_segment_to_mesh_add_mesh_poly(map_mesh,ptsz,px,py,pz,gx,gy,seg);
					break;

				case sg_floor:
				case sg_ceiling:
					ptsz=map_convert_segment_to_mesh_fc_to_polygon(seg,px,py,pz,gx,gy);
					map_convert_segment_to_mesh_add_mesh_poly(map_mesh,ptsz,px,py,pz,gx,gy,seg);
					break;

			}

		}
	}
	
	free(vlist);

		// turn off all segments

	map->nsegment=0;
	
		// enlarge other items
		// supergumba -- maybe do during load?
		
		// map scenery
	
	scenery=map->sceneries;
	
	for ((i=0);(i!=map->nscenery);i++) {
		portal=&map->portals[scenery->pos.rn];
		scenery->pos.x=(scenery->pos.x*map_enlarge);
		scenery->pos.y=(scenery->pos.y+1)*map_enlarge;
		scenery->pos.z=(scenery->pos.z*map_enlarge);
		scenery++;
	}
		
		// map lights
	
	light=map->lights;
	
	for ((i=0);(i!=map->nlight);i++) {
		portal=&map->portals[light->pos.rn];
		light->intensity*=map_enlarge;
		light->pos.x=(light->pos.x*map_enlarge);
		light->pos.y=(light->pos.y+1)*map_enlarge;
		light->pos.z=(light->pos.z*map_enlarge);
		light++;
	}
	
		// map sounds
	
	sound=map->sounds;
	
	for ((i=0);(i!=map->nsound);i++) {
		portal=&map->portals[sound->pos.rn];
		sound->pos.x=(sound->pos.x*map_enlarge);
		sound->pos.y=(sound->pos.y+1)*map_enlarge;
		sound->pos.z=(sound->pos.z*map_enlarge);
		
		sound++;
	}
	
		// map particles
	
	particle=map->particles;
	
	for ((i=0);(i!=map->nparticle);i++) {
		portal=&map->portals[particle->pos.rn];
		particle->pos.x=(particle->pos.x*map_enlarge);
		particle->pos.y=(particle->pos.y+1)*map_enlarge;
		particle->pos.z=(particle->pos.z*map_enlarge);
		
		particle++;
	}
	
		// nodes

	node=map->nodes;
	
	for ((i=0);(i!=map->nnode);i++) {
        node->idx=i;
		portal=&map->portals[node->pos.rn];
		node->pos.x=(node->pos.x*map_enlarge);
		node->pos.y=(node->pos.y+1)*map_enlarge;
		node->pos.z=(node->pos.z*map_enlarge);
		node++;
	}
    
		// object starts
		
	spot=map->spots;

	for ((i=0);(i!=map->nspot);i++) {
		portal=&map->portals[spot->pos.rn];
		spot->pos.x=(spot->pos.x*map_enlarge);
		spot->pos.y=(spot->pos.y+1)*map_enlarge;
		spot->pos.z=(spot->pos.z*map_enlarge);
		spot++;
	}

	return(TRUE);
}



