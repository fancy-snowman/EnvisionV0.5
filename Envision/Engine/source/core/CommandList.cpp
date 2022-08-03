#include "envision/envpch.h"
#include "envision/core/CommandList.h"

env::CommandList::CommandList(D3D12_COMMAND_LIST_TYPE type) :
	m_state(ListState::Unknown),
	m_type(type),
	m_list(nullptr),
	m_allocator(nullptr)
{
	//
}

void env::CommandList::Initialize(ID3D12Device* device)
{
	HRESULT hr = S_OK;

	hr = device->CreateCommandAllocator(m_type, IID_PPV_ARGS(&m_allocator));
	ASSERT_HR(hr, "Could not create command allocator");

	hr = device->CreateCommandList(NULL, m_type, m_allocator, NULL, IID_PPV_ARGS(&m_list));
	ASSERT_HR(hr, "Could not create command list");

	m_state = ListState::Recording;
}

env::CommandList::~CommandList()
{
	m_list->Release();
	m_allocator->Release();
}

D3D12_COMMAND_LIST_TYPE env::CommandList::GetType()
{
	return m_type;
}

void env::CommandList::Reset()
{
	m_allocator->Reset();
	m_list->Reset(m_allocator, NULL);
	m_state = ListState::Recording;
	ResetInherited();
}

void env::CommandList::Close()
{
	m_list->Close();
	m_state = ListState::Closed;
}

env::ListState env::CommandList::GetState()
{
	return m_state;
}

ID3D12GraphicsCommandList* env::CommandList::GetNative()
{
	return m_list;
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

void env::CommandList::SetDescriptorHeaps(UINT numHeaps, ID3D12DescriptorHeap* const* heaps)
{
	m_list->SetDescriptorHeaps(numHeaps, heaps);
}



// ######################################################################### //
// ############################### COPY LIST ############################### //
// ######################################################################### //

env::CopyList::CopyList(D3D12_COMMAND_LIST_TYPE type) : env::CommandList(type)
{
	//
}

env::CopyList::CopyList() : env::CommandList(D3D12_COMMAND_LIST_TYPE_COPY)
{
	//
}

env::CopyList::~CopyList()
{
	//
}

void env::CopyList::CopyBufferRegion(Resource* dest, UINT64 destOffset, Resource* src, UINT64 srcOffset, UINT64 numBytes)
{
	m_list->CopyBufferRegion(dest->Native,
		destOffset,
		src->Native,
		srcOffset,
		numBytes);
}

void env::CopyList::CopyResource(Resource* dest, Resource* src)
{
	m_list->CopyResource(dest->Native, src->Native);
}

void env::CopyList::UploadBufferData(Buffer* buffer, const void* data, UINT numBytes, UINT destinationOffset)
{
	assert(buffer);
	assert(data);
	assert(any(buffer->BindType & BufferBindType::Upload));
	if (numBytes == 0)
		numBytes = buffer->GetByteWidth();

	// Upload buffers require D3D12_RESOURCE_STATE_GENERIC_READ, which
	// can not be changed
	//TransitionResource(buffer, D3D12_RESOURCE_STATE_COPY_DEST);

	void* destination = nullptr;
	D3D12_RANGE readRange = { 0, 0 };

	HRESULT hr = buffer->Native->Map(0, &readRange, &destination);
	ASSERT_HR(hr, "Could no map buffer for upload");
	destination = ((char*)destination) + destinationOffset;
	memcpy(destination, data, numBytes);
	buffer->Native->Unmap(0, NULL);
}



// ######################################################################### //
// ############################## COMPUTE LIST ############################# //
// ######################################################################### //

env::ComputeList::ComputeList(D3D12_COMMAND_LIST_TYPE type) : env::CopyList(type)
{
	//
}

env::ComputeList::ComputeList() : env::CopyList(D3D12_COMMAND_LIST_TYPE_COMPUTE)
{
	//
}

env::ComputeList::~ComputeList()
{
	//
}

void env::ComputeList::Dispatch(UINT numThreadGroupsX, UINT numThreadGroupsY, UINT numThreadGroupsZ)
{
	m_list->Dispatch(numThreadGroupsX, numThreadGroupsY, numThreadGroupsZ);
}



// ######################################################################### //
// ############################## DIRECT LIST ############################## //
// ######################################################################### //

env::DirectList::DirectList() : env::ComputeList(D3D12_COMMAND_LIST_TYPE_DIRECT)
{
	//
}

void env::DirectList::ResetInherited()
{
	m_state.IndexBuffer = nullptr;

	for (int i = 0; i < 8; i++)
		m_state.VertexBuffers[i] = nullptr;
}

env::DirectList::~DirectList()
{
	//
}

void env::DirectList::ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE target, float red, float green, float blue, float alpha)
{
	const FLOAT clearColor[] = {red, green, blue, alpha};
	m_list->ClearRenderTargetView(target, clearColor, 0, NULL);
}

void env::DirectList::ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE target, const Float4& color)
{
	m_list->ClearRenderTargetView(target, (const FLOAT*)&color, 0, NULL);
}

void env::DirectList::ClearDepthStencil(Texture2D* stencil, bool clearDepth, bool clearStencil, FLOAT depthValue, UINT8 stencilValue)
{
	TransitionResource(stencil, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	UINT flags = 0;
	if (clearDepth) flags |= (UINT)D3D12_CLEAR_FLAG_DEPTH;
	if (clearStencil) flags |= (UINT)D3D12_CLEAR_FLAG_STENCIL;
	m_list->ClearDepthStencilView(stencil->Views.DepthStencil, (D3D12_CLEAR_FLAGS)flags, depthValue, stencilValue, 0, nullptr);
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

void env::DirectList::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology)
{
	m_list->IASetPrimitiveTopology(topology);
}

void env::DirectList::SetPipelineState(PipelineState* state)
{
	m_list->SetGraphicsRootSignature(state->RootSignature);
	m_list->SetPipelineState(state->State);
}

void env::DirectList::SetTarget(WindowTarget* target, Texture2D* depthStencil)
{
	m_list->RSSetViewports(1, &target->Viewport);
	m_list->RSSetScissorRects(1, &target->ScissorRect);

	TransitionResource(target, D3D12_RESOURCE_STATE_RENDER_TARGET);

	D3D12_CPU_DESCRIPTOR_HANDLE* depthDescriptor = nullptr;

	if (depthStencil) {
		TransitionResource(depthStencil, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		depthDescriptor = &depthStencil->Views.DepthStencil;
	}

	m_list->OMSetRenderTargets(1, &target->Views.RenderTarget, FALSE, depthDescriptor);
}

void env::DirectList::SetIndexBuffer(Buffer* buffer)
{
	assert(buffer->Views.Index.Format != DXGI_FORMAT_UNKNOWN);
	TransitionResource(buffer, D3D12_RESOURCE_STATE_INDEX_BUFFER);

	if (m_state.IndexBuffer != buffer->Native) {
		m_list->IASetIndexBuffer(&buffer->Views.Index);
		m_state.IndexBuffer = buffer->Native;
	}
}

void env::DirectList::SetVertexBuffer(Buffer* buffer, UINT slot)
{
	assert(buffer->Views.Vertex.StrideInBytes > 0);
	TransitionResource(buffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	m_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // Define this in vertex buffer

	if (m_state.VertexBuffers[slot] != buffer->Native) {
		m_list->IASetVertexBuffers(slot, 1, &buffer->Views.Vertex);
		m_state.VertexBuffers[slot] = buffer->Native;
	}
}
