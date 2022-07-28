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

	struct MaterialInfo
	{
		std::string Name = "Unknown";
		Float3 Ambient = Float3::Zero;
		Float3 Diffuse = Float3::Zero;
		Float3 Specular = Float3::Zero;
		float Shininess = 1.f;
		std::string AmbientMapPath;
		std::string DiffuseMapPath;
		std::string SpecularMapPath;
	};

	std::vector<ID> materialIDs(scene->mNumMaterials);
	for (unsigned int materialIndex = 0; materialIndex < scene->mNumMaterials; materialIndex++) {
		aiMaterial* material = scene->mMaterials[materialIndex];

		std::cout << material->GetName().C_Str() << std::endl;

		int numAmbientTextures = material->GetTextureCount(aiTextureType_AMBIENT);
		int numAmbientOcclusion = material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION);
		int numBaseColor = material->GetTextureCount(aiTextureType_BASE_COLOR);
		int numClearCoat = material->GetTextureCount(aiTextureType_CLEARCOAT);
		int numDiffuseTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
		int numDiffuseRoughness = material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS);
		int numDisplacementTextures = material->GetTextureCount(aiTextureType_DISPLACEMENT);
		int numEmissionColor = material->GetTextureCount(aiTextureType_EMISSION_COLOR);
		int numEmissiveTextures = material->GetTextureCount(aiTextureType_EMISSIVE);
		int numHeightTextures = material->GetTextureCount(aiTextureType_HEIGHT);
		int numLightmapTextures = material->GetTextureCount(aiTextureType_LIGHTMAP);
		int numMetalnessTextures = material->GetTextureCount(aiTextureType_METALNESS);
		int numNormalsTextures = material->GetTextureCount(aiTextureType_NORMALS);
		int numNormalCamera = material->GetTextureCount(aiTextureType_NORMAL_CAMERA);
		int numOpacityTextures = material->GetTextureCount(aiTextureType_OPACITY);
		int numReflectionTextures = material->GetTextureCount(aiTextureType_REFLECTION);
		int numSheenTextures = material->GetTextureCount(aiTextureType_SHEEN);
		int numShininessTextures = material->GetTextureCount(aiTextureType_SHININESS);
		int numSpecularTextures = material->GetTextureCount(aiTextureType_SPECULAR);
		int numTransmissionTextures = material->GetTextureCount(aiTextureType_TRANSMISSION);

		if (numDiffuseTextures > 0) {
			aiString path;
			aiReturn ret = material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &path);
			int a = 0;
		}



		aiString name;
		aiColor3D ambient;
		aiColor3D diffuse;
		aiColor3D specular;
		ai_real shininess;	

		if (material->Get(AI_MATKEY_NAME, name) != AI_SUCCESS)
			name = "Unkown";
		if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambient) != AI_SUCCESS)
			ambient = { 0.0f, 0.0f, 0.0f };
		if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) != AI_SUCCESS)
			diffuse = { 0.0f, 0.0f, 0.0f };
		if (material->Get(AI_MATKEY_COLOR_SPECULAR, specular) != AI_SUCCESS)
			specular = { 0.0f, 0.0f, 0.0f };
		if (material->Get(AI_MATKEY_SHININESS, shininess) != AI_SUCCESS)
			shininess = 0.0f;

		aiString aiAmbientMap;
		aiString aiDiffuseMap;
		aiString aiSpecularMap;

		material->GetTexture(aiTextureType::aiTextureType_AMBIENT, 0, &aiAmbientMap);
		material->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &aiDiffuseMap);
		material->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &aiSpecularMap);

		MaterialInfo info;
		info.Name = name.C_Str();
		info.Ambient = { ambient.r, ambient.g, ambient.b };
		info.Diffuse = { diffuse.r, diffuse.g, diffuse.b };
		info.Specular = { specular.r, specular.g, specular.b };
		info.Shininess = shininess;

		{
			size_t subSize = filePath.rfind('/');

			if (subSize == std::string::npos) {
				subSize = filePath.size() - 1;
			}

			std::string directory = filePath.substr(0, subSize);

			if (directory.size() && directory.back() != '/') {
				directory += '/';
			}

			info.AmbientMapPath = aiAmbientMap.C_Str();
			info.DiffuseMapPath = aiDiffuseMap.C_Str();
			info.SpecularMapPath = aiSpecularMap.C_Str();

			{
				// Replace all occurrences of "\\" with "/"
				size_t pos = 0;

				while ((pos = info.AmbientMapPath.find("\\")) != std::string::npos) {
					auto it = info.AmbientMapPath.begin() + pos;
					info.AmbientMapPath.replace(it, it + 2, "/");
				}

				while ((pos = info.DiffuseMapPath.find("\\")) != std::string::npos) {
					auto it = info.DiffuseMapPath.begin() + pos;
					info.DiffuseMapPath.replace(it, it + 1, "/");
				}

				while ((pos = info.SpecularMapPath.find("\\")) != std::string::npos) {
					auto it = info.SpecularMapPath.begin() + pos;
					info.SpecularMapPath.replace(it, it + 2, "/");
				}
			}

			if (info.AmbientMapPath != "") info.AmbientMapPath = directory + info.AmbientMapPath;
			if (info.DiffuseMapPath != "") info.DiffuseMapPath = directory + info.DiffuseMapPath;
			if (info.SpecularMapPath != "") info.SpecularMapPath = directory + info.SpecularMapPath;
		}

		std::cout << "\tCOLOR ambient: (" << ambient.r << ", " << ambient.g << ", " << ambient.b << ")\n";
		std::cout << "\tCOLOR diffuse: (" << diffuse.r << ", " << diffuse.g << ", " << diffuse.b << ")\n";
		std::cout << "\tCOLOR specular: (" << specular.r << ", " << specular.g << ", " << specular.b << ")\n";
		std::cout << "\tCOLOR shininess: " << info.Shininess << "\n";


		materialIDs[materialIndex] = AssetManager::Get()->CreatePhongMaterial(info.Name,
			info.Ambient,
			info.Diffuse,
			info.Specular,
			info.Shininess,
			info.AmbientMapPath,
			info.DiffuseMapPath,
			info.SpecularMapPath);
	}

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

	std::vector<ID> meshes(scene->mNumMeshes);
	for (UINT meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
		Submesh& submesh = submeshes[meshIndex];
		meshes[meshIndex] = AssetManager::Get()->CreateMesh(submesh.Name,
			vertexBuffer,
			submesh.OffsetVertices,
			submesh.NumVertices,
			indexBuffer,
			submesh.OffsetIndices,
			submesh.NumIndices);
	}

	// Create all entities
	int numEntities = 0;
	auto entityFactory = [&](aiNode* node, const Float4x4& parentTransform, auto&& entityFactory) -> void {

		Float4x4 transformMatrix;
		memcpy_s(&transformMatrix, sizeof(Float4x4), &node->mTransformation, sizeof(node->mTransformation));
		transformMatrix = transformMatrix.Transpose();
		transformMatrix *= parentTransform;

		TransformComponent transformInfo;
		transformInfo.Transformation = Transform(transformMatrix);

		for (unsigned int meshIndex = 0; meshIndex < node->mNumMeshes; meshIndex++) {

			ID meshID = meshes[node->mMeshes[meshIndex]];
			UINT materialIndex = scene->mMeshes[node->mMeshes[meshIndex]]->mMaterialIndex;

			RenderComponent renderInfo;
			renderInfo.Mesh = meshID;
			renderInfo.Material = materialIDs[materialIndex];

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
