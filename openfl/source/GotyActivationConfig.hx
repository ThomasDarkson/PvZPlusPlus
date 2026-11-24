package;

class GotyActivationConfig
{
    public static macro function configuredHashes():haxe.macro.Expr
    {
        var value = haxe.macro.Context.definedValue("PVZ_GOTY_SHA256");
        var hashes:Array<String> = [];
        if (value != null)
        {
            for (part in value.split(","))
            {
                var hash = StringTools.trim(part).toLowerCase();
                if (!~/^[0-9a-f]{64}$/.match(hash))
                    haxe.macro.Context.fatalError("PVZ_GOTY_SHA256 must contain comma-separated SHA-256 hex digests.", haxe.macro.Context.currentPos());
                if (!hashes.contains(hash)) hashes.push(hash);
            }
        }
        return macro $v{hashes};
    }
}
