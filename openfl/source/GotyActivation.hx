package;

import haxe.crypto.Sha256;
import haxe.io.Bytes;
import haxe.io.Path;
import sys.FileSystem;
import sys.io.File;

#if windows
@:cppFileCode('
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <dpapi.h>
#include <cstring>
#include <string>
#ifdef _MSC_VER
#pragma comment(lib, "crypt32.lib")
#endif

namespace PvzActivation
{
    static std::wstring wide(const char* text)
    {
        int count = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, nullptr, 0);
        if (count <= 1) return {};
        std::wstring result(count, wchar_t{});
        if (!MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, &result[0], count)) return {};
        result.pop_back();
        return result;
    }

    static DATA_BLOB payload(const char* value)
    {
        return {static_cast<DWORD>(std::strlen(value)), reinterpret_cast<BYTE*>(const_cast<char*>(value))};
    }

    static bool matches(const char* record, const char* expected)
    {
        const auto path = wide(record);
        if (path.empty()) return false;
        HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE,
            nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE) return false;
        BYTE bytes[16384];
        DWORD count = 0;
        LARGE_INTEGER size{};
        const bool read = GetFileSizeEx(file, &size) && size.QuadPart > 0 && size.QuadPart <= sizeof(bytes) &&
            ReadFile(file, bytes, static_cast<DWORD>(size.QuadPart), &count, nullptr) &&
            count == static_cast<DWORD>(size.QuadPart);
        CloseHandle(file);
        if (!read) return false;
        DATA_BLOB input{count, bytes}, clear{};
        if (!CryptUnprotectData(&input, nullptr, nullptr, nullptr, nullptr, CRYPTPROTECT_UI_FORBIDDEN, &clear))
            return false;
        const auto wanted = payload(expected);
        const bool valid = clear.cbData == wanted.cbData &&
            std::memcmp(clear.pbData, wanted.pbData, wanted.cbData) == 0;
        LocalFree(clear.pbData);
        return valid;
    }

    static bool save(const char* record, const char* value)
    {
        const auto path = wide(record);
        if (path.empty()) return false;
        const auto temp = path + L"." + std::to_wstring(GetCurrentProcessId()) + L".tmp";
        auto clear = payload(value);
        DATA_BLOB encrypted{};
        if (!CryptProtectData(&clear, nullptr, nullptr, nullptr, nullptr,
                CRYPTPROTECT_LOCAL_MACHINE | CRYPTPROTECT_UI_FORBIDDEN, &encrypted))
            return false;
        HANDLE file = CreateFileW(temp.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr);
        bool written = false;
        if (file != INVALID_HANDLE_VALUE)
        {
            DWORD count = 0;
            written = WriteFile(file, encrypted.pbData, encrypted.cbData, &count, nullptr) &&
                count == encrypted.cbData && FlushFileBuffers(file);
            CloseHandle(file);
        }
        LocalFree(encrypted.pbData);
        if (written && MoveFileExW(temp.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
            return true;
        if (file != INVALID_HANDLE_VALUE) DeleteFileW(temp.c_str());
        return false;
    }
}
')
#end
class GotyActivation
{
    static final trusted:Array<String> = GotyActivationConfig.configuredHashes();

    public static function requireConfiguration():Void
    {
        if (trusted.length == 0)
            throw "This build has no trusted GOTY main.pak SHA-256 configured.";
    }

    static function recordPath():String
    {
        var appData = Sys.getEnv("LOCALAPPDATA");
        if (appData == null || appData == "") throw "Windows Local AppData is unavailable.";
        return Path.join([appData, "PVZScriptable", "goty-activation.dat"]);
    }

    static function payload(hash:String):String
    {
        return "PVZScriptable GOTY activation v1\n" + SysInfo.getArchitecture() + "\n" + hash;
    }

    public static function isActive():Bool
    {
        #if windows
        var path = cpp.ConstCharStar.fromString(recordPath());
        for (hash in trusted)
        {
            var expected = cpp.ConstCharStar.fromString(payload(hash));
            if (untyped __cpp__("PvzActivation::matches({0}, {1})", path, expected)) return true;
        }
        #end
        return false;
    }

    public static function verify(path:String):{archive:Bytes, hash:String}
    {
        requireConfiguration();
        if (!FileSystem.exists(path) || FileSystem.isDirectory(path)) throw "Choose a GOTY main.pak file.";
        var archive = File.getBytes(path);
        var hash = Sha256.make(archive).toHex();
        if (!trusted.contains(hash)) throw "This main.pak does not match a trusted GOTY archive.";
        return {archive: archive, hash: hash};
    }

    public static function activate(hash:String):Void
    {
        if (!trusted.contains(hash)) throw "GOTY archive verification failed.";
        #if windows
        var record = recordPath();
        FileSystem.createDirectory(Path.directory(record));
        var path = cpp.ConstCharStar.fromString(record);
        var value = cpp.ConstCharStar.fromString(payload(hash));
        if (untyped __cpp__("PvzActivation::save({0}, {1})", path, value)) return;
        #end
        throw "Could not save the GOTY activation record.";
    }
}
