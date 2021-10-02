$input a_position, a_texcoord0
$output v_position

#include "se_shader.sh"

void main()
{
    mat4 staticView = mtx4FromMtx3(mtx3FromMtx4(u_view));
    mat4 staticViewProj = mul(u_proj, staticView);
    vec4 outPosition = mul(staticViewProj, vec4(a_position, 1.0));
    gl_Position = outPosition.xyww; // Force depth to always be 1.0

    vec4 vertexPosition = mul(u_model[0], vec4(a_position, 1.0));
    v_position = vertexPosition.xyz;
}
