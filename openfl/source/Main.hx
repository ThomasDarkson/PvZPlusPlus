package;

import haxe.Timer;
import haxe.io.Bytes;
import haxe.io.Path;
import hxcpp_jit.Cpp;
import hxcpp_jit.CppFunction;
import hxcpp_jit.CppPointer;
import SysInfo;
import openfl.Lib;
import openfl.net.SharedObject;
import lime.graphics.RenderContext;
import lime.ui.FileDialog;
import lime.ui.FileDialogType;
import openfl.display.Bitmap;
import openfl.display.BitmapData;
import openfl.display.Sprite;
import openfl.display3D.Context3DTextureFormat;
import openfl.display3D.textures.RectangleTexture;
import openfl.events.Event;
import openfl.events.KeyboardEvent;
import openfl.events.MouseEvent;
import openfl.geom.Rectangle;
import openfl.text.TextField;
import openfl.text.TextFormat;
import openfl.utils.ByteArray;
import openfl.utils.Endian;
import sys.FileSystem;
import sys.io.File;

@:headerCode('#include <cstring>\n#include <cstdint>')
class Main extends Sprite
{
    static inline var W = 1280;
    static inline var H = 720;
    var importing = false;
    var importDialog:FileDialog;
    var importButton:Sprite;
    var jit:Cpp;
    var frameFn:CppFunction;
    var pixelsFn:CppFunction;
    var frameWidthFn:CppFunction;
    var viewScale = 1.0;
    var mouseFn:CppFunction;
    var displayFlagsFn:CppFunction;
    static inline var CUSTOM_CONTENT_DISABLED = 4;
    var displayFlags = 2 | CUSTOM_CONTENT_DISABLED;
    var sharpnessFn:CppFunction;
    var sharpness = 0;
    var sharpnessShader:SharpnessShader;
    var sharpnessSaveAt = 0.0;
    var fpsLimitFn:CppFunction;
    var fpsLimit = 60;
    var vsyncPending = true;
    var gpuProbeDone = false;
    var gpuSupported = false;
    var gpuFlipped = false;
    var gpuOutputCleanupFailed = false;
    var gpuTexture:RectangleTexture;
    var gpuImage:BitmapData;
    var gpuOutputFrameFn:CppFunction;
    var gpuOutputEndFn:CppFunction;
    var windowSizeFn:CppFunction;
    var displaySettings:SharedObject;
    var versionCheck:VersionCheck;
    var windowSizeIndex = 0;
    static final WINDOW_SIZES = [[1280, 720], [1600, 900], [1920, 1080], [2560, 1440], [3840, 2160], [7680, 4320]];
    var keyFn:CppFunction;
    var focusFn:CppFunction;
    var screen:Bitmap;
    var image:BitmapData;
    var overlay:Sprite;
    var statusText:Sprite;
    var openLog:Sprite;
    var bootFont:BootFont;
    var stoneButton:Array<BitmapData> = [];
    var failed = false;
    var bytes:Bytes;
    var pixels:ByteArray;
    var bounds = new Rectangle(0, 0, W, H);
    var baseDir:String;
    var logPath:String;
    var running = false;
    var closed = false;
    var lastTime = 0.0;
    var sampleStart = 0.0;
    var frameCount = 0;
    var frameMs = 0.0;
    var currentStatus = "Preparing PvZ (first launch may take a few minutes)...";
    var keys = new Map<Int, Bool>();
    var buttons = new Map<Int, Bool>();
    var sceneName = "";
    var startupPhase = "Starting OpenFL";

    public function new()
    {
        super();
        if (stage != null) attach();
        else addEventListener(Event.ADDED_TO_STAGE, attach);
    }
    function attach(?event:Event):Void
    {
        removeEventListener(Event.ADDED_TO_STAGE, attach);
        baseDir = FileSystem.fullPath(Path.directory(Sys.programPath()));
        logPath = Path.join([baseDir, "openpvz.log"]);
        try File.saveContent(logPath, Lib.application.window.title + " startup diagnostics\n") catch (_:Dynamic) {}
        AppIcon.apply(Lib.application.window, baseDir);
        NativeDiagnostics.install(Path.join([baseDir, "openpvz-native-crash"]));
        NativeDiagnostics.setContext(startupPhase);
        image = new BitmapData(W, H, false, 0);
        screen = new Bitmap(image);
        screen.smoothing = true;
        addChild(screen);
        overlay = new Sprite();
        overlay.mouseEnabled = false;
        statusText = new Sprite();
        statusText.mouseEnabled = statusText.mouseChildren = false;
        overlay.addChild(statusText);
        addChild(overlay);
        importButton = statusButton(promptForAssets);
        openLog = statusButton(function()
        {
            if (FileSystem.exists(logPath)) lime.system.System.openFile(logPath);
        });
        loadStatusAssets();
        bytes = Bytes.alloc(W * H * 4);
        pixels = ByteArray.fromBytes(bytes);
        pixels.endian = Endian.BIG_ENDIAN;
        stage.addEventListener(Event.RESIZE, resize);
        stage.addEventListener(MouseEvent.MOUSE_MOVE, mouseMove);
        stage.addEventListener(MouseEvent.MOUSE_DOWN, mouseDown);
        stage.addEventListener(MouseEvent.MOUSE_UP, mouseUp);
        stage.addEventListener(MouseEvent.RIGHT_MOUSE_DOWN, rightDown);
        stage.addEventListener(MouseEvent.RIGHT_MOUSE_UP, rightUp);
        stage.addEventListener(MouseEvent.MOUSE_WHEEL, wheel);
        stage.addEventListener(Event.MOUSE_LEAVE, leave);
        stage.addEventListener(KeyboardEvent.KEY_DOWN, keyDown);
        stage.addEventListener(KeyboardEvent.KEY_UP, keyUp);
        stage.addEventListener(Event.DEACTIVATE, deactivate);
        stage.addEventListener(Event.ACTIVATE, activate);
                                                                                  
        Lib.application.window.onRender.add(tick, false, 100);
        Lib.application.window.onRender.add(applyVSync);
        Lib.application.window.onRenderContextLost.add(function()
        {
            if (gpuSupported) useCpuFrames("visible render context lost");
        });
        Lib.application.window.onRenderContextRestored.add(function(_) vsyncPending = true);
        Lib.application.onExit.add(shutdown);
        resize();
        show(currentStatus);
        Timer.delay(boot, 150);
    }
    var intro:IntroPlayer;

    function playStartupIntro():Void
    {
        try
        {
            intro = new IntroPlayer(Path.join([baseDir, "game-data/intro"]), finishStartupIntro);
            addChild(intro);
            resize();
        }
        catch (e:Dynamic)
        {
            intro = null;
            log("Startup intro unavailable: " + Std.string(e));
        }
    }
    function finishStartupIntro():Void
    {
        if (intro == null) return;
        var player = intro;
        intro = null;
        removeChild(player);
        player.dispose();
        lastTime = sampleStart = Timer.stamp();
    }

    function installReanimation(dataDir:String, sourceDir:String, animation:String):Void
    {
        var extensionDir = Path.join([dataDir, "extension"]);
        var reanimDir = Path.join([extensionDir, "reanim"]);
        FileSystem.createDirectory(extensionDir);
        FileSystem.createDirectory(reanimDir);
        for (name in FileSystem.readDirectory(sourceDir))
        {
            if (!StringTools.endsWith(name, ".png") && !StringTools.endsWith(name, ".reanim")) continue;
            var target = Path.join([reanimDir, name]);
            if (!FileSystem.exists(target)) File.copy(Path.join([sourceDir, name]), target);
        }
        var compiledName = animation + ".compiled";
        var compiledDir = Path.join([extensionDir, "compiled", "reanim"]);
        var compiledTarget = Path.join([compiledDir, compiledName]);
        var bundledCompiled = Path.join([sourceDir, compiledName]);
        if (!FileSystem.exists(compiledTarget) && FileSystem.exists(bundledCompiled) &&
            File.getBytes(Path.join([reanimDir, animation])).compare(
                File.getBytes(Path.join([sourceDir, animation]))) == 0)
        {
            FileSystem.createDirectory(compiledDir);
            File.saveBytes(compiledTarget, File.getBytes(bundledCompiled));
        }
    }

    function installVersusAssets(dataRoot:String, dataDir:String):Void
    {
        for (directory in ["reanim", "compiled/reanim/versus"])
        {
            var source = Path.join([dataRoot, "versus", "extension", directory]);
            if (!FileSystem.exists(source)) continue;
            var destination = Path.join([dataDir, "extension", directory]);
            FileSystem.createDirectory(destination);
            for (name in FileSystem.readDirectory(source))
            {
                var target = Path.join([destination, name]);
                if (!FileSystem.exists(target)) File.copy(Path.join([source, name]), target);
            }
        }
    }

    function boot():Void
    {
        if (closed) return;
        try
        {
            var dataRoot = Path.join([baseDir, "game-data"]);
            if (!FileSystem.exists(Path.join([dataRoot, "dependency.pak"])))
                throw "Missing bundled game-data/dependency.pak. Rebuild or reinstall the project.";
            GotyActivation.requireConfiguration();
            var assetsReady = PakImporter.isReady(dataRoot);
            if (!assetsReady || !GotyActivation.isActive())
            {
                show(assetsReady
                    ? "Choose GOTY main.pak from your legal copy (Steam). Your imported assets will be kept."
                    : "Choose GOTY main.pak from your legal copy (Steam) to activate this PC and import the game assets.");
                importButton.visible = true;
                promptForAssets();
                return;
            }
            var dataDir = PakImporter.assetDirectory(dataRoot);
            installVersusAssets(dataRoot, dataDir);
            for (animation in ["tank.reanim", "tank_turret.reanim", "tank_cattail.reanim"])
                installReanimation(dataDir, Path.join([dataRoot, "heavy-weapon"]), animation);
            var src = Path.join([baseDir, "pvz-src"]);
            var bridge = Path.join([src, "openfl/jit"]);
            var flags = ["-std=c++23", "-fms-extensions", "-O1", "-Wno-everything", "-I" + src, "-I" + bridge];
            for (directory in ["ImageLib", "sdl3", "sdl3_ttf", "bass", "portaudio", "tinyfiledialogs", "zlib"])
                flags.push("-I" + Path.join([src, "include", directory]));
                                                                                                                 
            flags.push("-idirafter");
            flags.push(Path.join([src, "include/dx8sdk"]));      
                                                                                  
            jit = new Cpp(flags);
            var dlls = ["SDL3.dll", "SDL3_ttf.dll", "portaudio_x64.dll", "pvz_codecs.dll", "bass.dll"];
            if (FileSystem.exists("dlls_are_here.json") && !FileSystem.isDirectory("dlls_are_here.json")) {
                try {
                    var content = File.getContent("dlls_are_here.json");
                    var data:Dynamic = haxe.Json.parse(content);
                    if (data != null && data.dlls_to_load != null && data.dlls_to_load is Array)
                        dlls = data.dlls_to_load;
                }
                catch (e) {}
            }
            for (library in dlls)
            {
                log("Loading library: " + library);
                jit.loadLibrary(Path.join([baseDir, "pvz-runtime", library]));
            }
            show("Compiling PvZ. The first launch may take a few minutes...");
            var cache = Path.join([baseDir, "cache/pvzscriptablea0dot1-split-core"]);
            log("Loading or compiling PvZ core");
            var began = Timer.stamp();
            if (FileSystem.exists(Path.join([baseDir, "disable-jit-cache.txt"])))
            {
                jit.eval(Path.join([bridge, "core.cpp"]), true);
            }
            else
            {
                var units = [
                    "windows-imports.cpp", "board.cpp", "challenge.cpp", "plants.cpp",
                    "zombies.cpp", "lawn-objects.cpp", "lawn-systems.cpp", "lawn-ui.cpp",
                    "lawn-app.cpp", "effects.cpp", "effects-raster.cpp", "effects-animation.cpp",
                    "graphics.cpp", "graphics-images.cpp",
                    "graphics-raster.cpp", "framework.cpp",
                    "audio.cpp", "resources.cpp", "core-cached.cpp"
                ];
                var sources = [for (unit in units) Path.join([bridge, unit])];
                var workers = nativeCompileWorkerLimit();
                var compileFirst = "framework.cpp,effects-raster.cpp,graphics-raster.cpp";
                log(jit.evalCachedFiles(sources, Path.join([bridge, "api.hpp"]), cache, workers, compileFirst));
            }
            var elapsed = Std.int((Timer.stamp() - began) * 1000);
            log("Loaded in " + elapsed + " ms");
            var imports:String = cast jit.call("pvz_import_error");
            if (imports != null && imports.length > 0) throw imports;
                                                                                 
            frameFn = jit.bind("pvz_frame");
            pixelsFn = jit.bind("pvz_pixels");
            gpuOutputFrameFn = jit.bind("pvz_gpu_output_frame");
            gpuOutputEndFn = jit.bind("pvz_gpu_output_end");
            frameWidthFn = jit.bind("pvz_frame_width");
            mouseFn = jit.bind("pvz_mouse");
            keyFn = jit.bind("pvz_key");
            focusFn = jit.bind("pvz_focus");
            windowSizeFn = jit.bind("pvz_window_size");
            displayFlagsFn = jit.bind("pvz_display_flags");
            fpsLimitFn = jit.bind("pvz_fps_limit");
            sharpnessFn = jit.bind("pvz_sharpness");
            try
            {
                displaySettings = SharedObject.getLocal("pvzscriptable-display");
                var saved:Dynamic = displaySettings.data.windowSize;
                if (Std.isOfType(saved, Int) && saved >= 0 && saved < WINDOW_SIZES.length) windowSizeIndex = saved;
                if (Std.isOfType(displaySettings.data.displayFlags, Int))
                    displayFlags = (displaySettings.data.displayFlags & 3) | CUSTOM_CONTENT_DISABLED;
                if (displaySettings.data.customContentEnabled == true)
                    displayFlags &= ~CUSTOM_CONTENT_DISABLED;
                var savedSharpness:Dynamic = displaySettings.data.sharpness;
                if (Std.isOfType(savedSharpness, Int) && savedSharpness >= 0 && savedSharpness <= 100)
                    sharpness = savedSharpness;
                var savedLimit:Dynamic = displaySettings.data.fpsLimit;
                if (Std.isOfType(savedLimit, Int) && savedLimit >= 30 && savedLimit <= 360) fpsLimit = savedLimit;
            }
            catch (_:Dynamic) {}
            jit.call("pvz_set_window_size", [windowSizeIndex]);
            applyWindowSize();
            jit.call("pvz_set_display_flags", [displayFlags]);
            Lib.application.window.fullscreen = (displayFlags & 1) != 0;
            jit.call("pvz_set_sharpness", [sharpness]);
            applySharpness();
            jit.call("pvz_set_fps_limit", [fpsLimit]);
            Lib.application.window.frameRate = fpsLimit;
            vsyncPending = true;
            show("Launching PvZ...");
                                                                                      
            Timer.delay(function() initializeNative(dataDir), 60);
        }
        catch (e:Dynamic) fail(Std.string(e));
    }
    function promptForAssets():Void
    {
        if (closed || importing || jit != null) return;
        importing = true;
        importButton.visible = false;
        importDialog = new FileDialog();
        importDialog.onCancel.add(function()
        {
            importing = false;
            if (closed) return;
            show("Setup canceled. Choose GOTY main.pak from your legal copy (Steam) to import your GOTY assets and start the game.");
            importButton.visible = true;
        });
        importDialog.onSelect.add(function(path:String)
        {
            if (closed) return;
            show("Reading main.pak to see if it's geniune...");
            var queue = new sys.thread.Deque<{kind:Int, message:String}>();
            var poll = new Timer(50);
            poll.run = function()
            {
                if (closed) { poll.stop(); return; }
                while (true)
                {
                    var event = queue.pop(false);
                    if (event == null) break;
                    if (event.kind == 0) show(event.message);
                    else
                    {
                        poll.stop();
                        importing = false;
                        if (event.kind == 1)
                        {
                            log(event.message);
                            loadStatusAssets();
                            show("GOTY verification complete. Preparing PvZ (this may take a while)...");
                            Timer.delay(boot, 100);
                        }
                        else
                        {
                            log(event.message);
                            openLog.visible = true;
                            show(event.message + "\nChoose main.pak to try again.");
                            importButton.visible = true;
                        }
                        break;
                    }
                }
            };
            sys.thread.Thread.create(function()
            {
                try
                {
                    queue.add({kind: 0, message: "Verifying GOTY main.pak..."});
                    var verified = GotyActivation.verify(path);
                    var count = PakImporter.extract(verified.archive, Path.join([baseDir, "game-data"]),
                        message -> queue.add({kind: 0, message: message}));
                    GotyActivation.activate(verified.hash);
                    queue.add({kind: 1, message: count == 0 ? "Activated existing GOTY assets" : "Imported " + count + " loose asset files"});
                }
                catch (error:Dynamic) queue.add({kind: 2, message: Std.string(error)});
            });
        });
        if (!importDialog.browse(FileDialogType.OPEN, "pak", Path.join([baseDir, "game-data/main.pak"]), "Choose Plants vs. Zombies GOTY main.pak"))
        {
            importing = false;
            show("The file picker is unavailable. Choose main.pak to retry.");
            importButton.visible = true;
        }
    }
    function initializeNative(dataDir:String):Void
    {
        if (closed) return;
        try
        {
            log("BEGIN pvz_init");
            var initResult = jit.call("pvz_init", [dataDir]);
            NativeDiagnostics.install(Path.join([baseDir, "openpvz-native-crash"]));
            if (initResult != 1) throw Std.string(jit.call("pvz_error"));
            running = true;
            lastTime = sampleStart = Timer.stamp();
            log(Std.string(jit.call("pvz_status")));
            overlay.visible = false;
            openLog.visible = false;
            playStartupIntro();
            try versionCheck = new VersionCheck(Lib.application.meta.get("version")) catch (_:Dynamic) {}
        }
        catch (e:Dynamic) fail(Std.string(e));
    }
    static function nativeCompileWorkerLimit():Int
    {
        try
        {
            var cores = SysInfo.getCPUCoreCount();
            var threads = SysInfo.getCPUThreadCount();
            var coreIsEqualToThreads = cores == threads;
            return cores > 0 && threads > 0 ? coreIsEqualToThreads ? threads : cores >= 8 ? cores : threads : throw 2;
        }
        catch (i:Int) 
        {
            return i;
        }
        catch (e:Dynamic)
        {
            return 2;
        }
    }

    function tick(context:RenderContext):Void
    {
        if (!running || closed) return;
        if (intro != null)
        {
            try intro.update() catch (e:Dynamic)
            {
                finishStartupIntro();
            }
            lastTime = Timer.stamp();
            return;
        }
        var now = Timer.stamp();
        var dt = Math.min(now - lastTime, 0.1);
        lastTime = now;
        try
        {
            var result:Int = cast frameFn.call([dt]);
            if (result == 0) throw Std.string(jit.call("pvz_error"));
            if (result == 2)
            {
                running = false;
                Lib.application.window.close();
                return;
            }
            if (result == 3) return;
            if (versionCheck != null)
                versionCheck.update(function() return jit.call("pvz_update_warning") == 1);
            var requestedSize:Int = cast windowSizeFn.call();
            if (requestedSize != windowSizeIndex && requestedSize >= 0 && requestedSize < WINDOW_SIZES.length)
            {
                windowSizeIndex = requestedSize;
                applyWindowSize();
                try
                {
                    if (displaySettings != null)
                    {
                        displaySettings.data.windowSize = windowSizeIndex;
                        displaySettings.flush();
                    }
                }
                catch (_:Dynamic) {}
            }
            var requestedSharpness:Int = cast sharpnessFn.call();
            if (requestedSharpness != sharpness)
            {
                sharpness = requestedSharpness;
                applySharpness();
                sharpnessSaveAt = now + 0.5;
            }
            if (sharpnessSaveAt > 0 && now >= sharpnessSaveAt) saveSharpness();
            var requestedLimit:Int = cast fpsLimitFn.call();
            if (requestedLimit != fpsLimit)
            {
                fpsLimit = requestedLimit;
                Lib.application.window.frameRate = fpsLimit;
                try
                {
                    if (displaySettings != null)
                    {
                        displaySettings.data.fpsLimit = fpsLimit;
                        displaySettings.flush();
                    }
                }
                catch (_:Dynamic) {}
            }
            var requestedFlags:Int = cast displayFlagsFn.call();
            if (requestedFlags != displayFlags)
            {
                Lib.application.window.fullscreen = (requestedFlags & 1) != 0;
                displayFlags = requestedFlags;
                vsyncPending = true;
                try
                {
                    if (displaySettings != null)
                    {
                        displaySettings.data.displayFlags = displayFlags & 3;
                        displaySettings.data.customContentEnabled = (displayFlags & CUSTOM_CONTENT_DISABLED) == 0;
                        displaySettings.flush();
                    }
                }
                catch (_:Dynamic) {}
                resize();
            }
            var frameWidth:Int = cast frameWidthFn.call();
            if (!gpuProbeDone) probeGpuSharing();
            if (!gpuSupported || !presentGpuFrame(frameWidth)) presentCpuFrame(frameWidth);
            frameCount++;
            frameMs += (Timer.stamp() - now) * 1000;
            if (now - sampleStart >= 1)
            {
                var scene:String = cast jit.call("pvz_scene");
                if (scene != sceneName) { 
                    sceneName = scene; 
                    log("Scene: " + scene); 
                }

                sampleStart = now;
                frameCount = 0;
                frameMs = 0;
            }
        }
        catch (e:Dynamic) fail(Std.string(e));
    }
    function applySharpness():Void
    {
        if (sharpness == 0)
        {
            screen.shader = null;
            return;
        }
        if (sharpnessShader == null) sharpnessShader = new SharpnessShader();
        sharpnessShader.strength.value = [sharpness / 100.0];
        screen.shader = sharpnessShader;
    }

    function saveSharpness():Void
    {
        sharpnessSaveAt = 0;
        try
        {
            if (displaySettings != null)
            {
                displaySettings.data.sharpness = sharpness;
                displaySettings.flush();
            }
        }
        catch (_:Dynamic) {}
    }

    function presentCpuFrame(width:Int):Void
    {
        if (width != image.width) resizeFrameBuffer(width);
        var pointer:CppPointer = cast pixelsFn.call();
        if (pointer == null || pointer.isNull()) throw Std.string(jit.call("pvz_error"));
        copyPixels(bytes, pointer.address);
        pixels.position = 0;
        image.setPixels(bounds, pixels);
    }

    function presentGpuFrame(width:Int):Bool
    {
        try
        {
            var firstFrame = gpuImage == null || gpuImage.width != width;
            if (firstFrame && gpuImage != null && !releaseGpuOutput())
                throw "could not release the previous frame texture";

            var texture:CppPointer = cast gpuOutputFrameFn.call();
            if (texture == null || texture.isNull()) throw "SDL could not share the live frame texture";

            var height = Std.int(width * H / W);
            var context = stage.context3D;
            if (context == null) throw "OpenFL has no GPU rendering context";
            if (firstFrame)
            {
                var device:CppPointer = cast jit.call("pvz_gpu_probe_device");
                if (device == null || device.isNull()) throw "SDL D3D11 device unavailable";
                var status = DisplayControls.beginGpuOutput(device.address, texture.address);
                if (status != 1) throw gpuFailure(status);

                gpuTexture = context.createRectangleTexture(width, height, Context3DTextureFormat.BGRA, false);
                                                                             
                var blank = new BitmapData(width, height, false, 0);
                gpuTexture.uploadFromBitmapData(blank);
                blank.dispose();
                gpuImage = BitmapData.fromTexture(gpuTexture);
            }

            @:privateAccess context.__bindGLTexture2D(gpuTexture.__textureID);
            var status = DisplayControls.copyGpuOutput(width, height, gpuFlipped);
            if (status != 1) throw gpuFailure(status);
            if (firstFrame)
            {
                screen.bitmapData = gpuImage;
                resize();
                log("GPU sharing active at " + width + "x" + height);
            }
            @:privateAccess screen.__setRenderDirty();
            return true;
        }
        catch (e:Dynamic)
        {
            useCpuFrames(Std.string(e));
            return false;
        }
    }

    function releaseGpuOutput():Bool
    {
        if (!gpuOutputCleanupFailed)
        {
            gpuOutputCleanupFailed = !DisplayControls.endGpuOutput();
            if (!gpuOutputCleanupFailed && gpuOutputEndFn != null)
                try gpuOutputEndFn.call() catch (_:Dynamic) gpuOutputCleanupFailed = true;
        }
        if (gpuImage != null)
        {
            screen.bitmapData = image;
            gpuImage.dispose();
            gpuImage = null;
        }
        gpuTexture = null;
        return !gpuOutputCleanupFailed;
    }

    function useCpuFrames(reason:String):Void
    {
        gpuSupported = false;
        if (!releaseGpuOutput()) reason += " (interop cleanup failed)";
        resize();
        log("CPU frame readback active: " + reason);
    }

    function resizeFrameBuffer(width:Int):Void
    {
        var height = Std.int(width * H / W);
        var previous = image;
        image = new BitmapData(width, height, false, 0);
        screen.bitmapData = image;
        previous.dispose();
        bounds = new Rectangle(0, 0, width, height);
        bytes = Bytes.alloc(width * height * 4);
        pixels = ByteArray.fromBytes(bytes);
        pixels.endian = Endian.BIG_ENDIAN;
        resize();
    }
    static function copyPixels(buffer:Bytes, address:Float):Void
    {
        untyped __cpp__("std::memcpy({0}->GetBase(), reinterpret_cast<const void*>(static_cast<uintptr_t>({1})), {2})", buffer.getData(), address, buffer.length);
    }
    function sendMouse(kind:Int, button:Int):Void
    {
        if (intro != null)
        {
            if (kind == 1) intro.skip();
            return;
        }
        if (!running || mouseFn == null) return;
        var x = Math.floor((stage.mouseX - screen.x) / viewScale);
        var y = Math.floor((stage.mouseY - screen.y) / viewScale);
        if (kind == 1 && (x < 0 || y < 0 || x >= W || y >= H)) return;
        try
        {
            mouseFn.call([kind, x, y, button]);
            if (kind == 1) buttons.set(button, true);
            if (kind == 2) buttons.remove(button);
        }
        catch (e:Dynamic) fail(Std.string(e));
    }
    function mouseMove(event:MouseEvent):Void { sendMouse(0, 0); }
    function mouseDown(event:MouseEvent):Void { sendMouse(1, 1); }
    function mouseUp(event:MouseEvent):Void { sendMouse(2, 1); }
    function rightDown(event:MouseEvent):Void { sendMouse(1, -1); }
    function rightUp(event:MouseEvent):Void { sendMouse(2, -1); }
    function wheel(event:MouseEvent):Void { sendMouse(3, event.delta); }
    function leave(event:Event):Void { releaseButtons(); }
    function releaseButtons():Void
    {
        var held = [for (button in buttons.keys()) button];
        for (button in held) sendMouse(2, button);
        buttons.clear();
    }
    function keyDown(event:KeyboardEvent):Void
    {
        if (intro != null) { intro.skip(); return; }
        if (!running || keyFn == null) return;
        try
        {
            keyFn.call([1, event.keyCode, event.charCode]);
            keys.set(event.keyCode, true);
        }
        catch (e:Dynamic) fail(Std.string(e));
    }
    function keyUp(event:KeyboardEvent):Void
    {
        if (intro != null) return;
        if (!running || keyFn == null) return;
        try { keyFn.call([2, event.keyCode, 0]); keys.remove(event.keyCode); }
        catch (e:Dynamic) fail(Std.string(e));
    }
    function deactivate(event:Event):Void
    {
        if (!running || focusFn == null) return;
        try
        {
            releaseButtons();
            for (key in keys.keys()) keyFn.call([2, key, 0]);
            keys.clear();
            focusFn.call([false]);
        }
        catch (e:Dynamic) fail(Std.string(e));
    }
    function activate(event:Event):Void
    {
        vsyncPending = true;
        lastTime = Timer.stamp();
        if (running && focusFn != null)
            try focusFn.call([true]) catch (e:Dynamic) fail(Std.string(e));
    }
    function applyWindowSize():Void
    {
        var size = WINDOW_SIZES[windowSizeIndex];
        Lib.application.window.fullscreen = false;
        if (running) jit.call("pvz_set_display_flags", [displayFlags & 6]);
        Lib.application.window.resize(size[0], size[1]);
        resize();
    }
    function resize(?event:Event):Void
    {
                                                                                
        viewScale = Math.max(0.001, Math.min(stage.stageWidth / W, stage.stageHeight / H));
        screen.scaleX = screen.scaleY = viewScale * W / screen.bitmapData.width;
        screen.x = (stage.stageWidth - W * viewScale) / 2;
        screen.y = (stage.stageHeight - H * viewScale) / 2;
        if (intro != null)
        {
            intro.x = screen.x;
            intro.y = screen.y;
            intro.scaleX = intro.scaleY = viewScale;
        }
        vsyncPending = true;
        overlay.x = screen.x;
        overlay.y = screen.y;
        overlay.scaleX = overlay.scaleY = viewScale;
    }
    function probeGpuSharing():Void
    {
        gpuProbeDone = true;
        var failure:String = null;
        try
        {
            var device:CppPointer = cast jit.call("pvz_gpu_probe_device");
            var red:CppPointer = cast jit.call("pvz_gpu_probe_frame", [0]);
            if (device == null || device.isNull() || red == null || red.isNull())
                throw "SDL did not expose a D3D11 device and test texture";

            var status = DisplayControls.beginGpuProbe(device.address, red.address);
            if (status != 1) throw gpuFailure(status);
            var first = DisplayControls.checkGpuProbe(false);
            if (first < 0) throw gpuFailure(first);
            var green:CppPointer = cast jit.call("pvz_gpu_probe_frame", [1]);
            if (green == null || green.isNull() || green.address != red.address)
                throw "could not update the same D3D11 texture";
            status = DisplayControls.checkGpuProbe(true);
            if (status < 0) throw gpuFailure(status);
            if (status != first) throw "shared texture orientation changed";
            gpuFlipped = first == 2;
        }
        catch (e:Dynamic) failure = Std.string(e);

        if (DisplayControls.endGpuProbe())
        {
            try jit.call("pvz_gpu_probe_end") catch (e:Dynamic) failure = Std.string(e);
        }
        else
            failure = "probe interop cleanup failed";

        gpuSupported = failure == null;
        if (!gpuSupported)
            log("CPU frame readback active: " + failure);
    }

    static function gpuFailure(status:Int):String
    {
        return switch (status)
        {
            case -1: "visible window has no current desktop WGL context";
            case -2: "driver did not expose WGL Direct3D interop entry points";
            case -3: "WGL rejected the D3D11 device";
            case -4: "WGL could not register the SDL D3D11 texture";
            case -5: "WGL could not lock the shared texture";
            case -6: "OpenGL texture dimensions or pixel readback state were unsuitable";
            case -7: "OpenGL did not see the expected color update";
            case -8: "WGL could not unlock the shared texture";
            case -9: "OpenGL framebuffer transfers are unavailable";
            case -10: "OpenGL framebuffers or output texture are incomplete";
            case -11: "OpenGL rejected the GPU frame transfer";
            default: "unknown GPU sharing result " + status;
        };
    }
    function applyVSync(context:RenderContext):Void
    {
                                                                             
        if (!vsyncPending || logPath == null) return;
        vsyncPending = false;
        var enabled = (displayFlags & 2) != 0;
        if (!DisplayControls.setVSync(enabled))
            log("VSync failed: " + (enabled ? "on" : "off"));
    }
    function show(message:String):Void
    {
        currentStatus = message;
        paintStatus(message);
        overlay.visible = true;
        setChildIndex(overlay, numChildren - 1);
        resize();
    }
    function loadStatusAssets():Void
    {
        var root = Path.join([baseDir, "game-data", PakImporter.DIRECTORY]);
        if (bootFont == null)
        {
            bootFont = BootFont.load(root, log);
            if (bootFont != null) log("Startup font loaded: " + bootFont.descriptor + " (" + bootFont.glyphCount + " glyphs)");
        }
        if (stoneButton.length == 0)
        {
            for (piece in ["left", "middle", "right"])
                stoneButton.push(BootFont.loadImage(Path.join([root, "images", "button_" + piece])));
            if (stoneButton.indexOf(null) >= 0)
            {
                for (piece in stoneButton) if (piece != null) piece.dispose();
                stoneButton = [];
            }
        }
    }

    function statusButton(action:Void->Void):Sprite
    {
        var button = new Sprite();
        button.buttonMode = button.useHandCursor = true;
        button.mouseChildren = false;
        button.visible = false;
        button.addEventListener(MouseEvent.CLICK, function(_) action());
        overlay.addChild(button);
        return button;
    }

    function statusLabel(parent:Sprite, value:String, width:Int, maxHeight:Int, size:Int):Int
    {
        if (bootFont != null) return bootFont.print(parent, value, width, maxHeight, size);
        var label = new TextField();
        label.defaultTextFormat = new TextFormat("_serif", size, 0xF2D17D, true);
        label.multiline = label.wordWrap = true;
        label.width = width;
        label.text = value;
        label.height = Math.min(maxHeight, label.textHeight + 6);
        parent.addChild(label);
        return Std.int(label.height);
    }

    function paintButton(button:Sprite, title:String):Void
    {
        button.removeChildren();
        button.graphics.clear();
        button.graphics.lineStyle(3, 0x8E869E);
        button.graphics.beginFill(0x3D3C50);
        button.graphics.drawRect(0, 0, 215, 48);
        button.graphics.endFill();
        if (stoneButton.length == 3)
        {
            for (i in 0...3)
            {
                var piece = new Bitmap(stoneButton[i]);
                piece.x = i == 0 ? 0 : i == 1 ? 22 : 193;
                piece.width = i == 1 ? 171 : 22;
                piece.height = 48;
                piece.smoothing = true;
                button.addChild(piece);
            }
        }
        var label = new Sprite();
        statusLabel(label, title, 191, 36, 24);
        label.x = (215 - label.width) / 2;
        label.y = (48 - label.height) / 2;
        button.addChild(label);
    }

    function paintStatus(message:String):Void
    {
        statusText.removeChildren();
        var height = statusLabel(statusText, message, 862, 370, 28);
        var panelHeight = Std.int(Math.max(134, height + 112));
        var x = 180;
        var y = Std.int((H - panelHeight) / 2);
        overlay.graphics.clear();
        overlay.graphics.lineStyle(6, 0x766D89);
        overlay.graphics.beginFill(0x252534);
        overlay.graphics.drawRect(x, y, 920, panelHeight);
        overlay.graphics.endFill();
        statusText.x = x + 29;
        statusText.y = y + 25;
        paintButton(importButton, "Choose main.pak");
        paintButton(openLog, "Open Log");
        importButton.x = x + 29;
        openLog.x = failed ? x + 29 : x + 260;
        importButton.y = openLog.y = y + panelHeight - 69;
    }

    static function errorSummary(message:String):String
    {
        var diagnostic = ~/^(.*:[0-9]+(?::[0-9]+)?):[ \t]*(?:fatal )?error:[ \t]*(.*)$/i;
        var msvc = ~/^(.*\([0-9]+(?:,[0-9]+)?\)):[ \t]*(?:fatal )?error[ \t]+([A-Z][0-9]+:[ \t]*.*)$/i;
        var lines = message.split("\n");
        for (i in 0...lines.length)
        {
            var line = StringTools.trim(lines[i]);
            var pattern = diagnostic.match(line) ? diagnostic : msvc.match(line) ? msvc : null;
            if (pattern == null) continue;
            var location = pattern.matched(1);
            var detail = pattern.matched(2);
            if (detail == "" && i + 1 < lines.length) detail = StringTools.trim(lines[i + 1]);
            return "C++ compiler error\n" + detail + "\n" + location;
        }
        return "Error\n" + StringTools.trim(message);
    }

    function log(message:String):Void
    {
        trace(message);
        if (StringTools.startsWith(message, "Scene: "))
            NativeDiagnostics.setContext(startupPhase + "\n" + message);
        else if (!running && !StringTools.startsWith(message, "ERROR:"))
        {
            startupPhase = message;
            NativeDiagnostics.setContext(startupPhase);
        }
        try
        {
            var output = File.append(logPath, false);
            output.writeString(Date.now().toString() + " | " + message + "\n");
            output.close();
        }
        catch (_:Dynamic) {}
    }
    function fail(message:String):Void
    {
        running = false;
        failed = true;
        log("ERROR: " + message);
        importButton.visible = false;
        openLog.visible = true;
        show(errorSummary(message));
    }
    function shutdown(code:Int):Void
    {
        if (versionCheck != null) { versionCheck.dispose(); versionCheck = null; }
        closed = true;
        if (sharpnessSaveAt > 0) saveSharpness();
        finishStartupIntro();
        running = false;
        if (jit != null)
        {
            if (gpuTexture != null) releaseGpuOutput();
            try jit.call("pvz_shutdown") catch (e:Dynamic) log("Native shutdown: " + Std.string(e));
            jit.dispose();
            jit = null;
        }
        image.dispose();
        if (bootFont != null) bootFont.dispose();
        for (piece in stoneButton) piece.dispose();
        log("Shutdown complete (exit " + code + ")");
    }
}