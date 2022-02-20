#pragma once
#include "envision\envpch.h"

#define REGISTER_RESOURCE(resourceName, resourceType) resourceName(const std::string name, ID resourceID) : env::Resource(name, resourceType, resourceID) {}

namespace env
{
	enum class ResourceType
	{
		UNKNOWN = 0,
		BUFFER,
		BUFFER_ARRAY,
		SHADER,
		TEXTURE2D,
		TEXTURE2D_ARRAY,
		WINDOW
	};

	enum class ElementFormat
	{
		UNKNOWN = 0,
		
		// int
		RGB_UINT_NORM_32BIT,
		RGBA_UINT_NORM_32BIT,
		
		// float
		RGB_FLOAT_32BIT,
		RGBA_FLOAT_32BIT,
	};

	enum class ShaderType
	{
		UNKNOWN = 0,

		// Vertex pipeline
		Vertex = 1,
		Hull = 2,
		Domain = 4,
		Geometry = 8,
		Pixel = 16,

		Compute = 32,
	};

	class env::ResourceManager;
	struct Resource
	{
		const std::string Name = "Unknown";
		const ResourceType Type = ResourceType::UNKNOWN;
		const ID ResourceID = ID_ERROR;

	protected:
		Resource(const std::string& name, const ResourceType type, const ID resourceID) :
			Name(name), Type(type), ResourceID(resourceID) {}
		friend class env::ResourceManager;
	};

	struct Buffer : public Resource
	{
		REGISTER_RESOURCE(Buffer, ResourceType::BUFFER)
		size_t ByteWidth = 0;
	};

	struct BufferArray : public Resource
	{
		REGISTER_RESOURCE(BufferArray, ResourceType::BUFFER_ARRAY)
		size_t ByteWidth = 0;
		size_t ElementStride = 0;
		int ElementCount = 0;
	};

	struct Shader : public Resource
	{
		REGISTER_RESOURCE(Shader, ResourceType::SHADER)
		std::string FilePath;
		ShaderType Type;
	};

	struct Texture2D : public Resource
	{
		REGISTER_RESOURCE(Texture2D, ResourceType::TEXTURE2D)
		int Width = 0;
		int Height = 0;
		int ChannelCount = 0;
	};

	struct Texture2DArray : public Resource
	{
		REGISTER_RESOURCE(Texture2DArray, ResourceType::TEXTURE2D_ARRAY)
		int Width = 0;
		int Height = 0;
		int ChannelCount = 0;
		int TextureCount = 0;
	};

	struct Window : public Resource
	{
		REGISTER_RESOURCE(Window, ResourceType::WINDOW)
		virtual void GetWidth() = 0;
		virtual void GetHeight() = 0;
		virtual void Update() = 0;
	};
}