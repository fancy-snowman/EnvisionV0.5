#include "envision/envpch.h"
#include "envision/resource/Resource.h"

env::BindType env::operator|(BindType a, BindType b)
{
	return BindType((int)a | (int)b);
}

env::BindType env::operator&(BindType a, BindType b)
{
	return BindType((int)a & (int)b);
}

bool env::any(BindType type)
{
	return (int)type > 0;
}
