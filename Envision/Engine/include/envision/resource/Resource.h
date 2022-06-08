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
		UnorderedAccess		= 1 << 3,
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
		UINT GetByteWidth() final { return (UINT)Layout.GetByteWidth() * NumBuffers; }
			
		BufferLayout Layout;
		UINT NumBuffers;

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE ShaderResource = { 0 };
		} Views;
	};

	struct Buffer : public Resource
	{
		RESOURCE_TYPE(Buffer)
		UINT GetByteWidth() final { return (UINT)(Layout.GetByteWidth() + 255) & ~255; }

		BufferLayout Layout;

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE Constant = { 0 };
			D3D12_INDEX_BUFFER_VIEW Index = { 0 };
			D3D12_VERTEX_BUFFER_VIEW Vertex = { 0 };
			D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccess = { 0 };
		} Views;
	};

	struct Texture2D : public Resource
	{
		RESOURCE_TYPE(Texture2D)
		UINT GetByteWidth() final { return (UINT)(ByteWidth); }

		int Width;
		int Height;
		UINT64 RowPitch;
		UINT64 ByteWidth;
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
		UINT GetByteWidth() final { return (UINT)(ByteWidth); }

		int NumTextures;
		int Width;
		int Height;
		int ByteWidth;
		int RowPitch;
		DXGI_FORMAT Format;

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE ShaderResource = { 0 };
		} Views;
	};

	struct PipelineState : public Resource
	{
		RESOURCE_TYPE(PipelineState)

		ShaderStage ShaderStages;
		ID3D12RootSignature* RootSignature;
		ID3D12PipelineState* State;
	};

	struct WindowTarget : public Resource
	{
		RESOURCE_TYPE(WindowTarget)

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget = { 0 };
			D3D12_CPU_DESCRIPTOR_HANDLE ShaderResource = { 0 };
		} Views;

		RECT ScissorRect;
		D3D12_VIEWPORT Viewport;

		float startXFactor;
		float startYFactor;
		float widthFactor;
		float heightFactor;
	};
}