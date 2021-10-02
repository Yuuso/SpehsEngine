$input v_texcoord0

#include "se_shader.sh"

SAMPLER2D(s_texColor, 0);

void main()
{
	gl_FragColor = u_primitiveColor * texture2D(s_texColor, v_texcoord0);
}
