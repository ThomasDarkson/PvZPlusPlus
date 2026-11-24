package;

import haxe.Json;
import haxe.io.Bytes;
import haxe.io.Path;
import sys.FileSystem;
import sys.io.File;
import haxe.io.BytesInput;

private typedef PakEntry = {var path:String; var size:Int;}

                                                                         
class PakImporter
{
    public static inline var DIRECTORY = "imported-assets";
    static inline var RECEIPT = ".import-complete.json";
    public static function assetDirectory(root:String):String return Path.join([root, DIRECTORY]);

    public static function isReady(root:String):Bool
    {
        var folder = assetDirectory(root);
        try
        {
            var receipt:Dynamic = Json.parse(File.getContent(Path.join([folder, RECEIPT])));
            return receipt.format == 1 && receipt.files > 0
                && FileSystem.exists(Path.join([folder, "properties/resources.xml"]))
                && FileSystem.isDirectory(Path.join([folder, "images"]));
        }
        catch (_:Dynamic) return false;
    }

    static function byte(input:BytesInput):Int return input.readByte() ^ 0xf7;
    static function integer(input:BytesInput):Int
    {
        var a = byte(input);
        var b = byte(input);
        var c = byte(input);
        var d = byte(input);
        return a | (b << 8) | (c << 16) | (d << 24);
    }

    static function safePath(name:String):String
    {
        name = StringTools.replace(name, "\\", "/");
        if (name.length == 0 || StringTools.startsWith(name, "/")) throw "Invalid PAK filename";
        for (part in name.split("/"))
        {
            if (part == "" || part == "." || part == ".." || StringTools.endsWith(part, ".")
                || StringTools.endsWith(part, " ") || ~/[\x00-\x1f<>:"|?*]/.match(part)
                || ~/^(con|prn|aux|nul|com[1-9]|lpt[1-9])(\.|$)/i.match(part))
                throw "Unsafe PAK filename: " + name;
        }
                                                                              
                                                                              
        return name.toLowerCase();
    }

    static function removeTree(path:String):Void
    {
        if (!FileSystem.exists(path)) return;
        if (FileSystem.isDirectory(path))
        {
            for (name in FileSystem.readDirectory(path)) removeTree(Path.join([path, name]));
            FileSystem.deleteDirectory(path);
        }
        else FileSystem.deleteFile(path);
    }

    public static function extract(pak:Bytes, root:String, progress:String->Void):Int
    {
        if (isReady(root)) return 0;
        FileSystem.createDirectory(root);
        var stage = Path.join([root, ".pvz-import-staging"]);
        var destination = assetDirectory(root);
        var input = new BytesInput(pak);
        var stageCreated = false;
        try
        {
            var sourceSize = pak.length;
            if (sourceSize < 9 || integer(input) != 0xbac04ac0 || integer(input) != 0)
                throw "This is not a supported PopCap main.pak (version 0)";
            var entries:Array<PakEntry> = [];
            var names = new Map<String, Bool>();
            var total = 0.0;
            while (true)
            {
                var flags = byte(input);
                if (flags == 0x80) break;
                if (flags != 0 || entries.length >= 100000) throw "Unsupported or invalid PAK directory";
                var length = byte(input);
                var name = new StringBuf();
                for (_ in 0...length) name.addChar(byte(input));
                var path = safePath(name.toString());
                var size = integer(input);
                for (_ in 0...8) byte(input);                                              
                if (size < 0 || names.exists(path)) throw "Invalid or duplicate PAK entry: " + path;
                names.set(path, true);
                entries.push({path: path, size: size});
                total += size;
                if (total > sourceSize) throw "PAK payload exceeds its file size";
            }
            if (!names.exists("properties/resources.xml") || !Lambda.exists(entries, e -> StringTools.startsWith(e.path, "images/")))
                throw "This PAK does not contain the expected GOTY resources and images";
            if (total > sourceSize - input.position) throw "The main.pak payload is truncated";
            for (entry in entries)
            {
                var parent = Path.directory(entry.path);
                while (parent != "")
                {
                    if (names.exists(parent)) throw "Conflicting PAK file and directory: " + parent;
                    parent = Path.directory(parent);
                }
            }
                                                                                            
            removeTree(stage);
            FileSystem.createDirectory(stage);
            stageCreated = true;
            var buffer = Bytes.alloc(64 * 1024);
            var completed = 0;
            for (entry in entries)
            {
                var target = Path.join([stage, entry.path]);
                FileSystem.createDirectory(Path.directory(target));
                var output = File.write(target, true);
                try
                {
                    var remaining = entry.size;
                    while (remaining > 0)
                    {
                        var count = Std.int(Math.min(buffer.length, remaining));
                        input.readFullBytes(buffer, 0, count);
                        for (i in 0...count) buffer.set(i, buffer.get(i) ^ 0xf7);
                        output.writeFullBytes(buffer, 0, count);
                        remaining -= count;
                    }
                    output.close();
                }
                catch (e:Dynamic) { output.close(); throw e; }
                completed++;
                if (completed % 25 == 0 || completed == entries.length)
                    progress("Extracting assets: " + completed + " / " + entries.length);
            }
            input.close();
                                                                                   
            File.saveContent(Path.join([stage, RECEIPT]), Json.stringify({format: 1, files: completed}, null, "  "));
            if (FileSystem.exists(destination))
                FileSystem.rename(destination, destination + ".incomplete-" + Std.string(Date.now().getTime()));
            FileSystem.rename(stage, destination);
            stageCreated = false;
            return completed;
        }
        catch (e:Dynamic)
        {
            try input.close() catch (_:Dynamic) {}
            if (stageCreated) try removeTree(stage) catch (_:Dynamic) {}
            throw "Asset import failed: " + Std.string(e);
        }
    }
}
