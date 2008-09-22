/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: View Bitmaps

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

#include "objects.h"
#include "remotes.h"
#include "consoles.h"
#include "interfaces.h"
#include "effects.h"
#include "video.h"

extern server_type			server;
extern view_type			view;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;

extern int					remote_slow_image_idx,remote_talk_image_idx;

/* =======================================================

      Load Images
      
======================================================= */

int view_images_load_single(char *path,bool simple)
{
	int					n,idx;
	view_image_type		*image;

		// already loaded?

	image=view.images;

	for (n=0;n!=view.count.image;n++) {
		if (strcmp(image->path,path)==0) return(n);
		image++;
	}

		// load it

	idx=view.count.image;
	view.count.image++;

	image=&view.images[idx];

	if (simple) {
		bitmap_open(&image->bitmap,path,anisotropic_mode_none,texture_quality_mode_high,mipmap_mode_none,FALSE,FALSE,FALSE);
	}
	else {
		bitmap_open(&image->bitmap,path,setup.anisotropic_mode,setup.texture_quality_mode,setup.mipmap_mode,setup.mipmap_card_generated,setup.texture_compression,FALSE);
	}

	strcpy(image->path,path);

	return(idx);
}

void view_images_load(void)
{
	int					n;
	char				path[1024];
	hud_bitmap_type		*hud_bitmap;
	hud_radar_icon_type	*icon;
	halo_type			*halo;
	mark_type			*mark;
	crosshair_type		*crosshair;
	particle_type		*particle;
	ring_type			*ring;

	view.count.image=0;
    
		// hud bitmaps

	hud_bitmap=hud.bitmaps;
	
	for (n=0;n!=hud.count.bitmap;n++) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Interface",hud_bitmap->filename,"png");
		hud_bitmap->image_idx=view_images_load_single(path,TRUE);
		hud_bitmap++;
	}

		// radar

	if (hud.radar.on) {
	
			// radar background

		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Radar",hud.radar.background_bitmap_name,"png");
		hud.radar.background_image_idx=view_images_load_single(path,TRUE);

			// radar icons
		
		icon=hud.radar.icons;
		
		for (n=0;n!=hud.radar.nicon;n++) {
			file_paths_data(&setup.file_path_setup,path,"Bitmaps/Radar",icon->bitmap_name,"png");
			icon->image_idx=view_images_load_single(path,TRUE);
			icon++;
		}
	}

		// halos

	halo=server.halos;
	
	for (n=0;n!=server.count.halo;n++) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Halos",halo->bitmap_name,"png");
		halo->image_idx=view_images_load_single(path,FALSE);
		halo++;
	}

		// marks

	mark=server.marks;
	
	for (n=0;n!=server.count.mark;n++) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Marks",mark->bitmap_name,"png");
		mark->image_idx=view_images_load_single(path,FALSE);
		mark++;
	}

		// crosshairs

	crosshair=server.crosshairs;
	
	for (n=0;n!=server.count.crosshair;n++) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Crosshairs",crosshair->bitmap_name,"png");
		crosshair->image_idx=view_images_load_single(path,TRUE);
		crosshair++;
	}

		// particles

	particle=server.particles;
	
	for (n=0;n!=server.count.particle;n++) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Particles",particle->bitmap_name,"png");
		particle->image_idx=view_images_load_single(path,FALSE);
		particle++;
	}
	
		// rings

	ring=server.rings;
	
	for (n=0;n!=server.count.ring;n++) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Rings",ring->bitmap_name,"png");
		ring->image_idx=view_images_load_single(path,FALSE);
		ring++;
	}

		// remote bitmaps

	if (net_setup.client.joined) {
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Network","slow","png");
		remote_slow_image_idx=view_images_load_single(path,TRUE);
	
		file_paths_data(&setup.file_path_setup,path,"Bitmaps/Network","talk","png");
		remote_talk_image_idx=view_images_load_single(path,TRUE);
	}
}

/* =======================================================

      Free Images
      
======================================================= */

void view_images_free(void)
{
	int					n;
	view_image_type		*image;
    
		// close the images
		
	image=view.images;
	
	for (n=0;n!=view.count.image;n++) {
		image->path[0]=0x0;
		bitmap_close(&image->bitmap);
		image++;
	}
}

/* =======================================================

      Get Images
      
======================================================= */

inline bitmap_type* view_images_get_bitmap(int idx)
{
	return(&view.images[idx].bitmap);
}

inline unsigned long view_images_get_gl_id(int idx)
{
	return(view.images[idx].bitmap.gl_id);
}
