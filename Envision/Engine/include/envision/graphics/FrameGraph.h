#pragma once
#include "envision/envpch.h"
#include "envision/core/CommandQueue.h"
#include "envision/graphics/FrameResource.h"
#include "envision/graphics/FramePass.h"
#include "envision/graphics/FramePassContext.h"
#include "envision/graphics/FramePassSetupContext.h"

namespace env
{

	class FrameGraph
	{
		static FrameGraph* s_instance;

		FrameGraph(ID3D12Device* device);
		~FrameGraph();

		FrameGraph(const FrameGraph& other) = delete;
		FrameGraph(const FrameGraph&& other) = delete;
		FrameGraph& operator=(const FrameGraph& other) = delete;
		FrameGraph& operator=(const FrameGraph&& other) = delete;

	private:

		ID3D12Device* m_device;

		// Record phase
		std::vector<FramePass*> m_passes;
		std::vector<FrameResource*> m_resources;


		// Execution phase

		// For now, use one list per type
		CopyList* m_copyList;
		ComputeList* m_computeList;
		DirectList* m_directList;

		using ExecutionLayer = std::set<FramePass*>;
		std::list<ExecutionLayer> m_executionLayers;

		FrameDescriptorLinearAllocator* m_resourceDescriptorHeap;
		FrameDescriptorLinearAllocator* m_samplerDescriptorHeap;
		FrameDescriptorLinearAllocator* m_rtvDescriptorHeap;
		FrameDescriptorLinearAllocator* m_dsvDescriptorHeap;

		std::vector<FramePassContext*> m_graphicsPassContexts;
		std::vector<FramePassContext*> m_computePassContexts;

	private:

		void InitDescriptorAllocators();
		void InitCommandLists();

		void FreeDescriptorAllocators();

		void ResetGraph();
		void BuildPassLayers();
		void ExecutePassLayers();

	public:

		static FrameGraph* Initialize(ID3D12Device* device);
		static void Finalize();

		static FrameResource* CreateBuffer(const std::string& name, UINT64 width);
		static FrameResource* CreateTexture2D(const std::string& name, DXGI_FORMAT format, UINT64 width, UINT height);

		static FrameResource* ImportBuffer(const std::string& name, ID3D12Resource* buffer);
		static FrameResource* ImportTexture2D(const std::string& name, ID3D12Resource* texture);

		static FrameResource* BeginFrame(ID3D12Resource* target);
		static void EndFrame();

		template <typename PassData, typename SetupFunc, typename RecordFunc>
		static PassData AddPass(const std::string& name, PASS_TYPE type, SetupFunc setupFunc, RecordFunc deferredRecordFunc);

	private:

		FrameResource* BeginFrameInternal(ID3D12Resource* target);
		void EndFrameInternal();

		FrameResource* CreateBufferInternal(const std::string& name, UINT64 width);
		FrameResource* CreateTexture2DInternal(const std::string& name, DXGI_FORMAT format, UINT64 width, UINT height);

		FrameResource* ImportBufferInternal(const std::string& name, ID3D12Resource* buffer);
		FrameResource* ImportTexture2DInternal(const std::string& name, ID3D12Resource* texture);

		template <typename PassData, typename SetupFunc, typename RecordFunc>
		PassData AddPassInternal(const std::string& name, PASS_TYPE type, SetupFunc setupFunc, RecordFunc deferredRecordFunc);
	};
}

template <typename PassData, typename SetupFunc, typename RecordFunc>
static PassData env::FrameGraph::AddPass(const std::string& name, PASS_TYPE type, SetupFunc setupFunc, RecordFunc deferredRecordFunc)
{
	assert(s_instance && "FrameGraph hasn't been initialized!");
	return s_instance->AddPassInternal<PassData>(name, type, setupFunc, deferredRecordFunc);
}

template <typename PassData, typename SetupFunc, typename RecordFunc>
PassData env::FrameGraph::AddPassInternal(const std::string& name, PASS_TYPE type, SetupFunc setupFunc, RecordFunc deferredRecordFunc)
{
	auto* pass = new TypedFramePass<PassData, RecordFunc>(name, type, deferredRecordFunc);

	FramePassSetupContext setupContext;
	setupFunc(pass->Data, setupContext);

	for (auto sampler : setupContext.Samplers)
		sampler->ReadBy.push_back(pass);
	for (auto resource : setupContext.ReadResources)
		resource->ReadBy.push_back(pass);
	for (auto resource : setupContext.ReadWriteResources)
		resource->WrittenBy.push_back(pass);
	for (auto resource : setupContext.RenderTargets)
		resource->WrittenBy.push_back(pass);
	for (auto resource : setupContext.DepthTargets)
		resource->WrittenBy.push_back(pass);

	pass->Inputs.insert(pass->Inputs.begin(), setupContext.ReadResources.begin(), setupContext.ReadResources.end());
	pass->Outputs.insert(pass->Outputs.begin(), setupContext.ReadWriteResources.begin(), setupContext.ReadWriteResources.end());
	pass->Outputs.insert(pass->Outputs.begin(), setupContext.RenderTargets.begin(), setupContext.RenderTargets.end());
	pass->Outputs.insert(pass->Outputs.begin(), setupContext.DepthTargets.begin(), setupContext.DepthTargets.end());
	pass->Samplers.insert(pass->Samplers.begin(), setupContext.Samplers.begin(), setupContext.Samplers.end());

	m_passes.push_back(pass);
	return pass->Data;
}
