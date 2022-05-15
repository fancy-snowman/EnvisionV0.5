#pragma once
#include "envision/core/IDGenerator.h"
#include "envision/graphics/Assets.h"
#include "envision/resource/ResourceManager.h"

namespace env
{
	class AssetManager
	{
		env::IDGenerator& m_IDGenerator;
		env::ResourceManager& m_resourceManager;

		std::unordered_map<ID, Mesh*> m_meshes;
		std::unordered_map<ID, Material*> m_materials;

	public:

		AssetManager(env::IDGenerator& idGenerator, env::ResourceManager& resourceManager);
		~AssetManager();

	public:

		Mesh* GetMesh(ID resourceID);
		Material* GetMaterial(ID resourceID);

	public:

		//ID CreateMesh(const std::string& name, const std::string& filePath);
		//ID CreateMesh(const std::string& name, VERTICES AND INDICES);

		ID CreateMesh(const std::string& name); // Prototype
		ID CreatePhongMaterial(const std::string& name); // Prototype
	};
}