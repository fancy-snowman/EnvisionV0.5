#pragma once
#include "envision/envpch.h"
#include "envision/resource/Resource.h"
#include "envision/core/IDGenerator.h"
#include "envision/core/DescriptorAllocator.h"

namespace env
{
	// Singleton
	class ResourceManager
	{
	private:

		IDGenerator& m_commonIDGenerator;

		std::unordered_map<ID, BufferArray*> m_buffersArrays;
		std::unordered_map<ID, ConstantBuffer*> m_constantBuffers;
		std::unordered_map<ID, IndexBuffer*> m_indexBuffers;
		std::unordered_map<ID, VertexBuffer*> m_vertexBuffers;
		std::unordered_map<ID, Texture2D*> m_texture2Ds;
		std::unordered_map<ID, Texture2DArray*> m_texture2DArrays;
		std::unordered_map<ID, PipelineState*> m_pipelineStates;
		std::unordered_map<ID, WindowTarget*> m_windowTargets;

		ConstantBuffer m_uploadBuffer;

		DescriptorAllocator m_CBVAllocator;
		DescriptorAllocator m_SRVAllocator;
		DescriptorAllocator m_UAVAllocator;
		DescriptorAllocator m_SamplerAllocator;
		DescriptorAllocator m_RTVAllocator;
		DescriptorAllocator m_DSVAllocator;

	public:

		static ResourceManager* Initialize(IDGenerator& commonIDGenerator);
		static ResourceManager* Get();
		static void Finalize();

	private:

		static ResourceManager* s_instance;

		ResourceManager(IDGenerator& commonIDGenerator);
		~ResourceManager();
	
		ResourceManager(const ResourceManager& other) = delete;
		ResourceManager(const ResourceManager&& other) = delete;
		ResourceManager& operator=(const ResourceManager& other) = delete;
		ResourceManager& operator=(const ResourceManager&& other) = delete;

	public:

		ID CreateBufferArray(const std::string& name, int numBuffers, const BufferLayout& layout, void* initialData = nullptr);
		ID CreateConstantBuffer(const std::string& name, const BufferLayout& layout, void* initialData = nullptr);
		ID CreateIndexBuffer(const std::string& name, int numIndices, void* initialData = nullptr);
		ID CreateVertexBuffer(const std::string& name, int numVertices, const BufferLayout& layout, void* initialData = nullptr);
		ID CreateTexture2D(const std::string& name, int width, int height, TextureLayout layout, void* initialData = nullptr);
		ID CreateTexture2DArray(const std::string& name, int numTextures, int width, int height, TextureLayout layout, void* initialData = nullptr);
		ID CreatePipelineState(const std::string& name, const std::string& shaderPath);
		ID CreateWindowTarget(const std::string& name, HWND window, float startXFactor = 0.f, float startYFactor = 0.f, float widthFactor = 1.f, float heightFactor = 1.f);

		const BufferArray* GetBufferArray(ID resourceID);
		const ConstantBuffer* GetConstantBuffer(ID resourceID);
		const IndexBuffer* GetIndexBuffer(ID resourceID);
		const VertexBuffer* GetVertexBuffer(ID resourceID);
		const Texture2D* GetTexture2D(ID resourceID);
		const Texture2DArray* GetTexture2DArray(ID resourceID);
		const PipelineState* GetPipelineState(ID resourceID);
		const WindowTarget* GetWindowTarget(ID resourceID);
	};
}