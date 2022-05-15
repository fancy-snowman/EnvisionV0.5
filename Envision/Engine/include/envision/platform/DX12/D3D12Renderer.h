#pragma once
#include "envision/envpch.h"
#include "envision/graphics/Renderer.h"

namespace env
{
	class D3D12Renderer : public Renderer
	{
		ID3D12Device* m_device;

		// TODO: Put these in a render pass class
		struct RenderPass
		{
			ID3D12RootSignature* RootSignature;
			ID3D12PipelineState* PSO;
			ID3DBlob* VertexShader;
			ID3DBlob* PixelShader;
		} m_pass;

		void InitPass();

	public:

		D3D12Renderer();
		~D3D12Renderer() final;

	public:

		void Initialize(env::AssetManager*& assetMgrOut, env::ResourceManager*& resourceMgrOut) final;

		void BeginFrame(ID target) final;
		void Submit(ID mesh, ID material) final;
		void EndFrame() final;
	};
}
