#pragma once

#include <stdint.h>


namespace se
{
	enum HexColor : uint32_t
	{
		Clear					= 0x0,
		Transparent				= 0x0,

		AlphaMask				= 0xffffff00,

		// HTML colors in 0xRRGGBBAA format with full opacity

		LightSalmon				= 0xffa07aff,
		Salmon					= 0xfa8072ff,
		DarkSalmon				= 0xe9967aff,
		LightCoral				= 0xf08080ff,
		IndianRed				= 0xcd5c5cff,
		Crimson					= 0xdc143cff,
		FireBrick				= 0xb22222ff,
		Red						= 0xff0000ff,
		DarkRed					= 0x8b0000ff,

		Coral					= 0xff7f50ff,
		Tomato					= 0xff6347ff,
		OrangeRed				= 0xff4500ff,
		Gold					= 0xffd700ff,
		Orange					= 0xffa500ff,
		DarkOrange				= 0xff8c00ff,

		LightYellow				= 0xffffe0ff,
		LemonChiffon			= 0xfffacdff,
		LightGoldenrodYellow	= 0xfafad2ff,
		PapayaWhip				= 0xffefd5ff,
		Moccasin				= 0xffe4b5ff,
		PeachPuff				= 0xffdab9ff,
		PaleGoldenrod			= 0xeee8aaff,
		Khaki					= 0xf0e68cff,
		DarkKhaki				= 0xbdb76bff,
		Yellow					= 0xffff00ff,

		LawnGreen				= 0x7cfc00ff,
		Chartreuse				= 0x7fff00ff,
		LimeGreen				= 0x32cd32ff,
		Lime					= 0x00ff00ff,
		ForestGreen				= 0x228b22ff,
		Green					= 0x008000ff,
		DarkGreen				= 0x006400ff,
		GreenYellow				= 0xadff2fff,
		YellowGreen				= 0x9acd32ff,
		SpringGreen				= 0x00ff7fff,
		MediumSpringGreen		= 0x00fa9aff,
		LightGreen				= 0x90ee90ff,
		PaleGreen				= 0x98fb98ff,
		DarkSeaGreen			= 0x8fbc8fff,
		MediumSeaGreen			= 0x3cb371ff,
		SeaGreen				= 0x2e8b57ff,
		Olive					= 0x808000ff,
		DarkOliveGreen			= 0x556b2fff,
		OliveDrab				= 0x6b8e23ff,

		LightCyan				= 0xe0ffffff,
		Cyan					= 0x00ffffff,
		Aqua					= 0x00ffffff,
		Aquamarine				= 0x7fffd4ff,
		MediumAquamarine		= 0x66cdaaff,
		PaleTurquoise			= 0xafeeeeff,
		Turquoise				= 0x40e0d0ff,
		MediumTurquoise			= 0x48d1ccff,
		DarkTurquoise			= 0x00ced1ff,
		LightSeaGreen			= 0x20b2aaff,
		CadetBlue				= 0x5f9ea0ff,
		DarkCyan				= 0x008b8bff,
		Teal					= 0x008080ff,

		PowderBlue				= 0xb0e0e6ff,
		LightBlue				= 0xadd8e6ff,
		LightSkyBlue			= 0x87cefaff,
		SkyBlue					= 0x87ceebff,
		DeepSkyBlue				= 0x00bfffff,
		LightSteelBlue			= 0xb0c4deff,
		DodgerBlue				= 0x1e90ffff,
		CornflowerBlue			= 0x6495edff,
		SteelBlue				= 0x4682b4ff,
		RoyalBlue				= 0x4169e1ff,
		Blue					= 0x0000ffff,
		MediumBlue				= 0x0000cdff,
		DarkBlue				= 0x00008bff,
		Navy					= 0x000080ff,
		MidnightBlue			= 0x191970ff,
		MediumSlateBlue			= 0x7b68eeff,
		SlateBlue				= 0x6a5acdff,
		DarkSlateBlue			= 0x483d8bff,

		Lavender				= 0xe6e6faff,
		Thistle					= 0xd8bfd8ff,
		Plum					= 0xdda0ddff,
		Violet					= 0xee82eeff,
		Orchid					= 0xda70d6ff,
		Fuchsia					= 0xff00ffff,
		Magenta					= 0xff00ffff,
		MediumOrchid			= 0xba55d3ff,
		MediumPurple			= 0x9370dbff,
		BlueViolet				= 0x8a2be2ff,
		DarkViolet				= 0x9400d3ff,
		DarkOrchid				= 0x9932ccff,
		DarkMagenta				= 0x8b008bff,
		Purple					= 0x800080ff,
		Indigo					= 0x4b0082ff,

		Pink					= 0xffc0cbff,
 		LightPink				= 0xffb6c1ff,
 		HotPink					= 0xff69b4ff,
 		DeepPink				= 0xff1493ff,
 		PaleVioletRed			= 0xdb7093ff,
 		MediumVioletRed			= 0xc71585ff,

		White					= 0xffffffff,
		Snow					= 0xfffafaff,
		Honeydew				= 0xf0fff0ff,
		MintCream				= 0xf5fffaff,
		Azure					= 0xf0ffffff,
		AliceBlue				= 0xf0f8ffff,
		GhostWhite				= 0xf8f8ffff,
		WhiteSmoke				= 0xf5f5f5ff,
		Seashell				= 0xfff5eeff,
		Beige					= 0xf5f5dcff,
		OldLace					= 0xfdf5e6ff,
		FloralWhite				= 0xfffaf0ff,
		Ivory					= 0xfffff0ff,
		AntiqueWhite			= 0xfaebd7ff,
		Linen					= 0xfaf0e6ff,
		LavenderBlush			= 0xfff0f5ff,
		MistyRose				= 0xffe4e1ff,

		Gainsboro				= 0xdcdcdcff,
 		LightGray				= 0xd3d3d3ff,
 		Silver					= 0xc0c0c0ff,
 		DarkGray				= 0xa9a9a9ff,
 		Gray					= 0x808080ff,
 		DimGray					= 0x696969ff,
 		LightSlateGray			= 0x778899ff,
 		SlateGray				= 0x708090ff,
 		DarkSlateGray			= 0x2f4f4fff,
 		Black					= 0x000000ff,

		Cornsilk				= 0xfff8dcff,
		BlanchedAlmond			= 0xffebcdff,
		Bisque					= 0xffe4c4ff,
		NavajoWhite				= 0xffdeadff,
		Wheat					= 0xf5deb3ff,
		Burlywood				= 0xdeb887ff,
		Tan						= 0xd2b48cff,
		RosyBrown				= 0xbc8f8fff,
		SandyBrown				= 0xf4a460ff,
		Goldenrod				= 0xdaa520ff,
		Peru					= 0xcd853fff,
		Chocolate				= 0xd2691eff,
		SaddleBrown				= 0x8b4513ff,
		Sienna					= 0xa0522dff,
		Brown					= 0xa52a2aff,
		Maroon					= 0x800000ff,
	};
}