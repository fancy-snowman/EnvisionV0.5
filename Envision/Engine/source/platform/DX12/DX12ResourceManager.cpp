#include "envision/envpch.h"
#include "envision/platform/DX12/D3D12Resource.h"
#include "envision/platform/DX12/D3D12ResourceManager.h"
#include "envision/platform/Windows/WindowsWindow.h"

env::D3D12ResourceManager::D3D12ResourceManager(env::IDGenerator& idGenerator, ID3D12Device* device) :
	m_device(device), env::ResourceManager(idGenerator)
{	
	Initialize();
}

void env::D3D12ResourceManager::Initialize()
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

		hr = m_device->CreateHeap(&desc, IID_PPV_ARGS(&m_staticBufferHeap));
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
		desc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES; // TODO: Create a specific heap for targets?

		hr = m_device->CreateHeap(&desc, IID_PPV_ARGS(&m_staticTextureHeap));
		ASSERT_HR(hr, "Could not create static texture heap");
	}

	{ // Descriptor heaps
		D3D12_DESCRIPTOR_HEAP_DESC desc;

		// Buffer descriptors
		ZeroMemory(&desc, sizeof(desc));
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 10000; // TODO: Make this more dynamic
		hr = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_bufferDescriptorHeap));
		ASSERT_HR(hr, "Could not create buffer descriptor heap");

		// Texture descriptors
		ZeroMemory(&desc, sizeof(desc));
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 10000; // TODO: Make this more dynamic
		hr = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_textureDescriptorHeap));
		ASSERT_HR(hr, "Could not create texture descriptor heap");

		// Sampler descriptors
		ZeroMemory(&desc, sizeof(desc));
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 50; // TODO: Make this more dynamic
		hr = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_bufferDescriptorHeap));
		ASSERT_HR(hr, "Could not create sampler descriptor heap");
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
		textureHeapDesc.Flags = D3D12_HEAP_FLAG_DENY_BUFFERS | D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES;

		D3D12_HEAP_PROPERTIES uploadHeapProps;
		ZeroMemory(&uploadHeapProps, sizeof(uploadHeapProps));
		uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
		uploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		uploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		uploadHeapProps.CreationNodeMask = 0;
		uploadHeapProps.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC uploadResourceDesc;
		ZeroMemory(&uploadResourceDesc, sizeof(uploadResourceDesc));
		uploadResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		uploadResourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		uploadResourceDesc.Width = 10000000;
		uploadResourceDesc.Height = 1;
		uploadResourceDesc.DepthOrArraySize = 1;
		uploadResourceDesc.MipLevels = 1;
		uploadResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		uploadResourceDesc.SampleDesc.Count = 1;
		uploadResourceDesc.SampleDesc.Quality = 0;
		uploadResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		uploadResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		for (int i = 0; i < m_numFrames; i++)
		{
			FrameData& frameData = m_frameData[i];

			hr = m_device->CreateHeap(&bufferHeapDesc, IID_PPV_ARGS(&m_frameData[i].BufferHeap));
			ASSERT_HR(hr, "Could not create dynamic buffer heap #" + std::to_string(i));

			hr = m_device->CreateHeap(&textureHeapDesc, IID_PPV_ARGS(&m_frameData[i].TextureHeap));
			ASSERT_HR(hr, "Could not create dynamic texture heap #" + std::to_string(i));

			hr = m_device->CreateCommittedResource(&uploadHeapProps,
				D3D12_HEAP_FLAG_NONE,
				&uploadResourceDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				NULL,
				IID_PPV_ARGS(&m_frameData[i].UploadBuffer));
			ASSERT_HR(hr, "Could not create dynamic upload heap #" + std::to_string(i));
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

ID env::D3D12ResourceManager::CreateWindowTarget(const std::string& name, env::Window* window, float startXFactor, float startYFactor, float widthFactor, float heightFactor)
{
	ID3D12CommandQueue* commandQueue = nullptr;
	ID3D12CommandAllocator* commandAllocator = nullptr;
	ID3D12GraphicsCommandList* commandList = nullptr;
	ID3D12Fence* fence = nullptr;
	HANDLE fenceHandle = NULL;

	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	IDXGISwapChain1* swapChain = nullptr;

	HWND windowHandle = NULL;
	UINT windowWidth = 0;
	UINT windowHeight = 0;

	{
		WindowsWindow* windowsWindow = static_cast<WindowsWindow*>(window);
		windowHandle = windowsWindow->GetHandle();
		windowWidth = windowsWindow->GetWidth();
		windowHeight = windowsWindow->GetHeight();
	}

	HRESULT hr = S_OK;

	{ // Present command interface
		D3D12_COMMAND_QUEUE_DESC presentQueueDesc;
		ZeroMemory(&presentQueueDesc, sizeof(presentQueueDesc));
		hr = m_device->CreateCommandQueue(&presentQueueDesc,
			IID_PPV_ARGS(&commandQueue));
		ASSERT_HR(hr, "Could not create present queue");

		hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocator));
		ASSERT_HR(hr, "Could not create present command allocator");

		hr = m_device->CreateCommandList(NULL,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			commandAllocator,
			NULL,
			IID_PPV_ARGS(&commandList));
		ASSERT_HR(hr, "Could not create present command list");
	}

	{ // Present syncronization
		hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
		ASSERT_HR(hr, "Could not create present fence");
		fenceHandle = CreateEvent(0, FALSE, FALSE, 0);
	}

	{ // Swap chain

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.Width = windowWidth;
		swapChainDesc.Height = windowHeight;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = m_numFrames;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		//swapChainDesc.Windowed = true;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		IDXGIFactory7* factory = nullptr;

#ifdef _DEBUG
		hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
#else
		hr = CreateDXGIFactory(IID_PPV_ARGS(&g_factory));
#endif
		ASSERT_HR(hr, "Could not create DXGIFactory");

		hr = factory->CreateSwapChainForHwnd(commandQueue, windowHandle, &swapChainDesc, NULL, NULL, &swapChain);
		factory->Release();
		ASSERT_HR(hr, "Could not create swapchain");
	}

	{ // Descriptor heap for render target views
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
		ZeroMemory(&descriptorHeapDesc, sizeof(descriptorHeapDesc));
		descriptorHeapDesc.NumDescriptors = m_numFrames;
		
		hr = m_device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
		ASSERT_HR(hr, "Could not create present descriptor heap");
	}

	ID targetID = m_IDGenerator.GenerateUnique();
	D3D12WindowTarget* target = new D3D12WindowTarget(name, targetID);

	target->PresentQueue = commandQueue;
	target->PresentCommandAllocator = commandAllocator;
	target->PresentCommandList = commandList;

	target->PresentFence = fence;
	target->PresentFenceValue = 0;
	target->PresentFenceEvent = fenceHandle;

	target->SwapChain = swapChain;
	target->State = D3D12_RESOURCE_STATE_PRESENT;

	target->Viewport.TopLeftX = (FLOAT)windowWidth * startXFactor;
	target->Viewport.TopLeftY = (FLOAT)windowHeight * startYFactor;
	target->Viewport.Width = (FLOAT)windowWidth * widthFactor;
	target->Viewport.Height = (FLOAT)windowHeight * heightFactor;
	target->Viewport.MinDepth = (FLOAT)0.0f;
	target->Viewport.MaxDepth = (FLOAT)1.0f;

	target->ScissorRect.left = (LONG)target->Viewport.TopLeftX;
	target->ScissorRect.top = (LONG)target->Viewport.TopLeftY;
	target->ScissorRect.right = (LONG)target->Viewport.Width;
	target->ScissorRect.bottom = (LONG)target->Viewport.Height;

	m_windowTargets[targetID] = target;
	return targetID;
}
