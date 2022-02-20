#pragma once
#include "envision\core\idGenerator.h"
#include "envision\resource\resource.h"

namespace env
{
	class ResourceManager
	{
	protected:

		env::IDGenerator m_IDGenerator;

		std::unordered_map<ID, Buffer*> m_buffers;
		std::unordered_map<ID, BufferArray*> m_bufferArrays;
		std::unordered_map<ID, Shader*> m_shaders;
		std::unordered_map<ID, Texture2D*> m_texture2Ds;
		std::unordered_map<ID, Texture2DArray*> m_texture2DArrays;
		std::unordered_map<ID, Window*> m_windows;

	public:

		ResourceManager() = default;
		~ResourceManager() = default;

	public:

		Buffer* GetBuffer(ID resourceID);
		BufferArray* GetBufferArray(ID resourceID);
		Shader* GetShader(ID resourceID);
		Texture2D* GetTexture2D(ID resourceID);
		Texture2DArray* GetTexture2DArray(ID resourceID);
		Window* GetWindow(ID resourceID);
	
	public:

		virtual Buffer* CreateBuffer() = 0;
		virtual BufferArray* CreateBufferArray() = 0;
		virtual Shader* CreateShader() = 0;
		virtual Texture2D* CreateTexture2D() = 0;
		virtual Texture2DArray* CreateTexture2DArray() = 0;
		virtual Window* CreateWindow() = 0;
	};
}