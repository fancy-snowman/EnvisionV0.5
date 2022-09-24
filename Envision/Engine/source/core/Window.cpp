#include "envision/envpch.h"
#include "envision/core/Window.h"

#include "envision/core/Application.h"
#include "envision/core/Event.h"
#include "envision/core/GPU.h"
#include "envision/graphics/GUI.h"
#include "envision/resource/ResourceManager.h"

WNDCLASS env::Window::s_windowClass = { 0 };
const WCHAR* env::Window::s_WINDOW_CLASS_NAME = L"ENV_WINDOW_CLASS";

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void env::Window::InitWindowClass()
{
	WNDCLASS windowClass = { 0 };
	windowClass.lpszClassName = s_WINDOW_CLASS_NAME;
	windowClass.cbWndExtra = sizeof(Window*);

	windowClass.lpfnWndProc = [](HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
		
		Window* window = GetWindowObject(hwnd);

		if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
			return true;
		
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
			}

			else if (wParam == VK_UP) { code = KeyCode::Up; }
			else if (wParam == VK_LEFT) { code = KeyCode::Left; }
			else if (wParam == VK_DOWN) { code = KeyCode::Down; }
			else if (wParam == VK_RIGHT) { code = KeyCode::Right; }

			KeyDownEvent event(code, info);
			GetWindowObject(hwnd)->m_application.PublishEvent(event);

			return 0;
		}

		case WM_KEYUP:
		{
			KeyCode code = KeyCode::Unknown;
			KeyInfo info;

			if (wParam >= 0x41 && wParam <= 0x5A) // Between A-Z
			{
				int index = (int)wParam - 0x41;
				code = (KeyCode)((int)KeyCode::A + index);
			}

			else if (wParam == VK_UP) { code = KeyCode::Up; }
			else if (wParam == VK_LEFT) { code = KeyCode::Left; }
			else if (wParam == VK_DOWN) { code = KeyCode::Down; }
			else if (wParam == VK_RIGHT) { code = KeyCode::Right; }

			KeyUpEvent event(code, info);
			GetWindowObject(hwnd)->m_application.PublishEvent(event);

			return 0;
		}

		case WM_MOUSEMOVE:
		{
			static float lastPosX = LOWORD(lParam);
			static float lastPosY = HIWORD(lParam);

			float posX = LOWORD(lParam);
			float posY = HIWORD(lParam);
			float deltaX = lastPosX - posX;
			float deltaY = lastPosY - posY;
			lastPosX = posX;
			lastPosY = posY;

			MouseModifiers modifiers;
			ZeroMemory(&modifiers, sizeof(modifiers));
			
			modifiers.Ctrl = ((wParam & MK_CONTROL) == MK_CONTROL);
			modifiers.Shift = ((wParam & MK_SHIFT) == MK_SHIFT);
			modifiers.LeftMouse = ((wParam & MK_LBUTTON) == MK_LBUTTON);
			modifiers.MiddleMouse = ((wParam & MK_MBUTTON) == MK_MBUTTON);
			modifiers.RightMouse = ((wParam & MK_RBUTTON) == MK_RBUTTON);

			MouseMoveEvent event(posX, posY, deltaX, deltaY, modifiers);
			GetWindowObject(hwnd)->m_application.PublishEvent(event);
			return 0;
		}

		case WM_MOUSEWHEEL:
		{
			//static float lastPosX = LOWORD(lParam);
			//static float lastPosY = HIWORD(lParam);

			//float posX = LOWORD(lParam);
			//float posY = HIWORD(lParam);
			//float deltaX = lastPosX - posX;
			//float deltaY = lastPosY - posY;
			//lastPosX = posX;
			//lastPosY = posY;

			WORD eventMods = LOWORD(wParam);
			short eventDelta = HIWORD(wParam);

			float delta = (float)eventDelta / WHEEL_DELTA;

			MouseModifiers modifiers;
			ZeroMemory(&modifiers, sizeof(modifiers));

			modifiers.Ctrl = ((eventMods & MK_CONTROL) == MK_CONTROL);
			modifiers.Shift = ((eventMods & MK_SHIFT) == MK_SHIFT);
			modifiers.LeftMouse = ((eventMods & MK_LBUTTON) == MK_LBUTTON);
			modifiers.MiddleMouse = ((eventMods & MK_MBUTTON) == MK_MBUTTON);
			modifiers.RightMouse = ((eventMods & MK_RBUTTON) == MK_RBUTTON);

			MouseScrollEvent event(delta, modifiers);
			GetWindowObject(hwnd)->m_application.PublishEvent(event);
			return 0;
		}

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
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
	{ // Init Win32
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

	HRESULT hr = S_OK;

	{ // Create swap chain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.Width = GetWidth();
		swapChainDesc.Height = GetHeight();
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		//swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		IDXGIFactory7* factory = nullptr;

#ifdef _DEBUG
		hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
#else
		hr = CreateDXGIFactory(IID_PPV_ARGS(&factory));
#endif
		ASSERT_HR(hr, "Could not create DXGIFactory");

		hr = factory->CreateSwapChainForHwnd(GPU::GetPresentQueue().GetCommandQueue(),
			m_handle,
			&swapChainDesc,
			NULL,
			NULL,
			&m_swapchain);

		ASSERT_HR(hr, "Could not create swap chain");
	}

	{ // Create internal resources, one per backbuffer
		for (int i = 0; i < NUM_BACK_BUFFERS; i++)
		{
			ID3D12Resource* buffer = nullptr;
			hr = m_swapchain->GetBuffer(i, IID_PPV_ARGS(&buffer));
			ASSERT_HR(hr, "Could not query backbuffer resource");

			ID backbufferID = ResourceManager::Get()->CreateTexture2D(
				"window_backbuffer" + std::to_string(i),
				TextureBindType::RenderTarget,
				buffer);

			m_backbuffers[i] = backbufferID;
		}
	}

	env::GUI::Initialize(this);
}

env::Window::~Window()
{
	//
}

HWND env::Window::GetHandle()
{
	return m_handle;
}

int env::Window::GetWidth() const
{
	RECT rect;
	GetWindowRect(m_handle, &rect);
	int width = (int)(rect.right - rect.left);
	return width;
}

int env::Window::GetHeight() const
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

ID env::Window::GetCurrentBackbuffer()
{
	return m_backbuffers[m_currentBackbufferindex];
}

DXGI_FORMAT env::Window::GetBackbufferFormat() const
{
	DXGI_SWAP_CHAIN_DESC desc;
	m_swapchain->GetDesc(&desc);

	return desc.BufferDesc.Format;
}

void env::Window::Present()
{
	m_swapchain->Present(0, 0);
	m_currentBackbufferindex = (m_currentBackbufferindex + 1) % NUM_BACK_BUFFERS;
}

void env::Window::OnEventUpdate()
{
	UpdateWindow(m_handle);

	MSG msg = { 0 };
	while (PeekMessageA(&msg, m_handle, NULL, NULL, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
}
