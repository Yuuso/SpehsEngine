$input v_color0, v_texcoord0

#include "bgfx_shader.sh"

SAMPLER2D(s_fontTex, 0);

void main()
{
    vec4 fontColor = vec4(1.0, 1.0, 1.0, texture2D(s_fontTex, v_texcoord0).x);
    if (fontColor.a == 0.0)
        discard;
	gl_FragColor = v_color0 * fontColor;
}
