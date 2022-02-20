#pragma once
#include "envision\envpch.h"

#define ID_ERROR 0;

namespace env
{

	class IDGenerator
	{
		ID m_IDCounter = 1;

	public:

		IDGenerator() = default;
		~IDGenerator() = default;

		ID GenerateUnique();
	};
}