/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map File Reading (v1 [segment] maps)

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3maputility.h"
#endif

extern maputility_settings_type		maputility_settings;

extern char				media_type_str[][32],
						sky_type_str[][32],
						gl_fog_type_str[][32],
						liquid_tide_direction_str[][32],
                        light_type_str[][32],
						lighting_mode_str[][32],
						map_bump_mode_str[][32];


char					segment_type_str[][32]={"wall","floor","ceiling","liquid","ambient",""},
                        segment_clip_str[][32]={"none","top","bottom","slant","top_curve","top_arch","bottom_curve","bottom_arch","top_curve_arch","bottom_curve_arch",""},
                        segment_curve_str[][32]={"none","forward","backward",""},
                        segment_push_str[][32]={"none","top","bottom","left","right","up","down",""};

extern bool map_convert_v1(map_type *map);

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
	seg->climbable=xml_get_attribute_boolean(tag,"climbable");
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

bool decode_map_v1_xml(map_type *map,int map_head)
{
	int						i,k,j,y,idx,
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
	        
        // portals

    main_portal_tag=xml_findfirstchild("Portals",map_head);
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

	return(map_convert_v1(map));
}
