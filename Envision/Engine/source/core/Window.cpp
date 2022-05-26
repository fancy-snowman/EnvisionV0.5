#include "envision/envpch.h"
#include "envision/core/Window.h"

#include "envision/core/Application.h"
#include "envision/core/Event.h"

WNDCLASS env::Window::s_windowClass = { 0 };
const WCHAR* env::Window::s_WINDOW_CLASS_NAME = L"ENV_WINDOW_CLASS";

void env::Window::InitWindowClass()
{
	WNDCLASS windowClass = { 0 };
	windowClass.lpszClassName = s_WINDOW_CLASS_NAME;
	windowClass.cbWndExtra = sizeof(Window*);

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

void env::Window::SetWindowObject(HWND handle, Window* window)
{
	SetWindowLongPtrA(handle, GWLP_USERDATA, (LONG_PTR)window);
}

env::Window* env::Window::GetWindowObject(HWND handle)
{
	return (Window*)GetWindowLongPtrA(handle, GWLP_USERDATA);
}

env::Window::Window(int width, int height, const std::string& title, Application& application) :
	m_application(application)
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

env::Window::~Window()
{
	//
}

HWND env::Window::GetHandle()
{
	return m_handle;
}

int env::Window::GetWidth()
{
	RECT rect;
	GetWindowRect(m_handle, &rect);
	int width = (int)(rect.right - rect.left);
	return width;
}

int env::Window::GetHeight()
{
	RECT rect;
	GetWindowRect(m_handle, &rect);
	int height = (int)(rect.bottom - rect.top);
	return height;
}

float env::Window::GetAspectRatio()
{
	RECT rect;
	GetWindowRect(m_handle, &rect);
	float height = (float)(rect.bottom - rect.top);
	float width = (float)(rect.right - rect.left);
	return width / height;
}

void env::Window::OnEventUpdate()
{
	MSG msg = { 0 };
	while (PeekMessageA(&msg, m_handle, NULL, NULL, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}
