$input v_position, v_normal, v_color0, v_texcoord0

#include "bgfx_shader.sh"
#include "se_shader.sh"

#define MAX_POINT_LIGHTS 16

uniform vec4 u_ambientLight_ColorIntensity;
uniform vec4 u_pointLight_Count;
uniform vec4 u_pointLight_PositionIRadius[MAX_POINT_LIGHTS];
uniform vec4 u_pointLight_ColorORadius[MAX_POINT_LIGHTS];
uniform vec4 u_phong_ShininessStrength;

#define u_ambientLightColor 		u_ambientLight_ColorIntensity.xyz
#define u_ambientLightIntensity 	u_ambientLight_ColorIntensity.w

#define u_pointLightCount 			int(u_pointLight_Count.x)
#define u_pointLightPosition(n)		u_pointLight_PositionIRadius[n].xyz
#define u_pointLightInnerRadius(n)	u_pointLight_PositionIRadius[n].w
#define u_pointLightOuterRadius(n)	u_pointLight_ColorORadius[n].w
#define u_pointLightColor(n)		u_pointLight_ColorORadius[n].xyz

#define u_phongShininess			u_phong_ShininessStrength.x;
#define u_phongSpecularStrength		u_phong_ShininessStrength.y;

SAMPLER2D(s_texColor, 0);
SAMPLER2D(s_texNormal, 1);

void main()
{
	vec3 position = v_position;
	vec3 viewPosition = getViewPosition();
	vec3 viewDirection = normalize(viewPosition - position);
	vec3 normal = v_normal + texture2D(s_texNormal, v_texcoord0).rgb * viewDirection;
	normal = normalize(normal);

	float specularStrength = u_phongShininess;
	float shininess = u_phongSpecularStrength;

	vec3 ambientColor = u_ambientLightColor * u_ambientLightIntensity;

	vec3 diffuseColor = vec3_splat(0.0);
	vec3 specularColor = vec3_splat(0.0);
	for (int i = 0; i < u_pointLightCount; i++)
	{
		ASSERT(u_pointLightInnerRadius(i) < u_pointLightOuterRadius(i));
		float distanceToLight = distance(u_pointLightPosition(i), position);
		float distanceToInner = max(0.0, distanceToLight - u_pointLightInnerRadius(i));
		float fadeZone = u_pointLightOuterRadius(i) - u_pointLightInnerRadius(i);
		float distanceFactor = 1.0 - clamp(distanceToInner / fadeZone, 0.0, 1.0);

		vec3 lightDirection = normalize(u_pointLightPosition(i) - position);
		float diffuseFactor = max(0.0, dot(normal, lightDirection));
		diffuseColor = diffuseColor + diffuseFactor * distanceFactor * u_pointLightColor(i);
		
		vec3 reflectDirection = reflect(-lightDirection, normal);
		float specularFactor = pow(max(0.0, dot(viewDirection, reflectDirection)), shininess);
		specularColor = specularColor + specularFactor * specularStrength * distanceFactor * u_pointLightColor(i);
	}

	gl_FragColor = v_color0 * texture2D(s_texColor, v_texcoord0) * vec4(ambientColor + diffuseColor + specularColor, 1.0);
}
