#include "envision/envpch.h"
#include "envision/resource/Resource.h"

env::BufferBindType env::operator|(BufferBindType a, BufferBindType b)
{
	return BufferBindType((int)a | (int)b);
}

env::BufferBindType env::operator&(BufferBindType a, BufferBindType b)
{
	return BufferBindType((int)a & (int)b);
}

bool env::any(BufferBindType type)
{
	return (int)type > 0;
}

env::TextureBindType env::operator|(TextureBindType a, TextureBindType b)
{
	return TextureBindType((int)a | (int)b);
}

env::TextureBindType env::operator&(TextureBindType a, TextureBindType b)
{
	return TextureBindType((int)a & (int)b);
}

bool env::any(TextureBindType type)
{
	return (int)type > 0;
}
