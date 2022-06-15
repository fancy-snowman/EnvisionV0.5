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

void env::Scene::LoadScene(const std::string& filePath)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_ConvertToLeftHanded);

	std::vector<ID> meshIDs(scene->mNumMeshes);

	// Create all meshes
	for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
		aiMesh* mesh = scene->mMeshes[meshIndex];

		// Query size of vertex buffer and index buffer
		int numVertices = mesh->mNumVertices;
		int numIndices = 0;
		for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
			aiFace& face = mesh->mFaces[faceIndex];
			numIndices += face.mNumIndices;
		}

		// Construct the intermediate buffer data for the buffers
		std::vector<VertexType> vertexData(numVertices);
		std::vector<IndexType> indexData(numIndices);

		// Query vertices and indices to the intermediate buffers
		for (unsigned int vertexIndex = 0; vertexIndex < numVertices; vertexIndex++) {
			const aiVector3D& position = mesh->mVertices[vertexIndex];
			const aiVector3D& normal = mesh->mNormals[vertexIndex];
			const aiVector3D& texCoord = mesh->mTextureCoords[0][vertexIndex];

			VertexType& vertex = vertexData[vertexIndex];
			vertex.Position = { position.x, position.y, position.z };
			vertex.Normal = { normal.x, normal.y, normal.z };
			vertex.Texcoord = { texCoord.x, texCoord.y };
		}

		int nextIndex = 0;
		for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
			const aiFace& face = mesh->mFaces[faceIndex];
			for (unsigned int indexIndex = 0; indexIndex < face.mNumIndices; indexIndex++) {
				indexData[nextIndex++] = face.mIndices[indexIndex];
			}
		}

		// Construct the mesh and store its ID
		ID meshID = AssetManager::Get()->CreateMesh(mesh->mName.C_Str(),
			vertexData.data(),
			BufferLayout({
				{ "POSITION", ShaderDataType::Float3},
				{ "NORMAL", ShaderDataType::Float3},
				{ "TEXCOORD", ShaderDataType::Float2} },
				(UINT)vertexData.size()),
			indexData.data(),
			numIndices);
		meshIDs[meshIndex] = meshID;
	}

	// Create all entities
	int numEntities = 0;
	auto entityFactory = [&](aiNode* node, const Float4x4& parentTransform, auto&& entityFactory) -> void {

		Float4x4 transformMatrix;
		//memcpy_s(&transformMatrix, sizeof(Float4x4), &node->mTransformation, sizeof(node->mTransformation));
		//transformMatrix = transformMatrix.Transpose();

		//transformMatrix *= parentTransform;
		//transformMatrix = parentTransform * transformMatrix;

		transformMatrix = DirectX::XMMatrixTranslation(
			node->mTransformation.a4 + parentTransform._14,
			node->mTransformation.b4 + parentTransform._24,
			node->mTransformation.c4 + parentTransform._34);

		TransformComponent transformInfo;
		transformInfo.Transformation = Transform(transformMatrix);

		for (unsigned int meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++) {

			RenderComponent renderInfo;
			renderInfo.Mesh = meshIDs[node->mMeshes[meshIndex]];

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
