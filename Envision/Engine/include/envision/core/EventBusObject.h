#pragma once
#include "envision\envpch.h"
#include "envision\core\Event.h"

namespace env
{
	// Defined in Application.h
	class Application;

	class EventBusObject
	{
		Application& m_application;

		EventBusObject(Application& application);
		friend class env::Application;

	public:

		EventBusObject(const EventBusObject& other);
		~EventBusObject();

	public:

		void PublishEvent(Event& event);
	};
}