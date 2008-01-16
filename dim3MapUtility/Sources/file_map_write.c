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

bool portal_has_segment_type(map_type *map,int rn,int seg_type)
{
	int				n;
	segment_type	*seg;
	
	seg=map->segments;
	
	for (n=0;n!=map->nsegment;n++) {
		if ((seg->rn==rn) && (seg->type==seg_type)) return(TRUE);
		seg++;
	}
	
	return(FALSE);
}

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

      Write Vertexes and Segments
      
======================================================= */

void write_single_vertex(int x,int y,int z)
{
    xml_add_tagstart("v");
	xml_add_attribute_3_coord_int("c3",x,y,z);
    xml_add_tagend(TRUE);
}

void write_single_segment(segment_type *seg)
{
	if (seg->group_idx!=-1) xml_add_attribute_int("group",seg->group_idx);
	
	if (seg->primitive_uid[0]!=-1) xml_add_attribute_short_array("primitive",seg->primitive_uid,max_primitive_stack,TRUE);
	
	if (seg->clip!=wc_none) xml_add_attribute_list("clip",(char*)segment_clip_str,seg->clip);
	if (seg->curve!=cv_none) xml_add_attribute_list("curve",(char*)segment_curve_str,seg->curve);
	xml_add_attribute_boolean("pass",seg->pass_through);
	xml_add_attribute_boolean("moveable",seg->moveable);
	xml_add_attribute_boolean("shiftable",seg->shiftable);
	xml_add_attribute_boolean("climbable",seg->climbable);
	xml_add_attribute_boolean("off",!seg->on);

	xml_add_attribute_int("fill",seg->fill);
	if (seg->txt_ang!=0) xml_add_attribute_int("ang",(seg->txt_ang*90));
	xml_add_attribute_2_coord_float("uv_off",seg->x_txtoff,seg->y_txtoff);
	xml_add_attribute_2_coord_float("uv_fct",seg->x_txtfact,seg->y_txtfact);
	if ((seg->x_shift!=0) || (seg->y_shift!=0)) xml_add_attribute_2_coord_float("shift",seg->x_shift,seg->y_shift);
	if (seg->dark_factor!=1.0f) xml_add_attribute_float("dark_fct",seg->dark_factor);
	if (seg->alpha!=1.0f) xml_add_attribute_float("alpha",seg->alpha);
	
	if (seg->tag!=0) xml_add_attribute_int("tag",seg->tag);

	if (seg->type==sg_liquid) {
		xml_add_attribute_int("harm",seg->data.liquid.harm);
		xml_add_attribute_int("drown_harm",seg->data.liquid.drown_harm);
		xml_add_attribute_int("drown_tick",seg->data.liquid.drown_tick);
		xml_add_attribute_float("speed_alter",seg->data.liquid.speed_alter);
		xml_add_attribute_int("wave_size",seg->data.liquid.wavesize);
		xml_add_attribute_int("tide_rate",seg->data.liquid.tiderate);
		xml_add_attribute_int("tide_size",seg->data.liquid.tidesize);
		xml_add_attribute_list("tide_direction",(char*)liquid_tide_direction_str,seg->data.liquid.tidedirection);
		xml_add_attribute_color("rgb",&seg->data.liquid.col);
		xml_add_attribute_float("tint_alpha",seg->data.liquid.tint_alpha);
	}
	
	if (seg->type==sg_ambient_wall) {
		xml_add_attribute_list("push",(char*)segment_push_str,seg->data.ambient_wall.push);
	}
	
	if (seg->type==sg_ambient_fc) {
		xml_add_attribute_list("push",(char*)segment_push_str,seg->data.ambient_fc.push);
	}
	
	xml_add_attribute_boolean("lock",seg->lock);
}

/* =======================================================

      Write Map XML
      
======================================================= */

bool write_map_xml(map_type *map)
{
    int						i,k,j,frame_count;
	bool					ok;
	group_type				*group;
    portal_type				*portal;
    portal_sight_list_type	*sight;
    segment_type			*seg;
    texture_type			*texture;
	movement_type			*movement;
	movement_move_type		*move;
    map_light_type			*light;
    map_sound_type			*sound;
	map_particle_type		*particle;
    node_type				*node;
    spot_type				*spot;
	map_scenery_type		*scenery;

	return(TRUE);		// supergumba -- save in new format
	
    xml_new_file();
    
    xml_add_tagstart("Map");
    xml_add_tagend(FALSE);
    
        // map info
    
    xml_add_tagstart("Creator");
    xml_add_attribute_text("name","dim3 Map Editor");
    xml_add_attribute_text("version","1.0");
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
	
         // groups

	xml_add_tagstart("Groups");
    xml_add_tagend(FALSE);
    
    group=map->groups;
    
    for (i=0;i!=map->ngroup;i++) {
		xml_add_tagstart("Group");
		xml_add_attribute_text("name",group->name);
		xml_add_tagend(TRUE);
        
        group++;
    }
    
    xml_add_tagclose("Groups");
  
        // textures

	xml_add_tagstart("Fills");
    xml_add_tagend(FALSE);
    
    texture=map->textures;
    
    for (i=0;i!=max_map_texture;i++) {
    
		frame_count=map_count_texture_frames(map,i);
		if (frame_count!=0) {
			xml_add_tagstart("Fill");
            xml_add_attribute_int("id",i);
			bitmap_texture_write_xml(texture,frame_count,TRUE);
            xml_add_tagclose("Fill");
         }
        
        texture++;
    }
    
    xml_add_tagclose("Fills");
	
         // movements

	xml_add_tagstart("Movements");
    xml_add_tagend(FALSE);
    
    movement=map->movements;
    
    for (i=0;i!=map->nmovement;i++) {
    
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
   
        // portals

    xml_add_tagstart("Portals");
    xml_add_tagend(FALSE);
    
    portal=map->portals;
    
    for (i=0;i!=map->nportal;i++) {
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
		
			// wall segments
		
		if (portal_has_segment_type(map,i,sg_wall)) {
			xml_add_tagstart("Walls");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nsegment;k++) {
				seg=&map->segments[k];
				if (seg->rn!=i) continue;
				if (seg->type!=sg_wall) continue;
		
				xml_add_tagstart("Wall");
				write_single_segment(seg);
				xml_add_tagend(FALSE);
			
				write_single_vertex(seg->data.wall.lx,seg->data.wall.ty,seg->data.wall.lz);
				write_single_vertex(seg->data.wall.rx,seg->data.wall.by,seg->data.wall.rz);
			
				xml_add_tagclose("Wall");
			}
			
			xml_add_tagclose("Walls");
		}
		
 			// floor segments
			
		if (portal_has_segment_type(map,i,sg_floor)) {
			xml_add_tagstart("Floors");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nsegment;k++) {
				seg=&map->segments[k];
				if (seg->rn!=i) continue;
				if (seg->type!=sg_floor) continue;
		
				xml_add_tagstart("Floor");
				write_single_segment(seg);
				xml_add_tagend(FALSE);
			
				for (j=0;j!=seg->data.fc.ptsz;j++) {
					write_single_vertex(seg->data.fc.x[j],seg->data.fc.y[j],seg->data.fc.z[j]);
				}
			
				xml_add_tagclose("Floor");
			}
			
			xml_add_tagclose("Floors");
		}
		
 			// ceiling segments
			
		if (portal_has_segment_type(map,i,sg_ceiling)) {
			xml_add_tagstart("Ceilings");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nsegment;k++) {
				seg=&map->segments[k];
				if (seg->rn!=i) continue;
				if (seg->type!=sg_ceiling) continue;
		
				xml_add_tagstart("Ceiling");
				write_single_segment(seg);
				xml_add_tagend(FALSE);
			
				for (j=0;j!=seg->data.fc.ptsz;j++) {
					write_single_vertex(seg->data.fc.x[j],seg->data.fc.y[j],seg->data.fc.z[j]);
				}
			
				xml_add_tagclose("Ceiling");
			}
			
			xml_add_tagclose("Ceilings");
		}
		
 			// liquid segments
			
		if (portal_has_segment_type(map,i,sg_liquid)) {
			xml_add_tagstart("Liquids");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nsegment;k++) {
				seg=&map->segments[k];
				if (seg->rn!=i) continue;
				if (seg->type!=sg_liquid) continue;
		
				xml_add_tagstart("Liquid");
				write_single_segment(seg);
				xml_add_tagend(FALSE);
			
				write_single_vertex(seg->data.liquid.lft,seg->data.liquid.y,seg->data.liquid.top);
				write_single_vertex(seg->data.liquid.rgt,seg->data.liquid.y,seg->data.liquid.bot);
			
				xml_add_tagclose("Liquid");
			}
			
			xml_add_tagclose("Liquids");
		}
		
 			// ambient wall segments
			
		if (portal_has_segment_type(map,i,sg_ambient_wall)) {
			xml_add_tagstart("Ambients");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nsegment;k++) {
				seg=&map->segments[k];
				if (seg->rn!=i) continue;
				if (seg->type!=sg_ambient_wall) continue;
		
				xml_add_tagstart("Ambient");
				write_single_segment(seg);
				xml_add_tagend(FALSE);
			
				write_single_vertex(seg->data.ambient_wall.lx,seg->data.ambient_wall.ty,seg->data.ambient_wall.lz);
				write_single_vertex(seg->data.ambient_wall.rx,seg->data.ambient_wall.by,seg->data.ambient_wall.rz);
			
				xml_add_tagclose("Ambient");
			}
			
			xml_add_tagclose("Ambients");
		}
		
 			// ambient floor/ceiling segments
			
		if (portal_has_segment_type(map,i,sg_ambient_fc)) {
			xml_add_tagstart("Ambient_FCs");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nsegment;k++) {
				seg=&map->segments[k];
				if (seg->rn!=i) continue;
				if (seg->type!=sg_ambient_fc) continue;
		
				xml_add_tagstart("Ambient_FC");
				write_single_segment(seg);
				xml_add_tagend(FALSE);
			
				for (j=0;j!=seg->data.ambient_fc.ptsz;j++) {
					write_single_vertex(seg->data.ambient_fc.x[j],seg->data.ambient_fc.y[j],seg->data.ambient_fc.z[j]);
				}
			
				xml_add_tagclose("Ambient_FC");
			}
			
			xml_add_tagclose("Ambient_FCs");
		}
		
			// scenery

		if (portal_has_scenery(map,i)) {
			xml_add_tagstart("Sceneries");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nscenery;k++) {
				scenery=&map->sceneries[k];
				if (scenery->pos.rn!=i) continue;
				
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
        
		if (portal_has_map_light(map,i)) {
			xml_add_tagstart("Lights");
			xml_add_tagend(FALSE);
		
			for (k=0;k!=map->nlight;k++) {
				light=&map->lights[k];
				if (light->pos.rn!=i) continue;
		
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
			
		if (portal_has_map_sound(map,i)) {
			xml_add_tagstart("Sounds");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nsound;k++) {
				sound=&map->sounds[k];
				if (sound->pos.rn!=i) continue;
			
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
			
		if (portal_has_map_particle(map,i)) {
			xml_add_tagstart("Particles");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nparticle;k++) {
				particle=&map->particles[k];
				if (particle->pos.rn!=i) continue;
			
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
        
		if (portal_has_node(map,i)) {
			xml_add_tagstart("Nodes");
			xml_add_tagend(FALSE);
		
			for (k=0;k!=map->nnode;k++) {
				node=&map->nodes[k];
				if (node->pos.rn!=i) continue;
			
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

		if (portal_has_spot(map,i)) {
			xml_add_tagstart("Spots");
			xml_add_tagend(FALSE);
			
			for (k=0;k!=map->nspot;k++) {
				spot=&map->spots[k];
				if (spot->pos.rn!=i) continue;
				
				xml_add_tagstart("Spot");
				xml_add_attribute_int("id",k);
				xml_add_attribute_3_coord_int("c3",spot->pos.x,spot->pos.y,spot->pos.z);
				xml_add_attribute_text("name",spot->name);
				xml_add_attribute_text("type",spot->type);
				xml_add_attribute_text("script",spot->script);
				xml_add_attribute_text("display_model",spot->display_model);
				xml_add_attribute_text("params",spot->params);
				xml_add_attribute_float("angle",spot->ang.y);
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

