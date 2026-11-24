#include "BassLoader.h"
#include <stdlib.h>

using namespace Sexy;

BASS_INSTANCE* Sexy::gBass = NULL;
static long gBassLoadCount = 0;

                                                                               
                                                                               
static void CheckBassFunction(unsigned int theFunc, const char* theName)
{
	if (theFunc == 0)
	{
		char aBuf[1024];
		sprintf(aBuf, "%s function not found in bass.dll", theName);
		MessageBoxA(NULL, aBuf, "Error", MB_OK | MB_ICONERROR);
		exit(0);
	}
}

                                                                               
                                                                               
BASS_INSTANCE::BASS_INSTANCE(const char* dllName)
{
	mModule = LoadLibrary(dllName);
	if (!mModule)
		return;

#define GETPROC(_x) CheckBassFunction(*((uintptr_t *)&_x) = (uintptr_t)GetProcAddress(mModule, #_x),#_x)    

	GETPROC(BASS_Init);
	GETPROC(BASS_Free);
	GETPROC(BASS_Stop);
	GETPROC(BASS_Start);

	                                                                                                        
	*((uintptr_t*)&BASS_SetVolume) = (uintptr_t)GetProcAddress(mModule, "BASS_SetVolume");

	if (BASS_SetVolume == NULL                                       )
	{
		MessageBoxA(NULL, "Whoops! You forgot to put the CD in your computer.", "Error", MB_OK | MB_ICONERROR);
		exit(0);
	}

	                                                                                          
	                                                                                          
	GETPROC(BASS_SetConfig);
	GETPROC(BASS_GetConfig);

	GETPROC(BASS_GetVolume);
	GETPROC(BASS_GetInfo);

	GETPROC(BASS_GetVersion);
	GETPROC(BASS_ChannelStop);
	GETPROC(BASS_ChannelPlay);
	GETPROC(BASS_ChannelPause);
	GETPROC(BASS_ChannelSetAttribute);
	GETPROC(BASS_ChannelGetAttribute);
	GETPROC(BASS_ChannelSetPosition);
	GETPROC(BASS_ChannelGetPosition);
	GETPROC(BASS_ChannelIsActive);
	GETPROC(BASS_ChannelFlags);
	GETPROC(BASS_ChannelSlideAttribute);
	GETPROC(BASS_ChannelIsSliding);
	GETPROC(BASS_ChannelGetLevel);
	GETPROC(BASS_ChannelSetSync);
	GETPROC(BASS_ChannelRemoveSync);
	GETPROC(BASS_ChannelGetData);

	                                                                                    
	GETPROC(BASS_FXSetParameters);
	GETPROC(BASS_FXGetParameters);
	GETPROC(BASS_ChannelSetFX);
	GETPROC(BASS_ChannelRemoveFX);

	GETPROC(BASS_MusicLoad);
	GETPROC(BASS_MusicFree);
	                                  
	                                  

	GETPROC(BASS_StreamCreateFile);
	GETPROC(BASS_StreamFree);

	                               
	                                      

	GETPROC(BASS_SampleLoad);
	GETPROC(BASS_SampleFree);
	GETPROC(BASS_SampleSetInfo);
	GETPROC(BASS_SampleGetInfo);
	GETPROC(BASS_SampleGetChannel);
	GETPROC(BASS_SampleStop);

	GETPROC(BASS_ErrorGetCode);

	GETPROC(BASS_PluginLoad);
	GETPROC(BASS_ChannelGetLength);

	                                                     
	                                                                                            
	                                                                                                        

	  
                                    
               
  
                                             
                                                      
                   

                                                                
                        



                        
                                                                                                  
  
     
  
                         
                            
  
   

#undef GETPROC
}

                                                                               
                                                                               
BASS_INSTANCE::~BASS_INSTANCE()
{
	if (mModule)
		FreeLibrary(mModule);
}


BOOL BASS_INSTANCE::BASS_MusicSetAmplify(HMUSIC handle, DWORD amp)
{
	BASS_ChannelSetAttribute(handle, BASS_ATTRIB_MUSIC_AMPLIFY, amp);
	return true;
}


BOOL BASS_INSTANCE::BASS_MusicPlay(HMUSIC handle)
{
	return BASS_ChannelPlay(handle, true);
}


BOOL BASS_INSTANCE::BASS_MusicPlayEx(HMUSIC handle, DWORD pos, int flags, BOOL reset)
{
	(void)reset;
	                                     

	BASS_ChannelStop(handle);
	BASS_ChannelSetPosition(handle, MAKELONG(pos, 0), BASS_POS_MUSIC_ORDER);
	BASS_ChannelFlags(handle, flags, -1);

	return BASS_ChannelPlay(handle, false         );                                                    
}


BOOL BASS_INSTANCE::BASS_ChannelResume(DWORD handle)
{
	return BASS_ChannelPlay(handle, false);
}

BOOL BASS_INSTANCE::BASS_StreamPlay(HSTREAM handle, BOOL flush, DWORD flags)
{
	BASS_ChannelFlags(handle, flags, -1);
	return BASS_ChannelPlay(handle, flush);
}


                                                                               
                                                                               
void Sexy::LoadBassDLL()
{
	InterlockedIncrement(&gBassLoadCount);
	if (gBass != NULL)
		return;

	gBass = new BASS_INSTANCE("bass.dll");
	if (gBass->mModule == NULL)
	{
		MessageBoxA(NULL, "Can't find bass.dll.", "Error", MB_OK | MB_ICONERROR);
		exit(0);
	}
}

                                                                               
                                                                               
void Sexy::FreeBassDLL()
{
	if (gBass != NULL)
	{
		if (InterlockedDecrement(&gBassLoadCount) <= 0)
		{
			delete gBass;
			gBass = NULL;
		}
	}
}

