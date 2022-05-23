#pragma once

namespace env
{
	enum class TextureLayout
	{
		RGA_UINT,
		RGBA_UINT,

		RGA_FLOAT,
		RGBA_FLOAT,
	};

	size_t GetTextureLayoutTypeStride(TextureLayout type);
}