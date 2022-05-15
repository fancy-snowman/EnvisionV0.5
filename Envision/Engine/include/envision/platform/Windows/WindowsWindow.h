#pragma once
#include "envision\envpch.h"
#include "envision\core\Window.h"

namespace env
{
	class WindowsWindow : public Window
	{
		static WNDCLASS s_windowClass;
		static const WCHAR* s_WINDOW_CLASS_NAME;


		HWND m_handle = NULL;

		void InitWindowClass();
		static void SetWindowObject(HWND handle, WindowsWindow* window);
		static WindowsWindow* GetWindowObject(HWND handle);

	public:

		WindowsWindow(int width, int height, const std::string& title, Application& application);
		~WindowsWindow() final;

	public:

		void OnUpdate() final;

	public:

		int GetWidth() final;
		int GetHeight() final;
		float GetAspectRatio() final;

	public:

		inline HWND GetHandle()
		{
			return m_handle;
		}
	};
}
