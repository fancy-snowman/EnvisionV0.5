#pragma once
#include "envision/envpch.h"

namespace env
{
	struct VertexType
	{
		Float3 Position;
		Float3 Normal;
		Float2 Texcoord;
	};
	using IndexType = UINT;

	enum class ShaderDataType
	{
		Unknown = 0,

		// Scalar
		Bool,
		Int,
		Uint,
		Float,
		Double,

		// Vector
		Bool2,
		Bool3,
		Bool4,
		Int2,
		Int3,
		Int4,
		Uint2,
		Uint3,
		Uint4,
		Float2,
		Float3,
		Float4,
		Double2,
		Double3,
		Double4,

		// Matrix
		Bool2x2,
		Bool2x3,
		Bool2x4,
		Bool3x2,
		Bool3x3,
		Bool3x4,
		Bool4x2,
		Bool4x3,
		Bool4x4,
		Int2x2,
		Int2x3,
		Int2x4,
		Int3x2,
		Int3x3,
		Int3x4,
		Int4x2,
		Int4x3,
		Int4x4,
		Uint2x2,
		Uint2x3,
		Uint2x4,
		Uint3x2,
		Uint3x3,
		Uint3x4,
		Uint4x2,
		Uint4x3,
		Uint4x4,
		Float2x2,
		Float2x3,
		Float2x4,
		Float3x2,
		Float3x3,
		Float3x4,
		Float4x2,
		Float4x3,
		Float4x4,
		Double2x2,
		Double2x3,
		Double2x4,
		Double3x2,
		Double3x3,
		Double3x4,
		Double4x2,
		Double4x3,
		Double4x4
	};

	size_t GetShaderDataTypeSize(ShaderDataType type);
	DXGI_FORMAT GetDXGIFormat(ShaderDataType type);
	ShaderDataType GetShaderDataType(DXGI_FORMAT format);
}