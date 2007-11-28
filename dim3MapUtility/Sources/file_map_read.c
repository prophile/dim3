/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map File Reading

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
	#include "dim3maputility.h"
#endif

extern maputility_settings_type		maputility_settings;

char					media_type_str[][32]={"none","story","title","movie"},
						sky_type_str[][32]={"globe","dome_panoramic","dome_hemisphere","cylinder","cube"},
						gl_fog_type_str[][32]={"linear","exp","exp2"},
						segment_type_str[][32]={"wall","floor","ceiling","liquid","ambient",""},
                        segment_clip_str[][32]={"none","top","bottom","slant","top_curve","top_arch","bottom_curve","bottom_arch","top_curve_arch","bottom_curve_arch",""},
                        segment_curve_str[][32]={"none","forward","backward",""},
                        segment_push_str[][32]={"none","top","bottom","left","right","up","down",""},
						liquid_tide_direction_str[][32]={"horizontal","vertical",""},
                        light_type_str[][32]={"normal","blink","glow","pulse","flicker","failing",""},
						lighting_mode_str[][32]={"flat","hilite","hilite_diffuse","vertex"},
						map_bump_mode_str[][32]=bump_mode_xml_list_str;


bool map_convert_segment_to_mesh(map_type *map);	// supergumba -- new mesh stuff, forward reference

/* =======================================================

      Read Segment XML
      
======================================================= */

void read_single_segment(int tag,segment_type *seg,int rn,int seg_type)
{
	int				n;
	
	seg->rn=rn;
	seg->type=seg_type;
	seg->group_idx=xml_get_attribute_int_default(tag,"group",-1);
	
	for (n=0;n!=max_primitive_stack;n++) {
		seg->primitive_uid[n]=-1;
	}
	
	xml_get_attribute_short_array(tag,"primitive",seg->primitive_uid,max_primitive_stack);
	
	seg->clip=xml_get_attribute_list(tag,"clip",(char*)segment_clip_str);
	seg->curve=xml_get_attribute_list(tag,"curve",(char*)segment_curve_str);
	seg->pass_through=xml_get_attribute_boolean(tag,"pass");
	seg->moveable=xml_get_attribute_boolean(tag,"moveable");
	seg->shiftable=xml_get_attribute_boolean(tag,"shiftable");
	seg->on=!xml_get_attribute_boolean(tag,"off");
	
	seg->fill=xml_get_attribute_int(tag,"fill");
	seg->txt_ang=xml_get_attribute_int_default(tag,"ang",0)/90;
	xml_get_attribute_2_coord_float(tag,"uv_off",&seg->x_txtoff,&seg->y_txtoff);
	xml_get_attribute_2_coord_float(tag,"uv_fct",&seg->x_txtfact,&seg->y_txtfact);
	xml_get_attribute_2_coord_float_default(tag,"shift",&seg->x_shift,&seg->y_shift,0.0f,0.0f);
	seg->dark_factor=xml_get_attribute_float_default(tag,"dark_fct",1);
	seg->alpha=xml_get_attribute_float_default(tag,"alpha",1);
	seg->lock=xml_get_attribute_boolean(tag,"lock");
	
	seg->tag=xml_get_attribute_int_default(tag,"tag",0);
	
	if (seg_type==sg_liquid) {
		seg->data.liquid.harm=xml_get_attribute_int(tag,"harm");
		seg->data.liquid.drown_harm=xml_get_attribute_int_default(tag,"drown_harm",10);
		seg->data.liquid.drown_tick=xml_get_attribute_int_default(tag,"drown_tick",10000);
		seg->data.liquid.speed_alter=xml_get_attribute_float_default(tag,"speed_alter",0.1f);
		seg->data.liquid.wavesize=xml_get_attribute_int_default(tag,"wave_size",(map_enlarge<<2));
		seg->data.liquid.tiderate=xml_get_attribute_int(tag,"tide_rate");
		seg->data.liquid.tidesize=xml_get_attribute_int(tag,"tide_size");
		seg->data.liquid.tidedirection=xml_get_attribute_list(tag,"tide_direction",(char*)liquid_tide_direction_str);
		xml_get_attribute_color(tag,"rgb",&seg->data.liquid.col);
		seg->data.liquid.tint_alpha=xml_get_attribute_float_default(tag,"tint_alpha",0.5f);
		
		if (seg->data.liquid.wavesize<=(map_enlarge<<2)) seg->data.liquid.wavesize=map_enlarge<<2;
		if (seg->data.liquid.tiderate<0) seg->data.liquid.tiderate=0;
		if (seg->data.liquid.tidesize<0) seg->data.liquid.tidesize=0;
	}
	
	if (seg_type==sg_ambient_wall) {
		seg->data.ambient_wall.push=xml_get_attribute_list(tag,"push",(char*)segment_push_str);
	}

	if (seg_type==sg_ambient_fc) {
		seg->data.ambient_fc.push=xml_get_attribute_list(tag,"push",(char*)segment_push_str);
	}
}

/* =======================================================

      Read Map XML
      
======================================================= */

bool read_map_xml(map_type *map)
{
	int						i,k,j,y,idx,map_tag,main_ambient_light_tag,main_ambient_sound_tag,main_rain_tag,
							main_background_tag,main_sky_tag,main_fog_tag,main_group_tag,group_tag,
							main_movement_tag,movement_tag,main_movement_move_tag,movement_move_tag,main_fill_tag,fill_tag,
							main_portal_tag,portal_tag,msg_tag,main_path_tag,path_tag,
							main_seg_tag,seg_tag,main_light_tag,light_tag,main_sound_tag,sound_tag,
							main_particle_tag,particle_tag,main_node_tag,node_tag,
							main_obj_tag,obj_tag,tag,id,cnt;
    portal_type				*portal;
	portal_sight_list_type	*sight;
    segment_type			*seg;
    map_light_type			*light;
    map_sound_type			*sound;
	map_particle_type		*particle;
    node_type				*node;
    spot_type				*spot;
	map_scenery_type		*scenery;
	movement_type			*movement;
	movement_move_type		*move;

		// supergumba -- NEW COLLIDE -- editing here also
//	segment_type temp_seg;
	
	if (!xml_open_file(map->info.load_path)) return(FALSE);
    
    map_tag=xml_findrootchild("Map");
    if (map_tag==-1) {
		xml_close_file();
		return(FALSE);
    }
	
        // map info
    
    tag=xml_findfirstchild("Info",map_tag);
    if (tag!=-1) {
        xml_get_attribute_text(tag,"name",map->info.title,name_str_len);
        xml_get_attribute_text(tag,"author",map->info.author,name_str_len);
    }
    
    tag=xml_findfirstchild("Settings",map_tag);
    if (tag!=-1) {
        map->settings.gravity=xml_get_attribute_float_default(tag,"gravity",1);
        map->settings.gravity_max_power=xml_get_attribute_float_default(tag,"gravity_max_power",32);
		map->settings.gravity_max_speed=xml_get_attribute_float_default(tag,"gravity_max_speed",400);
		map->settings.resistance=xml_get_attribute_float_default(tag,"resistance",1);
		map->settings.txt_scale_x=xml_get_attribute_float_default(tag,"txt_scale_x",0.04f);
		map->settings.txt_scale_y=xml_get_attribute_float_default(tag,"txt_scale_y",0.04f);
		map->settings.speculars_hilite=xml_get_attribute_boolean(tag,"speculars_hilite");
		map->settings.editor_link_always_start=xml_get_attribute_boolean(tag,"editor_link_always_start");
	}
	
    tag=xml_findfirstchild("Optimizations",map_tag);
    if (tag!=-1) {
		map->optimizations.lod_light_distance=xml_get_attribute_int(tag,"lod_light_distance");
		map->optimizations.lod_bump_distance=xml_get_attribute_int(tag,"lod_bump_distance");
		map->optimizations.lod_specular_distance=xml_get_attribute_int_default(tag,"lod_specular_distance",800);
		map->optimizations.lod_glow_distance=xml_get_attribute_int_default(tag,"lod_glow_distance",800);
		map->optimizations.lod_model_distance=xml_get_attribute_int(tag,"lod_model_distance");
		map->optimizations.lod_shadow_distance=xml_get_attribute_int(tag,"lod_shadow_distance");
		map->optimizations.lod_effect_distance=xml_get_attribute_int(tag,"lod_effect_distance");
	}
	
    tag=xml_findfirstchild("Media",map_tag);
    if (tag!=-1) {
		map->media.type=xml_get_attribute_list(tag,"type",(char*)media_type_str);
		xml_get_attribute_text(tag,"name",map->media.name,name_str_len);
		xml_get_attribute_text(tag,"title_sound_name",map->media.title_sound_name,name_str_len);
	}
	
    tag=xml_findfirstchild("Music",map_tag);
    if (tag!=-1) {
		map->music.fade_msec=xml_get_attribute_int(tag,"fade_msec");
		xml_get_attribute_text(tag,"name",map->music.name,name_str_len);
	}
	
	main_ambient_light_tag=xml_findfirstchild("Ambient_Light",map_tag);
    if (main_ambient_light_tag!=-1) {
		xml_get_attribute_color(main_ambient_light_tag,"rgb",&map->ambient.light_color);
		map->ambient.light_drop_off_factor=xml_get_attribute_float_default(main_ambient_light_tag,"drop_off_factor",0.75);
	}
	
	main_ambient_sound_tag=xml_findfirstchild("Ambient_Sound",map_tag);
    if (main_ambient_sound_tag!=-1) {
		xml_get_attribute_text(main_ambient_sound_tag,"name",map->ambient.sound_name,name_str_len);
		map->ambient.sound_pitch=xml_get_attribute_float_default(main_ambient_sound_tag,"pitch",1.0f);
	}
	
	main_background_tag=xml_findfirstchild("Background",map_tag);
    if (main_background_tag!=-1) {
		map->background.on=xml_get_attribute_boolean(main_background_tag,"on");
		map->background.fill=xml_get_attribute_int(main_background_tag,"fill");
		xml_get_attribute_2_coord_float(main_background_tag,"scroll",&map->background.x_scroll_fact,&map->background.y_scroll_fact);
    }
  
	main_sky_tag=xml_findfirstchild("Sky",map_tag);
    if (main_sky_tag!=-1) {
		tag=xml_findfirstchild("Settings",main_sky_tag);
		if (tag!=-1) {
			map->sky.on=xml_get_attribute_boolean(tag,"on");
			map->sky.type=xml_get_attribute_list(tag,"type",(char*)sky_type_str);
			map->sky.radius=xml_get_attribute_int(tag,"radius");
			map->sky.extra_height=xml_get_attribute_int(tag,"extra_height");
			map->sky.dome_y=xml_get_attribute_int(tag,"dome_y");
		}
		tag=xml_findfirstchild("Fill",main_sky_tag);
		if (tag!=-1) {
			map->sky.fill=xml_get_attribute_int_default(tag,"index",-1);
			map->sky.bottom_fill=xml_get_attribute_int_default(tag,"bottom_index",-1);
			map->sky.north_fill=xml_get_attribute_int_default(tag,"north_index",-1);
			map->sky.south_fill=xml_get_attribute_int_default(tag,"south_index",-1);
			map->sky.east_fill=xml_get_attribute_int_default(tag,"east_index",-1);
			map->sky.west_fill=xml_get_attribute_int_default(tag,"west_index",-1);
			map->sky.txt_fact=xml_get_attribute_float_default(tag,"factor",1);
			xml_get_attribute_2_coord_float(tag,"shift",&map->sky.txt_x_shift,&map->sky.txt_y_shift);
		}
    }

	main_fog_tag=xml_findfirstchild("Fog",map_tag);
    if (main_fog_tag!=-1) {
		map->fog.on=xml_get_attribute_boolean(main_fog_tag,"on");
		map->fog.count=xml_get_attribute_int(main_fog_tag,"count");
		map->fog.inner_radius=xml_get_attribute_int(main_fog_tag,"inner_radius");
		map->fog.outer_radius=xml_get_attribute_int(main_fog_tag,"outer_radius");
		map->fog.high=xml_get_attribute_int(main_fog_tag,"high");
		map->fog.drop=xml_get_attribute_int(main_fog_tag,"drop");
		map->fog.texture_idx=xml_get_attribute_int_default(main_fog_tag,"texture_index",-1);
		map->fog.speed=xml_get_attribute_float(main_fog_tag,"speed");
		map->fog.txt_x_fact=xml_get_attribute_float(main_fog_tag,"txt_x_fact");
		map->fog.txt_y_fact=xml_get_attribute_float(main_fog_tag,"txt_y_fact");
		xml_get_attribute_color(main_fog_tag,"rgb",&map->fog.col);
		map->fog.alpha=xml_get_attribute_float(main_fog_tag,"alpha");
		map->fog.use_solid_color=xml_get_attribute_boolean(main_fog_tag,"use_solid_color");
    }
	
	main_rain_tag=xml_findfirstchild("Rain",map_tag);
    if (main_rain_tag!=-1) {
		map->rain.on=xml_get_attribute_boolean(main_rain_tag,"on");
		map->rain.density=xml_get_attribute_int(main_rain_tag,"density");
		map->rain.radius=xml_get_attribute_int(main_rain_tag,"radius");
		map->rain.height=xml_get_attribute_int(main_rain_tag,"height");
		map->rain.speed=xml_get_attribute_int(main_rain_tag,"speed");
		map->rain.line_width=xml_get_attribute_int(main_rain_tag,"line_width");
		map->rain.line_length=xml_get_attribute_int(main_rain_tag,"line_length");
		map->rain.slant_add=xml_get_attribute_int(main_rain_tag,"slant_add");
		map->rain.slant_time_msec=xml_get_attribute_int(main_rain_tag,"slant_time_msec");
		map->rain.slant_change_msec=xml_get_attribute_int(main_rain_tag,"slant_change_msec");
		map->rain.alpha=xml_get_attribute_float(main_rain_tag,"alpha");
		xml_get_attribute_color(main_rain_tag,"start_rgb",&map->rain.start_color);
		xml_get_attribute_color(main_rain_tag,"end_rgb",&map->rain.end_color);
	}

        // groups
    
    main_group_tag=xml_findfirstchild("Groups",map_tag);
    if (main_group_tag!=-1) {
    
        map->ngroup=xml_countchildren(main_group_tag);
		group_tag=xml_findfirstchild("Group",main_group_tag);
    
        for (i=0;i!=map->ngroup;i++) {
 			xml_get_attribute_text(group_tag,"name",map->groups[i].name,name_str_len);
			group_tag=xml_findnextchild(group_tag);
		}
    }
    
        // textures
    
    main_fill_tag=xml_findfirstchild("Fills",map_tag);
    if (main_fill_tag!=-1) {
    
        cnt=xml_countchildren(main_fill_tag);
		fill_tag=xml_findfirstchild("Fill",main_fill_tag);
    
        for (i=0;i!=cnt;i++) {
			id=xml_get_attribute_int(fill_tag,"id");
			bitmap_texture_read_xml(&map->textures[id],fill_tag,TRUE);
			fill_tag=xml_findnextchild(fill_tag);
        }
    }

        // movements
    
    main_movement_tag=xml_findfirstchild("Movements",map_tag);
    if (main_movement_tag!=-1) {
    
        map->nmovement=xml_countchildren(main_movement_tag);
		movement_tag=xml_findfirstchild("Movement",main_movement_tag);
    
		movement=map->movements;
		
        for (i=0;i!=map->nmovement;i++) {
 			xml_get_attribute_text(movement_tag,"name",movement->name,name_str_len);
			movement->group_idx=xml_get_attribute_int(movement_tag,"group");
			movement->reverse_group_idx=xml_get_attribute_int_default(movement_tag,"group_reverse",-1);
 			movement->auto_start=xml_get_attribute_boolean(movement_tag,"auto_start");
 			movement->auto_open=xml_get_attribute_boolean(movement_tag,"auto_open");
 			movement->auto_open_never_close=xml_get_attribute_boolean(movement_tag,"auto_open_never_close");
 			movement->auto_open_distance=xml_get_attribute_int(movement_tag,"auto_open_distance");
			movement->loop=xml_get_attribute_boolean(movement_tag,"loop");
			
			movement->nmove=0;
			
			main_movement_move_tag=xml_findfirstchild("Moves",movement_tag);
			if (main_movement_move_tag!=-1) {
    
				movement->nmove=xml_countchildren(main_movement_move_tag);
				movement_move_tag=xml_findfirstchild("Move",main_movement_move_tag);
			
				move=movement->moves;
				
				for (k=0;k!=movement->nmove;k++) {
					xml_get_attribute_3_coord_int(movement_move_tag,"c3",&move->mov.x,&move->mov.y,&move->mov.z);
					move->msec=(short)xml_get_attribute_int(movement_move_tag,"msec");
					move->user_id=(short)xml_get_attribute_int(movement_move_tag,"user_id");
					xml_get_attribute_text(movement_move_tag,"sound_name",move->sound_name,name_str_len);
					move->sound_pitch=xml_get_attribute_float(movement_move_tag,"sound_pitch");
					
					movement_move_tag=xml_findnextchild(movement_move_tag);
					move++;
				}
			
			}
			
			movement_tag=xml_findnextchild(movement_tag);
			movement++;
		}
    }
        
        // portals

    main_portal_tag=xml_findfirstchild("Portals",map_tag);
    if (main_portal_tag!=-1) {
    
        map->nportal=xml_countchildren(main_portal_tag);
		portal_tag=xml_findfirstchild("Portal",main_portal_tag);
		
        portal=map->portals;
    
        for (i=0;i!=map->nportal;i++) {
			xml_get_attribute_text(portal_tag,"name",portal->name,name_str_len);
			
            xml_get_attribute_3_coord_int(portal_tag,"tl_c3",&portal->x,&y,&portal->z);
            xml_get_attribute_3_coord_int(portal_tag,"br_c3",&portal->ex,&y,&portal->ez);
   
            portal->msg.entry_on=portal->msg.exit_on=portal->msg.base_on=portal->msg.map_change_on=FALSE;
            portal->msg.entry_id=portal->msg.exit_id=0;
			portal->msg.map_name[0]=0x0;
			strcpy(portal->msg.map_spot_name,"Start");
			strcpy(portal->msg.map_spot_type,"Player");
            
            msg_tag=xml_findfirstchild("Messages",portal_tag);
            if (msg_tag!=-1) {
                tag=xml_findfirstchild("Entry",msg_tag);
                if (tag!=-1) {
                    portal->msg.entry_on=xml_get_attribute_boolean(tag,"on");
                    portal->msg.entry_id=xml_get_attribute_int(tag,"id");
                }
                tag=xml_findfirstchild("Exit",msg_tag);
                if (tag!=-1) {
                    portal->msg.exit_on=xml_get_attribute_boolean(tag,"on");
                    portal->msg.exit_id=xml_get_attribute_int(tag,"id");
                }
                tag=xml_findfirstchild("Base",msg_tag);
                if (tag!=-1) {
                    portal->msg.base_on=xml_get_attribute_boolean(tag,"on");
                    portal->msg.base_team=xml_get_attribute_int(tag,"team");
                }
                tag=xml_findfirstchild("Map",msg_tag);
                if (tag!=-1) {
                    portal->msg.map_change_on=xml_get_attribute_boolean(tag,"on");
					xml_get_attribute_text(tag,"name", portal->msg.map_name,name_str_len);
					xml_get_attribute_text(tag,"spot_name", portal->msg.map_spot_name,name_str_len);
					xml_get_attribute_text(tag,"spot_type", portal->msg.map_spot_type,name_str_len);
                }
            }

				// default some items

			portal->mesh.nmesh=0;
			portal->mesh.meshes=NULL;
        
                // paths
                
            main_path_tag=xml_findfirstchild("Paths",portal_tag);
            if (main_path_tag!=-1) {
                
                cnt=xml_countchildren(main_path_tag);
				path_tag=xml_findfirstchild("Path",main_path_tag);
                
                for (k=0;k!=cnt;k++) {
					id=xml_get_attribute_int(path_tag,"id");
					sight=&portal->sight[id];
					
					sight->rn=xml_get_attribute_int(path_tag,"portal");
					xml_get_attribute_short_array(path_tag,"link",(short*)sight->link,max_sight_link);
					sight->root=xml_get_attribute_boolean(path_tag,"root");
                    
					path_tag=xml_findnextchild(path_tag);
                }
             }
			
				// walls
				
            main_seg_tag=xml_findfirstchild("Walls",portal_tag);
            if (main_seg_tag!=-1) {
                
                cnt=xml_countchildren(main_seg_tag);
				seg_tag=xml_findfirstchild("Wall",main_seg_tag);
                
                for (k=0;k!=cnt;k++) {
					seg=&map->segments[map->nsegment];
					map->nsegment++;
					
					read_single_segment(seg_tag,seg,i,sg_wall);

					tag=xml_findfirstchild("v",seg_tag);
					xml_get_attribute_3_coord_int(tag,"c3",&seg->data.wall.lx,&seg->data.wall.ty,&seg->data.wall.lz);
					tag=xml_findnextchild(tag);
					xml_get_attribute_3_coord_int(tag,"c3",&seg->data.wall.rx,&seg->data.wall.by,&seg->data.wall.rz);


					// supergumba -- NEW COLLIDE -- start -- testing new collisions


					/*
					memmove(&temp_seg,seg,sizeof(segment_type));

					switch (seg->clip) {
						case wc_top:
							seg->data.fc.ptsz=3;
							seg->data.fc.x[0]=seg->data.fc.x[2]=temp_seg.data.wall.lx;
							seg->data.fc.x[1]=temp_seg.data.wall.rx;
							seg->data.fc.z[0]=seg->data.fc.z[2]=temp_seg.data.wall.lz;
							seg->data.fc.z[1]=temp_seg.data.wall.rz;
							seg->data.fc.y[0]=temp_seg.data.wall.ty;
							seg->data.fc.y[1]=seg->data.fc.y[2]=temp_seg.data.wall.by+1;
							seg->simple_tessel=TRUE;
							break;
						case wc_bottom:
							seg->data.fc.ptsz=3;
							seg->data.fc.x[0]=seg->data.fc.x[2]=temp_seg.data.wall.lx;
							seg->data.fc.x[1]=temp_seg.data.wall.rx;
							seg->data.fc.z[0]=seg->data.fc.z[2]=temp_seg.data.wall.lz;
							seg->data.fc.z[1]=temp_seg.data.wall.rz;
							seg->data.fc.y[0]=temp_seg.data.wall.by+1;
							seg->data.fc.y[1]=seg->data.fc.y[2]=temp_seg.data.wall.ty;
							seg->simple_tessel=TRUE;
							break;
						default:
							seg->data.fc.ptsz=4;
							seg->data.fc.x[0]=seg->data.fc.x[3]=temp_seg.data.wall.lx;
							seg->data.fc.x[1]=seg->data.fc.x[2]=temp_seg.data.wall.rx;
							seg->data.fc.z[0]=seg->data.fc.z[3]=temp_seg.data.wall.lz;
							seg->data.fc.z[1]=seg->data.fc.z[2]=temp_seg.data.wall.rz;
							seg->data.fc.y[0]=seg->data.fc.y[1]=temp_seg.data.wall.ty;
							seg->data.fc.y[2]=seg->data.fc.y[3]=temp_seg.data.wall.by+1;
							break;
					}
					
					seg->type=sg_floor;
					seg->clip=wc_none;
					seg->curve=cv_none;

					seg->x_txtoff=seg->y_txtoff=0.0f;
					seg->x_txtfact=seg->y_txtfact=0.0f;
					*/

					// supergumba -- NEW COLLIDE -- end

					seg_tag=xml_findnextchild(seg_tag);
				}
			}

				// floors
				
            main_seg_tag=xml_findfirstchild("Floors",portal_tag);
            if (main_seg_tag!=-1) {
                
                cnt=xml_countchildren(main_seg_tag);
				seg_tag=xml_findfirstchild("Floor",main_seg_tag);
                
                for (k=0;k!=cnt;k++) {
					seg=&map->segments[map->nsegment];
					map->nsegment++;
					
					read_single_segment(seg_tag,seg,i,sg_floor);
					
					seg->data.fc.ptsz=xml_countchildren(seg_tag);
					tag=xml_findfirstchild("v",seg_tag);
					
					for (j=0;j!=seg->data.fc.ptsz;j++) {
						xml_get_attribute_3_coord_int(tag,"c3",&seg->data.fc.x[j],&seg->data.fc.y[j],&seg->data.fc.z[j]);
						tag=xml_findnextchild(tag);
					}
					
					seg_tag=xml_findnextchild(seg_tag);
				}
			}
			
				// ceilings
				
            main_seg_tag=xml_findfirstchild("Ceilings",portal_tag);
            if (main_seg_tag!=-1) {
                
                cnt=xml_countchildren(main_seg_tag);
				seg_tag=xml_findfirstchild("Ceiling",main_seg_tag);
                
                for (k=0;k!=cnt;k++) {
					seg=&map->segments[map->nsegment];
					map->nsegment++;
					
					read_single_segment(seg_tag,seg,i,sg_ceiling);
					
					seg->data.fc.ptsz=xml_countchildren(seg_tag);
					tag=xml_findfirstchild("v",seg_tag);
					
					for (j=0;j!=seg->data.fc.ptsz;j++) {
						xml_get_attribute_3_coord_int(tag,"c3",&seg->data.fc.x[j],&seg->data.fc.y[j],&seg->data.fc.z[j]);
						tag=xml_findnextchild(tag);
					}
					
					seg_tag=xml_findnextchild(seg_tag);
				}
			}

				// liquids
				
            main_seg_tag=xml_findfirstchild("Liquids",portal_tag);
            if (main_seg_tag!=-1) {
                
                cnt=xml_countchildren(main_seg_tag);
				seg_tag=xml_findfirstchild("Liquid",main_seg_tag);
                
                for (k=0;k!=cnt;k++) {
					seg=&map->segments[map->nsegment];
					map->nsegment++;
					
					read_single_segment(seg_tag,seg,i,sg_liquid);
					
					tag=xml_findfirstchild("v",seg_tag);
					xml_get_attribute_3_coord_int(tag,"c3",&seg->data.liquid.lft,&seg->data.liquid.y,&seg->data.liquid.top);
					tag=xml_findnextchild(tag);
					xml_get_attribute_3_coord_int(tag,"c3",&seg->data.liquid.rgt,&seg->data.liquid.y,&seg->data.liquid.bot);
					
					seg_tag=xml_findnextchild(seg_tag);
				}
			}
			
				// ambient walls
				
            main_seg_tag=xml_findfirstchild("Ambients",portal_tag);
            if (main_seg_tag!=-1) {
                
                cnt=xml_countchildren(main_seg_tag);
				seg_tag=xml_findfirstchild("Ambient",main_seg_tag);
                
                for (k=0;k!=cnt;k++) {
					seg=&map->segments[map->nsegment];
					map->nsegment++;
					
					read_single_segment(seg_tag,seg,i,sg_ambient_wall);
					
					tag=xml_findfirstchild("v",seg_tag);
					xml_get_attribute_3_coord_int(tag,"c3",&seg->data.ambient_wall.lx,&seg->data.ambient_wall.ty,&seg->data.ambient_wall.lz);
					tag=xml_findnextchild(tag);
					xml_get_attribute_3_coord_int(tag,"c3",&seg->data.ambient_wall.rx,&seg->data.ambient_wall.by,&seg->data.ambient_wall.rz);
					
					seg_tag=xml_findnextchild(seg_tag);
				}
			}
			
				// ambient floor/ceilings

            main_seg_tag=xml_findfirstchild("Ambient_FCs",portal_tag);
            if (main_seg_tag!=-1) {
                
                cnt=xml_countchildren(main_seg_tag);
				seg_tag=xml_findfirstchild("Ambient_FC",main_seg_tag);
                
                for (k=0;k!=cnt;k++) {
					seg=&map->segments[map->nsegment];
					map->nsegment++;
					
					read_single_segment(seg_tag,seg,i,sg_ambient_fc);
					
					seg->data.ambient_fc.ptsz=xml_countchildren(seg_tag);
					tag=xml_findfirstchild("v",seg_tag);
					
					for (j=0;j!=seg->data.ambient_fc.ptsz;j++) {
						xml_get_attribute_3_coord_int(tag,"c3",&seg->data.ambient_fc.x[j],&seg->data.ambient_fc.y[j],&seg->data.ambient_fc.z[j]);
						tag=xml_findnextchild(tag);
					}
					
					seg_tag=xml_findnextchild(seg_tag);
				}
			}

				// scenery
		
			main_obj_tag=xml_findfirstchild("Sceneries",portal_tag);
			if (main_obj_tag!=-1) {
			
				cnt=xml_countchildren(main_obj_tag);
				obj_tag=xml_findfirstchild("Scenery",main_obj_tag);
				
				for (k=0;k!=cnt;k++) {
					if (map->nscenery>=max_map_scenery) break;
					
					scenery=&map->sceneries[map->nscenery];
					map->nscenery++;
					
					scenery->pos.rn=i;
					xml_get_attribute_3_coord_int(obj_tag,"c3",&scenery->pos.x,&scenery->pos.y,&scenery->pos.z);
					
					xml_get_attribute_text(obj_tag,"model_name",scenery->model_name,name_str_len);
					xml_get_attribute_text(obj_tag,"animation_name",scenery->animation_name,name_str_len);
					xml_get_attribute_3_coord_float(obj_tag,"angle",&scenery->ang.x,&scenery->ang.y,&scenery->ang.z);
					scenery->lighting_mode=xml_get_attribute_list(obj_tag,"lighting_mode",(char*)lighting_mode_str);
					scenery->contact=xml_get_attribute_boolean(obj_tag,"contact");
					scenery->contact_hit_box=xml_get_attribute_boolean(obj_tag,"contact_hit_box");
					scenery->face_forward=xml_get_attribute_boolean(obj_tag,"face_forward");
					scenery->shadow=xml_get_attribute_boolean(obj_tag,"shadow");
					scenery->shadow_cast_down=xml_get_attribute_boolean(obj_tag,"shadow_cast_down");
					
					scenery->override_size=xml_get_attribute_boolean(obj_tag,"override_size");
					xml_get_attribute_3_coord_int(obj_tag,"size",&scenery->size.x,&scenery->size.y,&scenery->size.z);
					
					obj_tag=xml_findnextchild(obj_tag);
				}
			}

				// lights
			
			main_light_tag=xml_findfirstchild("Lights",portal_tag);
			if (main_light_tag!=-1) {
			
				cnt=xml_countchildren(main_light_tag);
				light_tag=xml_findfirstchild("Light",main_light_tag);
				
				for (k=0;k!=cnt;k++) {
					if (map->nlight>=max_map_light) break;
					
					light=&map->lights[map->nlight];
					map->nlight++;
					
					light->pos.rn=i;
					light->type=xml_get_attribute_list(light_tag,"type",(char*)light_type_str);
					xml_get_attribute_3_coord_int(light_tag,"c3",&light->pos.x,&light->pos.y,&light->pos.z);
					light->intensity=xml_get_attribute_int(light_tag,"intensity");
					xml_get_attribute_color(light_tag,"rgb",&light->col);
					light->confine_to_portal=xml_get_attribute_boolean(light_tag,"confine");
					light->on=!xml_get_attribute_boolean(light_tag,"off");
				
					light_tag=xml_findnextchild(light_tag);
				}
			}
			
				// sounds
				
			main_sound_tag=xml_findfirstchild("Sounds",portal_tag);
			if (main_sound_tag!=-1) {
			
				cnt=xml_countchildren(main_sound_tag);
				sound_tag=xml_findfirstchild("Sound",main_sound_tag);
				
				for (k=0;k!=cnt;k++) {
					if (map->nsound>=max_map_sound) break;
					
					sound=&map->sounds[map->nsound];
					map->nsound++;
					
					sound->pos.rn=i;
					xml_get_attribute_text(sound_tag,"name",sound->name,file_str_len);
					xml_get_attribute_3_coord_int(sound_tag,"c3",&sound->pos.x,&sound->pos.y,&sound->pos.z);
					sound->pitch=xml_get_attribute_float(sound_tag,"pitch");
					sound->on=!xml_get_attribute_boolean(sound_tag,"off");
					
					sound_tag=xml_findnextchild(sound_tag);
				}
			}
			
				// particles
				
			main_particle_tag=xml_findfirstchild("Particles",portal_tag);
			if (main_particle_tag!=-1) {
			
				cnt=xml_countchildren(main_particle_tag);
				particle_tag=xml_findfirstchild("Particle",main_particle_tag);
				
				for (k=0;k!=cnt;k++) {
					if (map->nparticle>=max_map_particle) break;
					
					particle=&map->particles[map->nparticle];
					map->nparticle++;
					
					particle->pos.rn=i;
					xml_get_attribute_text(particle_tag,"name",particle->name,file_str_len);
					xml_get_attribute_3_coord_int(particle_tag,"c3",&particle->pos.x,&particle->pos.y,&particle->pos.z);
					particle->spawn_tick=xml_get_attribute_int(particle_tag,"spawn_tick");
					particle->slop_tick=xml_get_attribute_int(particle_tag,"slop_tick");
					particle->on=!xml_get_attribute_boolean(particle_tag,"off");
					
					particle_tag=xml_findnextchild(particle_tag);
				}
			}
        
				// nodes
		
			main_node_tag=xml_findfirstchild("Nodes",portal_tag);
			if (main_node_tag!=-1) {
			
				cnt=xml_countchildren(main_node_tag);
				node_tag=xml_findfirstchild("Node",main_node_tag);
				
				for (k=0;k!=cnt;k++) {
				
					idx=xml_get_attribute_int(node_tag,"id");
					node=&map->nodes[idx];
					if (idx>=map->nnode) map->nnode=idx+1;
					
					for (j=0;j!=max_node_link;j++) {
						node->link[j]=-1;
					}
					for (j=0;j!=max_node;j++) {
						node->path_hint[j]=-1;
					}
					
					node->pos.rn=i;
					xml_get_attribute_3_coord_int(node_tag,"c3",&node->pos.x,&node->pos.y,&node->pos.z);

					xml_get_attribute_3_coord_float(node_tag,"angle",&node->ang.x,&node->ang.y,&node->ang.z);

					xml_get_attribute_text(node_tag,"name",node->name,name_str_len);
					node->user_value=xml_get_attribute_int_default(node_tag,"user",0);
					
					tag=xml_findfirstchild("Link",node_tag);
					xml_get_attribute_short_array(tag,"node",(short*)node->link,max_node_link);
					
					tag=xml_findfirstchild("Hint",node_tag);
					xml_get_attribute_short_array(tag,"node",(short*)node->path_hint,map->nnode);
					
					node_tag=xml_findnextchild(node_tag);
				}
			}

				// spots
		
			main_obj_tag=xml_findfirstchild("Spots",portal_tag);
			if (main_obj_tag!=-1) {
			
				cnt=xml_countchildren(main_obj_tag);
				obj_tag=xml_findfirstchild("Spot",main_obj_tag);
				
				for (k=0;k!=cnt;k++) {
				
					idx=xml_get_attribute_int(obj_tag,"id");
					spot=&map->spots[idx];
					if (idx>=map->nspot) map->nspot=idx+1;
					
					spot->pos.rn=i;
					xml_get_attribute_3_coord_int(obj_tag,"c3",&spot->pos.x,&spot->pos.y,&spot->pos.z);
					
					xml_get_attribute_text(obj_tag,"name",spot->name,name_str_len);
					xml_get_attribute_text(obj_tag,"type",spot->type,name_str_len);
					if (!xml_get_attribute_text(obj_tag,"script",spot->script,name_str_len)) {		// supergumba -- remove later -- here to fix XML from older version
						strcpy(spot->script,spot->name);
					}
					xml_get_attribute_text(obj_tag,"display_model",spot->display_model,name_str_len);
					xml_get_attribute_text(obj_tag,"params",spot->params,param_str_len);
					spot->ang.y=xml_get_attribute_float(obj_tag,"angle");
					
					obj_tag=xml_findnextchild(obj_tag);
				}
			}
             
            portal++;
			portal_tag=xml_findnextchild(portal_tag);
        }
    }
	
	xml_close_file();

	// supergumba -- new mesh stuff

//	return(map_convert_segment_to_mesh(map));
    
	return(TRUE);
}




// supergumba -- segment to mesh converter

int map_convert_segment_to_mesh_add_point_vlist(int nvertex,d3pnt *vlist,int x,int y,int z)
{
	int				n;
	d3pnt			*vptr;

	vptr=vlist;

	for (n=0;n!=nvertex;n++) {
		if ((vptr->x==x) && (vptr->y==y) && (vptr->z==z)) return(nvertex);
		vptr++;
	}

	vptr->x=x;
	vptr->y=y;
	vptr->z=z;

	return(nvertex+1);
}

int map_convert_segment_to_mesh_add_poly_vlist(int nvertex,d3pnt *vlist,int ptsz,int *x,int *y,int *z)
{
	int				n;

	for (n=0;n!=ptsz;n++) {
		nvertex=map_convert_segment_to_mesh_add_point_vlist(nvertex,vlist,x[n],y[n],z[n]);
	}

	return(nvertex);
}

bool map_convert_segment_to_mesh(map_type *map)
{
	int				n,i,vlist_sz,nvertex,npoly;
	d3pnt			*vlist;
	segment_type	*seg;

			// memory for vertex lists
			// just use enough vertexes to cover most maps,
			// hopefully this'll work on all conversions

	vlist_sz=sizeof(d3pnt)*10000;

	vlist=valloc(vlist_sz);
	if (vlist==NULL) return(FALSE);

		// create a mesh out of all segments in a portal

	for (n=0;n!=map->nportal;n++) {

			// clear vertex list

		bzero(vlist,vlist_sz);
		
			// count the vertexes and polys

		nvertex=npoly=0;

		for (i=0;i!=map->nsegment;i++) {
			seg=&map->segments[i];
			if (seg->rn!=n) continue;

			switch (seg->type) {

				case sg_wall:
					nvertex=map_convert_segment_to_mesh_add_poly_vlist(nvertex,vlist,seg->data.wall.ptsz,seg->data.wall.x,seg->data.wall.y,seg->data.wall.z);
					npoly++;
					break;

				case sg_floor:
				case sg_ceiling:
					nvertex=map_convert_segment_to_mesh_add_poly_vlist(nvertex,vlist,seg->data.fc.ptsz,seg->data.fc.x,seg->data.fc.y,seg->data.fc.z);
					npoly++;
					break;

			}

		}

		if ((nvertex==0) || (npoly==0)) continue;

			// create new mesh

	//	if (!map_portal_mesh_add(map,n,1)) {
	//		free(vlist);
	//		return(FALSE);
	//	}

			// add vertexes and polys

		fprintf(stdout,"portal = %d; nvertex=%d, npoly=%d\n",n,nvertex,npoly);



		
	}
	
	free(vlist);

	return(TRUE);
}

