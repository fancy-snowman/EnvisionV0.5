#pragma once


namespace env
{
	class FrameDescriptorLinearAllocator;
	class CommandList;

	struct FramePassRecordContext
	{
		CommandList* CommandList;
		FrameDescriptorLinearAllocator* ResourceViewAllocator;
		FrameDescriptorLinearAllocator* RenderTargetViewAllocator;
		FrameDescriptorLinearAllocator* DepthStencilViewAllocator;
		FrameDescriptorLinearAllocator* SamplerViewAllocator;
	};
}
