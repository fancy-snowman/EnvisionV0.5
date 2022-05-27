#include "envision/envpch.h"
#include "envision/resource/ResourceManager.h"
#include "envision/core/GPU.h"

env::ResourceManager* env::ResourceManager::s_instance = nullptr;

env::ResourceManager* env::ResourceManager::Initialize(IDGenerator& commonIDGenerator)
{
	if (!s_instance)
		s_instance = new ResourceManager(commonIDGenerator);
	return s_instance;
}

env::ResourceManager* env::ResourceManager::Get()
{
	assert(s_instance);
	return s_instance;
}

void env::ResourceManager::Finalize()
{
	delete s_instance;
	s_instance = nullptr;
}

env::ResourceManager::ResourceManager(IDGenerator& commonIDGenerator) :
	m_commonIDGenerator(commonIDGenerator),
	m_CBVAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 100, false),
	m_SRVAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 100, false),
	m_UAVAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 100, false),
	m_SamplerAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 10, false),
	m_RTVAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 20, false),
	m_DSVAllocator(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 20, false)
{
	HRESULT hr = S_OK;

	m_uploadBuffer.Name = "UploadBuffer";
	m_uploadBuffer.State = D3D12_RESOURCE_STATE_GENERIC_READ;
	m_uploadBuffer.Layout = { BufferElement("Data", ShaderDataType::Float, 0, 0, 1000000000) };

	{
		D3D12_HEAP_PROPERTIES heapProperties;
		ZeroMemory(&heapProperties, sizeof(heapProperties));
		heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDescription;
		ZeroMemory(&resourceDescription, sizeof(resourceDescription));
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDescription.Width = (UINT64)m_uploadBuffer.Layout.GetByteWidth();
		resourceDescription.Height = 1;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		hr = GPU::GetDevice()->CreateCommittedResource(&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDescription,
			m_uploadBuffer.State,
			NULL,
			IID_PPV_ARGS(&m_uploadBuffer.Native));

		ASSERT_HR(hr, "Could not create upload buffer");
	}

	m_transitionList = GPU::CreateDirectCommandList();
	m_copyList = GPU::CreateCopyCommandList();
}

env::ResourceManager::~ResourceManager()
{
	for (auto& pair : m_buffersArrays) { delete pair.second; }
	m_buffersArrays.clear();

	for (auto& pair : m_constantBuffers) { delete pair.second; }
	m_constantBuffers.clear();

	for (auto& pair : m_indexBuffers) { delete pair.second; }
	m_indexBuffers.clear();

	for (auto& pair : m_vertexBuffers) { delete pair.second; }
	m_vertexBuffers.clear();

	for (auto& pair : m_texture2Ds) { delete pair.second; }
	m_texture2Ds.clear();

	for (auto& pair : m_texture2DArrays) { delete pair.second; }
	m_texture2DArrays.clear();

	for (auto& pair : m_pipelineStates) { delete pair.second; }
	m_pipelineStates.clear();

	for (auto& pair : m_windowTargets) { delete pair.second; }
	m_windowTargets.clear();
}

env::Resource* env::ResourceManager::GetResourceNonConst(ID resourceID)
{
	if (m_buffersArrays.count(resourceID) > 0)
		return m_buffersArrays[resourceID];
	else if (m_constantBuffers.count(resourceID) > 0)
		return m_constantBuffers[resourceID];
	else if (m_indexBuffers.count(resourceID) > 0)
		return m_indexBuffers[resourceID];
	else if (m_vertexBuffers.count(resourceID) > 0)
		return m_vertexBuffers[resourceID];
	else if (m_texture2Ds.count(resourceID) > 0)
		return m_texture2Ds[resourceID];
	else if (m_texture2DArrays.count(resourceID) > 0)
		return m_texture2DArrays[resourceID];
	else if (m_pipelineStates.count(resourceID) > 0)
		return m_pipelineStates[resourceID];
	else if (m_windowTargets.count(resourceID) > 0)
		return m_windowTargets[resourceID];
	return nullptr;
}

void env::ResourceManager::AdjustViewportAndScissorRect(WindowTarget& target)
{
	int windowWidth = target.AppWindow->GetWidth();
	int windowHeight = target.AppWindow->GetHeight();
	target.Viewport.TopLeftX = windowWidth * target.startXFactor;
	target.Viewport.TopLeftY = windowHeight * target.startYFactor;
	target.Viewport.Width = windowWidth * target.widthFactor;
	target.Viewport.Height = windowHeight * target.heightFactor;
	target.Viewport.MinDepth = 0.0f;
	target.Viewport.MaxDepth = 0.0f;

	target.ScissorRect.left = target.Viewport.TopLeftX;
	target.ScissorRect.top = target.Viewport.TopLeftY;
	target.ScissorRect.right = target.Viewport.TopLeftX + target.Viewport.Width;
	target.ScissorRect.bottom = target.Viewport.TopLeftY + target.Viewport.Height;
}

ID env::ResourceManager::CreateBufferArray(const std::string& name, int numBuffers, const BufferLayout& layout, void* initialData)
{
	return ID();
}

ID env::ResourceManager::CreateConstantBuffer(const std::string& name, const BufferLayout& layout, void* initialData)
{
	HRESULT hr = S_OK;

	ConstantBuffer bufferDesc;

	bufferDesc.Name = name;
	bufferDesc.State = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	bufferDesc.Layout = layout;

	UINT bufferWidth = (UINT)bufferDesc.Layout.GetByteWidth();

	{ // Create the resource
		D3D12_HEAP_PROPERTIES heapProperties;
		ZeroMemory(&heapProperties, sizeof(heapProperties));
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDescription;
		ZeroMemory(&resourceDescription, sizeof(resourceDescription));
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDescription.Width = (UINT64)bufferWidth;
		resourceDescription.Height = 1;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		hr = GPU::GetDevice()->CreateCommittedResource(&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDescription,
			bufferDesc.State,
			NULL,
			IID_PPV_ARGS(&bufferDesc.Native));

		ASSERT_HR(hr, "Could not create constant buffer");
	}

	ID resourceID = m_commonIDGenerator.GenerateUnique();
	ConstantBuffer* buffer = new ConstantBuffer(std::move(bufferDesc));
	m_constantBuffers[resourceID] = buffer;

	if (initialData)
	{
		UploadBufferData(resourceID, initialData, bufferWidth);
	}

	return resourceID;
}

ID env::ResourceManager::CreateIndexBuffer(const std::string& name, int numIndices, void* initialData)
{
	HRESULT hr = S_OK;

	IndexBuffer bufferDesc;

	bufferDesc.Name = name;
	bufferDesc.State = D3D12_RESOURCE_STATE_INDEX_BUFFER;
	bufferDesc.NumIndices = numIndices;

	UINT bufferWidth = bufferDesc.GetByteWidth();

	{ // Create the resource
		D3D12_HEAP_PROPERTIES heapProperties;
		ZeroMemory(&heapProperties, sizeof(heapProperties));
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDescription;
		ZeroMemory(&resourceDescription, sizeof(resourceDescription));
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDescription.Width = (UINT64)bufferWidth;
		resourceDescription.Height = 1;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		hr = GPU::GetDevice()->CreateCommittedResource(&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDescription,
			bufferDesc.State,
			NULL,
			IID_PPV_ARGS(&bufferDesc.Native));

		ASSERT_HR(hr, "Could not create index buffer");
	}

	ID resourceID = m_commonIDGenerator.GenerateUnique();
	IndexBuffer* buffer = new IndexBuffer(std::move(bufferDesc));
	m_indexBuffers[resourceID] = buffer;

	if (initialData)
	{
		UploadBufferData(resourceID, initialData, bufferWidth);
	}

	return resourceID;
}

ID env::ResourceManager::CreateVertexBuffer(const std::string& name, int numVertices, const BufferLayout& layout, void* initialData)
{
	HRESULT hr = S_OK;

	VertexBuffer bufferDesc;

	bufferDesc.Name = name;
	bufferDesc.State = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	bufferDesc.Layout = layout;

	UINT bufferWidth = (UINT)bufferDesc.Layout.GetByteWidth();

	{ // Create the resource
		D3D12_HEAP_PROPERTIES heapProperties;
		ZeroMemory(&heapProperties, sizeof(heapProperties));
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDescription;
		ZeroMemory(&resourceDescription, sizeof(resourceDescription));
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDescription.Width = (UINT64)bufferWidth;
		resourceDescription.Height = 1;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		hr = GPU::GetDevice()->CreateCommittedResource(&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDescription,
			bufferDesc.State,
			NULL,
			IID_PPV_ARGS(&bufferDesc.Native));

		ASSERT_HR(hr, "Could not create vertex buffer");
	}

	ID resourceID = m_commonIDGenerator.GenerateUnique();
	VertexBuffer* buffer = new VertexBuffer(std::move(bufferDesc));
	m_vertexBuffers[resourceID] = buffer;

	if (initialData)
	{
		UploadBufferData(resourceID, initialData, bufferWidth);
	}

	return resourceID;
}

ID env::ResourceManager::CreateTexture2D(const std::string& name, int width, int height, TextureLayout layout, BindType bindType, void* initialData)
{
	HRESULT hr = S_OK;

	Texture2D textureDesc;

	textureDesc.Name = name;
	textureDesc.State = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Layout = layout;

	{ // Create the resource
		D3D12_HEAP_PROPERTIES heapProperties;
		ZeroMemory(&heapProperties, sizeof(heapProperties));
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDescription;
		ZeroMemory(&resourceDescription, sizeof(resourceDescription));
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		resourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDescription.Width = (UINT64)textureDesc.Width;
		resourceDescription.Height = textureDesc.Height;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.Format = GetDXGIFormat(layout);
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = { 0 };
		UINT numRows = 0;
		GPU::GetDevice()->GetCopyableFootprints(&resourceDescription,
			0,
			1,
			0,
			&footprint,
			&numRows,
			&textureDesc.RowPitch,
			&textureDesc.ByteWidth);

		hr = GPU::GetDevice()->CreateCommittedResource(&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDescription,
			textureDesc.State,
			NULL,
			IID_PPV_ARGS(&textureDesc.Native));

		ASSERT_HR(hr, "Could not create texture 2D buffer");
	}

	ID resourceID = m_commonIDGenerator.GenerateUnique();
	Texture2D* texture = new Texture2D(std::move(textureDesc));
	m_texture2Ds[resourceID] = texture;

	{
		if (any(bindType & BindType::RenderTarget))
			texture->Views.RenderTarget = m_RTVAllocator.Allocate(texture);
		if (any(bindType & BindType::ShaderResource))
			texture->Views.ShaderResource = m_SRVAllocator.Allocate(texture);
		if (any(bindType & BindType::UnorderedAccess))
			texture->Views.UnorderedAccess = m_UAVAllocator.Allocate(texture);
	}

	if (initialData)
	{
		// TODO: UploadTextureData
		//UploadBufferData(resourceID, initialData, bufferWidth);
	}

	return resourceID;
}

ID env::ResourceManager::CreateTexture2DArray(const std::string& name, int numTextures, int width, int height, TextureLayout layout, void* initialData)
{
	return ID();
}

ID env::ResourceManager::CreatePipelineState(const std::string& name, const std::string& shaderPath)
{
	return ID();
}

ID env::ResourceManager::CreateWindowTarget(const std::string& name, Window* window, float startXFactor, float startYFactor, float widthFactor, float heightFactor)
{
	HRESULT hr = S_OK;

	WindowTarget targetDesc;	

	{ // Init base values
		targetDesc.AppWindow = window;

		UINT windowWidth = (UINT)window->GetWidth();
		UINT windiwHeight = (UINT)window->GetHeight();

		targetDesc.Name = name;
		targetDesc.State = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		targetDesc.startXFactor = startXFactor;
		targetDesc.startYFactor = startYFactor;
		targetDesc.startYFactor = startYFactor;
		targetDesc.widthFactor = widthFactor;
		targetDesc.heightFactor = heightFactor;

		// TODO: Adjust these to factors?
		targetDesc.Width = windowWidth;
		targetDesc.Height = windiwHeight;

		AdjustViewportAndScissorRect(targetDesc);
	}

	{ // Create swap chain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.Width = targetDesc.Width;
		swapChainDesc.Height = targetDesc.Height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
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

		hr = factory->CreateSwapChainForHwnd(GPU::GetPresentQueue().GetCommandQueue(),
			window->GetHandle(),
			&swapChainDesc,
			NULL,
			NULL,
			&targetDesc.SwapChain);

		ASSERT_HR(hr, "Could not create swap chain");
	}

	ID resourceID = m_commonIDGenerator.GenerateUnique();
	WindowTarget* target = new WindowTarget(std::move(targetDesc));
	m_windowTargets[resourceID] = target;

	return resourceID;
}

const env::BufferArray* env::ResourceManager::GetBufferArray(ID resourceID)
{
	if (m_buffersArrays.count(resourceID) == 0)
		return nullptr;
	return m_buffersArrays[resourceID];
}

const env::ConstantBuffer* env::ResourceManager::GetConstantBuffer(ID resourceID)
{
	if (m_constantBuffers.count(resourceID) == 0)
		return nullptr;
	return m_constantBuffers[resourceID];
}

const env::IndexBuffer* env::ResourceManager::GetIndexBuffer(ID resourceID)
{
	if (m_indexBuffers.count(resourceID) == 0)
		return nullptr;
	return m_indexBuffers[resourceID];
}

const env::VertexBuffer* env::ResourceManager::GetVertexBuffer(ID resourceID)
{
	if (m_vertexBuffers.count(resourceID) == 0)
		return nullptr;
	return m_vertexBuffers[resourceID];
}

const env::Texture2D* env::ResourceManager::GetTexture2D(ID resourceID)
{
	if (m_texture2Ds.count(resourceID) == 0)
		return nullptr;
	return m_texture2Ds[resourceID];
}

const env::Texture2DArray* env::ResourceManager::GetTexture2DArray(ID resourceID)
{
	if (m_texture2DArrays.count(resourceID) == 0)
		return nullptr;
	return m_texture2DArrays[resourceID];
}

const env::PipelineState* env::ResourceManager::GetPipelineState(ID resourceID)
{
	if (m_pipelineStates.count(resourceID) == 0)
		return nullptr;
	return m_pipelineStates[resourceID];
}

const env::WindowTarget* env::ResourceManager::GetWindowTarget(ID resourceID)
{
	if (m_windowTargets.count(resourceID) == 0)
		return nullptr;
	return m_windowTargets[resourceID];
}

const env::Resource* env::ResourceManager::GetResource(ID resourceID)
{
	return (const env::Resource*)GetResourceNonConst(resourceID);
}

void env::ResourceManager::UploadBufferData(ID resourceID, void* data, UINT numBytes, UINT destinationOffset)
{
	Resource* buffer = GetResourceNonConst(resourceID);
	assert(buffer);

	if (numBytes == 0)
		numBytes = buffer->GetByteWidth();

	{ // Upload data
		void* destination = nullptr;
		D3D12_RANGE readRange = { 0,0 };

		HRESULT hr = S_OK;
		hr = m_uploadBuffer.Native->Map(0, &readRange, &destination);
		ASSERT_HR(hr, "Could not map constant buffer");

		destination = ((char*)destination) + destinationOffset;
		memcpy(destination, data, numBytes);

		m_uploadBuffer.Native->Unmap(0, NULL);
	}

	{ // Copy data to the actual buffer

		CommandQueue& directQueue = GPU::GetDirectQueue();

		D3D12_RESOURCE_STATES initialState = buffer->State;

		{
			m_transitionList->Reset();
			m_transitionList->TransitionResource(buffer, D3D12_RESOURCE_STATE_COPY_DEST);
			m_transitionList->CopyBufferRegion(buffer, 0, &m_uploadBuffer, 0, numBytes);
			m_transitionList->TransitionResource(buffer, initialState);
			m_transitionList->Close();
			directQueue.QueueList(m_transitionList);
			directQueue.Execute();
			directQueue.WaitForIdle();
		}

		//{
			//CommandQueue& copyQueue = GPU::GetCopyQueue();

			//m_transitionList->Reset();
			//m_transitionList->TransitionResource(buffer, D3D12_RESOURCE_STATE_COPY_DEST);
			//m_transitionList->Close();
			//directQueue.QueueList(m_transitionList);
			//directQueue.Execute();
			//directQueue.IncrementFence();

			//m_copyList->Reset();
			//m_copyList->CopyBufferRegion(buffer, 0, &m_uploadBuffer, 0, numBytes);
			//m_copyList->Close();
			//copyQueue.WaitForQueue(&directQueue, directQueue.GetFenceValue());
			//copyQueue.QueueList(m_copyList);
			//copyQueue.Execute();
			//copyQueue.IncrementFence();

			//m_transitionList->Reset();
			//m_transitionList->TransitionResource(buffer, initialState);
			//m_transitionList->Close();
			//directQueue.WaitForQueue(&copyQueue, copyQueue.GetFenceValue());
			//directQueue.QueueList(m_transitionList);
			//directQueue.Execute();
			//directQueue.WaitForIdle();
		//}
	}
}
