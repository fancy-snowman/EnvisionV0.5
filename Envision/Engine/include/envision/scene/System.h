#pragma once
#include "envision/envpch.h"
#include "envision/core/Time.h"

namespace env
{
	class System
	{
	private:

		System() {}
		virtual ~System() {}

		System(const System& other) = delete;
		System(const System&& other) = delete;
		System& operator=(const System& other) = delete;
		System& operator=(const System&& other) = delete;

	public:

		virtual bool OnUpdate(entt::registry& registry, const Duration& delta) = 0;
	};
}
