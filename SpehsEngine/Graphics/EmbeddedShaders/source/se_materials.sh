
#ifndef SE_MATERIALS_HEADER_GUARD
#define SE_MATERIALS_HEADER_GUARD


// --------------------
// Phong

uniform vec4 u_phong_ShininessStrength;

#define u_phongShininess			u_phong_ShininessStrength.x;
#define u_phongSpecularStrength		u_phong_ShininessStrength.y;


#endif // SE_MATERIALS_HEADER_GUARD
