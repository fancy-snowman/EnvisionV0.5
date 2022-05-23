#include "envision/resource/ShaderDataType.h"

size_t env::GetShaderDataTypeSize(ShaderDataType type)
{
	switch (type)
	{
		// Scalar
	case ShaderDataType::Bool:			return sizeof(bool);
	case ShaderDataType::Int:			return sizeof(int);
	case ShaderDataType::Uint:			return sizeof(unsigned int);
	case ShaderDataType::Float:			return sizeof(float);
	case ShaderDataType::Double:		return sizeof(double);

		// Vector
	case ShaderDataType::Bool2:			return sizeof(bool) * 2;
	case ShaderDataType::Bool3:			return sizeof(bool) * 3;
	case ShaderDataType::Bool4:			return sizeof(bool) * 4;
	case ShaderDataType::Int2:			return sizeof(int) * 2;
	case ShaderDataType::Int3:			return sizeof(int) * 3;
	case ShaderDataType::Int4:			return sizeof(int) * 4;
	case ShaderDataType::Uint2:			return sizeof(unsigned int) * 2;
	case ShaderDataType::Uint3:			return sizeof(unsigned int) * 3;
	case ShaderDataType::Uint4:			return sizeof(unsigned int) * 4;
	case ShaderDataType::Float2:		return sizeof(float) * 2;
	case ShaderDataType::Float3:		return sizeof(float) * 3;
	case ShaderDataType::Float4:		return sizeof(float) * 4;
	case ShaderDataType::Double2:		return sizeof(double) * 2;
	case ShaderDataType::Double3:		return sizeof(double) * 3;
	case ShaderDataType::Double4:		return sizeof(double) * 4;

		// Matrix
	case ShaderDataType::Bool2x2:		return sizeof(bool) * 2 * 2;
	case ShaderDataType::Bool2x3:		return sizeof(bool) * 2 * 3;
	case ShaderDataType::Bool2x4:		return sizeof(bool) * 2 * 4;
	case ShaderDataType::Bool3x2:		return sizeof(bool) * 3 * 2;
	case ShaderDataType::Bool3x3:		return sizeof(bool) * 3 * 3;
	case ShaderDataType::Bool3x4:		return sizeof(bool) * 3 * 4;
	case ShaderDataType::Bool4x2:		return sizeof(bool) * 4 * 2;
	case ShaderDataType::Bool4x3:		return sizeof(bool) * 4 * 3;
	case ShaderDataType::Bool4x4:		return sizeof(bool) * 4 * 4;
	case ShaderDataType::Int2x2:		return sizeof(int) * 2 * 2;
	case ShaderDataType::Int2x3:		return sizeof(int) * 2 * 3;
	case ShaderDataType::Int2x4:		return sizeof(int) * 2 * 4;
	case ShaderDataType::Int3x2:		return sizeof(int) * 3 * 2;
	case ShaderDataType::Int3x3:		return sizeof(int) * 3 * 3;
	case ShaderDataType::Int3x4:		return sizeof(int) * 3 * 4;
	case ShaderDataType::Int4x2:		return sizeof(int) * 4 * 2;
	case ShaderDataType::Int4x3:		return sizeof(int) * 4 * 3;
	case ShaderDataType::Int4x4:		return sizeof(int) * 4 * 4;
	case ShaderDataType::Uint2x2:		return sizeof(unsigned int) * 2 * 2;
	case ShaderDataType::Uint2x3:		return sizeof(unsigned int) * 2 * 3;
	case ShaderDataType::Uint2x4:		return sizeof(unsigned int) * 2 * 4;
	case ShaderDataType::Uint3x2:		return sizeof(unsigned int) * 3 * 2;
	case ShaderDataType::Uint3x3:		return sizeof(unsigned int) * 3 * 3;
	case ShaderDataType::Uint3x4:		return sizeof(unsigned int) * 3 * 4;
	case ShaderDataType::Uint4x2:		return sizeof(unsigned int) * 4 * 2;
	case ShaderDataType::Uint4x3:		return sizeof(unsigned int) * 4 * 3;
	case ShaderDataType::Uint4x4:		return sizeof(unsigned int) * 4 * 4;
	case ShaderDataType::Float2x2:		return sizeof(float) * 2 * 2;
	case ShaderDataType::Float2x3:		return sizeof(float) * 2 * 3;
	case ShaderDataType::Float2x4:		return sizeof(float) * 2 * 4;
	case ShaderDataType::Float3x2:		return sizeof(float) * 3 * 2;
	case ShaderDataType::Float3x3:		return sizeof(float) * 3 * 3;
	case ShaderDataType::Float3x4:		return sizeof(float) * 3 * 4;
	case ShaderDataType::Float4x2:		return sizeof(float) * 4 * 2;
	case ShaderDataType::Float4x3:		return sizeof(float) * 4 * 3;
	case ShaderDataType::Float4x4:		return sizeof(float) * 4 * 4;
	case ShaderDataType::Double2x2:		return sizeof(double) * 2 * 2;
	case ShaderDataType::Double2x3:		return sizeof(double) * 2 * 3;
	case ShaderDataType::Double2x4:		return sizeof(double) * 2 * 4;
	case ShaderDataType::Double3x2:		return sizeof(double) * 3 * 2;
	case ShaderDataType::Double3x3:		return sizeof(double) * 3 * 3;
	case ShaderDataType::Double3x4:		return sizeof(double) * 3 * 4;
	case ShaderDataType::Double4x2:		return sizeof(double) * 4 * 2;
	case ShaderDataType::Double4x3:		return sizeof(double) * 4 * 3;
	case ShaderDataType::Double4x4:		return sizeof(double) * 4 * 4;
	}
	return 0;
}