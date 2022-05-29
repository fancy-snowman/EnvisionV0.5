#pragma once
#include "envision/envpch.h"
#include "envision/resource/Resource.h"

namespace env
{
	class DescriptorAllocator
	{
	private:

		D3D12_DESCRIPTOR_HEAP_TYPE m_type;
		UINT m_stride;

		ID3D12DescriptorHeap* m_heap;

		D3D12_CPU_DESCRIPTOR_HANDLE m_begin;
		UINT m_nextIndex;
		UINT m_endIndex;

		std::vector<UINT> m_freeList;

	public:

		DescriptorAllocator();
		DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool isShaderVisible);
		~DescriptorAllocator();

		void Initialize(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool isShaderVisible);
		ID3D12DescriptorHeap* GetHeap();

		DescriptorAllocator(DescriptorAllocator&& other) = delete;
		DescriptorAllocator(const DescriptorAllocator& other) = delete;
		DescriptorAllocator& operator=(DescriptorAllocator&& other) = delete;
		DescriptorAllocator& operator=(const DescriptorAllocator& other) = delete;

	public:

		D3D12_CPU_DESCRIPTOR_HANDLE Allocate(UINT numDescriptors = 1);
		void Free(D3D12_CPU_DESCRIPTOR_HANDLE handle);
		void Clear();
	};
}