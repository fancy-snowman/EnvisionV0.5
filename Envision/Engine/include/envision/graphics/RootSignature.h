#pragma once
#include "envision/envpch.h"
#include "envision/graphics/Shader.h"

#define ROOT_CONSTANTS(stages, shaderRegister, registerSpace, numValues) \
	{ ParameterType::Constant, stages, D3D12_ROOT_CONSTANTS{ shaderRegister, registerSpace, numValues } }
#define ROOT_DESCRIPTOR(type, stages, shaderRegister, registerSpace) \
	{ type, stages, D3D12_ROOT_DESCRIPTOR{shaderRegister, registerSpace} }
#define ROOT_CBV_DESCRIPTOR(stages, shaderRegister, registerSpace) \
	ROOT_DESCRIPTOR(ParameterType::CBV, stages, shaderRegister, registerSpace)
#define ROOT_UAV_DESCRIPTOR(stages, shaderRegister, registerSpace) \
	ROOT_DESCRIPTOR(ParameterType::UAV, stages, shaderRegister, registerSpace)
#define ROOT_RTV_DESCRIPTOR(stages, shaderRegister, registerSpace) \
	ROOT_DESCRIPTOR(ParameterType::RTV, stages, shaderRegister, registerSpace)
#define TABLE_RANGE(type, numDescriptors, baseShaderRegister, registerSpace, offsetDescTable) \
	{ type, numDescriptors, baseShaderRegister, registerSpace, offsetDescTable }
#define SRV_RANGE(numDescriptors, baseShaderRegister, registerSpace, offsetDescTable) \
	TABLE_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, numDescriptors, baseShaderRegister, registerSpace, offsetDescTable)
#define UAV_RANGE(numDescriptors, baseShaderRegister, registerSpace, offsetDescTable) \
	TABLE_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, numDescriptors, baseShaderRegister, registerSpace, offsetDescTable)
#define RTV_RANGE(numDescriptors, baseShaderRegister, registerSpace, offsetDescTable) \
	TABLE_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, numDescriptors, baseShaderRegister, registerSpace, offsetDescTable)
#define SAMPLER_RANGE(numDescriptors, baseShaderRegister, registerSpace, offsetDescTable) \
	TABLE_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, numDescriptors, baseShaderRegister, registerSpace, offsetDescTable)
#define ROOT_TABLE(stages, ranges) \
	{ ParameterType::Table, stages, { ranges } }


namespace env
{
	enum class ParameterType
	{
		Unknown = 0,
		Constant,
		Table,
		CBV,
		SRV,
		UAV,
	};
	D3D12_ROOT_PARAMETER_TYPE GetD3D12Type(const ParameterType& type);

	struct RootParameter
	{
		D3D12_ROOT_PARAMETER Info;

		// In case of D3D12_ROOT_DESCRIPTOR_TABLE
		int RangeOffset = -1;
		std::vector<D3D12_DESCRIPTOR_RANGE> Ranges = std::vector<D3D12_DESCRIPTOR_RANGE>(0);

		RootParameter(ParameterType type, ShaderStage shaderStage, D3D12_ROOT_CONSTANTS constants);
		RootParameter(ParameterType type, ShaderStage shaderStage, D3D12_ROOT_DESCRIPTOR descriptor);
		RootParameter(ParameterType type, ShaderStage shaderStage, std::initializer_list<D3D12_DESCRIPTOR_RANGE> ranges);
	};

	class RootSignature
	{
	private:

		std::vector<D3D12_DESCRIPTOR_RANGE> m_ranges;
		std::vector<D3D12_ROOT_PARAMETER> m_parameters;

		void AdjustRangeTablePointers();

	public:

		RootSignature();
		~RootSignature();

		RootSignature(const std::vector<RootParameter>& elements);
		RootSignature(std::initializer_list<RootParameter> elements);

		UINT GetNumParameters() const;
		const D3D12_ROOT_PARAMETER* GetParameterArrayStart() const;
	};
}