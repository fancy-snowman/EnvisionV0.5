#pragma once
#include "envision/envpch.h"

struct GPUHeapAllocation
{

};

class GPUHeap
{
	D3D12_HEAP_DESC m_desc;
	ID3D12Heap* m_heap;

public:

	GPUHeap(ID3D12Device* device, UINT byteWidth);
	~GPUHeap();

public:


};
