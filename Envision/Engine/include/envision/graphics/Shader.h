#pragma once
#include "envision/envpch.h"

namespace env
{
	enum class ShaderStage
	{
		Unknown = 0,

		Vertex		= 1 << 0,
		Hull		= 1 << 1,
		Domain		= 1 << 2,
		Geometry	= 1 << 3,
		Pixel		= 1 << 4,

		Compute		= 1 << 5,
		
		AllVertex = Vertex | Hull | Domain | Geometry | Pixel,
	};
	ShaderStage operator|(ShaderStage a, ShaderStage b);
	ShaderStage operator&(ShaderStage a, ShaderStage b);
	bool any(ShaderStage stage);
	D3D12_SHADER_VISIBILITY GetD3D12Visibility(const ShaderStage& stages);

	enum class ShaderModel
	{
		Unknown = 0,
		V5_0,
		V5_1,
	};

	struct ShaderDesc
	{
		ShaderStage Stage;
		ShaderModel Model;
		std::string Path;
		std::string EntryPoint;
	};

	std::string GetTargetModelString(ShaderStage stage, ShaderModel model);
}