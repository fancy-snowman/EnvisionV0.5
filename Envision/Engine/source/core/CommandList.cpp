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

void env::DirectList::SetIndexBuffer(IndexBuffer* buffer)
{
	TransitionResource(buffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);
	m_list->IASetIndexBuffer(&buffer->Views.IndexBuffer);
}

void env::DirectList::SetVertexBuffer(VertexBuffer* buffer, UINT slot)
{
	TransitionResource(buffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	m_list->IASetVertexBuffers(slot, 1, &buffer->Views.VertexBuffer);
}

void env::DirectList::SetPipelineState(PipelineState* state)
{
	m_list->SetPipelineState(state->State);
}

void env::DirectList::Draw(UINT numVertices, UINT vertexOffset)
{
	m_list->DrawInstanced(numVertices, 1, vertexOffset, 0);
}

void env::DirectList::DrawIndexed(UINT numIndices, UINT indexOffset, UINT vertexOffset)
{
	m_list->DrawIndexedInstanced(numIndices, 1, indexOffset, vertexOffset, 0);
}

void env::DirectList::DrawInstanced(UINT numVertices, UINT numInstanes, UINT vertexOffset, UINT instanceOffset)
{
	m_list->DrawInstanced(numVertices, numInstanes, vertexOffset, instanceOffset);
}

void env::DirectList::DrawIndexedInstanced(UINT numIndices, UINT numInstances, UINT indexOffset, INT vertexOffset, UINT instanceOffset)
{
	m_list->DrawIndexedInstanced(numIndices, numInstances, indexOffset, vertexOffset, instanceOffset);
}
