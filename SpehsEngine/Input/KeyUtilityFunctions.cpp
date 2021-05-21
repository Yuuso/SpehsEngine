#include "stdafx.h"
#include "SpehsEngine/Input/KeyUtilityFunctions.h"

#include "SDL/SDL.h"

//se_pragma_optimize(off);

namespace se
{
	namespace input
	{
		bool isKeyDown(const Key key)
		{
			int numkeys = 0;
			const Uint8 *keyboardState = SDL_GetKeyboardState(&numkeys);
			const int scancode = SDL_GetScancodeFromKey(SDL_Keycode(key));
			if (scancode >= numkeys)
			{
				se_assert(false && "key value is not supported");
				return false;
			}
			return keyboardState[scancode] != 0;
		}

		bool isCtrlDown()
		{
			const Uint8 *keyboardState = SDL_GetKeyboardState(nullptr);
			return keyboardState[SDL_Scancode::SDL_SCANCODE_LCTRL] != 0 || keyboardState[SDL_Scancode::SDL_SCANCODE_RCTRL] != 0;
		}

		bool isShiftDown()
		{
			const Uint8 *keyboardState = SDL_GetKeyboardState(nullptr);
			return keyboardState[SDL_Scancode::SDL_SCANCODE_LSHIFT] != 0 || keyboardState[SDL_Scancode::SDL_SCANCODE_RSHIFT] != 0;
		}

		bool isAltDown()
		{
			const Uint8 *keyboardState = SDL_GetKeyboardState(nullptr);
			return keyboardState[SDL_Scancode::SDL_SCANCODE_LALT] != 0 || keyboardState[SDL_Scancode::SDL_SCANCODE_RALT] != 0;
		}

		int getScancode(const Key key)
		{
			switch (key)
			{
			case Key::DOLLAR:
			case Key::QUOTEDBL:
			case Key::QUOTE:
			case Key::ASTERISK:
			case Key::QUESTION:
			case Key::CARET:
			case Key::UNDERSCORE:
			case Key::BACKQUOTE:
				// Not sure what the scan codes for these are
			case Key::UNKNOWN:
				return SDL_SCANCODE_UNKNOWN;

			case Key::RETURN: return SDL_SCANCODE_RETURN;
			case Key::ESCAPE: return SDL_SCANCODE_ESCAPE;
			case Key::BACKSPACE: return SDL_SCANCODE_BACKSPACE;
			case Key::TAB: return SDL_SCANCODE_TAB;
			case Key::SPACE: return SDL_SCANCODE_SPACE;
			case Key::EXCLAIM: return SDL_SCANCODE_KP_EXCLAM;
			case Key::HASH: return SDL_SCANCODE_KP_HASH;
			case Key::PERCENT: return SDL_SCANCODE_KP_PERCENT;
			case Key::AMPERSAND: return SDL_SCANCODE_KP_AMPERSAND;
			case Key::LEFTPAREN: return SDL_SCANCODE_KP_LEFTPAREN;
			case Key::RIGHTPAREN: return SDL_SCANCODE_KP_RIGHTPAREN;
			case Key::PLUS: return SDL_SCANCODE_KP_PLUS;
			case Key::COMMA: return SDL_SCANCODE_COMMA;
			case Key::MINUS: return SDL_SCANCODE_MINUS;
			case Key::PERIOD: return SDL_SCANCODE_PERIOD;
			case Key::SLASH: return SDL_SCANCODE_SLASH;
			case Key::KEY_0: return SDL_SCANCODE_0;
			case Key::KEY_1: return SDL_SCANCODE_1;
			case Key::KEY_2: return SDL_SCANCODE_2;
			case Key::KEY_3: return SDL_SCANCODE_3;
			case Key::KEY_4: return SDL_SCANCODE_4;
			case Key::KEY_5: return SDL_SCANCODE_5;
			case Key::KEY_6: return SDL_SCANCODE_6;
			case Key::KEY_7: return SDL_SCANCODE_7;
			case Key::KEY_8: return SDL_SCANCODE_8;
			case Key::KEY_9: return SDL_SCANCODE_9;
			case Key::COLON: return SDL_SCANCODE_KP_COLON;
			case Key::SEMICOLON: return SDL_SCANCODE_SEMICOLON;
			case Key::LESS: return SDL_SCANCODE_KP_LESS;
			case Key::EQUALS: return SDL_SCANCODE_EQUALS;
			case Key::GREATER: return SDL_SCANCODE_KP_GREATER;
			case Key::AT: return SDL_SCANCODE_KP_AT;
			case Key::LEFTBRACKET: return SDL_SCANCODE_LEFTBRACKET;
			case Key::BACKSLASH: return SDL_SCANCODE_BACKSLASH;
			case Key::RIGHTBRACKET: return SDL_SCANCODE_RIGHTBRACKET;
			case Key::A: return SDL_SCANCODE_A;
			case Key::B: return SDL_SCANCODE_B;
			case Key::C: return SDL_SCANCODE_C;
			case Key::D: return SDL_SCANCODE_D;
			case Key::E: return SDL_SCANCODE_E;
			case Key::F: return SDL_SCANCODE_F;
			case Key::G: return SDL_SCANCODE_G;
			case Key::H: return SDL_SCANCODE_H;
			case Key::I: return SDL_SCANCODE_I;
			case Key::J: return SDL_SCANCODE_J;
			case Key::K: return SDL_SCANCODE_K;
			case Key::L: return SDL_SCANCODE_L;
			case Key::M: return SDL_SCANCODE_M;
			case Key::N: return SDL_SCANCODE_N;
			case Key::O: return SDL_SCANCODE_O;
			case Key::P: return SDL_SCANCODE_P;
			case Key::Q: return SDL_SCANCODE_Q;
			case Key::R: return SDL_SCANCODE_R;
			case Key::S: return SDL_SCANCODE_S;
			case Key::T: return SDL_SCANCODE_T;
			case Key::U: return SDL_SCANCODE_U;
			case Key::V: return SDL_SCANCODE_V;
			case Key::W: return SDL_SCANCODE_W;
			case Key::X: return SDL_SCANCODE_X;
			case Key::Y: return SDL_SCANCODE_Y;
			case Key::Z: return SDL_SCANCODE_Z;
			case Key::CAPSLOCK: return SDL_SCANCODE_CAPSLOCK;
			case Key::F1: return SDL_SCANCODE_F1;
			case Key::F2: return SDL_SCANCODE_F2;
			case Key::F3: return SDL_SCANCODE_F3;
			case Key::F4: return SDL_SCANCODE_F4;
			case Key::F5: return SDL_SCANCODE_F5;
			case Key::F6: return SDL_SCANCODE_F6;
			case Key::F7: return SDL_SCANCODE_F7;
			case Key::F8: return SDL_SCANCODE_F8;
			case Key::F9: return SDL_SCANCODE_F9;
			case Key::F10: return SDL_SCANCODE_F10;
			case Key::F11: return SDL_SCANCODE_F11;
			case Key::F12: return SDL_SCANCODE_F12;
			case Key::PRINTSCREEN: return SDL_SCANCODE_PRINTSCREEN;
			case Key::SCROLLLOCK: return SDL_SCANCODE_SCROLLLOCK;
			case Key::PAUSE: return SDL_SCANCODE_PAUSE;
			case Key::INSERT: return SDL_SCANCODE_INSERT;
			case Key::HOME: return SDL_SCANCODE_HOME;
			case Key::PAGEUP: return SDL_SCANCODE_PAGEUP;
			case Key::KEY_DELETE: return SDL_SCANCODE_DELETE;
			case Key::END: return SDL_SCANCODE_END;
			case Key::PAGEDOWN: return SDL_SCANCODE_PAGEDOWN;
			case Key::RIGHT: return SDL_SCANCODE_RIGHT;
			case Key::LEFT: return SDL_SCANCODE_LEFT;
			case Key::DOWN: return SDL_SCANCODE_DOWN;
			case Key::UP: return SDL_SCANCODE_UP;
			case Key::NUMLOCKCLEAR: return SDL_SCANCODE_NUMLOCKCLEAR;
			case Key::KP_DIVIDE: return SDL_SCANCODE_KP_DIVIDE;
			case Key::KP_MULTIPLY: return SDL_SCANCODE_KP_MULTIPLY;
			case Key::KP_MINUS: return SDL_SCANCODE_KP_MINUS;
			case Key::KP_PLUS: return SDL_SCANCODE_KP_PLUS;
			case Key::KP_ENTER: return SDL_SCANCODE_KP_ENTER;
			case Key::KP_1: return SDL_SCANCODE_KP_1;
			case Key::KP_2: return SDL_SCANCODE_KP_2;
			case Key::KP_3: return SDL_SCANCODE_KP_3;
			case Key::KP_4: return SDL_SCANCODE_KP_4;
			case Key::KP_5: return SDL_SCANCODE_KP_5;
			case Key::KP_6: return SDL_SCANCODE_KP_6;
			case Key::KP_7: return SDL_SCANCODE_KP_7;
			case Key::KP_8: return SDL_SCANCODE_KP_8;
			case Key::KP_9: return SDL_SCANCODE_KP_9;
			case Key::KP_0: return SDL_SCANCODE_KP_0;
			case Key::KP_PERIOD: return SDL_SCANCODE_KP_PERIOD;
			case Key::APPLICATION: return SDL_SCANCODE_APPLICATION;
			case Key::POWER: return SDL_SCANCODE_POWER;
			case Key::KP_EQUALS: return SDL_SCANCODE_KP_EQUALS;
			case Key::F13: return SDL_SCANCODE_F13;
			case Key::F14: return SDL_SCANCODE_F14;
			case Key::F15: return SDL_SCANCODE_F15;
			case Key::F16: return SDL_SCANCODE_F16;
			case Key::F17: return SDL_SCANCODE_F17;
			case Key::F18: return SDL_SCANCODE_F18;
			case Key::F19: return SDL_SCANCODE_F19;
			case Key::F20: return SDL_SCANCODE_F20;
			case Key::F21: return SDL_SCANCODE_F21;
			case Key::F22: return SDL_SCANCODE_F22;
			case Key::F23: return SDL_SCANCODE_F23;
			case Key::F24: return SDL_SCANCODE_F24;
			case Key::EXECUTE: return SDL_SCANCODE_EXECUTE;
			case Key::HELP: return SDL_SCANCODE_HELP;
			case Key::MENU: return SDL_SCANCODE_MENU;
			case Key::SELECT: return SDL_SCANCODE_SELECT;
			case Key::STOP: return SDL_SCANCODE_STOP;
			case Key::AGAIN: return SDL_SCANCODE_AGAIN;
			case Key::UNDO: return SDL_SCANCODE_UNDO;
			case Key::CUT: return SDL_SCANCODE_CUT;
			case Key::COPY: return SDL_SCANCODE_COPY;
			case Key::PASTE: return SDL_SCANCODE_PASTE;
			case Key::FIND: return SDL_SCANCODE_FIND;
			case Key::MUTE: return SDL_SCANCODE_MUTE;
			case Key::VOLUMEUP: return SDL_SCANCODE_VOLUMEUP;
			case Key::VOLUMEDOWN: return SDL_SCANCODE_VOLUMEDOWN;
			case Key::KP_COMMA: return SDL_SCANCODE_KP_COMMA;
			case Key::KP_EQUALSAS400: return SDL_SCANCODE_KP_EQUALSAS400;
			case Key::ALTERASE: return SDL_SCANCODE_ALTERASE;
			case Key::SYSREQ: return SDL_SCANCODE_SYSREQ;
			case Key::CANCEL: return SDL_SCANCODE_CANCEL;
			case Key::CLEAR: return SDL_SCANCODE_CLEAR;
			case Key::PRIOR: return SDL_SCANCODE_PRIOR;
			case Key::RETURN2: return SDL_SCANCODE_RETURN2;
			case Key::SEPARATOR: return SDL_SCANCODE_SEPARATOR;
			case Key::KEY_OUT: return SDL_SCANCODE_OUT;
			case Key::OPER: return SDL_SCANCODE_OPER;
			case Key::CLEARAGAIN: return SDL_SCANCODE_CLEARAGAIN;
			case Key::CRSEL: return SDL_SCANCODE_CRSEL;
			case Key::EXSEL: return SDL_SCANCODE_EXSEL;
			case Key::KP_00: return SDL_SCANCODE_KP_00;
			case Key::KP_000: return SDL_SCANCODE_KP_000;
			case Key::THOUSANDSSEPARATOR: return SDL_SCANCODE_THOUSANDSSEPARATOR;
			case Key::DECIMALSEPARATOR: return SDL_SCANCODE_DECIMALSEPARATOR;
			case Key::CURRENCYUNIT: return SDL_SCANCODE_CURRENCYUNIT;
			case Key::CURRENCYSUBUNIT: return SDL_SCANCODE_CURRENCYSUBUNIT;
			case Key::KP_LEFTPAREN: return SDL_SCANCODE_KP_LEFTPAREN;
			case Key::KP_RIGHTPAREN: return SDL_SCANCODE_KP_RIGHTPAREN;
			case Key::KP_LEFTBRACE: return SDL_SCANCODE_KP_LEFTBRACE;
			case Key::KP_RIGHTBRACE: return SDL_SCANCODE_KP_RIGHTBRACE;
			case Key::KP_TAB: return SDL_SCANCODE_KP_TAB;
			case Key::KP_BACKSPACE: return SDL_SCANCODE_KP_BACKSPACE;
			case Key::KP_A: return SDL_SCANCODE_KP_A;
			case Key::KP_B: return SDL_SCANCODE_KP_B;
			case Key::KP_C: return SDL_SCANCODE_KP_C;
			case Key::KP_D: return SDL_SCANCODE_KP_D;
			case Key::KP_E: return SDL_SCANCODE_KP_E;
			case Key::KP_F: return SDL_SCANCODE_KP_F;
			case Key::KP_XOR: return SDL_SCANCODE_KP_XOR;
			case Key::KP_POWER: return SDL_SCANCODE_KP_POWER;
			case Key::KP_PERCENT: return SDL_SCANCODE_KP_PERCENT;
			case Key::KP_LESS: return SDL_SCANCODE_KP_LESS;
			case Key::KP_GREATER: return SDL_SCANCODE_KP_GREATER;
			case Key::KP_AMPERSAND: return SDL_SCANCODE_KP_AMPERSAND;
			case Key::KP_DBLAMPERSAND: return SDL_SCANCODE_KP_DBLAMPERSAND;
			case Key::KP_VERTICALBAR: return SDL_SCANCODE_KP_VERTICALBAR;
			case Key::KP_DBLVERTICALBAR: return SDL_SCANCODE_KP_DBLVERTICALBAR;
			case Key::KP_COLON: return SDL_SCANCODE_KP_COLON;
			case Key::KP_HASH: return SDL_SCANCODE_KP_HASH;
			case Key::KP_SPACE: return SDL_SCANCODE_KP_SPACE;
			case Key::KP_AT: return SDL_SCANCODE_KP_AT;
			case Key::KP_EXCLAM: return SDL_SCANCODE_KP_EXCLAM;
			case Key::KP_MEMSTORE: return SDL_SCANCODE_KP_MEMSTORE;
			case Key::KP_MEMRECALL: return SDL_SCANCODE_KP_MEMRECALL;
			case Key::KP_MEMCLEAR: return SDL_SCANCODE_KP_MEMCLEAR;
			case Key::KP_MEMADD: return SDL_SCANCODE_KP_MEMADD;
			case Key::KP_MEMSUBTRACT: return SDL_SCANCODE_KP_MEMSUBTRACT;
			case Key::KP_MEMMULTIPLY: return SDL_SCANCODE_KP_MEMMULTIPLY;
			case Key::KP_MEMDIVIDE: return SDL_SCANCODE_KP_MEMDIVIDE;
			case Key::KP_PLUSMINUS: return SDL_SCANCODE_KP_PLUSMINUS;
			case Key::KP_CLEAR: return SDL_SCANCODE_KP_CLEAR;
			case Key::KP_CLEARENTRY: return SDL_SCANCODE_KP_CLEARENTRY;
			case Key::KP_BINARY: return SDL_SCANCODE_KP_BINARY;
			case Key::KP_OCTAL: return SDL_SCANCODE_KP_OCTAL;
			case Key::KP_DECIMAL: return SDL_SCANCODE_KP_DECIMAL;
			case Key::KP_HEXADECIMAL: return SDL_SCANCODE_KP_HEXADECIMAL;
			case Key::LCTRL: return SDL_SCANCODE_LCTRL;
			case Key::LSHIFT: return SDL_SCANCODE_LSHIFT;
			case Key::LALT: return SDL_SCANCODE_LALT;
			case Key::LGUI: return SDL_SCANCODE_LGUI;
			case Key::RCTRL: return SDL_SCANCODE_RCTRL;
			case Key::RSHIFT: return SDL_SCANCODE_RSHIFT;
			case Key::RALT: return SDL_SCANCODE_RALT;
			case Key::RGUI: return SDL_SCANCODE_RGUI;
			case Key::MODE: return SDL_SCANCODE_MODE;
			case Key::AUDIONEXT: return SDL_SCANCODE_AUDIONEXT;
			case Key::AUDIOPREV: return SDL_SCANCODE_AUDIOPREV;
			case Key::AUDIOSTOP: return SDL_SCANCODE_AUDIOSTOP;
			case Key::AUDIOPLAY: return SDL_SCANCODE_AUDIOPLAY;
			case Key::AUDIOMUTE: return SDL_SCANCODE_AUDIOMUTE;
			case Key::MEDIASELECT: return SDL_SCANCODE_MEDIASELECT;
			case Key::WWW: return SDL_SCANCODE_WWW;
			case Key::MAIL: return SDL_SCANCODE_MAIL;
			case Key::CALCULATOR: return SDL_SCANCODE_CALCULATOR;
			case Key::COMPUTER: return SDL_SCANCODE_COMPUTER;
			case Key::AC_SEARCH: return SDL_SCANCODE_AC_SEARCH;
			case Key::AC_HOME: return SDL_SCANCODE_AC_HOME;
			case Key::AC_BACK: return SDL_SCANCODE_AC_BACK;
			case Key::AC_FORWARD: return SDL_SCANCODE_AC_FORWARD;
			case Key::AC_STOP: return SDL_SCANCODE_AC_STOP;
			case Key::AC_REFRESH: return SDL_SCANCODE_AC_REFRESH;
			case Key::AC_BOOKMARKS: return SDL_SCANCODE_AC_BOOKMARKS;
			case Key::BRIGHTNESSDOWN: return SDL_SCANCODE_BRIGHTNESSDOWN;
			case Key::BRIGHTNESSUP: return SDL_SCANCODE_BRIGHTNESSUP;
			case Key::DISPLAYSWITCH: return SDL_SCANCODE_DISPLAYSWITCH;
			case Key::KBDILLUMTOGGLE: return SDL_SCANCODE_KBDILLUMTOGGLE;
			case Key::KBDILLUMDOWN: return SDL_SCANCODE_KBDILLUMDOWN;
			case Key::KBDILLUMUP: return SDL_SCANCODE_KBDILLUMUP;
			case Key::EJECT: return SDL_SCANCODE_EJECT;
			case Key::SLEEP: return SDL_SCANCODE_SLEEP;
			}
			return SDL_SCANCODE_UNKNOWN;
		}
	}
}
