
#ifndef SE_SHADER_HEADER_GUARD
#define SE_SHADER_HEADER_GUARD

#include "bgfx_shader.sh"

uniform mat4 u_normal[BGFX_CONFIG_MAX_BONES];
uniform vec4 u_primitiveColor;

vec3 getViewPosition()
{
#if BGFX_SHADER_LANGUAGE_GLSL
	return vec3(u_invView[3][0], u_invView[3][1], u_invView[3][2]);
#else
	return vec3(u_invView[0][3], u_invView[1][3], u_invView[2][3]);
#endif
}

#endif // SE_SHADER_HEADER_GUARD
