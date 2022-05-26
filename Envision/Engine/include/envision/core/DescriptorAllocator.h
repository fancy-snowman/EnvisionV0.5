#pragma once
#include "envision/envpch.h"
#include "envision/resource/Resource.h"

namespace env
{
	class DescriptorAllocator
	{
	private:

		const D3D12_DESCRIPTOR_HEAP_TYPE m_type;
		UINT m_stride;

		ID3D12DescriptorHeap* m_heap;

		D3D12_CPU_DESCRIPTOR_HANDLE m_begin;
		UINT m_nextIndex;
		UINT m_endIndex;

		std::vector<UINT> m_freeList;

	public:

		DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool isShaderVisible);
		~DescriptorAllocator();

		DescriptorAllocator(DescriptorAllocator&& other) = delete;
		DescriptorAllocator(const DescriptorAllocator& other) = delete;
		DescriptorAllocator& operator=(DescriptorAllocator&& other) = delete;
		DescriptorAllocator& operator=(const DescriptorAllocator& other) = delete;

	public:

		D3D12_CPU_DESCRIPTOR_HANDLE Allocate(Resource* resource);
		void Free(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	};
}