/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Video Definitions Header

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

//
// vertex objects
//

#define view_vertex_object_count				8

//
// back renderers
//

#define back_render_texture_pixel_size			256

//
// shadows
//

#define shadow_max_poly_draw					128

//
// screen sizes
//

#define max_screen_size							64

typedef struct		{
						int						wid,high;
					} screen_size_type;
					
//
// render info
//
 
#define max_fps									100

typedef struct		{
						int						view_x,view_y,
												monitor_x_sz,monitor_y_sz,
												monitor_refresh_rate,
												texture_unit_count,texture_max_size,
												nscreen_size;
						char					name[64],ext_string[8192];
						screen_size_type		screen_sizes[max_screen_size];
					} render_info_type;

//
// fonts
//

#define tx_left									0
#define tx_center								1
#define tx_right								2
