/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Portal Transformation Routines

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
#include "dialog.h"
#include "common_view.h"
#include "portal_view.h"

extern int					cr;
extern bool					dp_auto_texture;

extern map_type				map;

/* =======================================================

      Get Sizes of portals
      
======================================================= */

void portal_get_xz_size(int rn,int *pex,int *pez)
{
	int					n,k,ex,ez;
	d3pnt				*pt;
	portal_type			*portal;
	map_mesh_type		*mesh;
	
	ex=ez=0;
	
	portal=&map.portals[rn];
		
	mesh=portal->mesh.meshes;
		
	for (n=0;n!=portal->mesh.nmesh;n++) {

		pt=mesh->vertexes;
		
		for (k=0;k!=mesh->nvertex;k++) {
			if (pt->x>ex) ex=pt->x;
			if (pt->z>ez) ez=pt->z;
			
			pt++;
		}
	
		mesh++;
	}

	if (ex==0) ex=10*map_enlarge;
	if (ez==0) ez=10*map_enlarge;
	
	*pex=ex;
	*pez=ez;
}

void portal_get_y_size(int rn,int *pty,int *pby)
{
	int					n,k,ty,by;
	d3pnt				*pt;
	portal_type			*portal;
	map_mesh_type		*mesh;

	ty=map_max_size;
	by=0;
	
	portal=&map.portals[rn];
		
	mesh=portal->mesh.meshes;
		
	for (n=0;n!=portal->mesh.nmesh;n++) {

		pt=mesh->vertexes;
		
		for (k=0;k!=mesh->nvertex;k++) {
			if (pt->y<ty) ty=pt->y;
			if (pt->y>by) by=pt->y;
			
			pt++;
		}
	
		mesh++;
	}
    
    if ((ty==map_max_size) || (by==0)) ty=by=map_max_size/2;

	*pty=ty;
	*pby=by;
}

/* =======================================================

      Room Boxes
      
======================================================= */

void portal_get_dimension(int rn,int *lx,int *rx,int *tz,int *bz)
{
	*lx=map.portals[rn].x;
	*rx=map.portals[rn].ex;
	*tz=map.portals[rn].z;
	*bz=map.portals[rn].ez;
}

void portal_set_dimension(int rn,int x,int ex,int z,int ez)
{
	map.portals[rn].x=x;
	map.portals[rn].ex=ex;
	map.portals[rn].z=z;
	map.portals[rn].ez=ez;
}

void portal_set_spot(int rn,int x,int z)
{
	int			sx,sz;
	
	sx=map.portals[rn].ex-map.portals[rn].x;
	sz=map.portals[rn].ez-map.portals[rn].z;		// perserve room size
	map.portals[rn].x=x;
	map.portals[rn].ex=x+sx;
	map.portals[rn].z=z;
	map.portals[rn].ez=z+sz;
}

/* =======================================================

      Portal Transforms
      
======================================================= */

void portal_flip_horizontal(void)
{
	int				i,ex;
	
	ex=map.portals[cr].ex-map.portals[cr].x;
	
	// flip meshes and liquids
    
    for (i=0;i!=map.nspot;i++) {
        if (map.spots[i].pos.rn==cr) {
            map.spots[i].pos.x=ex-map.spots[i].pos.x;
			map.spots[i].ang.y=angle_add(0,-map.spots[i].ang.y);
        }
    }
    
    for (i=0;i!=map.nnode;i++) {
        if (map.nodes[i].pos.rn==cr) {
            map.nodes[i].pos.x=ex-map.nodes[i].pos.x;
        }
    }
	
    for (i=0;i!=map.nsound;i++) {
        if (map.sounds[i].pos.rn==cr) {
            map.sounds[i].pos.x=ex-map.sounds[i].pos.x;
        }
    }
	
    for (i=0;i!=map.nlight;i++) {
        if (map.lights[i].pos.rn==cr) {
            map.lights[i].pos.x=ex-map.lights[i].pos.x;
        }
    }
	
    for (i=0;i!=map.nparticle;i++) {
        if (map.particles[i].pos.rn==cr) {
            map.particles[i].pos.x=ex-map.particles[i].pos.x;
        }
    }
	
    for (i=0;i!=map.nscenery;i++) {
        if (map.sceneries[i].pos.rn==cr) {
            map.sceneries[i].pos.x=ex-map.sceneries[i].pos.x;
			map.sceneries[i].ang.y=angle_add(0,-map.sceneries[i].ang.y);
        }
    }
	
	main_wind_draw();
}
	
void portal_flip_vertical(void)
{
	int				i,ey;
	
	ey=map.portals[cr].ez-map.portals[cr].z;
	
	// flip meshes and liquids
	    
    for (i=0;i!=map.nspot;i++) {
        if (map.spots[i].pos.rn==cr) {
            map.spots[i].pos.z=ey-map.spots[i].pos.z;
			map.spots[i].ang.y=angle_add(90.0f,(90.0f-map.spots[i].ang.y));
        }
    }
    
    for (i=0;i!=map.nnode;i++) {
        if (map.nodes[i].pos.rn==cr) {
            map.nodes[i].pos.z=ey-map.nodes[i].pos.z;
        }
    }
	
    for (i=0;i!=map.nsound;i++) {
        if (map.sounds[i].pos.rn==cr) {
            map.sounds[i].pos.z=ey-map.sounds[i].pos.z;
        }
    }
	
    for (i=0;i!=map.nlight;i++) {
        if (map.lights[i].pos.rn==cr) {
            map.lights[i].pos.z=ey-map.lights[i].pos.z;
        }
    }
	
    for (i=0;i!=map.nparticle;i++) {
        if (map.particles[i].pos.rn==cr) {
            map.particles[i].pos.z=ey-map.particles[i].pos.z;
        }
    }
	
    for (i=0;i!=map.nscenery;i++) {
        if (map.sceneries[i].pos.rn==cr) {
            map.sceneries[i].pos.z=ey-map.sceneries[i].pos.z;
			map.sceneries[i].ang.y=angle_add(90.0f,(90.0f-map.sceneries[i].ang.y));
        }
    }
	
	main_wind_draw();
}

void portal_rotate(void)
{
	int				i,k,ex,ey;
	
	// supergumba -- rotate meshes and liquids
    
    for (i=0;i!=map.nspot;i++) {
        if (map.spots[i].pos.rn==cr) {
            k=map.spots[i].pos.x;
            map.spots[i].pos.x=map.spots[i].pos.z;
            map.spots[i].pos.z=k;
			map.spots[i].ang.y=angle_add(map.spots[i].ang.y,-90.0f);
			map.spots[i].ang.y=angle_add(90.0f,(90.0f-map.spots[i].ang.y));
        }
    }
    
    for (i=0;i!=map.nnode;i++) {
        if (map.nodes[i].pos.rn==cr) {
            k=map.nodes[i].pos.x;
            map.nodes[i].pos.x=map.nodes[i].pos.z;
            map.nodes[i].pos.z=k;
        }
    }
	
    for (i=0;i!=map.nsound;i++) {
        if (map.sounds[i].pos.rn==cr) {
            k=map.sounds[i].pos.x;
            map.sounds[i].pos.x=map.sounds[i].pos.z;
            map.sounds[i].pos.z=k;
        }
    }
	
    for (i=0;i!=map.nlight;i++) {
        if (map.lights[i].pos.rn==cr) {
            k=map.lights[i].pos.x;
            map.lights[i].pos.x=map.lights[i].pos.z;
            map.lights[i].pos.z=k;
        }
    }
	
    for (i=0;i!=map.nparticle;i++) {
        if (map.particles[i].pos.rn==cr) {
            k=map.particles[i].pos.x;
            map.particles[i].pos.x=map.particles[i].pos.z;
            map.particles[i].pos.z=k;
        }
    }
	
    for (i=0;i!=map.nscenery;i++) {
        if (map.sceneries[i].pos.rn==cr) {
            k=map.sceneries[i].pos.x;
            map.sceneries[i].pos.x=map.sceneries[i].pos.z;
            map.sceneries[i].pos.z=k;
			map.sceneries[i].ang.y=angle_add(map.sceneries[i].ang.y,-90.0f);
			map.sceneries[i].ang.y=angle_add(90.0f,(90.0f-map.sceneries[i].ang.y));
        }
    }
	
	ex=map.portals[cr].ex-map.portals[cr].x;
	ey=map.portals[cr].ez-map.portals[cr].z;
	
	map.portals[cr].ex=map.portals[cr].x+ey;
	map.portals[cr].ez=map.portals[cr].z+ex;
	
	main_wind_draw();
}

/* =======================================================

      Portal Changes
      
======================================================= */

void portal_y_change(int yadd)
{
	int				n,k;
	d3pnt			*pt;
	portal_type		*portal;
	map_mesh_type	*mesh;
	map_liquid_type	*liq;
	
	portal=&map.portals[cr];
	
		// run through meshes
		
	mesh=portal->mesh.meshes;
		
	for (n=0;n!=portal->mesh.nmesh;n++) {

		pt=mesh->vertexes;
		
		for (k=0;k!=mesh->nvertex;k++) {
			pt->y+=yadd;
			pt++;
		}
	
		mesh++;
	}
    
		// run through liquids
		
	liq=portal->liquid.liquids;
	
	for (n=0;n!=portal->liquid.nliquid;n++) {
		liq->y+=yadd;
		liq++;
	}
		
		// run through all other items
		
    for (n=0;n!=map.nspot;n++) {
        if (map.spots[n].pos.rn==cr) {
            map.spots[n].pos.y+=yadd;
        }
    }
    
    for (n=0;n!=map.nnode;n++) {
        if (map.nodes[n].pos.rn==cr) {
            map.nodes[n].pos.y+=yadd;
        }
    }
	
    for (n=0;n!=map.nsound;n++) {
        if (map.sounds[n].pos.rn==cr) {
            map.sounds[n].pos.y+=yadd;
        }
    }
	
    for (n=0;n!=map.nlight;n++) {
        if (map.lights[n].pos.rn==cr) {
            map.lights[n].pos.y+=yadd;
        }
    }
	
    for (n=0;n!=map.nparticle;n++) {
        if (map.particles[n].pos.rn==cr) {
            map.particles[n].pos.y+=yadd;
        }
    }
	
    for (n=0;n!=map.nscenery;n++) {
        if (map.sceneries[n].pos.rn==cr) {
            map.sceneries[n].pos.y+=yadd;
        }
    }
}

void portal_all_y_change(int yadd)
{
	int				n,old_rn;
	
	old_rn=cr;
	
	for (n=0;n!=map.nportal;n++) {
		cr=n;
		portal_y_change(yadd);
	}
	
	cr=old_rn;
}

/* =======================================================

      Resize Portal
      
======================================================= */

void portal_resize(void)
{
    int				i,t,ptsz,fct,sx,sz;
    segment_type	*seg;
    
    fct=dialog_portal_resize_run();
    if ((fct==-1) || (fct==100)) return;
    
        // portal size
        
	sx=((map.portals[cr].ex-map.portals[cr].x)*fct)/100;
	map.portals[cr].ex=map.portals[cr].x+sx;
	sz=((map.portals[cr].ez-map.portals[cr].z)*fct)/100;
	map.portals[cr].ez=map.portals[cr].z+sz;
	
        // segments
        
	for (i=0;i!=map.nsegment;i++) {
        seg=&map.segments[i];
        if (seg->rn!=cr) continue;
        
        switch (seg->type) {
        
            case sg_wall:
                seg->data.wall.lx=(seg->data.wall.lx*fct)/100;
                seg->data.wall.rx=(seg->data.wall.rx*fct)/100;
                seg->data.wall.lz=(seg->data.wall.lz*fct)/100;
                seg->data.wall.rz=(seg->data.wall.rz*fct)/100;
                break;
                
            case sg_floor:
            case sg_ceiling:
                ptsz=seg->data.fc.ptsz;
                for ((t=0);(t!=ptsz);t++) {
                    seg->data.fc.x[t]=(seg->data.fc.x[t]*fct)/100;
                    seg->data.fc.z[t]=(seg->data.fc.z[t]*fct)/100;
                }
                break;
                
            case sg_liquid:
                seg->data.liquid.lft=(seg->data.liquid.lft*fct)/100;
                seg->data.liquid.rgt=(seg->data.liquid.rgt*fct)/100;
                seg->data.liquid.top=(seg->data.liquid.top*fct)/100;
                seg->data.liquid.bot=(seg->data.liquid.bot*fct)/100;
                break;
        }
	}
	
        // objects, nodes, and cameras
        
	for (i=0;i<map.nspot;i++) {
		if (map.spots[i].pos.rn==cr) {
			map.spots[i].pos.x=(map.spots[i].pos.x*fct)/100;
			map.spots[i].pos.z=(map.spots[i].pos.z*fct)/100;
		}
	}
	for (i=0;i<map.nnode;i++) {
		if (map.nodes[i].pos.rn==cr) {
			map.nodes[i].pos.x=(map.nodes[i].pos.x*fct)/100;
			map.nodes[i].pos.z=(map.nodes[i].pos.z*fct)/100;
		}
	}

	main_wind_draw();
}

/* =======================================================

      Portal Split
      
======================================================= */

void portal_split_segment_to_global(segment_type *seg)
{
	int						k;
		
	switch (seg->type) {
	
		case sg_wall:
			seg->data.wall.lx+=map.portals[seg->rn].x;
			seg->data.wall.rx+=map.portals[seg->rn].x;
			seg->data.wall.lz+=map.portals[seg->rn].z;
			seg->data.wall.rz+=map.portals[seg->rn].z;
			break;
			
		case sg_floor:
		case sg_ceiling:
			for (k=0;k!=seg->data.fc.ptsz;k++) {
				seg->data.fc.x[k]+=map.portals[seg->rn].x;
				seg->data.fc.z[k]+=map.portals[seg->rn].z;
			}
			break;
			
	}
}

void portal_split_segment_to_local(segment_type *seg)
{
	int						k;
		
	switch (seg->type) {
	
		case sg_wall:
			seg->data.wall.lx-=map.portals[seg->rn].x;
			seg->data.wall.rx-=map.portals[seg->rn].x;
			seg->data.wall.lz-=map.portals[seg->rn].z;
			seg->data.wall.rz-=map.portals[seg->rn].z;
			break;
			
		case sg_floor:
		case sg_ceiling:
			for (k=0;k!=seg->data.fc.ptsz;k++) {
				seg->data.fc.x[k]-=map.portals[seg->rn].x;
				seg->data.fc.z[k]-=map.portals[seg->rn].z;
			}
			break;
			
	}
}

void portal_split_segments_wall(int seg_idx,bool is_local)
{
	int						lx1,rx1,lz1,rz1,lx2,rx2,lz2,rz2,y,
							lrn,rrn;
	segment_type			*seg,*seg2;
	portal_type				*lportal,*rportal;
	
	seg=&map.segments[seg_idx];
	
	if (is_local) portal_split_segment_to_global(seg);
	
		// find portals for all vertexes
		
	lrn=map_find_portal(&map,seg->data.wall.lx,seg->data.wall.by,seg->data.wall.lz);
	rrn=map_find_portal(&map,seg->data.wall.rx,seg->data.wall.by,seg->data.wall.rz);
	
		// single portal?
		
	if (lrn==rrn) {
		seg->rn=lrn;
		if (is_local) portal_split_segment_to_local(seg);
		return;
	}
	
		// split the line
	
	lportal=&map.portals[lrn];
	rportal=&map.portals[rrn];
	
	lx1=-1;
	rx1=lz1=rz1=lx2=rx2=lz2=rz2=0;
	
		// clipped to right
		
	if (lportal->ex==rportal->x) {
		lx1=seg->data.wall.lx;
		lz1=seg->data.wall.lz;
		y=0;
		clip_extend_line_to_2D_vertical_line(lportal->ex,&rx1,&y,&rz1,seg->data.wall.lx,0,seg->data.wall.lz,seg->data.wall.rx,0,seg->data.wall.rz);

		lx2=seg->data.wall.rx;
		lz2=seg->data.wall.rz;
		y=0;
		clip_extend_line_to_2D_vertical_line(lportal->ex,&rx2,&y,&rz2,seg->data.wall.rx,0,seg->data.wall.rz,seg->data.wall.lx,0,seg->data.wall.lz);
	}
	
		// clipped to left
		
	if (lportal->x==rportal->ex) {
		lx1=seg->data.wall.lx;
		lz1=seg->data.wall.lz;
		y=0;
		clip_extend_line_to_2D_vertical_line(lportal->x,&rx1,&y,&rz1,seg->data.wall.lx,0,seg->data.wall.lz,seg->data.wall.rx,0,seg->data.wall.rz);

		lx2=seg->data.wall.rx;
		lz2=seg->data.wall.rz;
		y=0;
		clip_extend_line_to_2D_vertical_line(lportal->x,&rx2,&y,&rz2,seg->data.wall.rx,0,seg->data.wall.rz,seg->data.wall.lx,0,seg->data.wall.lz);
	}
	
		// clipped to bottom
		
	if (lportal->ez==rportal->z) {
		lx1=seg->data.wall.lx;
		lz1=seg->data.wall.lz;
		y=0;
		clip_extend_line_to_2D_horizontal_line(lportal->ez,&rx1,&y,&rz1,seg->data.wall.lx,0,seg->data.wall.lz,seg->data.wall.rx,0,seg->data.wall.rz);

		lx2=seg->data.wall.rx;
		lz2=seg->data.wall.rz;
		y=0;
		clip_extend_line_to_2D_horizontal_line(lportal->ez,&rx2,&y,&rz2,seg->data.wall.rx,0,seg->data.wall.rz,seg->data.wall.lx,0,seg->data.wall.lz);
	}
	
		// clipped to top
		
	if (lportal->z==rportal->ez) {
		lx1=seg->data.wall.lx;
		lz1=seg->data.wall.lz;
		y=0;
		clip_extend_line_to_2D_horizontal_line(lportal->z,&rx1,&y,&rz1,seg->data.wall.lx,0,seg->data.wall.lz,seg->data.wall.rx,0,seg->data.wall.rz);

		lx2=seg->data.wall.rx;
		lz2=seg->data.wall.rz;
		y=0;
		clip_extend_line_to_2D_horizontal_line(lportal->z,&rx2,&y,&rz2,seg->data.wall.rx,0,seg->data.wall.rz,seg->data.wall.lx,0,seg->data.wall.lz);
	}
	
		// never got clipped, ignore
		
	if (lx1==-1) return;

		// replace first wall
		
	seg->rn=lrn;
	seg->primitive_uid[0]=-1;
	seg->data.wall.lx=lx1;
	seg->data.wall.lz=lz1;
	seg->data.wall.rx=rx1;
	seg->data.wall.rz=rz1;
	
	if (is_local) portal_split_segment_to_local(seg);
		
		// add for second wall
		
	if (map.nsegment>=max_segment) return;
	
	seg2=&map.segments[map.nsegment];
	map.nsegment++;
	
	memmove(seg2,seg,sizeof(segment_type));
	
	seg2->rn=rrn;
	seg2->type=sg_wall;
	seg2->primitive_uid[0]=-1;
	seg2->data.wall.lx=lx2;
	seg2->data.wall.lz=lz2;
	seg2->data.wall.rx=rx2;
	seg2->data.wall.rz=rz2;
	seg2->data.wall.ty=seg->data.wall.ty;
	seg2->data.wall.by=seg->data.wall.by;
	
	if (is_local) portal_split_segment_to_local(seg2);
}

void portal_split_segments_fc(int seg_idx,bool is_local)
{
	int						n,k,rn,ptsz,x[8],y[8],z[8],
							org_ptsz,org_x[8],org_y[8],org_z[8],
							nportal_list,portal_list[8];
	bool					first_poly;
	segment_type			*seg,*seg2;
	portal_type				*portal;
	
	seg=&map.segments[seg_idx];
	
	if (is_local) portal_split_segment_to_global(seg);
	
		// find portals for all vertexes

	nportal_list=0;
	
	for (n=0;n!=seg->data.fc.ptsz;n++) {
		rn=map_find_portal(&map,seg->data.fc.x[n],seg->data.fc.y[n],seg->data.fc.z[n]);
		
		for (k=0;k!=nportal_list;k++) {
			if (rn==portal_list[k]) {
				rn=-1;
				break;
			}
		}
		
		if (rn!=-1) {
			portal_list[nportal_list]=rn;
			nportal_list++;
		}
	}
	
		// only a single portal?
		
	if (nportal_list==1) {
		seg->rn=portal_list[0];
		if (is_local) portal_split_segment_to_local(seg);
		return;
	}
	
		// split it up
		
	first_poly=TRUE;
	
	org_ptsz=seg->data.fc.ptsz;
	memmove(org_x,seg->data.fc.x,(org_ptsz*sizeof(int)));
	memmove(org_y,seg->data.fc.y,(org_ptsz*sizeof(int)));
	memmove(org_z,seg->data.fc.z,(org_ptsz*sizeof(int)));

	for (n=0;n!=nportal_list;n++) {
	
		rn=portal_list[n];
		portal=&map.portals[rn];
		
		ptsz=org_ptsz;
		memmove(x,org_x,(ptsz*sizeof(int)));
		memmove(y,org_y,(ptsz*sizeof(int)));
		memmove(z,org_z,(ptsz*sizeof(int)));
		
		ptsz=clip_extend_polygon_to_2D_rect(ptsz,x,y,z,portal->x,portal->ex,portal->z,portal->ez);
		if (ptsz==0) continue;
		
		if (first_poly) {
			seg2=seg;			// replace original for first polygon
			first_poly=FALSE;
		}
		else {
			if (map.nsegment>=max_segment) break;
		
			seg2=&map.segments[map.nsegment];
			map.nsegment++;
		}
		
		memmove(seg2,seg,sizeof(segment_type));
		
		seg2->rn=rn;
		seg2->type=seg->type;
		seg2->primitive_uid[0]=-1;
		seg2->data.fc.ptsz=ptsz;
		memmove(seg2->data.fc.x,x,(ptsz*sizeof(int)));
		memmove(seg2->data.fc.y,y,(ptsz*sizeof(int)));
		memmove(seg2->data.fc.z,z,(ptsz*sizeof(int)));
		
		if (is_local) portal_split_segment_to_local(seg2);
	}
}

void portal_split_object_pos(d3pos *pos)
{
	pos->x+=map.portals[pos->rn].x;
	pos->z+=map.portals[pos->rn].z;
	map_find_portal_by_pos(&map,pos);
	pos->x-=map.portals[pos->rn].x;
	pos->z-=map.portals[pos->rn].z;
}

void portal_split(int rn,bool vertical)
{
	int					n,x,z,rn2,nsegment;
	portal_type			*org_portal,*portal;
	segment_type		*seg;
	
	undo_clear();
	
		// create new portal
		
	if (map.nportal>max_portal) return;
	
	rn2=map.nportal;
	portal=&map.portals[rn2];
	map.nportal++;
	
	org_portal=&map.portals[rn];
	memmove(portal,org_portal,sizeof(portal_type));
	map_portal_sight_clear(&map,rn2);
		
	if (!vertical) {
		z=(org_portal->z+org_portal->ez)>>1;
		org_portal->ez=z;
		portal->z=z;
	}
	else {
		x=(org_portal->x+org_portal->ex)>>1;
		org_portal->ex=x;
		portal->x=x;
	}
	
		// segments list can grow with splits
		// only check original segments
		
	nsegment=map.nsegment;
	
	for (n=0;n!=nsegment;n++) {
		seg=&map.segments[n];
		if (seg->rn!=rn) continue;
		
		switch (seg->type) {
		
			case sg_wall:
				portal_split_segments_wall(n,TRUE);
				break;
				
			case sg_floor:
			case sg_ceiling:
				portal_split_segments_fc(n,TRUE);
				break;
				
		}
	}
	
		// run through all the spots, scenery, etc,
		// and fix for correct portal
		
    for (n=0;n!=map.nspot;n++) {
        if (map.spots[n].pos.rn==rn) portal_split_object_pos(&map.spots[n].pos);
    }
    
    for (n=0;n!=map.nnode;n++) {
        if (map.nodes[n].pos.rn==rn) portal_split_object_pos(&map.nodes[n].pos);
    }
	
    for (n=0;n!=map.nsound;n++) {
        if (map.sounds[n].pos.rn==rn) portal_split_object_pos(&map.sounds[n].pos);
    }
	
    for (n=0;n!=map.nlight;n++) {
        if (map.lights[n].pos.rn==rn) portal_split_object_pos(&map.lights[n].pos);
    }
	
    for (n=0;n!=map.nscenery;n++) {
        if (map.sceneries[n].pos.rn==rn) portal_split_object_pos(&map.sceneries[n].pos);
    }

	
}

