/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis 'TREMOR' CODEC SOURCE CODE.   *
 *                                                                  *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis 'TREMOR' SOURCE CODE IS (C) COPYRIGHT 1994-2002    *
 * BY THE Xiph.Org FOUNDATION http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: stdio-based convenience library for opening/seeking/decoding

 ********************************************************************/

#ifndef _OV_FILE_H_
#define _OV_FILE_H_

#ifdef __cplusplus
extern "C"
{
#endif                  

#include <stdio.h>
#ifdef OPENFL_HOST
#define OV_IMPORT extern __declspec(dllimport)
#else
#define OV_IMPORT extern
#endif
#include "ivorbiscodec.h"

#define CHUNKSIZE 1024
                                                                          
                                                    
                                                                          
                                                                             
                                                                               
   
                                                                            
                                                                           
             
   
typedef struct {
  size_t (*read_func)  (void *ptr, size_t size, size_t nmemb, void *datasource);
  int    (*seek_func)  (void *datasource, ogg_int64_t offset, int whence);
  int    (*close_func) (void *datasource);
  long   (*tell_func)  (void *datasource);
} ov_callbacks;

#define  NOTOPEN   0
#define  PARTOPEN  1
#define  OPENED    2
#define  STREAMSET 3
#define  INITSET   4

typedef struct OggVorbis_File {
  void            *datasource;                                
  int              seekable;
  ogg_int64_t      offset;
  ogg_int64_t      end;
  ogg_sync_state   *oy; 

                                                                     
                      
  int              links;
  ogg_int64_t     *offsets;
  ogg_int64_t     *dataoffsets;
  ogg_uint32_t    *serialnos;
  ogg_int64_t     *pcmlengths;
  vorbis_info     *vi;
  vorbis_comment  *vc;

                                            
  ogg_int64_t      pcm_offset;
  int              ready_state;
  ogg_uint32_t     current_serialno;
  int              current_link;

  ogg_int64_t      bittrack;
  ogg_int64_t      samptrack;

  ogg_stream_state *os;                                            
                                              
  vorbis_dsp_state vd;                                                        
  vorbis_block     vb;                                                 

  ov_callbacks callbacks;

} OggVorbis_File;

OV_IMPORT int ov_clear(OggVorbis_File *vf);
OV_IMPORT int ov_open(FILE *f,OggVorbis_File *vf,char *initial,long ibytes);
OV_IMPORT int ov_open_callbacks(void *datasource, OggVorbis_File *vf,
		char *initial, long ibytes, ov_callbacks callbacks);

OV_IMPORT int ov_test(FILE *f,OggVorbis_File *vf,char *initial,long ibytes);
OV_IMPORT int ov_test_callbacks(void *datasource, OggVorbis_File *vf,
		char *initial, long ibytes, ov_callbacks callbacks);
OV_IMPORT int ov_test_open(OggVorbis_File *vf);

OV_IMPORT long ov_bitrate(OggVorbis_File *vf,int i);
OV_IMPORT long ov_bitrate_instant(OggVorbis_File *vf);
OV_IMPORT long ov_streams(OggVorbis_File *vf);
OV_IMPORT long ov_seekable(OggVorbis_File *vf);
OV_IMPORT long ov_serialnumber(OggVorbis_File *vf,int i);

OV_IMPORT ogg_int64_t ov_raw_total(OggVorbis_File *vf,int i);
OV_IMPORT ogg_int64_t ov_pcm_total(OggVorbis_File *vf,int i);
OV_IMPORT ogg_int64_t ov_time_total(OggVorbis_File *vf,int i);

OV_IMPORT int ov_raw_seek(OggVorbis_File *vf,ogg_int64_t pos);
OV_IMPORT int ov_pcm_seek(OggVorbis_File *vf,ogg_int64_t pos);
OV_IMPORT int ov_pcm_seek_page(OggVorbis_File *vf,ogg_int64_t pos);
OV_IMPORT int ov_time_seek(OggVorbis_File *vf,ogg_int64_t pos);
OV_IMPORT int ov_time_seek_page(OggVorbis_File *vf,ogg_int64_t pos);

OV_IMPORT ogg_int64_t ov_raw_tell(OggVorbis_File *vf);
OV_IMPORT ogg_int64_t ov_pcm_tell(OggVorbis_File *vf);
OV_IMPORT ogg_int64_t ov_time_tell(OggVorbis_File *vf);

OV_IMPORT vorbis_info *ov_info(OggVorbis_File *vf,int link);
OV_IMPORT vorbis_comment *ov_comment(OggVorbis_File *vf,int link);

OV_IMPORT long ov_read(OggVorbis_File *vf,char *buffer,int length,
		    int *bitstream);

#ifdef __cplusplus
}
#endif                  

#endif


