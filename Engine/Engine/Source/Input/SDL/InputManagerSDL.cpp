#include "Containers/HashMap.h"
#include "Engine/Logging.h"
#include "Engine/SDL/EngineWindowSDL.h"
#include "Input/SDL/InputManagerSDL.h"
#include "Misc/DoOnce.h"
#include "Templates/IsSame.h"
#include <SDL2/SDL.h>

[[maybe_unused]] static void PrintSdlKeyValues()
{
	struct FSdlKey
	{
		int32 KeyValue;
		SDL_KeyCode KeyCode;
		FStringView Name;
	};

	TArray<FSdlKey> keys;

#define VISIT_SDL_KEY(Key) keys.Add({ static_cast<int32>(RemoveFlag(Key, SDLK_SCANCODE_MASK)), Key, UM_STRINGIFY_AS_VIEW(Key) })

	VISIT_SDL_KEY(SDLK_UNKNOWN);

	VISIT_SDL_KEY(SDLK_RETURN);
	VISIT_SDL_KEY(SDLK_ESCAPE);
	VISIT_SDL_KEY(SDLK_BACKSPACE);
	VISIT_SDL_KEY(SDLK_TAB);
	VISIT_SDL_KEY(SDLK_SPACE);
	VISIT_SDL_KEY(SDLK_EXCLAIM);
	VISIT_SDL_KEY(SDLK_QUOTEDBL);
	VISIT_SDL_KEY(SDLK_HASH);
	VISIT_SDL_KEY(SDLK_PERCENT);
	VISIT_SDL_KEY(SDLK_DOLLAR);
	VISIT_SDL_KEY(SDLK_AMPERSAND);
	VISIT_SDL_KEY(SDLK_QUOTE);
	VISIT_SDL_KEY(SDLK_LEFTPAREN);
	VISIT_SDL_KEY(SDLK_RIGHTPAREN);
	VISIT_SDL_KEY(SDLK_ASTERISK);
	VISIT_SDL_KEY(SDLK_PLUS);
	VISIT_SDL_KEY(SDLK_COMMA);
	VISIT_SDL_KEY(SDLK_MINUS);
	VISIT_SDL_KEY(SDLK_PERIOD);
	VISIT_SDL_KEY(SDLK_SLASH);
	VISIT_SDL_KEY(SDLK_0);
	VISIT_SDL_KEY(SDLK_1);
	VISIT_SDL_KEY(SDLK_2);
	VISIT_SDL_KEY(SDLK_3);
	VISIT_SDL_KEY(SDLK_4);
	VISIT_SDL_KEY(SDLK_5);
	VISIT_SDL_KEY(SDLK_6);
	VISIT_SDL_KEY(SDLK_7);
	VISIT_SDL_KEY(SDLK_8);
	VISIT_SDL_KEY(SDLK_9);
	VISIT_SDL_KEY(SDLK_COLON);
	VISIT_SDL_KEY(SDLK_SEMICOLON);
	VISIT_SDL_KEY(SDLK_LESS);
	VISIT_SDL_KEY(SDLK_EQUALS);
	VISIT_SDL_KEY(SDLK_GREATER);
	VISIT_SDL_KEY(SDLK_QUESTION);
	VISIT_SDL_KEY(SDLK_AT);

	VISIT_SDL_KEY(SDLK_LEFTBRACKET);
	VISIT_SDL_KEY(SDLK_BACKSLASH);
	VISIT_SDL_KEY(SDLK_RIGHTBRACKET);
	VISIT_SDL_KEY(SDLK_CARET);
	VISIT_SDL_KEY(SDLK_UNDERSCORE);
	VISIT_SDL_KEY(SDLK_BACKQUOTE);
	VISIT_SDL_KEY(SDLK_a);
	VISIT_SDL_KEY(SDLK_b);
	VISIT_SDL_KEY(SDLK_c);
	VISIT_SDL_KEY(SDLK_d);
	VISIT_SDL_KEY(SDLK_e);
	VISIT_SDL_KEY(SDLK_f);
	VISIT_SDL_KEY(SDLK_g);
	VISIT_SDL_KEY(SDLK_h);
	VISIT_SDL_KEY(SDLK_i);
	VISIT_SDL_KEY(SDLK_j);
	VISIT_SDL_KEY(SDLK_k);
	VISIT_SDL_KEY(SDLK_l);
	VISIT_SDL_KEY(SDLK_m);
	VISIT_SDL_KEY(SDLK_n);
	VISIT_SDL_KEY(SDLK_o);
	VISIT_SDL_KEY(SDLK_p);
	VISIT_SDL_KEY(SDLK_q);
	VISIT_SDL_KEY(SDLK_r);
	VISIT_SDL_KEY(SDLK_s);
	VISIT_SDL_KEY(SDLK_t);
	VISIT_SDL_KEY(SDLK_u);
	VISIT_SDL_KEY(SDLK_v);
	VISIT_SDL_KEY(SDLK_w);
	VISIT_SDL_KEY(SDLK_x);
	VISIT_SDL_KEY(SDLK_y);
	VISIT_SDL_KEY(SDLK_z);

	VISIT_SDL_KEY(SDLK_CAPSLOCK);

	VISIT_SDL_KEY(SDLK_F1);
	VISIT_SDL_KEY(SDLK_F2);
	VISIT_SDL_KEY(SDLK_F3);
	VISIT_SDL_KEY(SDLK_F4);
	VISIT_SDL_KEY(SDLK_F5);
	VISIT_SDL_KEY(SDLK_F6);
	VISIT_SDL_KEY(SDLK_F7);
	VISIT_SDL_KEY(SDLK_F8);
	VISIT_SDL_KEY(SDLK_F9);
	VISIT_SDL_KEY(SDLK_F10);
	VISIT_SDL_KEY(SDLK_F11);
	VISIT_SDL_KEY(SDLK_F12);

	VISIT_SDL_KEY(SDLK_PRINTSCREEN);
	VISIT_SDL_KEY(SDLK_SCROLLLOCK);
	VISIT_SDL_KEY(SDLK_PAUSE);
	VISIT_SDL_KEY(SDLK_INSERT);
	VISIT_SDL_KEY(SDLK_HOME);
	VISIT_SDL_KEY(SDLK_PAGEUP);
	VISIT_SDL_KEY(SDLK_DELETE);
	VISIT_SDL_KEY(SDLK_END);
	VISIT_SDL_KEY(SDLK_PAGEDOWN);
	VISIT_SDL_KEY(SDLK_RIGHT);
	VISIT_SDL_KEY(SDLK_LEFT);
	VISIT_SDL_KEY(SDLK_DOWN);
	VISIT_SDL_KEY(SDLK_UP);

	VISIT_SDL_KEY(SDLK_NUMLOCKCLEAR);
	VISIT_SDL_KEY(SDLK_KP_DIVIDE);
	VISIT_SDL_KEY(SDLK_KP_MULTIPLY);
	VISIT_SDL_KEY(SDLK_KP_MINUS);
	VISIT_SDL_KEY(SDLK_KP_PLUS);
	VISIT_SDL_KEY(SDLK_KP_ENTER);
	VISIT_SDL_KEY(SDLK_KP_1);
	VISIT_SDL_KEY(SDLK_KP_2);
	VISIT_SDL_KEY(SDLK_KP_3);
	VISIT_SDL_KEY(SDLK_KP_4);
	VISIT_SDL_KEY(SDLK_KP_5);
	VISIT_SDL_KEY(SDLK_KP_6);
	VISIT_SDL_KEY(SDLK_KP_7);
	VISIT_SDL_KEY(SDLK_KP_8);
	VISIT_SDL_KEY(SDLK_KP_9);
	VISIT_SDL_KEY(SDLK_KP_0);
	VISIT_SDL_KEY(SDLK_KP_PERIOD);

	VISIT_SDL_KEY(SDLK_APPLICATION);
	VISIT_SDL_KEY(SDLK_POWER);
	VISIT_SDL_KEY(SDLK_KP_EQUALS);
	VISIT_SDL_KEY(SDLK_F13);
	VISIT_SDL_KEY(SDLK_F14);
	VISIT_SDL_KEY(SDLK_F15);
	VISIT_SDL_KEY(SDLK_F16);
	VISIT_SDL_KEY(SDLK_F17);
	VISIT_SDL_KEY(SDLK_F18);
	VISIT_SDL_KEY(SDLK_F19);
	VISIT_SDL_KEY(SDLK_F20);
	VISIT_SDL_KEY(SDLK_F21);
	VISIT_SDL_KEY(SDLK_F22);
	VISIT_SDL_KEY(SDLK_F23);
	VISIT_SDL_KEY(SDLK_F24);
	VISIT_SDL_KEY(SDLK_EXECUTE);
	VISIT_SDL_KEY(SDLK_HELP);
	VISIT_SDL_KEY(SDLK_MENU);
	VISIT_SDL_KEY(SDLK_SELECT);
	VISIT_SDL_KEY(SDLK_STOP);
	VISIT_SDL_KEY(SDLK_AGAIN);
	VISIT_SDL_KEY(SDLK_UNDO);
	VISIT_SDL_KEY(SDLK_CUT);
	VISIT_SDL_KEY(SDLK_COPY);
	VISIT_SDL_KEY(SDLK_PASTE);
	VISIT_SDL_KEY(SDLK_FIND);
	VISIT_SDL_KEY(SDLK_MUTE);
	VISIT_SDL_KEY(SDLK_VOLUMEUP);
	VISIT_SDL_KEY(SDLK_VOLUMEDOWN);
	VISIT_SDL_KEY(SDLK_KP_COMMA);
	VISIT_SDL_KEY(SDLK_KP_EQUALSAS400);

	VISIT_SDL_KEY(SDLK_ALTERASE);
	VISIT_SDL_KEY(SDLK_SYSREQ);
	VISIT_SDL_KEY(SDLK_CANCEL);
	VISIT_SDL_KEY(SDLK_CLEAR);
	VISIT_SDL_KEY(SDLK_PRIOR);
	VISIT_SDL_KEY(SDLK_RETURN2);
	VISIT_SDL_KEY(SDLK_SEPARATOR);
	VISIT_SDL_KEY(SDLK_OUT);
	VISIT_SDL_KEY(SDLK_OPER);
	VISIT_SDL_KEY(SDLK_CLEARAGAIN);
	VISIT_SDL_KEY(SDLK_CRSEL);
	VISIT_SDL_KEY(SDLK_EXSEL);

	VISIT_SDL_KEY(SDLK_KP_00);
	VISIT_SDL_KEY(SDLK_KP_000);
	VISIT_SDL_KEY(SDLK_THOUSANDSSEPARATOR);
	VISIT_SDL_KEY(SDLK_DECIMALSEPARATOR);
	VISIT_SDL_KEY(SDLK_CURRENCYUNIT);
	VISIT_SDL_KEY(SDLK_CURRENCYSUBUNIT);
	VISIT_SDL_KEY(SDLK_KP_LEFTPAREN);
	VISIT_SDL_KEY(SDLK_KP_RIGHTPAREN);
	VISIT_SDL_KEY(SDLK_KP_LEFTBRACE);
	VISIT_SDL_KEY(SDLK_KP_RIGHTBRACE);
	VISIT_SDL_KEY(SDLK_KP_TAB);
	VISIT_SDL_KEY(SDLK_KP_BACKSPACE);
	VISIT_SDL_KEY(SDLK_KP_A);
	VISIT_SDL_KEY(SDLK_KP_B);
	VISIT_SDL_KEY(SDLK_KP_C);
	VISIT_SDL_KEY(SDLK_KP_D);
	VISIT_SDL_KEY(SDLK_KP_E);
	VISIT_SDL_KEY(SDLK_KP_F);
	VISIT_SDL_KEY(SDLK_KP_XOR);
	VISIT_SDL_KEY(SDLK_KP_POWER);
	VISIT_SDL_KEY(SDLK_KP_PERCENT);
	VISIT_SDL_KEY(SDLK_KP_LESS);
	VISIT_SDL_KEY(SDLK_KP_GREATER);
	VISIT_SDL_KEY(SDLK_KP_AMPERSAND);
	VISIT_SDL_KEY(SDLK_KP_DBLAMPERSAND);
	VISIT_SDL_KEY(SDLK_KP_VERTICALBAR);
	VISIT_SDL_KEY(SDLK_KP_DBLVERTICALBAR);
	VISIT_SDL_KEY(SDLK_KP_COLON);
	VISIT_SDL_KEY(SDLK_KP_HASH);
	VISIT_SDL_KEY(SDLK_KP_SPACE);
	VISIT_SDL_KEY(SDLK_KP_AT);
	VISIT_SDL_KEY(SDLK_KP_EXCLAM);
	VISIT_SDL_KEY(SDLK_KP_MEMSTORE);
	VISIT_SDL_KEY(SDLK_KP_MEMRECALL);
	VISIT_SDL_KEY(SDLK_KP_MEMCLEAR);
	VISIT_SDL_KEY(SDLK_KP_MEMADD);
	VISIT_SDL_KEY(SDLK_KP_MEMSUBTRACT);
	VISIT_SDL_KEY(SDLK_KP_MEMMULTIPLY);
	VISIT_SDL_KEY(SDLK_KP_MEMDIVIDE);
	VISIT_SDL_KEY(SDLK_KP_PLUSMINUS);
	VISIT_SDL_KEY(SDLK_KP_CLEAR);
	VISIT_SDL_KEY(SDLK_KP_CLEARENTRY);
	VISIT_SDL_KEY(SDLK_KP_BINARY);
	VISIT_SDL_KEY(SDLK_KP_OCTAL);
	VISIT_SDL_KEY(SDLK_KP_DECIMAL);
	VISIT_SDL_KEY(SDLK_KP_HEXADECIMAL);

	VISIT_SDL_KEY(SDLK_LCTRL);
	VISIT_SDL_KEY(SDLK_LSHIFT);
	VISIT_SDL_KEY(SDLK_LALT);
	VISIT_SDL_KEY(SDLK_LGUI);
	VISIT_SDL_KEY(SDLK_RCTRL);
	VISIT_SDL_KEY(SDLK_RSHIFT);
	VISIT_SDL_KEY(SDLK_RALT);
	VISIT_SDL_KEY(SDLK_RGUI);

	VISIT_SDL_KEY(SDLK_MODE);

	VISIT_SDL_KEY(SDLK_AUDIONEXT);
	VISIT_SDL_KEY(SDLK_AUDIOPREV);
	VISIT_SDL_KEY(SDLK_AUDIOSTOP);
	VISIT_SDL_KEY(SDLK_AUDIOPLAY);
	VISIT_SDL_KEY(SDLK_AUDIOMUTE);
	VISIT_SDL_KEY(SDLK_MEDIASELECT);
	VISIT_SDL_KEY(SDLK_WWW);
	VISIT_SDL_KEY(SDLK_MAIL);
	VISIT_SDL_KEY(SDLK_CALCULATOR);
	VISIT_SDL_KEY(SDLK_COMPUTER);
	VISIT_SDL_KEY(SDLK_AC_SEARCH);
	VISIT_SDL_KEY(SDLK_AC_HOME);
	VISIT_SDL_KEY(SDLK_AC_BACK);
	VISIT_SDL_KEY(SDLK_AC_FORWARD);
	VISIT_SDL_KEY(SDLK_AC_STOP);
	VISIT_SDL_KEY(SDLK_AC_REFRESH);
	VISIT_SDL_KEY(SDLK_AC_BOOKMARKS);

	VISIT_SDL_KEY(SDLK_BRIGHTNESSDOWN);
	VISIT_SDL_KEY(SDLK_BRIGHTNESSUP);
	VISIT_SDL_KEY(SDLK_DISPLAYSWITCH);
	VISIT_SDL_KEY(SDLK_KBDILLUMTOGGLE);
	VISIT_SDL_KEY(SDLK_KBDILLUMDOWN);
	VISIT_SDL_KEY(SDLK_KBDILLUMUP);
	VISIT_SDL_KEY(SDLK_EJECT);
	VISIT_SDL_KEY(SDLK_SLEEP);
	VISIT_SDL_KEY(SDLK_APP1);
	VISIT_SDL_KEY(SDLK_APP2);

	VISIT_SDL_KEY(SDLK_AUDIOREWIND);
	VISIT_SDL_KEY(SDLK_AUDIOFASTFORWARD);

	VISIT_SDL_KEY(SDLK_SOFTLEFT);
	VISIT_SDL_KEY(SDLK_SOFTRIGHT);
	VISIT_SDL_KEY(SDLK_CALL);
	VISIT_SDL_KEY(SDLK_ENDCALL);

#undef VISIT_SDL_KEY

	keys.Sort([](const FSdlKey& left, const FSdlKey& right)
	{
		return TComparisonTraits<int32>::Compare(left.KeyValue, right.KeyValue);
	});

	for (const FSdlKey& key : keys)
	{
		UM_LOG(Info, "// {} = {}", key.KeyValue, key.Name);
	}
}

[[nodiscard]] static EMouseButton GetUmbralButtonFromSdlButton(const int32 button)
{
	switch (button)
	{
	case SDL_BUTTON_LEFT: return EMouseButton::Left;
	case SDL_BUTTON_MIDDLE: return EMouseButton::Middle;
	case SDL_BUTTON_RIGHT: return EMouseButton::Right;
	case SDL_BUTTON_X1: return EMouseButton::Extra1;
	case SDL_BUTTON_X2: return EMouseButton::Extra2;
	}

	UM_ASSERT_NOT_REACHED();
}

[[nodiscard]] static EKey GetUmbralKeyFromSdlKey(const int32 key)
{
	// TODO Don't use a hash map here, just use a static array of EKey where `key` is the index
	static const THashMap<int32, EKey> keyCodeToKeyMap
	{{
		{ SDLK_a,               EKey::A },
		{ SDLK_b,               EKey::B },
		{ SDLK_c,               EKey::C },
		{ SDLK_d,               EKey::D },
		{ SDLK_e,               EKey::E },
		{ SDLK_f,               EKey::F },
		{ SDLK_g,               EKey::G },
		{ SDLK_h,               EKey::H },
		{ SDLK_i,               EKey::I },
		{ SDLK_j,               EKey::J },
		{ SDLK_k,               EKey::K },
		{ SDLK_l,               EKey::L },
		{ SDLK_m,               EKey::M },
		{ SDLK_n,               EKey::N },
		{ SDLK_o,               EKey::O },
		{ SDLK_p,               EKey::P },
		{ SDLK_q,               EKey::Q },
		{ SDLK_r,               EKey::R },
		{ SDLK_s,               EKey::S },
		{ SDLK_t,               EKey::T },
		{ SDLK_u,               EKey::U },
		{ SDLK_v,               EKey::V },
		{ SDLK_w,               EKey::W },
		{ SDLK_x,               EKey::X },
		{ SDLK_y,               EKey::Y },
		{ SDLK_z,               EKey::Z },
		{ SDLK_0,               EKey::D0 },
		{ SDLK_1,               EKey::D1 },
		{ SDLK_2,               EKey::D2 },
		{ SDLK_3,               EKey::D3 },
		{ SDLK_4,               EKey::D4 },
		{ SDLK_5,               EKey::D5 },
		{ SDLK_6,               EKey::D6 },
		{ SDLK_7,               EKey::D7 },
		{ SDLK_8,               EKey::D8 },
		{ SDLK_9,               EKey::D9 },
		{ SDLK_KP_0,            EKey::NumPad0 },
		{ SDLK_KP_1,            EKey::NumPad1 },
		{ SDLK_KP_2,            EKey::NumPad2 },
		{ SDLK_KP_3,            EKey::NumPad3 },
		{ SDLK_KP_4,            EKey::NumPad4 },
		{ SDLK_KP_5,            EKey::NumPad5 },
		{ SDLK_KP_6,            EKey::NumPad6 },
		{ SDLK_KP_7,            EKey::NumPad7 },
		{ SDLK_KP_8,            EKey::NumPad8 },
		{ SDLK_KP_9,            EKey::NumPad9 },
		{ SDLK_KP_CLEAR,        EKey::OemClear },
		{ SDLK_KP_DECIMAL,      EKey::Decimal },
		{ SDLK_KP_DIVIDE,       EKey::Divide },
		{ SDLK_KP_ENTER,        EKey::Enter },
		{ SDLK_KP_MINUS,        EKey::Subtract },
		{ SDLK_KP_MULTIPLY,     EKey::Multiply },
		{ SDLK_KP_PERIOD,       EKey::OemPeriod },
		{ SDLK_KP_PLUS,         EKey::Add },
		{ SDLK_F1,              EKey::F1 },
		{ SDLK_F2,              EKey::F2 },
		{ SDLK_F3,              EKey::F3 },
		{ SDLK_F4,              EKey::F4 },
		{ SDLK_F5,              EKey::F5 },
		{ SDLK_F6,              EKey::F6 },
		{ SDLK_F7,              EKey::F7 },
		{ SDLK_F8,              EKey::F8 },
		{ SDLK_F9,              EKey::F9 },
		{ SDLK_F10,             EKey::F10 },
		{ SDLK_F11,             EKey::F11 },
		{ SDLK_F12,             EKey::F12 },
		{ SDLK_F13,             EKey::F13 },
		{ SDLK_F14,             EKey::F14 },
		{ SDLK_F15,             EKey::F15 },
		{ SDLK_F16,             EKey::F16 },
		{ SDLK_F17,             EKey::F17 },
		{ SDLK_F18,             EKey::F18 },
		{ SDLK_F19,             EKey::F19 },
		{ SDLK_F20,             EKey::F20 },
		{ SDLK_F21,             EKey::F21 },
		{ SDLK_F22,             EKey::F22 },
		{ SDLK_F23,             EKey::F23 },
		{ SDLK_F24,             EKey::F24 },
		{ SDLK_SPACE,           EKey::Space },
		{ SDLK_UP,              EKey::Up },
		{ SDLK_DOWN,            EKey::Down },
		{ SDLK_LEFT,            EKey::Left },
		{ SDLK_RIGHT,           EKey::Right },
		{ SDLK_LALT,            EKey::LeftAlt },
		{ SDLK_RALT,            EKey::RightAlt },
		{ SDLK_LCTRL,           EKey::LeftControl },
		{ SDLK_RCTRL,           EKey::RightControl },
		{ SDLK_LGUI,            EKey::LeftWindows },
		{ SDLK_RGUI,            EKey::RightWindows },
		{ SDLK_LSHIFT,          EKey::LeftShift },
		{ SDLK_RSHIFT,          EKey::RightShift },
		{ SDLK_APPLICATION,     EKey::Apps },
		{ SDLK_MENU,            EKey::Apps },
		{ SDLK_SLASH,           EKey::OemQuestion },
		{ SDLK_BACKSLASH,       EKey::OemPipe },
		{ SDLK_LEFTBRACKET,     EKey::OemOpenBrackets },
		{ SDLK_RIGHTBRACKET,    EKey::OemCloseBrackets },
		{ SDLK_CAPSLOCK,        EKey::CapsLock },
		{ SDLK_COMMA,           EKey::OemComma },
		{ SDLK_DELETE,          EKey::Delete },
		{ SDLK_END,             EKey::End },
		{ SDLK_BACKSPACE,       EKey::Backspace },
		{ SDLK_RETURN,          EKey::Enter },
		{ SDLK_ESCAPE,          EKey::Escape },
		{ SDLK_HOME,            EKey::Home },
		{ SDLK_INSERT,          EKey::Insert },
		{ SDLK_MINUS,           EKey::OemMinus },
		{ SDLK_NUMLOCKCLEAR,    EKey::NumLock },
		{ SDLK_PAGEUP,          EKey::PageUp },
		{ SDLK_PAGEDOWN,        EKey::PageDown },
		{ SDLK_PAUSE,           EKey::Pause },
		{ SDLK_PERIOD,          EKey::OemPeriod },
		{ SDLK_EQUALS,          EKey::OemPlus },
		{ SDLK_PRINTSCREEN,     EKey::PrintScreen },
		{ SDLK_QUOTE,           EKey::OemQuotes },
		{ SDLK_SCROLLLOCK,      EKey::Scroll },
		{ SDLK_SEMICOLON,       EKey::OemSemicolon },
		{ SDLK_SLEEP,           EKey::Sleep },
		{ SDLK_TAB,             EKey::Tab },
		{ SDLK_BACKQUOTE,       EKey::OemTilde },
		{ SDLK_VOLUMEUP,        EKey::VolumeUp },
		{ SDLK_VOLUMEDOWN,      EKey::VolumeDown },
		{ SDLK_UNKNOWN,         EKey::None }
	}};

	if (const EKey* result = keyCodeToKeyMap.Find(key))
	{
		return *result;
	}

	UM_LOG(Error, "Failed to find equivalent key to SDL keycode \"{}\"", key);

	return EKey::None;
}

[[maybe_unused]] static EKey GetUmbralKeyFromSdlScancode(const int32 scancode)
{
	// TODO Don't use a hash map here, just use a static array of EKey where `scancode` is the index
	static const THashMap<int32, EKey> scancodeToKeyMap
	{{
		{ SDL_SCANCODE_A,               EKey::A },
		{ SDL_SCANCODE_B,               EKey::B },
		{ SDL_SCANCODE_C,               EKey::C },
		{ SDL_SCANCODE_D,               EKey::D },
		{ SDL_SCANCODE_E,               EKey::E },
		{ SDL_SCANCODE_F,               EKey::F },
		{ SDL_SCANCODE_G,               EKey::G },
		{ SDL_SCANCODE_H,               EKey::H },
		{ SDL_SCANCODE_I,               EKey::I },
		{ SDL_SCANCODE_J,               EKey::J },
		{ SDL_SCANCODE_K,               EKey::K },
		{ SDL_SCANCODE_L,               EKey::L },
		{ SDL_SCANCODE_M,               EKey::M },
		{ SDL_SCANCODE_N,               EKey::N },
		{ SDL_SCANCODE_O,               EKey::O },
		{ SDL_SCANCODE_P,               EKey::P },
		{ SDL_SCANCODE_Q,               EKey::Q },
		{ SDL_SCANCODE_R,               EKey::R },
		{ SDL_SCANCODE_S,               EKey::S },
		{ SDL_SCANCODE_T,               EKey::T },
		{ SDL_SCANCODE_U,               EKey::U },
		{ SDL_SCANCODE_V,               EKey::V },
		{ SDL_SCANCODE_W,               EKey::W },
		{ SDL_SCANCODE_X,               EKey::X },
		{ SDL_SCANCODE_Y,               EKey::Y },
		{ SDL_SCANCODE_Z,               EKey::Z },
		{ SDL_SCANCODE_0,               EKey::D0 },
		{ SDL_SCANCODE_1,               EKey::D1 },
		{ SDL_SCANCODE_2,               EKey::D2 },
		{ SDL_SCANCODE_3,               EKey::D3 },
		{ SDL_SCANCODE_4,               EKey::D4 },
		{ SDL_SCANCODE_5,               EKey::D5 },
		{ SDL_SCANCODE_6,               EKey::D6 },
		{ SDL_SCANCODE_7,               EKey::D7 },
		{ SDL_SCANCODE_8,               EKey::D8 },
		{ SDL_SCANCODE_9,               EKey::D9 },
		{ SDL_SCANCODE_KP_0,            EKey::NumPad0 },
		{ SDL_SCANCODE_KP_1,            EKey::NumPad1 },
		{ SDL_SCANCODE_KP_2,            EKey::NumPad2 },
		{ SDL_SCANCODE_KP_3,            EKey::NumPad3 },
		{ SDL_SCANCODE_KP_4,            EKey::NumPad4 },
		{ SDL_SCANCODE_KP_5,            EKey::NumPad5 },
		{ SDL_SCANCODE_KP_6,            EKey::NumPad6 },
		{ SDL_SCANCODE_KP_7,            EKey::NumPad7 },
		{ SDL_SCANCODE_KP_8,            EKey::NumPad8 },
		{ SDL_SCANCODE_KP_9,            EKey::NumPad9 },
		{ SDL_SCANCODE_KP_CLEAR,        EKey::OemClear },
		{ SDL_SCANCODE_KP_DECIMAL,      EKey::Decimal },
		{ SDL_SCANCODE_KP_DIVIDE,       EKey::Divide },
		{ SDL_SCANCODE_KP_ENTER,        EKey::Enter },
		{ SDL_SCANCODE_KP_MINUS,        EKey::Subtract },
		{ SDL_SCANCODE_KP_MULTIPLY,     EKey::Multiply },
		{ SDL_SCANCODE_KP_PERIOD,       EKey::OemPeriod },
		{ SDL_SCANCODE_KP_PLUS,         EKey::Add },
		{ SDL_SCANCODE_F1,              EKey::F1 },
		{ SDL_SCANCODE_F2,              EKey::F2 },
		{ SDL_SCANCODE_F3,              EKey::F3 },
		{ SDL_SCANCODE_F4,              EKey::F4 },
		{ SDL_SCANCODE_F5,              EKey::F5 },
		{ SDL_SCANCODE_F6,              EKey::F6 },
		{ SDL_SCANCODE_F7,              EKey::F7 },
		{ SDL_SCANCODE_F8,              EKey::F8 },
		{ SDL_SCANCODE_F9,              EKey::F9 },
		{ SDL_SCANCODE_F10,             EKey::F10 },
		{ SDL_SCANCODE_F11,             EKey::F11 },
		{ SDL_SCANCODE_F12,             EKey::F12 },
		{ SDL_SCANCODE_F13,             EKey::F13 },
		{ SDL_SCANCODE_F14,             EKey::F14 },
		{ SDL_SCANCODE_F15,             EKey::F15 },
		{ SDL_SCANCODE_F16,             EKey::F16 },
		{ SDL_SCANCODE_F17,             EKey::F17 },
		{ SDL_SCANCODE_F18,             EKey::F18 },
		{ SDL_SCANCODE_F19,             EKey::F19 },
		{ SDL_SCANCODE_F20,             EKey::F20 },
		{ SDL_SCANCODE_F21,             EKey::F21 },
		{ SDL_SCANCODE_F22,             EKey::F22 },
		{ SDL_SCANCODE_F23,             EKey::F23 },
		{ SDL_SCANCODE_F24,             EKey::F24 },
		{ SDL_SCANCODE_SPACE,           EKey::Space },
		{ SDL_SCANCODE_UP,              EKey::Up },
		{ SDL_SCANCODE_DOWN,            EKey::Down },
		{ SDL_SCANCODE_LEFT,            EKey::Left },
		{ SDL_SCANCODE_RIGHT,           EKey::Right },
		{ SDL_SCANCODE_LALT,            EKey::LeftAlt },
		{ SDL_SCANCODE_RALT,            EKey::RightAlt },
		{ SDL_SCANCODE_LCTRL,           EKey::LeftControl },
		{ SDL_SCANCODE_RCTRL,           EKey::RightControl },
		{ SDL_SCANCODE_LGUI,            EKey::LeftWindows },
		{ SDL_SCANCODE_RGUI,            EKey::RightWindows },
		{ SDL_SCANCODE_LSHIFT,          EKey::LeftShift },
		{ SDL_SCANCODE_RSHIFT,          EKey::RightShift },
		{ SDL_SCANCODE_APPLICATION,     EKey::Apps },
		{ SDL_SCANCODE_MENU,            EKey::Apps },
		{ SDL_SCANCODE_SLASH,           EKey::OemQuestion },
		{ SDL_SCANCODE_BACKSLASH,       EKey::OemPipe },
		{ SDL_SCANCODE_LEFTBRACKET,     EKey::OemOpenBrackets },
		{ SDL_SCANCODE_RIGHTBRACKET,    EKey::OemCloseBrackets },
		{ SDL_SCANCODE_CAPSLOCK,        EKey::CapsLock },
		{ SDL_SCANCODE_COMMA,           EKey::OemComma },
		{ SDL_SCANCODE_DELETE,          EKey::Delete },
		{ SDL_SCANCODE_END,             EKey::End },
		{ SDL_SCANCODE_BACKSPACE,       EKey::Backspace },
		{ SDL_SCANCODE_RETURN,          EKey::Enter },
		{ SDL_SCANCODE_ESCAPE,          EKey::Escape },
		{ SDL_SCANCODE_HOME,            EKey::Home },
		{ SDL_SCANCODE_INSERT,          EKey::Insert },
		{ SDL_SCANCODE_MINUS,           EKey::OemMinus },
		{ SDL_SCANCODE_NUMLOCKCLEAR,    EKey::NumLock },
		{ SDL_SCANCODE_PAGEUP,          EKey::PageUp },
		{ SDL_SCANCODE_PAGEDOWN,        EKey::PageDown },
		{ SDL_SCANCODE_PAUSE,           EKey::Pause },
		{ SDL_SCANCODE_PERIOD,          EKey::OemPeriod },
		{ SDL_SCANCODE_EQUALS,          EKey::OemPlus },
		{ SDL_SCANCODE_PRINTSCREEN,     EKey::PrintScreen },
		{ SDL_SCANCODE_APOSTROPHE,      EKey::OemQuotes },
		{ SDL_SCANCODE_SCROLLLOCK,      EKey::Scroll },
		{ SDL_SCANCODE_SEMICOLON,       EKey::OemSemicolon },
		{ SDL_SCANCODE_SLEEP,           EKey::Sleep },
		{ SDL_SCANCODE_TAB,             EKey::Tab },
		{ SDL_SCANCODE_GRAVE,           EKey::OemTilde },
		{ SDL_SCANCODE_VOLUMEUP,        EKey::VolumeUp },
		{ SDL_SCANCODE_VOLUMEDOWN,      EKey::VolumeDown },
		{ SDL_SCANCODE_UNKNOWN,         EKey::None },
	}};

	if (const EKey* result = scancodeToKeyMap.Find(scancode))
	{
		return *result;
	}

	UM_LOG(Error, "Failed to find equivalent key to SDL scancode \"{}\"", scancode);

	return EKey::None;
}

EButtonState UInputManagerSDL::GetButtonState(const EMouseButton button) const
{
	return m_CurrentButtonStates[static_cast<int32>(button)];
}

EKeyState UInputManagerSDL::GetKeyState(const EKey key) const
{
	return m_CurrentKeyStates[static_cast<int32>(key)];
}

void UInputManagerSDL::ProcessGamePadAxisEvent(const SDL_ControllerAxisEvent& event)
{
	(void)event;
}

void UInputManagerSDL::ProcessGamePadButtonDownEvent(const SDL_ControllerButtonEvent& event)
{
	(void)event;
}

void UInputManagerSDL::ProcessGamePadButtonUpEvent(const SDL_ControllerButtonEvent& event)
{
	(void)event;
}

void UInputManagerSDL::ProcessGamePadAddedEvent(const SDL_ControllerDeviceEvent& event)
{
	(void)event;
}

void UInputManagerSDL::ProcessGamePadRemovedEvent(const SDL_ControllerDeviceEvent& event)
{
	(void)event;
}

void UInputManagerSDL::ProcessGamePadRemappedEvent(const SDL_ControllerDeviceEvent& event)
{
	(void)event;
}

void UInputManagerSDL::ProcessKeyDownEvent(const SDL_KeyboardEvent& event)
{
	const int32 keyCode = event.keysym.sym;
	const EKey key = GetUmbralKeyFromSdlKey(keyCode);
	m_CurrentKeyStates[static_cast<int32>(key)] = EKeyState::Pressed;
}

void UInputManagerSDL::ProcessKeyUpEvent(const SDL_KeyboardEvent& event)
{
	const int32 keyCode = event.keysym.sym;
	const EKey key = GetUmbralKeyFromSdlKey(keyCode);
	m_CurrentKeyStates[static_cast<int32>(key)] = EKeyState::Released;
}

void UInputManagerSDL::ProcessMouseMoveEvent(const SDL_MouseMotionEvent& event)
{
	(void)event;
}

void UInputManagerSDL::ProcessMouseButtonDownEvent(const SDL_MouseButtonEvent& event)
{
	const EMouseButton button = GetUmbralButtonFromSdlButton(event.button);
	m_CurrentButtonStates[static_cast<int32>(button)] = EButtonState::Pressed;
}

void UInputManagerSDL::ProcessMouseButtonUpEvent(const SDL_MouseButtonEvent& event)
{
	const EMouseButton button = GetUmbralButtonFromSdlButton(event.button);
	m_CurrentButtonStates[static_cast<int32>(button)] = EButtonState::Released;
}

void UInputManagerSDL::ProcessMouseWheelEvent(const SDL_MouseWheelEvent& event)
{
	(void)event;
}

void UInputManagerSDL::UpdateBeforePollingEvents()
{
	//DO_ONCE(PrintSdlKeyValues);

	static_assert(IsSame<decltype(m_CurrentKeyStates), decltype(m_PreviousKeyStates)>);
	FMemory::Copy(m_PreviousKeyStates.GetData(), m_CurrentKeyStates.GetData(), sizeof(m_CurrentKeyStates));

	static_assert(IsSame<decltype(m_CurrentButtonStates), decltype(m_PreviousButtonStates)>);
	FMemory::Copy(m_PreviousButtonStates.GetData(), m_CurrentButtonStates.GetData(), sizeof(m_CurrentButtonStates));
}

bool UInputManagerSDL::WasButtonPressed(const EMouseButton button) const
{
	const EButtonState previousState = m_PreviousButtonStates[static_cast<int32>(button)];
	const EButtonState currentState = m_CurrentButtonStates[static_cast<int32>(button)];
	return previousState == EButtonState::Released && currentState == EButtonState::Pressed;
}

bool UInputManagerSDL::WasButtonReleased(EMouseButton button) const
{
	const EButtonState previousState = m_PreviousButtonStates[static_cast<int32>(button)];
	const EButtonState currentState = m_CurrentButtonStates[static_cast<int32>(button)];
	return previousState == EButtonState::Pressed && currentState == EButtonState::Released;
}

bool UInputManagerSDL::WasKeyPressed(const EKey key) const
{
	const EKeyState previousState = m_PreviousKeyStates[static_cast<int32>(key)];
	const EKeyState currentState = m_CurrentKeyStates[static_cast<int32>(key)];
	return previousState == EKeyState::Released && currentState == EKeyState::Pressed;
}

bool UInputManagerSDL::WasKeyReleased(const EKey key) const
{
	const EKeyState previousState = m_PreviousKeyStates[static_cast<int32>(key)];
	const EKeyState currentState = m_CurrentKeyStates[static_cast<int32>(key)];
	return previousState == EKeyState::Pressed && currentState == EKeyState::Released;
}

void UInputManagerSDL::Created(const FObjectCreationContext& context)
{
	Super::Created(context);

	m_Window = FindAncestorOfType<UEngineWindowSDL>();

	m_PreviousKeyStates.Fill(EKeyState::Released);
	m_CurrentKeyStates.Fill(EKeyState::Released);
	m_PreviousButtonStates.Fill(EButtonState::Released);
	m_CurrentButtonStates.Fill(EButtonState::Released);
}