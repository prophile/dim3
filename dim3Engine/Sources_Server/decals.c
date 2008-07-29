/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Decals

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
	#include "dim3engine.h"
#endif

extern map_type			map;
extern server_type		server;
extern setup_type		setup;

extern int game_time_get(void);

/* =======================================================

      Initialize Marks
      
======================================================= */

void mark_initialize(void)
{
	server.count.mark=0;
}

/* =======================================================

      Find Marks
      
======================================================= */

int mark_find(char *name)
{
	int			n;
	mark_type	*mark;

	mark=server.marks;
	
	for (n=0;n!=server.count.mark;n++) {
		if (strcasecmp(mark->name,name)==0)  return(n);
		mark++;
	}
	
	return(-1);
}

/* =======================================================

      Setup Decals
      
======================================================= */

void decal_clear(void)
{
	server.count.decal=0;
}

/* =======================================================

      Get Free Decal
      
======================================================= */

decal_type* decal_find_free(void)
{
	if (server.count.decal>=max_decal) return(NULL);
	
    server.count.decal++;
    return(&server.decals[server.count.decal-1]);
}

/* =======================================================

      Check if Segment is OK for Decal
      
======================================================= */

bool decal_segment_ok(map_mesh_poly_type *poly,int mark_idx)
{
	if ((poly->alpha==1.0f) && (map.textures[poly->txt_idx].bitmaps[0].alpha_mode!=alpha_mode_transparent)) {
		return(!server.marks[mark_idx].no_opaque);
	}

	return(!server.marks[mark_idx].no_transparent);
}

/* =======================================================

      Move Decals Attached To Meshes
      
======================================================= */

void decal_move_with_mesh(int mesh_idx,int xmove,int ymove,int zmove)
{
	int			n,i;
	decal_type	*decal;

	decal=server.decals;

	for (n=0;n!=server.count.decal;n++) {
		if (decal->mesh_idx==mesh_idx) {
			for (i=0;i!=4;i++) {
				decal->x[i]+=xmove;
				decal->y[i]+=ymove;
				decal->z[i]+=zmove;
			}
		}
		decal++;
	}
}

void decal_rotate_with_mesh(int mesh_idx,float y)
{
	int				n;
	d3pnt			mpt;
	decal_type		*decal;
	map_mesh_type	*mesh;

		// get center point for mesh

	mesh=&map.mesh.meshes[mesh_idx];

	map_portal_mesh_calculate_center(&map,mesh_idx,&mpt);
	mpt.x+=mesh->rot_off.x;
	mpt.y+=mesh->rot_off.y;
	mpt.z+=mesh->rot_off.z;

		// rotate any decals on it

	decal=server.decals;

	for (n=0;n!=server.count.decal;n++) {
		if (decal->mesh_idx==mesh_idx) {
			rotate_2D_polygon(4,decal->x,decal->z,mpt.x,mpt.z,y);
		}
		decal++;
	}
}

/* =======================================================

      Specific Poly Types
      
======================================================= */

void decal_add_wall_like(d3pos *pos,decal_type *decal,map_mesh_poly_type *poly,int mark_idx,int sz)
{
	int				idx,lx,rx,lz,rz;
    
		// decal rotation

	idx=0;
	if (!server.marks[mark_idx].no_rotate) idx=random_int(4);

        // setup decal
        
    line_2D_find_inside_infinite(pos->x,pos->z,sz,poly->line.lx,poly->line.lz,poly->line.rx,poly->line.rz,&lx,&lz,&rx,&rz);
        
	decal->x[idx]=decal->x[((idx+3)&0x3)]=lx;
	decal->x[((idx+1)&0x3)]=decal->x[((idx+2)&0x3)]=rx;
	decal->z[idx]=decal->z[((idx+3)&0x3)]=lz;
	decal->z[((idx+1)&0x3)]=decal->z[((idx+2)&0x3)]=rz;

	decal->y[idx]=pos->y-sz;
	decal->y[((idx+1)&0x3)]=pos->y-sz;
	decal->y[((idx+2)&0x3)]=pos->y+sz;
	decal->y[((idx+3)&0x3)]=pos->y+sz;
}

void decal_add_floor_like(d3pos *pos,decal_type *decal,map_mesh_type *mesh,map_mesh_poly_type *poly,int mark_idx,int sz)
{
	int				n,ptsz,px[8],py[8],pz[8];
	d3pnt			*pt;

	decal->x[0]=decal->x[3]=decal->z[0]=decal->z[1]=-sz;
	decal->x[1]=decal->x[2]=decal->z[2]=decal->z[3]=sz;
	decal->y[0]=decal->y[1]=decal->y[2]=decal->y[3]=0;

	if (!server.marks[mark_idx].no_rotate) rotate_polygon_center(4,decal->x,decal->y,decal->z,0,random_float(359),0);
	
		// if poly not flat, need array of
		// coordinates to calculate y position

	if (!poly->box.flat) {

		ptsz=poly->ptsz;

		for (n=0;n!=ptsz;n++) {
			pt=&mesh->vertexes[poly->v[n]];
			px[n]=pt->x;
			py[n]=pt->y;
			pz[n]=pt->z;
		}

	}

	for (n=0;n!=4;n++) {
		decal->x[n]=decal->x[n]+pos->x;
		decal->z[n]=decal->z[n]+pos->z;

		if (poly->box.flat) {
			decal->y[n]=poly->box.mid.y;
		}
		else {
			decal->y[n]=polygon_find_y(ptsz,px,py,pz,decal->x[n],decal->z[n]);
			if (decal->y[n]==-1) decal->y[n]=poly->box.mid.y;
		}
	}
}

/* =======================================================

      Add Decal to Poly
      
======================================================= */

void decal_add(d3pos *pos,poly_pointer_type *poly_ptr,int mark_idx,int sz,float alpha)
{
	decal_type			*decal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
    
		// any more decals?

	if (server.count.decal>=max_decal) return;

		// can decal this poly?

	mesh=&map.mesh.meshes[poly_ptr->mesh_idx];
	poly=&mesh->polys[poly_ptr->poly_idx];
	
	if (!decal_segment_ok(poly,mark_idx)) return;
    
        // find a decal spot
        
    decal=&server.decals[server.count.decal];
	server.count.decal++;
	
	decal->start_tick=game_time_get();
	
        // wall-like decals

	if (poly->box.wall_like) {
		decal_add_wall_like(pos,decal,poly,mark_idx,sz);
	}

		// floor-like decals

	else {
		decal_add_floor_like(pos,decal,mesh,poly,mark_idx,sz);
	}

		// finish decal setup

	decal->mesh_idx=poly_ptr->mesh_idx;
	decal->poly_idx=poly_ptr->poly_idx;
    
    decal->mark_idx=mark_idx;
    decal->alpha=alpha;
}

/* =======================================================

      Dispose Decals
      
======================================================= */

void decal_dispose(void)
{
	int					i,tick;
	decal_type			*decal;
	mark_type			*mark;
	
	tick=game_time_get();
	
		// delete all decals that have timed-out
	
	i=0;
	
	while (i<server.count.decal) {
		decal=&server.decals[i];
		
		mark=&server.marks[decal->mark_idx];
		
		if ((tick-decal->start_tick)<mark->total_msec) {
			i++;
			continue;
		}

			// remove decal
			
		if (i<(server.count.decal-1)) {
			memmove(&server.decals[i],&server.decals[i+1],(sizeof(decal_type)*((server.count.decal-i)-1)));
		}
			
		server.count.decal--;
		if (server.count.decal==0) break;
	}
}

