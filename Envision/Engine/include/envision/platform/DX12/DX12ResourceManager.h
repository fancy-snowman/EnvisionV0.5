#pragma once
#include "envision\envpch.h"
#include "envision\resource\ResourceManager.h"

namespace env
{
	class DX12ResourceManager : public ResourceManager
	{
	public:

		DX12ResourceManager() = default;
		~DX12ResourceManager() = default;

	public:

		Buffer* CreateBuffer() final;
		BufferArray* CreateBufferArray() final;
		Shader* CreateShader() final;
		Texture2D* CreateTexture2D() final;
		Texture2DArray* CreateTexture2DArray() final;
		Window* CreateWindow() final;
	};
}
