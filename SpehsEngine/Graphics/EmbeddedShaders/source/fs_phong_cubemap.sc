$input v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include "se_shader.sh"
#include "se_materials.sh"

SAMPLERCUBE(s_texColor, 0);
SAMPLERCUBE(s_texNormal, 1);
SAMPLERCUBE(s_texRoughness, 2);


void main()
{
    vec3 texCoord = normalize(v_position).xyz;
	vec3 normal = CALC_NORMAL_CUBEMAP(texCoord);
    float specular = 1.0 - textureCube(s_texRoughness, texCoord).r;
    gl_FragColor = u_primitiveColor * textureCube(s_texColor, texCoord) * phong(v_position, normal, specular);
}
