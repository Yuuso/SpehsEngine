static const uint8_t fs_text_glsl[345] =
{
	0x46, 0x53, 0x48, 0x08, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x73, // FSH............s
	0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // _fontTex........
	0x34, 0x01, 0x00, 0x00, 0x76, 0x61, 0x72, 0x79, 0x69, 0x6e, 0x67, 0x20, 0x68, 0x69, 0x67, 0x68, // 4...varying high
	0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x3b, // p vec4 v_color0;
	0x0a, 0x76, 0x61, 0x72, 0x79, 0x69, 0x6e, 0x67, 0x20, 0x68, 0x69, 0x67, 0x68, 0x70, 0x20, 0x76, // .varying highp v
	0x65, 0x63, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x3b, // ec2 v_texcoord0;
	0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, // .uniform sampler
	0x32, 0x44, 0x20, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x3b, 0x0a, 0x76, 0x6f, // 2D s_fontTex;.vo
	0x69, 0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x20, 0x28, 0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x6c, // id main ().{.  l
	0x6f, 0x77, 0x70, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, // owp vec4 tmpvar_
	0x31, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x2e, 0x78, 0x79, // 1;.  tmpvar_1.xy
	0x7a, 0x20, 0x3d, 0x20, 0x76, 0x65, 0x63, 0x33, 0x28, 0x31, 0x2e, 0x30, 0x2c, 0x20, 0x31, 0x2e, // z = vec3(1.0, 1.
	0x30, 0x2c, 0x20, 0x31, 0x2e, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, // 0, 1.0);.  tmpva
	0x72, 0x5f, 0x31, 0x2e, 0x77, 0x20, 0x3d, 0x20, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x32, // r_1.w = texture2
	0x44, 0x20, 0x28, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x2c, 0x20, 0x76, 0x5f, // D (s_fontTex, v_
	0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x29, 0x2e, 0x78, 0x3b, 0x0a, 0x20, 0x20, // texcoord0).x;.  
	0x69, 0x66, 0x20, 0x28, 0x28, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, 0x5f, 0x31, 0x2e, 0x77, 0x20, // if ((tmpvar_1.w 
	0x3c, 0x20, 0x30, 0x2e, 0x30, 0x31, 0x29, 0x29, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x64, // < 0.01)) {.    d
	0x69, 0x73, 0x63, 0x61, 0x72, 0x64, 0x3b, 0x0a, 0x20, 0x20, 0x7d, 0x3b, 0x0a, 0x20, 0x20, 0x67, // iscard;.  };.  g
	0x6c, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x3d, 0x20, 0x28, 0x76, // l_FragColor = (v
	0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x20, 0x2a, 0x20, 0x74, 0x6d, 0x70, 0x76, 0x61, 0x72, // _color0 * tmpvar
	0x5f, 0x31, 0x29, 0x3b, 0x0a, 0x7d, 0x0a, 0x0a, 0x00,                                           // _1);.}...
};
static const uint8_t fs_text_spv[1066] =
{
	0x46, 0x53, 0x48, 0x08, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x10, 0x73, // FSH............s
	0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x11, // _fontTexSampler.
	0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x10, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, // ........s_fontTe
	0x78, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x11, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, // xTexture........
	0x09, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x30, 0x00, 0x40, 0x00, 0x00, 0x00, // .s_fontTex0.@...
	0x00, 0x02, 0xd0, 0x03, 0x00, 0x00, 0x03, 0x02, 0x23, 0x07, 0x00, 0x00, 0x01, 0x00, 0x0a, 0x00, // ........#.......
	0x08, 0x00, 0xa6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x02, 0x00, 0x01, 0x00, // ................
	0x00, 0x00, 0x0b, 0x00, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x4c, 0x53, 0x4c, 0x2e, 0x73, // ..........GLSL.s
	0x74, 0x64, 0x2e, 0x34, 0x35, 0x30, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x00, 0x00, 0x00, // td.450..........
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, // ................
	0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x5b, 0x00, 0x00, 0x00, 0x5f, 0x00, // ..main....[..._.
	0x00, 0x00, 0x6a, 0x00, 0x00, 0x00, 0x10, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x00, 0x07, 0x00, // ..j.............
	0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x05, 0x00, 0x00, 0x00, 0xf4, 0x01, 0x00, 0x00, 0x05, 0x00, // ................
	0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x6d, 0x61, 0x69, 0x6e, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, // ......main......
	0x07, 0x00, 0x22, 0x00, 0x00, 0x00, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x53, // .."...s_fontTexS
	0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x07, 0x00, 0x25, 0x00, // ampler........%.
	0x00, 0x00, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x54, 0x65, 0x78, 0x74, 0x75, // ..s_fontTexTextu
	0x72, 0x65, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x5b, 0x00, 0x00, 0x00, 0x76, 0x5f, // re........[...v_
	0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x05, 0x00, 0x5f, 0x00, // color0........_.
	0x00, 0x00, 0x76, 0x5f, 0x74, 0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x00, 0x05, 0x00, // ..v_texcoord0...
	0x06, 0x00, 0x6a, 0x00, 0x00, 0x00, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x44, // ..j...bgfx_FragD
	0x61, 0x74, 0x61, 0x30, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0x22, 0x00, // ata0..G..."...".
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x22, 0x00, 0x00, 0x00, 0x21, 0x00, // ......G..."...!.
	0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x25, 0x00, 0x00, 0x00, 0x22, 0x00, // ..P...G...%...".
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x25, 0x00, 0x00, 0x00, 0x21, 0x00, // ......G...%...!.
	0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x5b, 0x00, 0x00, 0x00, 0x1e, 0x00, // ..@...G...[.....
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x5f, 0x00, 0x00, 0x00, 0x1e, 0x00, // ......G..._.....
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x47, 0x00, 0x04, 0x00, 0x6a, 0x00, 0x00, 0x00, 0x1e, 0x00, // ......G...j.....
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x21, 0x00, // ..............!.
	0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1a, 0x00, 0x02, 0x00, 0x06, 0x00, // ................
	0x00, 0x00, 0x16, 0x00, 0x03, 0x00, 0x07, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x19, 0x00, // .......... .....
	0x09, 0x00, 0x08, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
	0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x02, 0x00, // ................
	0x00, 0x00, 0x17, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x04, 0x00, // ................
	0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, // .. ...!.........
	0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x21, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, // ..;...!...".....
	0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, // .. ...$.........
	0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x24, 0x00, 0x00, 0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, // ..;...$...%.....
	0x00, 0x00, 0x1b, 0x00, 0x03, 0x00, 0x31, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x2b, 0x00, // ......1.......+.
	0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, 0x2b, 0x00, // ......C......?+.
	0x04, 0x00, 0x07, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x0a, 0xd7, 0x23, 0x3c, 0x14, 0x00, // ......P.....#<..
	0x02, 0x00, 0x51, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x5a, 0x00, 0x00, 0x00, 0x01, 0x00, // ..Q... ...Z.....
	0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x5a, 0x00, 0x00, 0x00, 0x5b, 0x00, // ......;...Z...[.
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x5e, 0x00, 0x00, 0x00, 0x01, 0x00, // ...... ...^.....
	0x00, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x5e, 0x00, 0x00, 0x00, 0x5f, 0x00, // ......;...^..._.
	0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x04, 0x00, 0x69, 0x00, 0x00, 0x00, 0x03, 0x00, // ...... ...i.....
	0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x3b, 0x00, 0x04, 0x00, 0x69, 0x00, 0x00, 0x00, 0x6a, 0x00, // ......;...i...j.
	0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x36, 0x00, 0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x04, 0x00, // ......6.........
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00, 0x05, 0x00, // ................
	0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x22, 0x00, // ..=.......#...".
	0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x26, 0x00, 0x00, 0x00, 0x25, 0x00, // ..=.......&...%.
	0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x00, 0x00, 0x5b, 0x00, // ..=...........[.
	0x00, 0x00, 0x3d, 0x00, 0x04, 0x00, 0x0b, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x5f, 0x00, // ..=.......`..._.
	0x00, 0x00, 0x56, 0x00, 0x05, 0x00, 0x31, 0x00, 0x00, 0x00, 0xa3, 0x00, 0x00, 0x00, 0x26, 0x00, // ..V...1.......&.
	0x00, 0x00, 0x23, 0x00, 0x00, 0x00, 0x57, 0x00, 0x05, 0x00, 0x0d, 0x00, 0x00, 0x00, 0xa5, 0x00, // ..#...W.........
	0x00, 0x00, 0xa3, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x51, 0x00, 0x05, 0x00, 0x07, 0x00, // ......`...Q.....
	0x00, 0x00, 0x8b, 0x00, 0x00, 0x00, 0xa5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x00, // ..............P.
	0x07, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x8c, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00, 0x00, 0x43, 0x00, // ..........C...C.
	0x00, 0x00, 0x43, 0x00, 0x00, 0x00, 0x8b, 0x00, 0x00, 0x00, 0xb8, 0x00, 0x05, 0x00, 0x51, 0x00, // ..C...........Q.
	0x00, 0x00, 0x8f, 0x00, 0x00, 0x00, 0x8b, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0xf7, 0x00, // ..........P.....
	0x03, 0x00, 0x91, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfa, 0x00, 0x04, 0x00, 0x8f, 0x00, // ................
	0x00, 0x00, 0x90, 0x00, 0x00, 0x00, 0x91, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x02, 0x00, 0x90, 0x00, // ................
	0x00, 0x00, 0xfc, 0x00, 0x01, 0x00, 0xf8, 0x00, 0x02, 0x00, 0x91, 0x00, 0x00, 0x00, 0x85, 0x00, // ................
	0x05, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x94, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x00, 0x00, 0x8c, 0x00, // ................
	0x00, 0x00, 0x3e, 0x00, 0x03, 0x00, 0x6a, 0x00, 0x00, 0x00, 0x94, 0x00, 0x00, 0x00, 0xfd, 0x00, // ..>...j.........
	0x01, 0x00, 0x38, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,                                     // ..8.......
};
static const uint8_t fs_text_dx9[333] =
{
	0x46, 0x53, 0x48, 0x08, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x73, // FSH............s
	0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x30, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // _fontTex0.......
	0x28, 0x01, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xfe, 0xff, 0x20, 0x00, 0x43, 0x54, 0x41, 0x42, // (......... .CTAB
	0x1c, 0x00, 0x00, 0x00, 0x53, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, // ....S...........
	0x1c, 0x00, 0x00, 0x00, 0x00, 0x91, 0x00, 0x00, 0x4c, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, // ........L...0...
	0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ........<.......
	0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x00, 0xab, 0xab, 0x04, 0x00, 0x0c, 0x00, // s_fontTex.......
	0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x73, 0x5f, 0x33, // ............ps_3
	0x5f, 0x30, 0x00, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x73, 0x6f, 0x66, 0x74, 0x20, 0x28, 0x52, 0x29, // _0.Microsoft (R)
	0x20, 0x48, 0x4c, 0x53, 0x4c, 0x20, 0x53, 0x68, 0x61, 0x64, 0x65, 0x72, 0x20, 0x43, 0x6f, 0x6d, //  HLSL Shader Com
	0x70, 0x69, 0x6c, 0x65, 0x72, 0x20, 0x31, 0x30, 0x2e, 0x31, 0x00, 0xab, 0x51, 0x00, 0x00, 0x05, // piler 10.1..Q...
	0x00, 0x00, 0x0f, 0xa0, 0x0a, 0xd7, 0x23, 0xbc, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0xbf, // ......#.........
	0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x02, 0x0a, 0x00, 0x00, 0x80, 0x00, 0x00, 0x0f, 0x90, // ................
	0x1f, 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x80, 0x01, 0x00, 0x03, 0x90, 0x1f, 0x00, 0x00, 0x02, // ................
	0x00, 0x00, 0x00, 0x90, 0x00, 0x08, 0x0f, 0xa0, 0x42, 0x00, 0x00, 0x03, 0x00, 0x00, 0x0f, 0x80, // ........B.......
	0x01, 0x00, 0xe4, 0x90, 0x00, 0x08, 0xe4, 0xa0, 0x02, 0x00, 0x00, 0x03, 0x00, 0x00, 0x02, 0x80, // ................
	0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0xa0, 0x58, 0x00, 0x00, 0x04, 0x01, 0x00, 0x0f, 0x80, // ........X.......
	0x00, 0x00, 0x55, 0x80, 0x00, 0x00, 0x55, 0xa0, 0x00, 0x00, 0xaa, 0xa0, 0x41, 0x00, 0x00, 0x01, // ..U...U.....A...
	0x01, 0x00, 0x0f, 0x80, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0f, 0x80, 0x00, 0x00, 0x00, 0x80, // ................
	0x00, 0x00, 0x95, 0xa1, 0x00, 0x00, 0x6a, 0xa1, 0x05, 0x00, 0x00, 0x03, 0x00, 0x08, 0x0f, 0x80, // ......j.........
	0x00, 0x00, 0xe4, 0x80, 0x00, 0x00, 0xe4, 0x90, 0xff, 0xff, 0x00, 0x00, 0x00,                   // .............
};
static const uint8_t fs_text_dx11[468] =
{
	0x46, 0x53, 0x48, 0x08, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x09, 0x73, // FSH............s
	0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x30, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // _fontTex0.......
	0xac, 0x01, 0x00, 0x00, 0x44, 0x58, 0x42, 0x43, 0x51, 0x15, 0x8c, 0x2a, 0xa8, 0x62, 0xe4, 0xc4, // ....DXBCQ..*.b..
	0xf9, 0xf1, 0x3c, 0xdc, 0x65, 0x53, 0x26, 0x09, 0x01, 0x00, 0x00, 0x00, 0xac, 0x01, 0x00, 0x00, // ..<.eS&.........
	0x03, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00, 0x00, 0xa0, 0x00, 0x00, 0x00, 0xd4, 0x00, 0x00, 0x00, // ....,...........
	0x49, 0x53, 0x47, 0x4e, 0x6c, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, // ISGNl...........
	0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, // P...............
	0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ................
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x00, 0x00, // ................
	0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, // b...............
	0x02, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x50, 0x4f, 0x53, 0x49, 0x54, // ........SV_POSIT
	0x49, 0x4f, 0x4e, 0x00, 0x43, 0x4f, 0x4c, 0x4f, 0x52, 0x00, 0x54, 0x45, 0x58, 0x43, 0x4f, 0x4f, // ION.COLOR.TEXCOO
	0x52, 0x44, 0x00, 0xab, 0x4f, 0x53, 0x47, 0x4e, 0x2c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, // RD..OSGN,.......
	0x08, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // .... ...........
	0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x53, 0x56, 0x5f, 0x54, // ............SV_T
	0x41, 0x52, 0x47, 0x45, 0x54, 0x00, 0xab, 0xab, 0x53, 0x48, 0x44, 0x52, 0xd0, 0x00, 0x00, 0x00, // ARGET...SHDR....
	0x40, 0x00, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x5a, 0x00, 0x00, 0x03, 0x00, 0x60, 0x10, 0x00, // @...4...Z....`..
	0x00, 0x00, 0x00, 0x00, 0x58, 0x18, 0x00, 0x04, 0x00, 0x70, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, // ....X....p......
	0x55, 0x55, 0x00, 0x00, 0x62, 0x10, 0x00, 0x03, 0xf2, 0x10, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, // UU..b...........
	0x62, 0x10, 0x00, 0x03, 0x32, 0x10, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x03, // b...2.......e...
	0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0x00, // . ......h.......
	0x45, 0x00, 0x00, 0x09, 0xf2, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x10, 0x10, 0x00, // E...........F...
	0x02, 0x00, 0x00, 0x00, 0x96, 0x73, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x10, 0x00, // .....s.......`..
	0x00, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00, 0x07, 0x22, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, // ....1...".......
	0x3a, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x0a, 0xd7, 0x23, 0x3c, // :........@....#<
	0x0d, 0x00, 0x04, 0x03, 0x1a, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x05, // ............6...
	0x12, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 0x80, 0x3f, // .........@.....?
	0x38, 0x00, 0x00, 0x07, 0xf2, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0c, 0x10, 0x00, // 8.... ..........
	0x00, 0x00, 0x00, 0x00, 0x46, 0x1e, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x01, // ....F.......>...
	0x00, 0x00, 0x00, 0x00,                                                                         // ....
};
static const uint8_t fs_text_mtl[791] =
{
	0x46, 0x53, 0x48, 0x08, 0x01, 0x83, 0xf2, 0xe1, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x10, 0x73, // FSH............s
	0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x11, // _fontTexSampler.
	0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x10, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, // ........s_fontTe
	0x78, 0x54, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x11, 0x01, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, // xTexture........
	0x09, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, // .s_fontTex......
	0x00, 0x00, 0xbd, 0x02, 0x00, 0x00, 0x23, 0x69, 0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x20, 0x3c, // ......#include <
	0x6d, 0x65, 0x74, 0x61, 0x6c, 0x5f, 0x73, 0x74, 0x64, 0x6c, 0x69, 0x62, 0x3e, 0x0a, 0x23, 0x69, // metal_stdlib>.#i
	0x6e, 0x63, 0x6c, 0x75, 0x64, 0x65, 0x20, 0x3c, 0x73, 0x69, 0x6d, 0x64, 0x2f, 0x73, 0x69, 0x6d, // nclude <simd/sim
	0x64, 0x2e, 0x68, 0x3e, 0x0a, 0x0a, 0x75, 0x73, 0x69, 0x6e, 0x67, 0x20, 0x6e, 0x61, 0x6d, 0x65, // d.h>..using name
	0x73, 0x70, 0x61, 0x63, 0x65, 0x20, 0x6d, 0x65, 0x74, 0x61, 0x6c, 0x3b, 0x0a, 0x0a, 0x73, 0x74, // space metal;..st
	0x72, 0x75, 0x63, 0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, // ruct xlatMtlMain
	0x5f, 0x6f, 0x75, 0x74, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, // _out.{.    float
	0x34, 0x20, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x44, 0x61, 0x74, 0x61, 0x30, // 4 bgfx_FragData0
	0x20, 0x5b, 0x5b, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x28, 0x30, 0x29, 0x5d, 0x5d, 0x3b, 0x0a, 0x7d, //  [[color(0)]];.}
	0x3b, 0x0a, 0x0a, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, // ;..struct xlatMt
	0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x69, 0x6e, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, // lMain_in.{.    f
	0x6c, 0x6f, 0x61, 0x74, 0x34, 0x20, 0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, 0x20, 0x5b, // loat4 v_color0 [
	0x5b, 0x75, 0x73, 0x65, 0x72, 0x28, 0x6c, 0x6f, 0x63, 0x6e, 0x30, 0x29, 0x5d, 0x5d, 0x3b, 0x0a, // [user(locn0)]];.
	0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x32, 0x20, 0x76, 0x5f, 0x74, 0x65, 0x78, //     float2 v_tex
	0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x20, 0x5b, 0x5b, 0x75, 0x73, 0x65, 0x72, 0x28, 0x6c, 0x6f, // coord0 [[user(lo
	0x63, 0x6e, 0x31, 0x29, 0x5d, 0x5d, 0x3b, 0x0a, 0x7d, 0x3b, 0x0a, 0x0a, 0x66, 0x72, 0x61, 0x67, // cn1)]];.};..frag
	0x6d, 0x65, 0x6e, 0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, // ment xlatMtlMain
	0x5f, 0x6f, 0x75, 0x74, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, // _out xlatMtlMain
	0x28, 0x78, 0x6c, 0x61, 0x74, 0x4d, 0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x69, 0x6e, 0x20, // (xlatMtlMain_in 
	0x69, 0x6e, 0x20, 0x5b, 0x5b, 0x73, 0x74, 0x61, 0x67, 0x65, 0x5f, 0x69, 0x6e, 0x5d, 0x5d, 0x2c, // in [[stage_in]],
	0x20, 0x74, 0x65, 0x78, 0x74, 0x75, 0x72, 0x65, 0x32, 0x64, 0x3c, 0x66, 0x6c, 0x6f, 0x61, 0x74, //  texture2d<float
	0x3e, 0x20, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x20, 0x5b, 0x5b, 0x74, 0x65, // > s_fontTex [[te
	0x78, 0x74, 0x75, 0x72, 0x65, 0x28, 0x30, 0x29, 0x5d, 0x5d, 0x2c, 0x20, 0x73, 0x61, 0x6d, 0x70, // xture(0)]], samp
	0x6c, 0x65, 0x72, 0x20, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, 0x53, 0x61, 0x6d, // ler s_fontTexSam
	0x70, 0x6c, 0x65, 0x72, 0x20, 0x5b, 0x5b, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x28, 0x30, // pler [[sampler(0
	0x29, 0x5d, 0x5d, 0x29, 0x0a, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x78, 0x6c, 0x61, 0x74, 0x4d, // )]]).{.    xlatM
	0x74, 0x6c, 0x4d, 0x61, 0x69, 0x6e, 0x5f, 0x6f, 0x75, 0x74, 0x20, 0x6f, 0x75, 0x74, 0x20, 0x3d, // tlMain_out out =
	0x20, 0x7b, 0x7d, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x20, //  {};.    float4 
	0x5f, 0x31, 0x36, 0x32, 0x20, 0x3d, 0x20, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, 0x78, // _162 = s_fontTex
	0x2e, 0x73, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x28, 0x73, 0x5f, 0x66, 0x6f, 0x6e, 0x74, 0x54, 0x65, // .sample(s_fontTe
	0x78, 0x53, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x72, 0x2c, 0x20, 0x69, 0x6e, 0x2e, 0x76, 0x5f, 0x74, // xSampler, in.v_t
	0x65, 0x78, 0x63, 0x6f, 0x6f, 0x72, 0x64, 0x30, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x66, // excoord0);.    f
	0x6c, 0x6f, 0x61, 0x74, 0x20, 0x5f, 0x31, 0x33, 0x36, 0x20, 0x3d, 0x20, 0x5f, 0x31, 0x36, 0x32, // loat _136 = _162
	0x2e, 0x78, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x69, 0x66, 0x20, 0x28, 0x5f, 0x31, 0x33, 0x36, // .x;.    if (_136
	0x20, 0x3c, 0x20, 0x30, 0x2e, 0x30, 0x30, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x39, 0x37, 0x37, //  < 0.00999999977
	0x36, 0x34, 0x38, 0x32, 0x35, 0x38, 0x32, 0x30, 0x39, 0x32, 0x32, 0x38, 0x35, 0x31, 0x35, 0x36, // 6482582092285156
	0x32, 0x35, 0x29, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, // 25).    {.      
	0x20, 0x20, 0x64, 0x69, 0x73, 0x63, 0x61, 0x72, 0x64, 0x5f, 0x66, 0x72, 0x61, 0x67, 0x6d, 0x65, //   discard_fragme
	0x6e, 0x74, 0x28, 0x29, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x20, 0x7d, 0x0a, 0x20, 0x20, 0x20, 0x20, // nt();.    }.    
	0x6f, 0x75, 0x74, 0x2e, 0x62, 0x67, 0x66, 0x78, 0x5f, 0x46, 0x72, 0x61, 0x67, 0x44, 0x61, 0x74, // out.bgfx_FragDat
	0x61, 0x30, 0x20, 0x3d, 0x20, 0x69, 0x6e, 0x2e, 0x76, 0x5f, 0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x30, // a0 = in.v_color0
	0x20, 0x2a, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x34, 0x28, 0x31, 0x2e, 0x30, 0x2c, 0x20, 0x31, //  * float4(1.0, 1
	0x2e, 0x30, 0x2c, 0x20, 0x31, 0x2e, 0x30, 0x2c, 0x20, 0x5f, 0x31, 0x33, 0x36, 0x29, 0x3b, 0x0a, // .0, 1.0, _136);.
	0x20, 0x20, 0x20, 0x20, 0x72, 0x65, 0x74, 0x75, 0x72, 0x6e, 0x20, 0x6f, 0x75, 0x74, 0x3b, 0x0a, //     return out;.
	0x7d, 0x0a, 0x0a, 0x00, 0x00, 0x20, 0x00,                                                       // }.... .
};
extern const uint8_t* fs_text_pssl;
extern const uint32_t fs_text_pssl_size;
