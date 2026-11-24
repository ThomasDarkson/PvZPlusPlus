#ifndef __SEXYMEMMGR_H__
#define __SEXYMEMMGR_H__

                                                                          
                            
  
                                                                  
                                                                          
                                                                        
                                                                                
                                                                                
                                                
  
                                                                          


#include <list>
#include <stdio.h>
#include <stdlib.h>

extern void SexyDumpUnfreed();

#if defined(SEXY_MEMTRACE) && !defined(RELEASEFINAL)

                                                                          
                                                   
                                                                          
void SexyMemAddTrack(void* addr,  int asize,  const char *fname, int lnum);
void SexyMemRemoveTrack(void *addr);


                                                                              
                                
inline void* __cdecl operator new(unsigned int size, const char* file, int line)
{
	void* ptr = (void*)malloc(size);
	SexyMemAddTrack(ptr, size, file, line);
	return(ptr);
}

                               
inline void* __cdecl operator new[](unsigned int size, const char* file, int line)
{
	void* ptr = (void*)malloc(size);
	SexyMemAddTrack(ptr, size, file, line);
	return(ptr);
}


                                                                               
inline void* __cdecl operator new(unsigned int size)
{
	void* ptr = (void*)malloc(size);
	return(ptr);
}

inline void* __cdecl operator new[](unsigned int size)
{
	void* ptr = (void*)malloc(size);
	return(ptr);
}


                         
inline void __cdecl operator delete(void* p)
{
	SexyMemRemoveTrack(p);
	free(p);
}

inline void __cdecl operator delete[](void* p)
{
	SexyMemRemoveTrack(p);
	free(p);
}

                                                                                     
inline void __cdecl operator delete(void* pMem, const char *file, int line)
{
	free(pMem);
}

inline void __cdecl operator delete[](void* pMem, const char *file, int line)
{
	free(pMem);
}


#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW


#endif                 


#endif