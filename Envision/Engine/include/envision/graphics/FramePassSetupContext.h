#pragma once

#include <vector>

namespace env
{
	class FrameGraph;
	class FrameResource;

	class FramePassSetupContext
	{
		friend class FrameGraph;

		FramePassSetupContext();

		std::vector<FrameResource*> Samplers;
		std::vector<FrameResource*> ReadResources;
		std::vector<FrameResource*> ReadWriteResources;
		std::vector<FrameResource*> RenderTargets;
		std::vector<FrameResource*> DepthTargets;
	public:
		void UseSampler(FrameResource* resource);
		void ReadResource(FrameResource* resource);
		void ReadWriteResource(FrameResource* resource);
		void WriteRenderTarget(FrameResource* resource);
		void WriteDepthTarget(FrameResource* resource);
	};
}