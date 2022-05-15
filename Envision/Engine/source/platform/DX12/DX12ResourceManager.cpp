#include "envision\envpch.h"
#include "..\..\include\envision\platform\DX12\D3D12ResourceManager.h"

env::D3D12ResourceManager::D3D12ResourceManager()
{
	//
}

void env::D3D12ResourceManager::Initialize(ID3D12Device* device)
{
	m_numFrames = 3;
	m_currentFrameIndex = 0;

	HRESULT hr = S_OK;

	{ // Static buffer heap
		D3D12_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.SizeInBytes = 10000000;
		desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		desc.Properties.CreationNodeMask = 0;
		desc.Properties.VisibleNodeMask = 0;
		desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;

		hr = device->CreateHeap(&desc, IID_PPV_ARGS(&m_staticBufferHeap));
		ASSERT_HR(hr, "Could not create static buffer heap");
	}

	{ // Static texture heap
		D3D12_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.SizeInBytes = 10000000;
		desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		desc.Properties.CreationNodeMask = 0;
		desc.Properties.VisibleNodeMask = 0;
		desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		desc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS;

		hr = device->CreateHeap(&desc, IID_PPV_ARGS(&m_staticTextureHeap));
		ASSERT_HR(hr, "Could not create static texture heap");
	}

	{ // Static sampler heap
		D3D12_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.SizeInBytes = 10000000;
		desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		desc.Properties.CreationNodeMask = 0;
		desc.Properties.VisibleNodeMask = 0;
		desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		desc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS;

		hr = device->CreateHeap(&desc, IID_PPV_ARGS(&m_staticSamplerHeap));
		ASSERT_HR(hr, "Could not create static sampler heap");
	}

	{ // Dynamic resources
		m_frameData = new FrameData[m_numFrames];

		D3D12_HEAP_DESC bufferHeapDesc;
		ZeroMemory(&bufferHeapDesc, sizeof(bufferHeapDesc));
		bufferHeapDesc.SizeInBytes = 100000;
		bufferHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		bufferHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		bufferHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		bufferHeapDesc.Properties.CreationNodeMask = 0;
		bufferHeapDesc.Properties.VisibleNodeMask = 0;
		bufferHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		bufferHeapDesc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;

		D3D12_HEAP_DESC textureHeapDesc;
		ZeroMemory(&textureHeapDesc, sizeof(textureHeapDesc));
		textureHeapDesc.SizeInBytes = 10000000;
		textureHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		textureHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		textureHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		textureHeapDesc.Properties.CreationNodeMask = 0;
		textureHeapDesc.Properties.VisibleNodeMask = 0;
		textureHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		textureHeapDesc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS;

		D3D12_HEAP_DESC samplerHeapDesc;
		ZeroMemory(&samplerHeapDesc, sizeof(samplerHeapDesc));
		samplerHeapDesc.SizeInBytes = 10000000;
		samplerHeapDesc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		samplerHeapDesc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		samplerHeapDesc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		samplerHeapDesc.Properties.CreationNodeMask = 0;
		samplerHeapDesc.Properties.VisibleNodeMask = 0;
		samplerHeapDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		samplerHeapDesc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS;

		D3D12_HEAP_PROPERTIES uploadHeapProps;
		ZeroMemory(&uploadHeapProps, sizeof(uploadHeapProps));
		uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		uploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		uploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadHeapProps.CreationNodeMask = 0;
		uploadHeapProps.VisibleNodeMask = 0;

		D3D12_HEAP_FLAGS uploadHeapFlags;
		ZeroMemory(&uploadHeapFlags, sizeof(uploadHeapFlags));
		uploadHeapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;

		D3D12_RESOURCE_DESC uploadResourceDesc;
		ZeroMemory(&uploadResourceDesc, sizeof(uploadResourceDesc));
		uploadResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		uploadResourceDesc.Width = 10000000;
		uploadResourceDesc.Height = 1;
		uploadResourceDesc.DepthOrArraySize = 1;
		uploadResourceDesc.MipLevels = 0;
		uploadResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadResourceDesc.SampleDesc.Count = 1;
		uploadResourceDesc.SampleDesc.Quality = 0;
		uploadResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		for (int i = 0; i < m_numFrames; i++)
		{
			FrameData& frameData = m_frameData[i];

			hr = device->CreateHeap(&bufferHeapDesc, IID_PPV_ARGS(&m_frameData[i].BufferHeap));
			ASSERT_HR(hr, "Could not create dynamic buffer heap #" + std::to_string(i));

			hr = device->CreateHeap(&textureHeapDesc, IID_PPV_ARGS(&m_frameData[i].TextureHeap));
			ASSERT_HR(hr, "Could not create dynamic buffer heap #" + std::to_string(i));

			hr = device->CreateHeap(&samplerHeapDesc, IID_PPV_ARGS(&m_frameData[i].SamplerHeap));
			ASSERT_HR(hr, "Could not create dynamic buffer heap #" + std::to_string(i));

			hr = device->CreateCommittedResource(&uploadHeapProps,
				uploadHeapFlags,
				&uploadResourceDesc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				NULL,
				IID_PPV_ARGS(&m_frameData[i].UploadBuffer));
			ASSERT_HR(hr, "Could not create dynamic buffer heap #" + std::to_string(i));
		}
	}
}

void env::D3D12ResourceManager::Finalize()
{
}

ID env::D3D12ResourceManager::CreateBuffer(const std::string& name, const UpdatePattern update, const AccessPattern access, const size_t byteWidth)
{
	return ID();
}

ID env::D3D12ResourceManager::CreateBufferArray(const std::string& name, const UpdatePattern update, const AccessPattern access, const size_t byteWidth, const size_t elementStride, const int elementCount)
{
	return ID();
}

ID env::D3D12ResourceManager::CreateShader(const std::string& name, const std::string& FilePath, const ShaderType type)
{
	return ID();
}

ID env::D3D12ResourceManager::CreateTexture2D(const std::string& name, const UpdatePattern update, const AccessPattern access, const int width, const int height, const ElementFormatType elementFormat)
{
	return ID();
}

ID env::D3D12ResourceManager::CreateTexture2DArray(const std::string& name, const UpdatePattern update, const AccessPattern access, const int width, const int height, const ElementFormatType elementFormat, const int textureCount)
{
	return ID();
}

ID env::D3D12ResourceManager::CreateAppWindow(const std::string& name, const int width, const int height)
{
	return ID();
}
