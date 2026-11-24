package;

import haxe.io.Path;
import lime.graphics.Image;
import lime.ui.Window;
import lime.utils.Assets;
import sys.FileSystem;

#if windows
@:cppFileCode('
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <string>
#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#endif

namespace PvzAppIcon
{
    static std::wstring wide(const char* text)
    {
        const int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, nullptr, 0);
        if (size <= 1) return {};
        std::wstring result(size, wchar_t{});
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text, -1, &result[0], size);
        result.pop_back();
        return result;
    }

    struct WindowSearch
    {
        std::wstring title;
        HWND window;
    };

    static BOOL CALLBACK findWindow(HWND window, LPARAM parameter)
    {
        auto& search = *reinterpret_cast<WindowSearch*>(parameter);
        if (GetWindowTextLengthW(window) != static_cast<int>(search.title.size())) return TRUE;
        std::wstring title(search.title.size() + 1, wchar_t{});
        GetWindowTextW(window, &title[0], static_cast<int>(title.size()));
        title.resize(search.title.size());
        if (title != search.title) return TRUE;
        search.window = window;
        return FALSE;
    }

    static bool applyIco(const char* path, const char* title)
    {
        WindowSearch search{wide(title), nullptr};
        if (search.title.empty()) return false;
        EnumThreadWindows(GetCurrentThreadId(), &findWindow, reinterpret_cast<LPARAM>(&search));
        if (!search.window) return false;

        const auto file = wide(path);
        if (file.empty()) return false;
        const auto load = [&](int width, int height) {
            return reinterpret_cast<HICON>(LoadImageW(nullptr, file.c_str(), IMAGE_ICON,
                GetSystemMetrics(width), GetSystemMetrics(height), LR_LOADFROMFILE));
        };
        HICON large = load(SM_CXICON, SM_CYICON);
        HICON small = load(SM_CXSMICON, SM_CYSMICON);
        if (!large && !small) return false;
        SendMessageW(search.window, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(large ? large : small));
        SendMessageW(search.window, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(small ? small : large));
        return true;
    }
}
')
#end
class AppIcon
{
    public static function apply(window:Window, directory:String):Void
    {
        try
        {
            var png = Path.join([directory, "icon.png"]);
            if (FileSystem.exists(png) && !FileSystem.isDirectory(png))
            {
                var image = Image.fromFile(png);
                if (image != null && image.width > 0 && image.height > 0)
                {
                    window.setIcon(image);
                    return;
                }
            }
        }
        catch (_:Dynamic) {}

        #if windows
        try
        {
            var ico = Path.join([directory, "icon.ico"]);
            if (FileSystem.exists(ico) && !FileSystem.isDirectory(ico))
            {
                var path = cpp.ConstCharStar.fromString(ico);
                var title = cpp.ConstCharStar.fromString(window.title);
                if (untyped __cpp__("PvzAppIcon::applyIco({0}, {1})", path, title)) return;
            }
        }
        catch (_:Dynamic) {}
        #end

        try
        {
            var image = Assets.getImage("pvz-default-icon.png");
            if (image != null) window.setIcon(image);
        }
        catch (_:Dynamic) {}
    }
}
