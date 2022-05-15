#include "envision/envpch.h"
#include "envision/platform/Windows/WindowsWindow.h"

#include "envision/core/Event.h"
#include "envision/core/Application.h"

WNDCLASS env::WindowsWindow::s_windowClass = { 0 };
const WCHAR* env::WindowsWindow::s_WINDOW_CLASS_NAME = L"ENV_WINDOW_CLASS";

void env::WindowsWindow::InitWindowClass()
{
	WNDCLASS windowClass = { 0 };
	windowClass.lpszClassName = s_WINDOW_CLASS_NAME;
	windowClass.cbWndExtra = sizeof(WindowsWindow*);

	windowClass.lpfnWndProc = [](HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
		switch (uMsg)
		{
		case WM_KEYDOWN:
		{
			KeyCode code = KeyCode::Unknown;
			KeyInfo info;

			if (wParam >= 0x41 && wParam <= 0x5A) // Between A-Z
			{
				int index = (int)wParam - 0x41;
				code = (KeyCode)((int)KeyCode::A + index);

				KeyDownEvent event(code, info);
				GetWindowObject(hwnd)->m_application.PublishEvent(event);
			}
		}

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	};
	RegisterClass(&windowClass);
}

void env::WindowsWindow::SetWindowObject(HWND handle, WindowsWindow* window)
{
	SetWindowLongPtrA(handle, GWLP_USERDATA, (LONG_PTR)window);
}

env::WindowsWindow* env::WindowsWindow::GetWindowObject(HWND handle)
{
	return (WindowsWindow*)GetWindowLongPtrA(handle, GWLP_USERDATA);
}

env::WindowsWindow::WindowsWindow(int width, int height, const std::string& title, Application& application) :
	env::Window(application)
{
	if (!s_windowClass.lpfnWndProc)
	{
		InitWindowClass();
	}

	std::wstring wtitle(title.begin(), title.end());

	m_handle = CreateWindow(
		s_WINDOW_CLASS_NAME,
		wtitle.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		NULL,
		NULL,
		NULL,
		NULL);

	ASSERT(m_handle, "Could not create application window");

	SetWindowObject(m_handle, this);
	ShowWindow(m_handle, SW_SHOW);
}

env::WindowsWindow::~WindowsWindow()
{

}

void env::WindowsWindow::OnUpdate()
{
	MSG msg = { 0 };
	while (PeekMessageA(&msg, m_handle, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}

int env::WindowsWindow::GetWidth()
{
	return 0;
}

int env::WindowsWindow::GetHeight()
{
	return 0;
}

int env::WindowsWindow::GetAspectRatio()
{
	return 0;
}
