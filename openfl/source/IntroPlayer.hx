package;

import haxe.Timer;
import haxe.io.Path;
import haxe.zip.Uncompress;
import openfl.display.Bitmap;
import openfl.display.BitmapData;
import openfl.display.Sprite;
import openfl.geom.Rectangle;
import openfl.media.Sound;
import openfl.media.SoundChannel;
import openfl.events.Event;
import openfl.utils.ByteArray;
import openfl.utils.Endian;
import sys.io.File;
import sys.io.FileInput;

                                                                                 
                                                                          
class IntroPlayer extends Sprite
{
    var input:FileInput;
    var picture:BitmapData;
    var channel:SoundChannel;
    var sound:Sound;
    var started:Float;
    var fps:Float;
    var duration:Float;
    var frameCount:Int;
    var frameIndex:Int = -1;
    var done:Bool = false;
    var audioEnded:Bool = false;
    var complete:Void->Void;

    public function new(folder:String, onComplete:Void->Void)
    {
        super();
        complete = onComplete;
        try
        {
            var meta:Dynamic = haxe.Json.parse(File.getContent(Path.join([folder, "intro.json"])));
            fps = meta.fps;
            duration = meta.duration;
            frameCount = Std.int(meta.frames);
            if (Std.int(meta.width) != 1280 || Std.int(meta.height) != 720 || fps <= 0 || frameCount <= 0)
                throw "Invalid startup intro metadata";
            input = File.read(Path.join([folder, "frames.pvzi"]), true);
            input.bigEndian = false;
            if (input.readString(5) != "PVZI1") throw "Invalid startup intro frames";
            picture = new BitmapData(1280, 720, false, 0xffffff);
            var bitmap = new Bitmap(picture);
            bitmap.smoothing = true;
            addChild(bitmap);
            readFrame();
            try sound = Sound.fromFile(Path.join([folder, "audio.wav"])) catch (_:Dynamic) sound = null;
            channel = sound != null ? sound.play() : null;
            if (channel != null) channel.addEventListener(Event.SOUND_COMPLETE, onAudioEnded);
            started = Timer.stamp();
        }
        catch (e:Dynamic)
        {
            dispose();
            throw e;
        }
    }

    function onAudioEnded(event:Event):Void
    {
        audioEnded = true;
    }

    function readFrame():Void
    {
        var x = input.readUInt16();
        var y = input.readUInt16();
        var w = input.readUInt16();
        var h = input.readUInt16();
        var length = input.readInt32();
        if (length < 0 || length > 16777216 || x + w > 1280 || y + h > 720)
            throw "Invalid startup intro frame";
        if (length > 0)
        {
            if (w <= 0 || h <= 0) throw "Invalid startup intro dimensions";
            var expected = w * h * 4;
                                                                             
                                                                                
            var raw = Uncompress.run(input.read(length), expected + 1);
            if (raw.length != expected) throw "Invalid startup intro pixels";
            var pixels = ByteArray.fromBytes(raw);
            pixels.endian = Endian.BIG_ENDIAN;
            picture.setPixels(new Rectangle(x, y, w, h), pixels);
        }
        frameIndex++;
    }

    public function update():Void
    {
        if (done) return;
        var elapsed = Timer.stamp() - started;
        var clock = channel != null && !audioEnded && channel.position > 0 ? channel.position / 1000 : elapsed;
        var wanted = Std.int(Math.min(frameCount - 1, Math.floor(clock * fps)));
                                                                                      
        while (frameIndex < wanted) readFrame();
        if (clock >= duration || (audioEnded && frameIndex == frameCount - 1) || elapsed >= duration + 1)
            skip();
    }

    public function skip():Void
    {
        if (done) return;
        done = true;
        if (complete != null) complete();
    }

    public function dispose():Void
    {
        done = true;
        if (channel != null)
        {
            channel.removeEventListener(Event.SOUND_COMPLETE, onAudioEnded);
            channel.stop();
            channel = null;
        }
        if (sound != null)
        {
            try sound.close() catch (_:Dynamic) {}
            sound = null;
        }
        if (input != null)
        {
            input.close();
            input = null;
        }
        if (picture != null)
        {
            picture.dispose();
            picture = null;
        }
        complete = null;
    }
}
