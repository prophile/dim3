/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Map File Writting

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

extern char					media_type_str[][32],
							sky_type_str[][32],
							gl_fog_type_str[][32],
							segment_type_str[][32],
							segment_clip_str[][32],
							segment_curve_str[][32],
							segment_push_str[][32],
							liquid_tide_direction_str[][32],
							light_type_str[][32],
							lighting_mode_str[][32],
							map_bump_mode_str[][32];

extern maputility_settings_type		maputility_settings;

/* =======================================================

      Check for Items in Portals
      
======================================================= */

bool portal_has_map_light(map_type *map,int rn)
{
	int				n;
	map_light_type	*light;
	
	light=map->lights;
	
	for (n=0;n!=map->nlight;n++) {
		if (light->pos.rn==rn) return(TRUE);
		light++;
	}
	
	return(FALSE);
}

bool portal_has_map_sound(map_type *map,int rn)
{
	int				n;
	map_sound_type	*sound;
	
	sound=map->sounds;
	
	for (n=0;n!=map->nsound;n++) {
		if (sound->pos.rn==rn) return(TRUE);
		sound++;
	}
	
	return(FALSE);
}

bool portal_has_map_particle(map_type *map,int rn)
{
	int					n;
	map_particle_type	*particle;
	
	particle=map->particles;
	
	for (n=0;n!=map->nparticle;n++) {
		if (particle->pos.rn==rn) return(TRUE);
		particle++;
	}
	
	return(FALSE);
}

bool portal_has_node(map_type *map,int rn)
{
	int				n;
	node_type		*node;
	
	node=map->nodes;
	
	for (n=0;n!=map->nnode;n++) {
		if (node->pos.rn==rn) return(TRUE);
		node++;
	}
	
	return(FALSE);
}

bool portal_has_spot(map_type *map,int rn)
{
	int				n;
	spot_type		*spot;
	
	spot=map->spots;
	
	for (n=0;n!=map->nspot;n++) {
		if (spot->pos.rn==rn) return(TRUE);
		spot++;
	}
	
	return(FALSE);
}

bool portal_has_scenery(map_type *map,int rn)
{
	int					n;
	map_scenery_type	*scenery;
	
	scenery=map->sceneries;
	
	for (n=0;n!=map->nscenery;n++) {
		if (scenery->pos.rn==rn) return(TRUE);
		scenery++;
	}
	
	return(FALSE);
}

/* =======================================================

      Write Settings
      
======================================================= */

void write_map_settings_xml(map_type *map)
{
    xml_add_tagstart("Creator");
    xml_add_attribute_text("name","dim3 Map Editor");
    xml_add_attribute_text("version","2.0");
    xml_add_tagend(TRUE);
    
    xml_add_tagstart("Info");
    xml_add_attribute_text("name",map->info.title);
    xml_add_attribute_text("author",map->info.author);
    xml_add_tagend(TRUE);
    
    xml_add_tagstart("Settings");
    xml_add_attribute_float("gravity",map->settings.gravity);
	xml_add_attribute_float("gravity_max_power",map->settings.gravity_max_power);
	xml_add_attribute_float("gravity_max_speed",map->settings.gravity_max_speed);
    xml_add_attribute_float("resistance",map->settings.resistance);
	xml_add_attribute_float("txt_scale_x",map->settings.txt_scale_x);
	xml_add_attribute_float("txt_scale_y",map->settings.txt_scale_y);
	xml_add_attribute_boolean("speculars_hilite",map->settings.speculars_hilite);
	xml_add_attribute_boolean("editor_link_always_start",map->settings.editor_link_always_start);
    xml_add_tagend(TRUE);
	
    xml_add_tagstart("Optimizations");
	xml_add_attribute_int("lod_light_distance",map->optimizations.lod_light_distance);
	xml_add_attribute_int("lod_bump_distance",map->optimizations.lod_bump_distance);
	xml_add_attribute_int("lod_specular_distance",map->optimizations.lod_specular_distance);
	xml_add_attribute_int("lod_glow_distance",map->optimizations.lod_glow_distance);
	xml_add_attribute_int("lod_model_distance",map->optimizations.lod_model_distance);
	xml_add_attribute_int("lod_shadow_distance",map->optimizations.lod_shadow_distance);
	xml_add_attribute_int("lod_effect_distance",map->optimizations.lod_effect_distance);
    xml_add_tagend(TRUE);
	
    xml_add_tagstart("Media");
	xml_add_attribute_list("type",(char*)media_type_str,map->media.type);
	xml_add_attribute_text("name",map->media.name);
	xml_add_attribute_text("title_sound_name",map->media.title_sound_name);
	xml_add_tagend(TRUE);
	
    xml_add_tagstart("Music");
	xml_add_attribute_int("fade_msec",map->music.fade_msec);
	xml_add_attribute_text("name",map->music.name);
	xml_add_tagend(TRUE);
	
    xml_add_tagstart("Ambient_Light");
	xml_add_attribute_color("rgb",&map->ambient.light_color);
	xml_add_attribute_float("drop_off_factor",map->ambient.light_drop_off_factor);
    xml_add_tagend(TRUE);
	
    xml_add_tagstart("Ambient_Sound");
	xml_add_attribute_text("name",map->ambient.sound_name);
	xml_add_attribute_float("pitch",map->ambient.sound_pitch);
    xml_add_tagend(TRUE);
	
    xml_add_tagstart("Background");
	xml_add_attribute_boolean("on",map->background.on);
    xml_add_attribute_int("fill",map->background.fill);
    xml_add_attribute_2_coord_float("scroll",map->background.x_scroll_fact,map->background.y_scroll_fact);
    xml_add_tagend(TRUE);
    
    xml_add_tagstart("Sky");
    xml_add_tagend(FALSE);
    xml_add_tagstart("Settings");
	xml_add_attribute_boolean("on",map->sky.on);
	xml_add_attribute_list("type",(char*)sky_type_str,map->sky.type);
    xml_add_attribute_int("radius",map->sky.radius);
    xml_add_attribute_int("extra_height",map->sky.extra_height);
    xml_add_attribute_int("dome_y",map->sky.dome_y);
    xml_add_tagend(TRUE);
    xml_add_tagstart("Fill");
    xml_add_attribute_int("index",map->sky.fill);
    xml_add_attribute_int("bottom_index",map->sky.bottom_fill);
    xml_add_attribute_int("north_index",map->sky.north_fill);
    xml_add_attribute_int("south_index",map->sky.south_fill);
    xml_add_attribute_int("east_index",map->sky.east_fill);
    xml_add_attribute_int("west_index",map->sky.west_fill);
    xml_add_attribute_float("factor",map->sky.txt_fact);
    xml_add_attribute_2_coord_float("shift",map->sky.txt_x_shift,map->sky.txt_y_shift);
    xml_add_tagend(TRUE);
    xml_add_tagclose("Sky");

    xml_add_tagstart("Fog");
	xml_add_attribute_boolean("on",map->fog.on);
	xml_add_attribute_int("count",map->fog.count);
	xml_add_attribute_int("inner_radius",map->fog.inner_radius);
	xml_add_attribute_int("outer_radius",map->fog.outer_radius);
	xml_add_attribute_int("high",map->fog.high);
	xml_add_attribute_int("drop",map->fog.drop);
	xml_add_attribute_int("texture_index",map->fog.texture_idx);
    xml_add_attribute_float("speed",map->fog.speed);
    xml_add_attribute_float("txt_x_fact",map->fog.txt_x_fact);
    xml_add_attribute_float("txt_y_fact",map->fog.txt_y_fact);
	xml_add_attribute_color("rgb",&map->fog.col);
    xml_add_attribute_float("alpha",map->fog.alpha);
	xml_add_attribute_boolean("use_solid_color",map->fog.use_solid_color);
    xml_add_tagend(TRUE);
	
    xml_add_tagstart("Rain");
	xml_add_attribute_boolean("on",map->rain.on);
	xml_add_attribute_int("density",map->rain.density);
	xml_add_attribute_int("radius",map->rain.radius);
	xml_add_attribute_int("height",map->rain.height);
	xml_add_attribute_int("speed",map->rain.speed);
	xml_add_attribute_int("line_width",map->rain.line_width);
	xml_add_attribute_int("line_length",map->rain.line_length);
	xml_add_attribute_int("slant_add",map->rain.slant_add);
	xml_add_attribute_int("slant_time_msec",map->rain.slant_time_msec);
	xml_add_attribute_int("slant_change_msec",map->rain.slant_change_msec);
    xml_add_attribute_float("alpha",map->rain.alpha);
	xml_add_attribute_color("start_rgb",&map->rain.start_color);
	xml_add_attribute_color("end_rgb",&map->rain.end_color);
    xml_add_tagend(TRUE);
}

void write_map_groups_xml(map_type *map)
{
	int						n;
	group_type				*group;

	xml_add_tagstart("Groups");
    xml_add_tagend(FALSE);
    
    group=map->groups;
    
    for (n=0;n!=map->ngroup;n++) {
		xml_add_tagstart("Group");
		xml_add_attribute_text("name",group->name);
		xml_add_tagend(TRUE);
        
        group++;
    }
    
    xml_add_tagclose("Groups");
}

void write_map_textures_xml(map_type *map)
{
	int						n,frame_count;
    texture_type			*texture;

	xml_add_tagstart("Fills");
    xml_add_tagend(FALSE);
    
    texture=map->textures;
    
    for (n=0;n!=max_map_texture;n++) {
    
		frame_count=map_count_texture_frames(map,n);
		if (frame_count!=0) {
			xml_add_tagstart("Fill");
            xml_add_attribute_int("id",n);
			bitmap_texture_write_xml(texture,frame_count,TRUE);
            xml_add_tagclose("Fill");
         }
        
        texture++;
    }
    
    xml_add_tagclose("Fills");
}

void write_map_movements_xml(map_type *map)
{
	int						n,k;
	movement_type			*movement;
	movement_move_type		*move;

	xml_add_tagstart("Movements");
    xml_add_tagend(FALSE);
    
    movement=map->movements;
    
    for (n=0;n!=map->nmovement;n++) {
    
		xml_add_tagstart("Movement");
		xml_add_attribute_text("name",movement->name);
		xml_add_attribute_int("group",movement->group_idx);
		xml_add_attribute_int("group_reverse",movement->reverse_group_idx);
		xml_add_attribute_boolean("auto_start",movement->auto_start);
		xml_add_attribute_boolean("auto_open",movement->auto_open);
		xml_add_attribute_boolean("auto_open_never_close",movement->auto_open_never_close);
		xml_add_attribute_int("auto_open_distance",movement->auto_open_distance);
		xml_add_attribute_boolean("loop",movement->loop);
		xml_add_tagend(FALSE);
		
		xml_add_tagstart("Moves");
		xml_add_tagend(FALSE);

		move=movement->moves;
    
		for (k=0;k!=movement->nmove;k++) {
			xml_add_tagstart("Move");
			xml_add_attribute_3_coord_int("c3",move->mov.x,move->mov.y,move->mov.z);
			xml_add_attribute_3_coord_float("r3",move->rot.x,move->rot.y,move->rot.z);
			xml_add_attribute_int("msec",move->msec);
			xml_add_attribute_int("user_id",move->user_id);
			xml_add_attribute_text("sound_name",move->sound_name);
			xml_add_attribute_float("sound_pitch",move->sound_pitch);
			xml_add_tagend(TRUE);
			
			move++;
		}

		xml_add_tagclose("Moves");
		xml_add_tagclose("Movement");
        
        movement++;
    }
    
    xml_add_tagclose("Movements");
}

/* =======================================================

      Write Vertexes, Meshes, Liquids
      
======================================================= */

void write_single_vertex(int x,int y,int z)
{
    xml_add_tagstart("v");
	xml_add_attribute_3_coord_int("c3",x,y,z);
    xml_add_tagend(TRUE);
}

void write_single_mesh(map_mesh_type *mesh)
{
	int					n,nvertex,npoly;
	d3pnt				*pt;
	map_mesh_poly_type	*poly;

		// mesh settings

    xml_add_tagstart("Mesh");

  	if (mesh->group_idx!=-1) xml_add_attribute_int("group",mesh->group_idx);

	xml_add_attribute_boolean("off",!mesh->flag.on);
	xml_add_attribute_boolean("pass",mesh->flag.pass_through);
	xml_add_attribute_boolean("moveable",mesh->flag.moveable);
	xml_add_attribute_boolean("climbable",mesh->flag.climbable);
	xml_add_attribute_boolean("hilite",mesh->flag.hilite);
	xml_add_attribute_boolean("lock_uv",mesh->flag.lock_uv);
	
	xml_add_attribute_3_coord_int("rot_off",mesh->rot_off.x,mesh->rot_off.y,mesh->rot_off.z);

	xml_add_tagend(FALSE);

		// vertexes

    xml_add_tagstart("Vertexes");
	xml_add_tagend(FALSE);

	nvertex=mesh->nvertex;
	pt=mesh->vertexes;

	for (n=0;n!=nvertex;n++) {
		write_single_vertex(pt->x,pt->y,pt->z);
		pt++;
	}
	
	xml_add_tagclose("Vertexes");

		// polys

    xml_add_tagstart("Polys");
	xml_add_tagend(FALSE);

	npoly=mesh->npoly;
	poly=mesh->polys;

	for (n=0;n!=npoly;n++) {

			// single polygon

		xml_add_tagstart("p");
		
		xml_add_attribute_int("txt",poly->txt_idx);
		
		xml_add_attribute_int_array("v",poly->v,poly->ptsz,FALSE);
		xml_add_attribute_float_array("x",poly->gx,poly->ptsz);
		xml_add_attribute_float_array("y",poly->gy,poly->ptsz);
		
		if ((poly->x_shift!=0) || (poly->y_shift!=0)) xml_add_attribute_2_coord_float("shift",poly->x_shift,poly->y_shift);
		if (poly->dark_factor!=1.0f) xml_add_attribute_float("dark_fct",poly->dark_factor);
		if (poly->alpha!=1.0f) xml_add_attribute_float("alpha",poly->alpha);

		xml_add_tagend(TRUE);

		poly++;
	}
	
	xml_add_tagclose("Polys");

	xml_add_tagclose("Mesh");
}

void write_single_liquid(map_liquid_type *liq)
{
		// liquid settings

    xml_add_tagstart("Liquid");
  	if (liq->group_idx!=-1) xml_add_attribute_int("group",liq->group_idx);
	xml_add_tagend(FALSE);

		// polygon

	xml_add_tagstart("Poly");
	xml_add_attribute_int("txt",liq->txt_idx);
	xml_add_attribute_3_coord_int("v1",liq->lft,liq->y,liq->top);
	xml_add_attribute_3_coord_int("v2",liq->rgt,liq->y,liq->bot);
	xml_add_attribute_2_coord_float("uv_off",liq->x_txtoff,liq->y_txtoff);
	xml_add_attribute_2_coord_float("uv_size",liq->x_txtfact,liq->y_txtfact);
	xml_add_attribute_color("rgb",&liq->col);
	xml_add_attribute_float("alpha",liq->alpha);
	xml_add_attribute_float("tint_alpha",liq->tint_alpha);
	if ((liq->x_shift!=0) || (liq->y_shift!=0)) xml_add_attribute_2_coord_float("shift",liq->x_shift,liq->y_shift);
	xml_add_tagend(TRUE);

		// physics

	xml_add_tagstart("Physic");
	xml_add_attribute_float("speed_alter",liq->speed_alter);
	xml_add_tagend(TRUE);

		// harm

	xml_add_tagstart("Harm");
	xml_add_attribute_int("harm",liq->harm.in_harm);
	xml_add_attribute_int("drown_harm",liq->harm.drown_harm);
	xml_add_attribute_int("drown_tick",liq->harm.drown_tick);
	xml_add_tagend(TRUE);

		// tides

	xml_add_tagstart("Tide");
	xml_add_attribute_int("rate",liq->tide.rate);
	xml_add_attribute_int("high",liq->tide.high);
	xml_add_attribute_int("split",liq->tide.split);
	xml_add_attribute_list("tide_direction",(char*)liquid_tide_direction_str,liq->tide.direction);
	xml_add_tagend(TRUE);

	xml_add_tagclose("Liquid");
}

/* =======================================================

      Write Map XML
      
======================================================= */

bool write_map_xml(map_type *map)
{
    int						n,k,nmesh,nliq;
	bool					ok;
    portal_type				*portal;
    portal_sight_list_type	*sight;
    map_mesh_type			*mesh;
	map_liquid_type			*liq;
    map_light_type			*light;
    map_sound_type			*sound;
	map_particle_type		*particle;
    node_type				*node;
    spot_type				*spot;
	map_scenery_type		*scenery;

    xml_new_file();
    
    xml_add_tagstart("Map");
    xml_add_tagend(FALSE);
    
        // map settings

	write_map_settings_xml(map);
	write_map_groups_xml(map);
	write_map_textures_xml(map);
	write_map_movements_xml(map);
   
        // portals

    xml_add_tagstart("Portals");
    xml_add_tagend(FALSE);
    
    portal=map->portals;
    
    for (n=0;n!=map->nportal;n++) {
        xml_add_tagstart("Portal");
        
        xml_add_attribute_text("name",portal->name);
        
        xml_add_attribute_3_coord_int("tl_c3",portal->x,0,portal->z);
        xml_add_attribute_3_coord_int("br_c3",portal->ex,0,portal->ez);
   
        xml_add_tagend(FALSE);
       
        if ((portal->msg.entry_on) || (portal->msg.exit_on) || (portal->msg.base_on) || (portal->msg.map_change_on)) {
            xml_add_tagstart("Messages");
            xml_add_tagend(FALSE);
            
            xml_add_tagstart("Entry");
            xml_add_attribute_boolean("on",portal->msg.entry_on);
            xml_add_attribute_int("id",portal->msg.entry_id);
            xml_add_tagend(TRUE);
            
            xml_add_tagstart("Exit");
            xml_add_attribute_boolean("on",portal->msg.exit_on);
            xml_add_attribute_int("id",portal->msg.exit_id);
            xml_add_tagend(TRUE);
			
            xml_add_tagstart("Base");
            xml_add_attribute_boolean("on",portal->msg.base_on);
            xml_add_attribute_int("team",portal->msg.base_team);
            xml_add_tagend(TRUE);
			
            xml_add_tagstart("Map");
            xml_add_attribute_boolean("on",portal->msg.map_change_on);
			xml_add_attribute_text("name",portal->msg.map_name);
			xml_add_attribute_text("spot_name",portal->msg.map_spot_name);
			xml_add_attribute_text("spot_type",portal->msg.map_spot_type);
			xml_add_tagend(TRUE);
  
            xml_add_tagclose("Messages");
        }
        
            // paths

        xml_add_tagstart("Paths");
        xml_add_tagend(FALSE);
        
        sight=portal->sight;
        
        for (k=0;k!=max_sight_list;k++) {
        
            if (sight->rn!=-1) {
            
                xml_add_tagstart("Path");
                
                xml_add_attribute_int("id",k);
                xml_add_attribute_int("portal",sight->rn);
                xml_add_attribute_short_array("link",(short*)sight->link,max_sight_link,TRUE);
                xml_add_attribute_boolean("root",sight->root);
                xml_add_tagend(TRUE);
             }
            
            sight++;
        }
        
        xml_add_tagclose("Paths");

			// meshes

		xml_add_tagstart("Meshes");
		xml_add_tagend(FALSE);

		nmesh=portal->mesh.nmesh;
		mesh=portal->mesh.meshes;

		for (k=0;k!=nmesh;k++) {
			write_single_mesh(mesh);
			mesh++;
		}

        xml_add_tagclose("Meshes");

			// liquids

		xml_add_tagstart("Liquids");
		xml_add_tagend(FALSE);

		nliq=portal->liquid.nliquid;
		liq=portal->liquid.liquids;

		for (k=0;k!=nliq;k++) {
			write_single_liquid(liq);
			liq++;
		}

        xml_add_tagclose("Liquids");
		
			// scenery

		if (portal_has_scenery(map,n)) {
			xml_add_tagstart("Sceneries");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nscenery;k++) {
				scenery=&map->sceneries[k];
				if (scenery->pos.rn!=n) continue;
				
				xml_add_tagstart("Scenery");
				xml_add_attribute_3_coord_int("c3",scenery->pos.x,scenery->pos.y,scenery->pos.z);
				xml_add_attribute_text("model_name",scenery->model_name);
				xml_add_attribute_text("animation_name",scenery->animation_name);
				xml_add_attribute_3_coord_float("angle",scenery->ang.x,scenery->ang.y,scenery->ang.z);
				xml_add_attribute_list("lighting_mode",(char*)lighting_mode_str,scenery->lighting_mode);
				xml_add_attribute_boolean("contact",scenery->contact);
				xml_add_attribute_boolean("contact_hit_box",scenery->contact_hit_box);
				xml_add_attribute_boolean("face_forward",scenery->face_forward);
				xml_add_attribute_boolean("shadow",scenery->shadow);
				xml_add_attribute_boolean("shadow_cast_down",scenery->shadow_cast_down);
				xml_add_attribute_boolean("override_size",scenery->override_size);
				xml_add_attribute_3_coord_int("size",scenery->size.x,scenery->size.y,scenery->size.z);
				xml_add_tagend(TRUE);
			}
			
			xml_add_tagclose("Sceneries");
		}
		
			// lights
        
		if (portal_has_map_light(map,n)) {
			xml_add_tagstart("Lights");
			xml_add_tagend(FALSE);
		
			for (k=0;k!=map->nlight;k++) {
				light=&map->lights[k];
				if (light->pos.rn!=n) continue;
		
				xml_add_tagstart("Light");
				xml_add_attribute_list("type",(char*)light_type_str,light->type);
				xml_add_attribute_3_coord_int("c3",light->pos.x,light->pos.y,light->pos.z);
				xml_add_attribute_int("intensity",light->intensity);
				xml_add_attribute_color("rgb",&light->col);
				xml_add_attribute_boolean("confine",light->confine_to_portal);
				xml_add_attribute_boolean("off",!light->on);
				xml_add_tagend(TRUE);
			}
			
			xml_add_tagclose("Lights");
		}
    
			// sounds
			
		if (portal_has_map_sound(map,n)) {
			xml_add_tagstart("Sounds");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nsound;k++) {
				sound=&map->sounds[k];
				if (sound->pos.rn!=n) continue;
			
				xml_add_tagstart("Sound");
				xml_add_attribute_text("name",sound->name);
				xml_add_attribute_3_coord_int("c3",sound->pos.x,sound->pos.y,sound->pos.z);
				xml_add_attribute_float("pitch",sound->pitch);
				xml_add_attribute_boolean("off",!sound->on);
				xml_add_tagend(TRUE);
			}
			
			xml_add_tagclose("Sounds");
		}
		
			// particles
			
		if (portal_has_map_particle(map,n)) {
			xml_add_tagstart("Particles");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nparticle;k++) {
				particle=&map->particles[k];
				if (particle->pos.rn!=n) continue;
			
				xml_add_tagstart("Particle");
				xml_add_attribute_text("name",particle->name);
				xml_add_attribute_3_coord_int("c3",particle->pos.x,particle->pos.y,particle->pos.z);
				xml_add_attribute_int("spawn_tick",particle->spawn_tick);
				xml_add_attribute_int("slop_tick",particle->slop_tick);
				xml_add_attribute_boolean("off",!particle->on);
				xml_add_tagend(TRUE);
			}
			
			xml_add_tagclose("Particles");
		}
		
			// nodes
        
		if (portal_has_node(map,n)) {
			xml_add_tagstart("Nodes");
			xml_add_tagend(FALSE);
		
			for (k=0;k!=map->nnode;k++) {
				node=&map->nodes[k];
				if (node->pos.rn!=n) continue;
			
				xml_add_tagstart("Node");
				xml_add_attribute_int("id",k);
				xml_add_attribute_3_coord_int("c3",node->pos.x,node->pos.y,node->pos.z);
				xml_add_attribute_3_coord_float("angle",node->ang.x,node->ang.y,node->ang.z);
				xml_add_attribute_text("name",node->name);
				if (node->user_value!=0) xml_add_attribute_int("user",node->user_value);
				xml_add_tagend(FALSE);
			
				xml_add_tagstart("Link");
				xml_add_attribute_short_array("node",(short*)node->link,max_node_link,TRUE);
				xml_add_tagend(TRUE);
			
				xml_add_tagstart("Hint");
				xml_add_attribute_short_array("node",(short*)node->path_hint,map->nnode,FALSE);
				xml_add_tagend(TRUE);
			
				xml_add_tagclose("Node");
			}
		
			xml_add_tagclose("Nodes");
		}
			
			// spots

		if (portal_has_spot(map,n)) {
			xml_add_tagstart("Spots");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nspot;k++) {
				spot=&map->spots[k];
				if (spot->pos.rn!=n) continue;
				
				xml_add_tagstart("Spot");
				xml_add_attribute_int("id",k);
				xml_add_attribute_3_coord_int("c3",spot->pos.x,spot->pos.y,spot->pos.z);
				xml_add_attribute_text("name",spot->name);
				xml_add_attribute_text("type",spot->type);
				xml_add_attribute_text("script",spot->script);
				xml_add_attribute_text("display_model",spot->display_model);
				xml_add_attribute_text("params",spot->params);
				xml_add_attribute_float("angle",spot->ang.y);
				xml_add_attribute_int("skill",spot->skill);
				xml_add_tagend(TRUE);
			}
			
			xml_add_tagclose("Spots");
		}
  
            // end portal
            
        xml_add_tagclose("Portal");
        
        portal++;
    }
            
    xml_add_tagclose("Portals");
	
    xml_add_tagclose("Map");

        // save the map
		
	ok=xml_save_file(map->info.load_path);
    xml_close_file();
	
	return(ok);
}

