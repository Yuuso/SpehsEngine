#include "stdafx.h"
#include "SpehsEngine/Input/Key.h"

namespace se
{
	namespace input
	{
		const char* toString(const Key key)
		{
			switch (key)
			{
			case Key::UNKNOWN: return "Unknown";
			case Key::RETURN: return"Return";
			case Key::ESCAPE: return"Escape";
			case Key::BACKSPACE: return"KP_3";
			case Key::TAB: return"Tab";
			case Key::SPACE: return"Space";
			case Key::EXCLAIM: return"Exclaim";
			case Key::QUOTEDBL: return"\"";
			case Key::HASH: return"#";
			case Key::PERCENT: return"%";
			case Key::DOLLAR: return"$";
			case Key::AMPERSAND: return"&";
			case Key::QUOTE: return"'";
			case Key::LEFTPAREN: return "(";
			case Key::RIGHTPAREN: return ")";
			case Key::ASTERISK: return "*";
			case Key::PLUS: return "+";
			case Key::COMMA: return ",";
			case Key::MINUS: return "-";
			case Key::PERIOD: return ".";
			case Key::SLASH: return "/";
			case Key::KEY_0: return "0";
			case Key::KEY_1: return "1";
			case Key::KEY_2: return "2";
			case Key::KEY_3: return "3";
			case Key::KEY_4: return "4";
			case Key::KEY_5: return "5";
			case Key::KEY_6: return "6";
			case Key::KEY_7: return "7";
			case Key::KEY_8: return "8";
			case Key::KEY_9: return "9";
			case Key::COLON: return ":";
			case Key::SEMICOLON: return ";";
			case Key::LESS: return "<";
			case Key::EQUALS: return "=";
			case Key::GREATER: return ">";
			case Key::QUESTION: return "?";
			case Key::AT: return "@";
			case Key::LEFTBRACKET: return "{";
			case Key::BACKSLASH: return "\\";
			case Key::RIGHTBRACKET: return "}";
			case Key::CARET: return "^";
			case Key::UNDERSCORE: return "_";
			case Key::BACKQUOTE: return "`";
			case Key::A: return "A";
			case Key::B: return "B";
			case Key::C: return "C";
			case Key::D: return "D";
			case Key::E: return "E";
			case Key::F: return "F";
			case Key::G: return "G";
			case Key::H: return "H";
			case Key::I: return "I";
			case Key::J: return "J";
			case Key::K: return "K";
			case Key::L: return "L";
			case Key::M: return "M";
			case Key::N: return "N";
			case Key::O: return "O";
			case Key::P: return "P";
			case Key::Q: return "Q";
			case Key::R: return "R";
			case Key::S: return "S";
			case Key::T: return "T";
			case Key::U: return "U";
			case Key::V: return "V";
			case Key::W: return "W";
			case Key::X: return "X";
			case Key::Y: return "Y";
			case Key::Z: return "Z";
			case Key::CAPSLOCK: return "Caps lock";
			case Key::F1: return "F1";
			case Key::F2: return "F2";
			case Key::F3: return "F3";
			case Key::F4: return "F4";
			case Key::F5: return "F5";
			case Key::F6: return "F6";
			case Key::F7: return "F7";
			case Key::F8: return "F8";
			case Key::F9: return "F9";
			case Key::F10: return "F10";
			case Key::F11: return "F11";
			case Key::F12: return "F12";
			case Key::PRINTSCREEN: return "Print screen";
			case Key::SCROLLLOCK: return "Scroll lock";
			case Key::PAUSE: return "Pause";
			case Key::INSERT: return "Insert";
			case Key::HOME: return "Home";
			case Key::PAGEUP: return "Page up";
			case Key::KEY_DELETE: return "Delete";
			case Key::END: return "End";
			case Key::PAGEDOWN: return "Page down";
			case Key::RIGHT: return "Right arrow";
			case Key::LEFT: return "Left arrow";
			case Key::DOWN: return "Down arrow";
			case Key::UP: return "Up arrow";
			case Key::NUMLOCKCLEAR: return "Num lock/clear";
			case Key::KP_DIVIDE: return "KP /";
			case Key::KP_MULTIPLY: return "KP *";
			case Key::KP_MINUS: return "KP -";
			case Key::KP_PLUS: return "KP +";
			case Key::KP_ENTER: return "KP Enter";
			case Key::KP_1: return "KP 1";
			case Key::KP_2: return "KP 2";
			case Key::KP_3: return "KP 3";
			case Key::KP_4: return "KP 4";
			case Key::KP_5: return "KP 5";
			case Key::KP_6: return "KP 6";
			case Key::KP_7: return "KP 7";
			case Key::KP_8: return "KP 8";
			case Key::KP_9: return "KP 9";
			case Key::KP_0: return "KP 0";
			case Key::KP_PERIOD: return "KP .";
			case Key::APPLICATION: return "Application";
			case Key::POWER: return "Power";
			case Key::KP_EQUALS: return "KP =";
			case Key::F13: return "F13";
			case Key::F14: return "F14";
			case Key::F15: return "F15";
			case Key::F16: return "F16";
			case Key::F17: return "F17";
			case Key::F18: return "F18";
			case Key::F19: return "F19";
			case Key::F20: return "F20";
			case Key::F21: return "F21";
			case Key::F22: return "F22";
			case Key::F23: return "F23";
			case Key::F24: return "F24";
			case Key::EXECUTE: return "Execute";
			case Key::HELP: return "Help";
			case Key::MENU: return "Menu";
			case Key::SELECT: return "Select";
			case Key::STOP: return "Stop";
			case Key::AGAIN: return "Again";
			case Key::UNDO: return "Undo";
			case Key::CUT: return "Cut";
			case Key::COPY:return "Copy";
			case Key::PASTE: return "Paste";
			case Key::FIND: return "Find";
			case Key::MUTE: return "Mute";
			case Key::VOLUMEUP: return "Volume up";
			case Key::VOLUMEDOWN: return "Volume down";
			case Key::KP_COMMA: return "KP ,";
			case Key::KP_EQUALSAS400: return "KP EQUALSAS400";
			case Key::ALTERASE: return "Alterbase";
			case Key::SYSREQ: return "Sysreq";
			case Key::CANCEL: return "Cancel";
			case Key::CLEAR: return "Clear";
			case Key::PRIOR: return "Prior";
			case Key::RETURN2: return "Return2";
			case Key::SEPARATOR: return "Separator";
			case Key::KEY_OUT: return "Out";
			case Key::OPER: return "Oper";
			case Key::CLEARAGAIN: return "Clear again";
			case Key::CRSEL: return "Crsel";
			case Key::EXSEL: return "Exsel";
			case Key::KP_00: return "KP 00";
			case Key::KP_000: return "KP 000";
			case Key::THOUSANDSSEPARATOR: return "Thousands separator";
			case Key::DECIMALSEPARATOR: return "Decimal separator";
			case Key::CURRENCYUNIT: return "Currency unit";
			case Key::CURRENCYSUBUNIT: return "Currency sub unit";
			case Key::KP_LEFTPAREN: return "KP (";
			case Key::KP_RIGHTPAREN: return "KP )";
			case Key::KP_LEFTBRACE: return "KP {";
			case Key::KP_RIGHTBRACE: return "KP }";
			case Key::KP_TAB: return "KP Tab";
			case Key::KP_BACKSPACE: return "KP Backspace";
			case Key::KP_A: return "KP A";
			case Key::KP_B: return "KP B";
			case Key::KP_C: return "KP C";
			case Key::KP_D: return "KP D";
			case Key::KP_E: return "KP E";
			case Key::KP_F: return "KP F";
			case Key::KP_XOR: return "KP XOR";
			case Key::KP_POWER: return "KP POWER";
			case Key::KP_PERCENT: return "KP %";
			case Key::KP_LESS: return "KP <";
			case Key::KP_GREATER: return "KP >";
			case Key::KP_AMPERSAND: return "KP &";
			case Key::KP_DBLAMPERSAND: return "KP Double ampersand";
			case Key::KP_VERTICALBAR: return "KP Vertical bar";
			case Key::KP_DBLVERTICALBAR: return "KP Double vertical bar";
			case Key::KP_COLON: return "KP :";
			case Key::KP_HASH: return "KP #";
			case Key::KP_SPACE: return "KP Space";
			case Key::KP_AT: return "KP @";
			case Key::KP_EXCLAM: return "KP !";
			case Key::KP_MEMSTORE: return "KP Mem store";
			case Key::KP_MEMRECALL: return "KP Mem recall";
			case Key::KP_MEMCLEAR: return "KP Mem clear";
			case Key::KP_MEMADD: return "KP Mem add";
			case Key::KP_MEMSUBTRACT: return "KP Mem subtract";
			case Key::KP_MEMMULTIPLY: return "KP Mem multiply";
			case Key::KP_MEMDIVIDE: return "KP Mem divide";
			case Key::KP_PLUSMINUS: return "KP Plus minus";
			case Key::KP_CLEAR: return "KP Clear";
			case Key::KP_CLEARENTRY: return "KP Clear entry";
			case Key::KP_BINARY: return "KP Binary";
			case Key::KP_OCTAL: return "KP Octal";
			case Key::KP_DECIMAL: return "KP Decimal";
			case Key::KP_HEXADECIMAL: return "KP Hexadecimal";
			case Key::LCTRL: return "Left ctrl";
			case Key::LSHIFT: return "Left shift";
			case Key::LALT: return "Left alt";
			case Key::LGUI: return "Left GUI";
			case Key::RCTRL: return "Right control";
			case Key::RSHIFT: return "Right shift";
			case Key::RALT: return "Right alt";
			case Key::RGUI: return "Right GUI";
			case Key::MODE: return "Mode";
			case Key::AUDIONEXT: return "Audio next";
			case Key::AUDIOPREV: return "Audio prev";
			case Key::AUDIOSTOP: return "Audio stop";
			case Key::AUDIOPLAY: return "Audio play";
			case Key::AUDIOMUTE: return "Audio mute";
			case Key::MEDIASELECT: return "Media select";
			case Key::WWW: return "www";
			case Key::MAIL: return "Mail";
			case Key::CALCULATOR: return "Calculator";
			case Key::COMPUTER: return "Computer";
			case Key::AC_SEARCH: return "AC Search";
			case Key::AC_HOME: return "AC Home";
			case Key::AC_BACK: return "AC Back";
			case Key::AC_FORWARD: return "AC Forward";
			case Key::AC_STOP: return "AC Stop";
			case Key::AC_REFRESH: return "AC Refresh";
			case Key::AC_BOOKMARKS: return "AC Bookmarks";
			case Key::BRIGHTNESSDOWN: return "Brightness down";
			case Key::BRIGHTNESSUP: return "Brightness up";
			case Key::DISPLAYSWITCH: return "Display switch";
			case Key::KBDILLUMTOGGLE: return "Illum toggle";
			case Key::KBDILLUMDOWN: return "Illum down";
			case Key::KBDILLUMUP: return "Illum up";
			case Key::EJECT: return "Eject";
			case Key::SLEEP: return "Sleep";
			}
			return "Error";
		}

		const char* toShortString(const Key key)
		{
			// Shorten everything down to max 4 characters
			switch (key)
			{
			case Key::UNKNOWN: return "Unkn";
			case Key::RETURN: return"Ret";
			case Key::ESCAPE: return"Esc";
			case Key::BACKSPACE: return "KP_3";
			case Key::TAB: return "Tab";
			case Key::SPACE: return "Spac";
			case Key::EXCLAIM: return"!";
			case Key::QUOTEDBL: return"\"";
			case Key::HASH: return"#";
			case Key::PERCENT: return"%";
			case Key::DOLLAR: return"$";
			case Key::AMPERSAND: return"&";
			case Key::QUOTE: return"'";
			case Key::LEFTPAREN: return "(";
			case Key::RIGHTPAREN: return ")";
			case Key::ASTERISK: return "*";
			case Key::PLUS: return "+";
			case Key::COMMA: return ",";
			case Key::MINUS: return "-";
			case Key::PERIOD: return ".";
			case Key::SLASH: return "/";
			case Key::KEY_0: return "0";
			case Key::KEY_1: return "1";
			case Key::KEY_2: return "2";
			case Key::KEY_3: return "3";
			case Key::KEY_4: return "4";
			case Key::KEY_5: return "5";
			case Key::KEY_6: return "6";
			case Key::KEY_7: return "7";
			case Key::KEY_8: return "8";
			case Key::KEY_9: return "9";
			case Key::COLON: return ":";
			case Key::SEMICOLON: return ";";
			case Key::LESS: return "<";
			case Key::EQUALS: return "=";
			case Key::GREATER: return ">";
			case Key::QUESTION: return "?";
			case Key::AT: return "@";
			case Key::LEFTBRACKET: return "{";
			case Key::BACKSLASH: return "\\";
			case Key::RIGHTBRACKET: return "}";
			case Key::CARET: return "^";
			case Key::UNDERSCORE: return "_";
			case Key::BACKQUOTE: return "`";
			case Key::A: return "A";
			case Key::B: return "B";
			case Key::C: return "C";
			case Key::D: return "D";
			case Key::E: return "E";
			case Key::F: return "F";
			case Key::G: return "G";
			case Key::H: return "H";
			case Key::I: return "I";
			case Key::J: return "J";
			case Key::K: return "K";
			case Key::L: return "L";
			case Key::M: return "M";
			case Key::N: return "N";
			case Key::O: return "O";
			case Key::P: return "P";
			case Key::Q: return "Q";
			case Key::R: return "R";
			case Key::S: return "S";
			case Key::T: return "T";
			case Key::U: return "U";
			case Key::V: return "V";
			case Key::W: return "W";
			case Key::X: return "X";
			case Key::Y: return "Y";
			case Key::Z: return "Z";
			case Key::CAPSLOCK: return "Caps";
			case Key::F1: return "F1";
			case Key::F2: return "F2";
			case Key::F3: return "F3";
			case Key::F4: return "F4";
			case Key::F5: return "F5";
			case Key::F6: return "F6";
			case Key::F7: return "F7";
			case Key::F8: return "F8";
			case Key::F9: return "F9";
			case Key::F10: return "F10";
			case Key::F11: return "F11";
			case Key::F12: return "F12";
			case Key::PRINTSCREEN: return "Prnt";
			case Key::SCROLLLOCK: return "Scrl";
			case Key::PAUSE: return "Paus";
			case Key::INSERT: return "Insr";
			case Key::HOME: return "Home";
			case Key::PAGEUP: return "Pgup";
			case Key::KEY_DELETE: return "Del";
			case Key::END: return "End";
			case Key::PAGEDOWN: return "Pgdn";
			case Key::RIGHT: return "Rght";
			case Key::LEFT: return "Left";
			case Key::DOWN: return "Down";
			case Key::UP: return "Up";
			case Key::NUMLOCKCLEAR: return "Numl";
			case Key::KP_DIVIDE: return "KP /";
			case Key::KP_MULTIPLY: return "KP *";
			case Key::KP_MINUS: return "KP -";
			case Key::KP_PLUS: return "KP +";
			case Key::KP_ENTER: return "KP E";
			case Key::KP_1: return "KP 1";
			case Key::KP_2: return "KP 2";
			case Key::KP_3: return "KP 3";
			case Key::KP_4: return "KP 4";
			case Key::KP_5: return "KP 5";
			case Key::KP_6: return "KP 6";
			case Key::KP_7: return "KP 7";
			case Key::KP_8: return "KP 8";
			case Key::KP_9: return "KP 9";
			case Key::KP_0: return "KP 0";
			case Key::KP_PERIOD: return "KP .";
			case Key::APPLICATION: return "App";
			case Key::POWER: return "Pwr";
			case Key::KP_EQUALS: return "KP =";
			case Key::F13: return "F13";
			case Key::F14: return "F14";
			case Key::F15: return "F15";
			case Key::F16: return "F16";
			case Key::F17: return "F17";
			case Key::F18: return "F18";
			case Key::F19: return "F19";
			case Key::F20: return "F20";
			case Key::F21: return "F21";
			case Key::F22: return "F22";
			case Key::F23: return "F23";
			case Key::F24: return "F24";
			case Key::EXECUTE: return "Exec";
			case Key::HELP: return "Help";
			case Key::MENU: return "Menu";
			case Key::SELECT: return "Slct";
			case Key::STOP: return "Stop";
			case Key::AGAIN: return "Agn";
			case Key::UNDO: return "Undo";
			case Key::CUT: return "Cut";
			case Key::COPY:return "Copy";
			case Key::PASTE: return "Pste";
			case Key::FIND: return "Find";
			case Key::MUTE: return "Mute";
			case Key::VOLUMEUP: return "VolU";
			case Key::VOLUMEDOWN: return "VolD";
			case Key::KP_COMMA: return "KP ,";
			case Key::KP_EQUALSAS400: return "KPe4";
			case Key::ALTERASE: return "Alrs";
			case Key::SYSREQ: return "Sysr";
			case Key::CANCEL: return "Canc";
			case Key::CLEAR: return "Clr";
			case Key::PRIOR: return "Prr";
			case Key::RETURN2: return "Ret2";
			case Key::SEPARATOR: return "Sepr";
			case Key::KEY_OUT: return "Out";
			case Key::OPER: return "Oper";
			case Key::CLEARAGAIN: return "Clra";
			case Key::CRSEL: return "Crse";
			case Key::EXSEL: return "Exse";
			case Key::KP_00: return "KP 00";
			case Key::KP_000: return "KP 000";
			case Key::THOUSANDSSEPARATOR: return "Thsd";
			case Key::DECIMALSEPARATOR: return "Decs";
			case Key::CURRENCYUNIT: return "Cuni";
			case Key::CURRENCYSUBUNIT: return "Csun";
			case Key::KP_LEFTPAREN: return "KP (";
			case Key::KP_RIGHTPAREN: return "KP )";
			case Key::KP_LEFTBRACE: return "KP {";
			case Key::KP_RIGHTBRACE: return "KP }";
			case Key::KP_TAB: return "KP Tab";
			case Key::KP_BACKSPACE: return "KPbs";
			case Key::KP_A: return "KP A";
			case Key::KP_B: return "KP B";
			case Key::KP_C: return "KP C";
			case Key::KP_D: return "KP D";
			case Key::KP_E: return "KP E";
			case Key::KP_F: return "KP F";
			case Key::KP_XOR: return "KP ^";
			case Key::KP_POWER: return "KP P";
			case Key::KP_PERCENT: return "KP %";
			case Key::KP_LESS: return "KP <";
			case Key::KP_GREATER: return "KP >";
			case Key::KP_AMPERSAND: return "KP &";
			case Key::KP_DBLAMPERSAND: return "KP&&";
			case Key::KP_VERTICALBAR: return "KPvp";
			case Key::KP_DBLVERTICALBAR: return "KPdv";
			case Key::KP_COLON: return "KP :";
			case Key::KP_HASH: return "KP #";
			case Key::KP_SPACE: return "KPsp";
			case Key::KP_AT: return "KP @";
			case Key::KP_EXCLAM: return "KP !";
			case Key::KP_MEMSTORE: return "KPms";
			case Key::KP_MEMRECALL: return "KPmr";
			case Key::KP_MEMCLEAR: return "KPmc";
			case Key::KP_MEMADD: return "KPm+";
			case Key::KP_MEMSUBTRACT: return "KPm-";
			case Key::KP_MEMMULTIPLY: return "KPm*";
			case Key::KP_MEMDIVIDE: return "KPm/";
			case Key::KP_PLUSMINUS: return "KPm-";
			case Key::KP_CLEAR: return "KPcl";
			case Key::KP_CLEARENTRY: return "KPce";
			case Key::KP_BINARY: return "KPbn";
			case Key::KP_OCTAL: return "KPoc";
			case Key::KP_DECIMAL: return "KPdc";
			case Key::KP_HEXADECIMAL: return "KP0x";
			case Key::LCTRL: return "Lctr";
			case Key::LSHIFT: return "Lshf";
			case Key::LALT: return "Lalt";
			case Key::LGUI: return "LGUI";
			case Key::RCTRL: return "Rctr";
			case Key::RSHIFT: return "Rshf";
			case Key::RALT: return "Ralt";
			case Key::RGUI: return "RGUI";
			case Key::MODE: return "Mode";
			case Key::AUDIONEXT: return "Anxt";
			case Key::AUDIOPREV: return "Aprn";
			case Key::AUDIOSTOP: return "Astp";
			case Key::AUDIOPLAY: return "Apla";
			case Key::AUDIOMUTE: return "Amut";
			case Key::MEDIASELECT: return "Mslc";
			case Key::WWW: return "www";
			case Key::MAIL: return "Mail";
			case Key::CALCULATOR: return "Calc";
			case Key::COMPUTER: return "Comp";
			case Key::AC_SEARCH: return "ACse";
			case Key::AC_HOME: return "AChm";
			case Key::AC_BACK: return "ACbc";
			case Key::AC_FORWARD: return "ACfw";
			case Key::AC_STOP: return "ACst";
			case Key::AC_REFRESH: return "ACrf";
			case Key::AC_BOOKMARKS: return "ACbm";
			case Key::BRIGHTNESSDOWN: return "Brt-";
			case Key::BRIGHTNESSUP: return "Brt+";
			case Key::DISPLAYSWITCH: return "Disp";
			case Key::KBDILLUMTOGGLE: return "Ilto";
			case Key::KBDILLUMDOWN: return "Ildo";
			case Key::KBDILLUMUP: return "Ilup";
			case Key::EJECT: return "Ejct";
			case Key::SLEEP: return "Slp";
			}
			return "Error";
		}
	}
}
