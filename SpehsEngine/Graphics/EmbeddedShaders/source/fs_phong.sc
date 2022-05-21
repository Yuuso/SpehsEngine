$input v_position, v_normal, v_tangent, v_bitangent, v_texcoord0

#include "se_shader.sh"
#include "se_materials.sh"

SAMPLER2D(s_texColor, 0);
SAMPLER2D(s_texNormal, 1);


void main()
{
	vec3 normal = CALC_NORMAL_TEXTURE;
    float specular = 1.0; // TODO: roughness map
    gl_FragColor = u_primitiveColor * texture2D(s_texColor, v_texcoord0) * phong(v_position, normal, specular);
}
