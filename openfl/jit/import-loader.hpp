#pragma once
#include <SDL3/SDL_main.h>
#include <map>
#include <string>
namespace OpenPVZImports
{
    std::string& errors() { static std::string value; return value; }
    FARPROC resolve(const char* dll, const char* symbol)
    {
        static std::map<std::string, HMODULE> modules;
        auto found = modules.find(dll);
        HMODULE module = found == modules.end() ? nullptr : found->second;
        if (!module)
        {
            const std::string name(dll);
            bool packaged = name == "SDL3.dll" || name == "SDL3_ttf.dll" || name == "portaudio_x64.dll" || name == "pvz_codecs.dll" || name == "bass.dll";
            std::wstring path;
            if (packaged)
            {
                wchar_t exe[32768];
                DWORD count = GetModuleFileNameW(nullptr, exe, 32768);
                path.assign(exe, count);
                auto slash = path.find_last_of(L"\\/");
                path = path.substr(0, slash + 1) + L"pvz-runtime\\";
            }
            path.append(name.begin(), name.end());
            module = LoadLibraryExW(path.c_str(), nullptr, packaged ?
                LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS : LOAD_LIBRARY_SEARCH_SYSTEM32);
            if (module) modules[dll] = module;
            else
            {
                errors() += std::string("Cannot load ") + dll + " (Windows " + std::to_string(GetLastError()) + ")\n";
                return nullptr;
            }
        }
        FARPROC address = GetProcAddress(module, symbol);
        if (!address) errors() += std::string("Missing export ") + dll + "!" + symbol + "\n";
        return address;
    }
}
const char* pvz_import_error() { return OpenPVZImports::errors().c_str(); }

extern "C" int PASCAL __WSAFDIsSet(SOCKET socket, fd_set* set)
{
    static auto fn = reinterpret_cast<decltype(&__WSAFDIsSet)>(OpenPVZImports::resolve("ws2_32.dll", "__WSAFDIsSet"));
    return fn ? fn(socket, set) : 0;
}
