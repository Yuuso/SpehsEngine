
#ifndef SE_SHADER_HEADER_GUARD
#define SE_SHADER_HEADER_GUARD

#include "bgfx_shader.sh"

uniform mat4 u_normal[BGFX_CONFIG_MAX_BONES];
uniform vec4 u_primitiveColor;
uniform vec4 u_billboardInfo;

vec3 getViewPosition()
{
#if BGFX_SHADER_LANGUAGE_GLSL
	return vec3(u_invView[3][0], u_invView[3][1], u_invView[3][2]);
#else
	return vec3(u_invView[0][3], u_invView[1][3], u_invView[2][3]);
#endif
}

vec3 getViewUp()
{
#if BGFX_SHADER_LANGUAGE_GLSL
	return vec3(u_view[0][1], u_view[1][1], u_view[2][1]);
#else
	return vec3(u_view[1][0], u_view[1][1], u_view[1][2]);
#endif
}

mat3 mtx3FromMtx4(mat4 _mtx)
{
#if BGFX_SHADER_LANGUAGE_GLSL
    return mtxFromCols3(_mtx[0].xyz, _mtx[1].xyz, _mtx[2].xyz);
#else
    return mtxFromRows3(_mtx[0].xyz, _mtx[1].xyz, _mtx[2].xyz);
#endif
}

mat4 mtx4FromMtx3(mat3 _mtx)
{
#if BGFX_SHADER_LANGUAGE_GLSL
    return mtxFromCols4(vec4(_mtx[0].xyz, 0.0), vec4(_mtx[1].xyz, 0.0), vec4(_mtx[2].xyz, 0.0), vec4(0.0, 0.0, 0.0, 1.0));
#else
    return mtxFromRows4(vec4(_mtx[0].xyz, 0.0), vec4(_mtx[1].xyz, 0.0), vec4(_mtx[2].xyz, 0.0), vec4(0.0, 0.0, 0.0, 1.0));
#endif
}

#endif // SE_SHADER_HEADER_GUARD
