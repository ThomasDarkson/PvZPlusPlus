package;

import openfl.display.DisplayObjectShader;

class SharpnessShader extends DisplayObjectShader
{
    @:glFragmentSource('
        #pragma header

        uniform float strength;
        void main(void)
        {
            float radius = max(1.0, openfl_TextureSize.x / 1280.0);
            vec2 texel = radius / openfl_TextureSize;
            vec2 halfPixel = 0.5 / openfl_TextureSize;
            vec2 low = halfPixel;
            vec2 high = vec2(1.0) - low;
            vec2 uv = clamp(openfl_TextureCoordv, low, high);
            vec4 center = texture2D(openfl_Texture, uv);
            vec3 north = texture2D(openfl_Texture, clamp(uv + vec2(0.0, -texel.y), low, high)).rgb;
            vec3 south = texture2D(openfl_Texture, clamp(uv + vec2(0.0, texel.y), low, high)).rgb;
            vec3 west = texture2D(openfl_Texture, clamp(uv + vec2(-texel.x, 0.0), low, high)).rgb;
            vec3 east = texture2D(openfl_Texture, clamp(uv + vec2(texel.x, 0.0), low, high)).rgb;
            vec3 average = (north + south + west + east) * 0.25;
            vec3 detail = clamp(center.rgb - average, vec3(-0.20), vec3(0.20));
            vec3 minimum = min(center.rgb, min(min(north, south), min(west, east)));
            vec3 maximum = max(center.rgb, max(max(north, south), max(west, east)));
            vec3 allowance = (maximum - minimum) * (0.08 * strength);
            vec3 sharpened = clamp(center.rgb + (3.0 * strength) * detail,
                max(vec3(0.0), minimum - allowance), min(vec3(1.0), maximum + allowance));
            
            gl_FragColor = vec4(sharpened, center.a) * openfl_Alphav;
        }
    ')
    public function new()
    {
        super();
        strength.value = [0.0];
    }
}
