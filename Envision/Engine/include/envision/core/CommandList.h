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

	public:

		void Reset();
		void TransitionResource(Resource* resource, D3D12_RESOURCE_STATES newState);
	};
}