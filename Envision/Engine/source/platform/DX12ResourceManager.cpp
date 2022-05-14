#include "envision\envpch.h"
#include "..\..\include\envision\platform\DX12\DX12ResourceManager.h"

env::DX12ResourceManager::DX12ResourceManager(EventBusObject eventBus) :
	env::ResourceManager(eventBus)
{
	//
}

void env::DX12ResourceManager::Initialize(IDX12Device* device)
{
	m_frameCount = 3;
	m_currentFrameIndex = 0;

	{ // static buffer heap
		D3D12_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.SizeInBytes = 10000000.f;
		desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		desc.Properties.CreationNodeMask = 0;
		desc.Properties.VisibleNodeMask = 0;
		desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;
		ASSERT_HR(device->CreateHeap(&desc, IID_PPV_ARGS(&m_staticBufferHeap)));
	}

	{ // static texture heap
		D3D12_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.SizeInBytes = 10000000.f;
		desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		desc.Properties.CreationNodeMask = 0;
		desc.Properties.VisibleNodeMask = 0;
		desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		desc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS;
		ASSERT_HR(device->CreateHeap(&desc, IID_PPV_ARGS(&m_staticTextureHeap)));
	}

	{ // static sampler heap
		D3D12_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.SizeInBytes = 10000000.f;
		desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		desc.Properties.CreationNodeMask = 0;
		desc.Properties.VisibleNodeMask = 0;
		desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		desc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS;
		ASSERT_HR(device->CreateHeap(&desc, IID_PPV_ARGS(&m_staticSamplerHeap)));
	}

	{ // dynamic buffer heaps
		D3D12_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.SizeInBytes = 100000.f;
		desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		desc.Properties.CreationNodeMask = 0;
		desc.Properties.VisibleNodeMask = 0;
		desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;

		m_frameBufferHeaps = new IDX12Heap*[m_frameCount];
		for (int i = 0; i < m_frameCount; i++)
		{
			ASSERT_HR(device->CreateHeap(&desc, IID_PPV_ARGS(&m_frameBufferHeaps[i])));
		}
	}

	{ // dynamic texture heap
		D3D12_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.SizeInBytes = 10000000.f;
		desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		desc.Properties.CreationNodeMask = 0;
		desc.Properties.VisibleNodeMask = 0;
		desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		desc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS;

		m_frameTextureHeaps = new IDX12Heap*[m_frameCount];
		for (int i = 0; i < m_frameCount; i++)
		{
			ASSERT_HR(device->CreateHeap(&desc, IID_PPV_ARGS(&m_frameTextureHeaps[i])));
		}
	}

	{ // static sampler heap
		D3D12_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.SizeInBytes = 10000000.f;
		desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
		desc.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		desc.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		desc.Properties.CreationNodeMask = 0;
		desc.Properties.VisibleNodeMask = 0;
		desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		desc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS;

		m_frameSamplerHeaps = new IDX12Heap*[m_frameCount];
		for (int i = 0; i < m_frameCount; i++)
		{
			ASSERT_HR(device->CreateHeap(&desc, IID_PPV_ARGS(&m_frameSamplerHeaps[i])));
		}
	}

	{ // frame upload buffers
		D3D12_HEAP_PROPERTIES heapProps;
		ZeroMemory(&heapProps, sizeof(heapProps));
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 0;
		heapProps.VisibleNodeMask = 0;

		D3D12_HEAP_FLAGS heapFlags;
		ZeroMemory(&heapFlags, sizeof(heapFlags));
		heapFlags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;

		D3D12_RESOURCE_DESC resourceDesc;
		ZeroMemory(&resourceDesc, sizeof(resourceDesc));
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDesc.Width = 10000000;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 0;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		m_frameBufferHeaps = new IDX12Heap * [m_frameCount];
		for (int i = 0; i < m_frameCount; i++)
		{
			ASSERT_HR(device->CreateCommittedResource(&heapProps,
				heapFlags,
				&resourceDesc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				NULL,
				IID_PPV_ARGS(&m_frameUploadBuffers[i])));
		}
	}
}

void env::DX12ResourceManager::Finalize()
{
}

ID env::DX12ResourceManager::CreateBuffer(const std::string& name, const FrameRWPatternType frameRWPattern, const size_t byteWidth)
{
	return ID();
}

ID env::DX12ResourceManager::CreateBufferArray(const std::string& name, const FrameRWPatternType frameRWPattern, const size_t byteWidth, const size_t elementStride, const int elementCount)
{
	return ID();
}

ID env::DX12ResourceManager::CreateShader(const std::string& name, const std::string& FilePath, const ShaderType type)
{
	return ID();
}

ID env::DX12ResourceManager::CreateTexture2D(const std::string& name, const FrameRWPatternType frameRWPattern, const int width, const int height, const ElementFormatType elementFormat)
{
	return ID();
}

ID env::DX12ResourceManager::CreateTexture2DArray(const std::string& name, const FrameRWPatternType frameRWPattern, const int width, const int height, const ElementFormatType elementFormat, const int textureCount)
{
	return ID();
}

ID env::DX12ResourceManager::CreateAppWindow(const std::string& name, const int width, const int height)
{
	return ID();
}
