#include "envision/envpch.h"
#include "envision/graphics/Assets.h"
#include "envision/graphics/AssetManager.h"

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

	//ID vertexBuffer = m_resourceManager.CreateBuffer(
	//	"Vertex buffer",
	//	UpdatePattern::NEVER,
	//	AccessPattern::GPU_READ,
	//	sizeof(vertices));

	//ID indexBuffer = m_resourceManager.CreateBuffer(
	//	"Index buffer",
	//	UpdatePattern::NEVER,
	//	AccessPattern::GPU_READ,
	//	sizeof(indices)); 

	//ID meshID = m_IDGenerator.GenerateUnique();
	//Mesh* mesh = new Mesh(meshID, "Test mesh");
	//mesh->VertexBuffer = vertexBuffer;
	//mesh->NumVertices = (int)vertices.size();
	//mesh->IndexBuffer = indexBuffer;
	//mesh->NumIndices = (int)indices.size();

	//m_meshes[meshID] = mesh;

	// TODO: Upload vertices and indices to the GPU using the resource manager

	//return meshID;
	return 0;
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
