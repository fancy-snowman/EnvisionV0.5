#pragma once
#include "envision/envpch.h"
#include "envision/core/CommandList.h"
#include "envision/graphics/FrameDescriptorLinearAllocator.h"

namespace env
{
	class FramePassContext
	{
		CommandList* m_commandList;

		FrameDescriptorLinearAllocator m_resourceDescriptorAllocator;
		FrameDescriptorLinearAllocator m_samplerDescriptorAllocator;
		FrameDescriptorLinearAllocator m_renderTargetDescriptorAllocator;
		FrameDescriptorLinearAllocator m_depthStencilDescriptorAllocator;

	public:

		FramePassContext(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
		~FramePassContext();

		CommandList* GetCommandList();
		CopyList* GetCopyList();
		ComputeList* GetComputeList();
		DirectList* GetDirectList();

		D3D12_CPU_DESCRIPTOR_HANDLE Allocate_CBV_SRV_UAV(UINT count);
		D3D12_CPU_DESCRIPTOR_HANDLE Allocate_sampler(UINT count);
		D3D12_CPU_DESCRIPTOR_HANDLE Allocate_RTV(UINT count);
		D3D12_CPU_DESCRIPTOR_HANDLE Allocate_DSV(UINT count);
	};
}
