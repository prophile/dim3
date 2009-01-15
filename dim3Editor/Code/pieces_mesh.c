/****************************** File *********************************

Module: dim3 Editor
Author: Brian Barnes
 Usage: Piece Mesh

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

#include "interface.h"
#include "dialog.h"
#include "common_view.h"

#define import_obj_float_to_int			1000.0f
#define import_obj_max_dimension		5000

extern int								drag_mode;
extern bool								dp_object,dp_node,dp_lightsoundparticle,dp_auto_texture;

extern map_type							map;

/* =======================================================

      Finish Mesh Addition
      
======================================================= */

void piece_add_mesh_finish(int mesh_idx)
{
	if (mesh_idx==-1) return;
	
		// make selection
		
	select_clear();
	select_add(mesh_piece,mesh_idx,0);

		// change mode to move entire mesh
		
	drag_mode=drag_mode_mesh;
	
	if (dp_auto_texture) {
		map_mesh_reset_uv(&map,mesh_idx);
	}
	else {
		map.mesh.meshes[mesh_idx].flag.lock_uv=TRUE;
	}
	
	main_wind_draw();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
}

/* =======================================================

      OBJ Replacementment
      
======================================================= */

void piece_add_obj_mesh_replace(d3fpnt *min,d3fpnt *max,d3fpnt *scale,d3pnt *pnt)
{
	int				n,type,mesh_idx,poly_idx,sel_cnt,hit_cnt;
	d3pnt			kmin,kmax,kpnt,pmin,pmax,ppnt;
	
	sel_cnt=select_count();
	if (sel_cnt==0) return;

		// sort segment so higher indexes are deleted first
		
	select_sort();

		// get size
		
	hit_cnt=0;
		
	for (n=0;n!=sel_cnt;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type!=mesh_piece) continue;
	
			// find farthest sizes
			
		map_mesh_calculate_extent(&map,mesh_idx,&pmin,&pmax);
		map_mesh_calculate_center(&map,mesh_idx,&ppnt);
		
		if (hit_cnt==0) {
			memmove(&kmin,&pmin,sizeof(d3pnt));
			memmove(&kmax,&pmax,sizeof(d3pnt));
			memmove(&kpnt,&ppnt,sizeof(d3pnt));
		}
		else {
			if (pmin.x<kmin.x) kmin.x=pmin.x;
			if (pmax.x>kmax.x) kmax.x=pmax.x;
			if (pmin.y<kmin.y) kmin.y=pmin.y;
			if (pmax.y>kmax.y) kmax.y=pmax.y;
			if (pmin.z<kmin.z) kmin.z=pmin.z;
			if (pmax.z>kmax.z) kmax.z=pmax.z;
			
			kpnt.x+=ppnt.x;
			kpnt.y+=ppnt.y;
			kpnt.z+=ppnt.z;
		}
		
		hit_cnt++;
	}
	
		// no meshes hit
		
	if (hit_cnt==0) return;
	
		// get center
		
	pnt->x=kpnt.x/hit_cnt;
	pnt->y=kpnt.y/hit_cnt;
	pnt->z=kpnt.z/hit_cnt;
		
		// get factor
		
	scale->x=(float)(kmax.x-kmin.x)/(float)(max->x-min->x);
	scale->y=(float)(kmax.y-kmin.y)/(float)(max->y-min->y);
	scale->z=(float)(kmax.z-kmin.z)/(float)(max->z-min->z);
	
		// delete old meshes
		
	for (n=0;n!=sel_cnt;n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==mesh_piece) map_mesh_delete(&map,mesh_idx);
	}
	
	select_clear();
}

/* =======================================================

      Add OBJ Mesh
      
======================================================= */

void piece_add_obj_mesh(void)
{
	int					n,k,nline,nvertex,npoly,nuv,npt,v_idx,uv_idx,
						mesh_idx,txt_idx,x,y,z;
	int					px[8],py[8],pz[8];
	char				*c,txt[256],vstr[256],uvstr[256],path[1024];
	float				fx,fy,fz,fsz,f_scale,gx[8],gy[8];
	float				*uvs,*uv;
	bool				replace,mesh_add;
	d3pnt				*vertexes,*dpt,pnt;
	d3fpnt				min,max,scale;
	map_mesh_type		*mesh;
	
	if (!piece_create_texture_ok()) return;
	
		// get import location

	piece_create_get_spot(&pnt);
	
		// import the file
		
	if (!import_load_file(path,"obj")) return;

	if (!textdecode_open(path,0x20)) {
		StandardAlert(kAlertStopAlert,"\pImport Failed","\pCould not open OBJ file.",NULL,NULL);
		return;
    }
	
    nline=textdecode_count();
	
		// count vertexes and faces
		// also determine the vertex extents for initial scaling size
		
	nvertex=0;
	npoly=0;
	nuv=0;
	
	min.x=min.y=min.z=0.0f;
	max.x=max.y=max.z=0.0f;
	
    for (n=0;n!=nline;n++) {
        textdecode_get_piece(n,0,txt);
        
        if (strcmp(txt,"v")==0) {
			nvertex++;
			
			textdecode_get_piece(n,1,txt);
			fx=strtod(txt,NULL);
			textdecode_get_piece(n,2,txt);
			fy=strtod(txt,NULL);
			textdecode_get_piece(n,3,txt);
			fz=strtod(txt,NULL);
			
			if (fx<min.x) min.x=fx;
			if (fx>max.x) max.x=fx;
			if (fy<min.y) min.y=fy;
			if (fy>max.y) max.y=fy;
			if (fz<min.z) min.z=fz;
			if (fz>max.z) max.z=fz;
		}
		else {
			if (strcmp(txt,"f")==0) {
				npoly++;
			}
			else {
				if (strcmp(txt,"vt")==0) {
					nuv++;
				}
			}
		}
	}
	
		// imporper OBJ errors
		
	if (nvertex==0) {
		textdecode_close();
		StandardAlert(kAlertStopAlert,"\pImport Failed","\pNo vertexes in OBJ.",NULL,NULL);
		return;
    }
	if (npoly==0) {
		textdecode_close();
		StandardAlert(kAlertStopAlert,"\pImport Failed","\pNo faces in OBJ.",NULL,NULL);
		return;
    }
	
		// get default scale
		
	fsz=max.x-min.x;
	if ((max.y-min.y)>fsz) fsz=max.y-min.y;
	if ((max.z-min.z)>fsz) fsz=max.z-min.z;
	
	f_scale=import_obj_max_dimension/fsz;
	k=(int)(f_scale*100.0f);
	f_scale=((float)k)/100.0f;

	replace=dialog_mesh_scale_run(&f_scale);
	
	scale.x=scale.y=scale.z=f_scale;
	
		// fix scale if a replace
		
	if (replace) piece_add_obj_mesh_replace(&min,&max,&scale,&pnt);
	
		// get texture index
		
	txt_idx=texture_palette_get_selected_texture();
	if (txt_idx==-1) txt_idx=0;
	
		// get the vertexes

	vertexes=(d3pnt*)malloc(sizeof(d3pnt)*nvertex);
	if (vertexes==NULL) {
		textdecode_close();
		StandardAlert(kAlertStopAlert,"\pImport Failed","\pOut of Memory.",NULL,NULL);
		return;
    }
	
 	dpt=vertexes;

    for (n=0;n!=nline;n++) {

        textdecode_get_piece(n,0,txt);
		if (strcmp(txt,"v")!=0) continue;
                
		textdecode_get_piece(n,1,txt);
		dpt->x=(int)(strtod(txt,NULL)*scale.x);
		
		textdecode_get_piece(n,2,txt);
		dpt->y=-(int)(strtod(txt,NULL)*scale.y);
		
		textdecode_get_piece(n,3,txt);
		dpt->z=(int)(strtod(txt,NULL)*scale.z);
        
		dpt++;
    }
	
		// recenter the vertexes
		
	x=y=z=0;
	
 	dpt=vertexes;
	
	for (n=0;n!=nvertex;n++) {
		x+=dpt->x;
		y+=dpt->y;
		z+=dpt->z;
		dpt++;
	}
	
	x/=nvertex;
	y/=nvertex;
	z/=nvertex;
		
	dpt=vertexes;
	
	for (n=0;n!=nvertex;n++) {
		dpt->x=(dpt->x-x)+pnt.x;
		dpt->y=(dpt->y-y)+pnt.y;
		dpt->z=(dpt->z-z)+pnt.z;
		dpt++;
	}
	
		// get the UVs
		
	if (nuv!=0) {
		uvs=(float*)malloc(sizeof(float)*(2*nuv));
		if (uvs==NULL) {
			textdecode_close();
			StandardAlert(kAlertStopAlert,"\pImport Failed","\pOut of Memory.",NULL,NULL);
			return;
		}

		uv=uvs;

		for (n=0;n!=nline;n++) {
			textdecode_get_piece(n,0,txt);
			if (strcmp(txt,"vt")!=0) continue;
					
			textdecode_get_piece(n,1,uvstr);
			*uv++=strtod(uvstr,NULL);
			textdecode_get_piece(n,2,uvstr);
			*uv++=1.0f-strtod(uvstr,NULL);
		}
	}

		// need to split up meshes by materials
		
	mesh=NULL;
	
		// get the polys

    for (n=0;n!=nline;n++) {
	
       textdecode_get_piece(n,0,txt);
	
			// new material, add a new mesh
			// if we don't have one yet and if we do,
			// the older one actually has polygons
			
		if ((strcmp(txt,"usemtl")==0) || (mesh==NULL)) {
		
			mesh_add=TRUE;
			
			if (mesh!=NULL) {
				mesh_add=(mesh->npoly!=0);
			}
				
			if (mesh_add) {
			
				mesh_idx=map_mesh_add(&map);
				
				if (mesh_idx==-1) {
					textdecode_close();
					StandardAlert(kAlertStopAlert,"\pImport Failed","\pNot enough memory to create mesh.",NULL,NULL);
					return;
				}
	
				mesh=&map.mesh.meshes[mesh_idx];
			}
		}

			// a face?
			
		if (strcmp(txt,"f")!=0) continue;
		
            // get the face points
        
        npt=0;
        
        for (k=0;k!=8;k++) {
            textdecode_get_piece(n,(k+1),txt);
            if (txt[0]==0x0) break;
            
            uvstr[0]=0x0;
            
            strcpy(vstr,txt);
            c=strchr(vstr,'/');
            if (c!=NULL) {
                strcpy(uvstr,(c+1));
                *c=0x0;
            }
            c=strchr(uvstr,'/');
            if (c!=NULL) *c=0x0;
            
            v_idx=atoi(vstr)-1;
			dpt=vertexes+v_idx;
			
			px[npt]=dpt->x;
			py[npt]=dpt->y;
			pz[npt]=dpt->z;
            
			if ((uvstr[0]==0x0) || (nuv==0)) {
				gx[npt]=gy[npt]=0.0f;
			}
			else {
				uv_idx=atoi(uvstr)-1;
				
				uv=uvs+(uv_idx*2);
                gx[npt]=*uv++;
                gy[npt]=*uv;
            }
			
            npt++;
        }
		
		map_mesh_add_poly(&map,mesh_idx,npt,px,py,pz,gx,gy,txt_idx);
	}
	
	free(vertexes);
	free(uvs);
	
	textdecode_close();
	
		// finish up
		
	piece_add_mesh_finish(mesh_idx);
	
		// if no uvs, force auto-texture
		
	if (nuv==0) map_mesh_reset_uv(&map,mesh_idx);
}

/* =======================================================

      Add Height Map Mesh
      
======================================================= */

float piece_add_height_map_mesh_get_height(unsigned char *data,int wid,int high,bool alpha_channel,int x,int z,int div_cnt)
{
	int				y;
	unsigned char	*p;
	
		// find offset in portal
		
	x=(int)(((float)x/(float)div_cnt)*(float)wid);
	y=(int)(((float)z/(float)div_cnt)*(float)high);
	
	if (x<0) x=0;
	if (x>=wid) x=wid-1;
	if (y<0) y=0;
	if (y>=high) y=high-1;
	
	if (alpha_channel) {
		p=data+((y*(wid<<2))+(x<<2));
	}
	else {
		p=data+((y*(wid*3))+(x*3));
	}
	
	return(((float)*p)/255.0f);
}

void piece_add_height_map_mesh(void)
{
	int					x,z,px,pz,div_cnt,div_sz,total_sz,bwid,bhigh,high,
						kx[4],ky[4],kz[4],y[4],
						mesh_idx,txt_idx;
	float				f_portal_y_sz,gx[4],gy[4];
	char				path[1024];
	unsigned char		*data;
	bool				alpha_channel;
	d3pnt				pnt;
	
		// get the png
		
	if (!import_load_file(path,"png")) return;
	data=bitmap_load_png_data(path,&bwid,&bhigh,&alpha_channel);
	if (data==NULL) return;
	
		// division and sizes
		
	if (!dialog_height_import_run(&div_cnt,&total_sz,&high)) {
		free(data);
		return;
	}
		
	div_sz=total_sz/div_cnt;
	f_portal_y_sz=(float)high;
	
	piece_create_get_spot(&pnt);
	
		// texture
		
	txt_idx=texture_palette_get_selected_texture();
	if (txt_idx==-1) txt_idx=0;
	
		// polygons
		
	mesh_idx=map_mesh_add(&map);
	if (mesh_idx==-1) {
		free(data);
		return;
	}
	
	SetCursor(*GetCursor(watchCursor));
	
	for (z=0;z!=div_cnt;z++) {
		for (x=0;x!=div_cnt;x++) {
			
			px=(pnt.x-(total_sz/2))+(x*div_sz);
			pz=(pnt.z-(total_sz/2))+(z*div_sz);
			
				// floors
				
			y[0]=pnt.y-(int)(f_portal_y_sz*piece_add_height_map_mesh_get_height(data,bwid,bhigh,alpha_channel,x,z,div_cnt));
			y[1]=pnt.y-(int)(f_portal_y_sz*piece_add_height_map_mesh_get_height(data,bwid,bhigh,alpha_channel,(x+1),z,div_cnt));
			y[2]=pnt.y-(int)(f_portal_y_sz*piece_add_height_map_mesh_get_height(data,bwid,bhigh,alpha_channel,(x+1),(z+1),div_cnt));
			y[3]=pnt.y-(int)(f_portal_y_sz*piece_add_height_map_mesh_get_height(data,bwid,bhigh,alpha_channel,x,(z+1),div_cnt));
										
			gx[0]=gx[1]=gx[2]=0.0f;
			gy[0]=gy[1]=gy[2]=0.0f;
					
			if (((x+z)&0x1)!=0) {
				kx[0]=px;
				kx[1]=kx[2]=px+div_sz;
				kz[0]=kz[1]=pz;
				kz[2]=pz+div_sz;
				ky[0]=y[0];
				ky[1]=y[1];
				ky[2]=y[2];
				
				map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
			
				kx[0]=kx[2]=px;
				kx[1]=px+div_sz;
				kz[0]=pz;
				kz[1]=kz[2]=pz+div_sz;
				ky[0]=y[0];
				ky[1]=y[2];
				ky[2]=y[3];
				
				map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
			}
			else {
				kx[0]=kx[2]=px;
				kx[1]=px+div_sz;
				kz[0]=kz[1]=pz;
				kz[2]=pz+div_sz;
				ky[0]=y[0];
				ky[1]=y[1];
				ky[2]=y[3];
				
				map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
			
				kx[0]=kx[1]=px+div_sz;
				kx[2]=px;
				kz[0]=pz;
				kz[1]=kz[2]=pz+div_sz;
				ky[0]=y[1];
				ky[1]=y[2];
				ky[2]=y[3];
				
				map_mesh_add_poly(&map,mesh_idx,3,kx,ky,kz,gx,gy,txt_idx);
			}
		}
	}
	
		// reset UVs
				
	map_mesh_reset_uv(&map,mesh_idx);
		
	free(data);
	
	os_set_arrow_cursor();
	
		// finish up
		
	piece_add_mesh_finish(mesh_idx);
}

/* =======================================================

      Add Grid Mesh
      
======================================================= */

void piece_add_grid_mesh(void)
{
	int				mesh_idx,xdiv,ydiv,zdiv,
					x,y,z,sz,txt_idx,
					px[4],py[4],pz[4];
	float			gx[4],gy[4];
	d3pnt			pnt;
	
	if (!piece_create_texture_ok()) return;

		// get mesh divisions
		
	if (!dialog_create_grid_mesh_run(&xdiv,&ydiv,&zdiv)) return;
	
		// texture and size
		
	txt_idx=texture_palette_get_selected_texture();
	if (txt_idx==-1) txt_idx=0;
	
	sz=map_enlarge*5;

	piece_create_get_spot(&pnt);
	pnt.x-=((xdiv*sz)/2);
	pnt.y-=((ydiv*sz)/2);
	pnt.z-=((zdiv*sz)/2);

		// create mesh
		
	mesh_idx=map_mesh_add(&map);
	if (mesh_idx==-1) return;
	
		// default UVs
		
	gx[0]=gx[1]=gx[2]=gx[3]=0.0f;
	gy[0]=gy[1]=gy[2]=gy[3]=0.0f;
	
	SetCursor(*GetCursor(watchCursor));
	
		// add top and bottom polys
		
	for (z=0;z!=zdiv;z++) {
		for (x=0;x!=xdiv;x++) {
			px[0]=px[3]=(x*sz)+pnt.x;
			px[1]=px[2]=((x+1)*sz)+pnt.x;
			pz[0]=pz[1]=(z*sz)+pnt.z;
			pz[2]=pz[3]=((z+1)*sz)+pnt.z;

			py[0]=py[1]=py[2]=py[3]=pnt.y;
			map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			
			if (ydiv!=0) {
				py[0]=py[1]=py[2]=py[3]=(ydiv*sz)+pnt.y;
				map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			}
		}
	}
	
		// sides
		
	for (x=0;x!=xdiv;x++) {
		for (y=0;y!=ydiv;y++) {
			px[0]=px[3]=(x*sz)+pnt.x;
			px[1]=px[2]=((x+1)*sz)+pnt.x;
			py[0]=py[1]=(y*sz)+pnt.y;
			py[2]=py[3]=((y+1)*sz)+pnt.y;
			
			pz[0]=pz[1]=pz[2]=pz[3]=pnt.z;
			map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			
			if (zdiv!=0) {
				pz[0]=pz[1]=pz[2]=pz[3]=(zdiv*sz)+pnt.z;
				map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			}
		}
	}
	
	for (z=0;z!=zdiv;z++) {
		for (y=0;y!=ydiv;y++) {
			pz[0]=pz[1]=(z*sz)+pnt.z;
			pz[2]=pz[3]=((z+1)*sz)+pnt.z;
			py[0]=py[3]=(y*sz)+pnt.y;
			py[1]=py[2]=((y+1)*sz)+pnt.y;
			
			px[0]=px[1]=px[2]=px[3]=pnt.x;
			map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			
			if (xdiv!=0) {
				px[0]=px[1]=px[2]=px[3]=(xdiv*sz)+pnt.x;
				map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			}
		}
	}
	
		// reset UVs
		
	map_mesh_reset_uv(&map,mesh_idx);
	
	os_set_arrow_cursor();
	
		// finish up
		
	piece_add_mesh_finish(mesh_idx);
}

/* =======================================================

      Add Polygon Mesh
      
======================================================= */

void piece_add_polygon_mesh(void)
{
	int				mesh_idx,sz,txt_idx,
					px[4],py[4],pz[4];
	float			gx[4],gy[4];
	d3pnt			pnt;
	
	if (!piece_create_texture_ok()) return;
	
		// texture and size
		
	txt_idx=texture_palette_get_selected_texture();
	if (txt_idx==-1) txt_idx=0;
	
		// create mesh
		
	mesh_idx=map_mesh_add(&map);
	if (mesh_idx==-1) return;
	
		// create polygon
		
	piece_create_get_spot(&pnt);

	sz=map_enlarge*5;

	px[0]=px[3]=pnt.x-sz;
	px[1]=px[2]=pnt.x+sz;
	pz[0]=pz[1]=pnt.z-sz;
	pz[2]=pz[3]=pnt.z+sz;
	py[0]=py[1]=py[2]=py[3]=pnt.y;
	
	gx[0]=gx[1]=gx[2]=gx[3]=0.0f;
	gy[0]=gy[1]=gy[2]=gy[3]=0.0f;

	map_mesh_add_poly(&map,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
	
		// reset UVs
		
	map_mesh_reset_uv(&map,mesh_idx);
	
		// finish up
		
	piece_add_mesh_finish(mesh_idx);
}

/* =======================================================

      Combine Meshes
      
======================================================= */

void piece_combine_mesh(void)
{
	int				n,k,nsel,org_idx,mesh_combine_idx,new_mesh_combine_idx,
					type,mesh_idx,poly_idx;
	int				sel_mesh_idx[select_max_item];
	
		// get all selected meshes
		// select will change as meshes are deleted so
		// we need our own list
		
	nsel=0;
	
	for (n=0;n!=select_count();n++) {
		select_get(n,&type,&mesh_idx,&poly_idx);
		if (type==mesh_piece) sel_mesh_idx[nsel++]=mesh_idx;
	}
	
	if (nsel<2) return;
	
		// clear selection
		
	select_clear();
	
		// combine all meshes
		
	mesh_combine_idx=sel_mesh_idx[0];
	
	for (n=1;n<nsel;n++) {
	
			// combine
			
		mesh_idx=sel_mesh_idx[n];
		new_mesh_combine_idx=map_mesh_combine(&map,mesh_combine_idx,mesh_idx);
		if (new_mesh_combine_idx==-1) return;
		
			// move other meshes in select as combine deleted them
			
		for (k=(n+1);k<nsel;k++) {
			org_idx=sel_mesh_idx[k];
			if (org_idx>mesh_idx) sel_mesh_idx[k]--;
			if (org_idx>mesh_combine_idx) sel_mesh_idx[k]--;
		}
		
		mesh_combine_idx=new_mesh_combine_idx;
	}
	
	if (mesh_combine_idx!=-1) {
		select_add(mesh_piece,mesh_combine_idx,0);
	}
}

/* =======================================================

      Split Meshes
      
======================================================= */

void piece_split_mesh(void)
{
	int					n,k,mesh_idx,x[8],y[8],z[8],
						type,add_mesh_idx,add_poly_idx;
	bool				first_mesh;
	d3pnt				*pt;
	map_mesh_type		*mesh,*copy_mesh;
	map_mesh_poly_type	*poly;
	
		// create new mesh
		
	mesh_idx=map_mesh_add(&map);
	if (mesh_idx==-1) return;
	
		// add in the selected polygons
		
	first_mesh=FALSE;
	mesh=&map.mesh.meshes[mesh_idx];
		
	for (n=0;n!=select_count();n++) {
		select_get(n,&type,&add_mesh_idx,&add_poly_idx);
		if (type!=mesh_piece) continue;
		
			// if first mesh hit, copy mesh settings
			
		copy_mesh=&map.mesh.meshes[add_mesh_idx];
		
		if (first_mesh) {
			mesh->group_idx=copy_mesh->group_idx;
			memmove(&mesh->flag,&copy_mesh->flag,sizeof(map_mesh_flag_type));
			memmove(&mesh->msg,&copy_mesh->msg,sizeof(map_mesh_message_type));
			
			first_mesh=FALSE;
		}
		
			// add the polygon
		
		poly=&map.mesh.meshes[add_mesh_idx].polys[add_poly_idx];
		
		for (k=0;k!=poly->ptsz;k++) {
			pt=&copy_mesh->vertexes[poly->v[k]];
			x[k]=pt->x;
			y[k]=pt->y;
			z[k]=pt->z;
		}
		
		map_mesh_add_poly(&map,mesh_idx,poly->ptsz,x,y,z,poly->gx,poly->gy,poly->txt_idx);
		
			// delete poly from mesh
			
		select_delete_move_index(mesh_piece,add_mesh_idx,add_poly_idx);
		map_mesh_delete_poly(&map,add_mesh_idx,add_poly_idx);
	}
	
		// if no polygons, delete mesh
		
	if (map.mesh.meshes[mesh_idx].npoly==0) {
		map_mesh_delete(&map,mesh_idx);
		return;
	}
	
		// make new mesh selection
		
	select_clear();
	select_add(mesh_piece,mesh_idx,0);
	
		// auto-switch to mesh drag mode
		
	drag_mode=drag_mode_mesh;

	main_wind_tool_reset();
	main_wind_tool_fix_enable();
}

/* =======================================================

      Map Changes
      
======================================================= */

void map_mesh_move_all(int x,int y,int z)
{
	int				n;
	d3pnt			pt;
	
	pt.x=x;
	pt.y=y;
	pt.z=z;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		map_mesh_move(&map,n,&pt,TRUE);
	}
}

void map_mesh_reset_uv_all(void)
{
	int				n;
	
	for (n=0;n!=map.mesh.nmesh;n++) {
		map_mesh_reset_uv(&map,n);
	}
}

void map_optimize(void)
{
	int					poly_threshold,comb_count;
	char				str[256];
	unsigned char		p_str[256];
	
	if (!dialog_optimize_run(&poly_threshold)) return;
	
	select_clear();
	
	os_set_wait_cursor();
	comb_count=map_mesh_combine_small(&map,poly_threshold);
	os_set_arrow_cursor();
	
	sprintf(str,"%d small meshes were combined with larger meshes.",comb_count);
	CopyCStringToPascal(str,p_str);
	StandardAlert(0,"\pOptimize",p_str,NULL,NULL);
}
