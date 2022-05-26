#pragma once
#include "envision/envpch.h"

namespace env
{
	// Defined in Application.h
	class Application;

	class Window
	{
	private:

		static WNDCLASS s_windowClass;
		static const WCHAR* s_WINDOW_CLASS_NAME;

		Application& m_application;
		HWND m_handle = NULL;	

		void InitWindowClass();
		static void SetWindowObject(HWND handle, Window* window);
		static Window* GetWindowObject(HWND handle);

	public:

		Window(int width, int height, const std::string& title, Application& application);
		virtual ~Window();

	public:

		HWND GetHandle();
		int GetWidth();
		int GetHeight();
		float GetAspectRatio();

		void OnEventUpdate();

	public:

		virtual void OnAttach() {}
		virtual void OnUpdate() {}
		virtual void OnDetach() {}
	};
}