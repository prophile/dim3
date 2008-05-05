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

bool decal_segment_ok(map_mesh_poly_type *mesh_poly,int mark_idx)
{
	if ((mesh_poly->alpha==1.0f) && (map.textures[mesh_poly->txt_idx].bitmaps[0].alpha_mode!=alpha_mode_transparent)) {
		return(!server.marks[mark_idx].no_opaque);
	}

	return(!server.marks[mark_idx].no_transparent);
}

/* =======================================================

      Move Decals Attached To Meshes
      
======================================================= */

void decal_move_for_mesh(int portal_idx,int mesh_idx,int xmove,int ymove,int zmove)
{
	int			n,i;
	decal_type	*decal;

	decal=server.decals;

	for (n=0;n!=server.count.decal;n++) {
		if ((decal->rn==portal_idx) && (decal->mesh_idx==mesh_idx)) {
			for (i=0;i!=4;i++) {
				decal->x[i]+=xmove;
				decal->z[i]+=zmove;
				decal->y[i]+=ymove;
			}
		}
		decal++;
	}
}

/* =======================================================

      Add Decal to Wall
      
======================================================= */

/* supergumba -- delete
void decal_add_wall(int x,int y,int z,float ang,int mark_idx,int seg_idx,int sz,float alpha)
{
	int					idx,kx,kz,lx,rx,lz,rz;
	segment_type		*seg;
	decal_type			*decal;
    
    if (!setup.mark) return;
	if ((mark_idx==-1) || (seg_idx==-1)) return;
	if (server.count.decal>=max_decal) return;

	if (!decal_segment_ok(seg_idx,mark_idx)) return;
	
        // find segment contact point
        
	seg=&map.segments[seg_idx];
	lx=seg->data.wall.lx;
	lz=seg->data.wall.lz;
	rx=seg->data.wall.rx;
	rz=seg->data.wall.rz;
	
	line_2D_find_angle_hit(x,z,ang,lx,lz,rx,rz,&kx,&kz);
	
        // find a decal spot
        
    decal=&server.decals[server.count.decal];
	server.count.decal++;
	
	decal->start_tick=game_time_get();

		// decal rotation

	idx=0;
	if (!server.marks[mark_idx].no_rotate) idx=random_int(4);

        // setup decal
        
    line_2D_find_inside_infinite(kx,kz,sz,lx,lz,rx,rz,&lx,&lz,&rx,&rz);
        
	decal->x[idx]=decal->x[((idx+3)&0x3)]=lx;
	decal->x[((idx+1)&0x3)]=decal->x[((idx+2)&0x3)]=rx;
	decal->z[idx]=decal->z[((idx+3)&0x3)]=lz;
	decal->z[((idx+1)&0x3)]=decal->z[((idx+2)&0x3)]=rz;

	decal->y[idx]=y-sz;
	decal->y[((idx+1)&0x3)]=y-sz;
	decal->y[((idx+2)&0x3)]=y+sz;
	decal->y[((idx+3)&0x3)]=y+sz;
    
    decal->seg_idx=seg_idx;
	decal->rn=seg->rn;
    
    decal->mark_idx=mark_idx;
    decal->alpha=alpha;
	
		// change portal/segment decal count
		
	map.segments[seg_idx].decal_count++;
	map.portals[seg->rn].decal_count++;
}
*/

/* =======================================================

      Add Decal to Poly
      
======================================================= */

void decal_add_poly(d3pos *pos,poly_pointer_type *poly_ptr,int mark_idx,int sz,float alpha)
{
	int					i,rn;
	decal_type			*decal;
	map_mesh_poly_type	*mesh_poly;
    
	if (server.count.decal>=max_decal) return;

		// can decal this poly?

	rn=poly_ptr->portal_idx;
	mesh_poly=&map.portals[rn].mesh.meshes[poly_ptr->mesh_idx].polys[poly_ptr->poly_idx];
	
	if (!decal_segment_ok(mesh_poly,mark_idx)) return;
    
        // find a decal spot
        
    decal=&server.decals[server.count.decal];
	server.count.decal++;
	
	decal->start_tick=game_time_get();
	
        // wall-like decals

	if (mesh_poly->box.wall_like) {

		decal->x[0]=decal->x[3]=pos->x-sz;
		decal->x[1]=decal->x[2]=pos->x+sz;
		decal->z[0]=decal->z[3]=pos->z-sz;
		decal->z[1]=decal->z[2]=pos->z+sz;
		decal->z[0]=decal->z[1]=pos->y-sz;
		decal->z[2]=decal->z[3]=pos->y+sz;
	}

		// floor-like decals

	else {
        
		decal->x[0]=decal->x[3]=decal->z[0]=decal->z[1]=-sz;
		decal->x[1]=decal->x[2]=decal->z[2]=decal->z[3]=sz;
		decal->y[0]=decal->y[1]=decal->y[2]=decal->y[3]=pos->y;

		if (!server.marks[mark_idx].no_rotate) rotate_polygon_center(4,decal->x,decal->y,decal->z,0,random_float(359),0);
		
		for (i=0;i!=4;i++) {
			decal->x[i]=decal->x[i]+pos->x;
			decal->z[i]=decal->z[i]+pos->z;
			decal->y[i]=pos->y;

			/* supergumba -- work on this!
			if (mesh_poly->box.flat) {
				decal->y[i]=mesh_poly->y[0];
			}
			else {
				decal->y[i]=polygon_find_y(fc->ptsz,fc->x,fc->y,fc->z,decal->x[i],decal->z[i]);
				if (decal->y[i]==-1) decal->y[i]=fc->y[0];
			}
			*/
		}
	}

		// finish decal setup

	decal->rn=rn;
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

