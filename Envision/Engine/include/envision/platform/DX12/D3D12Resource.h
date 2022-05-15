#pragma once
#include "envision/resource/Resource.h"

namespace env
{
	class D3D12ResourceManager;

	struct D3D12Buffer : public env::Buffer
	{
		ID3D12Resource* Resource;
	private:
		D3D12Buffer(const std::string& name, const ID resourceID);
		friend class env::D3D12ResourceManager;
	};

	struct D3D12BufferArray : public env::BufferArray
	{
		ID3D12Resource* Resource;
	private:
		D3D12BufferArray(const std::string& name, const ID resourceID);
		friend class env::D3D12ResourceManager;
	};

	struct D3D12Shader : public env::Shader
	{
		ID3D12Resource* Resource;
	private:
		D3D12Shader(const std::string& name, const ID resourceID);
		friend class env::D3D12ResourceManager;
	};

	struct D3D12Texture2D : public env::Texture2D
	{
		ID3D12Resource* Resource;
	private:
		D3D12Texture2D(const std::string& name, const ID resourceID);
		friend class env::D3D12ResourceManager;
	};

	struct D3D12Texture2DArray : public env::Texture2DArray
	{
		ID3D12Resource* Resource;
	private:
		D3D12Texture2DArray(const std::string& name, const ID resourceID);
		friend class env::D3D12ResourceManager;
	};

	struct D3D12WindowTarget : public env::WindowTarget
	{
		IDXGISwapChain* SwapChain;
		ID3D12CommandQueue* PresentQueue;

	private:
		D3D12WindowTarget(const std::string& name, const ID resourceID);
		friend class env::D3D12ResourceManager;
	};
}