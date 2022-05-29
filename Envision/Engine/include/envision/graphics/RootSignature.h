#pragma once
#include "envision/envpch.h"
#include "envision/graphics/Shader.h"

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