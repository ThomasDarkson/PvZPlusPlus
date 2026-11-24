#include "FlashWidget.h"
#include "DDImage.h"
#include "SexyAppBase.h"
#include "WidgetManager.h"
#include "Debug.h"

using namespace Sexy;

#pragma comment(lib, "comsuppw.lib")

                                                                          
                                     

#include <comdef.h>

#pragma pack(push, 8)

namespace ShockwaveFlashObjects 
{

  
                                  
  

struct __declspec(uuid("d27cdb6b-ae6d-11cf-96b8-444553540000"))
            __ShockwaveFlashObjects;
struct __declspec(uuid("d27cdb6c-ae6d-11cf-96b8-444553540000"))
                     IShockwaveFlash;
struct __declspec(uuid("d27cdb6d-ae6d-11cf-96b8-444553540000"))
                    _IShockwaveFlashEvents;
struct               ShockwaveFlash;
struct               FlashProp;
struct __declspec(uuid("d27cdb70-ae6d-11cf-96b8-444553540000"))
                IFlashFactory;
struct __declspec(uuid("d27cdb72-ae6d-11cf-96b8-444553540000"))
                IFlashObjectInterface;
struct __declspec(uuid("a6ef9860-c720-11d0-9337-00a0c90dcaa9"))
                IDispatchEx;
struct               FlashObjectInterface;

  
                                     
  

_COM_SMARTPTR_TYPEDEF(IShockwaveFlash, __uuidof(IShockwaveFlash));
_COM_SMARTPTR_TYPEDEF(_IShockwaveFlashEvents, __uuidof(_IShockwaveFlashEvents));
_COM_SMARTPTR_TYPEDEF(IFlashFactory, __uuidof(IFlashFactory));
_COM_SMARTPTR_TYPEDEF(IDispatchEx, __uuidof(IDispatchEx));
_COM_SMARTPTR_TYPEDEF(IFlashObjectInterface, __uuidof(IFlashObjectInterface));

  
                     
  

struct __declspec(uuid("d27cdb6c-ae6d-11cf-96b8-444553540000"))
IShockwaveFlash : IDispatch
{
      
                    
      

    __declspec(property(get=GetStacking,put=PutStacking))
    _bstr_t Stacking;
    __declspec(property(get=GetWMode,put=PutWMode))
    _bstr_t WMode;
    __declspec(property(get=GetMovie,put=PutMovie))
    _bstr_t Movie;
    __declspec(property(get=GetSAlign,put=PutSAlign))
    _bstr_t SAlign;
    __declspec(property(get=GetMenu,put=PutMenu))
    VARIANT_BOOL Menu;
    __declspec(property(get=GetQuality,put=PutQuality))
    int Quality;
    __declspec(property(get=GetLoop,put=PutLoop))
    VARIANT_BOOL Loop;
    __declspec(property(get=GetFrameNum,put=PutFrameNum))
    long FrameNum;
    __declspec(property(get=GetBase,put=PutBase))
    _bstr_t Base;
    __declspec(property(get=Getscale,put=Putscale))
    _bstr_t scale;
    __declspec(property(get=GetDeviceFont,put=PutDeviceFont))
    VARIANT_BOOL DeviceFont;
    __declspec(property(get=GetEmbedMovie,put=PutEmbedMovie))
    VARIANT_BOOL EmbedMovie;
    __declspec(property(get=GetBGColor,put=PutBGColor))
    _bstr_t BGColor;
    __declspec(property(get=GetQuality2,put=PutQuality2))
    _bstr_t Quality2;
    __declspec(property(get=GetReadyState))
    long ReadyState;
    __declspec(property(get=GetScaleMode,put=PutScaleMode))
    int ScaleMode;
    __declspec(property(get=GetAlignMode,put=PutAlignMode))
    int AlignMode;
    __declspec(property(get=GetBackgroundColor,put=PutBackgroundColor))
    long BackgroundColor;
    __declspec(property(get=GetTotalFrames))
    long TotalFrames;
    __declspec(property(get=GetPlaying,put=PutPlaying))
    VARIANT_BOOL Playing;
    __declspec(property(get=GetSWRemote,put=PutSWRemote))
    _bstr_t SWRemote;

      
                                        
      

      virtual HRESULT __stdcall get_ReadyState (
                         long * thestate ) = 0;
      virtual HRESULT __stdcall get_TotalFrames (
                         long * numframes ) = 0;
      virtual HRESULT __stdcall get_Playing (
                         VARIANT_BOOL * Playing ) = 0;
      virtual HRESULT __stdcall put_Playing (
                 VARIANT_BOOL Playing ) = 0;
      virtual HRESULT __stdcall get_Quality (
                         int * Quality ) = 0;
      virtual HRESULT __stdcall put_Quality (
                 int Quality ) = 0;
      virtual HRESULT __stdcall get_ScaleMode (
                         int * scale ) = 0;
      virtual HRESULT __stdcall put_ScaleMode (
                 int scale ) = 0;
      virtual HRESULT __stdcall get_AlignMode (
                         int * align ) = 0;
      virtual HRESULT __stdcall put_AlignMode (
                 int align ) = 0;
      virtual HRESULT __stdcall get_BackgroundColor (
                         long * color ) = 0;
      virtual HRESULT __stdcall put_BackgroundColor (
                 long color ) = 0;
      virtual HRESULT __stdcall get_Loop (
                         VARIANT_BOOL * Loop ) = 0;
      virtual HRESULT __stdcall put_Loop (
                 VARIANT_BOOL Loop ) = 0;
      virtual HRESULT __stdcall get_Movie (
                         BSTR * path ) = 0;
      virtual HRESULT __stdcall put_Movie (
                 BSTR path ) = 0;
      virtual HRESULT __stdcall get_FrameNum (
                         long * FrameNum ) = 0;
      virtual HRESULT __stdcall put_FrameNum (
                 long FrameNum ) = 0;
      virtual HRESULT __stdcall SetZoomRect (
                 long left,
                 long top,
                 long right,
                 long bottom ) = 0;
      virtual HRESULT __stdcall Zoom (
                 int factor ) = 0;
      virtual HRESULT __stdcall Pan (
                 long x,
                 long y,
                 int mode ) = 0;
      virtual HRESULT __stdcall Play ( ) = 0;
      virtual HRESULT __stdcall Stop ( ) = 0;
      virtual HRESULT __stdcall Back ( ) = 0;
      virtual HRESULT __stdcall Forward ( ) = 0;
      virtual HRESULT __stdcall Rewind ( ) = 0;
      virtual HRESULT __stdcall StopPlay ( ) = 0;
      virtual HRESULT __stdcall GotoFrame (
                 long FrameNum ) = 0;
      virtual HRESULT __stdcall CurrentFrame (
                         long * FrameNum ) = 0;
      virtual HRESULT __stdcall IsPlaying (
                         VARIANT_BOOL * Playing ) = 0;
      virtual HRESULT __stdcall PercentLoaded (
                         long * __MIDL_0011 ) = 0;
      virtual HRESULT __stdcall FrameLoaded (
                 long FrameNum,
                         VARIANT_BOOL * loaded ) = 0;
      virtual HRESULT __stdcall FlashVersion (
                         long * version ) = 0;
      virtual HRESULT __stdcall get_WMode (
                         BSTR * pVal ) = 0;
      virtual HRESULT __stdcall put_WMode (
                 BSTR pVal ) = 0;
      virtual HRESULT __stdcall get_SAlign (
                         BSTR * pVal ) = 0;
      virtual HRESULT __stdcall put_SAlign (
                 BSTR pVal ) = 0;
      virtual HRESULT __stdcall get_Menu (
                         VARIANT_BOOL * pVal ) = 0;
      virtual HRESULT __stdcall put_Menu (
                 VARIANT_BOOL pVal ) = 0;
      virtual HRESULT __stdcall get_Base (
                         BSTR * pVal ) = 0;
      virtual HRESULT __stdcall put_Base (
                 BSTR pVal ) = 0;
      virtual HRESULT __stdcall get_scale (
                         BSTR * pVal ) = 0;
      virtual HRESULT __stdcall put_scale (
                 BSTR pVal ) = 0;
      virtual HRESULT __stdcall get_DeviceFont (
                         VARIANT_BOOL * pVal ) = 0;
      virtual HRESULT __stdcall put_DeviceFont (
                 VARIANT_BOOL pVal ) = 0;
      virtual HRESULT __stdcall get_EmbedMovie (
                         VARIANT_BOOL * pVal ) = 0;
      virtual HRESULT __stdcall put_EmbedMovie (
                 VARIANT_BOOL pVal ) = 0;
      virtual HRESULT __stdcall get_BGColor (
                         BSTR * pVal ) = 0;
      virtual HRESULT __stdcall put_BGColor (
                 BSTR pVal ) = 0;
      virtual HRESULT __stdcall get_Quality2 (
                         BSTR * pVal ) = 0;
      virtual HRESULT __stdcall put_Quality2 (
                 BSTR pVal ) = 0;
      virtual HRESULT __stdcall LoadMovie (
                 int layer,
                 BSTR url ) = 0;
      virtual HRESULT __stdcall TGotoFrame (
                 BSTR target,
                 long FrameNum ) = 0;
      virtual HRESULT __stdcall TGotoLabel (
                 BSTR target,
                 BSTR label ) = 0;
      virtual HRESULT __stdcall TCurrentFrame (
                 BSTR target,
                         long * FrameNum ) = 0;
      virtual HRESULT __stdcall TCurrentLabel (
                 BSTR target,
                         BSTR * pVal ) = 0;
      virtual HRESULT __stdcall TPlay (
                 BSTR target ) = 0;
      virtual HRESULT __stdcall TStopPlay (
                 BSTR target ) = 0;
      virtual HRESULT __stdcall SetVariable (
                 BSTR name,
                 BSTR value ) = 0;
      virtual HRESULT __stdcall GetVariable (
                 BSTR name,
                         BSTR * pVal ) = 0;
      virtual HRESULT __stdcall TSetProperty (
                 BSTR target,
                 int property,
                 BSTR value ) = 0;
      virtual HRESULT __stdcall TGetProperty (
                 BSTR target,
                 int property,
                         BSTR * pVal ) = 0;
      virtual HRESULT __stdcall TCallFrame (
                 BSTR target,
                 int FrameNum ) = 0;
      virtual HRESULT __stdcall TCallLabel (
                 BSTR target,
                 BSTR label ) = 0;
      virtual HRESULT __stdcall TSetPropertyNum (
                 BSTR target,
                 int property,
                 double value ) = 0;
      virtual HRESULT __stdcall TGetPropertyNum (
                 BSTR target,
                 int property,
                         double * pVal ) = 0;
      virtual HRESULT __stdcall get_SWRemote (
                         BSTR * pVal ) = 0;
      virtual HRESULT __stdcall put_SWRemote (
                 BSTR pVal ) = 0;
      virtual HRESULT __stdcall get_Stacking (
                         BSTR * pVal ) = 0;
      virtual HRESULT __stdcall put_Stacking (
                 BSTR pVal ) = 0;
};

struct __declspec(uuid("d27cdb6d-ae6d-11cf-96b8-444553540000"))
_IShockwaveFlashEvents : IDispatch
{
      
                                         
      

               
    HRESULT OnReadyStateChange (
        long newState );
    HRESULT OnProgress (
        long percentDone );
    HRESULT FSCommand (
        _bstr_t command,
        _bstr_t args );
};

struct __declspec(uuid("d27cdb6e-ae6d-11cf-96b8-444553540000"))
ShockwaveFlash;
                                            
                                                               

struct __declspec(uuid("1171a62f-05d2-11d1-83fc-00a0c9089c5a"))
FlashProp;
                                     

struct __declspec(uuid("d27cdb70-ae6d-11cf-96b8-444553540000"))
IFlashFactory : IUnknown
{};

struct __declspec(uuid("a6ef9860-c720-11d0-9337-00a0c90dcaa9"))
IDispatchEx : IDispatch
{
      
                                         
      

    HRESULT GetDispID (
        _bstr_t bstrName,
        unsigned long grfdex,
        long * pid );
    HRESULT RemoteInvokeEx (
        long id,
        unsigned long lcid,
        unsigned long dwFlags,
        struct DISPPARAMS * pdp,
        VARIANT * pvarRes,
        struct EXCEPINFO * pei,
        struct IServiceProvider * pspCaller,
        unsigned int cvarRefArg,
        unsigned int * rgiRefArg,
        VARIANT * rgvarRefArg );
    HRESULT DeleteMemberByName (
        _bstr_t bstrName,
        unsigned long grfdex );
    HRESULT DeleteMemberByDispID (
        long id );
    HRESULT GetMemberProperties (
        long id,
        unsigned long grfdexFetch,
        unsigned long * pgrfdex );
    HRESULT GetMemberName (
        long id,
        BSTR * pbstrName );
    HRESULT GetNextDispID (
        unsigned long grfdex,
        long id,
        long * pid );
    HRESULT GetNameSpaceParent (
        IUnknown * * ppunk );

      
                                        
      

      virtual HRESULT __stdcall raw_GetDispID (
                 BSTR bstrName,
                 unsigned long grfdex,
                  long * pid ) = 0;
      virtual HRESULT __stdcall raw_RemoteInvokeEx (
                 long id,
                 unsigned long lcid,
                 unsigned long dwFlags,
                 struct DISPPARAMS * pdp,
                  VARIANT * pvarRes,
                  struct EXCEPINFO * pei,
                 struct IServiceProvider * pspCaller,
                 unsigned int cvarRefArg,
                 unsigned int * rgiRefArg,
                     VARIANT * rgvarRefArg ) = 0;
      virtual HRESULT __stdcall raw_DeleteMemberByName (
                 BSTR bstrName,
                 unsigned long grfdex ) = 0;
      virtual HRESULT __stdcall raw_DeleteMemberByDispID (
                 long id ) = 0;
      virtual HRESULT __stdcall raw_GetMemberProperties (
                 long id,
                 unsigned long grfdexFetch,
                  unsigned long * pgrfdex ) = 0;
      virtual HRESULT __stdcall raw_GetMemberName (
                 long id,
                  BSTR * pbstrName ) = 0;
      virtual HRESULT __stdcall raw_GetNextDispID (
                 unsigned long grfdex,
                 long id,
                  long * pid ) = 0;
      virtual HRESULT __stdcall raw_GetNameSpaceParent (
                  IUnknown * * ppunk ) = 0;
};

struct __declspec(uuid("d27cdb72-ae6d-11cf-96b8-444553540000"))
IFlashObjectInterface : IDispatchEx
{};

struct __declspec(uuid("d27cdb71-ae6d-11cf-96b8-444553540000"))
FlashObjectInterface;
                                                  

}                                   

#pragma pack(pop)

                                                                          
                                       

namespace Sexy
{

class FlashSink : public ShockwaveFlashObjects::_IShockwaveFlashEvents
{
public:	
	LPCONNECTIONPOINT		mConnectionPoint;	
	DWORD					mCookie;
	int						mRefCount;
	FlashWidget*			mFlashWidget;

public:
	FlashSink()
	{		
		mCookie = 0;
		mConnectionPoint = NULL;
		mRefCount = 0;
		mFlashWidget = NULL;		
	}

	virtual ~FlashSink()
	{
		mFlashWidget->mCOMCount--;
	}

	HRESULT Init(FlashWidget* theFlashWidget)
	{
		mFlashWidget = theFlashWidget;
		mFlashWidget->mCOMCount++;

		HRESULT aResult = NOERROR;
		LPCONNECTIONPOINTCONTAINER aConnectionPoint = NULL;
						
		if ((mFlashWidget->mFlashInterface->QueryInterface(IID_IConnectionPointContainer, (void**) &aConnectionPoint) == S_OK) &&
			(aConnectionPoint->FindConnectionPoint(__uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents), &mConnectionPoint) == S_OK))			
		{
			IDispatch* aDispatch = NULL;
			QueryInterface(__uuidof(IDispatch), (void**) &aDispatch);
			if (aDispatch != NULL)
			{
				aResult = mConnectionPoint->Advise((LPUNKNOWN)aDispatch, &mCookie);
				aDispatch->Release();
			}
		}
				
		if (aConnectionPoint != NULL) 
			aConnectionPoint->Release();

		return aResult;
	}

	HRESULT Shutdown()
	{
		HRESULT aResult = S_OK;

		if (mConnectionPoint)
		{
			if (mCookie)
			{
				aResult = mConnectionPoint->Unadvise(mCookie);
				mCookie = 0;
			}

			mConnectionPoint->Release();
			mConnectionPoint = NULL;
		}

		return aResult;
	}
 
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv)
	{
		*ppv = NULL;

		if (riid == IID_IUnknown)
		{
			*ppv = (LPUNKNOWN)this;
			AddRef();
			return S_OK;
		}
		else if (riid == IID_IDispatch)
		{
			*ppv = (IDispatch*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == __uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents))
		{
			*ppv = (ShockwaveFlashObjects::_IShockwaveFlashEvents*) this;
			AddRef();
			return S_OK;
		}
		else
		{   
			return E_NOTIMPL;
		}
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{  
		return ++mRefCount;
	}

	ULONG STDMETHODCALLTYPE Release()
	{  
		int aRefCount = --mRefCount;
		if (aRefCount == 0)		
			delete this;		

		return aRefCount;
	}

                    
  virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo)
   { return E_NOTIMPL; }

	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
	{
		return E_NOTIMPL; 
	}

	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames,
		UINT cNames, LCID lcid,DISPID* rgDispId)
	{
		return E_NOTIMPL; 
	}

	virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
		WORD wFlags, ::DISPPARAMS __RPC_FAR *pDispParams, VARIANT __RPC_FAR *pVarResult,
		::EXCEPINFO __RPC_FAR *pExcepInfo, UINT __RPC_FAR *puArgErr)
	{
		switch(dispIdMember)
		{
		case 0x7a6:			
			break;
		case 0x96:			
			if ((pDispParams->cArgs == 2) &&
				(pDispParams->rgvarg[0].vt == VT_BSTR) &&
				(pDispParams->rgvarg[1].vt == VT_BSTR))
			{
				FSCommand(pDispParams->rgvarg[1].bstrVal, pDispParams->rgvarg[0].bstrVal);
			}
			break;
		case DISPID_READYSTATECHANGE:					
			break;
		default:			
			return DISP_E_MEMBERNOTFOUND;
		} 
		
		return NOERROR;
	}

	HRESULT OnReadyStateChange (long newState)
	{	
		return S_OK;
	}
    
	HRESULT OnProgress(long percentDone )
	{		
		return S_OK;
	}

	HRESULT FSCommand (_bstr_t command, _bstr_t args)
	{
		if (mFlashWidget->mFlashListener != NULL)
			mFlashWidget->mFlashListener->FlashCommand(mFlashWidget->mId, (SexyChar*) command, (SexyChar*) args);
		return S_OK;
	}	
};

}

using namespace ShockwaveFlashObjects;

                                                                          
                                                       

namespace Sexy
{

class ControlSite : 	
	public IOleInPlaceSiteWindowless, 
	public IOleClientSite	
{
public:
	int						mRefCount;
	FlashWidget*			mFlashWidget;

public:
	ControlSite()
	{		
		mRefCount = 0;		
		mFlashWidget = NULL;
	}	

	virtual ~ControlSite()
	{
		if (mFlashWidget != NULL)
			mFlashWidget->mCOMCount--;
	}

	void Init(FlashWidget* theFlashWidget)
	{
		mFlashWidget = theFlashWidget;
		mFlashWidget->mCOMCount++;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv)
	{
		*ppv = NULL;

		if (riid == IID_IUnknown)
		{
			*ppv = (IUnknown*) (IOleWindow*) this;
			AddRef();
			return S_OK;
		}
		else if (riid == IID_IOleWindow)
		{
			*ppv = (IOleWindow*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == IID_IOleInPlaceSite)
		{
			*ppv = (IOleInPlaceSite*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == IID_IOleInPlaceSiteEx)
		{
			*ppv = (IOleInPlaceSiteEx*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == IID_IOleInPlaceSiteWindowless)
		{
			*ppv = (IOleInPlaceSiteWindowless*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == IID_IOleClientSite)
		{
			*ppv = (IOleClientSite*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == __uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents))
		{
			*ppv = (ShockwaveFlashObjects::_IShockwaveFlashEvents*) this;
			AddRef();
			return S_OK;
		}
		else
		{   
			return E_NOTIMPL;
		}
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{  
		return ++mRefCount;
	}

	ULONG STDMETHODCALLTYPE Release()
	{ 
		int aRefCount = --mRefCount;
		if (aRefCount == 0)		
			delete this;		

		return aRefCount;
	}

	                                                                           

	virtual HRESULT  STDMETHODCALLTYPE SaveObject(void)
	{
		return S_OK;
	}

	virtual HRESULT  STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,IMoniker** ppmk )
	{
		*ppmk = NULL;
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer ** theContainerP)
	{
		                                                                             
		return E_NOINTERFACE;
	}


	virtual HRESULT  STDMETHODCALLTYPE ShowObject(void)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT  STDMETHODCALLTYPE OnShowWindow(BOOL)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT  STDMETHODCALLTYPE RequestNewObjectLayout(void)
	{
		return E_NOTIMPL;
	}
		    
	

	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(           BOOL fEnterMode)
	{
	    return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetWindow(            HWND __RPC_FAR* theWnndow)
	{
		return E_FAIL;

	                        
		                                   
		              
	}

	HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void)
	{
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void)
	{		
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE OnUIActivate(void)
	{		
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE GetWindowContext(            IOleInPlaceFrame __RPC_FAR *__RPC_FAR *ppFrame,             IOleInPlaceUIWindow __RPC_FAR *__RPC_FAR *ppDoc,             LPRECT lprcPosRect,             LPRECT lprcClipRect,                 LPOLEINPLACEFRAMEINFO lpFrameInfo)
	{
		RECT aRect = mFlashWidget->GetRect().ToRECT();

		*lprcPosRect = aRect;
		*lprcClipRect = aRect;
		
		*ppFrame = NULL;
		QueryInterface(__uuidof(IOleInPlaceFrame), (void**) ppFrame);		
		*ppDoc = NULL;

		lpFrameInfo->fMDIApp = FALSE;
		lpFrameInfo->hwndFrame = NULL;
		lpFrameInfo->haccel = NULL;
		lpFrameInfo->cAccelEntries = 0;
		
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE Scroll(           SIZE scrollExtant)
	{
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE OnUIDeactivate(           BOOL fUndoable)
	{		
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void)
	{	
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE DiscardUndoState(void)
	{
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void)
	{
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE OnPosRectChange(           LPCRECT lprcPosRect)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnInPlaceActivateEx(            BOOL __RPC_FAR *pfNoRedraw,            DWORD dwFlags)
	{		
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE OnInPlaceDeactivateEx(           BOOL fNoRedraw)
	{		
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE RequestUIActivate(void)
	{
		return S_FALSE;
	}


	HRESULT STDMETHODCALLTYPE CanWindowlessActivate(void)
	{
		                               
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE GetCapture(void)
	{
		                                        
		return S_FALSE;
	}


	HRESULT STDMETHODCALLTYPE SetCapture(           BOOL fCapture)
	{
		                                        
		return S_FALSE;
	}


	HRESULT STDMETHODCALLTYPE GetFocus(void)
	{
		return S_OK;
	}


	HRESULT STDMETHODCALLTYPE SetFocus(           BOOL fFocus)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetDC(           LPCRECT pRect,            DWORD grfFlags,             HDC __RPC_FAR *phDC)
	{		
		return E_INVALIDARG;		
	}


	HRESULT STDMETHODCALLTYPE ReleaseDC(           HDC hDC)
	{
		return E_INVALIDARG;
	}


	HRESULT STDMETHODCALLTYPE InvalidateRect(           LPCRECT pRect,            BOOL fErase)
	{	
		if (pRect == NULL)
		{
			mFlashWidget->mDirtyRect = mFlashWidget->GetRect();
			mFlashWidget->mFlashDirty = true;
		}
		else if (!mFlashWidget->mFlashDirty)
		{
			mFlashWidget->mDirtyRect = Rect(pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top);
			mFlashWidget->mFlashDirty = true;
		}
		else
		{			
			mFlashWidget->mDirtyRect = mFlashWidget->mDirtyRect.Union(Rect(pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top));
		}		
		
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE InvalidateRgn(           HRGN hRGN,            BOOL fErase)
	{	
		mFlashWidget->mDirtyRect = mFlashWidget->GetRect();
		mFlashWidget->mFlashDirty = true;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ScrollRect(           INT dx,            INT dy,            LPCRECT pRectScroll,            LPCRECT pRectClip)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AdjustRect(                LPRECT prc)
	{
		if (prc == NULL)
		{
			return E_INVALIDARG;
		}
		
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnDefWindowMessage(           UINT msg,            WPARAM wParam,            LPARAM lParam,             LRESULT __RPC_FAR *plResult)
	{
		return S_FALSE;
	}
};

}

                                                                          

const CLSID CLSID_ShockwaveFlash =
  { 0xD27CDB6E, 0xAE6D, 0x11cf, {0x96, 0xB8, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00} };

typedef HRESULT (__stdcall *DllGetClassObjectFunc)(REFCLSID rclsid, REFIID riid, LPVOID * ppv); 

FlashWidget::FlashWidget(int theId, FlashListener* theFlashListener)
{
	HRESULT aResult;

	mState = STATE_IDLE;

	mCurCursor = CURSOR_POINTER;
	mCurOverrideCursor = NULL;

	mCOMCount = 0;
	mPauseCount = 0;
	mAutoPause = true;
	mHasLostFocus = !gSexyAppBase->mHasFocus;
	mFlashDirty = true;	

	mBkgColor.mAlpha = 0;
	mId = theId;
	mFlashListener = theFlashListener;	

	mFlashSink = NULL;
	
	mImage = NULL;	
	mFlashInterface = NULL;
	mOleObject = NULL;
	mWindowlessObject = NULL;

	mBkgImage = NULL;
	mFlashLibHandle = NULL;

	CoInitialize(NULL);

	mControlSite = new ControlSite();
	mControlSite->AddRef();	
	mControlSite->Init(this);
		
	mFlashLibHandle = LoadLibraryA("flash.ocx");
	if (mFlashLibHandle != NULL)
	{
		IClassFactory* aClassFactory = NULL;
		DllGetClassObjectFunc aDllGetClassObjectFunc = (DllGetClassObjectFunc) GetProcAddress(mFlashLibHandle, "DllGetClassObject");
		aResult = aDllGetClassObjectFunc(CLSID_ShockwaveFlash, IID_IClassFactory, (void**)&aClassFactory);
		aClassFactory->CreateInstance(NULL, IID_IOleObject, (void**)&mOleObject);
		aClassFactory->Release();	
	}
	else
	{
		CoCreateInstance(CLSID_ShockwaveFlash, NULL, 
			CLSCTX_INPROC_SERVER,
			IID_IOleObject,
			(void**)&mOleObject);
	}

	IOleClientSite* aClientSite = NULL;
	mControlSite->QueryInterface(__uuidof(IOleClientSite), (void**) &aClientSite);
	mOleObject->SetClientSite(aClientSite);	

	mOleObject->QueryInterface(__uuidof(IShockwaveFlash), (LPVOID*) &mFlashInterface);
	_bstr_t aTrans = "Transparent";
	mFlashInterface->put_WMode(aTrans);

	aResult = mOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, aClientSite, 0, NULL, NULL);
	aClientSite->Release();	
		
	mOleObject->QueryInterface(__uuidof(IOleInPlaceObjectWindowless), (LPVOID*) &mWindowlessObject);
			
	mFlashSink = new FlashSink();
	mFlashSink->AddRef();	
	mFlashSink->Init(this);
}

FlashWidget::~FlashWidget()
{
	CleanupImages();
		
	if (mWindowlessObject != NULL)
		mWindowlessObject->Release();
	if (mFlashInterface != NULL)
		mFlashInterface->Release();	

	if (mFlashSink != NULL)
	{
		mFlashSink->Shutdown();
		mFlashSink->Release();
	}

	mOleObject->Close(OLECLOSE_NOSAVE);
		
	if (mOleObject != NULL)
		mOleObject->Release();	

	if (mControlSite != NULL)
		mControlSite->Release();

	                                                        
	DBG_ASSERTE(mCOMCount == 0);

	if (mFlashLibHandle != NULL)
		FreeLibrary(mFlashLibHandle);	
}

double FlashWidget::GetFlashVersion()
{
	CoInitialize(NULL);

	IOleObject* anOleObject = NULL;
	if (FAILED(CoCreateInstance(CLSID_ShockwaveFlash, NULL, 
		CLSCTX_INPROC_SERVER,
		IID_IOleObject,
		(void**) &anOleObject)))
		return 0.0;	

	IShockwaveFlash* aFlashInterface = NULL;
	if (FAILED(anOleObject->QueryInterface(__uuidof(IShockwaveFlash), (LPVOID*) &aFlashInterface)))
		return 0.0;
	
	long aVersion = 0;
	aFlashInterface->FlashVersion(&aVersion);

	aFlashInterface->Release();
	anOleObject->Release();

	return aVersion / 65536.0;
}

void FlashWidget::CleanupImages()
{
	if (mImage != NULL)
	{
		delete mImage;
		mImage = NULL;
	}
}

void FlashWidget::RebuildImages()
{
	CleanupImages();

	mImage = new DDImage(gSexyAppBase->mDDInterface);
	mImage->Create(mWidth, mHeight);
	mImage->SetImageMode(false, false);	
}

void FlashWidget::CheckCursor()
{
	HCURSOR aCursor = GetCursor();
	if (aCursor == ::LoadCursor(NULL, IDC_ARROW))
	{
		mCurCursor = CURSOR_POINTER;
		gSexyAppBase->mOverrideCursor = NULL;
		gSexyAppBase->SetCursor(mCurCursor);
	}
	else if (aCursor != NULL)
	{
		gSexyAppBase->mOverrideCursor = aCursor;
		mCurCursor = CURSOR_HAND;
		gSexyAppBase->SetCursor(mCurCursor);
	}
}

void FlashWidget::DrawFlashBackground(Graphics* g)
{
	if (mBkgColor.mAlpha != 0)
	{
		g->SetColor(mBkgColor);
		g->FillRect(0, 0, mWidth, mHeight);
	}

	if (mBkgImage != NULL)
	{
		if ((mBkgImageSrcRect.mWidth != 0) && (mBkgImageSrcRect.mHeight != 0))
			g->DrawImage(mBkgImage, 0, 0, mBkgImageSrcRect);
		else
			g->DrawImage(mBkgImage, 0, 0);
	}
}

bool FlashWidget::StartAnimation(const SexyString& theFileName)
{
	SexyString aFullPath = GetFullPath(theFileName);
	
	_bstr_t bstr((char*) aFullPath.c_str());		
	mFlashInterface->put_Movie(bstr);                                     
	if ((mPauseCount == 0) && (mFlashInterface->Play() != S_OK))
		return false;	

	if ((mWidgetManager != NULL) && (mIsOver))
		MouseMove(mWidgetManager->mLastMouseX - mX, mWidgetManager->mLastMouseY - mY);

	mState = STATE_PLAYING;	
	return true;
}

void FlashWidget::SetQuality(int theQuality)
{
	static const char* aQualityNames[3] = {"Low", "Medium", "High"};
	
	_bstr_t aNewStr = aQualityNames[theQuality];
	mFlashInterface->put_Quality2(aNewStr);
}

void FlashWidget::Pause()
{
	mPauseCount++;

	if (mState != STATE_STOPPED)
		mState = STATE_IDLE;

	if ((mPauseCount == 1) && (mFlashInterface != NULL) && (mState != STATE_STOPPED))
		mFlashInterface->StopPlay();
}

void FlashWidget::Unpause()
{
	mPauseCount--;
	if ((mPauseCount == 0) && (mFlashInterface != NULL) && (mState != STATE_STOPPED))
	{
		mState = STATE_PLAYING;
		mFlashInterface->Play();
	}
}

void FlashWidget::Rewind()
{
	if (mFlashInterface != NULL)
	{
		mFlashInterface->Rewind();
		mFlashInterface->Play();
	}
}

void FlashWidget::GotoFrame(int theFrameNum)
{
	if (mFlashInterface != NULL)
	{
		mFlashInterface->GotoFrame(theFrameNum);
		mFlashInterface->Play();
	}
}

void FlashWidget::Back()
{
	if (mFlashInterface != NULL)
	{
		mFlashInterface->Back();
		mFlashInterface->Play();
	}
}

void FlashWidget::Forward()
{
	if (mFlashInterface != NULL)
	{
		mFlashInterface->Forward();
		mFlashInterface->Play();
	}
}

bool FlashWidget::IsPlaying()
{
	VARIANT_BOOL aBool = 0;
	if (mFlashInterface != NULL)
		mFlashInterface->IsPlaying(&aBool);
	return aBool != 0;
}

int FlashWidget::GetCurrentFrame()
{	
	long aCurrentFrame = -1;
	if (mFlashInterface != NULL)
		mFlashInterface->CurrentFrame(&aCurrentFrame);
	return aCurrentFrame;
}

SexyString FlashWidget::GetCurrentLabel(const SexyString& theTimeline)
{
	BSTR aBStr = nullptr;
	if (mFlashInterface != NULL)
		mFlashInterface->TCurrentLabel(_bstr_t(theTimeline.c_str()), &aBStr);
	return (const SexyChar*) _bstr_t(aBStr);
}

void FlashWidget::CallFrame(const SexyString& theTimeline, int theFrameNum)
{
	if (mFlashInterface != NULL)
		mFlashInterface->TCallFrame(_bstr_t(theTimeline.c_str()), theFrameNum);
}

void FlashWidget::CallLabel(const SexyString& theTimeline, const SexyString& theLabel)
{
	if (mFlashInterface != NULL)
		mFlashInterface->TCallLabel(_bstr_t(theTimeline.c_str()), _bstr_t(theLabel.c_str()));
}

SexyString FlashWidget::GetVariable(const SexyString& theName)
{
	BSTR aBStr = nullptr;
	if (mFlashInterface != NULL)
		mFlashInterface->GetVariable(_bstr_t(theName.c_str()), &aBStr);
	return (const SexyChar*) _bstr_t(aBStr);
}

void FlashWidget::SetVariable(const SexyString& theName, const SexyString& theValue)
{
	if (mFlashInterface != NULL)
		mFlashInterface->SetVariable(_bstr_t(theName.c_str()), _bstr_t(theValue.c_str()));
}

void FlashWidget::Update()
{
	if (mAutoPause)
	{
		if (!gSexyAppBase->mHasFocus)
		{
			if (!mHasLostFocus)
			{
				if (mAutoPause)
					Pause();
				mHasLostFocus = true;
			}

			return;
		}
		else
		{
			if (mHasLostFocus)
			{
				if (mAutoPause)
					Unpause();
				mHasLostFocus = false;
			}
		}
	}

	if (mState == STATE_PLAYING)
	{
		VARIANT_BOOL isPlaying = 0;
		mFlashInterface->IsPlaying(&isPlaying);	
		if (!isPlaying)
		{
			mState = STATE_STOPPED;
			if ((mFlashListener != NULL) && (mPauseCount == 0))
				mFlashListener->FlashAnimEnded(mId);
		}
	}

	if (mFlashDirty)
		MarkDirty();
}

void FlashWidget::Resize(int theX, int theY, int theWidth, int theHeight)
{
	Widget::Resize(theX, theY, theWidth, theHeight);

	mDirtyRect = Rect(theX, theY, theWidth, theHeight);	

	RebuildImages();

	IOleInPlaceObject* anInPlaceObject = NULL;	
	mOleObject->QueryInterface(__uuidof(IOleInPlaceObject), (LPVOID*) &anInPlaceObject);			

	if (anInPlaceObject != NULL)
	{
		RECT aRect = GetRect().ToRECT();
		anInPlaceObject->SetObjectRects(&aRect, &aRect);
		anInPlaceObject->Release();
	}
}

void FlashWidget::Draw(Graphics* g)
{	
	if (mFlashDirty)
	{
		Graphics anImageG(mImage);	
		DrawFlashBackground(&anImageG);	

		LPDIRECTDRAWSURFACE aSurface = mImage->GetSurface();
		if (aSurface == NULL)
			return;

		HDC aDC = NULL;
		if (aSurface->GetDC(&aDC) != S_OK)
			return;	

		IViewObject* aViewObject = NULL;
		mFlashInterface->QueryInterface(IID_IViewObject, (LPVOID*) &aViewObject);
		if (aViewObject != NULL)
		{
			RECTL aRect = {0, 0, mWidth, mHeight};

			Point anAbsPos = GetAbsPos();

			HRGN aRgn = CreateRectRgn(mDirtyRect.mX - anAbsPos.mX, mDirtyRect.mY - anAbsPos.mY, 
				mDirtyRect.mX + mDirtyRect.mWidth - anAbsPos.mX, 
				mDirtyRect.mY + mDirtyRect.mHeight - anAbsPos.mY);
			SelectClipRgn(aDC, aRgn);
			DeleteObject(aRgn);

			aViewObject->Draw(DVASPECT_CONTENT, 1,
				NULL, NULL, NULL, aDC, &aRect, NULL, NULL,
				0);

			aViewObject->Release();
		}

		aSurface->ReleaseDC(aDC);

		mFlashDirty = false;
	}
	
	g->DrawImage(mImage, 0, 0);
}

void FlashWidget::MouseMove(int x, int y)
{	
	Widget::MouseMove(x, y);

	LRESULT aResult;
	Point anAbsPos = GetAbsPos();
	mWindowlessObject->OnWindowMessage(WM_MOUSEMOVE, 0, MAKELPARAM(x + anAbsPos.mX, y + anAbsPos.mY), &aResult);

	if (mIsOver)
		CheckCursor();
}

void FlashWidget::MouseDown(int x, int y, int theBtnNum, int theClickCount)
{
	if (theBtnNum == 0)
	{
		LRESULT aResult;
		Point anAbsPos = GetAbsPos();
		mWindowlessObject->OnWindowMessage(WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x + anAbsPos.mX, y + anAbsPos.mY), &aResult);
		CheckCursor();
	}
}

void FlashWidget::MouseUp(int x, int y, int theBtnNum, int theClickCount)
{
	if (theBtnNum == 0)
	{
		LRESULT aResult;
		Point anAbsPos = GetAbsPos();
		mWindowlessObject->OnWindowMessage(WM_LBUTTONUP, 0, MAKELPARAM(x + anAbsPos.mX, y + anAbsPos.mY), &aResult);
		CheckCursor();
	}
}

void FlashWidget::MouseDrag(int x, int y)
{
	if (mWidgetManager->IsLeftButtonDown())
	{
		LRESULT aResult;
		Point anAbsPos = GetAbsPos();
		mWindowlessObject->OnWindowMessage(WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(x + anAbsPos.mX, y + anAbsPos.mY), &aResult);
		CheckCursor();
	}
}

void FlashWidget::MouseLeave()
{
	Widget::MouseLeave();

	                                                                 
	                                    
	MouseMove(-1, -1);

	gSexyAppBase->mOverrideCursor = NULL;
	gSexyAppBase->SetCursor(CURSOR_POINTER);	
}

void FlashWidget::SysColorChanged()
{
	mFlashDirty = true;
	mDirtyRect = GetRect();

	MarkDirty();
}
