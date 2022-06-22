#include "envision/envpch.h"
#include "envision/graphics/RootSignature.h"

D3D12_ROOT_PARAMETER_TYPE env::GetD3D12Type(const ParameterType& type)
{
	switch (type)
	{
		case ParameterType::Constant:	return D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		case ParameterType::Table:		return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		case ParameterType::CBV:		return D3D12_ROOT_PARAMETER_TYPE_CBV;
		case ParameterType::SRV:		return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case ParameterType::UAV:		return D3D12_ROOT_PARAMETER_TYPE_UAV;
	}
	return D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS; // Error
}

env::RootParameter::RootParameter(ParameterType type, ShaderStage shaderStage, D3D12_ROOT_CONSTANTS constants)
{
	Info.ParameterType = GetD3D12Type(type);
	Info.Constants = constants;
	Info.ShaderVisibility = GetD3D12Visibility(shaderStage);
	Ranges.shrink_to_fit();
}

env::RootParameter::RootParameter(ParameterType type, ShaderStage shaderStage, D3D12_ROOT_DESCRIPTOR descriptor)
{
	Info.ParameterType = GetD3D12Type(type);
	Info.Descriptor = descriptor;
	Info.ShaderVisibility = GetD3D12Visibility(shaderStage);
	Ranges.shrink_to_fit();
}

env::RootParameter::RootParameter(ParameterType type, ShaderStage shaderStage, std::initializer_list<D3D12_DESCRIPTOR_RANGE> ranges) :
	Ranges(ranges.begin(), ranges.end())
{
	Info.ParameterType = GetD3D12Type(type);
	Info.DescriptorTable.NumDescriptorRanges = ranges.size();
	Info.DescriptorTable.pDescriptorRanges = nullptr;
	Info.ShaderVisibility = GetD3D12Visibility(shaderStage);
	Ranges.shrink_to_fit();
}

void env::RootSignature::AdjustRangeTablePointers()
{
	m_ranges.shrink_to_fit();

	int rangeOffset = 0;
	for (auto& p : m_parameters) {
		if (p.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {

			p.DescriptorTable.pDescriptorRanges = (m_ranges.data() + rangeOffset);

			int descriptorOffset = 0;
			auto itRangesBegin = m_ranges.begin() + rangeOffset;
			auto itRangesEnd = itRangesBegin + p.DescriptorTable.NumDescriptorRanges;
			for (auto itRange = itRangesBegin; itRange != itRangesEnd; itRange++) {
				itRange->OffsetInDescriptorsFromTableStart = descriptorOffset;
				descriptorOffset += itRange->NumDescriptors;
			}

			rangeOffset += p.DescriptorTable.NumDescriptorRanges;
		}
	}
}

env::RootSignature::RootSignature()
{
}

env::RootSignature::~RootSignature()
{
}

env::RootSignature::RootSignature(const std::vector<RootParameter>& elements)
{
	for (auto& e : elements)
	{
		if (e.Info.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			int rangeOffset = m_ranges.size();
			m_ranges.insert(m_ranges.end(), e.Ranges.begin(), e.Ranges.end());
		}
		m_parameters.push_back(e.Info);
	}
	AdjustRangeTablePointers();
}

env::RootSignature::RootSignature(std::initializer_list<RootParameter> elements)
{
	for (auto& e : elements)
	{
		if (e.Info.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			int rangeOffset = m_ranges.size();
			m_ranges.insert(m_ranges.end(), e.Ranges.begin(), e.Ranges.end());
			// TODO: Find where these start
		}
		m_parameters.push_back(e.Info);
	}
	AdjustRangeTablePointers();
}

UINT env::RootSignature::GetNumParameters() const
{
	return (UINT)m_parameters.size();
}

const D3D12_ROOT_PARAMETER* env::RootSignature::GetParameterArrayStart() const
{
	return m_parameters.data();
}
