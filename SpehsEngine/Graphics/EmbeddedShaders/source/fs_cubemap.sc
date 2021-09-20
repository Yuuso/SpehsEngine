$input v_position

#include "se_shader.sh"

SAMPLERCUBE(s_texColor, 0);

void main()
{
	gl_FragColor = u_primitiveColor * textureCube(s_texColor, v_position);
}
