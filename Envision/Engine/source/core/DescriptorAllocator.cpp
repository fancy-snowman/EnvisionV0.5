#include "envision/envpch.h"
#include "envision/core/DescriptorAllocator.h"
#include "envision/core/GPU.h"

env::DescriptorAllocator::DescriptorAllocator() :
    m_type(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
    m_stride(0),
    m_heap(nullptr),
    m_beginCPU({ 0 }),
    m_nextIndex(0),
    m_endIndex(0)
{
}

env::DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool isShaderVisible) :
    m_type(type),
    m_nextIndex(0),
    m_endIndex(numDescriptors)
{
    Initialize(type, numDescriptors, isShaderVisible);
}

env::DescriptorAllocator::~DescriptorAllocator()
{
}

void env::DescriptorAllocator::Initialize(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool isShaderVisible)
{
    m_type = type;
    m_nextIndex = 0;
    m_endIndex = numDescriptors;
    m_stride = GPU::GetDevice()->GetDescriptorHandleIncrementSize(m_type);

    D3D12_DESCRIPTOR_HEAP_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Type = m_type;
    desc.NumDescriptors = numDescriptors;
    desc.Flags = (isShaderVisible) ?
        D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE :
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    HRESULT hr = GPU::GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap));
    ASSERT_HR(hr, "Could not create descriptor heap");

    m_beginCPU = m_heap->GetCPUDescriptorHandleForHeapStart();
    m_beginGPU = m_heap->GetGPUDescriptorHandleForHeapStart();
}

ID3D12DescriptorHeap* env::DescriptorAllocator::GetHeap()
{
    return m_heap;
}

env::DescriptorAllocation env::DescriptorAllocator::Allocate(UINT numDescriptors)
{
    // TODO: smartify this allocator. It makes the boat float for now. :)

    DescriptorAllocation handle = { 0 };

    if (numDescriptors == 1 && m_freeList.size() > 0) {
        UINT freeIndex = m_freeList.back();
        m_freeList.pop_back();

        handle.CPUHandle = { m_beginCPU.ptr + m_stride * freeIndex };
        handle.GPUHandle = { m_beginGPU.ptr + m_stride * freeIndex };

        return handle;
    }

    assert((m_nextIndex + numDescriptors) < m_endIndex); // TODO: Increase heap size

    handle.CPUHandle = { m_beginCPU.ptr + m_stride * m_nextIndex };
    handle.GPUHandle = { m_beginGPU.ptr + m_stride * m_nextIndex };

    m_nextIndex += numDescriptors;

    return handle;
}

void env::DescriptorAllocator::Free(DescriptorAllocation handle)
{
    // TODO: Check if handle is actually allocated
    UINT index = (UINT)((handle.CPUHandle.ptr - m_beginCPU.ptr) / m_stride);
    m_freeList.push_back(index);
}

void env::DescriptorAllocator::Clear()
{
    m_freeList.clear();
    m_nextIndex = 0;
}
