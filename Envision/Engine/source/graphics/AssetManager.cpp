#include "envision/envpch.h"
#include "envision/graphics/Assets.h"
#include "envision/graphics/AssetManager.h"

env::AssetManager* env::AssetManager::s_instance = nullptr;

env::AssetManager* env::AssetManager::Initialize(IDGenerator& commonIDGenerator)
{
	if (!s_instance)
		s_instance = new AssetManager(commonIDGenerator);
	return s_instance;
}

env::AssetManager* env::AssetManager::Get()
{
	assert(s_instance);
	return s_instance;
}

void env::AssetManager::Finalize()
{
	delete s_instance;
	s_instance = nullptr;
}

env::AssetManager::AssetManager(env::IDGenerator& commonIDGenerator) :
	m_commonIDGenerator(commonIDGenerator)
{
	//
}

env::AssetManager::~AssetManager()
{
	//
}

env::Mesh* env::AssetManager::GetMesh(ID resourceID)
{
	if (!m_meshes.count(resourceID))
		return nullptr;
	return m_meshes[resourceID];
}

env::Material* env::AssetManager::GetMaterial(ID resourceID)
{
	if (!m_materials.count(resourceID))
		return nullptr;
	return m_materials[resourceID];
}

ID env::AssetManager::CreateMesh(const std::string& name)
{
	struct Vertex
	{
		float x, y;
		float r, g, b;
		Vertex(float x, float y, float r, float g, float b) :
			x(x), y(y), r(r), g(g), b(b) {}
	};

	std::array<Vertex, 3> vertices = {
		Vertex( 0.0f,  0.5f,    1.0f, 0.0f, 0.0f),
		Vertex( 0.5f, -0.5f,    0.0f, 1.0f, 0.0f),
		Vertex(-0.5f, -0.5f,    0.0f, 0.0f, 1.0f),
	};

	std::array<int, 3> indices = { 0, 1, 2 };

	ID vertexBuffer = ResourceManager::Get()->CreateVertexBuffer("VertexBuffer",
		vertices.size(),
		{
			{ "position", ShaderDataType::Float2 },
			{ "color", ShaderDataType::Float3 },
		},
		vertices.data());

	ID indexBuffer = ResourceManager::Get()->CreateIndexBuffer("IndexBuffer",
		indices.size(),
		indices.data());

	ID meshID = m_commonIDGenerator.GenerateUnique();
	Mesh* mesh = new Mesh(meshID, "TestMesh");
	mesh->VertexBuffer = vertexBuffer;
	mesh->NumVertices = (int)vertices.size();
	mesh->IndexBuffer = indexBuffer;
	mesh->NumIndices = (int)indices.size();
	m_meshes[meshID] = mesh;

	return meshID;
}

ID env::AssetManager::CreatePhongMaterial(const std::string& name)
{
	//ID materialID = m_IDGenerator.GenerateUnique();
	//PhongMaterial* material = new PhongMaterial(materialID, "Test material");
	//material->DiffuseFactor = { 0.8f, 0.4f, 0.4f };
	//material->AmbientFactor = { 0.4f, 0.8f, 0.4f };
	//material->SpecularFactor = { 0.4f, 0.4f, 0.8f };

	//m_materials[materialID] = material;

	//return materialID;
	return 0;
}
