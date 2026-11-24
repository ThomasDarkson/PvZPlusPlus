package;

                                                                             
@:cppFileCode('
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <dbghelp.h>
#include <cstdio>
#include <cstdint>
#include <atomic>

namespace OpenPVZCrash
{
    static wchar_t prefix[32768] = {};
    static volatile LONG recording = 0;
    static LPTOP_LEVEL_EXCEPTION_FILTER previous = nullptr;
    static bool installed = false;
    static char lastContext[2][1024] = {};
    static std::atomic<unsigned> contextIndex{0};

    static inline void setContext(const char* value)
    {
        const unsigned next = 1 - contextIndex.load(std::memory_order_relaxed);
        std::snprintf(lastContext[next], sizeof(lastContext[next]), "%s", value);
        contextIndex.store(next, std::memory_order_release);
    }

    static bool readable(uintptr_t address, size_t bytes)
    {
        MEMORY_BASIC_INFORMATION info{};
        if (!VirtualQuery(reinterpret_cast<void*>(address), &info, sizeof(info)) ||
            info.State != MEM_COMMIT || (info.Protect & (PAGE_GUARD | PAGE_NOACCESS))) return false;
        const uintptr_t begin = reinterpret_cast<uintptr_t>(info.BaseAddress);
        if (address < begin || bytes > info.RegionSize || address - begin > info.RegionSize - bytes) return false;
        return (info.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY |
            PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) != 0;
    }
    static void writeText(HANDLE file, const char* text, int length)
    {
        if (length <= 0) return;
        DWORD written = 0;
        WriteFile(file, text, static_cast<DWORD>(length), &written, nullptr);
        FlushFileBuffers(file);
    }
#ifdef _WIN64
    static void writeStack(HANDLE file, const CONTEXT& fault)
    {
        CONTEXT context = fault;
        static char line[34000];
        int length = std::snprintf(line, sizeof(line),
            "RIP=%016llX RSP=%016llX RBP=%016llX%cRAX=%016llX RBX=%016llX RCX=%016llX RDX=%016llX%cRSI=%016llX RDI=%016llX R8=%016llX R9=%016llX%c",
            static_cast<unsigned long long>(fault.Rip), static_cast<unsigned long long>(fault.Rsp),
            static_cast<unsigned long long>(fault.Rbp), 10,
            static_cast<unsigned long long>(fault.Rax), static_cast<unsigned long long>(fault.Rbx),
            static_cast<unsigned long long>(fault.Rcx), static_cast<unsigned long long>(fault.Rdx), 10,
            static_cast<unsigned long long>(fault.Rsi), static_cast<unsigned long long>(fault.Rdi),
            static_cast<unsigned long long>(fault.R8), static_cast<unsigned long long>(fault.R9), 10);
        writeText(file, line, length);
        const uintptr_t codeStart = static_cast<uintptr_t>(fault.Rip) - 16;
        if (fault.Rip >= 16 && readable(codeStart, 48))
        {
            length = std::snprintf(line, sizeof(line), "Code bytes at %p (RIP is +16): ", reinterpret_cast<void*>(codeStart));
            const auto* bytes = reinterpret_cast<const unsigned char*>(codeStart);
            for (int i = 0; i < 48; ++i)
                length += std::snprintf(line + length, sizeof(line) - length, "%02X ", bytes[i]);
            length += std::snprintf(line + length, sizeof(line) - length, "%c", 10);
            writeText(file, line, length);
        }
        for (int depth = 0; depth < 48 && context.Rip; ++depth)
        {
            const DWORD64 oldPC = context.Rip, oldSP = context.Rsp;
            DWORD64 imageBase = 0;
            auto* entry = RtlLookupFunctionEntry(context.Rip, &imageBase, nullptr);
            HMODULE module = nullptr;
            static char modulePath[32768];
            modulePath[0] = 0;
            if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    reinterpret_cast<LPCSTR>(context.Rip), &module))
                GetModuleFileNameA(module, modulePath, sizeof(modulePath));
            modulePath[sizeof(modulePath) - 1] = 0;
            length = std::snprintf(line, sizeof(line),
                "Frame %d: PC=%016llX module=%s moduleOffset=0x%llX functionBase=%016llX functionOffset=0x%llX unwindImage=%016llX%c",
                depth, static_cast<unsigned long long>(context.Rip), modulePath,
                static_cast<unsigned long long>(module ? context.Rip - reinterpret_cast<uintptr_t>(module) : 0),
                static_cast<unsigned long long>(entry ? imageBase + entry->BeginAddress : 0),
                static_cast<unsigned long long>(entry ? context.Rip - imageBase - entry->BeginAddress : 0),
                static_cast<unsigned long long>(imageBase), 10);
            writeText(file, line, length);
            if (entry)
            {
                PVOID handlerData = nullptr;
                DWORD64 frame = 0;
                RtlVirtualUnwind(UNW_FLAG_NHANDLER, imageBase, context.Rip, entry,
                    &context, &handlerData, &frame, nullptr);
            }
            else
            {
                if (!readable(static_cast<uintptr_t>(context.Rsp), sizeof(DWORD64))) break;
                context.Rip = *reinterpret_cast<const DWORD64*>(context.Rsp);
                context.Rsp += sizeof(DWORD64);
            }
            if ((context.Rip == oldPC && context.Rsp == oldSP) || context.Rsp < oldSP) break;
        }
    }
#endif

    static inline LONG WINAPI handler(EXCEPTION_POINTERS* exception)
    {
        if (InterlockedCompareExchange(&recording, 1, 0) == 0)
        {
            static wchar_t path[32768];
            lstrcpynW(path, prefix, 32700);
            lstrcatW(path, L".txt");
            HANDLE file = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (file != INVALID_HANDLE_VALUE)
            {
                auto* record = exception ? exception->ExceptionRecord : nullptr;
                void* address = record ? record->ExceptionAddress : nullptr;
                HMODULE module = nullptr;
                static char modulePath[32768] = {};
                if (address && GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    reinterpret_cast<LPCSTR>(address), &module))
                    GetModuleFileNameA(module, modulePath, sizeof(modulePath));
                static char message[34000];
                int length = std::snprintf(message, sizeof(message),
                    "Plants vs Zombies++ unhandled native exception r12%cCode: 0x%08lX%cAddress: %p%cModule: %s%cModule offset: 0x%llX%c",
                    10, record ? record->ExceptionCode : 0UL, 10, address, 10, modulePath, 10,
                    static_cast<unsigned long long>(module ? reinterpret_cast<uintptr_t>(address) - reinterpret_cast<uintptr_t>(module) : 0), 10);
                writeText(file, message, length);
                length = std::snprintf(message, sizeof(message), "Last game context:%c%s%c",
                    10, lastContext[contextIndex.load(std::memory_order_acquire)], 10);
                writeText(file, message, length);
                if (record && record->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && record->NumberParameters >= 2)
                {
                    length = std::snprintf(message, sizeof(message), "Access kind: %llu (0=read, 1=write, 8=execute)%cAccessed address: %p%c",
                        static_cast<unsigned long long>(record->ExceptionInformation[0]), 10,
                        reinterpret_cast<void*>(record->ExceptionInformation[1]), 10);
                    writeText(file, message, length);
                }
#ifdef _WIN64
                if (exception && exception->ContextRecord) writeStack(file, *exception->ContextRecord);
#endif
                CloseHandle(file);
            }
            HMODULE dbghelp = LoadLibraryW(L"dbghelp.dll");
            if (dbghelp)
            {
                using DumpFn = decltype(&MiniDumpWriteDump);
                auto dump = reinterpret_cast<DumpFn>(GetProcAddress(dbghelp, "MiniDumpWriteDump"));
                lstrcpynW(path, prefix, 32700);
                lstrcatW(path, L".dmp");
                HANDLE dumpFile = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (dump && dumpFile != INVALID_HANDLE_VALUE)
                {
                    MINIDUMP_EXCEPTION_INFORMATION info{};
                    info.ThreadId = GetCurrentThreadId();
                    info.ExceptionPointers = exception;
                    info.ClientPointers = FALSE;
                    dump(GetCurrentProcess(), GetCurrentProcessId(), dumpFile, MiniDumpNormal,
                        exception ? &info : nullptr, nullptr, nullptr);
                }
                if (dumpFile != INVALID_HANDLE_VALUE) CloseHandle(dumpFile);
                FreeLibrary(dbghelp);
            }
        }
        if (previous && previous != &handler) return previous(exception);
        return EXCEPTION_CONTINUE_SEARCH;
    }
    static inline void install(const char* utf8Prefix)
    {
        if (!MultiByteToWideChar(CP_UTF8, 0, utf8Prefix, -1, prefix, 32768)) return;
        auto old = SetUnhandledExceptionFilter(&handler);
        if (!installed) { previous = old; installed = true; }
    }
}
')
class NativeDiagnostics
{
    public static function setContext(value:String):Void
    {
        #if windows
        var chars = cpp.ConstCharStar.fromString(value);
        untyped __cpp__("OpenPVZCrash::setContext({0})", chars);
        #end
    }

    public static function install(prefix:String):Void
    {
        #if windows
        var chars = cpp.ConstCharStar.fromString(prefix);
        untyped __cpp__("OpenPVZCrash::install({0})", chars);
        #end
    }
}
