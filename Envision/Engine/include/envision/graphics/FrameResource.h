#pragma once
#include "envision/envpch.h"

#include <string>
#include <vector>

namespace env
{
	struct FramePass;
	class FrameGraph;
	class FramePassSetupContext;

	class FrameResource
	{
		friend class env::FrameGraph;
		friend class env::FramePassSetupContext;

		bool Imported;
		std::string Name;
		ID3D12Resource* Resource;
		D3D12_RESOURCE_DESC Description;

		std::vector<env::FramePass*> ReadBy;
		std::vector<env::FramePass*> WrittenBy;

		FrameResource(const std::string& name, ID3D12Resource* resource); // Importing
		FrameResource(const std::string& name, const D3D12_RESOURCE_DESC& description); // Creating
		~FrameResource() = default;

		FrameResource(FrameResource&& other) = default;
		FrameResource(const FrameResource& other) = default;
		FrameResource& operator=(FrameResource&& other) = default;
		FrameResource& operator=(const FrameResource& other) = default;

	public:

		bool IsImported() const;
		const std::string& GetName() const;
		ID3D12Resource* GetResource();
		const D3D12_RESOURCE_DESC& GetDesc() const;

		void ClearDependencies();
	};

}