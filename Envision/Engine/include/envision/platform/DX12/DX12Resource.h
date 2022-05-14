#pragma once
#include "envision\resource\Resource.h"

namespace env
{
	class DX12ResourceManager;

	struct DX12Buffer : public env::Buffer
	{
		IDX12Resource* Resource;
	private:
		DX12Buffer(const std::string& name, const ID resourceID);
		friend class env::DX12ResourceManager;
	};

	struct DX12BufferArray : public env::BufferArray
	{
		IDX12Resource* Resource;
	private:
		DX12BufferArray(const std::string& name, const ID resourceID);
		friend class env::DX12ResourceManager;
	};

	struct DX12Shader : public env::Shader
	{
		IDX12Resource* Resource;
	private:
		DX12Shader(const std::string& name, const ID resourceID);
		friend class env::DX12ResourceManager;
	};

	struct DX12Texture2D : public env::Texture2D
	{
		IDX12Resource* Resource;
	private:
		DX12Texture2D(const std::string& name, const ID resourceID);
		friend class env::DX12ResourceManager;
	};

	struct DX12Texture2DArray : public env::Texture2DArray
	{
		IDX12Resource* Resource;
	private:
		DX12Texture2DArray(const std::string& name, const ID resourceID);
		friend class env::DX12ResourceManager;
	};

	struct DX12Window : public env::WindowTarget
	{
		IDXSwapChain* SwapChain;
		IDX12CommandQueue* PresentQueue;
		void GetWidth() final;
		void GetHeight() final;
		void Update() final;
	private:
		DX12Window(const std::string& name, const ID resourceID);
		friend class env::DX12ResourceManager;
	};
}