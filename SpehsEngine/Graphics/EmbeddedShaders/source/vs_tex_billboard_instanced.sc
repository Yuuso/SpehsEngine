$input a_position, a_texcoord0, i_data0, i_data1
$output v_texcoord0

#include "se_shader.sh"

void main()
{
    vec3 instancePosition = i_data0.xyz;
    float instanceRotationAngle = i_data0.w;
    vec2 instanceScale = i_data1.xy;
    vec2 instanceTexcoords = i_data1.zw;

    vec3 billboardCenterPosition = u_billboardInfo.xyz + instancePosition;
    bool cylindrical = u_billboardInfo.w != 0.0;
    vec3 globalUp = cylindrical ? vec3(0.0, 1.0, 0.0) : getViewUp();
    vec3 viewDir = normalize(billboardCenterPosition - getViewPosition());
    vec3 combinedRight = normalize(cross(viewDir, globalUp));
    
    // NOTE: SpehsEngine planes are oriented on the xz-plane, so we consider the primitive's z-axis to be up!

    float rightValue = a_position.x * instanceScale.x;
    float upValue = a_position.z * instanceScale.y;
    float rotatedRightValue = rightValue * cos(instanceRotationAngle) - upValue * sin(instanceRotationAngle);
    float rotatedUpValue = rightValue * sin(instanceRotationAngle) + upValue * cos(instanceRotationAngle);

    vec3 vertexPosition = combinedRight * rotatedRightValue + globalUp * rotatedUpValue + instancePosition;

	gl_Position = mul(u_modelViewProj, vec4(vertexPosition, 1.0));
    v_texcoord0 = a_texcoord0 + instanceTexcoords;
}
