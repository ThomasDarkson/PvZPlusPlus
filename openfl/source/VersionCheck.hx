package;

import haxe.Timer;
import openfl.events.Event;
import openfl.events.IOErrorEvent;
import openfl.events.SecurityErrorEvent;
import openfl.net.SharedObject;
import openfl.net.URLLoader;
import openfl.net.URLRequest;
import StringTools;

private typedef ReleaseVersion =
{
    var core:Array<Int>;
    var prerelease:Array<String>;
    var key:String;
}

class VersionCheck
{
    public static inline var URL = "https://thomasdarkson.com/pvzpp/version.txt";
    var pending:String;
    var history:SharedObject;
    var notified:Array<String> = [];
    var nextPrompt = 0.0;
    var loader:URLLoader;
    var timeout:Timer;
    var stopped = false;

    public function new(currentVersion:String)
    {
        var current = parseVersion(currentVersion);
        if (current == null) return;
        try
        {
            history = SharedObject.getLocal("pvzpp-update-notifications");
            var saved:Dynamic = history.data.notifiedVersions;
            if (Std.isOfType(saved, Array))
                for (value in (cast saved:Array<Dynamic>))
                    if (Std.isOfType(value, String)) notified.push(cast value);
        }
        catch (_:Dynamic) {}

        loader = new URLLoader();
        loader.addEventListener(Event.COMPLETE, function(_:Event)
        {
            if (stopped) return;
            var body = Std.string(loader.data);
            finishRequest();
            var latest = parseVersion(body);
            if (latest != null && compare(latest, current) > 0 && notified.indexOf(latest.key) < 0)
                pending = latest.key;
        });
        loader.addEventListener(IOErrorEvent.IO_ERROR, function(_:IOErrorEvent) finishRequest());
        loader.addEventListener(SecurityErrorEvent.SECURITY_ERROR, function(_:SecurityErrorEvent) finishRequest());
        timeout = Timer.delay(finishRequest, 10000);
        try loader.load(new URLRequest(URL)) catch (_:Dynamic) finishRequest();
    }

    function finishRequest():Void
    {
        stopped = true;
        if (timeout != null) { timeout.stop(); timeout = null; }
        if (loader != null)
        {
            try loader.close() catch (_:Dynamic) {}
            loader = null;
        }
    }

    public function update(showWarning:Void->Bool):Void
    {
        if (pending == null || Timer.stamp() < nextPrompt) return;
        nextPrompt = Timer.stamp() + 1;
        var shown = false;
        try shown = showWarning() catch (_:Dynamic) { pending = null; return; }
        if (!shown) return;
        notified.push(pending);
        pending = null;
        try
        {
            if (history != null)
            {
                history.data.notifiedVersions = notified;
                history.flush();
            }
        }
        catch (_:Dynamic) {}
    }

    public function dispose():Void
    {
        pending = null;
        finishRequest();
    }

    static function parseVersion(text:String):ReleaseVersion
    {
        if (text == null || text.length > 256) return null;
        text = StringTools.trim(text);
        if (text.charCodeAt(0) == 0xfeff) text = StringTools.trim(text.substr(1));
        var pattern = ~/^v?(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)(?:-([0-9A-Za-z-]+(?:\.[0-9A-Za-z-]+)*))?(?:\+([0-9A-Za-z-]+(?:\.[0-9A-Za-z-]+)*))?$/;
        if (!pattern.match(text)) return null;
        var core:Array<Int> = [];
        for (i in 1...4)
        {
            var value = pattern.matched(i);
            if (value.length > 9) return null;
            core.push(Std.parseInt(value));
        }
        var suffix = pattern.matched(4);
        var prerelease = suffix == null ? [] : suffix.split(".");
        for (id in prerelease)
            if (~/^[0-9]+$/.match(id) && id.length > 1 && id.charAt(0) == "0") return null;
        return {core: core, prerelease: prerelease,
            key: core.join(".") + (prerelease.length == 0 ? "" : "-" + prerelease.join("."))};
    }

    static function compare(a:ReleaseVersion, b:ReleaseVersion):Int
    {
        for (i in 0...3)
            if (a.core[i] != b.core[i]) return a.core[i] > b.core[i] ? 1 : -1;
        if (a.prerelease.length == 0 || b.prerelease.length == 0)
            return a.prerelease.length == b.prerelease.length ? 0 : a.prerelease.length == 0 ? 1 : -1;
        for (i in 0...Std.int(Math.min(a.prerelease.length, b.prerelease.length)))
        {
            var x = a.prerelease[i], y = b.prerelease[i];
            if (x == y) continue;
            var nx = ~/^[0-9]+$/.match(x), ny = ~/^[0-9]+$/.match(y);
            if (nx && ny && x.length != y.length) return x.length > y.length ? 1 : -1;
            if (nx != ny) return nx ? -1 : 1;
            return Reflect.compare(x, y);
        }
        return Reflect.compare(a.prerelease.length, b.prerelease.length);
    }
}
