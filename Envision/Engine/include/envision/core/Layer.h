#pragma once
#include "envision/envpch.h"
#include "envision\core\Time.h"

namespace env
{
	class Layer
	{
		std::string m_name;

	public:

		Layer(const std::string& name = "Layer") : m_name(name) {}
		virtual ~Layer() = default;

	public:

		inline const std::string& GetName() const { return m_name; }

	public:

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(const Duration& delta) {}
		virtual void OnEvent() {}
	};
}