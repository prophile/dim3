/****************************** File *********************************

Module: dim3 Model Utility
Author: Brian Barnes
 Usage: Normal Routines

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

      Calculate Normals for Model
      
======================================================= */

float model_find_normal_angle(float nx1,float nz1,float ny1,float nx2,float nz2,float ny2)
{
    float			f,f2,f3;
    
    f=(nx1*nx2)+(nz1*nz2)+(ny1*ny2);
    f2=sqrtf((nx1*nx1)+(nz1*nz1)+(ny1*ny1));
    f3=sqrtf((nx2*nx2)+(nz2*nz2)+(ny2*ny2));
        
	return((float)acos((double)(f/(f2*f3)))*(180.0f/TRIG_PI));
}

void model_recalc_normals(model_type *model,int mesh_idx)
{
	int					i,k,nt,ntrig,cnt,
                        x[3],z[3],y[3];
    float				f,fx,fz,fy;
	d3vct				v1,v2,v[max_model_trig];
	model_mesh_type		*mesh;
    model_vertex_type	*vertex;
	model_trig_type		*trig;

	mesh=&model->meshes[mesh_idx];
	
        // find polygon normals
        
	ntrig=mesh->ntrig;
	
	for (i=0;i!=ntrig;i++) {
        trig=&mesh->trigs[i];
    
            // get the vertexes
        
        for (k=0;k!=3;k++) {
            vertex=&mesh->vertexes[trig->v[k]];
            x[k]=vertex->pnt.x;
            z[k]=vertex->pnt.z;
            y[k]=vertex->pnt.y;
        }
		
			// get the vectors cross product
			
		vector_create(&v1,x[1],y[1],z[1],x[0],y[0],z[0]);
		vector_create(&v2,x[2],y[2],z[2],x[0],y[0],z[0]);
		
		vector_cross_product(&v[i],&v1,&v2);
	}
    
		// average polygon normals into vertexes
		
	nt=mesh->nvertex;
	vertex=mesh->vertexes;

	for (i=0;i!=nt;i++) {
	
			// find all trigs that share this vertex
		
		cnt=0;
		trig=mesh->trigs;
		
		fx=fz=fy=0;
		
		for (k=0;k!=ntrig;k++) {
			if ((trig->v[0]==i) || (trig->v[1]==i) || (trig->v[2]==i)) {
                fx+=v[k].x;
                fz+=v[k].z;
                fy+=v[k].y;
                cnt++;
			}
			trig++;
		}
		
		if (cnt==0) {
			vertex->normal.x=vertex->normal.z=vertex->normal.y=0.0f;
		}
		else {
			f=(float)cnt;
			vertex->normal.x=-(fx/f);
			vertex->normal.z=-(fz/f);
			vertex->normal.y=-(fy/f);
		}
		
		vertex++;
	}
}

