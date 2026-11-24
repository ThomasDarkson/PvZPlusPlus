package;

import haxe.io.Path;
import haxe.io.BytesBuffer;
import openfl.display.Bitmap;
import openfl.display.BitmapData;
import openfl.display.Sprite;
import openfl.geom.Point;
import openfl.geom.Rectangle;
import sys.FileSystem;
import sys.io.File;

private enum FontValue
{
    Atom(value:String);
    Group(values:Array<FontValue>);
}

class BootFont
{
    final glyphs:Map<String, BitmapData> = new Map();
    final advance:Map<String, Int> = new Map();
    var height = 0;

    static function atom(value:FontValue):String
    {
        return switch value { case Atom(text): text; case Group(_): ""; };
    }

    static function list(value:FontValue, definitions:Map<String, FontValue>):Array<FontValue>
    {
        return switch value
        {
            case Group(values): values;
            case Atom(name):
                switch definitions.get(name.toUpperCase()) { case Group(values): values; default: []; }
        };
    }

    public final descriptor:String;
    public var glyphCount(get, never):Int;

    function get_glyphCount():Int
    {
        var total = 0;
        for (_ in glyphs) ++total;
        return total;
    }

    static function number(value:FontValue):Null<Int>
    {
        return Std.parseInt(atom(value));
    }

    static function makeSpans(command:Array<FontValue>, definitions:Map<String, FontValue>):FontValue
    {
        var rect = list(command[2], definitions).map(number);
        var widths = list(command[3], definitions).map(number);
        if (rect.length != 4 || rect.indexOf(null) >= 0 || widths.length == 0 || widths.indexOf(null) >= 0)
            throw "Invalid CreateHorzSpanRectList for " + atom(command[1]);
        var x:Int = rect[0];
        var spans:Array<FontValue> = [];
        for (width in widths)
        {
            spans.push(Group([Atom(Std.string(x)), Atom(Std.string(rect[1])), Atom(Std.string(width)), Atom(Std.string(rect[3]))]));
            x += width;
        }
        return Group(spans);
    }

    public static function load(root:String, report:String->Void):BootFont
    {
        var candidates = ["data/dwarventodcraft18yellow.txt", "data/dwarventodcraft18.txt",
            "data/dwarventodcraft15.txt", "data/dwarventodcraft12.txt"];
        var manifest = Path.join([root, "properties/resources.xml"]);
        try
        {
            var document = Xml.parse(File.getContent(manifest)).firstElement();
            for (group in document.elementsNamed("Resources"))
            {
                var folder = "";
                for (entry in group.elements())
                {
                    if (entry.nodeName == "SetDefaults")
                    {
                        if (entry.exists("path")) folder = entry.get("path");
                        continue;
                    }
                    if (entry.nodeName != "Font" || !entry.exists("path")) continue;
                    var path = entry.get("path").toLowerCase();
                    if (path.indexOf("dwarventodcraft") < 0 || !StringTools.endsWith(path, ".txt")) continue;
                    path = Path.join([folder.toLowerCase(), path]);
                    if (candidates.indexOf(path) < 0) candidates.push(path);
                }
            }
        }
        catch (_:Dynamic) {}
        var found = false;
        for (name in candidates)
        {
            var path = Path.join([root, name]);
            if (!FileSystem.exists(path)) continue;
            found = true;
            try
            {
                var font = new BootFont(path);
                if (font.glyphCount >= 26 && (font.glyphs.exists("A") || font.glyphs.exists("a"))) return font;
                report("Startup font " + name + ": found " + font.glyphCount + " usable glyphs");
                font.dispose();
            }
            catch (error:Dynamic) report("Startup font " + name + ": " + Std.string(error));
        }
        if (!found && FileSystem.exists(manifest)) report("Startup font: Dwarven Todcraft descriptors were not found in the imported assets");
        return null;
    }

    static function readFontText(path:String):String
    {
        var raw = File.getBytes(path);
        if (raw.length >= 3 && raw.get(0) == 0xEF && raw.get(1) == 0xBB && raw.get(2) == 0xBF)
            return raw.getString(3, raw.length - 3);

        var utf8 = new BytesBuffer();
        for (i in 0...raw.length)
        {
            var code = raw.get(i);
            if (code < 0x80) utf8.addByte(code);
            else
            {
                utf8.addByte(0xC0 | (code >> 6));
                utf8.addByte(0x80 | (code & 0x3F));
            }
        }
        return utf8.getBytes().toString();
    }

    static function readDescriptor(source:String):Array<Array<FontValue>>
    {
        var stack:Array<Array<FontValue>> = [[]];
        var statements:Array<Array<FontValue>> = [];
        var token:Null<StringBuf> = null;
        var quote = "";
        var escaped = false;
        var lineStart = true;
        var comment = false;
        var listStart = 0;
        var i = source.charCodeAt(0) == 0xFEFF ? 1 : 0;
        while (i < source.length)
        {
            var c = source.charAt(i++);
            if (comment)
            {
                if (c == "\n")
                {
                    comment = false;
                    lineStart = true;
                }
                continue;
            }
            if (escaped || quote != "" || c == "\\" || c == "'" || c == '"')
            {
                if (token == null) token = new StringBuf();
                if (escaped)
                {
                    token.add(c);
                    escaped = false;
                }
                else if (c == "\\") escaped = true;
                else if (quote == "")
                    quote = c;
                else if (c == quote)
                {
                    if (source.charAt(i) == quote)
                    {
                        token.add(c);
                        ++i;
                    }
                    else quote = "";
                }
                else token.add(c);
                lineStart = false;
                continue;
            }
            if (c == "#" && lineStart)
            {
                comment = true;
                continue;
            }
            switch c
            {
                case " ", "\t", "\r", "\n", ",", "(", ")", ";":
                    if (token != null)
                    {
                        stack[stack.length - 1].push(Atom(token.toString()));
                        token = null;
                    }
                    switch c
                    {
                        case "(":
                            if (stack.length == 1) listStart = i - 1;
                            stack.push([]);
                        case ")":
                            if (stack.length < 2) throw "Unexpected ')' at offset " + (i - 1);
                            var group = stack.pop();
                            stack[stack.length - 1].push(Group(group));
                        case ";":
                            if (stack.length != 1) throw "Unclosed bitmap font list at offset " + listStart;
                            statements.push(stack[0]);
                            stack[0] = [];
                        default:
                    }
                    if (c == "\n") lineStart = true;
                    else if (c != " " && c != "\t" && c != "\r") lineStart = false;
                default:
                    if (token == null) token = new StringBuf();
                    token.add(c);
                    lineStart = false;
            }
        }
        if (quote != "" || escaped) throw "Unterminated font descriptor literal at end of file";
        if (stack.length != 1) throw "Unclosed bitmap font list at offset " + listStart;
        if (token != null) stack[0].push(Atom(token.toString()));
        if (stack[0].length > 0) statements.push(stack[0]);
        return statements;
    }

    function new(path:String)
    {
        descriptor = path;
        var definitions:Map<String, FontValue> = new Map();
        var layers:Map<String, String> = new Map();
        var statements = readDescriptor(readFontText(path));
        for (command in statements)
        {
            if (command.length < 3) continue;
            var key = atom(command[1]).toUpperCase();
            switch atom(command[0]).toLowerCase()
            {
                case "define":
                    var alias = definitions.get(atom(command[2]).toUpperCase());
                    definitions.set(key, alias == null ? command[2] : alias);
                case "createhorzspanrectlist":
                    if (command.length == 4) definitions.set(key, makeSpans(command, definitions));
                case "createlayerfrom":
                    var inheritedImage = layers.get(atom(command[2]).toUpperCase());
                    if (inheritedImage != null) layers.set(key, inheritedImage);
                case "layersetimage": layers.set(key, atom(command[2]));
                default:
            }
        }
        var maps = 0;
        for (command in statements)
        {
            if (command.length != 4 || atom(command[0]).toLowerCase() != "layersetimagemap") continue;
            ++maps;
            var layer = atom(command[1]).toUpperCase();
            var image = layers.get(layer);
            if (image == null) throw "Font atlas not declared for layer " + layer;
            if (definitions.exists(image.toUpperCase())) image = atom(definitions.get(image.toUpperCase()));
            var sourcePath = Path.normalize(Path.join([Path.directory(path), StringTools.replace(image, "\\", "/").toLowerCase()]));
            var atlas = loadImage(sourcePath);
            if (atlas == null) throw "Could not decode font atlas " + sourcePath;
            var chars = list(command[2], definitions);
            var rects = list(command[3], definitions);
            if (chars.length == 0 || rects.length != chars.length)
            {
                atlas.dispose();
                throw "Invalid glyph map for layer " + layer + " (" + chars.length + " characters, " + rects.length + " rectangles)";
            }
            for (i in 0...chars.length)
            {
                var values = [for (entry in list(rects[i], definitions)) Std.parseInt(atom(entry))];
                if (values.length != 4 || values.indexOf(null) >= 0 || values[2] <= 0 || values[3] <= 0) continue;
                var char = atom(chars[i]);
                if (char.length != 1 || glyphs.exists(char)) continue;
                var glyph = new BitmapData(values[2], values[3], true, 0);
                glyph.copyPixels(atlas, new Rectangle(values[0], values[1], values[2], values[3]), new Point());
                glyphs.set(char, glyph);
                height = Std.int(Math.max(height, values[3]));
            }
            atlas.dispose();
            if (glyphs.exists("A") || glyphs.exists("a"))
            {
                for (widthCommand in statements)
                {
                    if (widthCommand.length != 4 || atom(widthCommand[0]).toLowerCase() != "layersetcharwidths" ||
                        atom(widthCommand[1]).toUpperCase() != layer) continue;
                    var keys = list(widthCommand[2], definitions);
                    var widths = list(widthCommand[3], definitions);
                    for (i in 0...Std.int(Math.min(keys.length, widths.length)))
                    {
                        var width = Std.parseInt(atom(widths[i]));
                        if (width != null && width > 0) advance.set(atom(keys[i]), width);
                    }
                }
                break;
            }
        }
        if (maps == 0) throw "No LayerSetImageMap commands found";
    }

    public static function loadImage(path:String):BitmapData
    {
        for (candidate in [path, path + ".png", path + ".gif", path + ".jpg"])
        {
            try
            {
                if (!FileSystem.exists(candidate)) continue;
                var bitmap = BitmapData.fromFile(candidate);
                if (bitmap != null) return bitmap;
            }
            catch (_:Dynamic) {}
        }
        return null;
    }

    public function dispose():Void
    {
        for (glyph in glyphs) glyph.dispose();
    }

    public function print(parent:Sprite, text:String, width:Int, limit:Int, size:Int):Int
    {
        var scale = size / height;
        var x = 0.0;
        var y = 0;
        var step = size + 7;
        for (i in 0...text.length)
        {
            var char = text.charAt(i);
            var glyph = glyphs.get(char);
            if (glyph == null && char != " ") glyph = glyphs.get(char.toUpperCase());
            if (glyph == null && char != " ") glyph = glyphs.get(char.toLowerCase());
            if (glyph == null && char != " ") glyph = glyphs.get("?");
            var spacing = advance.exists(char) ? advance.get(char) : (glyph == null ? height / 3 : glyph.width + 1);
            if (char == "\n" || x + spacing * scale > width)
            {
                x = 0;
                y += step;
                if (y + size > limit) break;
                if (char == "\n" || char == " ") continue;
            }
            if (glyph != null)
            {
                var bitmap = new Bitmap(glyph);
                bitmap.x = x;
                bitmap.y = y + (height - glyph.height) * scale;
                bitmap.scaleX = bitmap.scaleY = scale;
                parent.addChild(bitmap);
            }
            x += spacing * scale;
        }
        return y + step;
    }
}
