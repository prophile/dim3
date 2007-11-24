/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Specific OS Definitions

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ñCreated with dim3 Technologyî is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2005 Klink! Software www.klinksoftware.com

*********************************************************************/

//
// includes
//

#include <winsock2.h>
#include <windows.h>

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <shlobj.h>

#include "glew.h"
#include "wglew.h"
#include <GL\gl.h>
#include <GL\glu.h>

#include "SDL_syswm.h"

#include "pthread.h"
#include "zlib.h"
#include "png.h"

//
// os flags
//

#define D3_OS_WINDOWS			1

//
// undefined types, constants, and functions
//

typedef int						bool;
typedef unsigned char*			ptr;
typedef SOCKET					d3socket;
typedef int						socklen_t;

#define FOUR_CHAR_CODE(x)		(x)

#define inline					__inline extern
#define random					rand
#define srandom					srand
#define snprintf				_snprintf
#define strtof(a,b)				((float)strtod(a,b))
#define valloc					malloc
#define bzero(a,b)				memset(a,0x0,(b))
#define bcmp(a,b,c)				memcmp(a,b,(c))
#define usleep(x)               Sleep((DWORD)((x)/1000))
#define fabsf(f)				((float)fabs((double)f))
#define sqrtf(f)				((float)sqrt((double)f))
#define strcasecmp(a,b)			_stricmp(a,b)
#define stat					_stat

#define D3_NULL_SOCKET			INVALID_SOCKET
