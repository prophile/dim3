/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Movies to Textures

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

#ifdef D3_OS_MAC
unsigned long					movie_gl_id;
Movie							movie;
GWorldPtr						movie_gworld;
ptr								movie_data;
#endif

/* =======================================================

      Movie Play Start and Stop
      
======================================================= */

bool bitmap_movie_start(char *path,int *wid,int *high)
{
#ifndef D3_OS_MAC
	return(FALSE);
#else
	int				err,psz;
	short			ref_num;
	float			fx,fy;
	FSSpec			fspec;
	FSRef			fsref;
	Rect			box,movie_box;
	MatrixRecord	matrix;
	
		// start movies
		
	err=EnterMovies();
	if (err!=noErr) return(FALSE);
	
		// load the movie
		
	FSPathMakeRef((unsigned char*)path,&fsref,NULL);
	FSGetCatalogInfo(&fsref,kFSCatInfoNone,NULL,NULL,&fspec,NULL);
	
	err=OpenMovieFile(&fspec,&ref_num,fsRdPerm);
	if (err!=noErr) {
		ExitMovies();
		return(FALSE);
	}
	
	err=NewMovieFromFile(&movie,ref_num,NULL,NULL,newMovieActive,NULL);
	CloseMovieFile(ref_num);
	if (err!=noErr) {
		ExitMovies();
		return(FALSE);
	}
	
		// create a gworld to draw movie to
		
	SetRect(&box,0,0,movie_wid,movie_high);
	NewGWorld(&movie_gworld,32,&box,NULL,NULL,0);
	
	SetMovieGWorld(movie,(CGrafPtr)movie_gworld,NULL);
	
		// scale the movie to the gworld
		
	GetMovieBox(movie,&movie_box);

	*wid=movie_box.right-movie_box.left;
	*high=movie_box.bottom-movie_box.top;
	
	fx=(float)movie_wid/(float)(movie_box.right-movie_box.left);
	fy=(float)movie_high/(float)(movie_box.bottom-movie_box.top);
	
	SetIdentityMatrix(&matrix);
	ScaleMatrix (&matrix,X2Fix(fx),X2Fix(fy),X2Fix(0.0),X2Fix(0.0));
	SetMovieMatrix(movie,&matrix);
	
	SetMovieBox(movie,&box);
	
		// create covert data for textures
		
	psz=(movie_wid<<2)*movie_high;
	movie_data=valloc(psz);
	if (movie_data==NULL) {
		DisposeGWorld(movie_gworld);
		ExitMovies();
		return(FALSE);
	}
	
	memset(movie_data,0x0,psz);
	
		// setup the texture
		
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1,&movie_gl_id);
	
    glBindTexture(GL_TEXTURE_2D,movie_gl_id);
	
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
		// load texture
		
	glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE,1);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,movie_wid,movie_high,0,GL_RGBA,GL_UNSIGNED_BYTE,movie_data);
	glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE,0);
	
		// start the movie playing
		
	GoToBeginningOfMovie(movie);
	StartMovie(movie);
	
	return(TRUE);
#endif
}

void bitmap_movie_end(void)
{
#ifdef D3_OS_MAC
	StopMovie(movie);
	
	DisposeGWorld(movie_gworld);
	free(movie_data);

	ExitMovies();
#endif
}

/* =======================================================

      Movie Refresh Texture
      
======================================================= */

int bitmap_movie_get_texture_id(void)
{
#ifndef D3_OS_MAC
	return(-1);
#else
	return(movie_gl_id);
#endif
}

bool bitmap_movie_playing(void)
{
#ifndef D3_OS_MAC
	return(FALSE);
#else
	return(!IsMovieDone(movie));
#endif
}

void bitmap_movie_texture_refresh(void)
{
#ifndef D3_OS_MAC
	return;
#else
	int				x,y,rbyte;
	ptr				pptr,sptr,dptr;
	PixMapHandle	pmap;
	
		// update the movie
		
	MoviesTask(movie,10);
	
		// covert to RGBA
		
	pmap=GetGWorldPixMap(movie_gworld);
		
	LockPixels(pmap);
	
	pptr=(ptr)GetPixBaseAddr(pmap);
	rbyte=GetPixRowBytes(pmap);
	dptr=movie_data;
	
	for (y=0;y!=movie_high;y++) {
		sptr=pptr+(rbyte*y);
		
		for (x=0;x!=movie_wid;x++) {
			*sptr++;
			*dptr++=*sptr++;
			*dptr++=*sptr++;
			*dptr++=*sptr++;
			*dptr++=0xFF;					// ignore the alpha
		}
	}

	UnlockPixels(pmap);
	
		// change the texture
		
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,movie_gl_id);
		
	glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE,1);
	glTexSubImage2D(GL_TEXTURE_2D,0,0,0,movie_wid,movie_high,GL_RGBA,GL_UNSIGNED_INT_8_8_8_8,movie_data);
	glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE,0);
#endif
}

