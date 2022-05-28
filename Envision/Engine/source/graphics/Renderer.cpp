#include "envision/envpch.h"
#include "envision/graphics/Renderer.h"
#include "envision/graphics/AssetManager.h"
#include "envision/resource/ResourceManager.h"

env::Renderer* env::Renderer::s_instance = nullptr;

env::Renderer* env::Renderer::Initialize(IDGenerator& commonIDGenerator)
{
	if (!s_instance)
		s_instance = new Renderer(commonIDGenerator);
	return s_instance;
}

env::Renderer* env::Renderer::Get()
{
	assert(s_instance);
	return s_instance;
}

void env::Renderer::Finalize()
{
	delete s_instance;
	s_instance = nullptr;
}

env::Renderer::Renderer(env::IDGenerator& commonIDGenerator) :
	m_commonIDGenerator(commonIDGenerator)
{
	m_directList = GPU::CreateDirectCommandList();

	m_pipelineState = ResourceManager::Get()->CreatePipelineState("PipelineState", {
		{ ShaderStage::Vertex, ShaderModel::V5_0, "shader.hlsl", "VS_main" },
		{ ShaderStage::Pixel, ShaderModel::V5_0, "shader.hlsl", "PS_main" } });

	m_intermediateTarget = ResourceManager::Get()->CreateTexture2D("IntermediateTarget",
		1200,
		800,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		BindType::RenderTarget);

	m_phongBuffer = ResourceManager::Get()->CreateConstantBuffer("PhongBuffer", {
		{ "DiffuseFactor", ShaderDataType::Float3 },
		{ "Padding1", ShaderDataType::Float },
		{ "AmbientFactor", ShaderDataType::Float3 },
		{ "Padding2", ShaderDataType::Float },
		{ "SpecularFactor", ShaderDataType::Float3 },
		{ "SpecularExponent", ShaderDataType::Float } });
}

env::Renderer::~Renderer()
{
	//
}

void env::Renderer::Initialize()
{
}

void env::Renderer::BeginFrame(ID target)
{
	WindowTarget* targetWindow = ResourceManager::Get()->GetWindowTarget(target);
	PipelineState* pipeline = ResourceManager::Get()->GetPipelineState(m_pipelineState);

	m_frameInfo.Target = targetWindow;

	m_directList->Reset();
	m_directList->SetWindowTarget(targetWindow);
	m_directList->SetPipelineState(pipeline);
}

void env::Renderer::Submit(ID mesh, ID material)
{
	const Mesh* meshAsset = AssetManager::Get()->GetMesh(mesh);
	const Material* materialAsset = AssetManager::Get()->GetMaterial(material);

	VertexBuffer* vertexBuffer = ResourceManager::Get()->GetVertexBuffer(meshAsset->VertexBuffer);
	IndexBuffer* indexBuffer = ResourceManager::Get()->GetIndexBuffer(meshAsset->IndexBuffer);

	m_directList->SetVertexBuffer(vertexBuffer, 0);
	m_directList->SetIndexBuffer(indexBuffer);

	m_directList->DrawIndexed(indexBuffer->NumIndices, 0, 0);
}

void env::Renderer::EndFrame()
{
	m_directList->Close();

	CommandQueue& queue = GPU::GetPresentQueue();
	queue.QueueList(m_directList);
	queue.Execute();
	queue.WaitForIdle();

	if (m_frameInfo.Target->GetType() == ResourceType::WindowTarget)
	{
		WindowTarget* window = (WindowTarget*)m_frameInfo.Target;
		Texture2D* backbuffer = window->GetActiveBackbuffer();


		m_directList->Reset();
		m_directList->TransitionResource(backbuffer, D3D12_RESOURCE_STATE_PRESENT);
		m_directList->Close();
		queue.QueueList(m_directList);
		queue.Execute();
		queue.WaitForIdle();

		window->SwapChain->Present(0, 0);
		window->ActiveBackBufferIndex = (window->ActiveBackBufferIndex + 1) % WindowTarget::NUM_BACK_BUFFERS;
	}
}
