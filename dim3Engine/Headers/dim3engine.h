/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Main Header

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
// System Includes
//

#include "os_defs.h"

#ifdef D3_OS_MAC
	#include <SDL/SDL.h>
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
	#include <OpenAL/MacOSX_OALExtensions.h>
#endif

#ifdef D3_OS_LINUX
	#include <SDL/SDL.h>
	#include <AL/al.h>
	#include <AL/alc.h>
#endif

#ifdef D3_OS_WINDOWS
	#include "SDL.h"
	#include "al.h"
	#include "alc.h"
#endif

//
// Application Headers
//

#include "baseutility.h"
#include "modelutility.h"
#include "maputility.h"

#include "setup_defs.h"
#include "script_defs.h"
#include "server_defs.h"
#include "camera_defs.h"
#include "view_defs.h"
#include "network_defs.h"
#include "interface_defs.h"
#include "video_defs.h"
#include "sound_defs.h"
#include "input_defs.h"

