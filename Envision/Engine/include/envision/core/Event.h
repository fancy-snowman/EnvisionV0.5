#pragma once

#include "KeyCodes.h"

// Inspiration:
// https://www.w3.org/TR/DOM-Level-3-Events/#events-keyboardevents

namespace env
{
	//enum class PKeyCode
	//{
	//	Unknown = 0,

	//	// Printable keys
	//	A, B, C, D,
	//	E, F, G, H,
	//	I, J, K, L,
	//	M, N, O, P,
	//	Q, R, S, T,
	//	U, V, W, X,
	//	Y, Z,

	//	Num0, Num1, Num2, Num3,
	//	Num4, Num5,	Num6, Num7,
	//	Num8, Num9,

	//	Tab, Space,

	//	// Function keys
	//	F1, F2, F3, F4,
	//	F5, F6, F7, F8,
	//	F9, F10, F11, F12,

	//	LControl, RControl,
	//	LShift, RShift,
	//	Alt, AltGr,

	//	Escape,
	//	PrintScreen,
	//	ScreenLock,
	//	Pause,
	//	Insert,
	//	Home, End,
	//	PageUp, PageDown,
	//	Backspace, Delete,
	//	
	//	// Layout specific
	//	Å, Ä, Ö,
	//	Quote,
	//	Comma, Period, Minus,
	//	LessThan,

	//	// Mouse
	//	MouseLeft,
	//	MouseMiddle,
	//	MouseRight,
	//};

	struct KeyInfo
	{
		// Modifiers
		bool Ctrl : 1;
		bool Shift : 1;
		bool Alt : 1;
		
		// Other
		bool Repeat : 1;
		bool Left : 1;
		bool Right : 1;

		bool ModifierCtrlOnly() { return Ctrl && !Shift && !Alt; }
		bool ModifierShiftOnly() { return !Ctrl && Shift && !Alt; }
		bool ModifierAltOnly() { return Ctrl && !Shift && !Alt; }
		bool ModifierCtrlShiftOnly() { return Ctrl && Shift && !Alt; }
		bool ModifierCtrlAltOnly() { return Ctrl && !Shift && Alt; }
		bool ModifierShiftAltOnly() { return !Ctrl && Shift && Alt; }
		bool ModifierAltGrOnly() { return Ctrl && !Shift && Alt; } // AltGr simulates Ctrl+Alt
	};

	//enum class VKeyCode
	//{
	//	Unknown = 0,

	//	// Alphabetical
	//	a, b, c, d,
	//	e, f, g, h,
	//	i, j, k, l,
	//	m, n, o, p,
	//	q, r, s, t,
	//	u, v, w, x,
	//	y, z,
	//	å, ä, ö,

	//	A, B, C, D,
	//	E, F, G, H,
	//	I, J, K, L,
	//	M, N, O, P,
	//	Q, R, S, T,
	//	U, V, W, X,
	//	Y, Z,
	//	Å, Ä, Ö,

	//	// Symbols
	//	Exclamation,					// !
	//	At, DoubleQuote,				// @, "
	//	Pound, Number,					// £, #
	//	Dollar, Currency,				// $, ¤
	//	Euro, Persent,					// €, %
	//	And,							// &
	//	LCurlyBracket, Slash,			// {, /
	//	LSquareBracket, LParanthesis,	// [, (
	//	RSquareBracket, RParanthesis,	// ], )
	//	RCurlyBracket, Equal,			// }, =
	//	Plus, BackSlash, Question,		// +, \, ?

	//	Quote, Asterisk,				// ', *

	//	Comma, Semicolon,				// ,, ;
	//	Period, Colon,					// ., :
	//	Minus, Underscore,				// -, _

	//	LessThan, GreaterThan, Pipe,	// <, >, |

	//	Tab,

	//	// Number row
	//	Num0, Num1, Num2, Num3,
	//	Num4, Num5, Num6, Num7,
	//	Num8, Num9,

	//	// Function numbers
	//	F1, F2, F3, F4,
	//	F5, F6, F7, F8,
	//	F9, F10, F11, F12,

	//	// Modifiers
	//	LControl, RControl,
	//	LShift, RShift,
	//	Alt, AltGr,

	//	// Other function keys
	//	Escape,
	//	PrintScreen,
	//	ScreenLock,
	//	Pause,
	//	Insert,
	//	Home, End,
	//	PageUp, PageDown,
	//	Backspace, Delete,

	//	// Mouse
	//	MouseLeft,
	//	MouseMiddle,
	//	MouseRight,
	//};

	//VKeyCode GetVirtualKey(PKeyCode key, KeyInfo info = KeyInfo())
	//{
	//	switch (key)
	//	{
	//		// Alphabetical
	//		case PKeyCode::A: case PKeyCode::B: case PKeyCode::C: case PKeyCode::D:
	//		case PKeyCode::E: case PKeyCode::F: case PKeyCode::G: case PKeyCode::H:
	//		case PKeyCode::I: case PKeyCode::J: case PKeyCode::K: case PKeyCode::L:
	//		case PKeyCode::M: case PKeyCode::N: case PKeyCode::O: case PKeyCode::P:
	//		case PKeyCode::Q: case PKeyCode::R: case PKeyCode::S: case PKeyCode::T:
	//		case PKeyCode::U: case PKeyCode::V: case PKeyCode::W: case PKeyCode::X:
	//		case PKeyCode::Y: case PKeyCode::Z:
	//		{
	//			int PStart = (int)PKeyCode::A;
	//			int VLowerStart = (int)VKeyCode::a;
	//			int VUpperStart = (int)VKeyCode::A;
	//			int indexInLetters = (int)key - PStart;

	//			VKeyCode vKey = (info.Shift) ? 
	//				(VKeyCode)(VUpperStart + indexInLetters) :
	//				(VKeyCode)(VLowerStart + indexInLetters);

	//			return vKey;
	//		}

	//		// Number row
	//		case PKeyCode::Num0:
	//		{
	//			VKeyCode vKey = VKeyCode::Num0;

	//			if (info.ModifierAltGrOnly())
	//				vKey = VKeyCode::RCurlyBracket;
	//			else if (info.ModifierShiftOnly())
	//				vKey = VKeyCode::Equal;

	//			return vKey;
	//		}

	//		case PKeyCode::Num1:
	//		{
	//			VKeyCode vKey = VKeyCode::Num1;

	//			if (info.ModifierAltGrOnly())
	//				vKey = VKeyCode::At;

	//			return vKey;
	//		}
	//		
	//		case PKeyCode::Num2:
	//		{
	//			VKeyCode vKey = VKeyCode::Num2;

	//			if (info.ModifierAltGrOnly())
	//				vKey = VKeyCode::At;
	//			else if (info.ModifierShiftOnly())
	//				vKey = VKeyCode::DoubleQuote;

	//			return vKey;
	//		}

	//		case PKeyCode::Num3:
	//		{
	//			VKeyCode vKey = VKeyCode::Num3;

	//			if (info.ModifierAltGrOnly())
	//				vKey = VKeyCode::Pound;
	//			else if (info.ModifierShiftOnly())
	//				vKey = VKeyCode::Number;

	//			return vKey;
	//		}

	//		case PKeyCode::Num4:
	//		{
	//			VKeyCode vKey = VKeyCode::Num4;

	//			if (info.ModifierAltGrOnly())
	//				vKey = VKeyCode::Dollar;
	//			else if (info.ModifierShiftOnly())
	//				vKey = VKeyCode::Currency;

	//			return vKey;
	//		}
	//		
	//		case PKeyCode::Num5:
	//		{
	//			VKeyCode vKey = VKeyCode::Num5;

	//			if (info.ModifierAltGrOnly())
	//				vKey = VKeyCode::Euro;
	//			else if (info.ModifierShiftOnly())
	//				vKey = VKeyCode::Persent;

	//			return vKey;
	//		}
	//		
	//		case PKeyCode::Num6:
	//		{
	//			VKeyCode vKey = VKeyCode::Num6;
	//			
	//			if (info.ModifierShiftOnly())
	//				vKey = VKeyCode::And;

	//			return vKey;
	//		}
	//		
	//		case PKeyCode::Num7:
	//		{
	//			VKeyCode vKey = VKeyCode::Num7;

	//			if (info.ModifierAltGrOnly())
	//				vKey = VKeyCode::LCurlyBracket;
	//			else if (info.ModifierShiftOnly())
	//				vKey = VKeyCode::Slash;

	//			return vKey;
	//		}

	//		case PKeyCode::Num8:
	//		{
	//			VKeyCode vKey = VKeyCode::Num8;

	//			if (info.ModifierAltGrOnly())
	//				vKey = VKeyCode::LSquareBracket;
	//			else if (info.ModifierShiftOnly())
	//				vKey = VKeyCode::LParanthesis;

	//			return vKey;
	//		}
	//		
	//		case PKeyCode::Num9:
	//		{
	//			VKeyCode vKey = VKeyCode::Num9;

	//			if (info.ModifierAltGrOnly())
	//				vKey = VKeyCode::RSquareBracket;
	//			else if (info.ModifierShiftOnly())
	//				vKey = VKeyCode::RParanthesis;

	//			return vKey;
	//		}

	//		// Function numbers
	//		case PKeyCode::F1: case PKeyCode::F2: case PKeyCode::F3: case PKeyCode::F4:
	//		case PKeyCode::F5: case PKeyCode::F6: case PKeyCode::F7: case PKeyCode::F8:
	//		case PKeyCode::F9: case PKeyCode::F10: case PKeyCode::F11: case PKeyCode::F12:
	//		{
	//			int PStartIndex = (int)PKeyCode::F1;
	//			int VStartIndex = (int)VKeyCode::F1;

	//			VKeyCode vKey = (VKeyCode)(VStartIndex + (int)key - PStartIndex);

	//			return vKey;
	//		}

	//		// Modifiers
	//		case PKeyCode::LControl: return VKeyCode::LControl;
	//		case PKeyCode::RControl: return VKeyCode::RControl;
	//		case PKeyCode::LShift: return VKeyCode::LShift;
	//		case PKeyCode::RShift: return VKeyCode::RShift;
	//		case PKeyCode::Alt: return VKeyCode::Alt;
	//		case PKeyCode::AltGr: return VKeyCode::AltGr;

	//		// Other character keys
	//		case PKeyCode::Tab: return VKeyCode::Tab;

	//		// Other function keys
	//		case PKeyCode::Escape: return VKeyCode::Escape;
	//		case PKeyCode::PrintScreen: return VKeyCode::PrintScreen;
	//		case PKeyCode::ScreenLock: return VKeyCode::ScreenLock;
	//		case PKeyCode::Pause: return VKeyCode::Pause;
	//		case PKeyCode::Insert: return VKeyCode::Insert;
	//		case PKeyCode::Home: return VKeyCode::Home;
	//		case PKeyCode::End: return VKeyCode::End;
	//		case PKeyCode::PageUp: return VKeyCode::PageUp;
	//		case PKeyCode::PageDown: return VKeyCode::PageDown;
	//		case PKeyCode::Backspace: return VKeyCode::Backspace;
	//		case PKeyCode::Delete: return VKeyCode::Delete;

	//		// Layout specific
	//		case PKeyCode::Å: return (info.ModifierShiftOnly()) ? VKeyCode::Å : VKeyCode::å;
	//		case PKeyCode::Ä: return (info.ModifierShiftOnly()) ? VKeyCode::Ä : VKeyCode::ä;
	//		case PKeyCode::Ö: return (info.ModifierShiftOnly()) ? VKeyCode::Ö : VKeyCode::ö;

	//		case PKeyCode::Quote: return (info.ModifierShiftOnly() ? VKeyCode::Asterisk : VKeyCode::Quote);
	//		case PKeyCode::Comma: return (info.ModifierShiftOnly()) ? VKeyCode::Comma : VKeyCode::Semicolon;
	//		case PKeyCode::Period: return (info.ModifierShiftOnly()) ? VKeyCode::Period : VKeyCode::Colon;
	//		case PKeyCode::Minus: return (info.ModifierShiftOnly()) ? VKeyCode::Minus : VKeyCode::Underscore;

	//		case PKeyCode::LessThan:
	//		{
	//			VKeyCode vKey = VKeyCode::LessThan;

	//			if (info.ModifierAltGrOnly())
	//				vKey = VKeyCode::Pipe;
	//			else if (info.ModifierShiftOnly())
	//				vKey = VKeyCode::GreaterThan;

	//			return vKey;
	//		}

	//		case PKeyCode::MouseLeft: return VKeyCode::MouseLeft;
	//		case PKeyCode::MouseMiddle: return VKeyCode::MouseMiddle;
	//		case PKeyCode::MouseRight: return VKeyCode::MouseRight;
	//	}
	//	return VKeyCode::Unknown;
	//}

	//PKeyCode GetPhysicalKey(VKeyCode key, KeyInfo* infoOut = nullptr)
	//{
	//	switch (key)
	//	{
	//		// Alphabetical
	//		case VKeyCode::a: case VKeyCode::b: case VKeyCode::c: case VKeyCode::d:
	//		case VKeyCode::e: case VKeyCode::f: case VKeyCode::g: case VKeyCode::h:
	//		case VKeyCode::i: case VKeyCode::j: case VKeyCode::k: case VKeyCode::l:
	//		case VKeyCode::m: case VKeyCode::n: case VKeyCode::o: case VKeyCode::p:
	//		case VKeyCode::q: case VKeyCode::r: case VKeyCode::s: case VKeyCode::t:
	//		case VKeyCode::u: case VKeyCode::v: case VKeyCode::w: case VKeyCode::x:
	//		case VKeyCode::y: case VKeyCode::z:
	//		case VKeyCode::å: case VKeyCode::ä: case VKeyCode::ö:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}

	//			int PStart = (int)PKeyCode::A;
	//			int VStart = (int)VKeyCode::a;
	//			int indexInLetters = (int)key - VStart;

	//			PKeyCode pKey = (PKeyCode)(PStart + indexInLetters);

	//			return pKey;
	//		}

	//		case VKeyCode::A: case VKeyCode::B: case VKeyCode::C: case VKeyCode::D:
	//		case VKeyCode::E: case VKeyCode::F: case VKeyCode::G: case VKeyCode::H:
	//		case VKeyCode::I: case VKeyCode::J: case VKeyCode::K: case VKeyCode::L:
	//		case VKeyCode::M: case VKeyCode::N: case VKeyCode::O: case VKeyCode::P:
	//		case VKeyCode::Q: case VKeyCode::R: case VKeyCode::S: case VKeyCode::T:
	//		case VKeyCode::U: case VKeyCode::V: case VKeyCode::W: case VKeyCode::X:
	//		case VKeyCode::Y: case VKeyCode::Z:
	//		case VKeyCode::Å: case VKeyCode::Ä: case VKeyCode::Ö:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}

	//			int PStart = (int)PKeyCode::A;
	//			int VStart = (int)VKeyCode::A;
	//			int indexInLetters = (int)key - VStart;

	//			PKeyCode pKey = (PKeyCode)(PStart + indexInLetters);

	//			return pKey;
	//		}

	//		// Symbols

	//		// Num1
	//		case VKeyCode::Exclamation:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Num1;
	//		}

	//		// Num2
	//		case VKeyCode::At:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Ctrl = true;
	//				infoOut->Alt = true;
	//			}
	//			return PKeyCode::Num2;
	//		}
	//		case VKeyCode::DoubleQuote:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Num2;
	//		}

	//		// Num3
	//		case VKeyCode::Pound:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Ctrl = true;
	//				infoOut->Alt = true;
	//			}
	//			return PKeyCode::Num3;
	//		}
	//		case VKeyCode::Number:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Num3;
	//		}




	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Ctrl = true;
	//				infoOut->Alt = true;
	//			}
	//			return PKeyCode::Num3;
	//		}
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Num3;
	//		}



	//		// Num4
	//		case VKeyCode::Dollar:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Ctrl = true;
	//				infoOut->Alt = true;
	//			}
	//			return PKeyCode::Num4;
	//		}
	//		case VKeyCode::Currency:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Num4;
	//		}

	//		// Num5
	//		case VKeyCode::Euro:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Ctrl = true;
	//				infoOut->Alt = true;
	//			}
	//			return PKeyCode::Num5;
	//		}
	//		case VKeyCode::Persent:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Num5;
	//		}

	//		// Num6
	//		case VKeyCode::And:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Num6;
	//		}

	//		// Num7
	//		case VKeyCode::LCurlyBracket:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Ctrl = true;
	//				infoOut->Alt = true;
	//			}
	//			return PKeyCode::Num7;
	//		}
	//		case VKeyCode::Slash:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Num7;
	//		}

	//		// Num8
	//		case VKeyCode::LSquareBracket:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Ctrl = true;
	//				infoOut->Alt = true;
	//			}
	//			return PKeyCode::Num8;
	//		}
	//		case VKeyCode::LParanthesis:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Num8;
	//		}

	//		// Num9
	//		case VKeyCode::RSquareBracket:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Ctrl = true;
	//				infoOut->Alt = true;
	//			}
	//			return PKeyCode::Num9;
	//		}
	//		case VKeyCode::RParanthesis:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Num9;
	//		}

	//		// Num0
	//		case VKeyCode::RCurlyBracket:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Ctrl = true;
	//				infoOut->Alt = true;
	//			}
	//			return PKeyCode::Num0;
	//		}
	//		case VKeyCode::Equal:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Num0;
	//		}

	//		//case VKeyCode::Plus:
	//		//{
	//		//	if (infoOut)
	//		//	{
	//		//		*infoOut = KeyInfo();
	//		//	}
	//		//	return PKeyCode::Plus;
	//		//}
	//		//{
	//		//	if (infoOut)
	//		//	{
	//		//		*infoOut = KeyInfo();
	//		//		infoOut->Shift = true;
	//		//	}
	//		//	return PKeyCode::Num3;
	//		//}

	//		//case VKeyCode::BackSlash:
	//		//case VKeyCode::Question:

	//		case VKeyCode::Quote:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::Quote;
	//		}
	//		case VKeyCode::Asterisk:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Quote;
	//		}

	//		case VKeyCode::Comma:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::Comma;
	//		}
	//		case VKeyCode::Semicolon:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Comma;
	//		}

	//		case VKeyCode::Period:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::Period;
	//		}
	//		case VKeyCode::Colon:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Period;
	//		}

	//		case VKeyCode::Minus:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::Minus;
	//		}
	//		case VKeyCode::Underscore:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::Minus;
	//		}

	//		case VKeyCode::LessThan:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::LessThan;
	//		}
	//		case VKeyCode::GreaterThan:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Shift = true;
	//			}
	//			return PKeyCode::LessThan;
	//		}
	//		case VKeyCode::Pipe:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//				infoOut->Ctrl = true;
	//				infoOut->Alt = true;
	//			}
	//			return PKeyCode::LessThan;
	//		}

	//		case VKeyCode::Tab:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::Tab;
	//		}

	//		// Number row
	//		case VKeyCode::Num0:
	//		case VKeyCode::Num1:
	//		case VKeyCode::Num2:
	//		case VKeyCode::Num3:
	//		case VKeyCode::Num4:
	//		case VKeyCode::Num5:
	//		case VKeyCode::Num6:
	//		case VKeyCode::Num7:
	//		case VKeyCode::Num8:
	//		case VKeyCode::Num9:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}

	//			int PStart = (int)PKeyCode::Num0;
	//			int VStart = (int)VKeyCode::Num0;
	//			int indexInLetters = (int)key - VStart;

	//			PKeyCode pKey = (PKeyCode)(PStart + indexInLetters);

	//			return pKey;
	//		}

	//		// Function numbers
	//		case VKeyCode::F1:
	//		case VKeyCode::F2:
	//		case VKeyCode::F3:
	//		case VKeyCode::F4:
	//		case VKeyCode::F5:
	//		case VKeyCode::F6:
	//		case VKeyCode::F7:
	//		case VKeyCode::F8:
	//		case VKeyCode::F9:
	//		case VKeyCode::F10:
	//		case VKeyCode::F11:
	//		case VKeyCode::F12:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}

	//			int PStart = (int)PKeyCode::F1;
	//			int VStart = (int)VKeyCode::F1;
	//			int indexInLetters = (int)key - VStart;

	//			PKeyCode pKey = (PKeyCode)(PStart + indexInLetters);

	//			return pKey;
	//		}

	//		// Modifiers
	//		case VKeyCode::LControl:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::LControl;
	//		}
	//		case VKeyCode::RControl:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::RControl;
	//		}

	//		case VKeyCode::LShift:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::LShift;
	//		}
	//		case VKeyCode::RShift:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::RShift;
	//		}

	//		case VKeyCode::Alt:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::Alt;
	//		}

	//		case VKeyCode::AltGr:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::AltGr;
	//		}

	//		// Other function keys
	//		case VKeyCode::Escape:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::Escape;
	//		}
	//		case VKeyCode::PrintScreen:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::PrintScreen;
	//		}
	//		case VKeyCode::ScreenLock:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::ScreenLock;
	//		}
	//		case VKeyCode::Pause:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::Pause;
	//		}
	//		case VKeyCode::Insert:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::Insert;
	//		}
	//		case VKeyCode::Home:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::Home;
	//		}
	//		case VKeyCode::End:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::End;
	//		}
	//		case VKeyCode::PageUp:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::PageUp;
	//		}
	//		case VKeyCode::PageDown:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::PageDown;
	//		}
	//		case VKeyCode::Backspace:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::Backspace;
	//		}
	//		case VKeyCode::Delete:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::Delete;
	//		}

	//		// Mouse
	//		case VKeyCode::MouseLeft:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::MouseLeft;
	//		}
	//		case VKeyCode::MouseMiddle:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::MouseMiddle;
	//		}
	//		case VKeyCode::MouseRight:
	//		{
	//			if (infoOut)
	//			{
	//				*infoOut = KeyInfo();
	//			}
	//			return PKeyCode::MouseRight;
	//		}
	//	}
	//}

	enum class EventType
	{
		Unknown = 0,

		KeyDown,
		KeyUp,

		MouseMove,
		MouseKeyDown,
		MouseKeyUp,
	};

	#define EVENT_TYPE(structName, type)\
		static EventType GetStaticType() { return EventType::type; }\
		static std::string GetStaticTypeName() { return #type; }\
		EventType GetType() override { return structName::GetStaticType(); }\
		std::string GetTypeName() override { return structName::GetStaticTypeName(); }

	struct Event
	{
	public:

		Event(EventType type) : Type(type) {};
		virtual ~Event() = default;

		template <typename T, typename Func>
		bool CallIf(const Func& func)
		{
			if (Type == T::GetStaticType())
			{
				Handled |= func(static_cast<T&>(*this));
				return true;
			}
			return false;
		}

		virtual EventType GetType() = 0;
		virtual std::string GetTypeName() = 0;

	public:

		bool Handled = false;
		const EventType Type;
	};

	struct KeyDownEvent : public Event
	{
		EVENT_TYPE(KeyDownEvent, KeyDown)
			KeyDownEvent(KeyCode key, KeyInfo info = KeyInfo()) :
			PKey(key),
			Info(info),
			Event(EventType::KeyDown) {}

		const KeyCode PKey;
		const KeyInfo Info;
	};

	struct KeyUpEvent : public Event
	{
		EVENT_TYPE(KeyUpEvent, KeyUp)
		KeyUpEvent(KeyCode key, KeyInfo info = KeyInfo()) :
			PKey(key),
			Info(info),
			Event(EventType::KeyUp) {}

		const KeyCode PKey;
		const KeyInfo Info;
	};

	struct MouseMoveEvent : public Event
	{
		EVENT_TYPE(MouseMoveEvent, MouseMove)
		MouseMoveEvent(float x, float y) : 
			X(x),
			Y(y),
			Event(EventType::MouseMove) {}

		// TODO: Change to float2
		float X, Y;
	};
}