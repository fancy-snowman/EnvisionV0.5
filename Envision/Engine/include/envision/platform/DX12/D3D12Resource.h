#pragma once
#include "envision/resource/Resource.h"

namespace env
{
	class D3D12ResourceManager;

	struct D3D12Buffer : public env::Buffer
	{
		ID3D12Resource* Resource;
		D3D12_RESOURCE_STATES State = D3D12_RESOURCE_STATE_COMMON;
	private:
		D3D12Buffer(const std::string& name, const ID resourceID);
		friend class env::D3D12ResourceManager;
	};

	struct D3D12BufferArray : public env::BufferArray
	{
		ID3D12Resource* Resource;
		D3D12_RESOURCE_STATES State = D3D12_RESOURCE_STATE_COMMON;
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
		D3D12_RESOURCE_STATES State = D3D12_RESOURCE_STATE_COMMON;
	private:
		D3D12Texture2D(const std::string& name, const ID resourceID);
		friend class env::D3D12ResourceManager;
	};

	struct D3D12Texture2DArray : public env::Texture2DArray
	{
		ID3D12Resource* Resource;
		D3D12_RESOURCE_STATES State = D3D12_RESOURCE_STATE_COMMON;
	private:
		D3D12Texture2DArray(const std::string& name, const ID resourceID);
		friend class env::D3D12ResourceManager;
	};

	struct D3D12WindowTarget : public env::WindowTarget
	{
		RECT ScissorRect = { 0 };
		D3D12_VIEWPORT Viewport = { 0 };
		IDXGISwapChain1* SwapChain = nullptr;

		ID3D12CommandQueue* PresentQueue = nullptr;
		ID3D12CommandAllocator* PresentCommandAllocator = nullptr;
		ID3D12GraphicsCommandList* PresentCommandList = nullptr;

		ID3D12DescriptorHeap* DescriptorHeap = nullptr;

		ID3D12Fence* PresentFence = nullptr;
		UINT64 PresentFenceValue = 0;
		HANDLE PresentFenceEvent = NULL;

		D3D12_RESOURCE_STATES State = D3D12_RESOURCE_STATE_COMMON;

	private:
		D3D12WindowTarget(const std::string& name, const ID resourceID) : 
			env::WindowTarget(resourceID, name) {}
		friend class env::D3D12ResourceManager;
	};
}