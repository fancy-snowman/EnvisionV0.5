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
	// Initialize all command lists.
	for (int i = 0; i < NUM_FRAME_PACKETS; i++) {
		m_copyLists[i] = GPU::CreateCopyCommandList("Renderer::copyList_" + std::to_string(i));
		m_directLists[i] = GPU::CreateDirectCommandList("Renderer::directList_" + std::to_string(i));
	}

	m_pipelineState = ResourceManager::Get()->CreatePipelineState("PipelineState",
		{
			{ ShaderStage::Vertex, ShaderModel::V5_1, "../Engine/shader.hlsl", "VS_main" },
			{ ShaderStage::Pixel, ShaderModel::V5_1, "../Engine/shader.hlsl", "PS_main" }
		},
		true,
		{
			ROOT_CONSTANTS(ShaderStage::Vertex | ShaderStage::Pixel, 0, 0, 1),			// Instance offset
			ROOT_TABLE(ShaderStage::Vertex | ShaderStage::Pixel, SRV_RANGE(1, 0, 0, 0)),// Instance buffer array
			ROOT_TABLE(ShaderStage::Pixel, SRV_RANGE(1, 1, 0, 0)),						// Material buffer array
			ROOT_CBV_DESCRIPTOR(ShaderStage::Vertex | ShaderStage::Pixel, 1, 0),		// Camera buffer
			ROOT_TABLE(ShaderStage::Pixel, SAMPLER_RANGE(1, 0, 0, 0)),					// Samplers
			ROOT_TABLE(ShaderStage::Pixel, SRV_RANGE(50, 0, 1, 0))						// Diffuse textures
		});

	const int DEFAULT_TARGET_WIDTH = 1200;
	const int DEFAULT_TARGET_HEIGHT = 800;
	const UINT DEFAULT_INSTANCE_CAPACITY = 6000;
	const UINT DEFAULT_MATERIAL_CAPACITY = 50;

	{ // Initialize intermediate memory for the upload buffers
		m_intermediateUploadMemoryByteWidth = 500000000; // ~ 0.5 GB
		m_intermediateUploadMemory = (char*)malloc(m_intermediateUploadMemoryByteWidth);
	}

	// Initialize all frame packets. All packets need their own set of buffers as
	// multiple buffers can be "in flight" at the same time.
	for (int i = 0; i < NUM_FRAME_PACKETS; i++) {
		FramePacket& packet = m_framePackets[i];

		// Frame targets
		packet.Targets.Intermediate = ResourceManager::Get()->CreateTexture2D("RenderIntermediateTarget",
			DEFAULT_TARGET_WIDTH,
			DEFAULT_TARGET_HEIGHT,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			TextureBindType::RenderTarget);
		packet.Targets.Depth = ResourceManager::Get()->CreateTexture2D("RenderDepthStencil",
			DEFAULT_TARGET_WIDTH,
			DEFAULT_TARGET_HEIGHT,
			DXGI_FORMAT_D32_FLOAT,
			TextureBindType::DepthStencil);

		// Frame buffers
		packet.Buffers.Camera = ResourceManager::Get()->CreateBuffer("RenderCameraBuffer",
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
		packet.Buffers.Instance = ResourceManager::Get()->CreateBufferArray("RenderInstanceBuffer",
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
		packet.Buffers.Material = ResourceManager::Get()->CreateBufferArray("RenderMaterialBuffer", 
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

		// Init a sampler heap allocator for the frame packet
		DescriptorAllocator& samplerAllocator = m_samplerAllocators[i];
		samplerAllocator.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 32, true);

		m_uploadBuffers[i] = ResourceManager::Get()->CreateBuffer("RenderUploadBuffer",
			BufferLayout({
				{ "Typeless", ShaderDataType::Unknown }},
				m_intermediateUploadMemoryByteWidth),
			BufferBindType::Upload);
	}

	{
		D3D12_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		desc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		desc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = (desc.Filter == D3D12_FILTER_ANISOTROPIC) ? 16 : 1;
		desc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		//desc.BorderColor
		desc.MinLOD = 0;
		desc.MaxLOD = D3D12_FLOAT32_MAX;

		m_sampler = ResourceManager::Get()->CreateSampler("DefaultSampler", desc);
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
	packet.MaterialInstanceIndices.clear();
	packet.MaterialInstances.clear();
	packet.SamplerInstanceIndices.clear();
	packet.SamplerInstances.clear();
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

		UINT diffuseMapIndex = 0;
		if (packet.TextureInstanceIndices.count(materialData->DiffuseMap)) {
			diffuseMapIndex = packet.TextureInstanceIndices[materialData->DiffuseMap];
		}
		else {
			diffuseMapIndex = (UINT)packet.TextureInstances.size();
			packet.TextureInstances.push_back(materialData->DiffuseMap);
			packet.TextureInstanceIndices[materialData->DiffuseMap] = diffuseMapIndex;
		}

		MaterialBufferInstanceData instanceData;
		instanceData.AmbientFactor = materialData->AmbientFactor;
		instanceData.AmbientMapIndex = materialData->AmbientMap;
		instanceData.DiffuseFactor = materialData->DiffuseFactor;
		instanceData.DiffuseMapIndex = diffuseMapIndex;
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

	CommandQueue& copyQueue = GPU::GetCopyQueue();
	CommandQueue& presentQueue = GPU::GetPresentQueue();

	CopyList* copyList = m_copyLists[m_currentFramePacketIndex];
	DirectList* presentList = m_directLists[m_currentFramePacketIndex];

	// When EndFrame is called, no more job submissions are allowed. We can now
	// iterate all submissions in order to create instanced render jobs. All
	// instance data are stored in one buffer array.
	struct RenderJob {
		ID Mesh;
		UINT InstanceOffset;
		UINT NumInstances;
	};
	static std::vector<RenderJob> jobs;
	static std::vector<InstanceBufferElementData> intermediateInstanceData;

	jobs.clear();
	intermediateInstanceData.clear();

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

	CameraBufferData cameraBufferData;
	{
		using namespace DirectX;

		Float4x4 cameraView = XMMatrixLookToLH(
			packet.Camera.Transform.GetPosition(),
			packet.Camera.Transform.GetForward(),
			packet.Camera.Transform.GetUp());

		Texture2D* target = ResourceManager::Get()->GetTexture2D(packet.Targets.Intermediate);

		Float4x4 cameraProjection = XMMatrixPerspectiveFovLH(
			packet.Camera.Settings.FieldOfView,
			target->Width / target->Height,
			packet.Camera.Settings.DistanceNearPlane,
			packet.Camera.Settings.DistanceFarPlane);

		Float4x4 cameraViewProjection = cameraView * cameraProjection;
		
		cameraView = cameraView.Transpose();
		cameraProjection = cameraProjection.Transpose();
		cameraViewProjection = cameraViewProjection.Transpose();

		cameraBufferData.Position = packet.Camera.Transform.GetPosition();
		cameraBufferData.ForwardDirection = packet.Camera.Transform.GetForward();
		cameraBufferData.UpDirection = packet.Camera.Transform.GetUp();
		cameraBufferData.ViewMatrix = cameraView;
		cameraBufferData.ProjectionMatrix = cameraProjection;
		cameraBufferData.ViewProjectionMatrix = cameraViewProjection;
		cameraBufferData.FieldOfView = packet.Camera.Settings.FieldOfView;
		cameraBufferData.DistanceNearPlane = packet.Camera.Settings.DistanceNearPlane;
		cameraBufferData.DistanceFarPlane = packet.Camera.Settings.DistanceFarPlane;
	}

	copyList->Reset();

	// Calculate amount of memory to be uploaded to the GPU this frame
	struct CopyInfo {
		UINT offset = 0;
		UINT numBytes = 0;
	};
	UINT numBytesTotal = 0;
	CopyInfo instanceUploadInfo;
	CopyInfo materialUploadInfo;
	CopyInfo cameraUploadInfo;

	instanceUploadInfo.offset = numBytesTotal;
	instanceUploadInfo.numBytes = intermediateInstanceData.size() * sizeof(InstanceBufferElementData);
	numBytesTotal += ALIGN(instanceUploadInfo.numBytes, 16);

	materialUploadInfo.offset = numBytesTotal;
	materialUploadInfo.numBytes = packet.MaterialInstances.size() * sizeof(MaterialBufferInstanceData);
	numBytesTotal += ALIGN(materialUploadInfo.numBytes, 16);

	cameraUploadInfo.offset = numBytesTotal;
	cameraUploadInfo.numBytes = sizeof(CameraBufferData);
	numBytesTotal += ALIGN(cameraUploadInfo.numBytes, 16);

	// Retrieve all data to be uploaded through the upload buffer
	Buffer* uploadBuffer = ResourceManager::Get()->GetBuffer(m_uploadBuffers[m_currentFramePacketIndex]);

	if (m_intermediateUploadMemoryByteWidth < numBytesTotal) {
		delete m_intermediateUploadMemory;
		m_intermediateUploadMemoryByteWidth = numBytesTotal;
		m_intermediateUploadMemory = (char*)malloc(m_intermediateUploadMemoryByteWidth);
	}

	memcpy(m_intermediateUploadMemory + instanceUploadInfo.offset,
		intermediateInstanceData.data(),
		instanceUploadInfo.numBytes);
	memcpy(m_intermediateUploadMemory + materialUploadInfo.offset,
		packet.MaterialInstances.data(),
		materialUploadInfo.numBytes);
	memcpy(m_intermediateUploadMemory + cameraUploadInfo.offset,
		&cameraBufferData,
		cameraUploadInfo.numBytes);

	copyList->UploadBufferData(uploadBuffer, m_intermediateUploadMemory, numBytesTotal);

	BufferArray* instanceBuffer = ResourceManager::Get()->GetBufferArray(packet.Buffers.Instance);
	BufferArray* materialBuffer = ResourceManager::Get()->GetBufferArray(packet.Buffers.Material);
	Buffer* cameraBuffer = ResourceManager::Get()->GetBuffer(packet.Buffers.Camera);

	copyList->CopyBufferRegion(instanceBuffer,
		0,
		uploadBuffer,
		instanceUploadInfo.offset,
		instanceUploadInfo.numBytes);
	copyList->CopyBufferRegion(materialBuffer,
		0,
		uploadBuffer,
		materialUploadInfo.offset,
		materialUploadInfo.numBytes);
	copyList->CopyBufferRegion(cameraBuffer,
		0,
		uploadBuffer,
		cameraUploadInfo.offset,
		cameraUploadInfo.numBytes);

	copyList->Close();

	copyQueue.QueueList(copyList);
	copyQueue.WaitForQueue(&presentQueue, packet.FinishedFenceValue);
	copyQueue.Execute();
	copyQueue.IncrementFence();

	DescriptorAllocator& currentDescriptorAllocator = m_descriptorAllocators[m_currentFramePacketIndex];
	DescriptorAllocator& currentSamplerAllocator = m_samplerAllocators[m_currentFramePacketIndex];
	currentDescriptorAllocator.Clear();
	currentSamplerAllocator.Clear();

	PipelineState* pipeline = resourceManager->GetPipelineState(m_pipelineState);
	Texture2D* target = resourceManager->GetTexture2D(packet.Targets.Intermediate);
	Texture2D* depth = resourceManager->GetTexture2D(packet.Targets.Depth);

	const Float4 TARGET_CLEAR_COLOR = { 0.2f, 0.2f, 0.2f, 1.0f };
	const float DEPTH_CLEAR_VALUE = 1.0f;

	presentList->Reset();
	presentList->ClearRenderTarget(target->Views.RenderTarget, TARGET_CLEAR_COLOR);
	presentList->ClearDepthStencil(depth, true, false, DEPTH_CLEAR_VALUE, 0);
	presentList->SetTarget(target, depth);
	presentList->SetPipelineState(pipeline);

	ID3D12DescriptorHeap* heaps[] = {
		currentDescriptorAllocator.GetHeap(),
		currentSamplerAllocator.GetHeap()
	};
	presentList->SetDescriptorHeaps(2, heaps);

	{ // Fill the resource descriptor heap
		DescriptorAllocation instanceAllocation = currentDescriptorAllocator.Allocate();
		DescriptorAllocation materialAllocation = currentDescriptorAllocator.Allocate();
		DescriptorAllocation textureAllocation = currentDescriptorAllocator.Allocate(packet.TextureInstances.size());

		// TODO: Allocate together and copy all at once
		GPU::GetDevice()->CopyDescriptorsSimple(1, instanceAllocation.CPUHandle, instanceBuffer->Views.ShaderResource, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		GPU::GetDevice()->CopyDescriptorsSimple(1, materialAllocation.CPUHandle, materialBuffer->Views.ShaderResource, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		UINT stride = currentDescriptorAllocator.GetStride();
		for (int i = 0; i < packet.TextureInstances.size(); i++) {
			ID textureID = packet.TextureInstances[i];
			Texture2D* texture = ResourceManager::Get()->GetTexture2D(textureID);

			D3D12_CPU_DESCRIPTOR_HANDLE allocation = { textureAllocation.CPUHandle.ptr + stride * i };
			GPU::GetDevice()->CopyDescriptorsSimple(1,
				allocation,
				texture->Views.ShaderResource,
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		presentList->GetNative()->SetGraphicsRootDescriptorTable(ROOT_INDEX_INSTANCE_TABLE, instanceAllocation.GPUHandle);
		presentList->GetNative()->SetGraphicsRootDescriptorTable(ROOT_INDEX_MATERIAL_TABLE, materialAllocation.GPUHandle);
		presentList->GetNative()->SetGraphicsRootDescriptorTable(ROOT_INDEX_TEXTURE_TABLE, textureAllocation.GPUHandle);
	}

	{ // Fill the sampler descriptor heap
		DescriptorAllocation allocation = currentSamplerAllocator.Allocate();

		Sampler* sampler = ResourceManager::Get()->GetSampler(m_sampler);
		GPU::GetDevice()->CopyDescriptorsSimple(1,
			allocation.CPUHandle,
			sampler->View,
			D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		presentList->GetNative()->SetGraphicsRootDescriptorTable(ROOT_INDEX_SAMPLER_TABLE, allocation.GPUHandle);
	}

	{ // Bind resources that are bound directly to the root signature
		presentList->GetNative()->SetGraphicsRootConstantBufferView(ROOT_INDEX_CAMERA_TABLE, cameraBuffer->Native->GetGPUVirtualAddress());
	}

	for (auto& job : jobs) {
		const Mesh* meshAsset = AssetManager::Get()->GetMesh(job.Mesh);
		Buffer* vertexBuffer = ResourceManager::Get()->GetBuffer(meshAsset->VertexBuffer);
		Buffer* indexBuffer = ResourceManager::Get()->GetBuffer(meshAsset->IndexBuffer);

		presentList->SetVertexBuffer(vertexBuffer, 0);
		presentList->SetIndexBuffer(indexBuffer);
		presentList->GetNative()->SetGraphicsRoot32BitConstant(ROOT_INDEX_INSTANCE_OFFSET_CONSTANT,
			job.InstanceOffset, 0);

		presentList->DrawIndexedInstanced(meshAsset->NumIndices,
			job.NumInstances,
			meshAsset->OffsetIndices,
			meshAsset->OffsetVertices,
			0);
	}
	presentList->Close();

	presentQueue.QueueList(presentList);
	presentQueue.WaitForQueue(&copyQueue, copyQueue.GetFenceValue());
	presentQueue.Execute();

	// --------------------
	WindowTarget* finalTarget = ResourceManager::Get()->GetTarget(packet.Targets.Result);

	presentQueue.WaitForIdle();
	presentList->Reset();
	presentList->CopyResource(finalTarget, target);

	{
		//D3D12_TEXTURE_COPY_LOCATION dest;
		//dest.

		//GPU::GetDevice()->GetCopyableFootprints(&finalTarget->Native->GetDesc(), 0, 1, 0, &dest.PlacedFootprint, finalTarget.)

		//presentList->GetNative()->CopyTextureRegion()
	}

	presentList->Close();
	presentQueue.QueueList(presentList);
	presentQueue.Execute();
	// --------------------

	packet.FinishedFenceValue = presentQueue.IncrementFence();
}
