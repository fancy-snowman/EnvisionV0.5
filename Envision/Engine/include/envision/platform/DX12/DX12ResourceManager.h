#pragma once
#include "envision\envpch.h"
#include "envision\resource\ResourceManager.h"

namespace env
{
	class DX12ResourceManager : public ResourceManager
	{
		struct FrameData
		{
			ID3D12Heap* BufferHeap = nullptr;
			ID3D12Heap* TextureHeap = nullptr;
			ID3D12Heap* SamplerHeap = nullptr;
			ID3D12Resource* UploadBuffer = nullptr;
			
			ID3D12Fence* Fence = nullptr;
			UINT64 FenceValue = 0;
			HANDLE FanceHandle = NULL;
		};

		int m_numFrames = 0;
		int m_currentFrameIndex = 0;

		ID3D12Heap* m_staticBufferHeap = nullptr;
		ID3D12Heap* m_staticTextureHeap = nullptr;
		ID3D12Heap* m_staticSamplerHeap = nullptr;

		FrameData* m_frameData = nullptr;

	public:

		DX12ResourceManager();
		~DX12ResourceManager() final = default;

	public:

		void Initialize(ID3D12Device* device);
		void Finalize();

	public:

		ID CreateBuffer(const std::string& name, const FrameRWPatternType frameRWPattern, const size_t byteWidth) final;
		ID CreateBufferArray(const std::string& name, const FrameRWPatternType frameRWPattern, const size_t byteWidth, const size_t elementStride, const int elementCount) final;
		ID CreateShader(const std::string& name, const std::string& FilePath, const ShaderType type) final;
		ID CreateTexture2D(const std::string& name, const FrameRWPatternType frameRWPattern, const int width, const int height, const ElementFormatType elementFormat) final;
		ID CreateTexture2DArray(const std::string& name, const FrameRWPatternType frameRWPattern, const int width, const int height, const ElementFormatType elementFormat, const int textureCount) final;
		ID CreateAppWindow(const std::string& name, const int width, const int height) final;
	};
}
