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
