#pragma once
#include "envision/envpch.h"
#include "envision/core/Time.h"
#include "envision/core/Event.h"
#include "envision/core/Scene.h"

namespace env
{
	class System
	{
		std::string m_name;

	public:

		System(const std::string& name = "Layer") : m_name(name) {}
		virtual ~System() = default;

	public:

		inline const std::string& GetName() const { return m_name; }

	public:

		virtual void OnAttach(Scene& scene) {}
		virtual void OnDetach(Scene& scene) {}
		virtual void OnUpdate(Scene& scene, const Duration& delta) {}
		virtual void OnEvent(Scene& scene, env::Event& event) {}
	};
}