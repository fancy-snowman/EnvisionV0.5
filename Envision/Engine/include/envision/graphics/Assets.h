#pragma once
#include "envision/envpch.h"
#include "envision/resource/BufferLayout.h"

namespace env
{
	enum class AssetType
	{
		UKNOWN = 0,

		Mesh,
		Material,
	};

	struct Asset
	{
		const ID ResourceID = ID_ERROR;
		const std::string Name = "Unknown";
		const AssetType Type = AssetType::UKNOWN;

	protected:
		Asset(const ID resourceID, const std::string& name, const AssetType type) :
			ResourceID(resourceID), Name(name), Type(type) {}
	};

	struct Mesh : public Asset
	{
		ID VertexBuffer = ID_ERROR;
		ID IndexBuffer = ID_ERROR;

		UINT NumVertices = 0;
		UINT NumIndices = 0;

		UINT OffsetVertices = 0;
		UINT OffsetIndices = 0;

		Mesh(const ID resourceID, const std::string& name) :
			Asset(resourceID, name, AssetType::Mesh) {}
	};

	struct Material : public Asset
	{
		Float3 AmbientFactor;
		Float3 DiffuseFactor;
		Float3 SpecularFactor;
		float Shininess;

		ID DiffuseMap = ID_ERROR;
		ID AmbientMap = ID_ERROR;
		ID SpecularMap = ID_ERROR;

		Material(const ID resourceID, const std::string& name) :
			Asset(resourceID, name, AssetType::Material) {}
	};
}