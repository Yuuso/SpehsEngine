$input a_position, a_texcoord0
$output v_texcoord0

#include "se_shader.sh"

void main()
{
    vec3 billboardPosition = u_billboardInfo.xyz;
    bool cylindrical = u_billboardInfo.w != 0.0;
    vec3 globalUp = cylindrical ? vec3(0.0, 1.0, 0.0) : getViewUp();
    vec3 viewDir = normalize(billboardPosition - getViewPosition());
    vec3 combinedRight = normalize(cross(viewDir, globalUp));

    // NOTE: SpehsEngine planes are oriented on the xz-plane, so we consider the primitive's z-axis to be up!

    vec3 position = combinedRight * a_position.x + globalUp * a_position.z;

	gl_Position = mul(u_modelViewProj, vec4(position, 1.0));
    v_texcoord0 = a_texcoord0;
}
