#pragma once
#include "envision/envpch.h"

namespace
{
	enum class ResourceType
	{
		UNKNOWN = 0,

		BUFFER,
		BUFFER_ARRAY,

		TEXTURE2D,
		TEXTURE2D_ARRAY,

		WINDOW_TARGET,
	};

	struct Resource
	{
		ID3D12Resource* Resource;
		D3D12_RESOURCE_STATES State = D3D12_RESOURCE_STATE_COMMON;
	};

	struct Buffer : Resource
	{
		const size_t ByteWidth;
	};

	struct BufferArray : Resource
	{
		const size_t ByteWidth;
		const size_t ElementStride;
		const int ElementCount;
	};
}