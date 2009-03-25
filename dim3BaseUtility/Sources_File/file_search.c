/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: File Searches

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
	#include "dim3baseutility.h"
#endif

/* =======================================================

      Read Directories OS X/Linux
            
======================================================= */

#ifndef D3_OS_WINDOWS

void file_paths_read_directory_single(file_path_directory_type *fpd,char *path,char *path_add,char *ext_name,char *required_file_name,int dir_type,bool recursive_search)
{
	int						n,sz,idx;
	char					*c,path2[1024],path_add2[1024],file_name[file_str_len];
	DIR						*dir;
	struct dirent			*de;
	
	if (path_add!=NULL) {
		sprintf(path2,"%s/%s",path,path_add);
	}
	else {
		strcpy(path2,path);
	}

	dir=opendir(path2);
	if (dir==NULL) return;
	
	while (TRUE) {
		de=readdir(dir);
		if (de==NULL) break;
		
		if (de->d_name[0]=='.') continue;
		
			// check extension
			
		c=strrchr(de->d_name,'.');
			
		if (ext_name==NULL) {			// looking for directory
			if (c!=NULL) continue;
		}
		else {

				// go deeper into directories

			if (c==NULL) {
			
				if (!recursive_search) continue;
				
				if (path_add!=NULL) {
					sprintf(path_add2,"%s/%s",path_add,de->d_name);
				}
				else {
					strcpy(path_add2,de->d_name);
				}
				
				file_paths_read_directory_single(fpd,path,path_add2,ext_name,required_file_name,dir_type,TRUE);
				continue;
			}
			
				// looking for file

			if (strcasecmp((c+1),ext_name)!=0) continue;
		}
		
			// is name a override?
			// if so, delete original
			
		strncpy(file_name,de->d_name,file_str_len);
		file_name[file_str_len-1]=0x0;
		
		c=strrchr(file_name,'.');
		if (c!=NULL) *c=0x0;
		
		idx=-1;
		
		for (n=0;n!=fpd->nfile;n++) {
			if (strcasecmp(fpd->files[n].file_name,file_name)==0) {
				idx=n;
				break;
			}
		}
		
		if (idx!=-1) {
			sz=(fpd->nfile-(idx+1))*sizeof(file_path_directory_file_type);
			if (sz>0) memmove(&fpd->files[idx],&fpd->files[idx+1],sz);
			fpd->nfile--;
		}
		
			// new name
			
		fpd->files[fpd->nfile].dir_type=dir_type;
		if (path_add!=NULL) {
			sprintf(fpd->files[fpd->nfile].file_name,"%s/%s",path_add,file_name);
		}
		else {
			strcpy(fpd->files[fpd->nfile].file_name,file_name);
		}
		fpd->nfile++;
		
			// move on to next file
			
		if (fpd->nfile>=file_paths_max_directory_file) break;
	}
	
	closedir(dir);
}

#endif

/* =======================================================

      Read Directories Windows
            
======================================================= */

#ifdef D3_OS_WINDOWS

void file_paths_read_directory_single(file_path_directory_type *fpd,char *path,char *path_add,char *ext_name,char *required_file_name,int dir_type,bool recursive_search)
{
	int					n,sz,idx;
	char				*c,path2[1024],path_add2[1024],file_name[file_str_len];
	bool				find_first;
	WIN32_FIND_DATA		find_data;
	HANDLE				find_hand;

	if (path_add!=NULL) {
		sprintf(path2,"%s\\%s\\*",path,path_add);
	}
	else {
		sprintf(path2,"%s\\*",path);
	}

	find_hand=FindFirstFile(path2,&find_data);
	if (find_hand==INVALID_HANDLE_VALUE) return;
	
	find_first=TRUE;

	while (TRUE) {

		if (!find_first) {
			if (!FindNextFile(find_hand,&find_data)) break;
		}
		find_first=FALSE;

			// skip dot paths

		if (find_data.cFileName[0]=='.') continue;
		
			// go deeper into directories

		if ((find_data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0) {
		
			if (!recursive_search) continue;
			
			if (path_add!=NULL) {
				sprintf(path_add2,"%s\\%s",path_add,find_data.cFileName);
			}
			else {
				strcpy(path_add2,find_data.cFileName);
			}

			file_paths_read_directory_single(fpd,path,path_add2,ext_name,required_file_name,dir_type,TRUE);
			continue;
		}

			// check extension

		c=strrchr(find_data.cFileName,'.');
		if ((c==NULL) || (ext_name==NULL)) continue;

		if (strcasecmp((c+1),ext_name)!=0) continue;

			// is name a override?
			// if so, delete original
		
		strncpy(file_name,find_data.cFileName,file_str_len);
		file_name[file_str_len-1]=0x0;
	
		c=strrchr(file_name,'.');
		if (c!=NULL) *c=0x0;
		
		idx=-1;
		
		for (n=0;n!=fpd->nfile;n++) {
			if (strcasecmp(fpd->files[n].file_name,file_name)==0) {
				idx=n;
				break;
			}
		}
		
		if (idx!=-1) {
			sz=(fpd->nfile-(idx+1))*sizeof(file_path_directory_file_type);
			if (sz>0) memmove(&fpd->files[idx],&fpd->files[idx+1],sz);
			fpd->nfile--;
		}
		
			// new name
			
		fpd->files[fpd->nfile].dir_type=dir_type;
		if (path_add!=NULL) {
			sprintf(fpd->files[fpd->nfile].file_name,"%s\\%s",path_add,file_name);
		}
		else {
			strcpy(fpd->files[fpd->nfile].file_name,file_name);
		}
		fpd->nfile++;
		
			// move on to next file
			
		if (fpd->nfile>=file_paths_max_directory_file) break;
	}

	FindClose(find_hand);
}

#endif

/* =======================================================

      Generic Routines
            
======================================================= */

file_path_directory_type* file_paths_read_directory_data(file_path_setup_type *file_path_setup,char *sub_path,char *ext_name)
{
	char						path[1024];
	file_path_directory_type	*fpd;
	
		// create memory for file list
		
	fpd=malloc(sizeof(file_path_directory_type));
	memset(fpd,0x0,sizeof(file_path_directory_type));
	
	fpd->nfile=0;

		// run through the dim3 directories

	if (file_path_setup->path_app[0]!=0x0) {
	
		strcpy(path,file_path_setup->path_app);
		if (sub_path!=NULL) {
			strcat(path,"/");
			strcat(path,sub_path);
		}
		
		file_paths_read_directory_single(fpd,path,NULL,ext_name,NULL,file_paths_dir_type_app,TRUE);
	}
	
	if (file_path_setup->path_data[0]!=0x0) {
	
		strcpy(path,file_path_setup->path_data);
		if (sub_path!=NULL) {
			strcat(path,"/");
			strcat(path,sub_path);
		}
		
		file_paths_read_directory_single(fpd,path,NULL,ext_name,NULL,file_paths_dir_type_data,TRUE);
	}
	
	if (file_path_setup->path_data_2[0]!=0x0) {
	
		strcpy(path,file_path_setup->path_data_2);
		if (sub_path!=NULL) {
			strcat(path,"/");
			strcat(path,sub_path);
		}
		
		file_paths_read_directory_single(fpd,path,NULL,ext_name,NULL,file_paths_dir_type_data2,TRUE);
	}

	return(fpd);
}

file_path_directory_type* file_paths_read_directory_data_dir(file_path_setup_type *file_path_setup,char *sub_path,char *required_file_name)
{
	char						path[1024];
	file_path_directory_type	*fpd;
	
		// create memory for file list
		
	fpd=malloc(sizeof(file_path_directory_type));
	memset(fpd,0x0,sizeof(file_path_directory_type));
	
	fpd->nfile=0;

		// run through the dim3 directories

	if (file_path_setup->path_app[0]!=0x0) {
	
		strcpy(path,file_path_setup->path_app);
		if (sub_path!=NULL) {
			strcat(path,"/");
			strcat(path,sub_path);
		}
		
		file_paths_read_directory_single(fpd,path,NULL,NULL,required_file_name,file_paths_dir_type_app,TRUE);
	}
	
	if (file_path_setup->path_data[0]!=0x0) {
	
		strcpy(path,file_path_setup->path_data);
		if (sub_path!=NULL) {
			strcat(path,"/");
			strcat(path,sub_path);
		}
		
		file_paths_read_directory_single(fpd,path,NULL,NULL,required_file_name,file_paths_dir_type_data,TRUE);
	}
	
	if (file_path_setup->path_data_2[0]!=0x0) {
	
		strcpy(path,file_path_setup->path_data_2);
		if (sub_path!=NULL) {
			strcat(path,"/");
			strcat(path,sub_path);
		}
		
		file_paths_read_directory_single(fpd,path,NULL,NULL,required_file_name,file_paths_dir_type_data2,TRUE);
	}

	return(fpd);
}

file_path_directory_type* file_paths_read_directory_document(file_path_setup_type *file_path_setup,char *sub_path,char *ext_name)
{
	char						path[1024];
	file_path_directory_type	*fpd;
	
		// create memory for file list
		
	fpd=malloc(sizeof(file_path_directory_type));
	memset(fpd,0x0,sizeof(file_path_directory_type));
	
	fpd->nfile=0;

		// read the files
		
	file_paths_documents(file_path_setup,path,sub_path,NULL,NULL);
	file_paths_read_directory_single(fpd,path,NULL,ext_name,NULL,file_paths_dir_type_documents,FALSE);

	return(fpd);
}

void file_paths_close_directory(file_path_directory_type *fpd)
{
	if (fpd!=NULL) free(fpd);
}

void file_paths_descript_directory_file(char *descript,int dir_type)
{
	char names[file_paths_dir_type_count][32]=file_paths_dir_names;
	
	strcpy(descript,names[dir_type]);
}

