/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Primitives Routines

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
#include "dialog.h"

extern int						cr;
extern bool						dp_auto_texture;

extern file_path_setup_type		file_path_setup;
extern map_type					map;

short							primitive_reform_idx[128];
bool							primitive_open_editor,primitive_open_data,
								primitive_reform_ok;
map_type						primitive_map_editor,primitive_map_data;
segment_type					primitive_backup_segments[128];

/* =======================================================

      Load/Unload Primitive Map
      
======================================================= */

void primitive_load(void)
{
	char			path[1024];
	
		// load editor specific primitive map
	
	file_paths_app(&file_path_setup,path,"Contents/Resources/Primitives","Primitives","xml");
	primitive_open_editor=map_open_primitive(&primitive_map_editor,path);
	if (!primitive_open_editor) primitive_map_editor.nportal=0;
	
		// load data specific primitive map
	
	file_paths_data(&file_path_setup,path,"Maps","Primitives","xml");
	primitive_open_data=map_open_primitive(&primitive_map_data,path);
	if (!primitive_open_data) primitive_map_data.nportal=0;
	
		// no reforms
		
	primitive_reform_ok=FALSE;
}

void primitive_free(void)
{
	if (primitive_open_editor) map_close(&primitive_map_editor);
	if (primitive_open_data) map_close(&primitive_map_data);
}

/* =======================================================

      Primitive Size and Position
      
======================================================= */

void primitive_get_extend(int primitive_uid,int *p_minx,int *p_maxx,int *p_minz,int *p_maxz,int *p_miny,int *p_maxy)
{
	int				i,k,minx,maxx,minz,maxz,miny,maxy;
	segment_type	*seg;
	
	minx=minz=miny=1000000;
	maxx=maxz=maxy=0;
	
	seg=map.segments;
	
	for (i=0;i!=map.nsegment;i++) {
		if (seg->primitive_uid[0]!=primitive_uid) {
			seg++;
			continue;
		}
			
		switch (seg->type) {
			case sg_wall:
				if (seg->data.wall.lx<minx) minx=seg->data.wall.lx;
				if (seg->data.wall.lx>maxx) maxx=seg->data.wall.lx;
				if (seg->data.wall.rx<minx) minx=seg->data.wall.rx;
				if (seg->data.wall.rx>maxx) maxx=seg->data.wall.rx;
				if (seg->data.wall.lz<minz) minz=seg->data.wall.lz;
				if (seg->data.wall.lz>maxz) maxz=seg->data.wall.lz;
				if (seg->data.wall.rz<minz) minz=seg->data.wall.rz;
				if (seg->data.wall.rz>maxz) maxz=seg->data.wall.rz;
				if (seg->data.wall.ty<miny) miny=seg->data.wall.ty;
				if (seg->data.wall.ty>maxy) maxy=seg->data.wall.ty;
				if (seg->data.wall.by<miny) miny=seg->data.wall.by;
				if (seg->data.wall.by>maxy) maxy=seg->data.wall.by;
				break;
			case sg_floor:
			case sg_ceiling:
				for (k=0;k!=seg->data.fc.ptsz;k++) {
					if (seg->data.fc.x[k]<minx) minx=seg->data.fc.x[k];
					if (seg->data.fc.x[k]>maxx) maxx=seg->data.fc.x[k];
					if (seg->data.fc.z[k]<minz) minz=seg->data.fc.z[k];
					if (seg->data.fc.z[k]>maxz) maxz=seg->data.fc.z[k];
					if (seg->data.fc.y[k]<miny) miny=seg->data.fc.y[k];
					if (seg->data.fc.y[k]>maxy) maxy=seg->data.fc.y[k];
				}
				break;
		}
		
		seg++;
	}
	
	*p_minx=minx;
	*p_maxx=maxx;
	*p_minz=minz;
	*p_maxz=maxz;
	*p_miny=miny;
	*p_maxy=maxy;
}

void primitive_get_center(int primitive_uid,int *x,int *z,int *y)
{
	int				minx,maxx,minz,maxz,miny,maxy;

	primitive_get_extend(primitive_uid,&minx,&maxx,&minz,&maxz,&miny,&maxy);
	
	*x=(minx+maxx)/2;
	*z=(minz+maxz)/2;
	*y=(miny+maxy)/2;
}

void primitive_get_3D_handles(int primitive_uid,int *x,int *z,int *y)
{
	int				mx,mz,my,minx,maxx,minz,maxz,miny,maxy;

	primitive_get_extend(primitive_uid,&minx,&maxx,&minz,&maxz,&miny,&maxy);
	primitive_get_center(primitive_uid,&mx,&mz,&my);
	
	x[0]=minx;
	z[0]=mz;
	y[0]=my;
	x[1]=maxx;
	z[1]=mz;
	y[1]=my;
	x[2]=mx;
	z[2]=minz;
	y[2]=my;
	x[3]=mx;
	z[3]=maxz;
	y[3]=my;
	x[4]=mx;
	z[4]=mz;
	y[4]=miny;
	x[5]=mx;
	z[5]=mz;
	y[5]=maxy;
}

int primitive_count_segments(int primitive_uid)
{
	int					n,cnt;
	segment_type		*seg;
	
	cnt=0;
	seg=map.segments;
	
	for (n=0;n!=map.nsegment;n++) {
		if (seg->primitive_uid[0]==primitive_uid) cnt++;
		seg++;
	}
	
	return(cnt);
}

/* =======================================================

      Primitive Combine/Break/Reform
      
======================================================= */

void primitive_combine(void)
{
/* supergumba
	int				n,k,nsel_count,
					type,index,first_seg_idx,uid,primitive_uid;
	segment_type	*seg;
	
	undo_clear();
	
		// turn segments into primitive
		
	first_seg_idx=-1;
	
	nsel_count=select_count();
	primitive_uid=map_primitive_create_uid(&map);
	
	for (n=0;n!=nsel_count;n++) {
		select_get(n,&type,&index);
		if ((type!=segment_piece) && (type!=primitive_piece)) continue;
		
		if (type==segment_piece) {
			seg=&map.segments[index];
			if ((seg->type==sg_ambient_wall) || (seg->type==sg_ambient_fc) || (seg->type==sg_liquid)) continue;
			
			map_primitive_push_uid(seg,primitive_uid);
		}
		else {
			uid=map.segments[index].primitive_uid[0];
			
			seg=map.segments;
			
			for (k=0;k!=map.nsegment;k++) {
				if (seg->primitive_uid[0]==uid) {
					map_primitive_push_uid(seg,primitive_uid);
					// seg->primitive_uid[0]=primitive_uid;
				}
				seg++;
			}
		}
		
		if (first_seg_idx==-1) first_seg_idx=index;
	}
	
		// primitive is now selection
		
	if (first_seg_idx!=-1) {
		select_clear();
	//	select_add(primitive_piece,first_seg_idx);
	}
	
	primitive_reform_ok=FALSE;
	menu_fix_enable();
	
	main_wind_draw();
	*/
}

void primitive_break(void)
{
/* supergumba
	int				n,k,nsel_count,reform_idx,
					type,index,primitive_uid;
	segment_type	*seg;
	
	undo_clear();
	
		// start duplicate selection
		
	select_duplicate_clear();
	
		// break up primitives
		
	nsel_count=select_count();
	reform_idx=0;
	
	for (n=0;n!=nsel_count;n++) {
		select_get(n,&type,&index);
		if (type!=primitive_piece) continue;
		
			// turn off all primitives
			
		primitive_uid=map.segments[index].primitive_uid[0];
		
		seg=map.segments;
		
		for (k=0;k!=map.nsegment;k++) {
			if (seg->primitive_uid[0]==primitive_uid) {
				map_primitive_pop_uid(seg);
				primitive_reform_idx[reform_idx++]=k;
				select_duplicate_add(segment_piece,k);
			}
			seg++;
		}
		
		primitive_reform_idx[reform_idx]=-1;
	}
	
	select_duplicate_copy();
	
	primitive_reform_ok=TRUE;
	menu_fix_enable();
	
	main_wind_draw();
	*/
}

void primitive_reform(void)
{
	int				reform_idx,seg_idx,primitive_uid;
	
	if (!primitive_reform_ok) return;

	undo_clear();

		// reform primitive
	
	reform_idx=0;
	primitive_uid=map_primitive_create_uid(&map);
	
	while (TRUE) {
		seg_idx=primitive_reform_idx[reform_idx++];
		if (seg_idx==-1) break;
		
		map.segments[seg_idx].primitive_uid[0]=primitive_uid;
	}
	
	select_clear();
//	select_add(primitive_piece,primitive_reform_idx[0]);
	
	primitive_reform_ok=FALSE;
	menu_fix_enable();
	
	main_wind_draw();
}

void primitive_reform_reset_for_delete(int seg_idx)
{
	int				n,reform_idx,idx;
	
	if (!primitive_reform_ok) return;

	reform_idx=0;
	
	while (TRUE) {
		idx=primitive_reform_idx[reform_idx];
		if (idx==-1) break;
		
			// this segment has been deleted, remove from list
			
		if (idx==seg_idx) {
			
			n=reform_idx;
			
			while (TRUE) {
				if (primitive_reform_idx[n]==-1) break;
				primitive_reform_idx[n]=primitive_reform_idx[n+1];
				n++;
			}
			
			continue;
		}
		
			// if this is a later segment, change index
			
		if (idx>seg_idx) primitive_reform_idx[reform_idx]=primitive_reform_idx[reform_idx]-1;
		
		reform_idx++;
	}
}

/* =======================================================

      Primitive Moving
      
======================================================= */

void primitive_move(int primitive_uid,int xchg,int zchg,int ychg)
{
	int					n;
	segment_type		*seg;
	
	seg=map.segments;
	
	for (n=0;n!=map.nsegment;n++) {
		if (seg->primitive_uid[0]==primitive_uid) {
			segment_move(seg,xchg,zchg,ychg);
		}
		seg++;
	}
}

void primitive_change_portal(int primitive_uid,int rn)
{
	int					n;
	segment_type		*seg;
	
	seg=map.segments;
	
	for (n=0;n!=map.nsegment;n++) {
		if (seg->primitive_uid[0]==primitive_uid) seg->rn=rn;
		seg++;
	}
}

/* =======================================================

      Duplicate and Delete Primitives
      
======================================================= */

int primitive_duplicate(int primitive_uid,int xadd,int zadd)
{
	int			i,k,old_cnt,nxt_primitive_uid,dup_index;
	
	dup_index=-1;
	nxt_primitive_uid=map_primitive_create_uid(&map);
	
	old_cnt=map.nsegment;
	
	for (i=0;i!=old_cnt;i++) {
		if (map.segments[i].primitive_uid[0]==primitive_uid) {
			k=segment_duplicate(i,nxt_primitive_uid,xadd,zadd);
			if (dup_index==-1) dup_index=k;
		}
	}
	
	return(dup_index);
}

void primitive_delete(int primitive_uid)
{
	int			i;
	
	i=0;
	
	while (i<map.nsegment) {
		if (map.segments[i].primitive_uid[0]!=primitive_uid) {
			i++;
			continue;
		}
		segment_delete(i);
	}
}

/* =======================================================

      Primitive Extrude
      
======================================================= */

void primitive_extrude(int seg_index)
{
	int					n,k,lx,rx,lz,rz,ty,by,yadd,primitive_uid;
	segment_type		*seg,*seg2;
	
	seg=&map.segments[seg_index];
	if ((seg->type!=sg_floor) && (seg->type!=sg_ceiling)) return;

	primitive_uid=map_primitive_create_uid(&map);
	
		// original segment part of primitive
		
	seg->primitive_uid[0]=primitive_uid;
	
		// dup floor/ceiling
		
	if (map.nsegment>=max_segment) return;
	
	seg2=&map.segments[map.nsegment];
	map.nsegment++;
	
	*seg2=*seg;
	
	if (seg->type==sg_floor) {
		seg->type=sg_ceiling;
		seg2->type=sg_floor;
		by=seg->data.fc.y[0];
		ty=by-10;
		yadd=-10;
	}
	else {
		seg->type=sg_floor;
		seg2->type=sg_ceiling;
		ty=seg->data.fc.y[0];
		by=ty+10;
		yadd=10;
	}
	
	for (n=0;n!=seg2->data.fc.ptsz;n++) {
		seg2->data.fc.y[n]+=yadd;
	}
	
		// create the walls
	
	for (n=0;n!=seg2->data.fc.ptsz;n++) {
		k=n+1;
		if (k==seg2->data.fc.ptsz) k=0;
		
		lx=seg2->data.fc.x[n];
		lz=seg2->data.fc.z[n];
		rx=seg2->data.fc.x[k];
		rz=seg2->data.fc.z[k];
		
		segment_add_wall(lx,lz,rx,rz,ty,(by-1),wc_none,cv_none,primitive_uid,FALSE);
		
		map.segments[map.nsegment-1].fill=seg->fill;
	}
}

/* =======================================================

      Resize Primitives
      
======================================================= */

void primitive_backup(int primitive_uid)
{
	int				i;
	segment_type	*seg,*bk_seg;
	
	seg=map.segments;
	bk_seg=primitive_backup_segments;
	
	for (i=0;i!=map.nsegment;i++) {
		if (seg->primitive_uid[0]==primitive_uid) {
			memmove(bk_seg,seg,sizeof(segment_type));
			bk_seg++;
		}
		seg++;
	}
}

void primitive_restore(int primitive_uid)
{
	int				i;
	segment_type	*seg,*bk_seg;
	
	seg=map.segments;
	bk_seg=primitive_backup_segments;
	
	for (i=0;i!=map.nsegment;i++) {
		if (seg->primitive_uid[0]==primitive_uid) {
			memmove(seg,bk_seg,sizeof(segment_type));
			bk_seg++;
		}
		seg++;
	}
}

void primitive_resize_vertex(int *k,int org_min,int org_max,int min,int max)
{
	int			d,org_d;
	
		// if already at min or max, just move to new min or max
		
	if (*k==org_min) {
		*k=min;
		return;
	}
	
	if (*k==org_max) {
		*k=max;
		return;
	}
	
		// resize
	
	d=max-min;
	org_d=org_max-org_min;

	*k=((((*k)-org_min)*d)/org_d)+min;
}

void primitive_resize(int primitive_uid,int minx,int maxx,int minz,int maxz,int miny,int maxy)
{
	int				i,k,
					org_minx,org_maxx,org_minz,org_maxz,org_miny,org_maxy;
	segment_type	*seg;
	
		// original size
		
	primitive_get_extend(primitive_uid,&org_minx,&org_maxx,&org_minz,&org_maxz,&org_miny,&org_maxy);
	
	seg=map.segments;
	
	for (i=0;i!=map.nsegment;i++) {
		if (seg->primitive_uid[0]!=primitive_uid) {
			seg++;
			continue;
		}
		
		switch (seg->type) {
		
			case sg_wall:
				primitive_resize_vertex(&seg->data.wall.lx,org_minx,org_maxx,minx,maxx);
				primitive_resize_vertex(&seg->data.wall.rx,org_minx,org_maxx,minx,maxx);
				primitive_resize_vertex(&seg->data.wall.lz,org_minz,org_maxz,minz,maxz);
				primitive_resize_vertex(&seg->data.wall.rz,org_minz,org_maxz,minz,maxz);
				primitive_resize_vertex(&seg->data.wall.ty,org_miny,org_maxy,miny,maxy);
				seg->data.wall.by++;
				primitive_resize_vertex(&seg->data.wall.by,org_miny,org_maxy,miny,maxy);
				seg->data.wall.by--;
				if (dp_auto_texture) map_segment_reset_texture_uvs(&map,seg);
				break;
				
			case sg_floor:
			case sg_ceiling:
				for (k=0;k!=seg->data.fc.ptsz;k++) {
					primitive_resize_vertex(&seg->data.fc.x[k],org_minx,org_maxx,minx,maxx);
					primitive_resize_vertex(&seg->data.fc.z[k],org_minz,org_maxz,minz,maxz);
					primitive_resize_vertex(&seg->data.fc.y[k],org_miny,org_maxy,miny,maxy);
				}
				if (dp_auto_texture) map_segment_reset_texture_uvs(&map,seg);
				break;
				
		}
		

		seg++;
	}
}

/* =======================================================

      Add/Replace Primitives
      
======================================================= */

void primitive_add(void)
{
	int				n,x,y,z,kx,ky,kz,minx,maxx,miny,maxy,minz,maxz,
					fill,portal_idx,primitive_uid,nsegment;
	bool			in_editor;
	segment_type	*seg,*primitive_seg;
	
		// get primitive
		
	primitive_load();
	
	if (!dialog_choose_primitive_run(&portal_idx,&in_editor)) {
		primitive_free();
		return;
	}
	
		// create

	primitive_uid=map_primitive_create_uid(&map);
	fill=texture_palette_get_selected_fill();

	if (in_editor) {
		nsegment=primitive_map_editor.nsegment;
		primitive_seg=primitive_map_editor.segments;
	}
	else {
		nsegment=primitive_map_data.nsegment;
		primitive_seg=primitive_map_data.segments;
	}
	
	for (n=0;n!=nsegment;n++) {
	
		if (map.nsegment>=max_segment) break;			// out of segments?
	
		if (primitive_seg->rn==portal_idx) {

			seg=&map.segments[map.nsegment];
			map.nsegment++;
			
			memmove(seg,primitive_seg,sizeof(segment_type));
			seg->rn=cr;
			seg->primitive_uid[0]=primitive_uid;
			seg->fill=fill;
		}

		primitive_seg++;
	}
	
		// recenter
		
	primitive_get_extend(primitive_uid,&minx,&maxx,&minz,&maxz,&miny,&maxy);
	piece_create_get_spot(&x,&y,&z,((maxx-minx)/2),((maxz-minz)/2),((maxy-miny)/2));
	
	primitive_get_center(primitive_uid,&kx,&kz,&ky);
	primitive_move(primitive_uid,(kx-x),(kz-z),(ky-y));
	
	primitive_free();

		// change selection and redraw
		
	select_clear();
//	select_add(primitive_piece,(map.nsegment-1));
	
	main_wind_draw();
	texture_palette_reset();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
	menu_fix_enable();
}

void primitive_replace(void)
{
	int				n,min_x,min_z,min_y,max_x,max_z,max_y,
					fill,portal_idx,primitive_uid,nsegment;
	bool			in_editor;
	segment_type	*seg,*primitive_seg;
	
		// if no selection, then just add
		
	if (select_count()==0) {
		primitive_add();
		return;
	}
	
		// get primitive
		
	primitive_load();
	
	if (!dialog_choose_primitive_run(&portal_idx,&in_editor)) {
		primitive_free();
		return;
	}
	
		// create

	primitive_uid=map_primitive_create_uid(&map);
	fill=texture_palette_get_selected_fill();

	if (in_editor) {
		nsegment=primitive_map_editor.nsegment;
		primitive_seg=primitive_map_editor.segments;
	}
	else {
		nsegment=primitive_map_data.nsegment;
		primitive_seg=primitive_map_data.segments;
	}
	
	for (n=0;n!=nsegment;n++) {
	
		if (map.nsegment>=max_segment) break;			// out of segments?
	
		if (primitive_seg->rn==portal_idx) {

			seg=&map.segments[map.nsegment];
			map.nsegment++;
			
			memmove(seg,primitive_seg,sizeof(segment_type));
			seg->rn=cr;
			seg->primitive_uid[0]=primitive_uid;
			seg->fill=fill;
		}

		primitive_seg++;
	}
	
		// delete old selection
		
	select_get_extent(&min_x,&min_z,&min_y,&max_x,&max_z,&max_y);
	piece_delete();
	
		// resize
		
	primitive_resize(primitive_uid,min_x,max_x,min_z,max_z,min_y,max_y);

	primitive_free();
	
		// change selection and redraw
		
	select_clear();
//	select_add(primitive_piece,(map.nsegment-1));
	
	main_wind_draw();
	texture_palette_reset();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
	menu_fix_enable();
}

/* =======================================================

      Save Primitives
      
======================================================= */

void primitive_save(void)
{
/* supergumba
	int				n,type,index,rn,nportal,x,z,x_sz,z_sz,
					primitive_uid,minx,maxx,minz,maxz,miny,maxy;
	char			name[name_str_len],path[1024];
	segment_type	*seg,*primitive_seg;
	
		// only save selected primitive
		
	if (select_count()==0) return;
	
	select_get(0,&type,&index);
	if (type!=primitive_piece) return;
	
	primitive_uid=map.segments[index].primitive_uid[0];
	
		// name of primitive
		
	if (!dialog_primitive_settings_run(name)) return;
	
		// does primitive map exist?  If not, create a new one
		
	file_paths_data(&file_path_setup,path,"Maps","Primitives","xml");
	if (!map_open_primitive(&primitive_map_data,path)) {
		map_new(&primitive_map_data,"Primitives");
	}
	
		// get primitive position to place at 0,0 point in new portal
		
	primitive_get_extend(primitive_uid,&minx,&maxx,&minz,&maxz,&miny,&maxy);

	x_sz=maxx-minx;
	if (x_sz<100) x_sz=100;
	
	z_sz=maxz-minz;
	if (z_sz<100) z_sz=100;
	
		// create portal
		
	nportal=primitive_map_data.nportal;
	x=((nportal%10)*120)+100;
	z=((nportal/10)*120)+100;
	rn=map_portal_create(&primitive_map_data,x,z,(x+x_sz),(z+z_sz));
	
	if (rn==-1) {
		map_close(&primitive_map_data);
		return;
	}
	
	strcpy(primitive_map_data.portals[rn].name,name);
	
		// add segments
		
	primitive_seg=map.segments;
	
	for (n=0;n!=map.nsegment;n++) {

		if (primitive_seg->primitive_uid[0]==primitive_uid) {
			if (primitive_map_data.nsegment>=max_segment) break;

			seg=&primitive_map_data.segments[primitive_map_data.nsegment];
			primitive_map_data.nsegment++;
			
			memmove(seg,primitive_seg,sizeof(segment_type));
			segment_move(seg,minx,minz,0);
			
			seg->rn=rn;
			seg->fill=0;
		}

		primitive_seg++;
	}
	
		// save map
		
	map_save(&primitive_map_data);
	
		// close map
	
	map_close(&primitive_map_data);
	*/
}
