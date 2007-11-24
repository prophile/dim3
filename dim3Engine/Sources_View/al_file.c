/****************************** File *********************************

Module: dim3 Engine
Author: Code lifted from original free ALUT installation, www.openal.org
        Various edits/Byte Swaping by Brian Barnes
 Usage: Wav File Loading

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

//
// wave file definitions
//

#if defined _MSC_VER
	#pragma pack (push,1) 							/* Turn off alignment */
#elif defined __GNUC__
	#define PADOFF_VAR __attribute__((packed))
#endif

#ifndef PADOFF_VAR
	#define PADOFF_VAR
#endif

typedef struct                                  /* WAV File-header */
{
  uint8	   Id[4]			PADOFF_VAR;
  int      Size				PADOFF_VAR;
  uint8	   Type[4]			PADOFF_VAR;
} WAVFileHdr_Struct;

typedef struct                                  /* WAV Fmt-header */
{
  uint16   Format			PADOFF_VAR;
  uint16   Channels			PADOFF_VAR;
  uint32   SamplesPerSec	PADOFF_VAR;
  uint32   BytesPerSec		PADOFF_VAR;
  uint16   BlockAlign		PADOFF_VAR;
  uint16   BitsPerSample	PADOFF_VAR;
} WAVFmtHdr_Struct;

typedef struct									/* WAV FmtEx-header */
{
  uint16   Size				PADOFF_VAR;
  uint16   SamplesPerBlock	PADOFF_VAR;
} WAVFmtExHdr_Struct;

typedef struct                                  /* WAV Smpl-header */
{
  uint32   Manufacturer		PADOFF_VAR;
  uint32   Product			PADOFF_VAR;
  uint32   SamplePeriod		PADOFF_VAR;
  uint32   Note				PADOFF_VAR;
  uint32   FineTune			PADOFF_VAR;
  uint32   SMPTEFormat		PADOFF_VAR;
  uint32   SMPTEOffest		PADOFF_VAR;
  uint32   Loops			PADOFF_VAR;
  uint32   SamplerData		PADOFF_VAR;
  struct
  {
	uint32 Identifier		PADOFF_VAR;
	uint32 Type				PADOFF_VAR;
	uint32 Start			PADOFF_VAR;
	uint32 End				PADOFF_VAR;
	uint32 Fraction			PADOFF_VAR;
	uint32 Count			PADOFF_VAR;
  }      Loop[1]			PADOFF_VAR;
} WAVSmplHdr_Struct;

typedef struct                                  /* WAV Chunk-header */
{
  uint8	   Id[4]			PADOFF_VAR;
  uint32   Size				PADOFF_VAR;
} WAVChunkHdr_Struct;

#ifdef PADOFF_VAR			    				/* Default alignment */
	#undef PADOFF_VAR
#endif

#if defined _MSC_VER
	#pragma pack (pop)
#endif

/* =======================================================

      Byte Swaping Utilities

======================================================= */

uint16 al_load_wav_file_byte_swap_16(uint16 b16)
{
	uint16			sb16;
	char			*p1,*p2;
	
	p1=(char*)&sb16;
	p2=(char*)&b16;
	
	p1[0]=p2[1];
	p1[1]=p2[0];
	
	return(sb16);
}

uint32 al_load_wav_file_byte_swap_32(uint32 b32)
{
	uint32			sb32;
	char			*p1,*p2;
	
	p1=(char*)&sb32;
	p2=(char*)&b32;
	
	p1[0]=p2[3];
	p1[1]=p2[2];
	p1[2]=p2[1];
	p1[3]=p2[0];
	
	return(sb32);
}

void al_load_wav_file_byte_swap_file_header(WAVFileHdr_Struct *FileHdr)
{
	#ifdef __BIG_ENDIAN__
		FileHdr->Size=al_load_wav_file_byte_swap_32(FileHdr->Size);
	#endif
}

void al_load_wav_file_byte_swap_format_header(WAVFmtHdr_Struct *FmtHdr)
{
	#ifdef __BIG_ENDIAN__
		FmtHdr->Format=al_load_wav_file_byte_swap_16(FmtHdr->Format);
		FmtHdr->Channels=al_load_wav_file_byte_swap_16(FmtHdr->Channels);
		FmtHdr->SamplesPerSec=al_load_wav_file_byte_swap_32(FmtHdr->SamplesPerSec);
		FmtHdr->BytesPerSec=al_load_wav_file_byte_swap_32(FmtHdr->BytesPerSec);
		FmtHdr->BlockAlign=al_load_wav_file_byte_swap_16(FmtHdr->BlockAlign);
		FmtHdr->BitsPerSample=al_load_wav_file_byte_swap_16(FmtHdr->BitsPerSample);
	#endif
}

void al_load_wav_file_byte_swap_format_ext_header(WAVFmtExHdr_Struct *FmtExHdr)
{
	#ifdef __BIG_ENDIAN__
		FmtExHdr->Size=al_load_wav_file_byte_swap_16(FmtExHdr->Size);
		FmtExHdr->SamplesPerBlock=al_load_wav_file_byte_swap_16(FmtExHdr->SamplesPerBlock);
	#endif
}

void al_load_wav_file_byte_swap_sample_header(WAVSmplHdr_Struct *SmplHdr)
{
	#ifdef __BIG_ENDIAN__
		SmplHdr->Manufacturer=al_load_wav_file_byte_swap_32(SmplHdr->Manufacturer);
		SmplHdr->Product=al_load_wav_file_byte_swap_32(SmplHdr->Product);
		SmplHdr->SamplePeriod=al_load_wav_file_byte_swap_32(SmplHdr->SamplePeriod);
		SmplHdr->Note=al_load_wav_file_byte_swap_32(SmplHdr->Note);
		SmplHdr->FineTune=al_load_wav_file_byte_swap_32(SmplHdr->FineTune);
		SmplHdr->SMPTEFormat=al_load_wav_file_byte_swap_32(SmplHdr->SMPTEFormat);
		SmplHdr->SMPTEOffest=al_load_wav_file_byte_swap_32(SmplHdr->SMPTEOffest);
		SmplHdr->Loops=al_load_wav_file_byte_swap_32(SmplHdr->Loops);
		SmplHdr->SamplerData=al_load_wav_file_byte_swap_32(SmplHdr->SamplerData);
			// this loader isn't concerned with loops, so ignore the count
		SmplHdr->Loop[0].Identifier=al_load_wav_file_byte_swap_32(SmplHdr->Loop[0].Identifier);
		SmplHdr->Loop[0].Type=al_load_wav_file_byte_swap_32(SmplHdr->Loop[0].Type);
		SmplHdr->Loop[0].Start=al_load_wav_file_byte_swap_32(SmplHdr->Loop[0].Start);
		SmplHdr->Loop[0].End=al_load_wav_file_byte_swap_32(SmplHdr->Loop[0].End);
		SmplHdr->Loop[0].Fraction=al_load_wav_file_byte_swap_32(SmplHdr->Loop[0].Fraction);
		SmplHdr->Loop[0].Count=al_load_wav_file_byte_swap_32(SmplHdr->Loop[0].Count);
	#endif
}

void al_load_wav_file_byte_swap_chunk_header(WAVChunkHdr_Struct *ChunkHdr)
{
	#ifdef __BIG_ENDIAN__
		ChunkHdr->Size=al_load_wav_file_byte_swap_32(ChunkHdr->Size);
	#endif
}

void al_load_wav_file_byte_swap_data(char *data,int data_sz,int bits_per_sample)
{
	#ifdef __BIG_ENDIAN__
	
		int				n;
		uint16			*b16;
		uint32			*b32;
		
		if (bits_per_sample==8) return;
		
			// 16 bit swaps
			
		if (bits_per_sample==16) {
		
			b16=(uint16*)data;
			for (n=0;n!=(data_sz>>1);n++) {
				*b16=al_load_wav_file_byte_swap_16(*b16);
				b16++;
			}
			
			return;
		}
		
			// 32 bit swaps
			
		b32=(uint32*)data;
		for (n=0;n!=(data_sz>>2);n++) {
			*b32=al_load_wav_file_byte_swap_32(*b32);
			b32++;
		}
		
	#endif
}

/* =======================================================

      Load Wav Files

======================================================= */

void al_load_wav_file(ALbyte *file,ALenum *format,void **data,ALsizei *size,ALsizei *freq,ALboolean *loop)
{
	WAVChunkHdr_Struct		ChunkHdr;
	WAVFmtExHdr_Struct		FmtExHdr;
	WAVFileHdr_Struct		FileHdr;
	WAVSmplHdr_Struct		SmplHdr;
	WAVFmtHdr_Struct		FmtHdr;
	FILE					*Stream;
	
	*format=AL_FORMAT_MONO16;
	*data=NULL;
	*size=0;
	*freq=22050;
	*loop=AL_FALSE;
	
		// open the file
		
	if (!file) return;
	
	Stream=fopen(file,"rb");
	if (Stream==NULL) return;
	
		// read the file
		
	fread(&FileHdr,1,sizeof(WAVFileHdr_Struct),Stream);
	al_load_wav_file_byte_swap_file_header(&FileHdr);
	
	FileHdr.Size=((FileHdr.Size+1)&~1)-4;
	
	while (FileHdr.Size!=0) {
	
		if (!fread(&ChunkHdr,1,sizeof(WAVChunkHdr_Struct),Stream)) break;
		al_load_wav_file_byte_swap_chunk_header(&ChunkHdr);
		
			// format chunk
			
		if (!memcmp(ChunkHdr.Id,"fmt ",4)) {
		
			fread(&FmtHdr,1,sizeof(WAVFmtHdr_Struct),Stream);
			al_load_wav_file_byte_swap_format_header(&FmtHdr);
		
			if ((FmtHdr.Format==0x0001)||(FmtHdr.Format==0xFFFE)) {
			
				if (FmtHdr.Channels==1) {
					*format=(FmtHdr.BitsPerSample==4?alGetEnumValue((ALchar*)"AL_FORMAT_MONO_IMA4"):(FmtHdr.BitsPerSample==8?AL_FORMAT_MONO8:AL_FORMAT_MONO16));
				}
				else {
					if (FmtHdr.Channels==2) {
						*format=(FmtHdr.BitsPerSample==4?alGetEnumValue((ALchar*)"AL_FORMAT_STEREO_IMA4"):(FmtHdr.BitsPerSample==8?AL_FORMAT_STEREO8:AL_FORMAT_STEREO16));
					}
				}
				
				*freq=FmtHdr.SamplesPerSec;
				fseek(Stream,ChunkHdr.Size-sizeof(WAVFmtHdr_Struct),SEEK_CUR);
			} 
			else {
			
				if (FmtHdr.Format==0x0011)
				{
					if (FmtHdr.Channels==1) {
						*format=alGetEnumValue((ALchar*)"AL_FORMAT_MONO_IMA4");
					}
					else {
						if (FmtHdr.Channels==2) {
							*format=alGetEnumValue((ALchar*)"AL_FORMAT_STEREO_IMA4");
						}
					}
					*freq=FmtHdr.SamplesPerSec;
					fseek(Stream,ChunkHdr.Size-sizeof(WAVFmtHdr_Struct),SEEK_CUR);
				}
				else
				{
					fread(&FmtExHdr,1,sizeof(WAVFmtExHdr_Struct),Stream);
					al_load_wav_file_byte_swap_format_ext_header(&FmtExHdr);
					
					fseek(Stream,ChunkHdr.Size-sizeof(WAVFmtHdr_Struct)-sizeof(WAVFmtExHdr_Struct),SEEK_CUR);
				}
			}
			
			fseek(Stream,ChunkHdr.Size&1,SEEK_CUR);
			FileHdr.Size-=(((ChunkHdr.Size+1)&~1)+8);
			
			continue;
		}

			// data chunk
			
		if (!memcmp(ChunkHdr.Id,"data",4)) {
			*size=ChunkHdr.Size;
			
			*data=malloc(ChunkHdr.Size+31);
			if (*data) {
				fread(*data,FmtHdr.BlockAlign,ChunkHdr.Size/FmtHdr.BlockAlign,Stream);
				al_load_wav_file_byte_swap_data(*data,(FmtHdr.BlockAlign*(ChunkHdr.Size/FmtHdr.BlockAlign)),FmtHdr.BitsPerSample);
				memset(((char*)*data)+ChunkHdr.Size,0,31);
			}
			
			fseek(Stream,ChunkHdr.Size&1,SEEK_CUR);
			FileHdr.Size-=(((ChunkHdr.Size+1)&~1)+8);
			
			continue;
		}
		
			// sample chunk
			
		if (!memcmp(ChunkHdr.Id,"smpl",4)) {
			fread(&SmplHdr,1,sizeof(WAVSmplHdr_Struct),Stream);
			al_load_wav_file_byte_swap_sample_header(&SmplHdr);
			
			*loop = (SmplHdr.Loops ? AL_TRUE : AL_FALSE);
			fseek(Stream,ChunkHdr.Size-sizeof(WAVSmplHdr_Struct),SEEK_CUR);
			
			fseek(Stream,ChunkHdr.Size&1,SEEK_CUR);
			FileHdr.Size-=(((ChunkHdr.Size+1)&~1)+8);
			continue;
		}
		
		fseek(Stream,ChunkHdr.Size,SEEK_CUR);
		
		fseek(Stream,ChunkHdr.Size&1,SEEK_CUR);
		FileHdr.Size-=(((ChunkHdr.Size+1)&~1)+8);
	}
		
		// close the file
		
	fclose(Stream);
}

/* =======================================================

      Close Wav Files

======================================================= */

void al_free_wav_file(ALenum format,ALvoid *data,ALsizei size,ALsizei freq)
{
	if (data!=NULL) free(data);
}
