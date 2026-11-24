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

 function: basic codebook pack/unpack/code/decode operations

 ********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ogg.h"
#include "ivorbiscodec.h"
#include "codebook.h"
#include "misc.h"
#include "os.h"

                                                                      
                                                                      
int vorbis_staticbook_unpack(oggpack_buffer *opb,static_codebook *s){
  long i,j;
  memset(s,0,sizeof(*s));

                                      
  if(oggpack_read(opb,24)!=0x564342)goto _eofout;

                                  
  s->dim=oggpack_read(opb,16);
  s->entries=oggpack_read(opb,24);
  if(s->entries==-1)goto _eofout;

                                                          
  switch((int)oggpack_read(opb,1)){
  case 0:
                   
    s->lengthlist=(long *)_ogg_malloc(sizeof(*s->lengthlist)*s->entries);

                                       
    if(oggpack_read(opb,1)){
                               

      for(i=0;i<s->entries;i++){
	if(oggpack_read(opb,1)){
	  long num=oggpack_read(opb,5);
	  if(num==-1)goto _eofout;
	  s->lengthlist[i]=num+1;
	}else
	  s->lengthlist[i]=0;
      }
    }else{
                                        
      for(i=0;i<s->entries;i++){
	long num=oggpack_read(opb,5);
	if(num==-1)goto _eofout;
	s->lengthlist[i]=num+1;
      }
    }
    
    break;
  case 1:
                 
    {
      long length=oggpack_read(opb,5)+1;
      s->lengthlist=(long *)_ogg_malloc(sizeof(*s->lengthlist)*s->entries);

      for(i=0;i<s->entries;){
	long num=oggpack_read(opb,_ilog(s->entries-i));
	if(num==-1)goto _eofout;
	for(j=0;j<num && i<s->entries;j++,i++)
	  s->lengthlist[i]=length;
	length++;
      }
    }
    break;
  default:
             
    return(-1);
  }
  
                                       
  switch((s->maptype=oggpack_read(opb,4))){
  case 0:
                    
    break;
  case 1: case 2:
                                            
                                            

    s->q_min=oggpack_read(opb,32);
    s->q_delta=oggpack_read(opb,32);
    s->q_quant=oggpack_read(opb,4)+1;
    s->q_sequencep=oggpack_read(opb,1);

    {
      int quantvals=0;
      switch(s->maptype){
      case 1:
	quantvals=_book_maptype1_quantvals(s);
	break;
      case 2:
	quantvals=s->entries*s->dim;
	break;
      }
      
                            
      s->quantlist=(long *)_ogg_malloc(sizeof(*s->quantlist)*quantvals);
      for(i=0;i<quantvals;i++)
	s->quantlist[i]=oggpack_read(opb,s->q_quant);
      
      if(quantvals&&s->quantlist[quantvals-1]==-1)goto _eofout;
    }
    break;
  default:
    goto _errout;
  }

               
  return(0);
  
 _errout:
 _eofout:
  vorbis_staticbook_clear(s);
  return(-1); 
}

                                                                     
                                                                      
                                                                  
                                                                    
                                                                    
                                                               
                                                   

static ogg_uint32_t bitreverse(ogg_uint32_t x){
  x=    ((x>>16)&0x0000ffff) | ((x<<16)&0xffff0000);
  x=    ((x>> 8)&0x00ff00ff) | ((x<< 8)&0xff00ff00);
  x=    ((x>> 4)&0x0f0f0f0f) | ((x<< 4)&0xf0f0f0f0);
  x=    ((x>> 2)&0x33333333) | ((x<< 2)&0xcccccccc);
  return((x>> 1)&0x55555555) | ((x<< 1)&0xaaaaaaaa);
}

STIN long decode_packed_entry_number(codebook *book, 
					      oggpack_buffer *b){
  int  read=book->dec_maxlength;
  long lo,hi;
  long lok = oggpack_look(b,book->dec_firsttablen);
 
  if (lok >= 0) {
    long entry = book->dec_firsttable[lok];
    if(entry&0x80000000UL){
      lo=(entry>>15)&0x7fff;
      hi=book->used_entries-(entry&0x7fff);
    }else{
      oggpack_adv(b, book->dec_codelengths[entry-1]);
      return(entry-1);
    }
  }else{
    lo=0;
    hi=book->used_entries;
  }

  lok = oggpack_look(b, read);

  while(lok<0 && read>1)
    lok = oggpack_look(b, --read);
  if(lok<0)return -1;

                                                          
  {
    ogg_uint32_t testword=bitreverse((ogg_uint32_t)lok);

    while(hi-lo>1){
      long p=(hi-lo)>>1;
      long test=book->codelist[lo+p]>testword;    
      lo+=p&(test-1);
      hi-=p&(-test);
    }

    if(book->dec_codelengths[lo]<=read){
      oggpack_adv(b, book->dec_codelengths[lo]);
      return(lo);
    }
  }
  
  oggpack_adv(b, read);
  return(-1);
}

                                                                   
                                                                       
                                        
   
                                                                
                                                                 

                                                                      
                                                              
                                              
                                           
                        
                                 

                                                                             
long vorbis_book_decode(codebook *book, oggpack_buffer *b){
  long packed_entry=decode_packed_entry_number(book,b);
  if(packed_entry>=0)
    return(book->dec_index[packed_entry]);
  
                                                                       
  return(packed_entry);
}

                                                                      
long vorbis_book_decodevs_add(codebook *book,ogg_int32_t *a,
			      oggpack_buffer *b,int n,int point){
  int step=n/book->dim;
  long *entry = (long *)alloca(sizeof(*entry)*step);
  ogg_int32_t **t = (ogg_int32_t **)alloca(sizeof(*t)*step);
  int i,j,o;
  int shift=point-book->binarypoint;

  if(shift>=0){
    for (i = 0; i < step; i++) {
      entry[i]=decode_packed_entry_number(book,b);
      if(entry[i]==-1)return(-1);
      t[i] = book->valuelist+entry[i]*book->dim;
    }
    for(i=0,o=0;i<book->dim;i++,o+=step)
      for (j=0;j<step;j++)
	a[o+j]+=t[j][i]>>shift;
  }else{
    for (i = 0; i < step; i++) {
      entry[i]=decode_packed_entry_number(book,b);
      if(entry[i]==-1)return(-1);
      t[i] = book->valuelist+entry[i]*book->dim;
    }
    for(i=0,o=0;i<book->dim;i++,o+=step)
      for (j=0;j<step;j++)
	a[o+j]+=t[j][i]<<-shift;
  }
  return(0);
}

long vorbis_book_decodev_add(codebook *book,ogg_int32_t *a,
			     oggpack_buffer *b,int n,int point){
  int i,j,entry;
  ogg_int32_t *t;
  int shift=point-book->binarypoint;
  
  if(shift>=0){
    for(i=0;i<n;){
      entry = decode_packed_entry_number(book,b);
      if(entry==-1)return(-1);
      t     = book->valuelist+entry*book->dim;
      for (j=0;j<book->dim;)
	a[i++]+=t[j++]>>shift;
    }
  }else{
    for(i=0;i<n;){
      entry = decode_packed_entry_number(book,b);
      if(entry==-1)return(-1);
      t     = book->valuelist+entry*book->dim;
      for (j=0;j<book->dim;)
	a[i++]+=t[j++]<<-shift;
    }
  }
  return(0);
}

long vorbis_book_decodev_set(codebook *book,ogg_int32_t *a,
			     oggpack_buffer *b,int n,int point){
  int i,j,entry;
  ogg_int32_t *t;
  int shift=point-book->binarypoint;
  
  if(shift>=0){

    for(i=0;i<n;){
      entry = decode_packed_entry_number(book,b);
      if(entry==-1)return(-1);
      t     = book->valuelist+entry*book->dim;
      for (j=0;j<book->dim;){
	a[i++]=t[j++]>>shift;
      }
    }
  }else{

    for(i=0;i<n;){
      entry = decode_packed_entry_number(book,b);
      if(entry==-1)return(-1);
      t     = book->valuelist+entry*book->dim;
      for (j=0;j<book->dim;){
	a[i++]=t[j++]<<-shift;
      }
    }
  }
  return(0);
}

long vorbis_book_decodevv_add(codebook *book,ogg_int32_t **a,\
			      long offset,int ch,
			      oggpack_buffer *b,int n,int point){
  long i,j,entry;
  int chptr=0;
  int shift=point-book->binarypoint;
  
  if(shift>=0){
    
    for(i=offset;i<offset+n;){
      entry = decode_packed_entry_number(book,b);
      if(entry==-1)return(-1);
      {
	const ogg_int32_t *t = book->valuelist+entry*book->dim;
	for (j=0;j<book->dim;j++){
	  a[chptr++][i]+=t[j]>>shift;
	  if(chptr==ch){
	    chptr=0;
	    i++;
	  }
	}
      }
    }
  }else{

    for(i=offset;i<offset+n;){
      entry = decode_packed_entry_number(book,b);
      if(entry==-1)return(-1);
      {
	const ogg_int32_t *t = book->valuelist+entry*book->dim;
	for (j=0;j<book->dim;j++){
	  a[chptr++][i]+=t[j]<<-shift;
	  if(chptr==ch){
	    chptr=0;
	    i++;
	  }
	}
      }
    }
  }
  return(0);
}
