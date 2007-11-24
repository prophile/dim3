/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Game Save/Load

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

#include "scripts.h"
#include "interfaces.h"

extern map_type			map;
extern hud_type			hud;
extern camera_type		camera;
extern view_type		view;
extern server_type		server;
extern js_type			js;
extern setup_type		setup;

unsigned long			game_file_sz,game_file_pos;
char					game_file_last_save_name[256];
ptr						game_file_data;

extern void game_time_pause_start(void);
extern void game_time_pause_end(void);
extern bool game_start(int remote_count,network_request_remote_add *remotes,char *err_str);
extern bool map_start(bool skip_media,char *err_str);
extern void map_end(void);
extern void game_time_reset(void);
extern int game_time_get(void);
extern void game_time_set(int tick);
extern void view_capture_draw(int tick,char *path);

/* =======================================================

      Initialize and Last Saves
      
======================================================= */

void game_file_initialize(void)
{
	game_file_last_save_name[0]=0x0;
}

/* =======================================================

      Chunks
      
======================================================= */

bool game_file_add_chunk(void *data,int sz)
{
	ptr			nptr;
	
	nptr=realloc(game_file_data,game_file_sz+(sz+sizeof(int)));
	if (nptr==NULL) return(FALSE);
	
	game_file_data=nptr;
	
	memmove((game_file_data+game_file_sz),&sz,sizeof(int));
	game_file_sz+=4;
	
	memmove((game_file_data+game_file_sz),data,sz);
	game_file_sz+=sz;
	
	return(TRUE);
}

char* game_file_get_chunk(void *data)
{
	int			sz;
	
	memmove(&sz,(game_file_data+game_file_pos),sizeof(int));
	game_file_pos+=4;
	
	if (data==NULL) {
		data=valloc(sz);
		if (data==NULL) return(NULL);
	}
	
	memmove(data,(game_file_data+game_file_pos),sz);
	game_file_pos+=sz;
	
	return(data);
}

/* =======================================================

      Files
      
======================================================= */

bool game_file_compress_save(char *path,char *err_str)
{
	int				err;
	unsigned long   compress_sz;
	ptr				compress_data;
	FILE			*file;
	
		// compress it
		
	compress_sz=game_file_sz+(int)(((double)game_file_sz)*0.1)+12;
	compress_data=valloc(compress_sz);
	if (compress_data==NULL) {
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}

	err=compress2(compress_data,&compress_sz,game_file_data,game_file_sz,9);
	if (err!=Z_OK) {
		sprintf(err_str,"Could not compress file (%d: %s)",err,zError(err));
		free(compress_data);
		return(FALSE);
	}
	
		// save file
		
	file=fopen(path,"wb");
	if (file==NULL) {
		strcpy(err_str,"Could not create file");
		free(compress_data);
		return(FALSE);
	}
		
	fwrite(&game_file_sz,1,sizeof(unsigned long),file);
	fwrite(&compress_sz,1,sizeof(unsigned long),file);
	fwrite(compress_data,1,compress_sz,file);
	fclose(file);
	
#ifndef D3_OS_WINDOWS
	truncate(path,((sizeof(unsigned long)*2)+compress_sz));
#endif

	free(compress_data);
	
	return(TRUE);
}

bool game_file_expand_load(char *path,char *err_str)
{
	int				err;
	unsigned long   file_sz,compress_sz;
	ptr				compress_data;
	FILE			*file;
	struct stat		sb;
	
		// get file size

	if (stat(path,&sb)!=0) {
		strcpy(err_str,"File does not exist");
		return(FALSE);
	}
	file_sz=sb.st_size;
	
	compress_data=valloc(file_sz);
	if (compress_data==NULL) {
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}
	
		// get compressed data
		
	file=fopen(path,"rb");
	if (file==NULL) {
		strcpy(err_str,"Could not open file");
		free(compress_data);
		return(FALSE);
	}
	
	fread(&game_file_sz,1,sizeof(unsigned long),file);
	fread(&compress_sz,1,sizeof(unsigned long),file);
	fread(compress_data,1,compress_sz,file);
	fclose(file);
	
		// decompress file
		
	game_file_data=valloc(game_file_sz);
	if (game_file_data==NULL) {
		strcpy(err_str,"Out of memory");
		free(compress_data);
		return(FALSE);
	}
	
	err=uncompress(game_file_data,&game_file_sz,compress_data,compress_sz);
	if (err!=Z_OK) {
		sprintf(err_str,"Could not uncompress file (%d: %s)",err,zError(err));
		free(compress_data);
		free(game_file_data);
		return(FALSE);
	}
	
	free(compress_data);
	
	return(TRUE);
}

/* =======================================================

      Save Game
      
======================================================= */

bool game_file_save(char *err_str)
{
	int				n,tick,seg_idx,ntouch_segment,sec;
	char			path[1024],file_name[256],elapse_str[32],time_str[32];
	bool			ok;
	struct tm		*tm;
	time_t			curtime;
	obj_type		*obj;
	weapon_type		*weap;
	proj_setup_type	*proj_setup;
	segment_type	*seg;
	
	progress_initialize("Saving");
	progress_draw(5);
	
		// get saved data file names
		
	tick=game_time_get();
	
	curtime=time(NULL);
	tm=localtime(&curtime);
	sprintf(time_str,"%.4d%.2d%.2d%.2d%.2d",(tm->tm_year+1900),(tm->tm_mon+1),tm->tm_mday,tm->tm_hour,tm->tm_min);
		
	sec=(game_time_get()-map.start_game_tick)/1000;
	if (sec>(99*60)) sec=99*60;
	sprintf(elapse_str,"%.2d%.2d",(sec/60),(sec%60));
	
		// save screen
		
	sprintf(file_name,"%s_%s_%s",time_str,elapse_str,map.info.name);
	file_paths_documents(&setup.file_path_setup,path,"Saved Games",file_name,"png");
	
	view_capture_draw(tick,path);
	
		// start chunks
		
	game_file_sz=0;
	game_file_data=valloc(32);

		// version
		
	game_file_add_chunk(dim3_version,16);
	
		// map name
		
	game_file_add_chunk(&map.info.name,name_str_len);

		// timing
		
	game_file_add_chunk(&tick,sizeof(int));
	
		// send scripts save state
		
	progress_draw(10);
	
	script_state_save(&js.game_attach);
	script_state_save(&js.course_attach);
	
	obj=server.objs;
	for (n=0;n!=server.count.obj;n++) {
		script_state_save(&obj->attach);
		obj++;
	}
	
	weap=server.weapons;
	for (n=0;n!=server.count.weapon;n++) {
		script_state_save(&weap->attach);
		weap++;
	}
	
	proj_setup=server.proj_setups;
	for (n=0;n!=server.count.proj_setup;n++) {
		script_state_save(&proj_setup->attach);
		proj_setup++;
	}
	
		// add structures
		
	progress_draw(20);
		
	game_file_add_chunk(&view.time,sizeof(view_time_type));
	game_file_add_chunk(&view.fps,sizeof(view_fps_type));
	game_file_add_chunk(&camera,sizeof(camera_type));
	
	game_file_add_chunk(&server.time,sizeof(server_time_type));
	game_file_add_chunk(&server.player_obj_uid,sizeof(int));
	
	progress_draw(30);
	
	game_file_add_chunk(&server.uid,sizeof(server_uid_type));
	game_file_add_chunk(&server.count,sizeof(server_count_type));
	game_file_add_chunk(&hud.count,sizeof(hud_count_type));
	
	progress_draw(40);

	game_file_add_chunk(server.objs,(server.count.obj*sizeof(obj_type)));
	game_file_add_chunk(server.weapons,(server.count.weapon*sizeof(weapon_type)));
	game_file_add_chunk(server.proj_setups,(server.count.proj_setup*sizeof(proj_setup_type)));
	
	progress_draw(50);
	
	game_file_add_chunk(server.projs,(server.count.proj*sizeof(proj_type)));
	game_file_add_chunk(server.effects,(server.count.effect*sizeof(effect_type)));
	game_file_add_chunk(server.decals,(server.count.decal*sizeof(decal_type)));
	
	progress_draw(60);
	
	game_file_add_chunk(hud.bitmaps,(hud.count.bitmap*sizeof(hud_bitmap_type)));
	game_file_add_chunk(hud.texts,(hud.count.text*sizeof(hud_text_type)));

	game_file_add_chunk(&map.fog,sizeof(map_fog_type));
	
		// touched segments and movements
		
	progress_draw(70);
	
	ntouch_segment=map_segments_count_touch(&map);
	
	game_file_add_chunk(&map.nsegment,sizeof(int));
	game_file_add_chunk(&ntouch_segment,sizeof(int));
	
	seg=map.segments;
	
	for (n=0;n<map.nsegment;n++) {
		if (seg->touched) {
			seg_idx=n;
			game_file_add_chunk(&seg_idx,sizeof(int));
			game_file_add_chunk(seg,sizeof(segment_type));
		}
		seg++;
	}
	
	game_file_add_chunk(map.movements,sizeof(movement_type)*map.nmovement);
	
		// timers, moves and globals
		
	progress_draw(80);
	
	game_file_add_chunk(&js.script_current_uid,sizeof(int));
	game_file_add_chunk(&js.count,sizeof(script_count_type));
	game_file_add_chunk(&js.time,sizeof(script_time_type));
		
	game_file_add_chunk(js.timers,(js.count.timer*sizeof(timer_type)));
	game_file_add_chunk(js.moves,(js.count.move*sizeof(move_type)));
	game_file_add_chunk(js.globals,(js.count.global*sizeof(global_type)));

		// compress and save
		
	progress_draw(90);
		
	file_paths_documents(&setup.file_path_setup,path,"Saved Games",file_name,"sav");
	ok=game_file_compress_save(path,err_str);
	
	progress_draw(100);
	
	free(game_file_data);
	
		// remember last map
		
	strcpy(game_file_last_save_name,strrchr(path,'/'));
	
		// finished
		
	progress_shutdown();
    
    return(ok);
}

/* =======================================================

      Load Game
      
======================================================= */

bool game_file_load(char *file_name,char *err_str)
{
	int				n,tick,ntouch_segment,seg_idx;
	char			*c,path[1024],fname[256],name[256],vers[16];
	obj_type		*obj;
	weapon_type		*weap;
	proj_setup_type	*proj_setup;

		// load and expand
		
	strcpy(fname,file_name);
	c=strrchr(fname,'.');
	if (c!=NULL) *c=0x0;			// remove any extensions
	
	file_paths_documents(&setup.file_path_setup,path,"Saved Games",fname,"sav");
	if (!game_file_expand_load(path,err_str)) return(FALSE);
	
	game_file_pos=0;
	
		// if game isn't running, then start
		
	if (!server.game_open) {
		if (!game_start(0,NULL,err_str)) {
			free(game_file_data);
			return(FALSE);
		}
	}

		// version
		
	game_file_get_chunk(vers);
	if (strcmp(vers,dim3_version)!=0) {
		sprintf(err_str,"This saved game file is from a different version of dim3");
		free(game_file_data);
		return(FALSE);
	}
	
		// map name
		
	game_file_get_chunk(name);
		
		// reload map
	
	if ((!server.map_open) || (strcmp(name,map.info.name)!=0)) {		// need to load a map?
	
		if (server.map_open) map_end();
		
		strcpy(map.info.name,name);
		map.info.player_start_name[0]=0x0;
		map.info.player_start_type[0]=0x0;
		map.info.in_load=TRUE;

		if (!map_start(TRUE,err_str)) {
			free(game_file_data);
			return(FALSE);
		}
	}
	
	progress_initialize("Loading");
	progress_draw(10);
	
		// timing
		
	game_file_get_chunk(&tick);
	game_time_set(tick);

		// load structures
		
	progress_draw(20);
					
	game_file_get_chunk(&view.time);
	game_file_get_chunk(&view.fps);
	game_file_get_chunk(&camera);
	
	game_file_get_chunk(&server.time);
	game_file_get_chunk(&server.player_obj_uid);
	
	progress_draw(30);

	game_file_get_chunk(&server.uid);
	game_file_get_chunk(&server.count);
	game_file_get_chunk(&hud.count);
	
	progress_draw(40);

	game_file_get_chunk(server.objs);
	game_file_get_chunk(server.weapons);
	game_file_get_chunk(server.proj_setups);
	
	progress_draw(50);

	game_file_get_chunk(server.projs);
	game_file_get_chunk(server.effects);
	game_file_get_chunk(server.decals);

	game_file_get_chunk(hud.bitmaps);
	game_file_get_chunk(hud.texts);
	
	game_file_get_chunk(&map.fog);
	
		// touched segments
		
	progress_draw(60);
		
	game_file_get_chunk(&map.nsegment);
	game_file_get_chunk(&ntouch_segment);

	for (n=0;n<ntouch_segment;n++) {
		game_file_get_chunk(&seg_idx);
		game_file_get_chunk(&map.segments[seg_idx]);
	}

	game_file_get_chunk(map.movements);
	
	map_portal_rebuild_vertex_lists(&map,setup.high_quality_lighting);
	
		// load timers, moves and globals
		
	progress_draw(70);

	game_file_get_chunk(&js.script_current_uid);
	game_file_get_chunk(&js.count);
	game_file_get_chunk(&js.time);
	
	progress_draw(80);

	game_file_get_chunk(js.timers);
	game_file_get_chunk(js.moves);
	game_file_get_chunk(js.globals);
	
		// send scripts load state
		
	progress_draw(90);
	
	script_state_load(&js.game_attach);
	script_state_load(&js.course_attach);
	
	obj=server.objs;
	for (n=0;n!=server.count.obj;n++) {
		script_state_load(&obj->attach);
		obj++;
	}
	
	weap=server.weapons;
	for (n=0;n!=server.count.weapon;n++) {
		script_state_load(&weap->attach);
		weap++;
	}
	
	proj_setup=server.proj_setups;
	for (n=0;n!=server.count.proj_setup;n++) {
		script_state_load(&proj_setup->attach);
		proj_setup++;
	}
	
		// free game data
		
	progress_draw(100);
	free(game_file_data);
	
		// finished

	progress_shutdown();

		// reset server timers
		
	game_time_reset();
  
    return(TRUE);
}

bool game_file_reload_ok(void)
{
	char			*c,name[256];
	
		// is there a last save?
		
	if (game_file_last_save_name[0]==0x0) return(FALSE);
	
		// find the map name for this save
		
	strcpy(name,(game_file_last_save_name+19));
	
	c=strchr(name,'.');			// remove the .sav
	if (c!=NULL) *c=0x0;

	return(strcmp(name,map.info.name)==0);
}

bool game_file_reload(char *err_str)
{
	return(game_file_load(game_file_last_save_name,err_str));
}

