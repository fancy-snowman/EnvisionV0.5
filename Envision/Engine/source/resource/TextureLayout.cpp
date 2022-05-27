#include "envision/envpch.h"
#include "envision/resource/TextureLayout.h"

size_t env::GetTextureLayoutTypeStride(TextureLayout layout)
{
	switch (layout)
	{
	case TextureLayout::RGA_UINT:		return sizeof(int) * 3;
	case TextureLayout::RGBA_UINT:		return sizeof(int) * 4;
	case TextureLayout::RGA_FLOAT:		return sizeof(float) * 3;
	case TextureLayout::RGBA_FLOAT:		return sizeof(float) * 4;
	}
	return 0;
}

DXGI_FORMAT env::GetDXGIFormat(TextureLayout layout)
{
	switch (layout)
	{
	case TextureLayout::RGA_UINT:		return DXGI_FORMAT_R32G32B32_UINT;
	case TextureLayout::RGBA_UINT:		return DXGI_FORMAT_R32G32B32A32_UINT;
	case TextureLayout::RGA_FLOAT:		return DXGI_FORMAT_R32G32B32_FLOAT;
	case TextureLayout::RGBA_FLOAT:		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	return DXGI_FORMAT_UNKNOWN;
}
