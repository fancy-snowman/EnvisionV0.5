#pragma once
#include "envision\envpch.h"

namespace env
{
	// Defined in Application.h
	class Application;

	class Window
	{
	protected:

		Application& m_application;

	public:

		Window(Application& application) : m_application(application) {}
		virtual ~Window() {}

	public:

		virtual void OnAttach() {}
		virtual void OnUpdate() {}
		virtual void OnDetach() {}

	public:

		virtual int GetWidth() = 0;
		virtual int GetHeight() = 0;
		virtual int GetAspectRatio() = 0;
	};

	// Defined in PlatformAdapter.cpp
	Window* CreateAppWindow(int width, int height, const std::string& title, Application& application);
}