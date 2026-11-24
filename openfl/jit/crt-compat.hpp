#pragma once
#include <direct.h>
#include <sys/stat.h>
#include <cstring>

                                                                          
                                                                   
#define chdir _chdir
#define getcwd _getcwd
#define mkdir _mkdir
#define rmdir _rmdir
#define strupr _strupr
#define stricmp _stricmp
#define strcmpi _stricmp
#define strnicmp _strnicmp
#undef stat
#define stat _stat64i32

#if defined(PVZ_DEFINE_CRT_COMPAT) && defined(__MINGW32__) && defined(_UCRT)
                                                                              
                                                                                 
extern "C" unsigned __int64* __cdecl __local_stdio_printf_options(void)
{
    static unsigned __int64 options = _CRT_INTERNAL_PRINTF_LEGACY_WIDE_SPECIFIERS |
        _CRT_INTERNAL_PRINTF_STANDARD_ROUNDING;
    return &options;
}
#endif
