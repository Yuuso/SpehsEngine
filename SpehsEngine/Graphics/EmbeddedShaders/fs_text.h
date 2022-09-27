static const uint8_t fs_text_glsl[510] =
{
	0x46, 0x53, 0x48, 0x0b, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x10, 0x75, // FSH............u
	0x5f, 0x70, 0x72, 0x69, 0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x02, // _primitiveColor.
	0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, // ..........s_font
	0x54, 0x65, 0x78, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbc, 0x01, 0x00, // Tex.............
	0x00, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, // .precision highp
	0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x32, 0x44, 0x41, 0x72, 0x72, 0x61, 0x79, 0x3b, //  sampler2DArray;
	0x0a, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, // .precision highp
	0x20, 0x69, 0x6e, 0x74, 0x3b, 0x0a, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6f, 0x6e, 0x20, //  int;.precision 
	0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x3b, 0x0a, 0x6f, 0x75, 0x74, // highp float;.out
	0x20, 0x6c, 0x6f, 0x77, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x62, 0x67, 0x66, 0x78, 0x5f, //  lowp vec4 bgfx_
	0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x3b, 0x0a, 0x69, 0x6e, 0x20, 0x76, 0x65, // FragColor;.in ve
	0x63, 0x34, 0x20, 0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x3b, 0x0a, 0x69, 0x6e, 0x20, // c4 v_color0;.in 
	0x76, 0x65, 0x63, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, // vec2 v_texcoord0
	0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x75, // ;.uniform vec4 u
	0x5f, 0x70, 0x72, 0x69, 0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x3b, // _primitiveColor;
	0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, // .uniform sampler
	0x32, 0x44, 0x20, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x3b, 0x0a, 0x76, 0x6f, // 2D s_fontTex;.vo
	0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, 0x28, 0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x6c, // id main ().{.  l
	0x6f, 0x77, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // owp vec4 tmpvar_
	0x31, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x2e, 0x78, 0x79, // 1;.  tmpvar_1.xy
	0x7a, 0x20, 0x3d, 0x20, 0x76, 0x65, 0x63, 0x33, 0x28, 0x31, 0x2e, 0x30, 0x2c, 0x20, 0x31, 0x2e, // z = vec3(1.0, 1.
	0x30, 0x2c, 0x20, 0x31, 0x2e, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, // 0, 1.0);.  tmpva
	0x72, 0x5f, 0x31, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x20, // r_1.w = texture 
	0x28, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x2c, 0x20, 0x76, 0x5f, 0x74, 0x65, // (s_fontTex, v_te
	0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x29, 0x2e, 0x78, 0x3b, 0x0a, 0x20, 0x20, 0x69, 0x66, // xcoord0).x;.  if
	0x20, 0x28, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x2e, 0x77, 0x20, 0x3d, 0x3d, //  ((tmpvar_1.w ==
	0x20, 0x30, 0x2e, 0x30, 0x29, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x64, 0x69, 0x73, //  0.0)) {.    dis
	0x63, 0x61, 0x72, 0x64, 0x3b, 0x0a, 0x20, 0x20, 0x7d, 0x3b, 0x0a, 0x20, 0x20, 0x62, 0x67, 0x66, // card;.  };.  bgf
	0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x3d, 0x20, 0x28, 0x28, // x_FragColor = ((
	0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x20, 0x2a, 0x20, 0x75, 0x5f, 0x70, 0x72, 0x69, // v_color0 * u_pri
	0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x29, 0x20, 0x2a, 0x20, 0x74, // mitiveColor) * t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x29, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00,             // mpvar_1);.}...
};
static const uint8_t fs_text_essl[510] =
{
	0x46, 0x53, 0x48, 0x0b, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x10, 0x75, // FSH............u
	0x5f, 0x70, 0x72, 0x69, 0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x02, // _primitiveColor.
	0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, // ..........s_font
	0x54, 0x65, 0x78, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbc, 0x01, 0x00, // Tex.............
	0x00, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, // .precision highp
	0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x32, 0x44, 0x41, 0x72, 0x72, 0x61, 0x79, 0x3b, //  sampler2DArray;
	0x0a, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, // .precision highp
	0x20, 0x69, 0x6e, 0x74, 0x3b, 0x0a, 0x70, 0x72, 0x65, 0x63, 0x69, 0x73, 0x69, 0x6f, 0x6e, 0x20, //  int;.precision 
	0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x3b, 0x0a, 0x6f, 0x75, 0x74, // highp float;.out
	0x20, 0x6c, 0x6f, 0x77, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x62, 0x67, 0x66, 0x78, 0x5f, //  lowp vec4 bgfx_
	0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x3b, 0x0a, 0x69, 0x6e, 0x20, 0x76, 0x65, // FragColor;.in ve
	0x63, 0x34, 0x20, 0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x3b, 0x0a, 0x69, 0x6e, 0x20, // c4 v_color0;.in 
	0x76, 0x65, 0x63, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, // vec2 v_texcoord0
	0x3b, 0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x75, // ;.uniform vec4 u
	0x5f, 0x70, 0x72, 0x69, 0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x3b, // _primitiveColor;
	0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, // .uniform sampler
	0x32, 0x44, 0x20, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x3b, 0x0a, 0x76, 0x6f, // 2D s_fontTex;.vo
	0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, 0x28, 0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x6c, // id main ().{.  l
	0x6f, 0x77, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // owp vec4 tmpvar_
	0x31, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x2e, 0x78, 0x79, // 1;.  tmpvar_1.xy
	0x7a, 0x20, 0x3d, 0x20, 0x76, 0x65, 0x63, 0x33, 0x28, 0x31, 0x2e, 0x30, 0x2c, 0x20, 0x31, 0x2e, // z = vec3(1.0, 1.
	0x30, 0x2c, 0x20, 0x31, 0x2e, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, // 0, 1.0);.  tmpva
	0x72, 0x5f, 0x31, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x20, // r_1.w = texture 
	0x28, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x2c, 0x20, 0x76, 0x5f, 0x74, 0x65, // (s_fontTex, v_te
	0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x29, 0x2e, 0x78, 0x3b, 0x0a, 0x20, 0x20, 0x69, 0x66, // xcoord0).x;.  if
	0x20, 0x28, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x2e, 0x77, 0x20, 0x3d, 0x3d, //  ((tmpvar_1.w ==
	0x20, 0x30, 0x2e, 0x30, 0x29, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x64, 0x69, 0x73, //  0.0)) {.    dis
	0x63, 0x61, 0x72, 0x64, 0x3b, 0x0a, 0x20, 0x20, 0x7d, 0x3b, 0x0a, 0x20, 0x20, 0x62, 0x67, 0x66, // card;.  };.  bgf
	0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x3d, 0x20, 0x28, 0x28, // x_FragColor = ((
	0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x20, 0x2a, 0x20, 0x75, 0x5f, 0x70, 0x72, 0x69, // v_color0 * u_pri
	0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x29, 0x20, 0x2a, 0x20, 0x74, // mitiveColor) * t
	0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x29, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00,             // mpvar_1);.}...
};
static const uint8_t fs_text_spv[1325] =
{
	0x46, 0x53, 0x48, 0x0b, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x10, 0x75, // FSH............u
	0x5f, 0x70, 0x72, 0x69, 0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x12, // _primitiveColor.
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, // ..........s_font
	0x54, 0x65, 0x78, 0x30, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x02, 0x1a, 0x00, 0xe8, 0x04, 0x00, // Tex0............
	0x00, 0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x0a, 0x00, 0x08, 0x00, 0xb5, 0x00, 0x00, // ...#............
	0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x06, // ................
	0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, 0x74, 0x64, 0x2e, 0x34, 0x35, // .....GLSL.std.45
	0x30, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, // 0...............
	0x00, 0x0f, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, // .............mai
	0x6e, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 0x69, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, // n....e...i...v..
	0x00, 0x10, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, // ................
	0x00, 0x05, 0x00, 0x00, 0x00, 0xf4, 0x01, 0x00, 0x00, 0x05, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, // ................
	0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x07, 0x00, 0x23, 0x00, 0x00, // .main........#..
	0x00, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, // .s_fontTexSample
	0x72, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x07, 0x00, 0x26, 0x00, 0x00, 0x00, 0x73, 0x5f, 0x66, // r........&...s_f
	0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x00, 0x00, 0x00, // ontTexTexture...
	0x00, 0x05, 0x00, 0x06, 0x00, 0x57, 0x00, 0x00, 0x00, 0x55, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, // .....W...Uniform
	0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x08, 0x00, 0x57, 0x00, 0x00, // Block........W..
	0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x5f, 0x70, 0x72, 0x69, 0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, // .....u_primitive
	0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x03, 0x00, 0x59, 0x00, 0x00, // Color........Y..
	0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x65, 0x00, 0x00, 0x00, 0x76, 0x5f, 0x63, // .........e...v_c
	0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x69, 0x00, 0x00, // olor0........i..
	0x00, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x00, 0x05, 0x00, 0x06, // .v_texcoord0....
	0x00, 0x76, 0x00, 0x00, 0x00, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x44, 0x61, // .v...bgfx_FragDa
	0x74, 0x61, 0x30, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x23, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, // ta0..G...#..."..
	0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x23, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, // .....G...#...!..
	0x00, 0x12, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x26, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, // .....G...&..."..
	0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x26, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, // .....G...&...!..
	0x00, 0x02, 0x00, 0x00, 0x00, 0x48, 0x00, 0x05, 0x00, 0x57, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // .....H...W......
	0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x03, 0x00, 0x57, 0x00, 0x00, // .#.......G...W..
	0x00, 0x02, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x59, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, // .....G...Y..."..
	0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x59, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, // .....G...Y...!..
	0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x65, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, // .....G...e......
	0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x69, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, // .....G...i......
	0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x76, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, // .....G...v......
	0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x21, 0x00, 0x03, // .............!..
	0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x02, 0x00, 0x06, 0x00, 0x00, // ................
	0x00, 0x16, 0x00, 0x03, 0x00, 0x07, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x19, 0x00, 0x09, // ......... ......
	0x00, 0x08, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
	0x00, 0x17, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, // ................
	0x00, 0x17, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, // ................
	0x00, 0x20, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, // . ..."..........
	0x00, 0x3b, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // .;..."...#......
	0x00, 0x20, 0x00, 0x04, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, // . ...%..........
	0x00, 0x3b, 0x00, 0x04, 0x00, 0x25, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // .;...%...&......
	0x00, 0x15, 0x00, 0x04, 0x00, 0x29, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, // .....)... ......
	0x00, 0x2b, 0x00, 0x04, 0x00, 0x29, 0x00, 0x00, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // .+...)..........
	0x00, 0x1b, 0x00, 0x03, 0x00, 0x32, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, // .....2.......+..
	0x00, 0x07, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x04, // .....@.......+..
	0x00, 0x07, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x14, 0x00, 0x02, // .....D......?...
	0x00, 0x51, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x03, 0x00, 0x57, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, // .Q.......W......
	0x00, 0x20, 0x00, 0x04, 0x00, 0x58, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x57, 0x00, 0x00, // . ...X.......W..
	0x00, 0x3b, 0x00, 0x04, 0x00, 0x58, 0x00, 0x00, 0x00, 0x59, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, // .;...X...Y......
	0x00, 0x20, 0x00, 0x04, 0x00, 0x5a, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, // . ...Z..........
	0x00, 0x20, 0x00, 0x04, 0x00, 0x61, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, // . ...a..........
	0x00, 0x3b, 0x00, 0x04, 0x00, 0x61, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, // .;...a...e......
	0x00, 0x20, 0x00, 0x04, 0x00, 0x68, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x00, // . ...h..........
	0x00, 0x3b, 0x00, 0x04, 0x00, 0x68, 0x00, 0x00, 0x00, 0x69, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, // .;...h...i......
	0x00, 0x20, 0x00, 0x04, 0x00, 0x75, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, // . ...u..........
	0x00, 0x3b, 0x00, 0x04, 0x00, 0x75, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, // .;...u...v......
	0x00, 0x36, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // .6..............
	0x00, 0x03, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, // .............=..
	0x00, 0x06, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, // .....$...#...=..
	0x00, 0x08, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, // .....'...&...=..
	0x00, 0x0d, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, // .....f...e...=..
	0x00, 0x0b, 0x00, 0x00, 0x00, 0x6a, 0x00, 0x00, 0x00, 0x69, 0x00, 0x00, 0x00, 0x56, 0x00, 0x05, // .....j...i...V..
	0x00, 0x32, 0x00, 0x00, 0x00, 0xb2, 0x00, 0x00, 0x00, 0x27, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00, // .2.......'...$..
	0x00, 0x57, 0x00, 0x05, 0x00, 0x0d, 0x00, 0x00, 0x00, 0xb4, 0x00, 0x00, 0x00, 0xb2, 0x00, 0x00, // .W..............
	0x00, 0x6a, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x07, 0x00, 0x00, 0x00, 0x97, 0x00, 0x00, // .j...Q..........
	0x00, 0xb4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, 0x07, 0x00, 0x0d, 0x00, 0x00, // .........P......
	0x00, 0x98, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, // .....D...D...D..
	0x00, 0x97, 0x00, 0x00, 0x00, 0xb4, 0x00, 0x05, 0x00, 0x51, 0x00, 0x00, 0x00, 0x9b, 0x00, 0x00, // .........Q......
	0x00, 0x97, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0xf7, 0x00, 0x03, 0x00, 0x9d, 0x00, 0x00, // .....@..........
	0x00, 0x00, 0x00, 0x00, 0x00, 0xfa, 0x00, 0x04, 0x00, 0x9b, 0x00, 0x00, 0x00, 0x9c, 0x00, 0x00, // ................
	0x00, 0x9d, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00, 0x9c, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x01, // ................
	0x00, 0xf8, 0x00, 0x02, 0x00, 0x9d, 0x00, 0x00, 0x00, 0x41, 0x00, 0x05, 0x00, 0x5a, 0x00, 0x00, // .........A...Z..
	0x00, 0x9f, 0x00, 0x00, 0x00, 0x59, 0x00, 0x00, 0x00, 0x2e, 0x00, 0x00, 0x00, 0x3d, 0x00, 0x04, // .....Y.......=..
	0x00, 0x0d, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00, 0x00, 0x9f, 0x00, 0x00, 0x00, 0x85, 0x00, 0x05, // ................
	0x00, 0x0d, 0x00, 0x00, 0x00, 0xa1, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00, // .........f......
	0x00, 0x85, 0x00, 0x05, 0x00, 0x0d, 0x00, 0x00, 0x00, 0xa3, 0x00, 0x00, 0x00, 0xa1, 0x00, 0x00, // ................
	0x00, 0x98, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x76, 0x00, 0x00, 0x00, 0xa3, 0x00, 0x00, // .....>...v......
	0x00, 0xfd, 0x00, 0x01, 0x00, 0x38, 0x00, 0x01, 0x00, 0x00, 0x00, 0x10, 0x00,                   // .....8.......
};
static const uint8_t fs_text_dx9[422] =
{
	0x46, 0x53, 0x48, 0x0b, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10, 0x75, // FSH............u
	0x5f, 0x70, 0x72, 0x69, 0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x12, // _primitiveColor.
	0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x01, 0x00, 0x00, 0x00, 0x03, 0xff, // .........x......
	0xff, 0xfe, 0xff, 0x34, 0x00, 0x43, 0x54, 0x41, 0x42, 0x1c, 0x00, 0x00, 0x00, 0xa3, 0x00, 0x00, // ...4.CTAB.......
	0x00, 0x00, 0x03, 0xff, 0xff, 0x02, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x91, 0x00, // ................
	0x00, 0x9c, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x03, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, // .....D..........
	0x00, 0x68, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, // .h.......x......
	0x00, 0x01, 0x00, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x5f, 0x66, // .............s_f
	0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x2b, 0x73, 0x5f, // ontTexSampler+s_
	0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x00, 0xab, // fontTexTexture..
	0xab, 0x04, 0x00, 0x07, 0x00, 0x01, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
	0x00, 0x75, 0x5f, 0x70, 0x72, 0x69, 0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, 0x43, 0x6f, 0x6c, 0x6f, // .u_primitiveColo
	0x72, 0x00, 0xab, 0xab, 0xab, 0x01, 0x00, 0x03, 0x00, 0x01, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00, // r...............
	0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x73, 0x5f, 0x33, 0x5f, 0x30, 0x00, 0x4d, 0x69, 0x63, 0x72, // .....ps_3_0.Micr
	0x6f, 0x73, 0x6f, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29, 0x20, 0x48, 0x4c, 0x53, 0x4c, 0x20, 0x53, // osoft (R) HLSL S
	0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6f, 0x6d, 0x70, 0x69, 0x6c, 0x65, 0x72, 0x20, 0x31, // hader Compiler 1
	0x30, 0x2e, 0x31, 0x00, 0xab, 0x51, 0x00, 0x00, 0x05, 0x01, 0x00, 0x0f, 0xa0, 0x00, 0x00, 0x80, // 0.1..Q..........
	0xbf, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x1f, 0x00, 0x00, // ............?...
	0x02, 0x0a, 0x00, 0x00, 0x80, 0x00, 0x00, 0x0f, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, // ................
	0x80, 0x01, 0x00, 0x03, 0x90, 0x1f, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x90, 0x01, 0x08, 0x0f, // ................
	0xa0, 0x42, 0x00, 0x00, 0x03, 0x00, 0x00, 0x0f, 0x80, 0x01, 0x00, 0xe4, 0x90, 0x01, 0x08, 0xe4, // .B..............
	0xa0, 0x58, 0x00, 0x00, 0x04, 0x01, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x8c, 0x01, 0x00, 0x00, // .X..............
	0xa0, 0x01, 0x00, 0x55, 0xa0, 0x41, 0x00, 0x00, 0x01, 0x01, 0x00, 0x0f, 0x80, 0x04, 0x00, 0x00, // ...U.A..........
	0x04, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0xea, 0xa0, 0x01, 0x00, 0xbf, // ................
	0xa0, 0x05, 0x00, 0x00, 0x03, 0x01, 0x00, 0x0f, 0x80, 0x00, 0x00, 0xe4, 0xa0, 0x00, 0x00, 0xe4, // ................
	0x90, 0x05, 0x00, 0x00, 0x03, 0x00, 0x08, 0x0f, 0x80, 0x00, 0x00, 0xe4, 0x80, 0x01, 0x00, 0xe4, // ................
	0x80, 0xff, 0xff, 0x00, 0x00, 0x00,                                                             // ......
};
static const uint8_t fs_text_dx11[565] =
{
	0x46, 0x53, 0x48, 0x0b, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x10, 0x75, // FSH............u
	0x5f, 0x70, 0x72, 0x69, 0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x12, // _primitiveColor.
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, // ..........s_font
	0x54, 0x65, 0x78, 0x30, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x73, 0x5f, // Tex0..........s_
	0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x30, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, // fontTex0........
	0x00, 0xdc, 0x01, 0x00, 0x00, 0x44, 0x58, 0x42, 0x43, 0x89, 0x70, 0x86, 0x60, 0x56, 0xcd, 0xd2, // .....DXBC.p.`V..
	0xc3, 0x28, 0x89, 0x3b, 0x80, 0xe7, 0x31, 0x3c, 0xc3, 0x01, 0x00, 0x00, 0x00, 0xdc, 0x01, 0x00, // .(.;..1<........
	0x00, 0x03, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00, 0x00, 0xd4, 0x00, 0x00, // .....,..........
	0x00, 0x49, 0x53, 0x47, 0x4e, 0x6c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, // .ISGNl..........
	0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, // .P..............
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, // ................
	0x00, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, // .b..............
	0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x50, 0x4f, 0x53, 0x49, // .........SV_POSI
	0x54, 0x49, 0x4f, 0x4e, 0x00, 0x43, 0x4f, 0x4c, 0x4f, 0x52, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, // TION.COLOR.TEXCO
	0x4f, 0x52, 0x44, 0x00, 0xab, 0x4f, 0x53, 0x47, 0x4e, 0x2c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, // ORD..OSGN,......
	0x00, 0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ..... ..........
	0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5f, // .............SV_
	0x54, 0x41, 0x52, 0x47, 0x45, 0x54, 0x00, 0xab, 0xab, 0x53, 0x48, 0x44, 0x52, 0x00, 0x01, 0x00, // TARGET...SHDR...
	0x00, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x59, 0x00, 0x00, 0x04, 0x46, 0x8e, 0x20, // .@...@...Y...F. 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x5a, 0x00, 0x00, 0x03, 0x00, 0x60, 0x10, // .........Z....`.
	0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x18, 0x00, 0x04, 0x00, 0x70, 0x10, 0x00, 0x00, 0x00, 0x00, // .....X....p.....
	0x00, 0x55, 0x55, 0x00, 0x00, 0x62, 0x10, 0x00, 0x03, 0xf2, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, // .UU..b..........
	0x00, 0x62, 0x10, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, // .b...2.......e..
	0x03, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, // .. ......h......
	0x00, 0x45, 0x00, 0x00, 0x09, 0xf2, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, // .E...........F..
	0x00, 0x02, 0x00, 0x00, 0x00, 0x96, 0x73, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x10, // ......s.......`.
	0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x07, 0x22, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, // ........."......
	0x00, 0x3a, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, // .:........@.....
	0x00, 0x0d, 0x00, 0x04, 0x03, 0x1a, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, // .............8..
	0x08, 0xf2, 0x00, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x46, 0x1e, 0x10, 0x00, 0x01, 0x00, 0x00, // .........F......
	0x00, 0x46, 0x8e, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, // .F. .........6..
	0x05, 0x12, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x80, // ..........@.....
	0x3f, 0x38, 0x00, 0x00, 0x07, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0c, 0x10, // ?8.... .........
	0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x0e, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, // .....F.......>..
	0x01, 0x00, 0x00, 0x10, 0x00,                                                                   // .....
};
static const uint8_t fs_text_mtl[915] =
{
	0x46, 0x53, 0x48, 0x0b, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x10, 0x73, // FSH............s
	0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x11, // _fontTexSampler.
	0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, // ..........s_font
	0x54, 0x65, 0x78, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x11, 0x01, 0xff, 0xff, 0x01, 0x00, // TexTexture......
	0x00, 0x00, 0x00, 0x00, 0x10, 0x75, 0x5f, 0x70, 0x72, 0x69, 0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, // .....u_primitive
	0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x12, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, // Color...........
	0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // s_fontTex.......
	0x00, 0x00, 0x00, 0x18, 0x03, 0x00, 0x00, 0x23, 0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x20, // .......#include 
	0x3c, 0x6d, 0x65, 0x74, 0x61, 0x6c, 0x5f, 0x73, 0x74, 0x64, 0x6c, 0x69, 0x62, 0x3e, 0x0a, 0x23, // <metal_stdlib>.#
	0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x20, 0x3c, 0x73, 0x69, 0x6d, 0x64, 0x2f, 0x73, 0x69, // include <simd/si
	0x6d, 0x64, 0x2e, 0x68, 0x3e, 0x0a, 0x0a, 0x75, 0x73, 0x69, 0x6e, 0x67, 0x20, 0x6e, 0x61, 0x6d, // md.h>..using nam
	0x65, 0x73, 0x70, 0x61, 0x63, 0x65, 0x20, 0x6d, 0x65, 0x74, 0x61, 0x6c, 0x3b, 0x0a, 0x0a, 0x73, // espace metal;..s
	0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x5f, 0x47, 0x6c, 0x6f, 0x62, 0x61, 0x6c, 0x0a, 0x7b, 0x0a, // truct _Global.{.
	0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x20, 0x75, 0x5f, 0x70, 0x72, 0x69, //     float4 u_pri
	0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x3b, 0x0a, 0x7d, 0x3b, 0x0a, // mitiveColor;.};.
	0x0a, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, // .struct xlatMtlM
	0x61, 0x69, 0x6e, 0x5f, 0x6f, 0x75, 0x74, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, // ain_out.{.    fl
	0x6f, 0x61, 0x74, 0x34, 0x20, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x44, 0x61, // oat4 bgfx_FragDa
	0x74, 0x61, 0x30, 0x20, 0x5b, 0x5b, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x28, 0x30, 0x29, 0x5d, 0x5d, // ta0 [[color(0)]]
	0x3b, 0x0a, 0x7d, 0x3b, 0x0a, 0x0a, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x78, 0x6c, 0x61, // ;.};..struct xla
	0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x69, 0x6e, 0x0a, 0x7b, 0x0a, 0x20, 0x20, // tMtlMain_in.{.  
	0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x20, 0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, //   float4 v_color
	0x30, 0x20, 0x5b, 0x5b, 0x75, 0x73, 0x65, 0x72, 0x28, 0x6c, 0x6f, 0x63, 0x6e, 0x30, 0x29, 0x5d, // 0 [[user(locn0)]
	0x5d, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x20, 0x76, 0x5f, // ];.    float2 v_
	0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x5b, 0x5b, 0x75, 0x73, 0x65, 0x72, // texcoord0 [[user
	0x28, 0x6c, 0x6f, 0x63, 0x6e, 0x31, 0x29, 0x5d, 0x5d, 0x3b, 0x0a, 0x7d, 0x3b, 0x0a, 0x0a, 0x66, // (locn1)]];.};..f
	0x72, 0x61, 0x67, 0x6d, 0x65, 0x6e, 0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, // ragment xlatMtlM
	0x61, 0x69, 0x6e, 0x5f, 0x6f, 0x75, 0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, // ain_out xlatMtlM
	0x61, 0x69, 0x6e, 0x28, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, // ain(xlatMtlMain_
	0x69, 0x6e, 0x20, 0x69, 0x6e, 0x20, 0x5b, 0x5b, 0x73, 0x74, 0x61, 0x67, 0x65, 0x5f, 0x69, 0x6e, // in in [[stage_in
	0x5d, 0x5d, 0x2c, 0x20, 0x63, 0x6f, 0x6e, 0x73, 0x74, 0x61, 0x6e, 0x74, 0x20, 0x5f, 0x47, 0x6c, // ]], constant _Gl
	0x6f, 0x62, 0x61, 0x6c, 0x26, 0x20, 0x5f, 0x6d, 0x74, 0x6c, 0x5f, 0x75, 0x20, 0x5b, 0x5b, 0x62, // obal& _mtl_u [[b
	0x75, 0x66, 0x66, 0x65, 0x72, 0x28, 0x30, 0x29, 0x5d, 0x5d, 0x2c, 0x20, 0x74, 0x65, 0x78, 0x74, // uffer(0)]], text
	0x75, 0x72, 0x65, 0x32, 0x64, 0x3c, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x3e, 0x20, 0x73, 0x5f, 0x66, // ure2d<float> s_f
	0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x20, 0x5b, 0x5b, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, // ontTex [[texture
	0x28, 0x30, 0x29, 0x5d, 0x5d, 0x2c, 0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x20, 0x73, // (0)]], sampler s
	0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x20, // _fontTexSampler 
	0x5b, 0x5b, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x28, 0x30, 0x29, 0x5d, 0x5d, 0x29, 0x0a, // [[sampler(0)]]).
	0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, // {.    xlatMtlMai
	0x6e, 0x5f, 0x6f, 0x75, 0x74, 0x20, 0x6f, 0x75, 0x74, 0x20, 0x3d, 0x20, 0x7b, 0x7d, 0x3b, 0x0a, // n_out out = {};.
	0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x20, 0x5f, 0x31, 0x38, 0x30, 0x20, //     float4 _180 
	0x3d, 0x20, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x2e, 0x73, 0x61, 0x6d, 0x70, // = s_fontTex.samp
	0x6c, 0x65, 0x28, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d, 0x70, // le(s_fontTexSamp
	0x6c, 0x65, 0x72, 0x2c, 0x20, 0x69, 0x6e, 0x2e, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, // ler, in.v_texcoo
	0x72, 0x64, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x20, // rd0);.    float 
	0x5f, 0x31, 0x35, 0x31, 0x20, 0x3d, 0x20, 0x5f, 0x31, 0x38, 0x30, 0x2e, 0x78, 0x3b, 0x0a, 0x20, // _151 = _180.x;. 
	0x20, 0x20, 0x20, 0x69, 0x66, 0x20, 0x28, 0x5f, 0x31, 0x35, 0x31, 0x20, 0x3d, 0x3d, 0x20, 0x30, //    if (_151 == 0
	0x2e, 0x30, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // .0).    {.      
	0x20, 0x20, 0x64, 0x69, 0x73, 0x63, 0x61, 0x72, 0x64, 0x5f, 0x66, 0x72, 0x61, 0x67, 0x6d, 0x65, //   discard_fragme
	0x6e, 0x74, 0x28, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, // nt();.    }.    
	0x6f, 0x75, 0x74, 0x2e, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x44, 0x61, 0x74, // out.bgfx_FragDat
	0x61, 0x30, 0x20, 0x3d, 0x20, 0x28, 0x69, 0x6e, 0x2e, 0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, // a0 = (in.v_color
	0x30, 0x20, 0x2a, 0x20, 0x5f, 0x6d, 0x74, 0x6c, 0x5f, 0x75, 0x2e, 0x75, 0x5f, 0x70, 0x72, 0x69, // 0 * _mtl_u.u_pri
	0x6d, 0x69, 0x74, 0x69, 0x76, 0x65, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x29, 0x20, 0x2a, 0x20, 0x66, // mitiveColor) * f
	0x6c, 0x6f, 0x61, 0x74, 0x34, 0x28, 0x31, 0x2e, 0x30, 0x2c, 0x20, 0x31, 0x2e, 0x30, 0x2c, 0x20, // loat4(1.0, 1.0, 
	0x31, 0x2e, 0x30, 0x2c, 0x20, 0x5f, 0x31, 0x35, 0x31, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, // 1.0, _151);.    
	0x72, 0x65, 0x74, 0x75, 0x72, 0x6e, 0x20, 0x6f, 0x75, 0x74, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00, // return out;.}...
	0x00, 0x30, 0x00,                                                                               // .0.
};
extern const uint8_t* fs_text_pssl;
extern const uint32_t fs_text_pssl_size;
