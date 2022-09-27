
#ifndef SE_MATERIALS_HEADER_GUARD
#define SE_MATERIALS_HEADER_GUARD

#include "se_lights.sh"


#define CALC_NORMAL_CUBEMAP(__coord) normalize(mul(mtxFromCols(normalize(v_tangent), normalize(v_bitangent), normalize(v_normal)), textureCube(s_texNormal, __coord).rgb * 2.0 - 1.0));
#define CALC_NORMAL_TEXTURE normalize(mul(mtxFromCols(normalize(v_tangent), normalize(v_bitangent), normalize(v_normal)), texture2D(s_texNormal, v_texcoord0).rgb * 2.0 - 1.0));
#define CALC_NORMAL normalize(mul(mtxFromCols(normalize(v_tangent), normalize(v_bitangent), normalize(v_normal)), vec3(0.5, 0.5, 1.0) * 2.0 - 1.0));

// --------------------
// Phong

uniform vec4 u_phong_ShininessStrength;

#define u_phongShininess			u_phong_ShininessStrength.x;
#define u_phongSpecularStrength		u_phong_ShininessStrength.y;

vec4 phong(vec3 position, vec3 normal, float specular)
{
	vec3 viewPosition = getViewPosition();
	vec3 viewDirection = normalize(viewPosition - position);

	float specularStrength = u_phongSpecularStrength;
	float shininess = u_phongShininess;

	vec3 ambientColor = vec3_splat(0.0);
	vec3 diffuseColor = vec3_splat(0.0);
	vec3 specularColor = vec3_splat(0.0);

	int lightCount = u_LightCount;
	for (int i = 0; i < lightCount; i++)
	{
		vec3 surfaceToLight;
		float attenuation = u_LightIntensity(i);

		if (attenuation <= 0.0)
			continue;

		if (u_LightIsGlobal(i))
		{
			if (length(u_LightDirection(i)) == 0.0)
			{
				// Ambient light
				ambientColor = ambientColor + u_LightColor(i) * attenuation;
				continue;
			}

			// Directional light
			surfaceToLight = -normalize(u_LightDirection(i));
		}
		else
		{
			// Point light
			surfaceToLight = normalize(u_LightPosition(i) - position);
			attenuation = attenuation * getLinearDistanceAttenuation(i, position);

			if (attenuation <= 0.0)
				continue;

			if (length(u_LightDirection(i)) > 0.0)
			{
				// Spot light
				attenuation = attenuation * getLinearConeAttenuation(i, surfaceToLight);
			}
		}

		if (attenuation <= 0.0)
			continue;

		// Diffuse
		float diffuseFactor = max(0.0, dot(normal, surfaceToLight));
		diffuseColor = diffuseColor + diffuseFactor * attenuation * u_LightColor(i);

        if (specular == 0.0)
            continue;

		// Specular
		float specularFactor;
		if (true)
		{
			// Blinn
			vec3 halfwayDirection = normalize(surfaceToLight + viewDirection);
			specularFactor = pow(max(0.0, dot(normal, halfwayDirection)), shininess);
		}
		else
		{
			vec3 reflectDirection = reflect(-surfaceToLight, normal);
			specularFactor = pow(max(0.0, dot(viewDirection, reflectDirection)), shininess);
		}
		specularColor = specularColor + specularFactor * specular * specularStrength * attenuation * u_LightColor(i);
	}

	return vec4(ambientColor + diffuseColor + specularColor, 1.0);
}


#endif // SE_MATERIALS_HEADER_GUARD
