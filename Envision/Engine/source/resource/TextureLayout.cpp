#include "envision/envpch.h"
#include "envision/resource/TextureLayout.h"

size_t env::GetTextureLayoutTypeStride(TextureLayout type)
{
	switch (type)
	{
	case TextureLayout::RGA_UINT:		return sizeof(int) * 3;
	case TextureLayout::RGBA_UINT:		return sizeof(int) * 4;
	case TextureLayout::RGA_FLOAT:		return sizeof(float) * 3;
	case TextureLayout::RGBA_FLOAT:		return sizeof(float) * 4;
	}
	return 0;
}
