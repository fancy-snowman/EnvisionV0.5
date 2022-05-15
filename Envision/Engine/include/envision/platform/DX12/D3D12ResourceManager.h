#pragma once
#include "envision/envpch.h"
#include "envision/resource/ResourceManager.h"

namespace env
{
	class D3D12ResourceManager : public ResourceManager
	{
		struct FrameData
		{
			ID3D12Heap* BufferHeap = nullptr;
			ID3D12Heap* TextureHeap = nullptr;
			ID3D12Resource* UploadBuffer = nullptr;
			
			ID3D12Fence* Fence = nullptr;
			UINT64 FenceValue = 0;
			HANDLE FanceHandle = NULL;
		};

		int m_numFrames = 0;
		int m_currentFrameIndex = 0;

		ID3D12Device* m_device = nullptr;

		ID3D12Heap* m_staticBufferHeap = nullptr;
		ID3D12Heap* m_staticTextureHeap = nullptr;

		// Descriptor heaps for storing resource views
		// These are not shader visible
		ID3D12DescriptorHeap* m_bufferDescriptorHeap = nullptr;
		ID3D12DescriptorHeap* m_textureDescriptorHeap = nullptr;
		ID3D12DescriptorHeap* m_samplerDescriptorHeap = nullptr;

		FrameData* m_frameData = nullptr;

	public:

		D3D12ResourceManager();
		~D3D12ResourceManager() final = default;

	public:

		void Initialize(ID3D12Device* device);
		void Finalize();

	public:

		ID CreateBuffer(const std::string& name, const UpdatePattern update, const AccessPattern access, const size_t byteWidth) final;
		ID CreateBufferArray(const std::string& name, const UpdatePattern update, const AccessPattern access, const size_t byteWidth, const size_t elementStride, const int elementCount) final;
		ID CreateShader(const std::string& name, const std::string& FilePath, const ShaderType type) final;
		ID CreateTexture2D(const std::string& name, const UpdatePattern update, const AccessPattern access, const int width, const int height, const ElementFormatType elementFormat) final;
		ID CreateTexture2DArray(const std::string& name, const UpdatePattern update, const AccessPattern access, const int width, const int height, const ElementFormatType elementFormat, const int textureCount) final;
		ID CreateWindowTarget(const std::string& name, env::Window* window, float startXFactor, float startYFactor, float widthFactor, float heightFactor) final;
	};
}
