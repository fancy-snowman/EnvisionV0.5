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
		ConstantBuffer,
		IndexBuffer,
		VertexBuffer,

		Texture2D,
		Texture2DArray,

		PipelineState,

		WindowTarget,
	};

	enum class BindType
	{
		Unknown = 0,

		RenderTarget = 1 << 0,
		ShaderResource = 1 << 1,
		UnorderedAccess = 1 << 2,
	};
	BindType operator|(BindType a, BindType b);
	BindType operator&(BindType a, BindType b);
	bool any(BindType type);

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

	struct ConstantBuffer : public Resource
	{
		RESOURCE_TYPE(ConstantBuffer)
		UINT GetByteWidth() final { return (UINT)(Layout.GetByteWidth() + 255) & ~255; }

		BufferLayout Layout;

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer = { 0 };
			D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccess = { 0 };
		} Views;
	};

	struct IndexBuffer : public Resource
	{
		RESOURCE_TYPE(IndexBuffer)
		UINT GetByteWidth() final { return sizeof(UINT) * NumIndices; }

		UINT NumIndices = 0;

		struct {
			D3D12_INDEX_BUFFER_VIEW IndexBuffer = { 0 };
		} Views;
	};

	struct VertexBuffer : public Resource
	{
		RESOURCE_TYPE(VertexBuffer)
		UINT GetByteWidth() final { return (UINT)Layout.GetByteWidth(); }

		BufferLayout Layout;

		struct {
			D3D12_VERTEX_BUFFER_VIEW VertexBuffer = { 0 };
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

	// Defined in Window.h
	class Window;
	struct WindowTarget : public Resource
	{
		RESOURCE_TYPE(WindowTarget)

		static const int NUM_BACK_BUFFERS = 2;

		int Width;
		int Height;
		float startXFactor;
		float startYFactor;
		float widthFactor;
		float heightFactor;
		env::Window* AppWindow;
		RECT ScissorRect;
		D3D12_VIEWPORT Viewport;

		IDXGISwapChain1* SwapChain;
		int ActiveBackBufferIndex = 0;

		Texture2D* Backbuffers[NUM_BACK_BUFFERS];
		inline Texture2D* GetActiveBackbuffer()
		{
			return Backbuffers[ActiveBackBufferIndex];
		}
	};
}