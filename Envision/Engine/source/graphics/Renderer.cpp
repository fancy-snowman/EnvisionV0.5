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
			ROOT_CONSTANTS(ShaderStage::Vertex | ShaderStage::Pixel, 0, 0, 1),			// Instance offset
			ROOT_TABLE(ShaderStage::Vertex | ShaderStage::Pixel, SRV_RANGE(1, 0, 0)),	// Instance buffer array
			ROOT_TABLE(ShaderStage::Pixel, SRV_RANGE(1, 1, 0)),							// Material buffer array
			ROOT_CBV_DESCRIPTOR(ShaderStage::Vertex | ShaderStage::Pixel, 1, 0),		// Camera buffer
		});

	const int DEFAULT_TARGET_WIDTH = 1200;
	const int DEFAULT_TARGET_HEIGHT = 800;
	const UINT DEFAULT_INSTANCE_CAPACITY = 6000;
	const UINT DEFAULT_MATERIAL_CAPACITY = 50;

	// Initialize all frame packets. All packets need their own set of buffers as
	// multiple buffers can be "in flight" at the same time.
	for (int i = 0; i < NUM_FRAME_PACKETS; i++) {
		FramePacket& packet = m_framePackets[i];

		// Frame targets
		packet.Targets.Intermediate = ResourceManager::Get()->CreateTexture2D("IntermediateTarget",
			DEFAULT_TARGET_WIDTH,
			DEFAULT_TARGET_HEIGHT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			TextureBindType::RenderTarget);
		packet.Targets.Depth = ResourceManager::Get()->CreateTexture2D("DepthStencil",
			DEFAULT_TARGET_WIDTH,
			DEFAULT_TARGET_HEIGHT,
			DXGI_FORMAT_D32_FLOAT,
			TextureBindType::DepthStencil);

		// Frame buffers
		packet.Buffers.Camera = ResourceManager::Get()->CreateBuffer("CameraBuffer",
			BufferLayout({
				{ "Position", ShaderDataType::Float3 },
				{ "FieldOfView", ShaderDataType::Float },
				{ "ForwardDirection", ShaderDataType::Float3 },
				{ "DistanceNearPlane", ShaderDataType::Float },
				{ "UpDirection", ShaderDataType::Float3 },
				{ "DistanceFarPlane", ShaderDataType::Float },
				{ "ViewMatrix", ShaderDataType::Float4x4 },
				{ "ProjectionMatrix", ShaderDataType::Float4x4 },
				{ "ViewProjectionMatrix", ShaderDataType::Float4x4 }
			}),
			BufferBindType::Constant);
		packet.Buffers.Instance = ResourceManager::Get()->CreateBufferArray("InstanceBuffer",
			BufferLayout({
				{ "Position", ShaderDataType::Float3 },
				{ "ID", ShaderDataType::Float },
				{ "ForwardDirection", ShaderDataType::Float3 },
				{ "MaterialID", ShaderDataType::Float },
				{ "UpDirection", ShaderDataType::Float3 },
				{ "Pad", ShaderDataType::Float },
				{ "WorldMatrix", ShaderDataType::Float4x4 } },
				DEFAULT_INSTANCE_CAPACITY),
			BufferBindType::ShaderResource);
		packet.Buffers.Material = ResourceManager::Get()->CreateBufferArray("MaterialBuffer", 
			BufferLayout({
				{ "AmbientFactor", ShaderDataType::Float3 },
				{ "AmbientMapIndex", ShaderDataType::Int },
				{ "DiffuseFactor", ShaderDataType::Float3 },
				{ "DiffuseMapIndex", ShaderDataType::Int },
				{ "SpecularFactor", ShaderDataType::Float3 },
				{ "SpecularMapIndex", ShaderDataType::Int },
				{ "Shininess", ShaderDataType::Float },
				{ "Shininess", ShaderDataType::Int },
				{ "Padding", ShaderDataType::Float2 }},
				DEFAULT_MATERIAL_CAPACITY),
			BufferBindType::ShaderResource);

		// Init a descriptor heap allocator for the frame packet
		DescriptorAllocator& allocator = m_descriptorAllocators[i];
		allocator.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 32, true);
	}
}

env::Renderer::~Renderer()
{
	//
}

int env::Renderer::StepCurrentFramePacketIndex()
{
	m_currentFramePacketIndex = (m_currentFramePacketIndex + 1) % NUM_FRAME_PACKETS;
	return m_currentFramePacketIndex;
}

void env::Renderer::ClearCurrentFramePacket()
{
	FramePacket& packet = GetCurrentFramePacket();

	packet.Camera.Settings.DistanceNearPlane = 1.0f;
	packet.Camera.Settings.DistanceFarPlane = 100.0f;
	packet.Camera.Settings.FieldOfView = 3.14f / 2.0f;
	packet.Camera.Settings.Orthographic = false;

	packet.Camera.Transform.SetPosition(Float3::Zero);
	packet.Camera.Transform.SetRotation(Quaternion::Identity);
	packet.Camera.Transform.SetScale(Float3::One);

	packet.MeshOpaqueInstances.clear();
}

env::FramePacket& env::Renderer::GetCurrentFramePacket()
{
	return m_framePackets[m_currentFramePacketIndex];
}

void env::Renderer::Initialize()
{
	//
}

void env::Renderer::BeginFrame(const CameraSettings& cameraSettings, Transform& cameraTransform, ID target)
{
	StepCurrentFramePacketIndex();
	ClearCurrentFramePacket();

	FramePacket& packet = GetCurrentFramePacket();

	// Initialize camera
	packet.Camera.Settings = cameraSettings;
	packet.Camera.Transform = cameraTransform;

	// Initialize targets
	packet.Targets.Result = target;
}

void env::Renderer::Submit(Transform& transform, ID mesh, ID material)
{
	FramePacket& packet = GetCurrentFramePacket();

	UINT materialIndex = 0;
	if (packet.MaterialInstanceIndices.count(material)) {
		materialIndex = packet.MaterialInstanceIndices[material];
	}
	else {
		Material* materialData = AssetManager::Get()->GetMaterial(material);

		MaterialBufferInstanceData instanceData;
		instanceData.AmbientFactor = materialData->AmbientFactor;
		instanceData.AmbientMapIndex = materialData->AmbientMap;
		instanceData.DiffuseFactor = materialData->DiffuseFactor;
		instanceData.DiffuseMapIndex = materialData->DiffuseMap;
		instanceData.SpecularFactor = materialData->SpecularFactor;
		instanceData.SpecularMapIndex = materialData->SpecularMap;
		instanceData.Shininess = materialData->Shininess;
		instanceData.MaterialID = (int)material;
		instanceData.Padding = Float2::Zero;

		materialIndex = (UINT)packet.MaterialInstanceIndices.size();
		packet.MaterialInstances.push_back(instanceData);
		packet.MaterialInstanceIndices[material] = materialIndex;
	}

	InstanceBufferElementData objectData;
	objectData.Position = transform.GetPosition();
	objectData.ID = mesh;
	objectData.ForwardDirection = transform.GetForward();
	objectData.MaterialIndex = materialIndex;
	objectData.UpDirection = transform.GetUp();
	objectData.Pad = 0;
	objectData.WorldMatrix = transform.GetMatrixTransposed();

	if (packet.MeshOpaqueInstances.count(mesh) == 0) {
		std::vector<InstanceBufferElementData>& meshInstances = packet.MeshOpaqueInstances[mesh];
		meshInstances.push_back(objectData);
	}
	else {
		packet.MeshOpaqueInstances[mesh].push_back(objectData);
	}
}

void env::Renderer::EndFrame()
{
	ResourceManager* resourceManager = ResourceManager::Get();
	FramePacket& packet = GetCurrentFramePacket();

	DescriptorAllocator& currentDescriptorAllocator = m_descriptorAllocators[m_currentFramePacketIndex];
	currentDescriptorAllocator.Clear();

	PipelineState* pipeline = resourceManager->GetPipelineState(m_pipelineState);
	WindowTarget* target = resourceManager->GetTarget(packet.Targets.Result);
	Texture2D* depth = resourceManager->GetTexture2D(packet.Targets.Depth);

	const Float4 TARGET_CLEAR_COLOR = { 0.2f, 0.2f, 0.2f, 1.0f };
	const float DEPTH_CLEAR_VALUE = 1.0f;

	m_directList->Reset();
	m_directList->ClearRenderTarget(target->Views.RenderTarget, TARGET_CLEAR_COLOR);
	m_directList->ClearDepthStencil(depth, true, false, DEPTH_CLEAR_VALUE, 0);
	m_directList->SetTarget(target, depth);
	m_directList->SetPipelineState(pipeline);

	ID3D12DescriptorHeap* descriptorHeap = currentDescriptorAllocator.GetHeap();
	m_directList->SetDescriptorHeaps(1, &descriptorHeap);

	{ // Update and set camera buffer
		using namespace DirectX;

		Float4x4 cameraView = XMMatrixLookToLH(
			packet.Camera.Transform.GetPosition(),
			packet.Camera.Transform.GetForward(),
			packet.Camera.Transform.GetUp());

		Float4x4 cameraProjection = XMMatrixPerspectiveFovLH(
			packet.Camera.Settings.FieldOfView,
			target->Viewport.Width / target->Viewport.Height,
			packet.Camera.Settings.DistanceNearPlane,
			packet.Camera.Settings.DistanceFarPlane);

		Float4x4 cameraViewProjection = cameraView * cameraProjection;
			
		cameraView = cameraView.Transpose();
		cameraProjection = cameraProjection.Transpose();
		cameraViewProjection = cameraViewProjection.Transpose();

		CameraBufferData bufferData;
		bufferData.Position = packet.Camera.Transform.GetPosition();
		bufferData.ForwardDirection = packet.Camera.Transform.GetForward();
		bufferData.UpDirection = packet.Camera.Transform.GetUp();
		bufferData.ViewMatrix = cameraView;
		bufferData.ProjectionMatrix = cameraProjection;
		bufferData.ViewProjectionMatrix = cameraViewProjection;
		bufferData.FieldOfView = packet.Camera.Settings.FieldOfView;
		bufferData.DistanceNearPlane = packet.Camera.Settings.DistanceNearPlane;
		bufferData.DistanceFarPlane = packet.Camera.Settings.DistanceFarPlane;
			
		Buffer* cameraBuffer = ResourceManager::Get()->GetBuffer(packet.Buffers.Camera);

		ResourceManager::Get()->UploadBufferData(packet.Buffers.Camera,
			&bufferData,
			sizeof(bufferData));

		m_directList->GetNative()->SetGraphicsRootConstantBufferView(ROOT_INDEX_CAMERA_BUFFER,
			cameraBuffer->Native->GetGPUVirtualAddress());
	}

	{ // Update and set material buffer
		ResourceManager::Get()->UploadBufferData(packet.Buffers.Material,
		packet.MaterialInstances.data(),
		packet.MaterialInstances.size() * sizeof(MaterialBufferInstanceData));

		BufferArray* materialBuffer = ResourceManager::Get()->GetBufferArray(packet.Buffers.Material);
		D3D12_CPU_DESCRIPTOR_HANDLE bufferShaderResource = materialBuffer->Views.ShaderResource;
		DescriptorAllocation frameAllocation = currentDescriptorAllocator.Allocate();
		GPU::GetDevice()->CopyDescriptorsSimple(1,
			frameAllocation.CPUHandle,
			bufferShaderResource,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		m_directList->GetNative()->SetGraphicsRootDescriptorTable(ROOT_INDEX_MATERIAL_TABLE,
			frameAllocation.GPUHandle);
	}

	struct RenderJob {
		ID Mesh;
		UINT InstanceOffset;
		UINT NumInstances;
	};
	std::vector<RenderJob> jobs;

	{ // Update and set instance buffer, create render jobs
		std::vector<InstanceBufferElementData> intermediateInstanceData;

		// Iterate all submission and
		//	A. Copy memory to intermediate instance buffer (CPU, to be uploaded to GPU)
		//	B. Create instance render jobs

		UINT instanceOffset = 0;
		for (auto& [meshID, meshInstances] : packet.MeshOpaqueInstances) {
			RenderJob job;
			job.Mesh = meshID;
			job.InstanceOffset = instanceOffset;
			job.NumInstances = (UINT)meshInstances.size();
			jobs.push_back(job);

			intermediateInstanceData.insert(intermediateInstanceData.end(), meshInstances.begin(), meshInstances.end());
			instanceOffset += job.NumInstances;
		}

		ResourceManager::Get()->UploadBufferData(packet.Buffers.Instance,
			intermediateInstanceData.data(),
			intermediateInstanceData.size() * sizeof(InstanceBufferElementData));

		BufferArray* instanceBuffer = ResourceManager::Get()->GetBufferArray(packet.Buffers.Instance);
		D3D12_CPU_DESCRIPTOR_HANDLE bufferShaderResource = instanceBuffer->Views.ShaderResource;
		DescriptorAllocation frameAllocation = currentDescriptorAllocator.Allocate();
		GPU::GetDevice()->CopyDescriptorsSimple(1,
			frameAllocation.CPUHandle,
			bufferShaderResource,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		m_directList->GetNative()->SetGraphicsRootDescriptorTable(ROOT_INDEX_INSTANCE_TABLE,
			frameAllocation.GPUHandle);
	}

	for (auto& job : jobs) {
		const Mesh* meshAsset = AssetManager::Get()->GetMesh(job.Mesh);
		Buffer* vertexBuffer = ResourceManager::Get()->GetBuffer(meshAsset->VertexBuffer);
		Buffer* indexBuffer = ResourceManager::Get()->GetBuffer(meshAsset->IndexBuffer);

		m_directList->SetVertexBuffer(vertexBuffer, 0);
		m_directList->SetIndexBuffer(indexBuffer);
		m_directList->GetNative()->SetGraphicsRoot32BitConstant(ROOT_INDEX_INSTANCE_OFFSET_CONSTANT,
			job.InstanceOffset, 0);

		m_directList->DrawIndexedInstanced(meshAsset->NumIndices,
			job.NumInstances,
			meshAsset->OffsetIndices,
			meshAsset->OffsetVertices,
			0);
	}


	m_directList->Close();

	CommandQueue& queue = GPU::GetPresentQueue();
	queue.QueueList(m_directList);
}
