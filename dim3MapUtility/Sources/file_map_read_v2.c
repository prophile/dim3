/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map File Reading (v2 [mesh] maps)

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

/* =======================================================

      Read Vertexes, Meshes, Liquids
      
======================================================= */

bool read_single_mesh(map_type *map,int portal_idx,int mesh_idx,int mesh_tag)
{
	int					n,nvertex,npoly,
						main_vertex_tag,vertex_tag,main_poly_tag,poly_tag;
	d3pnt				*pt;
	map_mesh_type		*mesh;
	map_mesh_poly_type	*poly;

	mesh=&map->portals[portal_idx].mesh.meshes[mesh_idx];

		// mesh settings

	mesh->group_idx=xml_get_attribute_int_default(mesh_tag,"group",-1);

	mesh->flag.on=!xml_get_attribute_boolean(mesh_tag,"off");
	mesh->flag.pass_through=xml_get_attribute_boolean(mesh_tag,"pass");
	mesh->flag.moveable=xml_get_attribute_boolean(mesh_tag,"moveable");
	mesh->flag.climbable=xml_get_attribute_boolean(mesh_tag,"climbable");
	mesh->flag.hilite=xml_get_attribute_boolean(mesh_tag,"hilite");
	mesh->flag.lock_uv=xml_get_attribute_boolean(mesh_tag,"lock_uv");
	
	xml_get_attribute_3_coord_int(mesh_tag,"rot_off",&mesh->rot_off.x,&mesh->rot_off.y,&mesh->rot_off.z);

		// vertexes

    main_vertex_tag=xml_findfirstchild("Vertexes",mesh_tag);
    if (main_vertex_tag!=-1) {
    
        nvertex=xml_countchildren(main_vertex_tag);
		vertex_tag=xml_findfirstchild("v",main_vertex_tag);

		if (!map_portal_mesh_set_vertex_count(map,portal_idx,mesh_idx,nvertex)) return(FALSE);

		pt=mesh->vertexes;

		for (n=0;n!=nvertex;n++) {
			xml_get_attribute_3_coord_int(vertex_tag,"c3",&pt->x,&pt->y,&pt->z);
			pt++;
			vertex_tag=xml_findnextchild(vertex_tag);
		}
	}

		// polys

    main_poly_tag=xml_findfirstchild("Polys",mesh_tag);
    if (main_poly_tag!=-1) {
    
        npoly=xml_countchildren(main_poly_tag);
		poly_tag=xml_findfirstchild("p",main_poly_tag);

		if (!map_portal_mesh_set_poly_count(map,portal_idx,mesh_idx,npoly)) return(FALSE);

		poly=mesh->polys;

		for (n=0;n!=npoly;n++) {

			poly->txt_idx=xml_get_attribute_int(poly_tag,"txt");
			poly->ptsz=xml_get_attribute_int_array(poly_tag,"v",poly->v,8);
			xml_get_attribute_float_array(poly_tag,"x",poly->gx,8);
			xml_get_attribute_float_array(poly_tag,"y",poly->gy,8);

			xml_get_attribute_2_coord_float(poly_tag,"shift",&poly->x_shift,&poly->y_shift);
			poly->dark_factor=xml_get_attribute_float_default(poly_tag,"dark_fct",1.0f);
			poly->alpha=xml_get_attribute_float_default(poly_tag,"alpha",1.0f);

			poly++;
			poly_tag=xml_findnextchild(poly_tag);
		}
	}

	return(TRUE);
}

void read_single_liquid(map_type *map,int portal_idx,int liquid_idx,int liquid_tag)
{
	int					tag;
	map_liquid_type		*liq;

	liq=&map->portals[portal_idx].liquid.liquids[liquid_idx];

		// liquid settings

	liq->group_idx=xml_get_attribute_int_default(liquid_tag,"group",-1);

		// polygon

    tag=xml_findfirstchild("Poly",liquid_tag);
    if (tag!=-1) {
		liq->txt_idx=xml_get_attribute_int(tag,"txt");
		xml_get_attribute_3_coord_int(tag,"v1",&liq->lft,&liq->y,&liq->top);
		xml_get_attribute_3_coord_int(tag,"v2",&liq->rgt,&liq->y,&liq->bot);
		xml_get_attribute_2_coord_float(tag,"uv_off",&liq->x_txtoff,&liq->y_txtoff);
		xml_get_attribute_2_coord_float(tag,"uv_size",&liq->x_txtfact,&liq->y_txtfact);
		xml_get_attribute_color(tag,"rgb",&liq->col);
		liq->alpha=xml_get_attribute_float_default(tag,"alpha",1.0f);
		liq->tint_alpha=xml_get_attribute_float(tag,"tint_alpha");
		xml_get_attribute_2_coord_float(tag,"shift",&liq->x_shift,&liq->y_shift);
	}

		// physics

    tag=xml_findfirstchild("Physic",liquid_tag);
    if (tag!=-1) {
		liq->speed_alter=xml_get_attribute_float(tag,"speed_alter");
	}

		// harm

    tag=xml_findfirstchild("Harm",liquid_tag);
    if (tag!=-1) {
		liq->harm.in_harm=xml_get_attribute_int(tag,"harm");
		liq->harm.drown_harm=xml_get_attribute_int(tag,"drown_harm");
		liq->harm.drown_tick=xml_get_attribute_int(tag,"drown_tick");
	}

		// tides

    tag=xml_findfirstchild("Tide",liquid_tag);
    if (tag!=-1) {
		liq->tide.rate=xml_get_attribute_int(tag,"rate");
		liq->tide.high=xml_get_attribute_int(tag,"high");
		liq->tide.split=xml_get_attribute_int(tag,"split");
		liq->tide.direction=xml_get_attribute_list(tag,"tide_direction",(char*)liquid_tide_direction_str);
	}
}

/* =======================================================

      Read Map XML
      
======================================================= */

bool decode_map_v2_xml(map_type *map,int map_head)
{
	int						i,k,j,y,idx,nmesh,mesh_idx,nliquid,liquid_idx,
							main_portal_tag,portal_tag,msg_tag,main_path_tag,path_tag,
							main_mesh_tag,mesh_tag,main_liquid_tag,liquid_tag,
							main_light_tag,light_tag,main_sound_tag,sound_tag,
							main_particle_tag,particle_tag,main_node_tag,node_tag,
							main_obj_tag,obj_tag,tag,id,cnt;
    portal_type				*portal;
	portal_sight_list_type	*sight;
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

				// default some ptrs

			portal->mesh.nmesh=0;
			portal->mesh.meshes=NULL;

			portal->liquid.nliquid=0;
			portal->liquid.liquids=NULL;
        
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

				// mesh

			main_mesh_tag=xml_findfirstchild("Meshes",portal_tag);
			if (main_mesh_tag!=-1) {
    
				nmesh=xml_countchildren(main_mesh_tag);
				if (nmesh!=0) {
					
					mesh_tag=xml_findfirstchild("Mesh",main_mesh_tag);

					for (k=0;k!=nmesh;k++) {

							// create new mesh

						mesh_idx=map_portal_mesh_add(map,i);
						if (mesh_idx==-1) return(FALSE);

							// read mesh

						if (!read_single_mesh(map,i,mesh_idx,mesh_tag)) return(FALSE);
						
						mesh_tag=xml_findnextchild(mesh_tag);
					}
				}
			}

				// liquids

			main_liquid_tag=xml_findfirstchild("Liquids",portal_tag);
			if (main_liquid_tag!=-1) {
    
				nliquid=xml_countchildren(main_liquid_tag);
				if (nliquid!=0) {
					
					liquid_tag=xml_findfirstchild("Liquid",main_liquid_tag);

					for (k=0;k!=nliquid;k++) {

							// create new liquid

						liquid_idx=map_portal_liquid_add(map,i);
						if (liquid_idx==-1) return(FALSE);

							// read liquid

						read_single_liquid(map,i,liquid_idx,liquid_tag);
						
						liquid_tag=xml_findnextchild(liquid_tag);
					}
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
					scenery->contact_object_on=xml_get_attribute_boolean(obj_tag,"contact");
					scenery->contact_projectile_on=xml_get_attribute_boolean(obj_tag,"contact_projectile");
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
					xml_get_attribute_short_array(tag,"node",(short*)node->path_hint,max_node);
					
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
					
					spot->skill=xml_get_attribute_int(obj_tag,"skill");
					
					obj_tag=xml_findnextchild(obj_tag);
				}
			}
             
            portal++;
			portal_tag=xml_findnextchild(portal_tag);
        }
    }

	return(TRUE);
}
