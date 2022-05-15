#pragma once
#include "envision/envpch.h"

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

		int NumVertices = 0;
		int NumIndices = 0;

		Mesh(const ID resourceID, const std::string& name) :
			Asset(resourceID, name, AssetType::Mesh) {}
	};

	struct Material : public Asset
	{
		// TODO: Is this needed?
		// Surely for custom materials?

		Material(const ID resourceID, const std::string& name) :
			Asset(resourceID, name, AssetType::Material) {}
	};

	struct PhongMaterial : public Material
	{
		ID DiffuseMap = ID_ERROR;
		ID AmbientMap = ID_ERROR;
		ID SpecularMap = ID_ERROR;

		float SpecularExponent = 0;

		// TODO: Change to float3
		struct {
			float X = 0;
			float Y = 0;
			float Z = 0;
		} DiffuseFactor;

		struct {
			float X = 0;
			float Y = 0;
			float Z = 0;
		} AmbientFactor;

		struct {
			float X = 0;
			float Y = 0;
			float Z = 0;
		} SpecularFactor;

		PhongMaterial(const ID resourceID, const std::string& name) :
			Material(resourceID, name) {}
	};
}