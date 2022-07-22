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

	for (auto& pair : m_buffers) { delete pair.second; }
	m_buffers.clear();

	for (auto& pair : m_texture2Ds) { delete pair.second; }
	m_texture2Ds.clear();

	for (auto& pair : m_texture2DArrays) { delete pair.second; }
	m_texture2DArrays.clear();

	for (auto& pair : m_pipelineStates) { delete pair.second; }
	m_pipelineStates.clear();

	//for (auto& pair : m_windowTargets) { delete pair.second; }
	//m_windowTargets.clear();

	for (auto& pair : m_targets) { delete pair.second; }
	m_targets.clear();
}

env::Resource* env::ResourceManager::GetResourceNonConst(ID resourceID)
{
	if (m_buffersArrays.count(resourceID) > 0)
		return m_buffersArrays[resourceID];
	else if (m_buffers.count(resourceID) > 0)
		return m_buffers[resourceID];
	else if (m_texture2Ds.count(resourceID) > 0)
		return m_texture2Ds[resourceID];
	else if (m_texture2DArrays.count(resourceID) > 0)
		return m_texture2DArrays[resourceID];
	else if (m_pipelineStates.count(resourceID) > 0)
		return m_pipelineStates[resourceID];
	else if (m_targets.count(resourceID) > 0)
		return m_targets[resourceID];
	return nullptr;
}

void env::ResourceManager::AdjustViewportAndScissorRect(WindowTarget& target, const Window& window)
{
	const int windowWidth = window.GetWidth();
	const int windowHeight = window.GetHeight();
	target.Viewport.TopLeftX = windowWidth * target.startXFactor;
	target.Viewport.TopLeftY = windowHeight * target.startYFactor;
	target.Viewport.Width = windowWidth * target.widthFactor;
	target.Viewport.Height = windowHeight * target.heightFactor;
	target.Viewport.MinDepth = 0.0f;
	target.Viewport.MaxDepth = 1.0f;

	target.ScissorRect.left = (LONG)target.Viewport.TopLeftX;
	target.ScissorRect.top = (LONG)target.Viewport.TopLeftY;
	target.ScissorRect.right = (LONG)target.Viewport.TopLeftX + (LONG)target.Viewport.Width;
	target.ScissorRect.bottom = (LONG)target.Viewport.TopLeftY + (LONG)target.Viewport.Height;
}

D3D12_CPU_DESCRIPTOR_HANDLE env::ResourceManager::CreateCBV(Resource* resource)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_CBVAllocator.Allocate().CPUHandle;

	D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferLocation = resource->Native->GetGPUVirtualAddress();
	desc.SizeInBytes = resource->GetByteWidth();

	GPU::GetDevice()->CreateConstantBufferView(&desc, handle);

	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE env::ResourceManager::CreateSRV(Resource* resource)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_SRVAllocator.Allocate().CPUHandle;

	D3D12_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	ResourceType type = resource->GetType();
	switch (type)
	{
	case ResourceType::Buffer:
	case ResourceType::IndexBuffer:
	case ResourceType::VertexBuffer:
	{
		// Are these necessary? A constant buffer will never
		// be bound as a shader resource view
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = 1;
		desc.Buffer.StructureByteStride = resource->GetByteWidth();
		break;
	}

	case ResourceType::Texture2D:
	{
		Texture2D* texture = (Texture2D*)resource;
		desc.Format = texture->Format;
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = 0;
		desc.Texture2D.MipLevels = 1;
		desc.Texture2D.PlaneSlice = 0;
		desc.Texture2D.ResourceMinLODClamp = 0.0f; // Correct?
		break;
	}

	case ResourceType::BufferArray:
	{
		BufferArray* buffer = (BufferArray*)resource;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = buffer->Layout.GetNumRepetitions();
		desc.Buffer.StructureByteStride = buffer->Layout.GetByteWidth();
		break;
	}

	case ResourceType::Texture2DArray:
	{
		Texture2DArray* textureArray = (Texture2DArray*)resource;
		desc.Format = textureArray->Format;
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MostDetailedMip = 0;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.ArraySize = textureArray->NumTextures;
		desc.Texture2DArray.PlaneSlice = 0;
		desc.Texture2DArray.ResourceMinLODClamp = 0.0f; // Correct?
		break;
	}
	
	default:
		return D3D12_CPU_DESCRIPTOR_HANDLE(); // TODO: handle error
	}

	GPU::GetDevice()->CreateShaderResourceView(resource->Native,
		&desc,
		handle);

	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE env::ResourceManager::CreateUAV(Resource* resource)
{

	//GPU::GetDevice()->CreateUnorderedAccessView()
	return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_CPU_DESCRIPTOR_HANDLE env::ResourceManager::CreateSampler(Resource* resource)
{
	return D3D12_CPU_DESCRIPTOR_HANDLE();
}

D3D12_CPU_DESCRIPTOR_HANDLE env::ResourceManager::CreateRTV(Resource* resource)
{
	{
		bool isTexture2D = (resource->GetType() == ResourceType::Texture2D);
		assert(isTexture2D);
		// There exist RTV for buffers, may need to add this later?
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_RTVAllocator.Allocate().CPUHandle;

	D3D12_RENDER_TARGET_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	

	ResourceType type = resource->GetType();
	switch (type)
	{
	case ResourceType::Texture2D:
	{
		Texture2D* texture = (Texture2D*)resource;
		desc.Format = texture->Format;
		desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;
		desc.Texture2D.PlaneSlice = 0;
		break;
	}

	default:
		return D3D12_CPU_DESCRIPTOR_HANDLE(); // TODO: handle error
	}

	GPU::GetDevice()->CreateRenderTargetView(resource->Native,
		&desc,
		handle);

	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE env::ResourceManager::CreateDSV(Resource* resource)
{
	{
		bool isTexture2D = (resource->GetType() == ResourceType::Texture2D);
		assert(isTexture2D);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_DSVAllocator.Allocate().CPUHandle;

	D3D12_DEPTH_STENCIL_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	ResourceType type = resource->GetType();
	switch (type)
	{
	case ResourceType::Texture2D:
	{
		Texture2D* texture = (Texture2D*)resource;
		desc.Format = texture->Format;
		desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		break;
	}

	default:
		return D3D12_CPU_DESCRIPTOR_HANDLE(); // TODO: handle error
	}

	GPU::GetDevice()->CreateDepthStencilView(resource->Native,
		&desc,
		handle);

	return handle;
}

ID env::ResourceManager::CreateBufferArray(const std::string& name, const BufferLayout& layout, BufferBindType bindType, void* initialData)
{
	HRESULT hr = S_OK;

	bool isShaderResource = any(bindType & BufferBindType::ShaderResource) || (bindType == BufferBindType::Unknown);
	bool isUnorderedAccess = any(bindType & BufferBindType::UnorderedAccess) || (bindType == BufferBindType::Unknown);

	BufferArray bufferDesc;

	bufferDesc.Name = name;
	bufferDesc.State = D3D12_RESOURCE_STATE_COMMON;
	bufferDesc.Layout = layout;

	UINT elementStride = bufferDesc.Layout.GetByteWidth();
	UINT numElements = bufferDesc.Layout.GetNumRepetitions();

	UINT bufferWidth = elementStride * numElements;

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

		ASSERT_HR(hr, "Could not create buffer array");
	}

	{ // Create views
		if (isShaderResource) {
			bufferDesc.Views.ShaderResource = CreateSRV(&bufferDesc);
		}
		if (isUnorderedAccess) {
			bufferDesc.Views.UnorderedAccess = CreateUAV(&bufferDesc);
		}
	}

	ID resourceID = m_commonIDGenerator.GenerateUnique();
	BufferArray* buffer = new BufferArray(std::move(bufferDesc));
	m_buffersArrays[resourceID] = buffer;

	if (initialData) {
		UploadBufferData(resourceID, initialData, bufferWidth);
	}

	return resourceID;
}

ID env::ResourceManager::CreateBuffer(const std::string& name, const BufferLayout& layout, BufferBindType bindType, void* initialData)
{
	HRESULT hr = S_OK;

	bool isConstantBuffer = any(bindType & BufferBindType::Constant) || (bindType == BufferBindType::Unknown);
	bool isIndexBuffer = any(bindType & BufferBindType::Index) || (bindType == BufferBindType::Unknown);
	bool isVertexBuffer = any(bindType & BufferBindType::Vertex) || (bindType == BufferBindType::Unknown);
	bool isUnorderedAccess = any(bindType & BufferBindType::UnorderedAccess) || (bindType == BufferBindType::Unknown);

	Buffer bufferDesc;

	bufferDesc.Name = name;
	bufferDesc.State = D3D12_RESOURCE_STATE_COMMON;
	bufferDesc.Layout = layout;

	UINT bufferWidth = (UINT)bufferDesc.Layout.GetByteWidth() * bufferDesc.Layout.GetNumRepetitions();

	if (isConstantBuffer) {
		bufferWidth = (bufferWidth + 255) & ~255;
	}

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

		ASSERT_HR(hr, "Could not create buffer");
	}

	{ // Create views
		if (isConstantBuffer) {
			bufferDesc.Views.Constant = CreateCBV(&bufferDesc);
		}
		if (isIndexBuffer) {
			bufferDesc.Views.Index.BufferLocation = bufferDesc.Native->GetGPUVirtualAddress();
			bufferDesc.Views.Index.SizeInBytes = bufferWidth;
			bufferDesc.Views.Index.Format = layout.GetDXGIFormat();
		}
		if (isVertexBuffer) {
			bufferDesc.Views.Vertex.BufferLocation = bufferDesc.Native->GetGPUVirtualAddress();
			bufferDesc.Views.Vertex.SizeInBytes = bufferWidth;
			bufferDesc.Views.Vertex.StrideInBytes = layout.GetByteWidth();
		}
		if (isUnorderedAccess) {
			bufferDesc.Views.UnorderedAccess = CreateUAV(&bufferDesc);
		}
	}

	ID resourceID = m_commonIDGenerator.GenerateUnique();
	Buffer* buffer = new Buffer(std::move(bufferDesc));
	m_buffers[resourceID] = buffer;

	if (initialData) {
		UploadBufferData(resourceID, initialData, bufferWidth);
	}

	return resourceID;
}

ID env::ResourceManager::CreateTexture2D(const std::string& name, int width, int height, DXGI_FORMAT format, TextureBindType bindType, void* initialData)
{
	HRESULT hr = S_OK;

	Texture2D textureDesc;

	textureDesc.Name = name;
	textureDesc.State = D3D12_RESOURCE_STATE_COMMON;

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Format = format;

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
		resourceDescription.Format = format;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

		bool isRenderTarget = any(bindType & TextureBindType::RenderTarget) || (bindType == TextureBindType::Unknown);
		bool isShaderResource = any(bindType & TextureBindType::ShaderResource) || (bindType == TextureBindType::Unknown);
		bool isUnorderedAccess = any(bindType & TextureBindType::UnorderedAccess) || (bindType == TextureBindType::Unknown);
		bool isDepthStencil = any(bindType & TextureBindType::DepthStencil);

		if (isRenderTarget)
			resourceDescription.Flags = resourceDescription.Flags | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		//if (!any(bindType & BindType::ShaderResource)) // requires D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL or D3D12_RESOURCE_FLAG_VIDEO_DECODE_REFERENCE_ONLY
		//	resourceDescription.Flags = resourceDescription.Flags | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
		if (isUnorderedAccess)
			resourceDescription.Flags = resourceDescription.Flags | D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		if (isDepthStencil) {
			resourceDescription.Flags = resourceDescription.Flags | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			textureDesc.State = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		}

		D3D12_CLEAR_VALUE clearValue = {};
		D3D12_CLEAR_VALUE* clearValuePtr = nullptr;

		if (isDepthStencil) {
			clearValue.Format = resourceDescription.Format;
			clearValue.DepthStencil.Depth = 1.0f;
			clearValue.DepthStencil.Stencil = 0;
			clearValuePtr = &clearValue;
		}

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
			clearValuePtr,
			IID_PPV_ARGS(&textureDesc.Native));

		ASSERT_HR(hr, "Could not create texture 2D buffer");
	}

	{
		if (any(bindType & TextureBindType::RenderTarget))
			textureDesc.Views.RenderTarget = CreateRTV(&textureDesc);
		if (any(bindType & TextureBindType::ShaderResource))
			textureDesc.Views.ShaderResource = CreateSRV(&textureDesc);
		if (any(bindType & TextureBindType::UnorderedAccess))
			textureDesc.Views.UnorderedAccess = CreateUAV(&textureDesc);
		if (any(bindType & TextureBindType::DepthStencil)) {
			textureDesc.Views.DepthStencil = CreateDSV(&textureDesc);
		}
	}

	ID resourceID = m_commonIDGenerator.GenerateUnique();
	Texture2D* texture = new Texture2D(std::move(textureDesc));
	m_texture2Ds[resourceID] = texture;


	if (initialData)
	{
		// TODO: UploadTextureData
		//UploadBufferData(resourceID, initialData, bufferWidth);
	}

	return resourceID;
}

ID env::ResourceManager::CreateTexture2D(const std::string& name, TextureBindType bindType, ID3D12Resource* existingTexture)
{
	D3D12_RESOURCE_DESC existingDesc = existingTexture->GetDesc();
	Texture2D textureDesc;

	bool isRenderTarget = any(bindType & TextureBindType::RenderTarget) || (bindType == TextureBindType::Unknown);
	bool isShaderResource = any(bindType & TextureBindType::ShaderResource) || (bindType == TextureBindType::Unknown);
	bool isUnorderedAccess = any(bindType & TextureBindType::UnorderedAccess) || (bindType == TextureBindType::Unknown);
	bool isDepthStencil = any(bindType & TextureBindType::DepthStencil);

	{
		textureDesc.Name = name;
		textureDesc.Native = existingTexture;

		if (isRenderTarget)
			textureDesc.State = D3D12_RESOURCE_STATE_RENDER_TARGET;
		else if (isShaderResource)
			textureDesc.State = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		else if (isUnorderedAccess)
			textureDesc.State = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		else if (isDepthStencil)
			textureDesc.State = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		else
			textureDesc.State = D3D12_RESOURCE_STATE_COMMON;

		textureDesc.Width = (int)existingDesc.Width;
		textureDesc.Height = (int)existingDesc.Height;
		textureDesc.Format = existingDesc.Format;

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
		UINT numRows;
		GPU::GetDevice()->GetCopyableFootprints(&existingDesc,
			0, 1, 0,
			&footprint,
			&numRows,
			&textureDesc.RowPitch,
			&textureDesc.ByteWidth);
	}

	{
		if (any(bindType & TextureBindType::RenderTarget))
			textureDesc.Views.RenderTarget = CreateRTV(&textureDesc);
		if (any(bindType & TextureBindType::ShaderResource))
			textureDesc.Views.ShaderResource = CreateSRV(&textureDesc);
		if (any(bindType & TextureBindType::UnorderedAccess))
			textureDesc.Views.UnorderedAccess = CreateUAV(&textureDesc);
	}

	ID resourceID = m_commonIDGenerator.GenerateUnique();
	Texture2D* texture = new Texture2D(std::move(textureDesc));
	m_texture2Ds[resourceID] = texture;

	return resourceID;
}

ID env::ResourceManager::CreateTexture2DArray(const std::string& name, int numTextures, int width, int height, DXGI_FORMAT format, void* initialData)
{
	return ID();
}

ID env::ResourceManager::CreatePipelineState(const std::string& name, std::initializer_list<ShaderDesc> shaderDescs, bool useInputLayout, const RootSignature& rootSignature)
{
	// Sanity check that there's at most one desc per shader stage
	ShaderStage stages = ShaderStage::Unknown;
	for (auto& s : shaderDescs) {
		if (any(stages & s.Stage))
			return ID_ERROR;
		stages = stages | s.Stage;
	}

	HRESULT hr = S_OK;

	PipelineState resourceDesc;

	std::unordered_map<ShaderStage, ID3DBlob*> shaders;
	ID3DBlob* errorBlob;

	{ // Compile shaders

		for (auto& s : shaderDescs)
		{
			std::string targetModel = GetTargetModelString(s.Stage, s.Model);
			std::wstring path(s.Path.begin(), s.Path.end());

			ID3DBlob* blob = nullptr;
			hr = D3DCompileFromFile(path.c_str(),
				NULL,
				NULL,
				s.EntryPoint.c_str(),
				targetModel.c_str(),
				NULL,
				NULL,
				&blob,
				&errorBlob);

			if (FAILED(hr))
			{
				if (errorBlob)
				{
					OutputDebugStringA((LPCSTR)errorBlob->GetBufferPointer());
					errorBlob->Release();
				}
				ASSERT_HR(hr, "Failed to compile shader");
			}

			shaders[s.Stage] = blob;
		}
	}

	{
		ID3DBlob* rootSignatureBlob = nullptr;

		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		ZeroMemory(&rootSignatureDesc, sizeof(rootSignatureDesc));
		rootSignatureDesc.NumParameters = rootSignature.GetNumParameters();
		rootSignatureDesc.pParameters = rootSignature.GetParameterArrayStart();

		if (useInputLayout)
			rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		hr = D3D12SerializeRootSignature(&rootSignatureDesc,
			D3D_ROOT_SIGNATURE_VERSION_1_0,
			&rootSignatureBlob,
			&errorBlob);

		if (FAILED(hr)) {
			if (errorBlob) {
				OutputDebugStringA((LPCSTR)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}
			ASSERT_HR(hr, "Failed to resialize root signature");
		}

		hr = GPU::GetDevice()->CreateRootSignature(NULL,
			rootSignatureBlob->GetBufferPointer(),
			rootSignatureBlob->GetBufferSize(),
			IID_PPV_ARGS(&resourceDesc.RootSignature));

		ASSERT_HR(hr, "Could not create root signature");

		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc;
		ZeroMemory(&pipelineDesc, sizeof(pipelineDesc));
		pipelineDesc.pRootSignature = resourceDesc.RootSignature;
		pipelineDesc.SampleMask = UINT_MAX;
		pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
		pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineDesc.NumRenderTargets = 1;
		pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipelineDesc.SampleDesc.Count = 1;

		pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineDesc.DepthStencilState.DepthEnable = true;
		pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		pipelineDesc.DepthStencilState.StencilEnable = false;
		pipelineDesc.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		pipelineDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		pipelineDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pipelineDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		pipelineDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pipelineDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		pipelineDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		pipelineDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		pipelineDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		pipelineDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		D3D12_RENDER_TARGET_BLEND_DESC defaultBlendDesc = {
			false, false,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL
		};
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
		{
			pipelineDesc.BlendState.RenderTarget[i] = defaultBlendDesc;
		}

		// Set required shaders
		pipelineDesc.VS.pShaderBytecode = shaders[ShaderStage::Vertex]->GetBufferPointer();
		pipelineDesc.VS.BytecodeLength = shaders[ShaderStage::Vertex]->GetBufferSize();
		pipelineDesc.PS.pShaderBytecode = shaders[ShaderStage::Pixel]->GetBufferPointer();
		pipelineDesc.PS.BytecodeLength = shaders[ShaderStage::Pixel]->GetBufferSize();

		// Set optional shaders
		if (any(stages & ShaderStage::Domain)) {
			pipelineDesc.DS.pShaderBytecode = shaders[ShaderStage::Domain]->GetBufferPointer();
			pipelineDesc.DS.BytecodeLength = shaders[ShaderStage::Domain]->GetBufferSize();
		}
		if (any(stages & ShaderStage::Hull)) {
			pipelineDesc.HS.pShaderBytecode = shaders[ShaderStage::Hull]->GetBufferPointer();
			pipelineDesc.HS.BytecodeLength = shaders[ShaderStage::Hull]->GetBufferSize();
		}
		if (any(stages & ShaderStage::Geometry)) {
			pipelineDesc.GS.pShaderBytecode = shaders[ShaderStage::Geometry]->GetBufferPointer();
			pipelineDesc.GS.BytecodeLength = shaders[ShaderStage::Geometry]->GetBufferSize();
		}

		
		std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
		pipelineDesc.InputLayout.NumElements = (UINT)inputLayout.size();
		pipelineDesc.InputLayout.pInputElementDescs = inputLayout.data();

		hr = GPU::GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&resourceDesc.State));
		ASSERT_HR(hr, "Could not create pipeline state");
	}

	ID resourceID = m_commonIDGenerator.GenerateUnique();
	PipelineState* pipeline = new PipelineState(std::move(resourceDesc));
	m_pipelineStates[resourceID] = pipeline;

	return resourceID;
}

ID env::ResourceManager::CreateWindowTarget(const std::string& name, Window* window, float startXFactor, float startYFactor, float widthFactor, float heightFactor)
{
	WindowTarget targetDesc;

	targetDesc.Name = name;
	targetDesc.State = D3D12_RESOURCE_STATE_PRESENT;
	targetDesc.startXFactor = startXFactor;
	targetDesc.startYFactor = startYFactor;
	targetDesc.startYFactor = startYFactor;
	targetDesc.widthFactor = widthFactor;
	targetDesc.heightFactor = heightFactor;
	targetDesc.AppWindow = window;

	AdjustViewportAndScissorRect(targetDesc, *window);

	ID resourceID = m_commonIDGenerator.GenerateUnique();
	WindowTarget* target = new WindowTarget(std::move(targetDesc));
	m_targets[resourceID] = target;

	window->PushTarget(target);

	return resourceID;
}

env::BufferArray* env::ResourceManager::GetBufferArray(ID resourceID)
{
	if (m_buffersArrays.count(resourceID) == 0)
		return nullptr;
	return m_buffersArrays[resourceID];
}

env::Buffer* env::ResourceManager::GetBuffer(ID resourceID)
{
	if (m_buffers.count(resourceID) == 0)
		return nullptr;
	return m_buffers[resourceID];
}

env::Texture2D* env::ResourceManager::GetTexture2D(ID resourceID)
{
	if (m_texture2Ds.count(resourceID) == 0)
		return nullptr;
	return m_texture2Ds[resourceID];
}

env::Texture2DArray* env::ResourceManager::GetTexture2DArray(ID resourceID)
{
	if (m_texture2DArrays.count(resourceID) == 0)
		return nullptr;
	return m_texture2DArrays[resourceID];
}

env::PipelineState* env::ResourceManager::GetPipelineState(ID resourceID)
{
	if (m_pipelineStates.count(resourceID) == 0)
		return nullptr;
	return m_pipelineStates[resourceID];
}

//env::WindowTarget* env::ResourceManager::GetWindowTarget(ID resourceID)
//{
//	if (m_windowTargets.count(resourceID) == 0)
//		return nullptr;
//	return m_windowTargets[resourceID];
//}

env::WindowTarget* env::ResourceManager::GetTarget(ID resourceID)
{
	if (m_targets.count(resourceID) == 0)
		return nullptr;
	return m_targets[resourceID];
}

env::Resource* env::ResourceManager::GetResource(ID resourceID)
{
	return (env::Resource*)GetResourceNonConst(resourceID);
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
	}
}
