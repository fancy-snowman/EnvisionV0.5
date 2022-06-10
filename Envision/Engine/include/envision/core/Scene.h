#pragma once
#include "envision/envpch.h"
#include "envision/core/Time.h"

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
	};



	/**
	*	Definitions for scene's template function below
	*/

	template<typename T>
	inline bool Scene::HasComponent(ID entity)
	{
		return m_registry.any_of<T>(entity);
	}

	template<typename T>
	inline void Scene::RemoveComponent(ID entity)
	{
		m_registry.remove<T>(entity);
	}

	template<typename T>
	inline T& Scene::GetComponent(ID entity)
	{
		return m_registry.get<T>(entity);
	}

	template<typename T>
	inline T& Scene::SetComponent(ID entity, const T& component)
	{
		return m_registry.emplace_or_replace<T>(entity, component);
	}

	template<typename T, typename ...Args>
	inline T& Scene::SetComponent(ID entity, Args ...args)
	{
		return m_registry.emplace_or_replace<T>(entity, std::forward(args));
	}
}