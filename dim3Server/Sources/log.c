/****************************** File *********************************

Module: dim3 Server
Author: Brian Barnes
 Usage: Network Host Logging

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
	#include "dim3server.h"
#endif

extern int					up_time;
extern file_path_setup_type	file_path_setup;

FILE						*log_file;
pthread_mutex_t				server_log_lock;

int							last_log_line_count;
server_last_log_line_type	*last_log_lines;

/* =======================================================

      Start/Stop Logging
      
======================================================= */

void log_start(void)
{
	char		path[1024];

	pthread_mutex_init(&server_log_lock,NULL);

	file_paths_documents(&file_path_setup,path,"Logs","dim3ServerLog","txt");
	log_file=fopen(path,"w");
	
	last_log_line_count=0;
	last_log_lines=valloc(server_max_last_log_line*sizeof(server_last_log_line_type));
}

void log_end(void)
{
	if (log_file!=NULL) fclose(log_file);
	if (last_log_lines!=NULL) free(last_log_lines);
	
	pthread_mutex_destroy(&server_log_lock);
}

/* =======================================================

      Get Last Log Line
      
======================================================= */

bool log_get_last_line(int idx,char *time_str,char *player_name,char *str)
{
	if (idx>=last_log_line_count) return(FALSE);
	
		// lock log
		
	pthread_mutex_lock(&server_log_lock);
	
		// get strings
		
	strcpy(time_str,last_log_lines[idx].time_str);
	strcpy(player_name,last_log_lines[idx].player_name);
	strcpy(str,last_log_lines[idx].str);
	
		// unlock log
		
	pthread_mutex_unlock(&server_log_lock);
	
	return(TRUE);
}

/* =======================================================

      Write Logging
      
======================================================= */

void log_write(char *player_name,char *str)
{
	time_t		gmttime;
	struct tm	ltime;
	char		time_str[24],sub_player_name[32];

		// check for NULL names

	if (player_name==NULL) {
		player_name=sub_player_name;
		strcpy(sub_player_name,"*");
	}

		// lock log
		
	pthread_mutex_lock(&server_log_lock);
	
		// write log line
	
	time(&gmttime);
	ltime=*localtime(&gmttime);
	
	strftime(time_str,24,"%Y-%m-%d %H:%M.%S",&ltime);
	
	if (log_file!=NULL) {
		fwrite(time_str,1,strlen(time_str),log_file);
		fwrite("\t",1,1,log_file);

		fwrite(player_name,1,strlen(player_name),log_file);
		fwrite("\t",1,1,log_file);

		fwrite(str,1,strlen(str),log_file);
		fwrite("\n",1,1,log_file);
	
		fflush(log_file);
	}
	
		// add to last log lines
		
	if (last_log_line_count==(server_max_last_log_line-1)) {
		memmove(&last_log_lines[0],&last_log_lines[1],(sizeof(server_last_log_line_type)*(server_max_last_log_line-1)));
	}
	
	strcpy(last_log_lines[last_log_line_count].time_str,time_str);
	strcpy(last_log_lines[last_log_line_count].player_name,player_name);
	strncpy(last_log_lines[last_log_line_count].str,str,128);
	last_log_lines[last_log_line_count].str[127]=0x0;
	
	if (last_log_line_count!=(server_max_last_log_line-1)) {
		last_log_line_count++;
	}

		// unlock log
		
	pthread_mutex_unlock(&server_log_lock);
}

/* =======================================================

      Write Logging Utilities
      
======================================================= */

void log_write_value(char *player_name,char *name,char *value)
{
	char			str[1024];
	
	sprintf(str,"%s: %s",name,value);
	log_write(player_name,str);
}

void log_write_error(char *player_name,char *err_str)
{
	char			str[1024];
	
	sprintf(str,"Error: %s",err_str);
	log_write(player_name,str);
}

