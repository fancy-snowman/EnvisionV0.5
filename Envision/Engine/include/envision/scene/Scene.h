#pragma once
#include "envision/envpch.h"
#include "envision/core/Time.h"
#include "envision/scene/System.h"

namespace env
{
	class Scene
	{
	private:

		entt::registry m_registry;

	public:

		Scene();
		~Scene();

		Scene(const Scene& other) = delete;
		Scene(const Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(const Scene&& other) = delete;

	public:

		// Entity related

		ID CreateEntity();
		void RemoveEntity(ID entity);

		int GetEntityCount();
		bool IsEntity(ID entity);
		
		// Component related

		template <typename T> bool HasComponent(ID entity);
		template <typename T> void RemoveComponent(ID entity);

		template <typename T> T& GetComponent(ID entity);
		template <typename T> T& SetComponent(ID entity, const T& component);
		template <typename T, typename ...Args> T& SetComponent(ID entity, Args... args);
		
		// System related

		template <typename T, typename... Args> System& CreateSystem(Args... args);
		void UpdateSystems(const Duration& delta);
	};
}