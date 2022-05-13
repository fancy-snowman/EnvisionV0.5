#pragma once

namespace env
{
	enum class EventType
	{
		Unknown = 0,

		KeyDown,
		KeyUp,

		MouseMove,
		MouseKeyDown,
		MouseKeyUp,
	};

	#define EVENT_TYPE(type) static EventType GetStaticType() { return EventType::type; }

	struct Event
	{
	public:

		Event(EventType type) : Type(type) {};
		virtual ~Event() = default;

		template <typename T, typename Func>
		bool CallIf(Func func)
		{
			if (Type == T::GetStaticType())
			{
				Handled |= func(static_cast<T&>(*this));
				return true;
			}
			return false;
		}

	public:

		bool Handled = false;
		const EventType Type;
	};
}