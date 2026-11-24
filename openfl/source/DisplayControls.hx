package;

@:cppFileCode('
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <GL/gl.h>
#include <cstdint>

static bool pvz_visible_swap_interval(bool enabled)
{
    HMODULE gl = GetModuleHandleW(L"opengl32.dll");
    if (gl)
    {
        using Current = HGLRC (WINAPI*)();
        using Lookup = PROC (WINAPI*)(LPCSTR);
        using Swap = BOOL (WINAPI*)(int);
        auto current = reinterpret_cast<Current>(GetProcAddress(gl, "wglGetCurrentContext"));
        auto lookup = reinterpret_cast<Lookup>(GetProcAddress(gl, "wglGetProcAddress"));
        if (current && lookup && current())
        {
            PROC address = lookup("wglSwapIntervalEXT");
            const auto value = reinterpret_cast<std::intptr_t>(address);
            if (value != 0 && value != 1 && value != 2 && value != 3 && value != -1)
            {
                const int interval = enabled ? 1 : 0;
                if (reinterpret_cast<Swap>(address)(interval) == FALSE) return false;
                PROC readback = lookup("wglGetSwapIntervalEXT");
                const auto readbackValue = reinterpret_cast<std::intptr_t>(readback);
                if (readbackValue == 0 || readbackValue == 1 || readbackValue == 2 ||
                    readbackValue == 3 || readbackValue == -1) return true;
                using GetInterval = int (WINAPI*)();
                return reinterpret_cast<GetInterval>(readback)() == interval;
            }
        }
    }
    HMODULE egl = GetModuleHandleW(L"libEGL.dll");
    if (egl)
    {
        using Current = void* (WINAPI*)();
        using Swap = unsigned int (WINAPI*)(void*, int);
        auto current = reinterpret_cast<Current>(GetProcAddress(egl, "eglGetCurrentDisplay"));
        auto swap = reinterpret_cast<Swap>(GetProcAddress(egl, "eglSwapInterval"));
        void* display = current ? current() : nullptr;
        if (display && swap) return swap(display, enabled ? 1 : 0) != 0;
    }
    return false;
}
static PROC pvz_wgl_extension(HMODULE gl, const char* symbol)
{
    using Lookup = PROC (WINAPI*)(LPCSTR);
    auto lookup = reinterpret_cast<Lookup>(GetProcAddress(gl, "wglGetProcAddress"));
    auto proc = lookup ? lookup(symbol) : nullptr;
    auto value = reinterpret_cast<std::intptr_t>(proc);
    return value == 0 || value == 1 || value == 2 || value == 3 || value == -1 ? nullptr : proc;
}

static HGLRC pvz_current_gl_context(HMODULE gl)
{
    using Current = HGLRC (WINAPI*)();
    auto current = reinterpret_cast<Current>(GetProcAddress(gl, "wglGetCurrentContext"));
    return current ? current() : nullptr;
}

struct PvzGpuInterop
{
    using Close = BOOL (WINAPI*)(HANDLE);
    using Unregister = BOOL (WINAPI*)(HANDLE, HANDLE);
    using Objects = BOOL (WINAPI*)(HANDLE, GLint, HANDLE*);
    HMODULE gl = nullptr;
    HGLRC context = nullptr;
    HANDLE device = nullptr, object = nullptr;
    GLuint name = 0;
    GLuint framebuffers[2] = {};
    using DeleteFramebuffers = void (APIENTRY*)(GLsizei, const GLuint*);
    DeleteFramebuffers deleteFramebuffers = nullptr;
    bool locked = false;
    Close close = nullptr;
    Unregister unregisterObject = nullptr;
    Objects lock = nullptr, unlock = nullptr;

    bool end()
    {
        bool ok = true;
        if (locked) ok = unlock(device, 1, &object) != FALSE;
        if (object) ok = (unregisterObject(device, object) != FALSE) && ok;
        if (device) ok = (close(device) != FALSE) && ok;
        if (gl && context && pvz_current_gl_context(gl) == context)
        {
            if (framebuffers[0]) deleteFramebuffers(2, framebuffers);
        }
        if (name && pvz_current_gl_context(gl) == context)
        {
            using Delete = void (WINAPI*)(GLsizei, const GLuint*);
            reinterpret_cast<Delete>(GetProcAddress(gl, "glDeleteTextures"))(1, &name);
        }
        *this = PvzGpuInterop{};
        return ok;
    }

    int begin(void* nativeDevice, void* nativeTexture)
    {
        gl = GetModuleHandleW(L"opengl32.dll");
        if (!gl) return -1;
        context = pvz_current_gl_context(gl);
        if (!context) return -1;
        using Open = HANDLE (WINAPI*)(void*);
        using Register = HANDLE (WINAPI*)(HANDLE, void*, GLuint, GLenum, GLenum);
        using Generate = void (WINAPI*)(GLsizei, GLuint*);
        auto open = reinterpret_cast<Open>(pvz_wgl_extension(gl, "wglDXOpenDeviceNV"));
        auto registerObject = reinterpret_cast<Register>(pvz_wgl_extension(gl, "wglDXRegisterObjectNV"));
        close = reinterpret_cast<Close>(pvz_wgl_extension(gl, "wglDXCloseDeviceNV"));
        unregisterObject = reinterpret_cast<Unregister>(pvz_wgl_extension(gl, "wglDXUnregisterObjectNV"));
        lock = reinterpret_cast<Objects>(pvz_wgl_extension(gl, "wglDXLockObjectsNV"));
        unlock = reinterpret_cast<Objects>(pvz_wgl_extension(gl, "wglDXUnlockObjectsNV"));
        if (!open || !registerObject || !close || !unregisterObject || !lock || !unlock) return -2;

        device = open(nativeDevice);
        if (!device) return -3;
        reinterpret_cast<Generate>(GetProcAddress(gl, "glGenTextures"))(1, &name);
        object = registerObject(device, nativeTexture, name, GL_TEXTURE_2D, 0);                           
        return object ? 1 : -4;
    }

    int check(bool green)
    {
        using GetInt = void (WINAPI*)(GLenum, GLint*);
        using Bind = void (WINAPI*)(GLenum, GLuint);
        using Size = void (WINAPI*)(GLenum, GLint, GLenum, GLint*);
        using Read = void (WINAPI*)(GLenum, GLint, GLenum, GLenum, void*);
        auto get = reinterpret_cast<GetInt>(GetProcAddress(gl, "glGetIntegerv"));
        auto bind = reinterpret_cast<Bind>(GetProcAddress(gl, "glBindTexture"));
        auto size = reinterpret_cast<Size>(GetProcAddress(gl, "glGetTexLevelParameteriv"));
        auto read = reinterpret_cast<Read>(GetProcAddress(gl, "glGetTexImage"));
        if (!get || !bind || !size || !read) return -6;

        GLint bound = 0, packBuffer = 0, rowLength = 0, skipRows = 0, skipPixels = 0;
        get(GL_TEXTURE_BINDING_2D, &bound);
        get(0x88ED, &packBuffer);                                
        get(GL_PACK_ROW_LENGTH, &rowLength);
        get(GL_PACK_SKIP_ROWS, &skipRows);
        get(GL_PACK_SKIP_PIXELS, &skipPixels);
        if (packBuffer || rowLength || skipRows || skipPixels) return -6;
        if (!lock(device, 1, &object)) return -5;
        locked = true;

        bind(GL_TEXTURE_2D, name);
        GLint w = 0, h = 0;
        size(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
        size(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
        unsigned char pixels[16 * 16 * 4] = {};
        if (w == 16 && h == 16)
            read(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        bind(GL_TEXTURE_2D, static_cast<GLuint>(bound));

        if (!unlock(device, 1, &object)) return -8;
        locked = false;
        if (w != 16 || h != 16) return -6;
        const bool flipped = pixels[2] == 255;
        for (int y = 0; y < 16; ++y)
        {
            const bool blue = (y >= 8) != flipped;
            for (int x = 0; x < 16; ++x)
            {
                const auto* pixel = pixels + (y * 16 + x) * 4;
                if (pixel[0] != (!blue && !green ? 255 : 0) ||
                    pixel[1] != (!blue && green ? 255 : 0) ||
                    pixel[2] != (blue ? 255 : 0) || pixel[3] != 255)
                    return -7;
            }
        }
        return flipped ? 2 : 1;
    }

    int copyToBoundTexture(int width, int height, bool flipped)
    {
        if (!object || pvz_current_gl_context(gl) != context) return -1;

        constexpr GLenum ReadFramebuffer = 0x8CA8;
        constexpr GLenum DrawFramebuffer = 0x8CA9;
        constexpr GLenum ReadFramebufferBinding = 0x8CAA;
        constexpr GLenum DrawFramebufferBinding = 0x8CA6;
        constexpr GLenum ColorAttachment = 0x8CE0;
        constexpr GLenum FramebufferComplete = 0x8CD5;

        using Bind = void (APIENTRY*)(GLenum, GLuint);
        using Attach = void (APIENTRY*)(GLenum, GLenum, GLenum, GLuint, GLint);
        using Check = GLenum (APIENTRY*)(GLenum);
        using Blit = void (APIENTRY*)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);
        using Generate = void (APIENTRY*)(GLsizei, GLuint*);
        auto bind = reinterpret_cast<Bind>(pvz_wgl_extension(gl, "glBindFramebuffer"));
        auto attach = reinterpret_cast<Attach>(pvz_wgl_extension(gl, "glFramebufferTexture2D"));
        auto check = reinterpret_cast<Check>(pvz_wgl_extension(gl, "glCheckFramebufferStatus"));
        auto blit = reinterpret_cast<Blit>(pvz_wgl_extension(gl, "glBlitFramebuffer"));
        using GetInt = void (APIENTRY*)(GLenum, GLint*);
        using IsEnabled = GLboolean (APIENTRY*)(GLenum);
        using Toggle = void (APIENTRY*)(GLenum);
        using GetError = GLenum (APIENTRY*)();
        auto get = reinterpret_cast<GetInt>(GetProcAddress(gl, "glGetIntegerv"));
        auto isEnabled = reinterpret_cast<IsEnabled>(GetProcAddress(gl, "glIsEnabled"));
        auto enable = reinterpret_cast<Toggle>(GetProcAddress(gl, "glEnable"));
        auto disable = reinterpret_cast<Toggle>(GetProcAddress(gl, "glDisable"));
        auto getError = reinterpret_cast<GetError>(GetProcAddress(gl, "glGetError"));
        if (!bind || !attach || !check || !blit) return -9;

        GLint destination = 0, readBefore = 0, drawBefore = 0;
        get(GL_TEXTURE_BINDING_2D, &destination);
        if (!destination || static_cast<GLuint>(destination) == name) return -10;
        if (!framebuffers[0])
        {
            auto generate = reinterpret_cast<Generate>(pvz_wgl_extension(gl, "glGenFramebuffers"));
            deleteFramebuffers = reinterpret_cast<DeleteFramebuffers>(pvz_wgl_extension(gl, "glDeleteFramebuffers"));
            if (!generate || !deleteFramebuffers) return -9;
            generate(2, framebuffers);
            if (!framebuffers[0] || !framebuffers[1]) return -10;
        }
        if (!lock(device, 1, &object)) return -5;
        locked = true;

        get(ReadFramebufferBinding, &readBefore);
        get(DrawFramebufferBinding, &drawBefore);
        bind(ReadFramebuffer, framebuffers[0]);
        bind(DrawFramebuffer, framebuffers[1]);
        attach(ReadFramebuffer, ColorAttachment, GL_TEXTURE_2D, name, 0);
        attach(DrawFramebuffer, ColorAttachment, GL_TEXTURE_2D, static_cast<GLuint>(destination), 0);
        const bool complete = check(ReadFramebuffer) == FramebufferComplete && check(DrawFramebuffer) == FramebufferComplete;
        int result = -10;
        if (complete)
        {
            const bool scissor = isEnabled(GL_SCISSOR_TEST) != GL_FALSE;
            if (scissor) disable(GL_SCISSOR_TEST);
            blit(0, flipped ? height : 0, width, flipped ? 0 : height,
                0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            result = getError() == GL_NO_ERROR ? 1 : -11;
            if (scissor) enable(GL_SCISSOR_TEST);
        }

        attach(ReadFramebuffer, ColorAttachment, GL_TEXTURE_2D, 0, 0);
        attach(DrawFramebuffer, ColorAttachment, GL_TEXTURE_2D, 0, 0);
        bind(ReadFramebuffer, static_cast<GLuint>(readBefore));
        bind(DrawFramebuffer, static_cast<GLuint>(drawBefore));
        if (!unlock(device, 1, &object)) return -8;
        locked = false;
        return result;
    }
};
static PvzGpuInterop pvzGpuProbe;
static PvzGpuInterop pvzGpuOutput;

')
class DisplayControls
{
    public static function setVSync(enabled:Bool):Bool
    {
        #if (cpp && windows)
        return untyped __cpp__("pvz_visible_swap_interval({0})", enabled);
        #else
        return false;
        #end
    }
    public static function beginGpuProbe(device:Float, texture:Float):Int
    {
        #if (cpp && windows)
        return untyped __cpp__("pvzGpuProbe.begin(reinterpret_cast<void*>(static_cast<uintptr_t>({0})), reinterpret_cast<void*>(static_cast<uintptr_t>({1})))", device, texture);
        #else
        return -1;
        #end
    }
    public static function checkGpuProbe(green:Bool):Int
    {
        #if (cpp && windows)
        return untyped __cpp__("pvzGpuProbe.check({0})", green);
        #else
        return -1;
        #end
    }
    public static function endGpuProbe():Bool
    {
        #if (cpp && windows)
        return untyped __cpp__("pvzGpuProbe.end()");
        #else
        return false;
        #end
    }

    public static function beginGpuOutput(device:Float, texture:Float):Int
    {
        #if (cpp && windows)
        return untyped __cpp__("pvzGpuOutput.begin(reinterpret_cast<void*>(static_cast<uintptr_t>({0})), reinterpret_cast<void*>(static_cast<uintptr_t>({1})))", device, texture);
        #else
        return -1;
        #end
    }

    public static function copyGpuOutput(width:Int, height:Int, flipped:Bool):Int
    {
        #if (cpp && windows)
        return untyped __cpp__("pvzGpuOutput.copyToBoundTexture({0}, {1}, {2})", width, height, flipped);
        #else
        return -1;
        #end
    }

    public static function endGpuOutput():Bool
    {
        #if (cpp && windows)
        return untyped __cpp__("pvzGpuOutput.end()");
        #else
        return false;
        #end
    }

}
