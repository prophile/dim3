/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Portal Vertex Lists

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

#include "lights.h"

extern map_type			map;
extern view_type		view;
extern setup_type		setup;

GLuint					vertex_vbo=-1;		// supergumba

/* =======================================================

      Compile Portal OpenGL Lists
      
======================================================= */


// supergumba -- do once
// need per mesh update?

extern int						nlight;
extern light_spot_type			lspot_cache[max_light_spot];


void map_calculate_light_color_normal(double x,double y,double z,float *cf,float *nf)
{
	int					i,cnt;
	double				dx,dz,dy,r,g,b,nx,nz,ny,d,mult;
	light_spot_type		*lspot;

		// combine all light spot normals
		// attenuated for distance
		
	cnt=0;
	
	r=g=b=0;
	nx=ny=nz=0;
	
	lspot=lspot_cache;
	
	for (i=0;i!=nlight;i++) {
		
		dx=lspot->d_x-x;
		dy=lspot->d_y-y;
		dz=lspot->d_z-z;
		
		d=(dx*dx)+(dz*dz)+(dy*dy);
		if (d<=lspot->d_intensity) {
			mult=(lspot->d_intensity-d)*lspot->d_inv_intensity;

			r+=(lspot->d_col_r*mult);
			g+=(lspot->d_col_g*mult);
			b+=(lspot->d_col_b*mult);

			nx+=(dx*mult);
			ny+=(dy*mult);
			nz+=(dz*mult);

			cnt++;
		}
		
		lspot++;
	}

		// set light value

	*cf++=(map.ambient.light_color.r+setup.gamma)+(float)r;
	*cf++=(map.ambient.light_color.g+setup.gamma)+(float)g;
	*cf=(map.ambient.light_color.b+setup.gamma)+(float)b;
	
		// no hits, then default normal

	if (cnt==0) {
		*nf++=0.5f;
		*nf++=0.5f;
		*nf=1.0f;
		return;
	}
	
		// average the normal vector
		
	d=(1.0/((double)cnt));
	nx*=d;
	ny*=d;
	nz*=d;
	
		// combine x and z together to make x
		// factor.  Note that this does not always
		// give the correct results but will be close
		// most of the time -- otherwise we're going to
		// have to calculate polygon vectors
		
	nx+=nz;

		// normalize normal vector

	d=sqrt((nx*nx)+(ny*ny));
	if (d!=0.0) {
		d=1.0/d;
		nx*=d;
		ny*=d;
	}
	
		// convert to needed format
		// x (1 = right [light from left], 0 = left [light from right])
		// y (1 = top [light from bottom], 0 = bottom [light from top])

		// supergumba -- possible (check math) that in the future
		// we can use the z coord for a distance hardness factor
		
	*nf++=(float)((nx*0.5)+0.5);
	*nf++=1.0f-(float)((ny*0.5)+0.5);
	*nf=1.0f;
}



void portal_compile_gl_lists(int tick,int rn)
{



	int						n,nvlist;
	float					fx,fy,fz;
	float					*pv,*pp,*pc,*pn;
	portal_vertex_list_type	*vl;

		// the arrays

	vl=map.vertexes.vertex_list;
	nvlist=map.vertexes.nvlist;

	pv=(float*)map.vertexes.pvert;
	pp=(float*)map.vertexes.pcoord;
	pc=(float*)map.vertexes.pcolor;
	pn=(float*)map.vertexes.pnormal;

		// the eye offset

	fx=(float)view.camera.pos.x;
	fy=(float)view.camera.pos.y;
	fz=(float)view.camera.pos.z;

		// regular lighting

	vl=map.vertexes.vertex_list;

	for (n=0;n!=nvlist;n++) {
		map_calculate_light_color_normal((double)vl->x,(double)vl->y,(double)vl->z,pc,pn);
		pc+=3;
	//	pn+=3;
		
		*pn++=0.5f;
		*pn++=0.5f;
		*pn=1.0f;


		*pv++=(vl->x-fx);
		*pv++=(vl->y-fy);
		*pv++=(fz-vl->z);
		*pp++=vl->gx;
		*pp++=vl->gy;

		vl++;
	}





/*
	int						n,nvlist;
	float					fx,fy,fz;
	float					*pv,*pp,*pc,*pn;
	portal_type				*portal;
	portal_vertex_list_type	*vl;
	
	portal=&map.portals[rn];

		// the arrays

	vl=portal->vertexes.vertex_list;
	nvlist=portal->vertexes.nvlist;

	pv=(float*)portal->vertexes.pvert;
	pp=(float*)portal->vertexes.pcoord;
	pc=(float*)portal->vertexes.pcolor;
	pn=(float*)portal->vertexes.pnormal;

		// the eye offset

	fx=(float)view.camera.pos.x;
	fy=(float)view.camera.pos.y;
	fz=(float)view.camera.pos.z;

		// check if lights or vertex data has changed in
		// room.  If not, only run through the vertexes
		// instead of recreating the lights

	if (!map_portal_light_check_changes(portal)) {

		vl=portal->vertexes.vertex_list;

		for (n=0;n!=nvlist;n++) {
			*pv++=(vl->x-fx);
			*pv++=(vl->y-fy);
			*pv++=(fz-vl->z);
			*pp++=vl->gx;
			*pp++=vl->gy;

			vl++;
		}
		
		return;
	}

		// ray-traced lighting
		
	if (setup.ray_trace_lighting) {

		vl=portal->vertexes.vertex_list;

		if (setup.bump_mapping) {

				// ray traced with bump normals

			for (n=0;n!=nvlist;n++) {
				light_trace_calculate_light_color(portal,vl->x,vl->y,vl->z,pc);
				pc+=3;

				*pv++=(vl->x-fx);
				*pv++=(vl->y-fy);
				*pv++=(fz-vl->z);
				*pp++=vl->gx;
				*pp++=vl->gy;
				
				map_portal_calculate_light_normal(portal,vl->x,vl->y,vl->z,pn);
				pn+=3;

				vl++;
			}

		}

		else {

				// ray trace with no bumps

			for (n=0;n!=nvlist;n++) {
				light_trace_calculate_light_color(portal,vl->x,vl->y,vl->z,pc);
				pc+=3;

				*pv++=(vl->x-fx);
				*pv++=(vl->y-fy);
				*pv++=(fz-vl->z);
				*pp++=vl->gx;
				*pp++=vl->gy;

				vl++;
			}
		}

		return;
	}

		// regular lighting

	vl=portal->vertexes.vertex_list;

	if (setup.bump_mapping) {

			// regular lighting with bumps

		for (n=0;n!=nvlist;n++) {
			map_portal_calculate_light_color_normal(portal,(double)vl->x,(double)vl->y,(double)vl->z,pc,pn);
			pc+=3;
			pn+=3;

			*pv++=(vl->x-fx);
			*pv++=(vl->y-fy);
			*pv++=(fz-vl->z);
			*pp++=vl->gx;
			*pp++=vl->gy;

			vl++;
		}

	}

	else {

			// regular lighting with no bumps

		for (n=0;n!=nvlist;n++) {
			map_portal_calculate_light_color(portal,(double)vl->x,(double)vl->y,(double)vl->z,pc);
			pc+=3;

			*pv++=(vl->x-fx);
			*pv++=(vl->y-fy);
			*pv++=(fz-vl->z);
			*pp++=vl->gx;
			*pp++=vl->gy;

			vl++;
		}

	}
	*/
}

/* =======================================================

      Use Portal OpenGL Lists
      
======================================================= */

void portal_compile_gl_list_attach(int rn)
{

	int						nvlist,sz;

	nvlist=map.vertexes.nvlist;
	
	sz=(nvlist*3)*sizeof(float);

		// vertexes
#ifdef D3_OS_MAC
	glVertexArrayRangeAPPLE(sz,map.vertexes.pvert);
	glEnableClientState(GL_VERTEX_ARRAY_RANGE_APPLE);
#endif

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,map.vertexes.pvert);

		// uvs
		
	glClientActiveTexture(GL_TEXTURE2);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,map.vertexes.pcoord);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,map.vertexes.pcoord);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,map.vertexes.pcoord);

		// color array

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,map.vertexes.pcolor);

#ifdef D3_OS_MAC
	glFlushVertexArrayRangeAPPLE(sz,map.vertexes.pvert);
#endif




/* supergumba
	int						nvlist,sz;
	portal_type				*portal;
	
	portal=&map.portals[rn];
	nvlist=portal->vertexes.nvlist;
	
	sz=(nvlist*3)*sizeof(float);

		// vertexes
#ifdef D3_OS_MAC
	glVertexArrayRangeAPPLE(sz,portal->vertexes.pvert);
	glEnableClientState(GL_VERTEX_ARRAY_RANGE_APPLE);
#endif

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,portal->vertexes.pvert);

		// uvs
		
	glClientActiveTexture(GL_TEXTURE2);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,portal->vertexes.pcoord);

	glClientActiveTexture(GL_TEXTURE1);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,portal->vertexes.pcoord);
	
	glClientActiveTexture(GL_TEXTURE0);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2,GL_FLOAT,0,portal->vertexes.pcoord);

		// color array

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3,GL_FLOAT,0,portal->vertexes.pcolor);

#ifdef D3_OS_MAC
	glFlushVertexArrayRangeAPPLE(sz,portal->vertexes.pvert);
#endif
*/


/*

	if (vertex_vbo==-1) {
		glGenBuffersARB(1,&vertex_vbo);
	}
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,vertex_vbo);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,sz,portal->vertexes.pvert,GL_STREAM_DRAW_ARB);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,0);

*/
}

void portal_compile_gl_list_dettach(void)
{
	glDisableClientState(GL_COLOR_ARRAY);

	glClientActiveTexture(GL_TEXTURE2);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE1);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_VERTEX_ARRAY);
	
#ifdef D3_OS_MAC
	glDisableClientState(GL_VERTEX_ARRAY_RANGE_APPLE);
#endif

/*
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

*/
}
