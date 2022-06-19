#include "envision/envpch.h"
#include "envision/core/Scene.h"
#include "envision/graphics/AssetManager.h"
#include "envision/resource/ShaderDataType.h"

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

void env::Scene::LoadScene(const std::string& name, const std::string& filePath)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_ConvertToLeftHanded);

	struct Submesh {
		std::string Name = "Unknown";
		ID VertexBuffer = ID_ERROR;
		UINT OffsetVertices = 0;
		UINT NumVertices = 0;
		ID IndexBuffer = ID_ERROR;
		UINT OffsetIndices = 0;
		UINT NumIndices = 0;
	};

	// All of the loaded meshes will share the same vertex- and index buffer.
	// This vector will store offset and counts per "sub mesh" within these buffers.
	std::vector<Submesh> submeshes(scene->mNumMeshes);

	// Query size of vertex buffer and index buffer
	int numVerticesTotal = 0;
	int numIndicesTotal = 0;
	for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
		aiMesh* mesh = scene->mMeshes[meshIndex];

		Submesh& submesh = submeshes[meshIndex];

		submesh.Name = mesh->mName.C_Str();
		submesh.NumVertices = mesh->mNumVertices;
		submesh.OffsetVertices = numVerticesTotal;
		submesh.OffsetIndices = numIndicesTotal;

		numVerticesTotal += submesh.NumVertices;

		int numIndices = 0;
		for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
			aiFace& face = mesh->mFaces[faceIndex];
			numIndices += face.mNumIndices;
		}

		submesh.NumIndices = numIndices;
		numIndicesTotal += numIndices;
	}

	// Construct the intermediate buffer data for the buffers
	std::vector<VertexType> vertexData(numVerticesTotal);
	std::vector<IndexType> indexData(numIndicesTotal);

	// Create all meshes
	for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {

		aiMesh* mesh = scene->mMeshes[meshIndex];	
		Submesh& submesh = submeshes[meshIndex];

		// Query vertices and indices to the intermediate buffers
		for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
			const aiVector3D& position = mesh->mVertices[vertexIndex];
			const aiVector3D& normal = mesh->mNormals[vertexIndex];
			const aiVector3D& texCoord = mesh->mTextureCoords[0][vertexIndex];

			VertexType& vertex = vertexData[submesh.OffsetVertices + vertexIndex];
			vertex.Position = { position.x, position.y, position.z };
			vertex.Normal = { normal.x, normal.y, normal.z };
			vertex.Texcoord = { texCoord.x, texCoord.y };
		}

		int nextIndex = 0;
		for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
			const aiFace& face = mesh->mFaces[faceIndex];
			for (unsigned int indexIndex = 0; indexIndex < face.mNumIndices; indexIndex++) {
				indexData[submesh.OffsetIndices + (nextIndex++)] = face.mIndices[indexIndex];
			}
		}
	}

	ID vertexBuffer = ResourceManager::Get()->CreateBuffer(name + "_vertexBuffer",
		BufferLayout({
			{ "POSITION", ShaderDataType::Float3},
			{ "NORMAL", ShaderDataType::Float3},
			{ "TEXCOORD", ShaderDataType::Float2} },
			numVerticesTotal),
		BufferBindType::Vertex,
		vertexData.data());

	ID indexBuffer = ResourceManager::Get()->CreateBuffer(name + "_indexBuffer",
		BufferLayout(
			{{ "index", ShaderDataType::Uint }},
			numIndicesTotal),
		BufferBindType::Index,
		indexData.data());


	// Construct the uber mesh and store its ID, all entities will store this ID
	// and which sections theis specifik mesh starts it the common vertex and index
	// buffers.
	/*ID meshID = AssetManager::Get()->CreateMesh(name,
		vertexData.data(),
		BufferLayout({
			{ "POSITION", ShaderDataType::Float3},
			{ "NORMAL", ShaderDataType::Float3},
			{ "TEXCOORD", ShaderDataType::Float2} },
			numVerticesTotal),
			indexData.data(),
			numIndicesTotal);*/

	// Create all entities
	int numEntities = 0;
	auto entityFactory = [&](aiNode* node, const Float4x4& parentTransform, auto&& entityFactory) -> void {

		Float4x4 transformMatrix;
		memcpy_s(&transformMatrix, sizeof(Float4x4), &node->mTransformation, sizeof(node->mTransformation));
		transformMatrix = transformMatrix.Transpose();

		transformMatrix *= parentTransform;
		//transformMatrix = parentTransform * transformMatrix;

		//transformMatrix = DirectX::XMMatrixTranslation(
		//	node->mTransformation.a4 + parentTransform._14,
		//	node->mTransformation.b4 + parentTransform._24,
		//	node->mTransformation.c4 + parentTransform._34);

		TransformComponent transformInfo;
		transformInfo.Transformation = Transform(transformMatrix);

		for (unsigned int meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++) {

			Submesh& submesh = submeshes[node->mMeshes[meshIndex]];
			ID meshID = AssetManager::Get()->CreateMesh(submesh.Name,
				vertexBuffer,
				submesh.OffsetVertices,
				submesh.NumVertices,
				indexBuffer,
				submesh.OffsetIndices,
				submesh.NumIndices);

			RenderComponent renderInfo;
			renderInfo.Mesh = meshID;

			entt::entity entity = m_registry.create();
			m_registry.emplace<TransformComponent>(entity, transformInfo);
			m_registry.emplace<RenderComponent>(entity, renderInfo);
			numEntities++;
		}

		for (unsigned int childIndex = 0; childIndex < node->mNumChildren; childIndex++) {
			entityFactory(node->mChildren[childIndex], transformMatrix, entityFactory);
		}
	};

	entityFactory(scene->mRootNode, Float4x4::Identity, entityFactory);
}
