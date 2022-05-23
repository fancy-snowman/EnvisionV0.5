#pragma once
#include "envision/envpch.h"

namespace env
{
	enum class ShaderType
	{
		UNKNOWN = 0,

		// Vertex pipeline
		Vertex		= 1 << 0,
		Hull		= 1 << 1,
		Domain		= 1 << 2,
		Geometry	= 1 << 3,
		Pixel		= 1 << 4,

		// Compute pipeline
		Compute		= 1 << 5,
	};
	inline ShaderType operator|(ShaderType a, ShaderType b)
	{
		return (ShaderType)((int)a | (int)b);
	}

	struct Shader
	{
		ID3DBlob* Shader;
	};

	struct PipelineState
	{
		ID3D12PipelineState* State;
		
	};
}