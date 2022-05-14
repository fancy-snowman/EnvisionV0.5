#include "envision\envpch.h"

#ifdef PLATFORM_DIRECT3D_12
#include "envision\platform\DX12\DX12ResourceManager.h"
env::ResourceManager* env::CreateResourceManager()
{
	return new env::DX12ResourceManager();
}

#include "envision\platform\Windows\WindowsWindow.h"
env::Window* env::CreateAppWindow(int width, int height, const std::string& title, env::Application& application)
{
	return new env::WindowsWindow(width, height, title, application);
}
#endif