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
#include "portal_view.h"

#define import_obj_float_to_int			1000.0f
#define import_obj_max_dimension		5000

extern int						cr,cy,drag_mode;
extern bool						dp_object,dp_node,dp_lightsoundparticle,dp_auto_texture;

extern file_path_setup_type		file_path_setup;
extern map_type					map;

/* =======================================================

      Import Library Mesh
      
======================================================= */

int piece_import_library_mesh(char *name,int mx,int my,int mz)
{
	int					n,k,nline,nvertex,npoly,nuv,npt,uv_idx,
						mesh_idx,x,y,z;
	char				path[1024],
						*c,txt[256],vstr[256],uvstr[256];
	float				scale,fx,fy,fz,fsz;
	float				*uvs,*uv;
	bool				mesh_ok,mesh_delete;
	d3pnt				*dpt;
	d3fpnt				min,max;
	portal_type			*portal;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;
	
		// import the file
		
	file_paths_data(&file_path_setup,path,"Library",name,"obj");
		
	if (!textdecode_open(path,0x20)) {
		StandardAlert(kAlertStopAlert,"\pImport Failed","\pCould not open OBJ file.",NULL,NULL);
		return(-1);
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
		return(-1);
    }
	if (npoly==0) {
		textdecode_close();
		StandardAlert(kAlertStopAlert,"\pImport Failed","\pNo faces in OBJ.",NULL,NULL);
		return(-1);
    }
	if (nuv==0) {
		textdecode_close();
		StandardAlert(kAlertStopAlert,"\pImport Failed","\pNo UV coordinates in OBJ.",NULL,NULL);
		return(-1);
    }
	
		// create new mesh
		
	portal=&map.portals[cr];
	
	mesh_ok=FALSE;
	mesh_delete=FALSE;
	
	mesh_idx=map_portal_mesh_add(&map,cr);
	if (mesh_idx!=-1) {
		mesh_delete=TRUE;
		
		if (map_portal_mesh_set_vertex_count(&map,cr,mesh_idx,nvertex)) {
			if (map_portal_mesh_set_poly_count(&map,cr,mesh_idx,npoly)) {
				mesh_ok=TRUE;
			}
		}
	}
	
	if (!mesh_ok) {
		if (mesh_delete) map_portal_mesh_delete(&map,cr,mesh_idx);
		textdecode_close();
		StandardAlert(kAlertStopAlert,"\pImport Failed","\pNot enough memory to create mesh.",NULL,NULL);
		return(-1);
    }
	
	mesh=&portal->mesh.meshes[mesh_idx];
	
		// scale the import
		
	fsz=max.x-min.x;
	if ((max.y-min.y)>fsz) fsz=max.y-min.y;
	if ((max.z-min.z)>fsz) fsz=max.z-min.z;
	
	scale=import_obj_max_dimension/fsz;
	k=(int)(scale*100.0f);
	scale=((float)k)/100.0f;

	dialog_mesh_scale_run(&scale);
	
		// get the vertexes

 	dpt=mesh->vertexes;

    for (n=0;n!=nline;n++) {

        textdecode_get_piece(n,0,txt);
		if (strcmp(txt,"v")!=0) continue;
                
		textdecode_get_piece(n,1,txt);
		dpt->x=(int)(strtod(txt,NULL)*scale);
		
		textdecode_get_piece(n,2,txt);
		dpt->y=-(int)(strtod(txt,NULL)*scale);
		
		textdecode_get_piece(n,3,txt);
		dpt->z=(int)(strtod(txt,NULL)*scale);
        
		dpt++;
    }
	
		// recenter the vertexes
		
	x=y=z=0;
	
 	dpt=mesh->vertexes;
	
	for (n=0;n!=nvertex;n++) {
		x+=dpt->x;
		y+=dpt->y;
		z+=dpt->z;
		dpt++;
	}
	
	x/=nvertex;
	y/=nvertex;
	z/=nvertex;
		
 	dpt=mesh->vertexes;
	
	for (n=0;n!=nvertex;n++) {
		dpt->x=(dpt->x-x)+mx;
		dpt->y=(dpt->y-y)+my;
		dpt->z=(dpt->z-z)+mz;
		dpt++;
	}
	
		// get the UVs
		
	uvs=(float*)valloc(sizeof(float)*(2*nuv));
	if (uvs==NULL) {
		map_portal_mesh_delete(&map,cr,mesh_idx);
		textdecode_close();
		StandardAlert(kAlertStopAlert,"\pImport Failed","\pOut of Memory.",NULL,NULL);
		return(-1);
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

		// get the polys

	poly=mesh->polys;
	
    for (n=0;n!=nline;n++) {

        textdecode_get_piece(n,0,txt);
        if (strcmp(txt,"f")!=0) continue;
		
			// setup poly
			
		poly->txt_idx=texture_palette_get_selected_texture();
		if (poly->txt_idx==-1) poly->txt_idx=0;
		
		poly->x_shift=poly->y_shift=0.0f;
		poly->dark_factor=1.0f;
		poly->alpha=1.0f;

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
            
            poly->v[npt]=atoi(vstr)-1;
            
			if (uvstr[0]==0x0) {
				poly->gx[npt]=poly->gy[npt]=0.0f;
			}
			else {
				uv_idx=atoi(uvstr)-1;
				
				uv=uvs+(uv_idx*2);
                poly->gx[npt]=*uv++;
                poly->gy[npt]=*uv;
            }
			
            npt++;
        }
		
		poly->ptsz=npt;
		
		poly++;
	}
	
	free(uvs);
	
	textdecode_close();
	
		// delete any unused vertexes
		
	map_portal_mesh_delete_unused_vertexes(&map,cr,mesh_idx);
	
		// push to grid
		
	mesh_snap_to_grid(cr,mesh_idx);
	
	return(mesh_idx);
}


/* =======================================================

      Auto Generate Mesh
      
======================================================= */

int piece_create_ag_mesh(char *name,int mx,int my,int mz)
{
	int				mesh_idx,xdiv,ydiv,zdiv,
					x,y,z,sz,txt_idx,
					px[4],py[4],pz[4];
	float			gx[4],gy[4];
	char			str[32];
	
		// get mesh divisions
		
	strcpy(str,(char*)&name[4]);
	str[3]=0x0;
	xdiv=atoi(str);
	
	strcpy(str,(char*)&name[8]);
	str[3]=0x0;
	ydiv=atoi(str);

	strcpy(str,(char*)&name[12]);
	str[3]=0x0;
	zdiv=atoi(str);
	
		// texture and size
		
	txt_idx=texture_palette_get_selected_texture();
	
	sz=100;
	mx-=((xdiv*sz)/2);
	my-=((ydiv*sz)/2);
	mz-=((zdiv*sz)/2);

		// create mesh
		
	mesh_idx=map_portal_mesh_add(&map,cr);
	if (mesh_idx==-1) return(-1);
	
		// default UVs
		
	gx[0]=gx[3]=0.0f;
	gx[1]=gx[2]=1.0f;
	gy[0]=gy[1]=0.0f;
	gy[2]=gy[3]=1.0f;
	
		// add top and bottom polys
		
	for (z=0;z!=zdiv;z++) {
		for (x=0;x!=xdiv;x++) {
			px[0]=px[3]=(x*sz)+mx;
			px[1]=px[2]=((x+1)*sz)+mx;
			pz[0]=pz[1]=(z*sz)+mz;
			pz[2]=pz[3]=((z+1)*sz)+mz;

			py[0]=py[1]=py[2]=py[3]=my;
			map_portal_mesh_add_poly(&map,cr,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			
			py[0]=py[1]=py[2]=py[3]=(ydiv*sz)+my;
			map_portal_mesh_add_poly(&map,cr,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
		}
	}
	
		// sides
		
	for (x=0;x!=xdiv;x++) {
		for (y=0;y!=ydiv;y++) {
			px[0]=px[3]=(x*sz)+mx;
			px[1]=px[2]=((x+1)*sz)+mx;
			py[0]=py[1]=(y*sz)+my;
			py[2]=py[3]=((y+1)*sz)+my;
			
			pz[0]=pz[1]=pz[2]=pz[3]=mz;
			map_portal_mesh_add_poly(&map,cr,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			
			pz[0]=pz[1]=pz[2]=pz[3]=(zdiv*sz)+mz;
			map_portal_mesh_add_poly(&map,cr,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
		}
	}
	
	for (z=0;z!=zdiv;z++) {
		for (y=0;y!=ydiv;y++) {
			pz[0]=pz[1]=(z*sz)+mz;
			pz[2]=pz[3]=((z+1)*sz)+mz;
			py[0]=py[3]=(y*sz)+my;
			py[1]=py[2]=((y+1)*sz)+my;
			
			px[0]=px[1]=px[2]=px[3]=mx;
			map_portal_mesh_add_poly(&map,cr,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
			
			px[0]=px[1]=px[2]=px[3]=(xdiv*sz)+mx;
			map_portal_mesh_add_poly(&map,cr,mesh_idx,4,px,py,pz,gx,gy,txt_idx);
		}
	}
	
	return(mesh_idx);
}

/* =======================================================

      Run Mesh Pick
      
======================================================= */

int piece_import_mesh(char *name,int mx,int my,int mz)
{
	if (memcmp(name,"AGM ",4)!=0) return(piece_import_library_mesh(name,mx,my,mz));
	return(piece_create_ag_mesh(name,mx,my,mz));
}

int piece_import_mesh_pick(int mx,int my,int mz)
{
	char				name[file_str_len];
	
	if (dialog_choose_library_object_run(name)) return(piece_import_mesh(name,mx,my,mz));
	
	return(-1);
}

/* =======================================================

      Add Library Mesh
      
======================================================= */

void piece_add_library_mesh(void)
{
	int					mesh_idx,rn,mx,my,mz;
	
	if (!piece_create_texture_ok()) return;
	
		// get import location
		
	rn=piece_create_get_spot(&mx,&my,&mz,0,0,0);
			
		// import mesh
		
	mesh_idx=piece_import_mesh_pick(mx,my,mz);
	if (mesh_idx==-1) return;
	
		// make selection
		
	select_clear();
	select_add(mesh_piece,rn,mesh_idx,0);
	
		// change mode to move entire mesh
		
	drag_mode=drag_mode_mesh;
	
	if (dp_auto_texture) {
		map_portal_mesh_reset_uv(&map,rn,mesh_idx);
	}
	else {
		map.portals[rn].mesh.meshes[mesh_idx].flag.lock_uv=TRUE;
	}
	
	main_wind_draw();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
}

/* =======================================================

      Replace Library Mesh
      
======================================================= */

void piece_replace_library_mesh(void)
{
	int			type,portal_idx,mesh_idx,poly_idx,
				rep_mesh_idx;
	d3pnt		min,max,mpt;
	
		// get mesh to replace
		
	if (select_count()==0) return;
	
	select_get(0,&type,&portal_idx,&mesh_idx,&poly_idx);
	if ((type!=mesh_piece) || (portal_idx!=cr)) return;
	
		// any textures?
		
	if (!piece_create_texture_ok()) return;
	
		// remember size
		
	map_portal_mesh_calculate_extent(&map,cr,mesh_idx,&min,&max);
	map_portal_mesh_calculate_center(&map,cr,mesh_idx,&mpt);
	
		// import new mesh
	
	rep_mesh_idx=piece_import_mesh_pick(mpt.x,mpt.y,mpt.z);
	if (rep_mesh_idx==-1) return;

		// delete orginial and replace
		// new mesh index will change with delete
		
	if (map_portal_mesh_delete(&map,cr,mesh_idx)) {
		rep_mesh_idx--;
	}
	
	map_portal_mesh_resize(&map,cr,rep_mesh_idx,&min,&max);
	
		// make selection
		
	select_clear();
	select_add(mesh_piece,cr,rep_mesh_idx,0);
	
		// change mode to move entire mesh
		
	drag_mode=drag_mode_mesh;
	
	if (dp_auto_texture) {
		map_portal_mesh_reset_uv(&map,cr,mesh_idx);
	}
	else {
		map.portals[portal_idx].mesh.meshes[mesh_idx].flag.lock_uv=TRUE;
	}
	
	main_wind_draw();
	main_wind_tool_reset();
	main_wind_tool_fix_enable();
}

/* =======================================================

      Combine Meshes
      
======================================================= */

void piece_combine_mesh(int portal_idx)
{
	int				n,k,nsel,org_idx,mesh_combine_idx,new_mesh_combine_idx,
					type,chk_portal_idx,mesh_idx,poly_idx;
	int				sel_mesh_idx[select_max_item];
	
		// get all selected meshes
		// select will change as meshes are deleted so
		// we need our own list
		
	nsel=0;
	
	for (n=0;n!=select_count();n++) {
		select_get(n,&type,&chk_portal_idx,&mesh_idx,&poly_idx);
		if ((type==mesh_piece) && (chk_portal_idx==portal_idx)) {
			sel_mesh_idx[nsel++]=mesh_idx;
		}
	}
	
	if (nsel<2) return;
	
		// clear selection
		
	select_clear();
	
		// combine all meshes
		
	mesh_combine_idx=sel_mesh_idx[0];
	
	for (n=1;n<nsel;n++) {
	
			// combine
			
		mesh_idx=sel_mesh_idx[n];
		new_mesh_combine_idx=map_portal_mesh_combine(&map,portal_idx,mesh_combine_idx,mesh_idx);
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
		select_add(mesh_piece,portal_idx,mesh_combine_idx,0);
	}
}

