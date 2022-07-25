#pragma once
#include "envision/core/IDGenerator.h"
#include "envision/graphics/Assets.h"
#include "envision/resource/ResourceManager.h"

namespace env
{
	struct LoadedEntityInfo
	{
		
	};

	// Singleton
	class AssetManager
	{
	private:

		env::IDGenerator& m_commonIDGenerator;

		ID m_defaultTexture_white = ID_ERROR;

		std::unordered_map<ID, Mesh*> m_meshes;
		std::unordered_map<ID, Material*> m_materials;

	public:

		static AssetManager* Initialize(IDGenerator& commonIDGenerator);
		static AssetManager* Get();
		static void Finalize();

	private:

		static AssetManager* s_instance;

		AssetManager(env::IDGenerator& commonIDGenerator);
		~AssetManager();

		AssetManager(const AssetManager& other) = delete;
		AssetManager(const AssetManager&& other) = delete;
		AssetManager& operator=(const AssetManager& other) = delete;
		AssetManager& operator=(const AssetManager&& other) = delete;

	public:

		Mesh* GetMesh(ID resourceID);
		Material* GetMaterial(ID resourceID);

	public:

		//ID CreateMesh(const std::string& name, const std::string& filePath);
		//ID CreateMesh(const std::string& name, VERTICES AND INDICES);

		ID CreateMesh(const std::string& name); // Prototype
		ID CreateMesh(const std::string& name, void* vertices, const BufferLayout& vertexBufferLayout, void* indices, UINT numIndices);
		ID CreateMesh(const std::string& name, ID vertexBuffer, UINT offsetVertices, UINT numVertices, ID indexBuffer, UINT offsetIndices, UINT numIndices);
		ID LoadMesh(const std::string& name, const std::string& filePath);
		ID CreatePhongMaterial(const std::string& name, Float3 ambient, Float3 diffuse, Float3 specular, float shininess);
		ID CreatePhongMaterial(const std::string& name, Float3 ambient, Float3 diffuse, Float3 specular, float shininess,
			const std::string& ambientMap, const std::string& diffuseMap, const std::string& specularMap);
	};
}