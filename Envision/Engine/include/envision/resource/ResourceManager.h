#pragma once
#include "envision/envpch.h"
#include "envision/core/Window.h"
#include "envision/core/IDGenerator.h"
#include "envision/core/DescriptorAllocator.h"
#include "envision/core/CommandList.h"
#include "envision/graphics/Shader.h"
#include "envision/graphics/RootSignature.h"
#include "envision/resource/Resource.h"

namespace env
{
	// Singleton
	class ResourceManager
	{
	private:

		IDGenerator& m_commonIDGenerator;

		std::unordered_map<ID, BufferArray*> m_buffersArrays;
		std::unordered_map<ID, Buffer*> m_buffers;
		std::unordered_map<ID, Texture2D*> m_texture2Ds;
		std::unordered_map<ID, Texture2DArray*> m_texture2DArrays;
		std::unordered_map<ID, PipelineState*> m_pipelineStates;
		std::unordered_map<ID, WindowTarget*> m_targets;

		DescriptorAllocator m_CBVAllocator;
		DescriptorAllocator m_SRVAllocator;
		DescriptorAllocator m_UAVAllocator;
		DescriptorAllocator m_SamplerAllocator;
		DescriptorAllocator m_RTVAllocator;
		DescriptorAllocator m_DSVAllocator;

		DirectList* m_transitionList;
		CopyList* m_copyList;

		Buffer m_uploadBuffer;

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

		Resource* GetResourceNonConst(ID resourceID);
		void AdjustViewportAndScissorRect(WindowTarget& target, const Window& window);

		D3D12_CPU_DESCRIPTOR_HANDLE CreateCBV(Resource* resource);
		D3D12_CPU_DESCRIPTOR_HANDLE CreateSRV(Resource* resource);
		D3D12_CPU_DESCRIPTOR_HANDLE CreateUAV(Resource* resource);
		D3D12_CPU_DESCRIPTOR_HANDLE CreateSampler(Resource* resource);
		D3D12_CPU_DESCRIPTOR_HANDLE CreateRTV(Resource* resource);
		D3D12_CPU_DESCRIPTOR_HANDLE CreateDSV(Resource* resource);

	public:

		ID CreateBufferArray(const std::string& name, const BufferLayout& layout, BufferBindType bindType = BufferBindType::Unknown, void* initialData = nullptr);
		ID CreateBuffer(const std::string& name, const BufferLayout& layout, BufferBindType bindType = BufferBindType::Unknown, void* initialData = nullptr);
		ID CreateTexture2D(const std::string& name, int width, int height, DXGI_FORMAT format, TextureBindType bindType = TextureBindType::Unknown, void* initialData = nullptr);
		ID CreateTexture2D(const std::string& name, TextureBindType bindType, ID3D12Resource* existingTexture);
		ID CreateTexture2DArray(const std::string& name, int numTextures, int width, int height, DXGI_FORMAT format, void* initialData = nullptr);
		ID CreatePipelineState(const std::string& name, std::initializer_list<ShaderDesc> shaderDescs, bool useInputLayout, const RootSignature& rootSignature);
		ID CreateWindowTarget(const std::string& name, Window* window, float startXFactor = 0.f, float startYFactor = 0.f, float widthFactor = 1.f, float heightFactor = 1.f);

		BufferArray* GetBufferArray(ID resourceID);
		Buffer* GetBuffer(ID resourceID);
		Texture2D* GetTexture2D(ID resourceID);
		Texture2DArray* GetTexture2DArray(ID resourceID);
		PipelineState* GetPipelineState(ID resourceID);
		WindowTarget* GetTarget(ID resourceID);
		Resource* GetResource(ID resourceID);

		void UploadBufferData(ID resourceID, void* data, UINT numBytes = 0, UINT destinationOffset = 0);
	};
}