#pragma once
#include "envision\envpch.h"
#include "envision\resource\ResourceManager.h"

namespace env
{
	class DX12ResourceManager : public ResourceManager
	{
		int m_frameCount;
		int m_currentFrameIndex;

		IDX12Heap* m_staticBufferHeap;
		IDX12Heap* m_staticTextureHeap;
		IDX12Heap* m_staticSamplerHeap;

		IDX12Heap** m_frameBufferHeaps;
		IDX12Heap** m_frameTextureHeaps;
		IDX12Heap** m_frameSamplerHeaps;

		IDX12Resource** m_frameUploadBuffers;

	public:

		DX12ResourceManager() = default;
		~DX12ResourceManager() = default;

	public:

		void Initialize(IDX12Device* device);
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
