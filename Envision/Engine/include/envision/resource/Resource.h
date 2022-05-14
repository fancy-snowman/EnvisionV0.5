#pragma once
#include "envision\envpch.h"
#include "envision\core\EventBusObject.h"
 
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
		WINDOW,
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

		// Compute pipeline
		Compute = 32,
	};

	enum class FrameRWPatternType
	{
		UNKNOWN = 0,

		CPU_READ_NEVER = 1,
		CPU_READ_ONCE = 2,
		CPU_READ_MULTIPLE = 4,

		GPU_READ_NEVER = 16,
		GPU_READ_ONCE = 32,
		GPU_READ_MULTIPLE = 64,
	};

	class ResourceManager;

	struct Resource
	{
		const ID ResourceID = ID_ERROR;
		const std::string Name = "Unknown";
		const FrameRWPatternType FrameRWPattern;
		const ResourceType Type = ResourceType::UNKNOWN;

	protected:
		Resource(const ID resourceID,
			const std::string& name,
			const FrameRWPatternType frameRWPattern,
			const ResourceType type) :
			ResourceID(resourceID),
			Name(name),
			FrameRWPattern(frameRWPattern),
			Type(type)  {}
		friend class env::ResourceManager;
	};

	struct Buffer : public Resource
	{
		const size_t ByteWidth;

	protected:
		Buffer(const ID resourceID,
			const std::string& name,
			const FrameRWPatternType frameRWPattern,
			const size_t byteWidth) :
			ByteWidth(byteWidth),
			Resource(resourceID, name, frameRWPattern, ResourceType::BUFFER) {}
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
			const FrameRWPatternType frameRWPattern,
			const size_t byteWidth,
			const size_t elementStride,
			const int elementCount) :
			ByteWidth(byteWidth),
			ElementStride(elementStride),
			ElementCount(elementCount),
			Resource(resourceID, name, frameRWPattern, ResourceType::BUFFER_ARRAY) {}
	};

	struct Shader : public Resource
	{
		const std::string FilePath;
		const ShaderType Type;

	protected:
		Shader(const ID resourceID,
			const std::string& name,
			const FrameRWPatternType frameRWPattern,
			const std::string& filePath,
			const ShaderType shaderType) :
			FilePath(filePath),
			Type(shaderType),
			Resource(resourceID, name, frameRWPattern, ResourceType::SHADER) {}
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
			const FrameRWPatternType frameRWPattern,
			const int width,
			const int height,
			const ElementFormatType elementFormat) :
			Width(width),
			Height(height),
			ElementFormat(elementFormat),
			Resource(resourceID, name, frameRWPattern, ResourceType::TEXTURE2D) {}
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
			const FrameRWPatternType frameRWPattern,
			const int width,
			const int height,
			const ElementFormatType elementFormat,
			const int textureCount) :
			Width(width),
			Height(height),
			ElementFormat(elementFormat),
			TextureCount(textureCount),
			Resource(resourceID, name, frameRWPattern, ResourceType::TEXTURE2D_ARRAY) {}
		friend class env::ResourceManager;
	};

	struct Window : public Resource
	{
		virtual void GetWidth() = 0;
		virtual void GetHeight() = 0;
		virtual void Update() = 0;
		EventBusObject EventBus;

	protected:
		Window(const ID resourceID,
			const std::string& name,
			const FrameRWPatternType frameRWPattern,
			EventBusObject eventBus) :
			EventBus(eventBus),
			Resource(resourceID, name, frameRWPattern, ResourceType::WINDOW) {}
		friend class env::ResourceManager;
	};
}