#pragma once
#include "envision/core/idGenerator.h"
#include "envision/resource/resource.h"
#include "envision/core/Window.h"

namespace env
{
	class Application;

	class ResourceManager
	{
		void Initialize(env::IDGenerator* idGenerator);
		friend class Application;

	protected:

		env::IDGenerator* m_IDGenerator = nullptr;

		std::unordered_map<ID, Buffer*> m_buffers;
		std::unordered_map<ID, BufferArray*> m_bufferArrays;
		std::unordered_map<ID, Shader*> m_shaders;
		std::unordered_map<ID, Texture2D*> m_texture2Ds;
		std::unordered_map<ID, Texture2DArray*> m_texture2DArrays;
		std::unordered_map<ID, WindowTarget*> m_windowTargets;

	public:

		ResourceManager();
		virtual ~ResourceManager() = default;

	public:

		Buffer* GetBuffer(ID resourceID);
		BufferArray* GetBufferArray(ID resourceID);
		Shader* GetShader(ID resourceID);
		Texture2D* GetTexture2D(ID resourceID);
		Texture2DArray* GetTexture2DArray(ID resourceID);
		WindowTarget* GetWindowTarget(ID resourceID);
	
	public:

		virtual ID CreateBuffer(const std::string& name, const UpdatePattern update, const AccessPattern access, const size_t byteWidth) = 0;
		virtual ID CreateBufferArray(const std::string& name, const UpdatePattern update, const AccessPattern access, const size_t byteWidth, const size_t elementStride, const int elementCount) = 0;
		virtual ID CreateShader(const std::string& name, const std::string& FilePath, const ShaderType type) = 0;
		virtual ID CreateTexture2D(const std::string& name, const UpdatePattern update, const AccessPattern access, const int width, const int height, const ElementFormatType elementFormat) = 0;
		virtual ID CreateTexture2DArray(const std::string& name, const UpdatePattern update, const AccessPattern access, const int width, const int height, const ElementFormatType elementFormat, const int textureCount) = 0;
		virtual ID CreateWindowTarget(const std::string& name, env::Window* window, float startXFactor = 0.f, float startYFactor = 0.f, float widthFactor = 1.f, float heightFactor = 1.f) = 0;
	};

	// Defined in PlatformAdapter.cpp
	ResourceManager* CreateResourceManager();
}