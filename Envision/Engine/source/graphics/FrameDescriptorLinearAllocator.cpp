#include "envision/graphics/FrameDescriptorLinearAllocator.h"
#include <mutex>

const UINT env::FrameDescriptorLinearAllocator::DEFAULT_CAPACITY = 32;

env::FrameDescriptorLinearAllocator::FrameDescriptorLinearAllocator(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
	: m_capacity(DEFAULT_CAPACITY), m_nextOffset(0)
{
	assert(device);
	m_stride = device->GetDescriptorHandleIncrementSize(type);

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = type;
	desc.NumDescriptors = m_capacity;
	desc.Flags = flags;

	ASSERT_HR(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap)), "Could not create descriptor heap for frame descriptor linear allocator");
	assert(m_heap);
}

env::FrameDescriptorLinearAllocator::~FrameDescriptorLinearAllocator()
{
	m_heap->Release();
}

D3D12_CPU_DESCRIPTOR_HANDLE env::FrameDescriptorLinearAllocator::Allocate(UINT numDescriptors)
{
	static std::mutex sharedLock;
	
	UINT offset = 0;

	{
		std::scoped_lock lock(sharedLock);
		assert(m_nextOffset + numDescriptors <= m_capacity);
		offset = m_nextOffset;
		m_nextOffset += numDescriptors;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE allocation = { m_heap->GetCPUDescriptorHandleForHeapStart().ptr + offset * m_stride };
	return allocation;
}

void env::FrameDescriptorLinearAllocator::Clear()
{
	m_nextOffset = 0;
}
