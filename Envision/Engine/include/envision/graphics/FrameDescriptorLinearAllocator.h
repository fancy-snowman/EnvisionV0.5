#pragma once
#include "envision/envpch.h"

namespace env
{
	class FrameDescriptorLinearAllocator
	{
		static const UINT DEFAULT_CAPACITY;

		UINT m_capacity;
		UINT m_nextOffset;
		UINT m_stride;
		ID3D12DescriptorHeap* m_heap;

	public:

		FrameDescriptorLinearAllocator(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
		~FrameDescriptorLinearAllocator();

		D3D12_CPU_DESCRIPTOR_HANDLE Allocate(UINT numDescriptors = 1);
		void Clear();

	private:

		FrameDescriptorLinearAllocator(FrameDescriptorLinearAllocator&& other) = delete;
		FrameDescriptorLinearAllocator(const FrameDescriptorLinearAllocator& other) = delete;
		FrameDescriptorLinearAllocator& operator=(FrameDescriptorLinearAllocator&& other) = delete;
		FrameDescriptorLinearAllocator& operator=(const FrameDescriptorLinearAllocator& other) = delete;
	};
}
