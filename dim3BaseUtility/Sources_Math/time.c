/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
	    Keith Bauer for Mac/Linux Timing Code
 Usage: Timing

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
	#include "dim3baseutility.h"
#endif

#ifndef D3_OS_WINDOWS
    static double           start_time;
#else
	static double			start_time,freq_time;
#endif

/* =======================================================

      Mac and Linux Low-Level Time Routines
      
======================================================= */

#ifndef D3_OS_WINDOWS

static double time_now()
{
    struct timeval		now;
	
    gettimeofday(&now, NULL);
    return((double)now.tv_sec + (double)now.tv_usec * 0.000001);
}

void time_start(void)
{
	start_time = time_now();
}

int time_get(void)
{
	return((time_now() - start_time) * 1000.0);
}

#endif

/* =======================================================

      Windows Low-Level Time Routines
      
======================================================= */

#ifdef D3_OS_WINDOWS

void time_start(void)
{
	LARGE_INTEGER		time;

	QueryPerformanceFrequency(&time);
	freq_time=(double)time.QuadPart;

	QueryPerformanceCounter(&time);
	start_time=(double)time.QuadPart;
}

int time_get(void)
{
	double				d;
	LARGE_INTEGER		time;

	QueryPerformanceCounter(&time);
	d=(double)time.QuadPart;

	return((int)(((d-start_time)/freq_time)*1000.0));
}

#endif

