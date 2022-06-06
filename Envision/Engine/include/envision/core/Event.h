#pragma once

#include "KeyCodes.h"

// Inspiration:
// https://www.w3.org/TR/DOM-Level-3-Events/#events-keyboardevents

namespace env
{
	struct MouseModifiers
	{
		bool Ctrl : 1;
		bool Shift : 1;
		bool LeftMouse : 1;
		bool MiddleMouse : 1;
		bool RightMouse : 1;
	};

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

	enum class EventType
	{
		Unknown = 0,

		KeyDown,
		KeyUp,

		MouseMove,
		MouseKeyDown,
		MouseKeyUp,
		MouseScroll,
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
			Code(key),
			Info(info),
			Event(EventType::KeyDown) {}

		const KeyCode Code;
		const KeyInfo Info;
	};

	struct KeyUpEvent : public Event
	{
		EVENT_TYPE(KeyUpEvent, KeyUp)
		KeyUpEvent(KeyCode key, KeyInfo info = KeyInfo()) :
			Code(key),
			Info(info),
			Event(EventType::KeyUp) {}

		const KeyCode Code;
		const KeyInfo Info;
	};

	struct MouseMoveEvent : public Event
	{
		EVENT_TYPE(MouseMoveEvent, MouseMove)
		MouseMoveEvent(float posX, float posY, float deltaX, float deltaY, MouseModifiers modifiers) : 
			PosX(posX),
			PosY(posY),
			DeltaX(deltaX),
			DeltaY(deltaY),
			Modifiers(modifiers),
			Event(EventType::MouseMove) {}

		// TODO: Change to float2
		float PosX, PosY;
		float DeltaX, DeltaY;
		MouseModifiers Modifiers;
	};

	struct MouseScrollEvent : public Event
	{
		EVENT_TYPE(MouseScrollEvent, MouseScroll)
		MouseScrollEvent(float delta, MouseModifiers modifiers) :
			Delta(delta),
			Modifiers(modifiers),
			Event(EventType::MouseScroll) {}

		// TODO: Change to float2
		float Delta;
		MouseModifiers Modifiers;
	};
}