#pragma once
#include "envision/envpch.h"
#include "envision/resource/Resource.h"

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

		static const UINT NUM_BACK_BUFFERS = 2;
		UINT m_currentBackbufferindex;
		IDXGISwapChain1* m_swapchain;
		ID m_backbuffers[NUM_BACK_BUFFERS];

		void InitWindowClass();
		static void SetWindowObject(HWND handle, Window* window);
		static Window* GetWindowObject(HWND handle);

	public:

		Window(int width, int height, const std::string& title, Application& application);
		virtual ~Window();

	public:

		HWND GetHandle();
		int GetWidth() const;
		int GetHeight() const;
		float GetAspectRatio();
		ID GetCurrentBackbuffer();
		DXGI_FORMAT GetBackbufferFormat() const;

		void Present();

		void OnEventUpdate();

	public:

		virtual void OnAttach() {}
		virtual void OnUpdate() {}
		virtual void OnDetach() {}
	};
}