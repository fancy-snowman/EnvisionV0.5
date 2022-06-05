#include "envision/envpch.h"
#include "envision/graphics/Renderer.h"
#include "envision/graphics/AssetManager.h"
#include "envision/resource/ResourceManager.h"

#include "DirectXMath.h"

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

	m_pipelineState = ResourceManager::Get()->CreatePipelineState("PipelineState",
		{
			{ ShaderStage::Vertex, ShaderModel::V5_0, "shader.hlsl", "VS_main" },
			{ ShaderStage::Pixel, ShaderModel::V5_0, "shader.hlsl", "PS_main" }
		},
		true,
		{
			{ ParameterType::CBV, ShaderStage::Vertex, D3D12_ROOT_DESCRIPTOR{0, 0} },
			{ ParameterType::CBV, ShaderStage::Vertex, D3D12_ROOT_DESCRIPTOR{1, 0} },
		});

	m_intermediateTarget = ResourceManager::Get()->CreateTexture2D("IntermediateTarget",
		1200,
		800,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		TextureBindType::RenderTarget);

	using namespace DirectX;

	{
		XMVECTOR cameraPosition = { 0.0f, 3.0f, -5.f };
		XMVECTOR cameraTarget = { 0.0f, 0.0f, 0.f };
		XMVECTOR cameraUp = { 0.0f, 1.0f, 0.f };
		
		float FOV = 3.14f / 2.0f;
		float aspect = 1200.f / 800.f;
		float nearZ = 1.0f;
		float farZ = 20.f;

		XMMATRIX view = XMMatrixLookAtLH(cameraPosition, cameraTarget, cameraUp);
		XMMATRIX projection = XMMatrixPerspectiveFovLH(FOV, aspect, nearZ, farZ);

		XMFLOAT4X4 viewProjection;
		XMStoreFloat4x4(&viewProjection, XMMatrixTranspose(view * projection));

		m_cameraBuffer = ResourceManager::Get()->CreateBuffer("CameraBuffer", {
			{ "ViewProjectionMatrix", ShaderDataType::Float4x4} },
			BufferBindType::Constant,
			&viewProjection);
	}

	{
		XMMATRIX translation = XMMatrixTranslation(0.1f, 0.5f, 0.0f);
		XMMATRIX rotation = XMMatrixRotationRollPitchYaw(0.785f, 1.0f, 0.0f);
		XMMATRIX scale = XMMatrixScaling(2.0f, 2.0f, 2.0f);

		XMFLOAT4X4 transform;
		XMStoreFloat4x4(&transform, XMMatrixTranspose(scale * rotation * translation));

		m_objectBuffer = ResourceManager::Get()->CreateBuffer("ObjectBuffer", {
			{ "WorldMatrix", ShaderDataType::Float4x4 } },
			BufferBindType::Constant,
			&transform);

	}

	//m_phongBuffer = ResourceManager::Get()->CreateConstantBuffer("PhongBuffer", {
	//	{ "DiffuseFactor", ShaderDataType::Float3 },
	//	{ "Padding1", ShaderDataType::Float },
	//	{ "AmbientFactor", ShaderDataType::Float3 },
	//	{ "Padding2", ShaderDataType::Float },
	//	{ "SpecularFactor", ShaderDataType::Float3 },
	//	{ "SpecularExponent", ShaderDataType::Float } });

	m_frameInfo.FrameDescriptorAllocator.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 32, true);
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
	WindowTarget* targetResource = ResourceManager::Get()->GetTarget(target);
	PipelineState* pipeline = ResourceManager::Get()->GetPipelineState(m_pipelineState);

	m_frameInfo.WindowTarget = targetResource;
	m_frameInfo.FrameDescriptorAllocator.Clear();

	m_directList->Reset();

	m_directList->SetTarget(targetResource);
	m_directList->ClearRenderTarget(targetResource->Views.RenderTarget, 0.2f, 0.2f, 0.2f);

	m_directList->SetPipelineState(pipeline);

	ID3D12DescriptorHeap* descriptorHeap = m_frameInfo.FrameDescriptorAllocator.GetHeap();
	m_directList->SetDescriptorHeaps(1, &descriptorHeap);

	{ // Set up camera
		Buffer* cameraBuffer = ResourceManager::Get()->GetBuffer(m_cameraBuffer);
		D3D12_CPU_DESCRIPTOR_HANDLE cameraBufferSource = cameraBuffer->Views.Constant;
		D3D12_CPU_DESCRIPTOR_HANDLE cameraBufferDest = m_frameInfo.FrameDescriptorAllocator.Allocate();
		GPU::GetDevice()->CopyDescriptorsSimple(1,
			cameraBufferDest,
			cameraBufferSource,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		m_directList->GetNative()->SetGraphicsRootConstantBufferView(0, cameraBuffer->Native->GetGPUVirtualAddress());
	}

	{ // Set up object
		Buffer* objectBuffer = ResourceManager::Get()->GetBuffer(m_objectBuffer);

		static const float factorRoll = 0.0f;
		static const float factorPitch = 1.0f;
		static const float factorYaw = 0.0f;
		static float rotationTime = 0.0f;
		rotationTime += 0.001f;
		using namespace DirectX;
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		XMMATRIX rotation = XMMatrixRotationRollPitchYaw(
			sin(rotationTime * factorRoll),
			3.14 + sin(rotationTime * factorPitch),
			sin(rotationTime * factorYaw));
		XMMATRIX scale = XMMatrixScaling(0.005f, 0.005f, 0.005f);

		XMFLOAT4X4 transform;
		XMStoreFloat4x4(&transform, XMMatrixTranspose(scale * rotation * translation));
		ResourceManager::Get()->UploadBufferData(m_objectBuffer, &transform, sizeof(transform));

		D3D12_CPU_DESCRIPTOR_HANDLE objectBufferSource = objectBuffer->Views.Constant;
		D3D12_CPU_DESCRIPTOR_HANDLE objectBufferDest = m_frameInfo.FrameDescriptorAllocator.Allocate();
		GPU::GetDevice()->CopyDescriptorsSimple(1,
			objectBufferDest,
			objectBufferSource,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		m_directList->GetNative()->SetGraphicsRootConstantBufferView(1, objectBuffer->Native->GetGPUVirtualAddress());
	}
}

void env::Renderer::Submit(ID mesh, ID material)
{
	const Mesh* meshAsset = AssetManager::Get()->GetMesh(mesh);
	const Material* materialAsset = AssetManager::Get()->GetMaterial(material);

	Buffer* vertexBuffer = ResourceManager::Get()->GetBuffer(meshAsset->VertexBuffer);
	Buffer* indexBuffer = ResourceManager::Get()->GetBuffer(meshAsset->IndexBuffer);

	m_directList->SetVertexBuffer(vertexBuffer, 0);
	m_directList->SetIndexBuffer(indexBuffer);

	m_directList->DrawIndexed(indexBuffer->Layout.GetNumRepetitions(), 0, 0);
}

void env::Renderer::EndFrame()
{
	m_directList->Close();

	CommandQueue& queue = GPU::GetPresentQueue();
	queue.QueueList(m_directList);
	queue.Execute();
	queue.WaitForIdle();

	if (m_frameInfo.WindowTarget->GetType() == ResourceType::WindowTarget)
	{
		WindowTarget* target = (WindowTarget*)m_frameInfo.WindowTarget;

		m_directList->Reset();
		m_directList->TransitionResource(target, D3D12_RESOURCE_STATE_PRESENT);
		m_directList->Close();
		queue.QueueList(m_directList);
		queue.Execute();
		queue.WaitForIdle();
	}
}
