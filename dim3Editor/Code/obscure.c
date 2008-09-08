/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Obscure Routines

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

extern int					obscure_mesh_idx,cx,cy,cz;
extern map_type				map;

extern AGLContext			ctx;

#define obscure_mesh_view_mesh_wid				160
#define obscure_mesh_view_mesh_high				120
#define obscure_mesh_view_mesh_aspect_ratio		1.0f
#define obscure_mesh_view_mesh_fov				60
#define obscure_mesh_view_mesh_near_z			(6*map_enlarge)
#define obscure_mesh_view_mesh_far_z			(2000*map_enlarge)
#define obscure_mesh_view_mesh_near_z_offset	-(3*map_enlarge)
#define obscure_mesh_view_min_distance			(100*map_enlarge)
#define obscure_mesh_view_slop_angle			30.0f

// #define OBSCURE_TEST		1

/* =======================================================

      Obscure Mesh Sorting
      
======================================================= */

int obscure_mesh_sort(d3pnt *pt,int *mesh_sort_list)
{
	int					n,t,sz,d,cnt,idx;
	int					*dist;
	map_mesh_type		*mesh;

	dist=valloc(max_mesh*sizeof(int));

	cnt=0;

	for (n=0;n!=map.mesh.nmesh;n++) {

		mesh=&map.mesh.meshes[n];

			// ignore meshes outside of draw distance

		d=map_calculate_mesh_distance(mesh,pt);
		if (d>(obscure_mesh_view_mesh_far_z-obscure_mesh_view_mesh_near_z)) continue;

			// sort mesh

		idx=-1;
	
		for (t=0;t!=cnt;t++) {
			if (dist[t]>d) {
				idx=t;
				break;
			}
		}
	
			// insert at end of list
			
		if (idx==-1) {
			dist[cnt]=d;
			mesh_sort_list[cnt]=n;
			cnt++;
			continue;
		}
		
			// insert in list
			
		sz=sizeof(int)*(cnt-idx);
		memmove(&dist[idx+1],&dist[idx],sz);
		memmove(&mesh_sort_list[idx+1],&mesh_sort_list[idx],sz);
		
		dist[idx]=d;
		mesh_sort_list[idx]=n;
		
		cnt++;
	}

	free(dist);

	return(cnt);
}

/* =======================================================

      Obscure Mesh View Clipping
      
======================================================= */

bool obscure_check_mesh_view_clip(d3pnt *min,d3pnt *max,d3pnt *view_pt,int *vport,double *mod_matrix,double *proj_matrix)
{
	int			n,px[8],py[8],pz[8];
	double		dx,dy,dz;
	bool		hit,lft,rgt,top,bot;

	px[0]=px[3]=px[4]=px[7]=min->x;
	px[1]=px[2]=px[5]=px[6]=max->x;

	py[0]=py[1]=py[2]=py[3]=min->y;
	py[4]=py[5]=py[6]=py[7]=max->y;

	pz[0]=pz[1]=pz[4]=pz[5]=min->z;
	pz[2]=pz[3]=pz[6]=pz[7]=max->z;

		// transform points
		
	for (n=0;n!=8;n++) {
		px[n]-=view_pt->x;
		py[n]-=view_pt->y;
		pz[n]=view_pt->z-pz[n];
	}
	
		// check if points are behind z
		
	hit=FALSE;
	
	for (n=0;n!=8;n++) {
		if (((((double)px[n])*mod_matrix[2])+(((double)py[n])*mod_matrix[6])+(((double)pz[n])*mod_matrix[10])+mod_matrix[14])<0.0) {
			hit=TRUE;
			break;
		}
	}
	
	if (!hit) return(FALSE);

		// check if poly is projected offscreen

	lft=rgt=top=bot=TRUE;

	for (n=0;n!=8;n++) {
		gluProject(px[n],py[n],pz[n],mod_matrix,proj_matrix,(GLint*)vport,&dx,&dy,&dz);

		lft=lft&&(dx<0);
		rgt=rgt&&(dx>=obscure_mesh_view_mesh_wid);
		top=top&&(dy<0);
		bot=bot&&(dy>=obscure_mesh_view_mesh_high);
	}

	return(!(lft||rgt||top||bot));
}

/* =======================================================

      Obscure Bits
      
======================================================= */

inline void obscure_mesh_view_bit_set(map_mesh_type *mesh,int idx)
{
	mesh->obscure.visibility_flag[idx>>3]=mesh->obscure.visibility_flag[idx>>3]|(0x1<<(idx&0x7));
}

inline bool obscure_mesh_view_bit_get(map_mesh_type *mesh,int idx)
{
	return((mesh->obscure.visibility_flag[idx>>3]&(0x1<<(idx&0x7)))!=0x0);
}

/* =======================================================

      Min Distance Between Meshes
      
======================================================= */

void obscure_calculate_meshes_close_single_point(int which_pt,map_mesh_type *mesh,d3pnt *pt)
{
	switch (which_pt) {
	
		case 1:
			pt->x=mesh->box.mid.x;
			pt->y=mesh->box.min.y;
			pt->z=mesh->box.mid.z;
			break;
			
		case 2:
			pt->x=mesh->box.mid.x;
			pt->y=mesh->box.max.y;
			pt->z=mesh->box.mid.z;
			break;
			
		case 3:
			pt->x=mesh->box.min.x;
			pt->y=mesh->box.mid.y;
			pt->z=mesh->box.mid.z;
			break;
			
		case 4:
			pt->x=mesh->box.max.x;
			pt->y=mesh->box.mid.y;
			pt->z=mesh->box.mid.z;
			break;
			
		case 5:
			pt->x=mesh->box.mid.x;
			pt->y=mesh->box.mid.y;
			pt->z=mesh->box.min.z;
			break;
			
		case 6:
			pt->x=mesh->box.mid.x;
			pt->y=mesh->box.mid.y;
			pt->z=mesh->box.max.z;
			break;
			
		default:
			pt->x=mesh->box.mid.x;
			pt->y=mesh->box.mid.y;
			pt->z=mesh->box.mid.z;
			break;
			
	}
}

bool obscure_calculate_meshes_close(map_mesh_type *mesh,map_mesh_type *view_mesh)
{
	int			n,k,dist;
	d3pnt		pt,pt2;
	
	for (n=0;n!=7;n++) {
	
		obscure_calculate_meshes_close_single_point(n,mesh,&pt);
		
		for (k=0;k!=7;k++) {

			obscure_calculate_meshes_close_single_point(k,view_mesh,&pt2);

			dist=distance_get(pt.x,pt.y,pt.z,pt2.x,pt2.y,pt2.z);
			if (dist<obscure_mesh_view_min_distance) return(TRUE);
		}
		
	}

	return(FALSE);
}

/* =======================================================

      Obscure Meshes
      
======================================================= */

void obscure_calculate_mesh_single_view(int mesh_idx,d3pnt *cpt,unsigned char *stencil_pixels,int *mesh_sort_list,float x_rot,float y_rot)
{
	int						n,k,t,mesh_cnt,stencil_idx,
							last_stencil_mesh_idx,next_last_stencil_mesh_idx,
							view_mesh_idx,idx;
	int						stencil_idx_to_mesh_idx[256];
	bool					transparent,more_batch;
	unsigned char			*sp;
	unsigned long			cur_gl_id;
	d3pnt					*pt;
	map_mesh_type			*mesh,*view_mesh;
	map_mesh_poly_type		*poly;
	texture_type			*texture;
	int						vport[4];
	double					mod_matrix[16],proj_matrix[16];
	
	mesh=&map.mesh.meshes[mesh_idx];

		// drawing rotation setup
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(x_rot,1,0,0);						// x pan
	glRotatef(0.0f,0,0,1);						// z pan
	glRotatef(((360.0f-y_rot)+180.0f),0,1,0);	// y rotate -- need to reverse the winding

	glGetIntegerv(GL_VIEWPORT,(GLint*)vport);
	glGetDoublev(GL_PROJECTION_MATRIX,proj_matrix);
	glGetDoublev(GL_MODELVIEW_MATRIX,mod_matrix);

		// setup texturing

	cur_gl_id=-1;

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
		// get sort list
		
	mesh_cnt=obscure_mesh_sort(cpt,mesh_sort_list);

		// run through all other meshes to test
		// their visibility against this mesh

		// as we are using the stencil buffer, we can only do up to 255
		// meshes at a time

	last_stencil_mesh_idx=0;
	next_last_stencil_mesh_idx=0;

	while (TRUE) {

		glClear(GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		
	// supergumba -- test obscure draw
#ifdef OBSCURE_TEST
		glClear(GL_COLOR_BUFFER_BIT);
		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
#endif

		stencil_idx=1;
		more_batch=FALSE;

		for (n=0;n!=mesh_cnt;n++) {
			
			view_mesh_idx=mesh_sort_list[n];
			view_mesh=&map.mesh.meshes[view_mesh_idx];
			
				// don't draw the mesh itself, as it might
				// uncessarly block
			
			if (view_mesh_idx==mesh_idx) {
				if (mesh->flag.no_self_obscure) continue;
			}
			
				// meshes within a certain min distance
				// are always included
				
			if (obscure_calculate_meshes_close(mesh,view_mesh)) obscure_mesh_view_bit_set(mesh,view_mesh_idx);
		
				// ignore all moving meshes as they
				// won't always obscure

			if (view_mesh->flag.moveable) {
				obscure_mesh_view_bit_set(mesh,view_mesh_idx);
				continue;
			}

				// don't draw meshes clipped from view

			if (!obscure_check_mesh_view_clip(&view_mesh->box.min,&view_mesh->box.max,cpt,vport,mod_matrix,proj_matrix)) continue;

				// stencil in the mesh we are looking at
				// since we might have more than 255 meshes, we
				// are doing this in batches

			if (n<last_stencil_mesh_idx) {
				glStencilFunc(GL_ALWAYS,0,0xFF);
			}
			else {
				if (stencil_idx!=256) {
					stencil_idx_to_mesh_idx[stencil_idx]=view_mesh_idx;
					next_last_stencil_mesh_idx=n+1;
					glStencilFunc(GL_ALWAYS,stencil_idx,0xFF);
					stencil_idx++;
				}
				else {
					glStencilFunc(GL_ALWAYS,0,0xFF);
					more_batch=TRUE;
				}
			}

				// draw polys of the mesh

			for (t=0;t!=view_mesh->npoly;t++) {

				poly=&view_mesh->polys[t];

					// transparent polygons don't write into the z
					// but are compared to see if they are obscured
					
				transparent=FALSE;

				texture=&map.textures[poly->txt_idx];
				if ((texture->bitmaps[0].alpha_mode==alpha_mode_transparent) || (poly->alpha!=1.0f)) {
					transparent=TRUE;
					glDepthMask(GL_FALSE);
				}

					// setup texture

				if (texture->bitmaps[0].gl_id!=cur_gl_id) {
					cur_gl_id=texture->bitmaps[0].gl_id;
					glBindTexture(GL_TEXTURE_2D,cur_gl_id);
				}

					// write to the stencil buffer if any part
					// of the polygon can be seen

				glBegin(GL_POLYGON);

				for (k=0;k!=poly->ptsz;k++) {
					pt=&view_mesh->vertexes[poly->v[k]];
					glTexCoord2f(poly->gx[k],poly->gy[k]);
					glVertex3i((pt->x-cpt->x),(pt->y-cpt->y),(cpt->z-pt->z));
				}

				glEnd();

				if (transparent) glDepthMask(GL_TRUE);
			}
		}

		glDisable(GL_TEXTURE_2D);
	
	// supergumba -- test obscure draw
#ifdef OBSCURE_TEST
		aglSwapBuffers(ctx);
		while (Button()) {}
		while (!Button()) {}
#endif
			// read the stencil to look for hits

		glReadPixels(0,0,obscure_mesh_view_mesh_wid,obscure_mesh_view_mesh_high,GL_STENCIL_INDEX,GL_UNSIGNED_BYTE,stencil_pixels);

		sp=stencil_pixels;

		for (k=0;k!=(obscure_mesh_view_mesh_wid*obscure_mesh_view_mesh_high);k++) {
			idx=(int)*sp++;
			if (idx!=0) obscure_mesh_view_bit_set(mesh,stencil_idx_to_mesh_idx[idx]);
		}

			// any more batches to do?

		if (!more_batch) break;

		last_stencil_mesh_idx=next_last_stencil_mesh_idx;
		if (last_stencil_mesh_idx>=map.mesh.nmesh) break;
	}
}

bool obscure_calculate_mesh(int mesh_idx)
{
	int				*mesh_sort_list;
	float			ratio;
	unsigned char	*stencil_pixels;
	d3pnt			cpt;
	map_mesh_type	*mesh;

	mesh=&map.mesh.meshes[mesh_idx];

		// pixels for reading stencil buffer
		
	stencil_pixels=valloc(obscure_mesh_view_mesh_wid*obscure_mesh_view_mesh_high);
	if (stencil_pixels==NULL) return(FALSE);
	
		// memory for sorted mesh list
		
	mesh_sort_list=valloc(sizeof(int)*max_mesh);
	if (mesh_sort_list==NULL) {
		free(stencil_pixels);
		return(FALSE);
	}

		// global drawing setup
		
	glDisable(GL_SCISSOR_TEST);

	glViewport(0,0,obscure_mesh_view_mesh_wid,obscure_mesh_view_mesh_high);
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	ratio=(((float)obscure_mesh_view_mesh_wid)/((float)obscure_mesh_view_mesh_high))*obscure_mesh_view_mesh_aspect_ratio;
	gluPerspective(obscure_mesh_view_mesh_fov,ratio,obscure_mesh_view_mesh_near_z,obscure_mesh_view_mesh_far_z);
	glScalef(-1.0f,-1.0f,1.0f);						// x and y are reversed
	glTranslatef(0.0f,0.0f,(float)obscure_mesh_view_mesh_near_z_offset);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_BLEND);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

		// clear visibility bits
		
	bzero(mesh->obscure.visibility_flag,max_mesh_visibility_bytes);

		// self always in visibility list

	obscure_mesh_view_bit_set(mesh,mesh_idx);

		// build mesh looking forward

	memmove(&cpt,&mesh->box.mid,sizeof(d3pnt));
	cpt.z=(mesh->box.min.z+mesh->box.mid.z)/2;
	obscure_calculate_mesh_single_view(mesh_idx,&cpt,stencil_pixels,mesh_sort_list,obscure_mesh_view_slop_angle,0.0f);
	obscure_calculate_mesh_single_view(mesh_idx,&cpt,stencil_pixels,mesh_sort_list,0.0f,0.0f);
	obscure_calculate_mesh_single_view(mesh_idx,&cpt,stencil_pixels,mesh_sort_list,-obscure_mesh_view_slop_angle,0.0f);

		// build mesh looking right

	memmove(&cpt,&mesh->box.mid,sizeof(d3pnt));
	cpt.x=(mesh->box.max.x+mesh->box.mid.x)/2;
	obscure_calculate_mesh_single_view(mesh_idx,&cpt,stencil_pixels,mesh_sort_list,obscure_mesh_view_slop_angle,90.0f);
	obscure_calculate_mesh_single_view(mesh_idx,&cpt,stencil_pixels,mesh_sort_list,0.0f,90.0f);
	obscure_calculate_mesh_single_view(mesh_idx,&cpt,stencil_pixels,mesh_sort_list,-obscure_mesh_view_slop_angle,90.0f);

		// build mesh looking back

	memmove(&cpt,&mesh->box.mid,sizeof(d3pnt));
	cpt.z=(mesh->box.max.z+mesh->box.mid.z)/2;
	obscure_calculate_mesh_single_view(mesh_idx,&cpt,stencil_pixels,mesh_sort_list,obscure_mesh_view_slop_angle,180.0f);
	obscure_calculate_mesh_single_view(mesh_idx,&cpt,stencil_pixels,mesh_sort_list,0.0f,180.0f);
	obscure_calculate_mesh_single_view(mesh_idx,&cpt,stencil_pixels,mesh_sort_list,-obscure_mesh_view_slop_angle,180.0f);

		// build mesh looking left

	memmove(&cpt,&mesh->box.mid,sizeof(d3pnt));
	cpt.x=(mesh->box.min.x+mesh->box.mid.x)/2;
	obscure_calculate_mesh_single_view(mesh_idx,&cpt,stencil_pixels,mesh_sort_list,obscure_mesh_view_slop_angle,270.0f);
	obscure_calculate_mesh_single_view(mesh_idx,&cpt,stencil_pixels,mesh_sort_list,0.0f,270.0f);
	obscure_calculate_mesh_single_view(mesh_idx,&cpt,stencil_pixels,mesh_sort_list,-obscure_mesh_view_slop_angle,270.0f);

		// disable some global setup

	glDisable(GL_STENCIL_TEST);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

		// free memory

	free(mesh_sort_list);
	free(stencil_pixels);
	
	return(TRUE);
}

/* =======================================================

      Obscure Groups
      
======================================================= */

void obscure_calculate_group_for_mesh(int mesh_idx,int group_idx)
{
	int						n,min,max;
	float					fx,fy,fz;
	bool					ok;
	map_mesh_type			*mesh,*chk_mesh;
	
	mesh=&map.mesh.meshes[mesh_idx];
	mesh->obscure.group_idx=group_idx;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
	
		if (n==mesh_idx) continue;
		
		chk_mesh=&map.mesh.meshes[n];
		
			// find the % that this mesh is inside the other mesh
			
		fx=fy=fz=0.0f;
		
		min=chk_mesh->box.min.x;
		if (mesh->box.min.x>min) min=mesh->box.min.x;
		max=chk_mesh->box.max.x;
		if (mesh->box.max.x<max) max=mesh->box.max.x;
		
		if (min<max) fx=(float)(max-min)/(chk_mesh->box.max.x-chk_mesh->box.min.x);

		min=chk_mesh->box.min.y;
		if (mesh->box.min.y>min) min=mesh->box.min.y;
		max=chk_mesh->box.max.y;
		if (mesh->box.max.y<max) max=mesh->box.max.y;
		
		if (min<max) fy=(float)(max-min)/(chk_mesh->box.max.y-chk_mesh->box.min.y);

		min=chk_mesh->box.min.z;
		if (mesh->box.min.z>min) min=mesh->box.min.z;
		max=chk_mesh->box.max.z;
		if (mesh->box.max.z<max) max=mesh->box.max.z;
		
		if (min<max) fz=(float)(max-min)/(chk_mesh->box.max.z-chk_mesh->box.min.z);

			// consider it the same if at least two
			// axises are within 90%
			
		ok=((fx>0.7f) && (fz>0.7f) && (abs(chk_mesh->box.mid.y-mesh->box.mid.y)<obscure_mesh_view_min_distance));
		
		if (ok) chk_mesh->obscure.group_idx=group_idx;
	}
}

void obscure_calculate_groups(void)
{
	int				n,group_idx;
	
		// clear all groups
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		map.mesh.meshes[n].obscure.group_idx=-1;
	}

		// put meshes into like groups
		// to share obscure visibility lists
		
	group_idx=0;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (map.mesh.meshes[n].obscure.group_idx==-1) {
			obscure_calculate_group_for_mesh(n,group_idx);
			group_idx++;
		}
	}
}

/* =======================================================

      Obscure Map
      
======================================================= */

bool obscure_calculate_map(void)
{
	int				n,tick;
	
//	tick=TickCount();
// supergumba -- testing	
		// setup mesh boxes
		
	map_prepare(&map);
	
		// combine meshes into like groups
		
	obscure_calculate_groups();
	
		// check visibility for all meshes
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (!obscure_calculate_mesh(n)) return(FALSE);
	}
	
//	fprintf(stdout,"tick = %d\n",(int)(TickCount()-tick));
// supergumba -- testing	
	return(TRUE);
}

/* =======================================================

      Obscure Test
      
======================================================= */

bool obscure_test(void)
{
	int				n,type,mesh_idx,poly_idx,group_idx;
	
		// if obscuring already on, turn off
		
	if (obscure_mesh_idx!=-1) {
		obscure_mesh_idx=-1;
		return(FALSE);
	}
	
		// get select
		
	select_get(0,&type,&mesh_idx,&poly_idx);
	if (type!=mesh_piece) return(FALSE);
		
		// setup mesh boxes
		
	map_prepare(&map);

		// combine meshes into like groups
		// and change select to represent
		// the grouping
/* supergumba -- do later		
	obscure_calculate_groups();
	
	group_idx=map.mesh.meshes[mesh_idx].obscure.group_idx;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (map.mesh.meshes[n].obscure.group_idx==group_idx) select_add(mesh_piece,n,0);
	}
*/
		// setup obscuring
	
	obscure_mesh_idx=mesh_idx;
	
	if (!obscure_calculate_mesh(obscure_mesh_idx)) {
		obscure_mesh_idx=-1;
		return(FALSE);
	}
	
	return(TRUE);
}

void obscure_reset(void)
{
	int				type,mesh_idx,poly_idx;
	bool			sel_ok;
	
		// if obscure is on and mesh changed, then
		// reset obscure
		
	if (obscure_mesh_idx==-1) return;
	
	sel_ok=FALSE;
	
	if (select_count()!=0) {
		select_get(0,&type,&mesh_idx,&poly_idx);
		sel_ok=(type==mesh_piece);
	}
	
	if (!sel_ok) {
		obscure_mesh_idx=-1;
		main_wind_obscure_tool_reset();
		return;
	}
	
	if (mesh_idx==obscure_mesh_idx) return;
	
		// run the obscure
		
	obscure_mesh_idx=-1;
	obscure_test();

		// necessary redraws
		
	main_wind_obscure_tool_reset();
	main_wind_draw();
}


