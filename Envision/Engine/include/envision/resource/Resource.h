#pragma once
#include "envision/envpch.h"
 
namespace env
{
	enum class ElementFormatType
	{
		UNKNOWN = 0,

		// int
		RGB_UINT_NORM_32BIT,
		RGBA_UINT_NORM_32BIT,

		// float
		RGB_FLOAT_32BIT,
		RGBA_FLOAT_32BIT,
	};

	enum class ResourceType
	{
		UNKNOWN = 0,
		BUFFER,
		BUFFER_ARRAY,
		SHADER,
		TEXTURE2D,
		TEXTURE2D_ARRAY,
		WINDOW_TARGET,
	};

	enum class ShaderType
	{
		UNKNOWN = 0,

		// Vertex pipeline
		Vertex      = 1 << 0,
		Hull        = 1 << 1,
		Domain      = 1 << 2,
		Geometry    = 1 << 3,
		Pixel       = 1 << 4,

		// Compute pipeline
		Compute     = 1 << 5,
	};

	enum class UpdatePattern
	{
		UNKNOWN = 0,

		NEVER,
		PER_FRAME,
		PER_PASS,
		PER_OBJECT,
	};

	enum class AccessPattern
	{
		UNKNOWN = 0,

		CPU_READ    = 1 << 0,
		CPU_WRITE   = 1 << 1,
		GPU_READ    = 1 << 2,
		GPU_WRITE   = 1 << 3,

		CPU_READ_WRITE = (int)CPU_READ | (int)CPU_WRITE,
		GPU_READ_WRITE = (int)GPU_READ | (int)GPU_WRITE,

		CPU_WRITE_GPU_READ = (int)CPU_WRITE | (int)GPU_READ,
		CPU_READ_GPU_WRITE = (int)CPU_READ | (int)GPU_WRITE,

		CPU_AND_GPU_READ_WRITE = (int)CPU_READ_WRITE | (int)GPU_READ_WRITE,
	};

	class ResourceManager;

	struct Resource
	{
		const ID ResourceID = ID_ERROR;
		const std::string Name = "Unknown";
		const UpdatePattern Update;
		const AccessPattern Access;
		const ResourceType Type = ResourceType::UNKNOWN;

	protected:
		Resource(const ID resourceID,
			const std::string& name,
			const UpdatePattern update,
			const AccessPattern access,
			const ResourceType type) :
			ResourceID(resourceID),
			Name(name),
			Update(update),
			Access(access),
			Type(type)  {}
		friend class env::ResourceManager;
	};

	struct Buffer : public Resource
	{
		const size_t ByteWidth;

	protected:
		Buffer(const ID resourceID,
			const std::string& name,
			const UpdatePattern update,
			const AccessPattern access,
			const size_t byteWidth) :
			ByteWidth(byteWidth),
			Resource(resourceID, name, update, access, ResourceType::BUFFER) {}
		friend class env::ResourceManager;
	};

	struct BufferArray : public Resource
	{
		size_t ByteWidth = 0;
		size_t ElementStride = 0;
		int ElementCount = 0;
	
	protected:
		BufferArray(const ID resourceID,
			const std::string& name,
			const UpdatePattern update,
			const AccessPattern access,
			const size_t byteWidth,
			const size_t elementStride,
			const int elementCount) :
			ByteWidth(byteWidth),
			ElementStride(elementStride),
			ElementCount(elementCount),
			Resource(resourceID, name, update, access, ResourceType::BUFFER_ARRAY) {}
	};

	struct Shader : public Resource
	{
		const std::string FilePath;
		const ShaderType Type;

	protected:
		Shader(const ID resourceID,
			const std::string& name,
			const std::string& filePath,
			const ShaderType shaderType) :
			FilePath(filePath),
			Type(shaderType),
			Resource(resourceID, name, UpdatePattern::UNKNOWN, AccessPattern::UNKNOWN, ResourceType::SHADER) {}
		friend class env::ResourceManager;
	};

	struct Texture2D : public Resource
	{
		const int Width = 0;
		const int Height = 0;
		const ElementFormatType ElementFormat;

	protected:
		Texture2D(const ID resourceID,
			const std::string& name,
			const UpdatePattern update,
			const AccessPattern access,
			const int width,
			const int height,
			const ElementFormatType elementFormat) :
			Width(width),
			Height(height),
			ElementFormat(elementFormat),
			Resource(resourceID, name, update, access, ResourceType::TEXTURE2D) {}
		friend class env::ResourceManager;
	};

	struct Texture2DArray : public Resource
	{
		const int Width = 0;
		const int Height = 0;
		const ElementFormatType ElementFormat;
		const int TextureCount = 0;

	protected:
		Texture2DArray(const ID resourceID,
			const std::string& name,
			const UpdatePattern update,
			const AccessPattern access,
			const int width,
			const int height,
			const ElementFormatType elementFormat,
			const int textureCount) :
			Width(width),
			Height(height),
			ElementFormat(elementFormat),
			TextureCount(textureCount),
			Resource(resourceID, name, update, access, ResourceType::TEXTURE2D_ARRAY) {}
		friend class env::ResourceManager;
	};

	struct WindowTarget : public Resource
	{
	protected:
		WindowTarget(const ID resourceID,
			const std::string& name) :
			Resource(resourceID, name, UpdatePattern::UNKNOWN, AccessPattern::GPU_WRITE, ResourceType::WINDOW_TARGET) {}
		friend class env::ResourceManager;
	};
}