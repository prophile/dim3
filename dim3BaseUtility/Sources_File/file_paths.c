/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: File Pathing

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

      Set File Paths
            
======================================================= */

bool file_paths_setup(file_path_setup_type *file_path_setup,bool app_name_override)
{
	struct stat		sb;

		// OS specific app path and name

#ifdef D3_OS_MAC	
	char			*c;
	CFURLRef		cfurl;
	
	cfurl=CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFURLGetFileSystemRepresentation(cfurl,TRUE,(unsigned char*)file_path_setup->path_base,1024);
	
		// get name
	
	c=strrchr(file_path_setup->path_base,'/');
	
	file_path_setup->app_name[0]=0x0;
	if (c!=NULL) {
		strcpy(file_path_setup->app_name,(c+1));
		*c=0x0;
		
		c=strchr(file_path_setup->app_name,'.');
		if (c!=NULL) *c=0x0;
	}
	
#endif

#ifdef D3_OS_LINUX
	pid_t			pid;
	char			proc_pid_exe[PATH_MAX],executable_path[PATH_MAX];
	char			*split;

	pid=getpid();
	sprintf(proc_pid_exe,"/proc/%d/exe",(int)pid);

	memset(executable_path,0,PATH_MAX);
	readlink(proc_pid_exe,executable_path,(PATH_MAX-1));

    *split = strrchr(executable_path, '/');
    ssize_t base_length = split - executable_path;
    
    strncpy(file_path_setup->path_base,executable_path, base_length);
    file_path_setup->path_base[base_length]=0x0;
    
    strcpy(file_path_setup->app_name,(split+1));
    
#endif

#ifdef D3_OS_WINDOWS
	char			*c;

	GetModuleFileName(NULL,file_path_setup->path_base,1024);

	if (file_path_setup->path_base[0]=='\"') {				// parse out the quotes
		strcpy(file_path_setup->path_base,(char*)&file_path_setup->path_base[1]);
		c=strchr(file_path_setup->path_base,'\"');
		if (c!=NULL) *c=0x0;
	}

	c=strrchr(file_path_setup->path_base,'\\');
	
	file_path_setup->app_name[0]=0x0;
	if (c!=NULL) {
		strcpy(file_path_setup->app_name,(c+1));
		*c=0x0;
		
		c=strchr(file_path_setup->app_name,'.');
		if (c!=NULL) *c=0x0;
	}
	
#endif

		// app_name_override is used when this is being called from
		// outside dim3 Engine app.  At this point it can only look
		// inside the engine if the name remains the dim3 Engine.app

	strcpy(file_path_setup->org_app_name,file_path_setup->app_name);
	
	if (app_name_override) {
		strcpy(file_path_setup->app_name,"dim3 Engine");
	}

		// check all the possible paths
	
		// app data path
		
#ifdef D3_OS_MAC	
	sprintf(file_path_setup->path_app,"%s/%s.app/Contents/Data",file_path_setup->path_base,file_path_setup->app_name);
	if (stat(file_path_setup->path_app,&sb)!=0) file_path_setup->path_app[0]=0x0;
#else
	file_path_setup->path_app[0]=0x0;
#endif

		// first data path
		
	sprintf(file_path_setup->path_data,"%s/Data",file_path_setup->path_base);
	if (stat(file_path_setup->path_data,&sb)!=0) file_path_setup->path_data[0]=0x0;
	
		// second data path
	
	sprintf(file_path_setup->path_data_2,"%s/Data2",file_path_setup->path_base);
	if (stat(file_path_setup->path_data_2,&sb)!=0) file_path_setup->path_data_2[0]=0x0;
	
	return((file_path_setup->path_app[0]!=0x0) || (file_path_setup->path_data[0]!=0x0) || (file_path_setup->path_data_2[0]!=0x0));
}

/* =======================================================

      Get File Path
            
======================================================= */

void file_paths_data(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name)
{
	char				add_path[1024];
	struct stat			sb;
	
		// construct path
		
	add_path[0]=0x0;
	
	if (sub_path!=NULL) {
		strcat(add_path,sub_path);
	}

	if (file_name!=NULL) {
		strcat(add_path,"/");
		strcat(add_path,file_name);
		strcat(add_path,".");
		strcat(add_path,ext_name);
	}
	
		// check where file or directory exists
		
	if (file_path_setup->path_data_2[0]!=0x0) {
		sprintf(path,"%s/%s",file_path_setup->path_data_2,add_path);
		if (stat(path,&sb)==0) return;
	}
	
	if (file_path_setup->path_data[0]!=0x0) {
		sprintf(path,"%s/%s",file_path_setup->path_data,add_path);
		if (stat(path,&sb)==0) return;
	}
	
	if (file_path_setup->path_app[0]!=0x0) {
		sprintf(path,"%s/%s",file_path_setup->path_app,add_path);
		if (stat(path,&sb)==0) return;
	}
	
		// if file not found, default to data path
		
	sprintf(path,"%s/%s",file_path_setup->path_data,add_path);
}

void file_paths_data_default(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name)
{
	char				add_path[1024];
	
		// construct path
		
	add_path[0]=0x0;
	
	if (sub_path!=NULL) {
		strcat(add_path,sub_path);
	}

	if (file_name!=NULL) {
		strcat(add_path,"/");
		strcat(add_path,file_name);
		strcat(add_path,".");
		strcat(add_path,ext_name);
	}
	
		// don't check file, but use highest data
		// folder
		
	if (file_path_setup->path_data_2[0]!=0x0) {
		sprintf(path,"%s/%s",file_path_setup->path_data_2,add_path);
		return;
	}
	
	if (file_path_setup->path_data[0]!=0x0) {
		sprintf(path,"%s/%s",file_path_setup->path_data,add_path);
		return;
	}
	
	if (file_path_setup->path_app[0]!=0x0) {
		sprintf(path,"%s/%s",file_path_setup->path_app,add_path);
		return;
	}
		
	sprintf(path,"%s/%s",file_path_setup->path_data,add_path);
}

void file_paths_app(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name)
{
	strcpy(path,file_path_setup->path_base);
	strcat(path,"/");
	strcat(path,file_path_setup->org_app_name);
	strcat(path,".app");
	
	if (sub_path!=NULL) {
		strcat(path,"/");
		strcat(path,sub_path);
	}

	if (file_name!=NULL) {
		strcat(path,"/");
		strcat(path,file_name);
		strcat(path,".");
		strcat(path,ext_name);
	}
}

void file_paths_base(file_path_setup_type *file_path_setup,char *path,char *file_name,char *ext_name)
{
	strcpy(path,file_path_setup->path_base);

	if (file_name!=NULL) {
		strcat(path,"/");
		strcat(path,file_name);
		strcat(path,".");
		strcat(path,ext_name);
	}
}

/* =======================================================

      Document Directories
            
======================================================= */

void file_paths_create_directory(char *path)
{
	struct stat			sb;

	if (stat(path,&sb)==0) return;

#ifndef D3_OS_WINDOWS
	mkdir(path,S_IRWXU|S_IRWXG|S_IRWXO);
#else
	CreateDirectory(path,NULL);
#endif
}

void file_paths_documents(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name)
{
		// get the documents path
		
#ifdef D3_OS_MAC
	FSRef		fsref;
	
	FSFindFolder(kUserDomain,kDocumentsFolderType,kDontCreateFolder,&fsref);
	FSRefMakePath(&fsref,(unsigned char*)path,1024);
#endif

#ifdef D3_OS_LINUX
	strcpy(path,file_path_setup->path_base);
#endif

#ifdef D3_OS_WINDOWS
	SHGetSpecialFolderPath(NULL,path,CSIDL_PERSONAL,FALSE);
#endif

		// get game's document directory based
		// on application name and create if necessary
		
	strcat(path,"/");
	strcat(path,file_path_setup->app_name);
		
	file_paths_create_directory(path);
	
		// sub-path in game documents
		
	strcat(path,"/");
	strcat(path,sub_path);
	
	file_paths_create_directory(path);
	
		// file name
	
	if (file_name!=NULL) {
		strcat(path,"/");
		strcat(path,file_name);
		strcat(path,".");
		strcat(path,ext_name);
	}
}

bool file_paths_documents_exist(file_path_setup_type *file_path_setup,char *path,char *sub_path,char *file_name,char *ext_name)
{
	struct stat			sb;
	
	file_paths_documents(file_path_setup,path,sub_path,file_name,ext_name);
	return(stat(path,&sb)==0);
}

