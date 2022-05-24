#pragma once
#include "envision/envpch.h"
#include "envision/resource/Resource.h"

namespace env
{
	class GPU;

	class CommandList
	{
	protected:

		const D3D12_COMMAND_LIST_TYPE m_type;

		ID3D12GraphicsCommandList* m_list;
		ID3D12CommandAllocator* m_allocator;

	private:

		// class GPU is factory for CommandQueue
		friend class env::GPU;

		CommandList(D3D12_COMMAND_LIST_TYPE type);
		void Initialize(ID3D12Device* device);

		CommandList(CommandList&& other) = delete;
		CommandList(const CommandList& other) = delete;
		CommandList& operator=(CommandList&& other) = delete;
		CommandList& operator=(const CommandList& other) = delete;

	public:

		void Reset();
		void TransitionResource(Resource* resource, D3D12_RESOURCE_STATES newState);
	};

	class DirectList : public CommandList
	{
	private:

	private:

		DirectList(DirectList&& other) = delete;
		DirectList(const DirectList& other) = delete;
		DirectList& operator=(DirectList&& other) = delete;
		DirectList& operator=(const DirectList& other) = delete;

	public:

		void SetIndexBuffer(IndexBuffer* buffer, UINT slot);
		void SetVertexBuffer(VertexBuffer* buffer, UINT slot);
		void SetPipelineState(PipelineState* state);

		void Draw(UINT vertexCount, UINT vertexOffset);
		void DrawIndexed(UINT indexCount, UINT indexOffset, UINT vertexOffset);
		void DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount);
		void DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT indexOffset, INT vertexOffset, UINT startOffset);
	};
}