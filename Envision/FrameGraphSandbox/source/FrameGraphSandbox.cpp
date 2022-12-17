#include "FrameGraphSandbox.hpp"
#include "envision/graphics/FrameGraph.h"

#ifdef _DEBUG
#pragma comment(lib, "Engine_d")
#else
#pragma comment(lib, "Engine")
#endif

env::Application* env::CreateApplication(int argc, char** argv)
{
	return new FrameGraphSandbox(argc, argv);
}

struct Vertex
{
	Float2 Position;
	Float3 Color;
};

FrameGraphSandbox::FrameGraphSandbox(int argc, char** argv) :
	env::Application(argc, argv, "FrameGraphSandbox")
{
	m_mainWindow = new env::Window(1200, 800, "Frame Graph", *this);
	PushWindow(m_mainWindow);

	env::FrameGraph::Initialize(env::GPU::GetDevice());

	std::vector<Vertex> vertices = {
		{ {  0.0f,  0.5f },		{ 1.0f, 0.0f, 0.0f }},
		{ {  0.5f, -0.5f },		{ 0.0f, 1.0f, 0.0f }},
		{ { -0.5f, -0.5f },		{ 0.0f, 0.0f, 1.0f }}
	};

	env::BufferLayout vertexBufferLayout({
			{ "Position", env::ShaderDataType::Float2 },
			{ "Color", env::ShaderDataType::Float3 }
		}, (UINT)vertices.size());

	m_vertexBuffer = env::ResourceManager::Get()->CreateBuffer("VertexBuffer",
		vertexBufferLayout,
		env::BufferBindType::Vertex,
		vertices.data());

	// TODO: env::ResourceManager::CreatePipelineState has a hardcoded input layout.
	m_PSO = env::ResourceManager::Get()->CreatePipelineState("MainState",
		{
			{ env::ShaderStage::Vertex, env::ShaderModel::V5_1, "../FrameGraphSandbox/shader.hlsl", "VS_main" },
			{ env::ShaderStage::Pixel, env::ShaderModel::V5_1, "../FrameGraphSandbox/shader.hlsl", "PS_main" },
		},
		true,
		{});
}

FrameGraphSandbox::~FrameGraphSandbox()
{
	env::FrameGraph::Finalize();
}

void FrameGraphSandbox::OnUpdate(const env::Duration& delta)
{
	std::cout << "[OnUpdate] " << delta.InSeconds() << "\n";

	env::FrameResource* backBuffer = nullptr;
	env::FrameResource* vertexBuffer = nullptr;
	UINT vertexBufferSize = 0;

	{
		env::Texture2D* envBackBuffer = env::ResourceManager::Get()->GetTexture2D(m_mainWindow->GetCurrentBackbuffer());
		env::Buffer* envVertexBuffer = env::ResourceManager::Get()->GetBuffer(m_vertexBuffer);

		backBuffer = env::FrameGraph::BeginFrame(envBackBuffer->Native);
		vertexBuffer = env::FrameGraph::ImportBuffer("VertexBuffer", envVertexBuffer->Native);
		vertexBufferSize = envVertexBuffer->GetByteWidth();
	}
	
	struct PassData
	{
		env::FrameResource* vertexBuffer;
		env::FrameResource* backBuffer;
	};

	PassData passData = env::FrameGraph::AddPass<PassData>("MainPass", env::PASS_TYPE::GRAPHICS,
		[&](PassData& passData, env::FramePassSetupContext& setupContext) {
			setupContext.ReadResource(vertexBuffer);
			setupContext.WriteRenderTarget(backBuffer);
		},
		[=](PassData passData, env::FramePassRecordContext& recordContext) {

			env::DirectList* list = static_cast<env::DirectList*>(recordContext.CommandList);

			D3D12_CPU_DESCRIPTOR_HANDLE vertexBufferAlloc = recordContext.ResourceViewAllocator->Allocate();
			D3D12_CPU_DESCRIPTOR_HANDLE renderTargetAlloc = recordContext.RenderTargetViewAllocator->Allocate();

			list->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			D3D12_VERTEX_BUFFER_VIEW vbv = {};
			vbv.BufferLocation = vertexBuffer->GetResource()->GetGPUVirtualAddress();
			vbv.SizeInBytes = vertexBufferSize;
			vbv.StrideInBytes = sizeof(Vertex);

			list->GetNative()->IASetVertexBuffers(0, 1, &vbv);
		});

	env::FrameGraph::EndFrame();
}
