/****************************** File *********************************

Module: dim3 Map Utility
Author: Brian Barnes
 Usage: Auto-Generate Blocking

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
	#include "dim3maputility.h"
#endif

unsigned char ag_block_data_empty[max_ag_block_sz][max_ag_block_sz]= \
						{ \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}};

unsigned char ag_block_data_circle[max_ag_block_sz][max_ag_block_sz]= \
						{ \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}};

unsigned char ag_block_data_top_u[max_ag_block_sz][max_ag_block_sz]= \
						{ \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}};
							
unsigned char ag_block_data_bottom_u[max_ag_block_sz][max_ag_block_sz]= \
						{ \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}};

unsigned char ag_block_data_left_u[max_ag_block_sz][max_ag_block_sz]= \
						{ \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{1,1,1,1,1,0,0,0,0,0}, \
						{1,1,1,1,1,0,0,0,0,0}, \
						{1,1,1,1,1,0,0,0,0,0}, \
						{1,1,1,1,1,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}};
							
unsigned char ag_block_data_right_u[max_ag_block_sz][max_ag_block_sz]= \
						{ \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,1,1,1,1,1}, \
						{0,0,0,0,0,1,1,1,1,1}, \
						{0,0,0,0,0,1,1,1,1,1}, \
						{0,0,0,0,0,1,1,1,1,1}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}};

unsigned char ag_block_data_vertical_h[max_ag_block_sz][max_ag_block_sz]= \
						{ \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}, \
						{0,0,0,1,1,1,1,0,0,0}};

unsigned char ag_block_data_horizontal_h[max_ag_block_sz][max_ag_block_sz]= \
						{ \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{1,1,1,0,0,0,0,1,1,1}, \
						{1,1,1,0,0,0,0,1,1,1}, \
						{1,1,1,0,0,0,0,1,1,1}, \
						{1,1,1,0,0,0,0,1,1,1}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}, \
						{0,0,0,0,0,0,0,0,0,0}};

/* =======================================================

      Auto-Generate Custom Blocking
      
======================================================= */

void map_auto_generate_block_preset(auto_generate_settings_type *ags,int block)
{
	switch (block) {
	
		case ag_block_preset_empty:
			memmove(ags->block,ag_block_data_empty,((sizeof(unsigned char)*max_ag_block_sz)*max_ag_block_sz));
			break;
	
		case ag_block_preset_circle:
			memmove(ags->block,ag_block_data_circle,((sizeof(unsigned char)*max_ag_block_sz)*max_ag_block_sz));
			break;
			
		case ag_block_preset_top_u:
			memmove(ags->block,ag_block_data_top_u,((sizeof(unsigned char)*max_ag_block_sz)*max_ag_block_sz));
			break;

		case ag_block_preset_bottom_u:
			memmove(ags->block,ag_block_data_bottom_u,((sizeof(unsigned char)*max_ag_block_sz)*max_ag_block_sz));
			break;

		case ag_block_preset_left_u:
			memmove(ags->block,ag_block_data_left_u,((sizeof(unsigned char)*max_ag_block_sz)*max_ag_block_sz));
			break;

		case ag_block_preset_right_u:
			memmove(ags->block,ag_block_data_right_u,((sizeof(unsigned char)*max_ag_block_sz)*max_ag_block_sz));
			break;

		case ag_block_preset_vertical_h:
			memmove(ags->block,ag_block_data_vertical_h,((sizeof(unsigned char)*max_ag_block_sz)*max_ag_block_sz));
			break;

		case ag_block_preset_horizontal_h:
			memmove(ags->block,ag_block_data_horizontal_h,((sizeof(unsigned char)*max_ag_block_sz)*max_ag_block_sz));
			break;
	}

}

