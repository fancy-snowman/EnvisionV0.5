#include "envision/envpch.h"
#include "envision/core/Scene.h"

env::Scene::Scene()
{

}

env::Scene::~Scene()
{

}

ID env::Scene::CreateEntity(const std::string& name)
{
	entt::entity entity = m_registry.create();
	m_registry.emplace<DebugInfoComponent>(entity, name);

	return (ID)entity;
}

void env::Scene::RemoveEntity(ID entity)
{
	m_registry.destroy((entt::entity)entity);
}

int env::Scene::GetEntityCount()
{
	return (int)m_registry.alive();
}

bool env::Scene::IsEntity(ID entity)
{
	return m_registry.valid((entt::entity)entity);
}

void env::Scene::LoadEntitiesFromFile(const std::string& filePath)
{
	//std::vector<
}
