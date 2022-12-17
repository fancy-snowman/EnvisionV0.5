#include "envision/envpch.h"
#include "envision/graphics/FramePassSetupContext.h"

env::FramePassSetupContext::FramePassSetupContext()
{
	//
}

void env::FramePassSetupContext::UseSampler(FrameResource* resource)
{
	Samplers.push_back(resource);
}

void env::FramePassSetupContext::ReadResource(FrameResource* resource)
{
	ReadResources.push_back(resource);
}

void env::FramePassSetupContext::ReadWriteResource(FrameResource* resource)
{
	ReadWriteResources.push_back(resource);
}

void env::FramePassSetupContext::WriteRenderTarget(FrameResource* resource)
{
	RenderTargets.push_back(resource);
}

void env::FramePassSetupContext::WriteDepthTarget(FrameResource* resource)
{
	DepthTargets.push_back(resource);
}
