#ifndef __POPDRMCOMM_H__
#define __POPDRMCOMM_H__

#include "../../SexyAppFramework/CritSect.h"
using namespace Sexy;

class PopDRMComm
{
public:
	enum
	{
		PopCapDRM_EnableLocking,
		PopCapDRM_EnableTiming,
		PopCapDRM_QueryData,
		PopCapDRM_IPC,
		PopCapDRM_IPC_Response,
		PopCapDRM_ForceLock,
		PopCapDRM_BuyGame,
		PopCapDRM_ShowScreen,
		PopCapDRM_WaitUI,
		PopCapDRM_NumMessages
	};

public:
	HWND					mDrmGameWnd;								     
	HWND					mHWnd;										      
	HANDLE					mUNKNOWN__FileMapping;						             
	LPVOID					mUNKNOWN__MapViewPtr;						             
	bool					mIsWindow;									       
	HANDLE					mEventHwnd;									       
	HANDLE					mEventThreadHwnd;							       
	DWORD					mEventThreadID;								       
	DWORD					mCurrentThreadID;							       
	UINT					mWindowMessage[PopCapDRM_NumMessages];		       
	CRITICAL_SECTION		mCritSect;									       
	
public:
	PopDRMComm();            
	~PopDRMComm();            

	void					Start();            
	void					EnableLocking();            
	void					BuyGame();
	bool					DoIPC();            
	bool					QueryData();            
};

#endif
