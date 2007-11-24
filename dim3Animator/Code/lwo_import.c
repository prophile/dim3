/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Lightwave/LWO Importer

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

#include "dialog.h"
#include "import.h"

extern int					cur_mesh;
extern model_type			model;

/* =======================================================

      Find Chunks
      
======================================================= */

ptr find_chunk(ptr data,int datasz,int cnt,unsigned long tag,int *sz)
{
    unsigned long		*lptr,fndtag,fndtagsz;
    
    data+=12;
    datasz-=12;			// move ahead of initial tags
    
    while (datasz>8) {
    
            // get chunk
            
        lptr=(unsigned long*)data;
        fndtag=*lptr;
        lptr++;
        fndtagsz=*lptr;
        
        data+=8;
        datasz-=8;
        
            // is it our chunk?
            
        if (fndtag==tag) {
            if (cnt==0) {
                *sz=(int)fndtagsz;
                return(data);
            }
            cnt--;
        }
        
            // move to next chunk
            
        data+=(int)fndtagsz;
        datasz-=(int)fndtagsz;
    }
    
    return(NULL);
}

/* =======================================================

      Search for a UV
      
======================================================= */

bool find_VMAP_uv(ptr data,int datasz,int v,float *gx,float *gy)
{
    int				i,k,chunksz;
    short			*sptr;
    long			*lptr;
    float			*fptr;
    unsigned char	c;
    ptr				chunk;
    
    i=0;
    *gx=*gy=0;
    
    while (TRUE) {
    
            // find first chunk of type
    
        chunk=find_chunk(data,datasz,i,FOUR_CHAR_CODE('VMAP'),&chunksz);    
        if (chunk==NULL) return(FALSE);
        
        i++;
        
        lptr=(long*)chunk;
        if (*lptr!=FOUR_CHAR_CODE('TXUV')) continue;
    
        chunk+=6;
        chunksz-=6;
        
        	// skip over string name
        
        i=0;
        while (chunksz>0) {					// find end of string
            i++;
            c=*chunk++;
            chunksz--;
            
            if (c==0x0) break;
        }
        
        if ((i&0x1)!=0) {					// odd sized string gets additional byte
            chunk++;
            chunksz--;
        }
        
            // search for UV
        
        while (chunksz>6) {
        
            sptr=(short*)chunk;
            k=*sptr;
            
            chunk+=2;
            chunksz-=2;
            
            if (k==v) {
                fptr=(float*)chunk;
                *gx=(*fptr++);
                *gy=1-(*fptr++);
                return(TRUE);
            }

            chunk+=8;
            chunksz-=8;
        }
        
    }
    
    return(FALSE);
}

bool find_VMAD_uv(ptr data,int datasz,int poly,int v,float *gx,float *gy)
{
    int				i,k,j,chunksz;
    short			*sptr;
    long			*lptr;
    float			*fptr;
    unsigned char	c;
    ptr				chunk;
    
    i=0;
    *gx=*gy=0;
    
    while (TRUE) {
    
            // find first chunk of type
    
        chunk=find_chunk(data,datasz,i,FOUR_CHAR_CODE('VMAD'),&chunksz);    
        if (chunk==NULL) return(FALSE);
        
        i++;
        
        lptr=(long*)chunk;
        if (*lptr!=FOUR_CHAR_CODE('TXUV')) continue;
    
        chunk+=6;
        chunksz-=6;
        
        	// skip over string name
        
        i=0;
        while (chunksz>0) {					// find end of string
            i++;
            c=*chunk++;
            chunksz--;
            
            if (c==0x0) break;
        }
        
        if ((i&0x1)!=0) {					// odd sized string gets additional byte
            chunk++;
            chunksz--;
        }
        
            // search for UV
        
        while (chunksz>6) {
        
            sptr=(short*)chunk;
            k=*sptr;
            j=*(sptr+1);
            
            chunk+=4;
            chunksz-=4;
            
            if ((k==v) && (j==poly)) {
                fptr=(float*)chunk;
                *gx=(*fptr++);
                *gy=1-(*fptr++);
                return(TRUE);
            }

            chunk+=8;
            chunksz-=8;
        }
        
    }
    
    return(FALSE);
}

/* =======================================================

      Import Lightwave LWO File
      
======================================================= */

bool import_lightwave(char *path,char *err_str)
{
    int						i,k,poly,npt,nvertex,ntrig,datasz,chunksz,texture_idx;
 	short					*sptr;
	long					*lptr;
    float					*fptr;
	FILE					*file;
	struct stat				sb;
   	ptr						data,chunk;
	model_vertex_type		*vertex;
    model_trig_type			*trig;
	model_material_type		*material;
	
		// clear materials
    
    clear_materials();
	
		// load the file
		
	if (stat(path,&sb)!=0) return(FALSE);
	datasz=sb.st_size;
		
	file=fopen(path,"r");
	if (file==NULL) {
		strcpy(err_str,"Couldn't open file.");
		return(FALSE);
    }
   
	data=valloc(datasz);
	if (data==NULL) {
		fclose(file);
		return(FALSE);
	}
	
	fread(data,1,datasz,file);
	fclose(file);
    
        // find vertexes

    chunk=find_chunk(data,datasz,0,FOUR_CHAR_CODE('PNTS'),&chunksz);    
    if (chunk==NULL) {
        strcpy(err_str,"Could not find any vertexes in file.");
        free(data);
        return(FALSE);
    }
    
    fptr=(float*)chunk;
    nvertex=chunksz/12;
	
	if (nvertex>=max_model_vertex) {
		sprintf(err_str,"Too many vertexes, models can have a maximum of %d vertexes.",max_model_vertex);
		free(data);
		return(FALSE);
	}

	vertex=model.meshes[cur_mesh].vertexes;
    
    for ((i=0);(i!=nvertex);i++) {
        vertex->pnt.x=(short)((*fptr++)*import_scale_factor);
        vertex->pnt.y=-(short)((*fptr++)*import_scale_factor);
        vertex->pnt.z=(short)((*fptr++)*import_scale_factor);
    
        vertex->major_bone_idx=vertex->minor_bone_idx=-1;
        vertex->bone_factor=1;
    
        vertex++;
    }
    
    model.meshes[cur_mesh].nvertex=nvertex;
        
        // find polygons
        
	chunk=find_chunk(data,datasz,0,FOUR_CHAR_CODE('POLS'),&chunksz);    
    if (chunk==NULL) {
        strcpy(err_str,"Could not find any polygons in file.");
        free(data);
        return(FALSE);
    }
    
    lptr=(long*)chunk;
    if (*lptr!=FOUR_CHAR_CODE('FACE')) {
        strcpy(err_str,"Can only handle models with face polygons.");
        free(data);
        return(FALSE);
    }
    
    chunk+=4;
    chunksz-=4;
    
	texture_idx=texture_pick("Fill 0",err_str);
	if (texture_idx==-1) {
		free(data);
		return(FALSE);
	}

	material=&model.meshes[cur_mesh].materials[texture_idx];
    material->trig_start=0;

    ntrig=poly=0;
	trig=model.meshes[cur_mesh].trigs;
    
    sptr=(short*)chunk;
    
    while ((chunksz>2) && (ntrig<max_model_trig)) {
        npt=*sptr++;
        chunksz-=2;
        
        npt=npt&0x03FF;
        
        for ((k=0);(k!=(npt-2));k++) {
            if (ntrig>=max_model_trig) {
				sprintf(err_str,"Too many triangles, models can have a maximum of %d triangles.",max_model_trig);
				free(data);
				return(FALSE);
			}
        
            trig->v[0]=*sptr;
            trig->v[1]=*(sptr+(k+1));
            trig->v[2]=*(sptr+(k+2));
            
                // lookup uvs

            if (!find_VMAD_uv(data,datasz,poly,trig->v[0],&trig->gx[0],&trig->gy[0])) {
                find_VMAP_uv(data,datasz,trig->v[0],&trig->gx[0],&trig->gy[0]);
            }
            if (!find_VMAD_uv(data,datasz,poly,trig->v[1],&trig->gx[1],&trig->gy[1])) {
                find_VMAP_uv(data,datasz,trig->v[1],&trig->gx[1],&trig->gy[1]);
            }
            if (!find_VMAD_uv(data,datasz,poly,trig->v[2],&trig->gx[2],&trig->gy[2])) {
                find_VMAP_uv(data,datasz,trig->v[2],&trig->gx[2],&trig->gy[2]);
            }

            ntrig++;
            trig++;
        }
        
        sptr+=npt;
        chunksz-=(npt*2);
        
        poly++;
    }
    
	material->trig_count=ntrig-material->trig_start;
    
	model.meshes[cur_mesh].ntrig=ntrig;
    
        // free the data
        
    free(data);
    
	return(TRUE);
}

