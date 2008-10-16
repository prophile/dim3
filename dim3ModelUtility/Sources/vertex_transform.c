/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Model Vertex Transformations

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
	#include "dim3modelutility.h"
#endif

/* =======================================================

      Vertex Center
      
======================================================= */

void model_find_center_sel_vertexes(model_type *model,int mesh_idx,int *p_cx,int *p_cy,int *p_cz)
{
	int					i,nt,cx,cz,cy,cnt;
	model_vertex_type	*vertex;
	
	nt=model->meshes[mesh_idx].nvertex;
	vertex=model->meshes[mesh_idx].vertexes;
	
	cx=cy=cz=0;
	cnt=0;
	
	for (i=0;i!=nt;i++) {
		if (model_check_sel_mask(model,mesh_idx,i)) {
			cx+=vertex->pnt.x;
			cy+=vertex->pnt.y;
			cz+=vertex->pnt.z;
			cnt++;
		}
		vertex++;
	}
	
	*p_cx=cx/cnt;
	*p_cy=cy/cnt;
	*p_cz=cz/cnt;
}

/* =======================================================

      Move Vertexes
      
======================================================= */

void model_move_sel_vertexes(model_type *model,int mesh_idx,int x,int y,int z)
{
	int					i,nt;
	model_vertex_type	*vertex;
	
	nt=model->meshes[mesh_idx].nvertex;
		
	vertex=model->meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nt;i++) {
		if (model_check_sel_mask(model,mesh_idx,i)) {
			vertex->pnt.x+=x;
			vertex->pnt.y+=y;
			vertex->pnt.z+=z;
		}
		vertex++;
	}
}

/* =======================================================

      Scale Vertexes
      
======================================================= */

void model_scale_sel_vertexes(model_type *model,int mesh_idx,float x,float y,float z)
{
	int					i,nt,
						x2,z2,y2,cx,cz,cy;
	model_vertex_type	*vertex;
	
		// find vertex center
		
	model_find_center_sel_vertexes(model,mesh_idx,&cx,&cy,&cz);
	
		// scale vertexes

	nt=model->meshes[mesh_idx].nvertex;
	vertex=model->meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nt;i++) {
		if (model_check_sel_mask(model,mesh_idx,i)) {
			x2=vertex->pnt.x-cx;
			y2=vertex->pnt.y-cy;
			z2=vertex->pnt.z-cz;
			x2=(int)((float)x2*x);
			y2=(int)((float)y2*y);
			z2=(int)((float)z2*z);
			vertex->pnt.x=x2+cx;
			vertex->pnt.y=y2+cy;
			vertex->pnt.z=z2+cz;
		}
		vertex++;
	}
}

/* =======================================================

      Rotate Vertexes
      
======================================================= */

void model_rotate_sel_vertexes(model_type *model,int mesh_idx,float ang_x,float ang_y,float ang_z)
{
	int					i,nt,cx,cz,cy;
	model_vertex_type	*vertex;
	
		// find vertex center
		
	model_find_center_sel_vertexes(model,mesh_idx,&cx,&cy,&cz);
	
		// rotate vertexes

	nt=model->meshes[mesh_idx].nvertex;
	vertex=model->meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nt;i++) {
		if (model_check_sel_mask(model,mesh_idx,i)) {
			rotate_point(&vertex->pnt.x,&vertex->pnt.y,&vertex->pnt.z,cx,cy,cz,ang_x,ang_y,ang_z);
		}
		vertex++;
	}
}

/* =======================================================

      Invert Normal Vertexes
      
======================================================= */

void model_invert_normal_sel_vertexes(model_type *model,int mesh_idx)
{
	int					i,nt;
	model_vertex_type	*vertex;
	
	nt=model->meshes[mesh_idx].nvertex;
	vertex=model->meshes[mesh_idx].vertexes;
	
	for (i=0;i!=nt;i++) {
		if (model_check_sel_mask(model,mesh_idx,i)) {
			vertex->normal.x=-vertex->normal.x;
			vertex->normal.y=-vertex->normal.y;
			vertex->normal.z=-vertex->normal.z;
		}
		vertex++;
	}
}

/* =======================================================

      Remove Vertexes
      
======================================================= */

void model_delete_sel_vertex(model_type *model,int mesh_idx)
{
	int					i,n,k,j,
						trig_start,trig_end,sz;
	char				vertex_sel[max_model_vertex];
	bool				hit;

		// convert sels to boolean
		
	for (i=0;i!=model->meshes[mesh_idx].nvertex;i++) {
		vertex_sel[i]=(char)model_check_sel_mask(model,mesh_idx,i);
	}
	
		// delete all vertexes
		
	i=0;
	while (i<model->meshes[mesh_idx].nvertex) {
	
		if (vertex_sel[i]==0) {
			i++;
			continue;
		}
		
			// delete the vertex
			
		sz=((model->meshes[mesh_idx].nvertex-1)-i);
		
		if (sz>0) {
			memmove(&model->meshes[mesh_idx].vertexes[i],&model->meshes[mesh_idx].vertexes[i+1],(sz*sizeof(model_vertex_type)));
			memmove(&vertex_sel[i],&vertex_sel[i+1],sz);
		}
		
		model->meshes[mesh_idx].nvertex--;
		
			// delete all trigs with vertex
			
		n=0;
		
		while (n<model->meshes[mesh_idx].ntrig) {
		
			hit=FALSE;
			
			for (j=0;j!=3;j++) {
				hit=hit||(model->meshes[mesh_idx].trigs[n].v[j]==i);
				if (model->meshes[mesh_idx].trigs[n].v[j]>i) model->meshes[mesh_idx].trigs[n].v[j]--;
			}
		
			if (!hit) {
				n++;
				continue;
			}
			
				// delete trig
					
			sz=((model->meshes[mesh_idx].ntrig-1)-n);
			
			if (sz>0) {
				memmove(&model->meshes[mesh_idx].trigs[n],&model->meshes[mesh_idx].trigs[n+1],(sz*sizeof(model_trig_type)));
			}
				
			model->meshes[mesh_idx].ntrig--;
		
				// move texture trig starts
					
			for (k=0;k!=max_model_texture;k++) {
			
				if (model->textures[k].bitmaps[0].gl_id!=-1) {
				
					trig_start=model->meshes[mesh_idx].materials[k].trig_start;
					trig_end=trig_start+model->meshes[mesh_idx].materials[k].trig_count;
					
					if (trig_start>n) {
						model->meshes[mesh_idx].materials[k].trig_start--;
					}
					else {
						if ((n>=trig_start) && (n<trig_end)) {
							model->meshes[mesh_idx].materials[k].trig_count--;
						}
					}
				
				}
				
			}
		}
	}
	
		// clear sels
		
	model_clear_sel_mask(model,mesh_idx);
	model_clear_hide_mask(model,mesh_idx);
}

/* =======================================================

      Delete Unused Vertexes
      
======================================================= */

void model_delete_unused_vertexes(model_type *model,int mesh_idx)
{
	int					n,k,i,t,nvertex,ntrig,sz;
	unsigned char		*v_ok;
    model_trig_type		*trig;
	
		// vertex hit list
		
	v_ok=(unsigned char*)valloc(max_model_vertex);
	bzero(v_ok,max_model_vertex);
	
		// find vertexes hit
		
	ntrig=model->meshes[mesh_idx].ntrig;
	trig=model->meshes[mesh_idx].trigs;
	
	for (n=0;n!=ntrig;n++) {
		for (k=0;k!=3;k++) {
			v_ok[trig->v[k]]=0x1;
		}
		trig++;
	}
	
		// delete unused vertexes
		
	nvertex=model->meshes[mesh_idx].nvertex;
	
	for (n=(nvertex-1);n>=0;n--) {
	
		if (v_ok[n]==0x1) continue;
		
			// change all trigs vertex pointers
	
		trig=model->meshes[mesh_idx].trigs;
		
		for (i=0;i!=ntrig;i++) {
			for (t=0;t!=3;t++) {
				if (trig->v[t]>n) trig->v[t]--;
			}
			trig++;
		}
		
			// delete vertex
			
		sz=(nvertex-n)*sizeof(model_vertex_type);
		if (sz>0) {
			memmove(&model->meshes[mesh_idx].vertexes[n],&model->meshes[mesh_idx].vertexes[n+1],sz);
			
			sz=nvertex-n;
			memmove(&v_ok[n],&v_ok[n+1],sz);
		}
		
		nvertex--;
	}
	
	model->meshes[mesh_idx].nvertex=nvertex;
}

