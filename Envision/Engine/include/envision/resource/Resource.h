#pragma once
#include "envision/envpch.h"
#include "envision/graphics/Shader.h"
#include "envision/resource/BufferLayout.h"

#define RESOURCE_TYPE(type)\
	static ResourceType GetStaticType() { return ResourceType::type; }\
	ResourceType GetType() override { return ResourceType::type; }\
	type() = default;\
	type(type&& other) = default;

namespace env
{
	enum class ResourceType
	{
		UNKNOWN = 0,

		BufferArray,
		Buffer,
		IndexBuffer,
		VertexBuffer,

		Sampler,

		Texture2D,
		Texture2DArray,

		PipelineState,

		WindowTarget,
	};

	enum class BufferBindType
	{
		Unknown = 0,

		Constant			= 1 << 0,
		Index				= 1 << 1,
		Vertex				= 1 << 2,
		ShaderResource		= 1 << 3,
		UnorderedAccess		= 1 << 4,
		Upload				= 1 << 5,
	};
	BufferBindType operator|(BufferBindType a, BufferBindType b);
	BufferBindType operator&(BufferBindType a, BufferBindType b);
	bool any(BufferBindType type);

	enum class TextureBindType
	{
		Unknown = 0,

		RenderTarget		= 1 << 0,
		ShaderResource		= 1 << 1,
		UnorderedAccess		= 1 << 2,
		DepthStencil		= 1 << 3,
	};
	TextureBindType operator|(TextureBindType a, TextureBindType b);
	TextureBindType operator&(TextureBindType a, TextureBindType b);
	bool any(TextureBindType type);

	struct Resource
	{
		Resource() = default;
		Resource(env::Resource && other) = default;

		virtual ResourceType GetType() = 0;
		virtual UINT GetByteWidth() { return 0; };

		std::string Name;
		ID3D12Resource* Native = nullptr;
		D3D12_RESOURCE_STATES State = D3D12_RESOURCE_STATE_COMMON;
	};

	struct BufferArray : public Resource
	{
		RESOURCE_TYPE(BufferArray)
		UINT GetByteWidth() final { return (UINT)Layout.GetByteWidth() * Layout.GetNumRepetitions(); }
			
		BufferLayout Layout;

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE ShaderResource = { 0 };
			D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccess = { 0 };
		} Views;
	};

	struct Buffer : public Resource
	{
		RESOURCE_TYPE(Buffer)
		UINT GetByteWidth() final { return (UINT)(Layout.GetByteWidth() + 255) & ~255; }

		BufferLayout Layout;
		BufferBindType BindType;

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE Constant = { 0 };
			D3D12_INDEX_BUFFER_VIEW Index = { 0 };
			D3D12_VERTEX_BUFFER_VIEW Vertex = { 0 };
			D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccess = { 0 };
		} Views;
	};

	struct Sampler : public Resource
	{
		RESOURCE_TYPE(Sampler)
		D3D12_SAMPLER_DESC Description;
		D3D12_CPU_DESCRIPTOR_HANDLE View;
	};

	struct Texture2D : public Resource
	{
		RESOURCE_TYPE(Texture2D)
		UINT GetByteWidth() final { return (UINT)(GPUByteWidth); }

		int Width;
		int Height;
		UINT64 CPURowPitch;
		UINT64 GPUByteWidth;
		UINT NumGPURows;
		DXGI_FORMAT Format;

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget = { 0 };
			D3D12_CPU_DESCRIPTOR_HANDLE ShaderResource = { 0 };
			D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccess = { 0 };
			D3D12_CPU_DESCRIPTOR_HANDLE DepthStencil = { 0 };
		} Views;
	};

	struct Texture2DArray : public Resource
	{
		RESOURCE_TYPE(Texture2DArray)
		UINT GetByteWidth() final { return (UINT)(GPUByteWidth); }

		int NumTextures;
		int Width;
		int Height;
		int GPUByteWidth;
		int CPURowPitch;
		DXGI_FORMAT Format;

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE ShaderResource = { 0 };
		} Views;
	};

	struct PipelineState : public Resource
	{
		RESOURCE_TYPE(PipelineState)

		ShaderStage ShaderStages = ShaderStage::Unknown;
		ID3D12RootSignature* RootSignature = nullptr;
		ID3D12PipelineState* State = nullptr;
	};

	class Window;

	struct WindowTarget : public Resource
	{
		RESOURCE_TYPE(WindowTarget)

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget = { 0 };
			D3D12_CPU_DESCRIPTOR_HANDLE ShaderResource = { 0 };
		} Views;

		Window* AppWindow = nullptr;
		RECT ScissorRect = { 0 };
		D3D12_VIEWPORT Viewport = { 0 };

		float startXFactor = 0.0f;
		float startYFactor = 0.0f;
		float widthFactor = 1.0f;
		float heightFactor = 1.0f;
	};
}