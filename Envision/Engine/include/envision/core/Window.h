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
		Texture2D* m_backbuffers[NUM_BACK_BUFFERS];

		// Not "owned" by the window, but holds a pointer to the current
		// backbuffer and thus needs to be updated during present.
		std::vector<WindowTarget*> m_targets;

		// Dear ImGui
		bool m_usingImgui = false;

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
		Texture2D* GetCurrentBackbuffer();

		// Returns true if the window requires need to forward
		// input to GUI library (Dear ImGui)
		bool IsGUIWindow();
		void InitializeGUI();

		void PushTarget(WindowTarget* target);
		void Present();

		void OnEventUpdate();

	public:

		virtual void OnAttach() {}
		virtual void OnUpdate() {}
		virtual void OnDetach() {}
	};
}