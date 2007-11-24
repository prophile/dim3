/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Camera Definitions Header

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
// camera modes
//

#define cv_fpp						0
#define cv_chase					1
#define cv_static					2
#define cv_chase_static				3

//
// projection types
//

#define cp_fov						0
#define cp_frustum					1

//
// camera structures
//

typedef struct		{
						int						node_seek_idx,node_dest_idx,
												msec,event_id;
						float					speed;
						bool					on,open_doors,in_freeze;
					} camera_auto_walk;

typedef struct		{
						int						type,
												lft,rgt,top,bot,
												near_z,far_z,near_z_offset;
						float					fov,aspect_ratio;
					} camera_plane;
					
typedef struct		{
						int						mode,chase_size,chase_distance,
												obj_uid;
						float					chase_track_speed;
						bool					static_follow;
						d3pos					pos,chase_offset;
						d3ang					ang,chase_ang,chase_slop,cur_chase_ang;
						camera_auto_walk		auto_walk;
						camera_plane			plane;
					} camera_type;
					
