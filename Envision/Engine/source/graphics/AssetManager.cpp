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
		Vertex( 0.0f,   1.0f,     1.0f, 0.0f, 0.0f),
		Vertex( 0.87f, -0.5f,    0.0f, 1.0f, 0.0f),
		Vertex(-0.87f, -0.5f,    0.0f, 0.0f, 1.0f),
	};

	std::array<unsigned int, 3> indices = { 0, 1, 2 };

	ID vertexBuffer = ResourceManager::Get()->CreateBuffer("VertexBuffer",
		BufferLayout({
			{ "position", ShaderDataType::Float2 },
			{ "color", ShaderDataType::Float3 },
		}, vertices.size()),
		BufferBindType::Vertex,
		vertices.data());

	ID indexBuffer = ResourceManager::Get()->CreateBuffer("IndexBuffer",
		BufferLayout({
			{ "index", ShaderDataType::Uint },
		}, indices.size()),
		BufferBindType::Index,
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

ID env::AssetManager::CreateMesh(const std::string& name, void* vertices, BufferLayout& vertexBufferLayout, void* indices, UINT numIndices)
{
	ID vertexBuffer = ResourceManager::Get()->CreateBuffer(name + "_vertexBuffer",
		vertexBufferLayout,
		BufferBindType::Vertex,
		vertices);

	ID indexBuffer = ResourceManager::Get()->CreateBuffer(name + "_indexBuffer",
		BufferLayout({
			{ "index", ShaderDataType::Uint },
			}, numIndices),
			BufferBindType::Index,
			indices);

	ID meshID = m_commonIDGenerator.GenerateUnique();
	Mesh* mesh = new Mesh(meshID, name);
	mesh->VertexBuffer = vertexBuffer;
	mesh->NumVertices = vertexBufferLayout.GetNumRepetitions();
	mesh->IndexBuffer = indexBuffer;
	mesh->NumIndices = (int)numIndices;
	m_meshes[meshID] = mesh;

	return meshID;
}

ID env::AssetManager::CreatePhongMaterial(const std::string& name)
{
	ID materialID = m_commonIDGenerator.GenerateUnique();
	PhongMaterial* material = new PhongMaterial(materialID, "TestMaterial");
	material->PerInstanceData.DiffuseFactor = { 0.8f, 0.4f, 0.4f };
	material->PerInstanceData.AmbientFactor = { 0.4f, 0.8f, 0.4f };
	material->PerInstanceData.SpecularFactor = { 0.4f, 0.4f, 0.8f };
	material->PerInstanceData.SpecularExponent = 5.0f;

	material->PerInstanceBufferLayout = {
		{ "DiffuseFactor", ShaderDataType::Float3 },
		{ "Padding1", ShaderDataType::Float },
		{ "AmbientFactor", ShaderDataType::Float3 },
		{ "Padding2", ShaderDataType::Float },
		{ "SpecularFactor", ShaderDataType::Float3 },
		{ "SpecularExponent", ShaderDataType::Float },
	};

	m_materials[materialID] = material;

	return materialID;
}

ID env::AssetManager::LoadMesh(const std::string& name, const std::string& filePath)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filePath, 0/*
		aiProcess_MakeLeftHanded
		| aiProcess_GenUVCoords
		| aiProcess_FlipUVs*/);

	if (!scene && scene->HasMeshes())
		return ID_ERROR;

	int numVertices = 0;
	int numIndices = 0;

	for (int i = 0; i < scene->mNumMeshes; i++) {
		numVertices += scene->mMeshes[i]->mNumVertices;
		const aiMesh* mesh = scene->mMeshes[i];
		for (int j = 0; j < mesh->mNumFaces; j++) {
			numIndices += mesh->mFaces[j].mNumIndices;
		}
	}

	struct Vertex
	{
		struct {
			float x, y, z;
		} Position;

		struct {
			float x, y, z;
			
		} Normal;

		struct {
			float u, v;
		} Texcoord;
	};

	std::vector<Vertex> vertices(numVertices);
	std::vector<UINT> indices(numIndices);

	int nextVertex = 0;
	int nextIndex = 0;

	int meshVertexOffset = 0;

	for (int i = 0; i < scene->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[i];

		for (int j = 0; j < mesh->mNumVertices; j++) {
			const aiVector3D& position = mesh->mVertices[j];
			const aiVector3D& normal = mesh->mNormals[j];
			const aiVector3D& texCoord = mesh->mTextureCoords[0][j];

			Vertex& vertex = vertices[nextVertex++];
			vertex.Position = { position.x, position.y, position.z };
			vertex.Normal = { normal.x, normal.y, normal.z };
			vertex.Texcoord = { texCoord.x, texCoord.y };
		}

		for (int j = 0; j < mesh->mNumFaces; j++) {
			const aiFace& face = mesh->mFaces[j];
			for (int k = 0; k < face.mNumIndices; k++) {
				indices[nextIndex++] = face.mIndices[k] + meshVertexOffset;
			}
		}

		meshVertexOffset += mesh->mNumVertices;
	}

	ID vertexBuffer = ResourceManager::Get()->CreateBuffer(name + "_vertexBuffer", BufferLayout({
		{ "POSITION", ShaderDataType::Float3 },
		{ "NORMAL", ShaderDataType::Float3 },
		{ "TEXCOORD", ShaderDataType::Float2 } },
		vertices.size()),
		BufferBindType::Vertex,
		vertices.data());

	ID indexBuffer = ResourceManager::Get()->CreateBuffer(name + "_indexBuffer", BufferLayout({
		{ "INDEX", ShaderDataType::Uint }},
		indices.size()),
		BufferBindType::Index,
		indices.data());

	ID meshID = m_commonIDGenerator.GenerateUnique();
	Mesh* mesh = new Mesh(meshID, name);
	mesh->VertexBuffer = vertexBuffer;
	mesh->NumVertices = (int)vertices.size();
	mesh->IndexBuffer = indexBuffer;
	mesh->NumIndices = (int)indices.size();
	m_meshes[meshID] = mesh;

	return meshID;
}
