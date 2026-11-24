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

 function: basic shared codebook operations

 ********************************************************************/

#ifndef _V_CODEBOOK_H_
#define _V_CODEBOOK_H_

#include "ogg.h"

                                                                      
                                          

                                                                
                                           

                                                                      
                                                                 
                                                                   
           

  

typedef struct static_codebook{
  long   dim;                                                           
  long   entries;                              
  long  *lengthlist;                                   

                                                                             
  int    maptype;                 
                                                     
                                   

                                                                   
  long     q_min;                                                              
  long     q_delta;                                                      
  int      q_quant;                                
  int      q_sequencep;              

  long     *quantlist;                                                        
                                                        
     
} static_codebook;

typedef struct codebook{
  long dim;                                                          
  long entries;                             
  long used_entries;                                  

                                                                
                             
  int           binarypoint;
  ogg_int32_t  *valuelist;                                                 
  ogg_uint32_t *codelist;                                                   

  int          *dec_index;  
  char         *dec_codelengths;
  ogg_uint32_t *dec_firsttable;
  int           dec_firsttablen;
  int           dec_maxlength;

  long     q_min;                                                              
  long     q_delta;                                                      

} codebook;

extern void vorbis_staticbook_clear(static_codebook *b);
extern void vorbis_staticbook_destroy(static_codebook *b);
extern int vorbis_book_init_decode(codebook *dest,const static_codebook *source);

extern void vorbis_book_clear(codebook *b);
extern long _book_maptype1_quantvals(const static_codebook *b);

extern int vorbis_staticbook_unpack(oggpack_buffer *b,static_codebook *c);

extern long vorbis_book_decode(codebook *book, oggpack_buffer *b);
extern long vorbis_book_decodevs_add(codebook *book, ogg_int32_t *a, 
				     oggpack_buffer *b,int n,int point);
extern long vorbis_book_decodev_set(codebook *book, ogg_int32_t *a, 
				    oggpack_buffer *b,int n,int point);
extern long vorbis_book_decodev_add(codebook *book, ogg_int32_t *a, 
				    oggpack_buffer *b,int n,int point);
extern long vorbis_book_decodevv_add(codebook *book, ogg_int32_t **a,
				     long off,int ch, 
				    oggpack_buffer *b,int n,int point);

extern int _ilog(unsigned int v);


#endif
