#include "envision/envpch.h"
#include "envision/graphics/FrameGraph.h"
#include "envision/graphics/FramePassRecordContext.h"
#include "envision/core/GPU.h"

env::FrameGraph* env::FrameGraph::s_instance = nullptr;

env::FrameGraph::FrameGraph(ID3D12Device* device) : m_device(device)
{
	InitDescriptorAllocators();
}

env::FrameGraph::~FrameGraph()
{
	FreeDescriptorAllocators();
}

void env::FrameGraph::InitDescriptorAllocators()
{
	m_resourceDescriptorHeap = new FrameDescriptorLinearAllocator(m_device,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	m_samplerDescriptorHeap = new FrameDescriptorLinearAllocator(m_device,
		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	m_rtvDescriptorHeap = new FrameDescriptorLinearAllocator(m_device,
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDescriptorHeap = new FrameDescriptorLinearAllocator(m_device,
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void env::FrameGraph::InitCommandLists()
{
	m_copyList = env::GPU::Get()->CreateCopyCommandList("FrameGraphCopyList");
	m_computeList = env::GPU::Get()->CreateComputeCommandList("FrameGraphCopyList");
	m_directList = env::GPU::Get()->CreateDirectCommandList("FrameGraphCopyList");
}

void env::FrameGraph::FreeDescriptorAllocators()
{
	delete m_resourceDescriptorHeap;
	delete m_samplerDescriptorHeap;
	delete m_rtvDescriptorHeap;
	delete m_dsvDescriptorHeap;
}

void env::FrameGraph::ResetGraph()
{
	for (auto& resource : m_resources)
		if (!resource->IsImported())
			delete resource;
	m_resources.clear();

	for (auto& pass : m_passes)
		delete pass;
	m_passes.clear();

	m_executionLayers.clear();
}


void env::FrameGraph::BuildPassLayers()
{
	std::vector<FrameResource*> dependencies;
	dependencies.push_back(m_resources.front()); // First resoruce is always the final target

	while (!dependencies.empty())
	{
		ExecutionLayer layer;

		for (auto& resource : dependencies)
		{
			for (auto& pass : resource->WrittenBy)
			{
				layer.insert(pass);
				// Erase pass in m_passes?
			}
		}
		dependencies.clear();

		for (auto& pass : layer)
			dependencies.insert(dependencies.end(), pass->Inputs.begin(), pass->Inputs.end());
		if (layer.size() > 0)
			m_executionLayers.push_front(std::move(layer));
	}
}

void env::FrameGraph::ExecutePassLayers()
{
	for (auto& layer : m_executionLayers)
	{
		for (auto& pass : layer)
		{
			FramePassRecordContext recordContext = {};
			recordContext.ResourceViewAllocator = m_resourceDescriptorHeap;
			recordContext.RenderTargetViewAllocator = m_rtvDescriptorHeap;
			recordContext.DepthStencilViewAllocator = m_dsvDescriptorHeap;
			recordContext.SamplerViewAllocator = m_samplerDescriptorHeap;

			recordContext.CommandList = nullptr;
			if (pass->Type == PASS_TYPE::COPY)
				recordContext.CommandList = m_copyList;
			else if (pass->Type == PASS_TYPE::COMPUTE)
				recordContext.CommandList = m_computeList;
			else if (pass->Type == PASS_TYPE::GRAPHICS)
				recordContext.CommandList = m_directList;

			pass->Record(recordContext);
		}
	}
}

env::FrameGraph* env::FrameGraph::Initialize(ID3D12Device* device)
{
	assert(!s_instance && "Initialize should not be called twice before Finalize!");
	if (!s_instance)
		s_instance = new FrameGraph(device);
	return s_instance;
}

void env::FrameGraph::Finalize()
{
	assert(s_instance && "Can't finalize an unitialized FrameGraph!");
	delete s_instance;
	s_instance = nullptr;
}

env::FrameResource* env::FrameGraph::CreateBuffer(const std::string& name, UINT64 width)
{
	assert(s_instance && "FrameGraph hasn't been initialized!");
	return s_instance->CreateBufferInternal(name, width);
}

env::FrameResource* env::FrameGraph::CreateTexture2D(const std::string& name, DXGI_FORMAT format, UINT64 width, UINT height)
{
	assert(s_instance && "FrameGraph hasn't been initialized!");
	return s_instance->CreateTexture2DInternal(name, format, width, height);
}

env::FrameResource* env::FrameGraph::ImportBuffer(const std::string& name, ID3D12Resource* resource)
{
	assert(s_instance && "FrameGraph hasn't been initialized!");
	return s_instance->ImportBufferInternal(name, resource);
}

env::FrameResource* env::FrameGraph::ImportTexture2D(const std::string& name, ID3D12Resource* resource)
{
	assert(s_instance && "FrameGraph hasn't been initialized!");
	return s_instance->ImportTexture2DInternal(name, resource);
}

env::FrameResource* env::FrameGraph::BeginFrame(ID3D12Resource* target)
{
	assert(s_instance && "FrameGraph hasn't been initialized!");
	return s_instance->BeginFrameInternal(target);
}

void env::FrameGraph::EndFrame()
{
	assert(s_instance && "FrameGraph hasn't been initialized!");
	s_instance->EndFrameInternal();
}

env::FrameResource* env::FrameGraph::BeginFrameInternal(ID3D12Resource* target)
{
	ResetGraph();
	FrameResource* targetResource = ImportTexture2DInternal("GraphTarget", target);


	return targetResource;
}

void env::FrameGraph::EndFrameInternal()
{
	BuildPassLayers();
	ExecutePassLayers();
}

env::FrameResource* env::FrameGraph::CreateBufferInternal(const std::string& name, UINT64 width)
{
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	desc.Width = width;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	FrameResource* resource = new FrameResource(name, desc);
	m_resources.push_back(resource);
	return resource;
}

env::FrameResource* env::FrameGraph::CreateTexture2DInternal(const std::string& name, DXGI_FORMAT format, UINT64 width, UINT height)
{
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE; // Will be filled out before the native resource is created/placed

	FrameResource* resource = new FrameResource(name, desc);
	m_resources.push_back(resource);
	return resource;
}

env::FrameResource* env::FrameGraph::ImportBufferInternal(const std::string& name, ID3D12Resource* buffer)
{
	FrameResource* resource = new FrameResource(name, buffer);
	m_resources.push_back(resource);
	return resource;
}

env::FrameResource* env::FrameGraph::ImportTexture2DInternal(const std::string& name, ID3D12Resource* texture)
{
	FrameResource* resource = new FrameResource(name, texture);
	m_resources.push_back(resource);
	return resource;
}
