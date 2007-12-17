/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Segment Drawing Setup

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

#include "consoles.h"
#include "video.h"

extern bool				hilite_on;

extern map_type			map;
extern server_type		server;
extern setup_type		setup;
extern camera_type		camera;
extern view_type		view;

extern void portal_compile_gl_lists(int tick,int rn);

/* =======================================================

      Normal Smoothing
      
======================================================= */

void segment_normal_smooth_init(void)
{
	int				n;
	segment_type	*seg;

	seg=map.segments;

	for (n=0;n!=map.nsegment;n++) {
		seg->render.normal[0]=0.5f;
		seg->render.normal[1]=0.5f;
		seg->render.normal[2]=1.0f;
		seg++;
	}
}

/* =======================================================

      Setup Segments To Render
      
======================================================= */

void segment_render_setup_single_opaque(portal_type *portal,short s_idx,segment_type *seg,texture_type *texture,int frame,int lod_dist,bool light_changed)
{
	portal_segment_draw_type	*draw;

	draw=&portal->segment_draw;
	
		// opaque bumps

	if ((setup.bump_mapping) && (lod_dist<map.optimizations.lod_bump_distance) && (texture->bumpmaps[frame].gl_id!=-1)) {
		draw->opaque_bump_list[draw->opaque_bump_count++]=s_idx;
		draw->opaque_light_list[draw->opaque_light_count++]=s_idx;

		if ((!light_changed) || (seg->moveable)) map_portal_calculate_normal_vector_smooth(portal,(double)seg->middle.x,(double)seg->middle.y,(double)seg->middle.z,seg->render.normal);
	}
	else {
		if (seg->render.light_simple) {
			draw->opaque_simple_normal_list[draw->opaque_simple_normal_count++]=s_idx;
		}
		else {
			draw->opaque_normal_list[draw->opaque_normal_count++]=s_idx;
			draw->opaque_light_list[draw->opaque_light_count++]=s_idx;
		}
	}

		// opaque specular

	if ((setup.specular_mapping) && (texture->specularmaps[frame].gl_id!=-1) && (lod_dist<map.optimizations.lod_specular_distance)) {
		draw->opaque_specular_list[draw->opaque_specular_count++]=s_idx;
	}
	
		// opaque glow

	if ((setup.glow_mapping) && (texture->glowmaps[frame].gl_id!=-1) && (lod_dist<map.optimizations.lod_glow_distance)) {
		draw->opaque_glow_list[draw->opaque_glow_count++]=s_idx;
	}
}

void segment_render_setup(int tick,int portal_cnt,int *portal_list)
{
	int							n,i,rn,cnt,frame,
								lod_dist,stencil_pass,stencil_idx;
	short						s_idx;
	bool						light_changed;
	short						*sptr;
	portal_type					*portal;
	segment_type				*seg;
	texture_type				*texture;
	portal_segment_draw_type	*draw;
	
		// setup segment rendering for draw types
	
	for (n=(portal_cnt-1);n>=0;n--) {
		rn=portal_list[n];
		
		portal=&map.portals[rn];
		draw=&portal->segment_draw;
		
			// create vertex, normal, and color lists
			
		portal_compile_gl_lists(tick,rn);
			
			// get ready for opaque/bump/transparent sorting
		
		draw->opaque_normal_count=0;
		draw->opaque_bump_count=0;
		draw->opaque_light_count=0;
		draw->opaque_simple_normal_count=0;
		draw->opaque_specular_count=0;
		draw->opaque_glow_count=0;
		draw->transparent_count=0;
		draw->shader_count=0;
		
			// has lighting changed in this portal?
			
		light_changed=map_portal_light_check_changes(portal);
	
			// create segment rendering values and lists

		cnt=portal->segment_list_draw.count;
		sptr=portal->segment_list_draw.list;

		for (i=0;i!=cnt;i++) {
			s_idx=*sptr++;
			
			seg=&map.segments[s_idx];
			
			texture=&map.textures[seg->fill];
			frame=(texture->animate.current_frame+seg->txt_offset)&max_texture_frame_mask;

			lod_dist=abs(seg->middle.x-view.camera.pos.x)+abs(seg->middle.y-view.camera.pos.y)+abs(seg->middle.z-view.camera.pos.z);

				// default setup
		
			seg->render.light_simple=(!setup.high_quality_lighting) || (lod_dist>map.optimizations.lod_light_distance) || (seg->simple_tessel);
			seg->render.lod_dist=lod_dist;

				// clear the stencil
				
			seg->render.stencil_pass=0;
			seg->render.stencil_idx=stencil_none;
			
				// shader only?
				
			if (texture->shader.on) {
				draw->shader_list[draw->shader_count++]=s_idx;
				continue;
			}
			
				// opaque
				
			if ((seg->alpha==1.0f) && (texture->bitmaps[frame].alpha_mode!=alpha_mode_transparent)) {
				segment_render_setup_single_opaque(portal,s_idx,seg,texture,frame,lod_dist,light_changed);
				continue;
			}
			
				// transparent
				
			draw->transparent_list[draw->transparent_count++]=s_idx;
		}

		portal++;
	}

		// setup the opaque per-portal stencil passes
		// there might be more than 256 segments per portal, so we need to run in passes
		// transparent items don't use stencils
		
	stencil_pass=0;
	stencil_idx=stencil_segment_start;
	
	for (n=(portal_cnt-1);n>=0;n--) {
		rn=portal_list[n];

		portal=&map.portals[rn];
		draw=&portal->segment_draw;

		portal->opaque_stencil_pass_start=stencil_pass;

		cnt=draw->opaque_light_count;
		sptr=draw->opaque_light_list;
		
		for (i=0;i!=cnt;i++) {
			seg=&map.segments[*sptr++];
			
			seg->render.stencil_pass=stencil_pass;
			seg->render.stencil_idx=stencil_idx;
			
			stencil_idx++;
			if (stencil_idx>stencil_segment_end) {
				stencil_idx=stencil_segment_start;
				stencil_pass++;
			}
		}

		portal->opaque_stencil_pass_end=stencil_pass;
			
		portal++;
	}
}
