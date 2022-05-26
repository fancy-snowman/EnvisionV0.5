#pragma once
#include "envision/envpch.h"
#include "envision/resource/BufferLayout.h"
#include "envision/resource/TextureLayout.h"

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

	struct Resource
	{
		Resource() = default;
		Resource(env::Resource && other) = default;

		virtual ResourceType GetType() = 0;

		std::string Name;
		ID3D12Resource* Native;
		D3D12_RESOURCE_STATES State = D3D12_RESOURCE_STATE_COMMON;
	};

	struct BufferArray : public Resource
	{
		RESOURCE_TYPE(BufferArray)
		BufferLayout Layout;

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE ShaderResource = { 0 };
		} Views;
	};

	struct ConstantBuffer : public Resource
	{
		RESOURCE_TYPE(ConstantBuffer)
		BufferLayout Layout;

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE ConstantBuffer = { 0 };
			D3D12_CPU_DESCRIPTOR_HANDLE ShaderResource = { 0 };
			D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccess = { 0 };
		} Views;
	};

	struct IndexBuffer : public Resource
	{
		RESOURCE_TYPE(IndexBuffer)
		BufferLayout Layout;

		struct {
			D3D12_INDEX_BUFFER_VIEW IndexBuffer = { 0 };
		} Views;
	};

	struct VertexBuffer : public Resource
	{
		RESOURCE_TYPE(VertexBuffer)
		BufferLayout Layout;

		struct {
			D3D12_VERTEX_BUFFER_VIEW VertexBuffer = { 0 };
		} Views;
	};

	struct Texture2D : public Resource
	{
		RESOURCE_TYPE(Texture2D)
		int Width;
		int Height;
		int ByteWidth;
		int RowPitch;
		TextureLayout Layout;

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE RenderTarget = { 0 };
			D3D12_CPU_DESCRIPTOR_HANDLE ShaderResource = { 0 };
			D3D12_CPU_DESCRIPTOR_HANDLE UnorderedAccess = { 0 };
		} Views;
	};

	struct Texture2DArray : public Resource
	{
		RESOURCE_TYPE(Texture2DArray)
		int NumTextures;
		int Width;
		int Height;
		int ByteWidth;
		int RowPitch;
		TextureLayout Layout;

		struct {
			D3D12_CPU_DESCRIPTOR_HANDLE ShaderResource = { 0 };
		} Views;
	};

	struct PipelineState : public Resource
	{
		RESOURCE_TYPE(PipelineState)
		ID3D12PipelineState* State;
	};

	struct WindowTarget : public Resource
	{
		RESOURCE_TYPE(WindowTarget)
		int Width;
		int Height;
		float startXFactor;
		float startYFactor;
		float widthFactor;
		float heightFactor;
		HWND Window;
		RECT ScissorRect;
		D3D12_VIEWPORT ViewPort;
	};
}