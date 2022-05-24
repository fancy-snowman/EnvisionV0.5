#include "envision/envpch.h"
#include "envision/core/CommandList.h"

void env::CommandList::Initialize(ID3D12Device* device)
{
	HRESULT hr = S_OK;

	hr = device->CreateCommandAllocator(m_type, IID_PPV_ARGS(&m_allocator));
	ASSERT_HR(hr, "Could not create command allocator");

	hr = device->CreateCommandList(NULL, m_type, m_allocator, NULL, IID_PPV_ARGS(&m_list));
	ASSERT_HR(hr, "Could not create command list");
}

void env::CommandList::Reset()
{
	m_allocator->Reset();
	m_list->Reset(m_allocator, NULL);
}

void env::CommandList::TransitionResource(Resource* resource, D3D12_RESOURCE_STATES newState)
{
	if (resource->State != newState)
	{
		D3D12_RESOURCE_BARRIER barrier;
		ZeroMemory(&barrier, sizeof(barrier));
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = resource->Native;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = resource->State;
		barrier.Transition.StateAfter = newState;

		m_list->ResourceBarrier(1, &barrier);
		resource->State = newState;
	}
}
