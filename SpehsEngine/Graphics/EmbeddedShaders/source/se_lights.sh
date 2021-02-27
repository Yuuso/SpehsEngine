
#ifndef SE_LIGHTS_HEADER_GUARD
#define SE_LIGHTS_HEADER_GUARD


// --------------------
// Lights

#define MAX_LIGHTS 16

uniform vec4 u_lightInfo;
uniform vec4 u_lightData1[MAX_LIGHTS];
uniform vec4 u_lightData2[MAX_LIGHTS];
uniform vec4 u_lightData3[MAX_LIGHTS];
uniform vec4 u_lightData4[MAX_LIGHTS];

#define u_LightCount                    int(u_lightInfo.x)
#define u_LightColor(n)                 u_lightData1[n].xyz
#define u_LightIntensity(n)             u_lightData1[n].w
#define u_LightPosition(n)              u_lightData2[n].xyz
#define u_LightIsGlobal(n)              bool(u_lightData2[n].w > 0.0)
#define u_LightDirection(n)             u_lightData3[n].xyz
#define u_LightInnerRadius(n)           u_lightData4[n].x
#define u_LightOuterRadius(n)           u_lightData4[n].y
#define u_LightInnerCone(n)	            u_lightData4[n].z
#define u_LightOuterCone(n)	            u_lightData4[n].w

float getLinearDistanceAttenuation(int i, vec3 position)
{
    float distanceToLight = distance(u_LightPosition(i), position);
    float distanceToInner = max(0.0, distanceToLight - u_LightInnerRadius(i));
    float fadeZone = max(0.0001, u_LightOuterRadius(i) - u_LightInnerRadius(i));
    float distanceFactor = 1.0 - clamp(distanceToInner / fadeZone, 0.0, 1.0);
    return distanceFactor;
}

float getLinearConeAttenuation(int i, vec3 surfaceToLight)
{
    float lightToSurfaceAngle = acos(dot(-surfaceToLight, normalize(u_LightDirection(i))));
    float angleToInner = max(0.0, lightToSurfaceAngle - (u_LightInnerCone(i) / 2.0));
    float fadeZone = max(0.0001, u_LightOuterCone(i) - u_LightInnerCone(i)) / 2.0;
    float coneFactor = 1.0 - clamp(angleToInner / fadeZone, 0.0, 1.0);
    return coneFactor;
}


#endif // SE_LIGHTS_HEADER_GUARD
