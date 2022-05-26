#include "envision/envpch.h"
#include "envision/core/DescriptorAllocator.h"
#include "envision/core/GPU.h"

env::DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool isShaderVisible) :
    m_type(type),
    m_nextIndex(0),
    m_endIndex(numDescriptors)
{
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

    m_begin = m_heap->GetCPUDescriptorHandleForHeapStart();
}

env::DescriptorAllocator::~DescriptorAllocator()
{
}

D3D12_CPU_DESCRIPTOR_HANDLE env::DescriptorAllocator::Allocate(Resource* resource)
{
    if (m_freeList.size() > 0)
    {
        UINT freeIndex = m_freeList.back();
        m_freeList.pop_back();
        return { m_begin.ptr + m_stride * freeIndex };
    }

    assert(m_nextIndex != m_endIndex); // TODO: Increase heap size
    return { m_begin.ptr + m_stride * m_nextIndex++ };
}

void env::DescriptorAllocator::Free(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
    // TODO: Check if handle is actually allocated
    UINT index = (handle.ptr - m_begin.ptr) / m_stride;
    m_freeList.push_back(index);
}
