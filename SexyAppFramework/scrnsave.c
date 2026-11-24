#pragma warning(disable:4312)
#pragma warning(disable:4244)

  
                                                                      

                                                         
                       

                                 

   
#include <windows.h>
#include <scrnsave.h>
#include <regstr.h>

                               
#define CLASS_SCRNSAVE TEXT("WindowsScreenSaverClass")

             
HWND            hMainWindow = NULL;
BOOL            fChildPreview = FALSE;
HINSTANCE       hMainInstance;
TCHAR           szName[TITLEBARNAMELEN];
TCHAR           szAppName[APPNAMEBUFFERLEN];
TCHAR           szIniFile[MAXFILELEN];
TCHAR           szScreenSaver[22];
TCHAR           szHelpFile[MAXFILELEN];
TCHAR           szNoHelpMemory[BUFFLEN];
UINT            MyHelpMessage;

                         
static HINSTANCE hPwdLib = NULL;
static POINT pt_orig;
static BOOL checking_pwd = FALSE;
static BOOL closing = FALSE;
static BOOL w95 = FALSE;

typedef BOOL (WINAPI *VERIFYPWDPROC)(HWND);
typedef DWORD (WINAPI *CHPWDPROC)(LPCTSTR, HWND, DWORD, PVOID);
static VERIFYPWDPROC VerifyScreenSavePwd = NULL;

                    
#define szVerifyPassword "VerifyScreenSavePwd"

#ifdef UNICODE
#define szPwdChangePassword "PwdChangePasswordW"
#else
#define szPwdChangePassword "PwdChangePasswordA"
#endif

static void TerminateScreenSaver(HWND hWnd);
static BOOL RegisterClasses(void);
static LRESULT WINAPI SysScreenSaverProc(HWND,UINT,WPARAM,LPARAM);
static int LaunchScreenSaver(HWND hParent);
static void LaunchConfig(void);

static int ISSPACE(char c)
{
  return (c == ' ' || c == '\t');
}

#define ISNUM(c) ((c) >= '0' && c <= '9')
static unsigned long
_toul(const char *s)
{
  unsigned long res;
  unsigned long n;
  const char *p;
  for (p = s; *p; p++)
    if (!ISNUM(*p)) break;
  p--;
  res = 0;
  for (n = 1; p >= s; p--, n *= 10)
    res += (*p - '0') * n;
  return res;
}

int APIENTRY ScreenSaverWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR CmdLine, int nCmdShow)
{
  LPSTR p;
  OSVERSIONINFO vi;

                  
  hMainInstance = hInst;

  vi.dwOSVersionInfoSize = sizeof(vi);
  GetVersionEx(&vi);
                                                    
  if (vi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
    {
      HKEY hKey;
                                   
      w95 = TRUE;
      if (RegOpenKey(HKEY_CURRENT_USER, REGSTR_PATH_SCREENSAVE ,&hKey) ==
          ERROR_SUCCESS)
        {
          DWORD check_pwd;
          DWORD size = sizeof(DWORD);
          DWORD type;
          LONG res;
          res = RegQueryValueEx(hKey, REGSTR_VALUE_USESCRPASSWORD,
                                NULL, &type, (PBYTE) &check_pwd, &size);
          if (check_pwd && res == ERROR_SUCCESS)
            {
              hPwdLib = LoadLibrary(TEXT("PASSWORD.CPL"));
              if (hPwdLib)
                VerifyScreenSavePwd = (VERIFYPWDPROC)GetProcAddress(hPwdLib, szVerifyPassword);
            }
          RegCloseKey(hKey);
        }
    }

                       
  for (p = CmdLine; *p; p++)
    {
      switch (*p)
        {
        case 'S':
        case 's':
                                  
          return LaunchScreenSaver(NULL);
          
        case 'P':
        case 'p':
          {
                                                      
            HWND hParent;
            fChildPreview = TRUE;
            while (ISSPACE(*++p));
            hParent = (HWND) _toul(p);
            if (hParent && IsWindow(hParent))
              return LaunchScreenSaver(hParent);
          }
          return 0;

        case 'C':
        case 'c':
                                        
          LaunchConfig();
          return 0;

        case 'A':
        case 'a':
          {
                                              
            HWND hParent;
            while (ISSPACE(*++p));
            hParent = (HWND) _toul(p);
            if (!hParent || !IsWindow(hParent))
              hParent = GetForegroundWindow();
            ScreenSaverChangePassword(hParent);
          }
          return 0;

        case '-':
        case '/':
        case ' ':
        default: 
          break;
        }
    }
  LaunchConfig();
  return 0;
}


static void LaunchConfig(void)
{
                                    
  RegisterDialogClasses(hMainInstance);
                                
  DialogBox(hMainInstance, MAKEINTRESOURCE(DLG_SCRNSAVECONFIGURE),
            GetForegroundWindow(), (DLGPROC) ScreenSaverConfigureDialog);
}


static int LaunchScreenSaver(HWND hParent)
{
  BOOL foo;
  UINT style;
  RECT rc;
  MSG msg;

                                                          
  if (w95 && !fChildPreview)
    SystemParametersInfo(SPI_SCREENSAVERRUNNING, TRUE, &foo, 0);

  msg.wParam = 0;

                                                                         
               
  if (!RegisterClasses())
    {
      MessageBox(NULL, TEXT("RegisterClasses() failed"), NULL, MB_ICONHAND);
      goto restore;
    }

                                                                   
                           
  if (hParent)
    {
      style = WS_CHILD;
      GetClientRect(hParent, &rc);
    }
  else
    {
      style = WS_POPUP;
      rc.right = GetSystemMetrics(SM_CXSCREEN);
      rc.bottom = GetSystemMetrics(SM_CYSCREEN);
      style |= WS_VISIBLE;
    }

                                       
  hMainWindow = CreateWindowEx(hParent ? 0 : WS_EX_TOPMOST, CLASS_SCRNSAVE,
                               TEXT("SCREENSAVER"), style,
                               0, 0, rc.right, rc.bottom, hParent, NULL,
                               hMainInstance, NULL);

                                                 
  if (hMainWindow)
    {
      UpdateWindow(hMainWindow);
      ShowWindow(hMainWindow, SW_SHOW);

      while (GetMessage(&msg, NULL, 0, 0) == TRUE)
        {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
    }

restore:
                      
  if (w95 && !fChildPreview)
    SystemParametersInfo(SPI_SCREENSAVERRUNNING, FALSE, &foo, 0);
  FreeLibrary(hPwdLib);
  return msg.wParam;
}

                                                                
                  
static LRESULT WINAPI SysScreenSaverProc(HWND hWnd, UINT msg,
                                  WPARAM wParam, LPARAM lParam)
{
  switch (msg)
    {
    case WM_CREATE:
      if (!fChildPreview)
        SetCursor(NULL);
                                                            
      GetCursorPos(&pt_orig);
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    case WM_TIMER:
      if (closing)
        return 0;
      break;
    case WM_PAINT:
      if (closing)
        return DefWindowProc(hWnd, msg, wParam, lParam);
      break;
    case WM_SYSCOMMAND:
      if (!fChildPreview)
        switch (wParam)
          {
          case SC_CLOSE:
          case SC_SCREENSAVE:
          case SC_NEXTWINDOW:
          case SC_PREVWINDOW:
            return FALSE;
          }
      break;
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_NCACTIVATE:
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
      if (closing)
        return DefWindowProc(hWnd, msg, wParam, lParam);
      break;
    }
  return ScreenSaverProc(hWnd, msg, wParam, lParam);
}

LONG WINAPI DefScreenSaverProc(HWND hWnd, UINT msg,
                               WPARAM wParam, LPARAM lParam)
{
                                                            
  if (fChildPreview || closing)
    return DefWindowProc(hWnd, msg, wParam, lParam);

  switch (msg)
    {
    case WM_CLOSE:
      TerminateScreenSaver(hWnd);
                                                                     
                                       
         
      return 0;
    case SCRM_VERIFYPW: 
                                                                             
      if (VerifyScreenSavePwd) 
        return VerifyScreenSavePwd(hWnd);
      else
        return TRUE;
    case WM_SETCURSOR:
      if (checking_pwd)
        break;
      SetCursor(NULL);
      return TRUE;
    case WM_NCACTIVATE:
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
      if (wParam != FALSE)
        break;
    case WM_MOUSEMOVE:
      {
        POINT pt;
        GetCursorPos(&pt);
        if (pt.x == pt_orig.x && pt.y == pt_orig.y)
          break;
      }
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
                                         
      if (!checking_pwd)
        PostMessage(hWnd, WM_CLOSE, 0, 0);
      break;
    }
  return DefWindowProc(hWnd, msg, wParam, lParam);
}

extern void ScreenSaverNotifyDestroy();
static void TerminateScreenSaver(HWND hWnd)
{
                             
  if (checking_pwd || closing)
    return;

                       
  if (VerifyScreenSavePwd)
    {
      checking_pwd = TRUE;
      closing = SendMessage(hWnd, SCRM_VERIFYPW, 0, 0);
      checking_pwd = FALSE;
    }
  else
    closing = TRUE;

                       
  if (closing)
    {
		ScreenSaverNotifyDestroy();
		DestroyWindow(hWnd);
    }
  else
    GetCursorPos(&pt_orig);                                     
}

  
                                                  
                
   
static BOOL RegisterClasses(void)
{
  WNDCLASS cls;
  
  cls.hCursor = NULL; 
  cls.hIcon = LoadIcon(hMainInstance, MAKEINTATOM(ID_APP)); 
  cls.lpszMenuName = NULL;
  cls.lpszClassName = CLASS_SCRNSAVE;
  cls.hbrBackground = GetStockObject(BLACK_BRUSH);
  cls.hInstance = hMainInstance;
  cls.style = CS_VREDRAW | CS_HREDRAW | CS_SAVEBITS | CS_PARENTDC;
  cls.lpfnWndProc = (WNDPROC) SysScreenSaverProc;
  cls.cbWndExtra = 0;
  cls.cbClsExtra = 0;
  
  if (!RegisterClass(&cls))
    return FALSE;

  return RegisterDialogClasses(hMainInstance);
}

void WINAPI ScreenSaverChangePassword(HWND hParent)
{
                                         
  HINSTANCE hMpr = LoadLibrary(TEXT("MPR.DLL"));
  
  if (hMpr)
    {
      CHPWDPROC ChangePassword;
      ChangePassword = (CHPWDPROC) GetProcAddress(hMpr, szPwdChangePassword);

                                                     
      if (ChangePassword)
        ChangePassword(TEXT("SCRSAVE"), hParent, 0, NULL);

      FreeLibrary(hMpr);
    }
}

