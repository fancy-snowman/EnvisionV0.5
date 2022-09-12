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
	const float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_defaultTexture_white = ResourceManager::Get()->CreateTexture2D("DefaultWhite1x1", 1, 1, DXGI_FORMAT_R32G32B32A32_FLOAT, TextureBindType::ShaderResource, (void*)white);
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
		}, (UINT)vertices.size()),
		BufferBindType::Vertex,
		vertices.data());

	ID indexBuffer = ResourceManager::Get()->CreateBuffer("IndexBuffer",
		BufferLayout({
			{ "index", ShaderDataType::Uint },
		}, (UINT)indices.size()),
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

ID env::AssetManager::CreateMesh(const std::string& name, void* vertices, const BufferLayout& vertexBufferLayout, void* indices, UINT numIndices)
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

ID env::AssetManager::CreateMesh(const std::string& name, ID vertexBuffer, UINT offsetVertices, UINT numVertices, ID indexBuffer, UINT offsetIndices, UINT numIndices)
{
	ID meshID = m_commonIDGenerator.GenerateUnique();
	Mesh* mesh = new Mesh(meshID, name);
	mesh->VertexBuffer = vertexBuffer;
	mesh->IndexBuffer = indexBuffer;
	mesh->OffsetVertices = offsetVertices;
	mesh->OffsetIndices = offsetIndices;
	mesh->NumVertices = numVertices;
	mesh->NumIndices = (int)numIndices;
	
	m_meshes[meshID] = mesh;

	return meshID;
}

ID env::AssetManager::CreatePhongMaterial(const std::string& name, Float3 ambient, Float3 diffuse, Float3 specular, float shininess)
{
	ID materialID = m_commonIDGenerator.GenerateUnique();
	Material* material = new Material(materialID, name);
	material->AmbientFactor = ambient;
	material->DiffuseFactor = diffuse;
	material->SpecularFactor = specular;
	material->Shininess = shininess;

	m_materials[materialID] = material;

	return materialID;
}

ID env::AssetManager::CreatePhongMaterial(const std::string& name, Float3 ambient, Float3 diffuse, Float3 specular, float shininess,
	const std::string& ambientMap, const std::string& diffuseMap, const std::string& specularMap)
{
	ID materialID = m_commonIDGenerator.GenerateUnique();
	Material* material = new Material(materialID, name);
	material->AmbientFactor = ambient;
	material->DiffuseFactor = diffuse;
	material->SpecularFactor = specular;
	material->Shininess = shininess;

	// Set default maps
	material->AmbientMap = m_defaultTexture_white;
	material->DiffuseMap = m_defaultTexture_white;
	material->SpecularMap = m_defaultTexture_white;

	// Override default maps if possible
	if (!ambientMap.empty()) {
		int width;
		int height;
		float* data = stbi_loadf(ambientMap.c_str(), &width, &height, nullptr, 4);
		material->AmbientMap = ResourceManager::Get()->CreateTexture2D(material->Name + "_ambient",
			width,
			height,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			TextureBindType::ShaderResource,
			data);
	}

	if (!diffuseMap.empty()) {
		int width;
		int height;
		float* data = stbi_loadf(diffuseMap.c_str(), &width, &height, nullptr, 4);
		material->DiffuseMap = ResourceManager::Get()->CreateTexture2D(material->Name + "_diffuse",
			width,
			height,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			TextureBindType::ShaderResource,
			data);
	}

	if (!specularMap.empty()) {
		int width;
		int height;
		float* data = stbi_loadf(specularMap.c_str(), &width, &height, nullptr, 4);
		material->SpecularMap = ResourceManager::Get()->CreateTexture2D(material->Name + "_specular",
			width,
			height,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			TextureBindType::ShaderResource,
			data);
	}

	m_materials[materialID] = material;

	return materialID;
}

ID env::AssetManager::LoadMesh(const std::string& name, const std::string& filePath)
{
	Assimp::Importer importer;

	//const aiScene* scene = importer.ReadFile(filePath, 
	//	aiProcess_MakeLeftHanded
	//	| aiProcess_GenUVCoords
	//	| aiProcess_FlipUVs);

	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_ConvertToLeftHanded);

	if (!scene && scene->HasMeshes())
		return ID_ERROR;

	int numVertices = 0;
	int numIndices = 0;

	for (size_t i = 0; i < scene->mNumMeshes; i++) {
		numVertices += scene->mMeshes[i]->mNumVertices;
		const aiMesh* mesh = scene->mMeshes[i];
		for (size_t j = 0; j < mesh->mNumFaces; j++) {
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

	for (size_t i = 0; i < scene->mNumMeshes; i++) {
		const aiMesh* mesh = scene->mMeshes[i];

		for (size_t j = 0; j < mesh->mNumVertices; j++) {
			const aiVector3D& position = mesh->mVertices[j];
			const aiVector3D& normal = mesh->mNormals[j];
			const aiVector3D& texCoord = mesh->mTextureCoords[0][j];

			Vertex& vertex = vertices[nextVertex++];
			vertex.Position = { position.x, position.y, position.z };
			vertex.Normal = { normal.x, normal.y, normal.z };
			vertex.Texcoord = { texCoord.x, texCoord.y };
		}

		for (size_t j = 0; j < mesh->mNumFaces; j++) {
			const aiFace& face = mesh->mFaces[j];
			for (size_t k = 0; k < face.mNumIndices; k++) {
				indices[nextIndex++] = face.mIndices[k] + meshVertexOffset;
			}
		}

		meshVertexOffset += mesh->mNumVertices;
	}

	ID vertexBuffer = ResourceManager::Get()->CreateBuffer(name + "_vertexBuffer", BufferLayout({
		{ "POSITION", ShaderDataType::Float3 },
		{ "NORMAL", ShaderDataType::Float3 },
		{ "TEXCOORD", ShaderDataType::Float2 } },
		(UINT)vertices.size()),
		BufferBindType::Vertex,
		vertices.data());

	ID indexBuffer = ResourceManager::Get()->CreateBuffer(name + "_indexBuffer", BufferLayout({
		{ "INDEX", ShaderDataType::Uint }},
		(UINT)indices.size()),
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

int env::AssetManager::GetNumMeshes() const
{
	return static_cast<int>(m_meshes.size());
}

int env::AssetManager::GetNumMaterials() const
{
	return static_cast<int>(m_materials.size());
}

void env::AssetManager::GetMeshes(std::vector<Mesh*>& outMeshes) const
{
	outMeshes.clear();
	outMeshes.reserve(m_meshes.size());
	for (auto& idMesh : m_meshes)
		outMeshes.push_back(idMesh.second);
}

void env::AssetManager::GetMaterials(std::vector<Material*>& outMaterials) const
{
	outMaterials.clear();
	outMaterials.reserve(m_materials.size());
	for (auto& idMaterial : m_materials)
		outMaterials.push_back(idMaterial.second);
}

void env::AssetManager::GetMeshIDs(std::vector<ID>& outIDs) const
{
	outIDs.clear();
	outIDs.reserve(m_meshes.size());
	for (auto& idMesh : m_meshes)
		outIDs.push_back(idMesh.first);
}

void env::AssetManager::GetMaterialIDs(std::vector<ID>& outIDs) const
{
	outIDs.clear();
	outIDs.reserve(m_materials.size());
	for (auto& idMaterial : m_materials)
		outIDs.push_back(idMaterial.first);
}
