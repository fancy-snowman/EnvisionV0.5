#include "envision/envpch.h"
#include "envision/graphics/FrameResource.h"

env::FrameResource::FrameResource(const std::string& name, ID3D12Resource* resource) :
	Imported(true), Name(name), Resource(resource)
{
	assert(Resource);
	Description = Resource->GetDesc();
}

env::FrameResource::FrameResource(const std::string& name, const D3D12_RESOURCE_DESC& description) :
	Imported(false), Name(name), Resource(nullptr), Description(description)
{
	//
}

bool env::FrameResource::IsImported() const
{
	return Imported;
}

const std::string& env::FrameResource::GetName() const
{
	return Name;
}

ID3D12Resource* env::FrameResource::GetResource()
{
	return Resource;
}

const D3D12_RESOURCE_DESC& env::FrameResource::GetDesc() const
{
	return Description;
}

void env::FrameResource::ClearDependencies()
{
	ReadBy.clear();
	WrittenBy.clear();
}
