
#ifndef SE_LIGHTS_HEADER_GUARD
#define SE_LIGHTS_HEADER_GUARD


// --------------------
// Ambient light

uniform vec4 u_ambientLight_ColorIntensity;

#define u_ambientLightColor 		u_ambientLight_ColorIntensity.xyz
#define u_ambientLightIntensity 	u_ambientLight_ColorIntensity.w


// --------------------
// Point lights

#define MAX_POINT_LIGHTS 32

uniform vec4 u_pointLight_Count;
uniform vec4 u_pointLight_PositionIRadius[MAX_POINT_LIGHTS];
uniform vec4 u_pointLight_ColorORadius[MAX_POINT_LIGHTS];

#define u_pointLightCount 			int(u_pointLight_Count.x)
#define u_pointLightPosition(n)		u_pointLight_PositionIRadius[n].xyz
#define u_pointLightInnerRadius(n)	u_pointLight_PositionIRadius[n].w
#define u_pointLightOuterRadius(n)	u_pointLight_ColorORadius[n].w
#define u_pointLightColor(n)		u_pointLight_ColorORadius[n].xyz


#endif // SE_LIGHTS_HEADER_GUARD
