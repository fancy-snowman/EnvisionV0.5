#include "envision/graphics/GPUHeap.h"

GPUHeap::GPUHeap(ID3D12Device* device, UINT byteWidth)
{
	assert(byteWidth > 0);

	ZeroMemory(&m_desc, sizeof(m_desc));
	m_desc.SizeInBytes = byteWidth;
	m_desc.Properties.Type;
	m_desc.Properties.CPUPageProperty;
	m_desc.Properties.MemoryPoolPreference;
	m_desc.Properties.CreationNodeMask;
	m_desc.Properties.VisibleNodeMask;
	m_desc.Alignment;
	m_desc.Flags;

	device->CreateHeap()
}

GPUHeap::~GPUHeap()
{
}
