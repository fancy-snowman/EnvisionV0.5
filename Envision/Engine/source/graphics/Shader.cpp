#include "envision/envpch.h"
#include "envision/graphics/Shader.h"

env::ShaderStage env::operator|(ShaderStage a, ShaderStage b)
{
	return ShaderStage((int)a | (int)b);
}

env::ShaderStage env::operator&(ShaderStage a, ShaderStage b)
{
	return ShaderStage((int)a & (int)b);
}

bool env::any(ShaderStage stage)
{
	return ((int)stage) > 0;
}

D3D12_SHADER_VISIBILITY env::GetD3D12Visibility(const ShaderStage& stage)
{
	{ // Handle unknown or all staes
		bool noSpecifications = (stage == ShaderStage::Unknown);
		bool allVertexPipelineStages = (stage == ShaderStage::AllVertex);
		if (noSpecifications || allVertexPipelineStages)
			return D3D12_SHADER_VISIBILITY_ALL;
	}

	// If not all, then only one stage can be visible
	switch (stage)
	{
		case ShaderStage::Vertex:	return D3D12_SHADER_VISIBILITY_VERTEX;
		case ShaderStage::Hull:		return D3D12_SHADER_VISIBILITY_HULL;
		case ShaderStage::Domain:	return D3D12_SHADER_VISIBILITY_DOMAIN;
		case ShaderStage::Geometry:	return D3D12_SHADER_VISIBILITY_GEOMETRY;
		case ShaderStage::Pixel:	return D3D12_SHADER_VISIBILITY_PIXEL;

		// In the compute pipeline, all shaders are visible since
		// there is only a single stage.
		case ShaderStage::Compute:	return D3D12_SHADER_VISIBILITY_ALL;
	}

	return D3D12_SHADER_VISIBILITY_ALL; // Error
}

std::string env::GetTargetModelString(ShaderStage stage, ShaderModel model)
{
	std::string target;

	assert(stage != ShaderStage::Unknown);
	assert(model != ShaderModel::Unknown);

	switch (stage)
	{
		case ShaderStage::Vertex:
			target += "vs_";
			break;
		case ShaderStage::Hull:
			target += "hs_";
			break;
		case ShaderStage::Domain:
			target += "ds_";
			break;
		case ShaderStage::Geometry:
			target += "gs_";
			break;
		case ShaderStage::Pixel:
			target += "ps_";
			break;
		case ShaderStage::Compute:
			target += "cs_";
			break;
	}

	switch (model)
	{
	case ShaderModel::V5_0:
		target += "5_0";
		break;
	}

	return target;
}
