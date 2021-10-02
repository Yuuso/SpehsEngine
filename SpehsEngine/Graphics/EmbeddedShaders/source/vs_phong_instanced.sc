$input a_position, a_normal, a_tangent, a_bitangent, a_texcoord0, i_data0, i_data1, i_data2, i_data3
$output v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include "se_shader.sh"

void main()
{
	mat4 instanceModel = mtxFromCols(i_data0, i_data1, i_data2, i_data3);
    mat4 model = mul(instanceModel, u_model[0]);

	vec4 position = mul(model, vec4(a_position, 1.0));
	v_position = position.xyz;
	gl_Position = mul(u_viewProj, position);

    mat4 instanceNormal = mtxFromCols(vec4(i_data0.xyz, 0.0), vec4(i_data1.xyz, 0.0), vec4(i_data2.xyz, 0.0), vec4(0.0, 0.0, 0.0, 1.0));
    mat4 normal = mul(instanceNormal, u_normal[0]);

	v_normal = normalize(mul(normal, vec4(a_normal, 1.0)).xyz);
	v_tangent = normalize(mul(normal, vec4(a_tangent, 1.0)).xyz);
	v_bitangent = normalize(mul(normal, vec4(a_bitangent, 1.0)).xyz);

	v_texcoord0 = a_texcoord0;
}
