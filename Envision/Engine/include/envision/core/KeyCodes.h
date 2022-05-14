// Inspiration, and mostly from, glfw3.h

namespace env
{
	enum class KeyCode
	{
		Unknown = 0,

		// Printable keys
		A, B, C, D,
		E, F, G, H,
		I, J, K, L,
		M, N, O, P,
		Q, R, S, T,
		U, V, W, X,
		Y, Z,

		Num0, Num1, Num2, Num3,
		Num4, Num5,	Num6, Num7,
		Num8, Num9,

		Tab, Space,

		// Function keys
		F1, F2, F3, F4,
		F5, F6, F7, F8,
		F9, F10, F11, F12,

		LControl, RControl,
		LShift, RShift,
		Alt, AltGr,

		Escape,

		PrintScreen,
		ScreenLock,
		Pause,

		Insert, Delete,
		Home, End,
		PageUp, PageDown,

		Backspace, Enter,

		Up, Left, Down, Right,
		
		// Layout specific
		Å, Ä, Ö,
		Quote,
		Comma, Period, Minus,
		LessThan,

		// Mouse
		MouseLeft,
		MouseMiddle,
		MouseRight,
	};
}
