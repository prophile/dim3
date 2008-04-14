/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: OBJ Import Routines

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
#include "import.h"
#include "common_view.h"
#include "portal_view.h"
#include "walk_view.h"

extern int						cr;
extern map_type					map;

#define import_max_vertex	50000

d3pnt						import_vertexes[import_max_vertex];	// supergumba -- make memory

extern void portal_split_segments_wall(int seg_idx,bool is_local);
extern void portal_split_segments_fc(int seg_idx,bool is_local);

/* =======================================================

      Import Segments from OBJs
      
======================================================= */

bool import_obj_segment_type(int npt,int *pvtx,segment_type *seg)
{
	int				n,y;
	
		// look for walls
	
	if (npt==4) {
	
		if ((import_vertexes[pvtx[0]].x==import_vertexes[pvtx[1]].x) && (import_vertexes[pvtx[0]].z==import_vertexes[pvtx[1]].z) && (import_vertexes[pvtx[2]].x==import_vertexes[pvtx[3]].x) && (import_vertexes[pvtx[2]].z==import_vertexes[pvtx[3]].z)) {
			seg->type=sg_wall;
			seg->data.wall.lx=import_vertexes[pvtx[0]].x;
			seg->data.wall.lz=import_vertexes[pvtx[0]].z;
			seg->data.wall.rx=import_vertexes[pvtx[2]].x;
			seg->data.wall.rz=import_vertexes[pvtx[2]].z;
			seg->data.wall.ty=import_vertexes[pvtx[0]].y;
			seg->data.wall.by=import_vertexes[pvtx[1]].y;
			if (seg->data.wall.ty>seg->data.wall.by) {
				y=seg->data.wall.by;
				seg->data.wall.by=seg->data.wall.ty;
				seg->data.wall.ty=y;
			}
			return(TRUE);
		}
			
		if ((import_vertexes[pvtx[0]].x==import_vertexes[pvtx[3]].x) && (import_vertexes[pvtx[0]].z==import_vertexes[pvtx[3]].z) && (import_vertexes[pvtx[1]].x==import_vertexes[pvtx[2]].x) && (import_vertexes[pvtx[1]].z==import_vertexes[pvtx[2]].z)) {
			seg->type=sg_wall;
			seg->data.wall.lx=import_vertexes[pvtx[0]].x;
			seg->data.wall.lz=import_vertexes[pvtx[0]].z;
			seg->data.wall.rx=import_vertexes[pvtx[1]].x;
			seg->data.wall.rz=import_vertexes[pvtx[1]].z;
			seg->data.wall.ty=import_vertexes[pvtx[0]].y;
			seg->data.wall.by=import_vertexes[pvtx[3]].y;
			if (seg->data.wall.ty>seg->data.wall.by) {
				y=seg->data.wall.by;
				seg->data.wall.by=seg->data.wall.ty;
				seg->data.wall.ty=y;
			}
			return(TRUE);
		}
	}
		
		// add as floors
		// when all segments are added, floors will be scanned
		// and turned into ceilings if they have x/z's touching
		// the top y of wall segments
		
	seg->type=sg_floor;
	seg->data.fc.ptsz=npt;
	
	for (n=0;n!=npt;n++) {
		seg->data.fc.x[n]=import_vertexes[pvtx[n]].x;
		seg->data.fc.z[n]=import_vertexes[pvtx[n]].z;
		seg->data.fc.y[n]=import_vertexes[pvtx[n]].y;
	}
	
	return(TRUE);
}

bool import_obj_create_segments(char *path,char *err_str)
{
	register int			i,k;
	int						nvertex,nline,
							pvtx[8],npt;
	char					txt[256],*c,vstr[256],vtstr[256];
	d3pnt					*vertex;
    segment_type			*seg;
	
		// load the file
		
	if (!textdecode_open(path,0x20)) {
		strcpy(err_str,"Couldn't open file.");
		return(FALSE);
    }
	
    nline=textdecode_count();
	
		// get the vertexes

    nvertex=0;
	vertex=import_vertexes;

    for (i=0;i!=nline;i++) {

        textdecode_get_piece(i,0,txt);
        
            // a vertex
            
        if (strcmp(txt,"v")==0) {
            if (nvertex>=import_max_vertex) {
				textdecode_close();
				sprintf(err_str,"Too many vertexes, can only import a maximum of %d vertexes.",import_max_vertex);
				return(FALSE);
			}
        
			textdecode_get_piece(i,1,txt);
			vertex->x=(int)(strtod(txt,NULL)*100);
			textdecode_get_piece(i,2,txt);
			vertex->y=-(int)(strtod(txt,NULL)*100);
			textdecode_get_piece(i,3,txt);
			vertex->z=-(int)(strtod(txt,NULL)*100);
        
            vertex++;
            nvertex++;
        }
    }

		// get the quads

    for (i=0;i!=nline;i++) {

        textdecode_get_piece(i,0,txt);

            // a face
            
        if (strcmp(txt,"f")!=0) continue;
        
            // get the face points
        
        npt=0;
        
        for ((k=0);(k!=8);k++) {
            textdecode_get_piece(i,(k+1),txt);
            if (txt[0]==0x0) break;
            
            vtstr[0]=0x0;
            
            strcpy(vstr,txt);
            c=strchr(vstr,'/');
            if (c!=NULL) {
                strcpy(vtstr,(c+1));
                *c=0x0;
            }
            c=strchr(vtstr,'/');
            if (c!=NULL) *c=0x0;
            
            pvtx[npt]=atoi(vstr)-1;
			
            npt++;
        }
		
			// create the segment
			
		seg=&map.segments[map.nsegment];
	//	segment_clear(seg,sg_wall,-1);
		
		if (import_obj_segment_type(npt,pvtx,seg)) {
			map.nsegment++;
		}
	}
	
	textdecode_close();

	return(TRUE);
}

/* =======================================================

      Fix Imported Segments to Fit Total Portal Size
      
======================================================= */

void import_obj_segments_fit_portals(import_obj_settings_type *oi_settings,int portal_x_start,int portal_z_start,int portal_total_x_sz,int portal_total_z_sz)
{
	int				i,t,minx,minz,maxx,maxz,miny,maxy;
	float			xfact,zfact,mfact;
	segment_type	*seg;
	
		// find extents for all segments
		
	minx=maxx=minz=maxz=miny=maxy=0;
	
	seg=map.segments;
	
	for (i=0;i!=map.nsegment;i++) {
		
		switch (seg->type) {
			case sg_wall:
				if (seg->data.wall.lx<minx) minx=seg->data.wall.lx;
				if (seg->data.wall.rx<minx) minx=seg->data.wall.rx;
				if (seg->data.wall.lz<minz) minz=seg->data.wall.lz;
				if (seg->data.wall.rz<minz) minz=seg->data.wall.rz;
				if (seg->data.wall.lx>maxx) maxx=seg->data.wall.lx;
				if (seg->data.wall.rx>maxx) maxx=seg->data.wall.rx;
				if (seg->data.wall.lz>maxz) maxz=seg->data.wall.lz;
				if (seg->data.wall.rz>maxz) maxz=seg->data.wall.rz;
				if (seg->data.wall.ty<miny) miny=seg->data.wall.ty;
				if (seg->data.wall.by<miny) miny=seg->data.wall.by;
				if (seg->data.wall.ty>maxy) maxy=seg->data.wall.ty;
				if (seg->data.wall.by>maxy) maxy=seg->data.wall.by;
				break;
			case sg_floor:
			case sg_ceiling:
				for (t=0;t!=seg->data.fc.ptsz;t++) {
					if (seg->data.fc.x[t]<minx) minx=seg->data.fc.x[t];
					if (seg->data.fc.z[t]<minz) minz=seg->data.fc.z[t];
					if (seg->data.fc.y[t]<miny) miny=seg->data.fc.y[t];
					if (seg->data.fc.x[t]>maxx) maxx=seg->data.fc.x[t];
					if (seg->data.fc.z[t]>maxz) maxz=seg->data.fc.z[t];
					if (seg->data.fc.y[t]>maxy) maxy=seg->data.fc.y[t];
				}
				break;
		}
		
		seg++;
	}
	
		// get segment factor
		
	xfact=(float)portal_total_x_sz/(float)(maxx-minx);
	zfact=(float)portal_total_z_sz/(float)(maxz-minz);
	
	mfact=xfact;
	if (zfact<mfact) mfact=zfact;
	
		// fit segments within portals total size
		
	seg=map.segments;
	
	for (i=0;i!=map.nsegment;i++) {
		
		switch (seg->type) {
			case sg_wall:
				seg->data.wall.lx=(int)((float)(seg->data.wall.lx-minx)*mfact)+portal_x_start;
				seg->data.wall.lz=(int)((float)(seg->data.wall.lz-minz)*mfact)+portal_z_start;
				seg->data.wall.rx=(int)((float)(seg->data.wall.rx-minx)*mfact)+portal_x_start;
				seg->data.wall.rz=(int)((float)(seg->data.wall.rz-minz)*mfact)+portal_z_start;
				seg->data.wall.ty=(int)(((float)(seg->data.wall.ty-miny))*mfact);
				seg->data.wall.by=((int)(((float)(seg->data.wall.by-miny))*mfact))-1;
				break;
			case sg_floor:
			case sg_ceiling:
				for (t=0;t!=seg->data.fc.ptsz;t++) {
					seg->data.fc.x[t]=(int)((float)(seg->data.fc.x[t]-minx)*mfact)+portal_x_start;
					seg->data.fc.z[t]=(int)((float)(seg->data.fc.z[t]-minz)*mfact)+portal_z_start;
					seg->data.fc.y[t]=(int)(((float)(seg->data.fc.y[t]-miny))*mfact);
				}
				break;
		}
		
		seg++;
	}
	
		// run any flips
	
	if ((oi_settings->flip_x) && (!oi_settings->flip_z)) return;
	
	seg=map.segments;
	
	for (i=0;i!=map.nsegment;i++) {
		
		switch (seg->type) {
			case sg_wall:
				if (oi_settings->flip_x) {
					seg->data.wall.lx=(portal_total_x_sz-(seg->data.wall.lx-portal_x_start))+portal_x_start;
					seg->data.wall.rx=(portal_total_x_sz-(seg->data.wall.rx-portal_x_start))+portal_x_start;
				}
				if (oi_settings->flip_z) {
					seg->data.wall.lz=(portal_total_z_sz-(seg->data.wall.lz-portal_z_start))+portal_z_start;
					seg->data.wall.rz=(portal_total_z_sz-(seg->data.wall.rz-portal_z_start))+portal_z_start;
				}
				break;
			case sg_floor:
			case sg_ceiling:
				for (t=0;t!=seg->data.fc.ptsz;t++) {
					if (oi_settings->flip_x) seg->data.fc.x[t]=(portal_total_x_sz-(seg->data.fc.x[t]-portal_x_start))+portal_x_start;
					if (oi_settings->flip_z) seg->data.fc.z[t]=(portal_total_z_sz-(seg->data.fc.z[t]-portal_z_start))+portal_z_start;
				}
				break;
		}
		
		seg++;
	}
	
}
	
/* =======================================================

      Convert Floors to Ceilings
      
======================================================= */

bool import_obj_fix_segment_xz_touch_wall_top(int x,int z,int y)
{
	register int	i;
	segment_type	*seg;
	
	for (i=0;i!=map.nsegment;i++) {
		seg=&map.segments[i];
		if (seg->type!=sg_wall) continue;
		if (seg->data.wall.ty!=y) continue;
		
		if ((x==seg->data.wall.lx) && (z==seg->data.wall.lz)) return(TRUE);
		if ((x==seg->data.wall.rx) && (z==seg->data.wall.rz)) return(TRUE);
	}
	
	return(FALSE);
}

bool import_obj_fix_segment_xz_touch_wall_bottom(int x,int z,int y)
{
	register int	i;
	segment_type	*seg;
	
	for (i=0;i!=map.nsegment;i++) {
		seg=&map.segments[i];
		if (seg->type!=sg_wall) continue;
		if ((seg->data.wall.by+1)!=y) continue;
		
		if ((x==seg->data.wall.lx) && (z==seg->data.wall.lz)) return(TRUE);
		if ((x==seg->data.wall.rx) && (z==seg->data.wall.rz)) return(TRUE);
	}
	
	return(FALSE);
}

void import_obj_fix_floor_to_ceilings(void)
{
	register int		i,t;
	bool				touch_top,touch_bottom;
	segment_type		*seg;
	
	for (i=0;i!=map.nsegment;i++) {
		seg=&map.segments[i];
		if (seg->type!=sg_floor) continue;
		
			// if a wall is touching below but not above,
			// then consider the segment a ceiling
		
		touch_top=touch_bottom=FALSE;
		
		for (t=0;t!=seg->data.fc.ptsz;t++) {
			touch_top|=import_obj_fix_segment_xz_touch_wall_top(seg->data.fc.x[t],seg->data.fc.z[t],seg->data.fc.y[t]);
			touch_bottom|=import_obj_fix_segment_xz_touch_wall_bottom(seg->data.fc.x[t],seg->data.fc.z[t],seg->data.fc.y[t]);
		}
		
		if ((touch_top) && (!touch_bottom)) seg->type=sg_ceiling;
	}
}

/* =======================================================

      Split segments that cross portals
      
======================================================= */

void import_obj_split_segments(void)
{
	int					n,nsegment;
	
		// segments list can grow with splits
		// only check original segments
		
	nsegment=map.nsegment;
	
	for (n=0;n!=nsegment;n++) {
		
		switch (map.segments[n].type) {
			case sg_wall:
				portal_split_segments_wall(n,FALSE);
				break;
			case sg_floor:
			case sg_ceiling:
				portal_split_segments_fc(n,FALSE);
				break;
		}
	}
}

/* =======================================================

      Put Segments in Correct Portal and Remove Unused
      
======================================================= */

void import_obj_fix_segment_portal(void)
{
	register int			i;
	register segment_type	*seg;
	int						rn,x,y,z;
	portal_type				*portal;
	
	seg=map.segments;
	
	for (i=0;i!=map.nsegment;i++) {
		
			// move segment into correct portal
			
	//	map_segment_calculate_center(&map,i,&x,&y,&z);	// supergumba -- all needs work
		rn=map_find_portal(&map,x,y,z);
		
		seg->rn=rn;
		portal=&map.portals[rn];
	//	segment_move(seg,portal->x,portal->z,0);
		
			// clean out other segment values
			
		seg->clip=wc_none;
		seg->curve=cv_none;
		seg->x_txtoff=seg->y_txtoff=0;
		seg->x_txtfact=seg->y_txtfact=1;
		seg->x_shift=seg->y_shift=0;
		seg->txt_ang=0;
		seg->dark_factor=1.0f;
		seg->alpha=1.0f;
		seg->group_idx=-1;
		seg->primitive_uid[0]=-1;
		seg->pass_through=FALSE;
		seg->moveable=FALSE;
		seg->lock=FALSE;
		seg->on=TRUE;
			
		seg++;
	}
}

void import_obj_remove_unused_portals(void)
{
	register int			n,i;
	register segment_type	*seg;
	register portal_type	*portal;
	bool					seg_hit[max_portal];
	
		// check for segments in portal
		
	portal=map.portals;
	
	for (n=0;n!=map.nportal;n++) {
		seg_hit[n]=FALSE;
		
		seg=map.segments;
	
		for (i=0;i!=map.nsegment;i++) {
			if (seg->rn==n) {
				seg_hit[n]=TRUE;
				break;
			}
			seg++;
		}
		
		portal++;
	}
	
		// delete unused portals
		
	for (n=map.nportal;n>=0;n--) {
		if (map.nportal==1) break;
		
		if (!seg_hit[n]) {
			cr=n;
			portal_delete();
		}
	}
}

/* =======================================================

      Segment Fills
      
======================================================= */

void import_obj_fix_segments_fill(import_obj_settings_type *oi_settings)
{
	register int		i;
	segment_type		*seg;
	
	seg=map.segments;
	
	for (i=0;i!=map.nsegment;i++) {
	
		switch (seg->type) {
			case sg_wall:
				seg->fill=oi_settings->texture_portal_wall;
				break;
			case sg_floor:
				seg->fill=oi_settings->texture_portal_floor;
				break;
			case sg_ceiling:
				seg->fill=oi_settings->texture_portal_ceiling;
				break;
		}
		
		seg++;
	}
}

/* =======================================================

      Import OBJ
      
======================================================= */

void map_import_obj(import_obj_settings_type *oi_settings)
{
	int							x,z,px,pz,pex,pez,
								portal_x_start,portal_z_start,
								portal_total_x_sz,portal_total_z_sz;
	char						err_str[256];
	unsigned char				c_str[256];
	
		// clear the map
		
	map_auto_generate_clear(&map);
	
		// get the portal sizes
	
	portal_total_x_sz=oi_settings->portal_x_sz*oi_settings->portal_x_cnt;
	portal_total_z_sz=oi_settings->portal_z_sz*oi_settings->portal_z_cnt;
	
	portal_x_start=(map_max_size/2)-((map_max_size-portal_total_x_sz)/2);
	portal_z_start=(map_max_size/2)-((map_max_size-portal_total_z_sz)/2);
	
		// create segments from OBJ
		
	if (!import_obj_create_segments(oi_settings->path,err_str)) {
		CopyCStringToPascal(err_str,c_str);
		StandardAlert(kAlertCautionAlert,"\pProblems Importing OBJ",c_str,NULL,NULL);
	}
	
		// fit all imported segments within total portal extent
		
	import_obj_segments_fit_portals(oi_settings,portal_x_start,portal_z_start,portal_total_x_sz,portal_total_z_sz);
	
		// all quads are imported as floors, find the ones that should be ceilings
		
	import_obj_fix_floor_to_ceilings();

		// create the portals
		
	for (z=0;z!=oi_settings->portal_z_cnt;z++) {
		for (x=0;x!=oi_settings->portal_x_cnt;x++) {
			px=portal_x_start+(x*oi_settings->portal_x_sz);
			pz=portal_z_start+(z*oi_settings->portal_z_sz);
			pex=px+oi_settings->portal_x_sz;
			pez=pz+oi_settings->portal_z_sz;
			
			cr=map_portal_create(&map,px,pz,pex,pez);
			if (cr==-1) continue;
		}
	}
	
		// split segments crossing portals
		
	import_obj_split_segments();
			
		// fix the segments to be in correct portals
				
	import_obj_fix_segment_portal();
	
		// remove any unused portals
		
	import_obj_remove_unused_portals();
	
		// fix segments uv and fills
		
	map_auto_generate_fix_segments_uv(&map);
	import_obj_fix_segments_fill(oi_settings);
	
		// lights and sight paths
		
	if (oi_settings->lights) map_auto_generate_add_simple_lights(&map);
	if (oi_settings->sight_path) map_portal_sight_generate_paths(&map,FALSE);
	
		// create player spot
		
	map_auto_generate_add_player_spot(&map);
}


