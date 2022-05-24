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
	m_commonIDGenerator(commonIDGenerator)
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

ID env::ResourceManager::CreateBufferArray(const std::string& name, int numBuffers, const BufferLayout& layout, void* initialData)
{
	return ID();
}

ID env::ResourceManager::CreateConstantBuffer(const std::string& name, const BufferLayout& layout, void* initialData)
{
	HRESULT hr = S_OK;

	ConstantBuffer buffer;

	buffer.Name = name;
	buffer.State = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	buffer.Layout = layout;

	{
		D3D12_HEAP_PROPERTIES heapProperties;
		ZeroMemory(&heapProperties, sizeof(heapProperties));
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProperties.CreationNodeMask = 1;
		heapProperties.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC resourceDescription;
		ZeroMemory(&resourceDescription, sizeof(resourceDescription));
		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDescription.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
		resourceDescription.Width = (UINT64)layout.GetByteWidth();
		resourceDescription.Height = 1;
		resourceDescription.DepthOrArraySize = 1;
		resourceDescription.MipLevels = 1;
		resourceDescription.Format = DXGI_FORMAT_UNKNOWN;
		resourceDescription.SampleDesc.Count = 1;
		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		hr = GPU::GetDevice()->CreateCommittedResource(&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDescription,
			buffer.State,
			NULL,
			IID_PPV_ARGS(&buffer.Native));

		ASSERT_HR(hr, "Could not create constant buffer");
	}

	{

	}

	ID resourceID = m_commonIDGenerator.GenerateUnique();
	m_constantBuffers[resourceID] = new ConstantBuffer(std::move(buffer));

	return resourceID;
}

ID env::ResourceManager::CreateIndexBuffer(const std::string& name, int numIndices, void* initialData)
{
	return ID();
}

ID env::ResourceManager::CreateVertexBuffer(const std::string& name, int numVertices, const BufferLayout& layout, void* initialData)
{
	return ID();
}

ID env::ResourceManager::CreateTexture2D(const std::string& name, int width, int height, TextureLayout layout, void* initialData)
{
	return ID();
}

ID env::ResourceManager::CreateTexture2DArray(const std::string& name, int numTextures, int width, int height, TextureLayout layout, void* initialData)
{
	return ID();
}

ID env::ResourceManager::CreatePipelineState(const std::string& name, const std::string& shaderPath)
{
	return ID();
}

ID env::ResourceManager::CreateWindowTarget(const std::string& name, HWND window, float startXFactor, float startYFactor, float widthFactor, float heightFactor)
{
	return ID();
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
