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

extern int					obscure_mesh_idx;
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
#define obscure_mesh_view_grid_size				(8*map_enlarge)
#define obscure_mesh_view_grid_push				(2*map_enlarge)
#define obscure_mesh_rough_radius				100000

typedef struct		{
						int									stencil_idx;
						bool								mesh_in_group[max_mesh];
					} obscure_group_type;

int												nobscure_group;
obscure_group_type								*obscure_groups;

// #define OBSCURE_TEST		1

/* =======================================================

      Obscure Bits
      
======================================================= */

inline void obscure_mesh_view_bit_set(unsigned char *visibility_flag,int idx)
{
	visibility_flag[idx>>3]=visibility_flag[idx>>3]|(0x1<<(idx&0x7));
}

inline bool obscure_mesh_view_bit_get(unsigned char *visibility_flag,int idx)
{
	return((visibility_flag[idx>>3]&(0x1<<(idx&0x7)))!=0x0);
}

void obscure_group_view_bit_set(unsigned char *visibility_flag,int group_idx)
{
	int				n;
	map_mesh_type	*mesh;
	
	mesh=map.mesh.meshes;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (obscure_groups[group_idx].mesh_in_group[n]) obscure_mesh_view_bit_set(visibility_flag,n);
		mesh++;
	}
}

int obscure_group_find_for_mesh(int mesh_idx)
{
	int				n;
	
	for (n=0;n!=nobscure_group;n++) {
		if (obscure_groups[n].mesh_in_group[mesh_idx]) return(n);
	}
	
	return(-1);
}

/* =======================================================

      Find Size for Obscure Group
      
======================================================= */

bool obscure_calculate_group_min_max(int group_idx,d3pnt *min,d3pnt *max)
{
	int					n,k,cnt;
	d3pnt				mid;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=map.mesh.meshes;
	
	cnt=0;
	
	min->x=max->x=-1;
	min->y=max->y=-1;
	min->z=max->z=-1;
	
	mid.x=mid.y=mid.z=0;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
	
		if (obscure_groups[group_idx].mesh_in_group[n]) {
		
				// always remember mid point if there is
				// a problem getting other coordinates
				
			mid.x+=mesh->box.mid.x;
			mid.y+=mesh->box.mid.y;
			mid.z+=mesh->box.mid.z;
			cnt++;
			
				// run through the polys
				
			poly=mesh->polys;
				
			for (k=0;k!=mesh->npoly;k++) {
			
					// only count walls in x/z min/maxes
					
				if (poly->box.wall_like) {
				
					if ((min->x==-1) && (max->x==-1)) {
						min->x=poly->box.min.x;
						max->x=poly->box.max.x;
						min->z=poly->box.min.z;
						max->z=poly->box.max.z;
					}
					else {
						if (poly->box.min.x<min->x) min->x=poly->box.min.x;
						if (poly->box.min.z<min->z) min->z=poly->box.min.z;
						if (poly->box.max.x>max->x) max->x=poly->box.max.x;
						if (poly->box.max.z>max->z) max->z=poly->box.max.z;
					}
					
				}
				
					// only count non-walls in y min/maxes
					
				else {
				
					if ((min->y==-1) && (max->y==-1)) {
						min->y=poly->box.min.y;
						max->y=poly->box.max.y;
					}
					else {
						if (poly->box.min.y<min->y) min->y=poly->box.min.y;
						if (poly->box.max.y>max->y) max->y=poly->box.max.y;
					}
					
				}
				
				poly++;
			}
		}
	
		mesh++;
	}
	
		// any meshes?
		
	if (cnt==0) return(FALSE);
	
		// any missed coordinates?
		
	if ((min->x==-1) && (max->x==-1)) min->x=max->x=(mid.x/cnt);
	if ((min->y==-1) && (max->y==-1)) min->y=max->y=(mid.y/cnt);
	if ((min->z==-1) && (max->z==-1)) min->z=max->z=(mid.z/cnt);
		
	return(TRUE);
}

/* =======================================================

      Obscure Mesh Sorting
      
======================================================= */

int obscure_mesh_sort(d3pnt *pt,int *mesh_sort_list)
{
	int					n,t,sz,d,cnt,idx;
	int					*dist;
	map_mesh_type		*mesh;

	dist=malloc(max_mesh*sizeof(int));

	cnt=0;

	for (n=0;n!=map.mesh.nmesh;n++) {

		mesh=&map.mesh.meshes[n];

			// ignore meshes outside of draw distance

		d=map_mesh_calculate_distance(mesh,pt);
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

bool obscure_calculate_meshes_close(d3pnt *cpt,map_mesh_type *view_mesh)
{
	int			n,dist;
	d3pnt		pt;
	
	for (n=0;n!=7;n++) {
		obscure_calculate_meshes_close_single_point(n,view_mesh,&pt);

		dist=distance_get(pt.x,pt.y,pt.z,cpt->x,cpt->y,cpt->z);
		if (dist<obscure_mesh_view_min_distance) return(TRUE);
	}

	return(FALSE);
}

/* =======================================================

      Complete Obscure
      
======================================================= */

void obscure_calculate_group_single_visibility(int group_idx,d3pnt *cpt,unsigned char *visibility_flag,unsigned char *stencil_pixels,int *mesh_sort_list,float x_rot,float y_rot)
{
	int						i,n,k,t,mesh_cnt,stencil_idx,
							view_mesh_idx,idx;
	bool					transparent;
	unsigned char			*sp;
	unsigned long			cur_gl_id;
	d3pnt					*pt;
	map_mesh_type			*view_mesh;
	map_mesh_poly_type		*poly;
	texture_type			*texture;
	int						vport[4];
	double					mod_matrix[16],proj_matrix[16];
	
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

		// run through all other mesh groups to test
		// their visibility against this mesh group

		// there is a potential problem here with the
		// number of groups going over 255 (the total depth
		// of the stencil buffer).  So far I haven't encounted
		// a map with this problem, but we might need to do
		// multiple passes in the future
		
	glClear(GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		
#ifdef OBSCURE_TEST
	glClearColor(1.0f,1.0f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
#endif
	
	stencil_idx=1;

	for (i=0;i!=nobscure_group;i++) {
	
		obscure_groups[i].stencil_idx=-1;
	
		for (n=0;n!=mesh_cnt;n++) {
			
				// get the draw mesh
			
			view_mesh_idx=mesh_sort_list[n];
			view_mesh=&map.mesh.meshes[view_mesh_idx];
		
				// only draw all meshes in this group
				
			if (!obscure_groups[i].mesh_in_group[view_mesh_idx]) continue;
			
				// don't draw meshes that are in the testing
				// group and have no self-obscure
				
			if (obscure_groups[group_idx].mesh_in_group[view_mesh_idx]) {
				if (view_mesh->flag.no_self_obscure) continue;
			}
			
				// meshes within a certain min distance
				// are always included
				
			if (obscure_calculate_meshes_close(cpt,view_mesh)) obscure_mesh_view_bit_set(visibility_flag,view_mesh_idx);
		
				// ignore all moving meshes as they
				// won't always obscure

			if (view_mesh->flag.moveable) {
				obscure_mesh_view_bit_set(visibility_flag,view_mesh_idx);
				continue;
			}

				// don't draw meshes clipped from view

			if (!obscure_check_mesh_view_clip(&view_mesh->box.min,&view_mesh->box.max,cpt,vport,mod_matrix,proj_matrix)) continue;

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
				
					// do we need to start a stencil for a group?
					
				if (obscure_groups[i].stencil_idx==-1) {
					obscure_groups[i].stencil_idx=stencil_idx;
					glStencilFunc(GL_ALWAYS,stencil_idx,0xFF);
		
					stencil_idx++;
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
	}
	
	glDisable(GL_TEXTURE_2D);
	
#ifdef OBSCURE_TEST
	aglSwapBuffers(ctx);
	while (Button()) {}
	while (!Button()) {}
#endif
		// read the stencil to look for hits

	glFinish();
	glReadPixels(0,0,obscure_mesh_view_mesh_wid,obscure_mesh_view_mesh_high,GL_STENCIL_INDEX,GL_UNSIGNED_BYTE,stencil_pixels);

	sp=stencil_pixels;

	for (k=0;k!=(obscure_mesh_view_mesh_wid*obscure_mesh_view_mesh_high);k++) {
	
		stencil_idx=(int)*sp++;
		if (stencil_idx==0) continue;
		
			// find group for this stencil_idx
			
		idx=-1;
		
		for (n=0;n!=nobscure_group;n++) {
		
			if (obscure_groups[n].stencil_idx==stencil_idx) {
				idx=n;
				break;
			}
		
			if (idx!=-1) obscure_group_view_bit_set(visibility_flag,idx);
		}
	}
}

bool obscure_calculate_group_visibility_complete(int group_idx)
{
	int				n,x,z,
					x_sz,y_sz,z_sz,x_cnt,y_cnt,z_cnt,x_add,y_add,z_add;
	int				*mesh_sort_list;
	float			ratio;
	unsigned char	*stencil_pixels;
	unsigned char	visibility_flag[max_mesh_visibility_bytes];
	d3pnt			min,max,pnt;
	map_mesh_type	*mesh;
	
		// get the min/max for group of meshes
		
	if (!obscure_calculate_group_min_max(group_idx,&min,&max)) return(FALSE);

		// pixels for reading stencil buffer
		
	stencil_pixels=malloc(obscure_mesh_view_mesh_wid*obscure_mesh_view_mesh_high);
	if (stencil_pixels==NULL) return(FALSE);
	
		// memory for sorted mesh list
		
	mesh_sort_list=malloc(sizeof(int)*max_mesh);
	if (mesh_sort_list==NULL) {
		free(stencil_pixels);
		return(FALSE);
	}

		// global drawing setup
		
	glDisable(GL_SCISSOR_TEST);

	glEnable(GL_SCISSOR_TEST);
	glScissor(0,0,obscure_mesh_view_mesh_wid,obscure_mesh_view_mesh_high);

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
		
	bzero(visibility_flag,max_mesh_visibility_bytes);

		// group meshes always in visibility list

	for (n=0;n!=map.mesh.nmesh;n++) {
		if (obscure_groups[group_idx].mesh_in_group[n]) obscure_mesh_view_bit_set(visibility_flag,n);
	}
	
		// divisions across the mesh

	x_sz=(max.x-min.x)-(obscure_mesh_view_grid_push<<1);
	x_cnt=x_sz/obscure_mesh_view_grid_size;
	if (x_cnt<2) x_cnt=2;
	x_add=(max.x-min.x)/x_cnt;
		
	y_sz=(max.y-min.y)-(obscure_mesh_view_grid_push<<1);
	y_cnt=y_sz/obscure_mesh_view_grid_size;
	if (y_cnt<2) y_cnt=2;
	y_add=(max.y-min.y)/y_cnt;
	
	z_sz=(max.z-min.z)-(obscure_mesh_view_grid_push<<1);
	z_cnt=z_sz/obscure_mesh_view_grid_size;
	if (z_cnt<2) z_cnt=2;
	z_add=(max.z-min.z)/z_cnt;
	
		// look across the mesh
		
//	for (y=0;y!=y_cnt;y++) {
	
//		if (y==(y_cnt-1)) {
//			pnt.y=max.y-obscure_mesh_view_grid_push;
//		}
//		else {
//			pnt.y=(min.y+obscure_mesh_view_grid_push)+(y*y_add);
//		}

// supergumba -- need flag for this
pnt.y=max.y-(map_enlarge*5);
		
		for (x=0;x!=x_cnt;x++) {
		
			if (x==(x_cnt-1)) {
				pnt.x=max.x-obscure_mesh_view_grid_push;
			}
			else {
				pnt.x=(min.x+obscure_mesh_view_grid_push)+(x*x_add);
			}
			
			for (z=0;z!=z_cnt;z++) {
			
				if (z==(z_cnt-1)) {
					pnt.z=max.z-obscure_mesh_view_grid_push;
				}
				else {
					pnt.z=(min.z+obscure_mesh_view_grid_push)+(z*z_add);
				}
		
				for (n=0;n!=8;n++) {
					obscure_calculate_group_single_visibility(group_idx,&pnt,visibility_flag,stencil_pixels,mesh_sort_list,0.0f,(float)(n*90));
					obscure_calculate_group_single_visibility(group_idx,&pnt,visibility_flag,stencil_pixels,mesh_sort_list,obscure_mesh_view_slop_angle,(float)(n*90));
					obscure_calculate_group_single_visibility(group_idx,&pnt,visibility_flag,stencil_pixels,mesh_sort_list,-obscure_mesh_view_slop_angle,(float)(n*90));
				}
			}
		}
//	}

		// disable some global setup

	glDisable(GL_STENCIL_TEST);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

		// free memory

	free(mesh_sort_list);
	free(stencil_pixels);
	
		// set the group
		
	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
		if (obscure_groups[group_idx].mesh_in_group[n]) memmove(mesh->obscure.visibility_flag,visibility_flag,max_mesh_visibility_bytes);
		mesh++;
	}

	return(TRUE);
}

/* =======================================================

      Rough Obscure
      
======================================================= */

bool obscure_calculate_group_visibility_rough(int group_idx)
{
	int				n;
	unsigned char	visibility_flag[max_mesh_visibility_bytes];
	bool			group_ok;
	d3pnt			min,max,mid,chk_min,chk_max,chk_mid;
	map_mesh_type	*mesh;
	
		// get the min/max for group of meshes
		
	if (!obscure_calculate_group_min_max(group_idx,&min,&max)) return(FALSE);
	
	mid.x=(min.x+max.x)>>1;
	mid.y=(min.y+max.y)>>1;
	mid.z=(min.z+max.z)>>1;
	
		// clear visibility bits
		
	bzero(visibility_flag,max_mesh_visibility_bytes);
	
		// find other groups within a certain radius
		// and set all the meshes within that group to be shown
		
	for (n=0;n!=nobscure_group;n++) {
	
		group_ok=FALSE;
		
		if (n==group_idx) {
			group_ok=TRUE;
		}
		else {
			if (obscure_calculate_group_min_max(n,&chk_min,&chk_max)) {
				chk_mid.x=(chk_min.x+chk_max.x)>>1;
				chk_mid.y=(chk_min.y+chk_max.y)>>1;
				chk_mid.z=(chk_min.z+chk_max.z)>>1;
		
				group_ok=(distance_get(mid.x,mid.y,mid.z,chk_mid.x,chk_mid.y,chk_mid.z)<=obscure_mesh_rough_radius);
			}
		}
		
		if (group_ok) obscure_group_view_bit_set(visibility_flag,n);
	}
	
		// set all members of the group to the shown meshes
		
	mesh=map.mesh.meshes;

	for (n=0;n!=map.mesh.nmesh;n++) {
		if (obscure_groups[group_idx].mesh_in_group[n]) memmove(mesh->obscure.visibility_flag,visibility_flag,max_mesh_visibility_bytes);
		mesh++;
	}

	return(TRUE);
}

/* =======================================================

      Obscure Calculate Main Line
      
======================================================= */

bool obscure_calculate_group_visibility(int group_idx)
{

// supergumba -- until complete is working, just do rough now

//	if (map.settings.obscure_type==obscure_type_rough) {
		return(obscure_calculate_group_visibility_rough(group_idx));
//	}
	
//	return(obscure_calculate_group_visibility_complete(group_idx));
}

/* =======================================================

      Obscure Groups
      
======================================================= */

void obscure_calculate_group_for_mesh(int mesh_idx,int group_idx,bool *mesh_hit)
{
	int						n,min,max;
	float					fx,fy,fz;
	bool					ok;
	map_mesh_type			*mesh,*chk_mesh;
	
	mesh=&map.mesh.meshes[mesh_idx];
	
	mesh_hit[mesh_idx]=TRUE;
	obscure_groups[group_idx].mesh_in_group[mesh_idx]=TRUE;
	
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
		
		if (ok) {
			mesh_hit[n]=TRUE;
			obscure_groups[group_idx].mesh_in_group[n]=TRUE;
		}
	}
}

bool obscure_groups_start(void)
{
	int					n,k;
	bool				*mesh_hit;
	
		// create groups
		
	nobscure_group=0;
	
	obscure_groups=malloc(map.mesh.nmesh*sizeof(obscure_group_type));
	if (obscure_groups==NULL) return(FALSE);
	
		// use list to tell what meshes got picked
		// up and processed with other meshes
		
	mesh_hit=malloc(map.mesh.nmesh*sizeof(bool));
	if (mesh_hit==NULL) {
		free(obscure_groups);
		return(FALSE);
	}
	
		// clear lists
		
	for (n=0;n!=map.mesh.nmesh;n++) {
		mesh_hit[n]=FALSE;

		for (k=0;k!=map.mesh.nmesh;k++) {
			obscure_groups[n].mesh_in_group[k]=FALSE;
		}
	}

		// put meshes into like groups
		// to share obscure visibility lists
		
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (!mesh_hit[n]) {
			obscure_calculate_group_for_mesh(n,nobscure_group,mesh_hit);
			nobscure_group++;
		}
	}
	
	free(mesh_hit);
	
	return(TRUE);
}

void obscure_groups_end(void)
{
	nobscure_group=0;
	free(obscure_groups);
}

/* =======================================================

      Obscure Map
      
======================================================= */

bool obscure_calculate_map(void)
{
	int				n;
	int			tick;
	
	tick=TickCount();
	
		// if no obscure on, ignore calculation
		
	if (map.settings.obscure_type==obscure_type_none) return(TRUE);

		// setup mesh boxes
		
	map_prepare(&map);
	
		// combine meshes into like groups
		
	if (!obscure_groups_start()) return(FALSE);
	
		// check visibility for all groups
		
	for (n=0;n!=nobscure_group;n++) {
		obscure_calculate_group_visibility(n);
	}
	
	obscure_groups_end();
	
	fprintf(stdout,"tick = %d\n",(int)(TickCount()-tick));

	return(TRUE);
}

/* =======================================================

      Obscure Test
      
======================================================= */

bool obscure_test(void)
{
	int				n,type,mesh_idx,poly_idx,group_idx;
	bool			ok;
	
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

	if (!obscure_groups_start()) return(FALSE);
	
	group_idx=obscure_group_find_for_mesh(mesh_idx);
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		if (obscure_groups[group_idx].mesh_in_group[n]) select_add(mesh_piece,n,0);
	}

		// setup obscuring
	
	obscure_mesh_idx=mesh_idx;
	
	ok=obscure_calculate_group_visibility(group_idx);
	obscure_groups_end();
	
	if (!ok) {
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


