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

			//{ ParameterType::Constant, ShaderStage::Vertex, D3D12_ROOT_CONSTANTS{ 1, 0, 1 } },
			//{ ParameterType::CBV, ShaderStage::Vertex, D3D12_ROOT_DESCRIPTOR{0, 0} },
			//{ ParameterType::Table, ShaderStage::Vertex, {
			//	{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, 0 },
			//}},
			//{ ParameterType::Constant, ShaderStage::Pixel, D3D12_ROOT_CONSTANTS{ 0, 0, 1 } },
			//{ ParameterType::Table, ShaderStage::Pixel, {
			//	{ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, 0 },
			//}}
		});

	m_intermediateTarget = ResourceManager::Get()->CreateTexture2D("IntermediateTarget",
		1200,
		800,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		TextureBindType::RenderTarget);

	m_depthStencil = ResourceManager::Get()->CreateTexture2D("DepthStencil",
		1200,
		800,
		DXGI_FORMAT_D32_FLOAT,
		TextureBindType::DepthStencil);

	using namespace DirectX;

	{		
		float aspect = 1200.f / 800.f;	

		m_frameInfo.CameraBufferInfo;
		m_frameInfo.CameraBufferInfo.Position = { 0.0f, 250.f, -500.0 };
		m_frameInfo.CameraBufferInfo.FieldOfView = 3.14f / 2.0f;
		m_frameInfo.CameraBufferInfo.ForwardDirection = { 0.0f, 0.0f, 1.0f };
		m_frameInfo.CameraBufferInfo.DistanceNearPlane = 10.0f;
		m_frameInfo.CameraBufferInfo.UpDirection = { 0.0f, 1.0f, 0.0f };
		m_frameInfo.CameraBufferInfo.DistanceFarPlane = 1000.0f;

		XMMATRIX view = XMMatrixLookToLH(
			XMLoadFloat3(&m_frameInfo.CameraBufferInfo.Position),
			XMLoadFloat3(&m_frameInfo.CameraBufferInfo.ForwardDirection),
			XMLoadFloat3(&m_frameInfo.CameraBufferInfo.UpDirection));
		XMMATRIX projection = XMMatrixPerspectiveFovLH(
			m_frameInfo.CameraBufferInfo.FieldOfView,
			aspect,
			m_frameInfo.CameraBufferInfo.DistanceNearPlane,
			m_frameInfo.CameraBufferInfo.DistanceFarPlane);
		XMMATRIX viewProjection = view * projection;

		view = XMMatrixTranspose(view);
		projection = XMMatrixTranspose(projection);
		viewProjection = XMMatrixTranspose(viewProjection);

		XMStoreFloat4x4(&m_frameInfo.CameraBufferInfo.ViewMatrix, view);
		XMStoreFloat4x4(&m_frameInfo.CameraBufferInfo.ProjectionMatrix, projection);
		XMStoreFloat4x4(&m_frameInfo.CameraBufferInfo.ViewProjectionMatrix, viewProjection);

		m_cameraBuffer = ResourceManager::Get()->CreateBuffer("CameraBuffer", {
			{ "Position", ShaderDataType::Float3 },
			{ "FieldOfView", ShaderDataType::Float },
			{ "ForwardDirection", ShaderDataType::Float3 },
			{ "DistanceNearPlane", ShaderDataType::Float },
			{ "UpDirection", ShaderDataType::Float3 },
			{ "DistanceFarPlane", ShaderDataType::Float },
			{ "ViewMatrix", ShaderDataType::Float4x4 },
			{ "ProjectionMatrix", ShaderDataType::Float4x4 },
			{ "ViewProjectionMatrix", ShaderDataType::Float4x4 } },
			BufferBindType::Constant,
			&m_frameInfo.CameraBufferInfo);
	}

	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		XMMATRIX rotation = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
		XMMATRIX scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);

		XMFLOAT4X4 transform;
		//XMStoreFloat4x4(&transform, XMMatrixTranspose(scale * rotation * translation));
		XMStoreFloat4x4(&transform, XMMatrixIdentity());

		InstanceBufferElementData data;
		data.Position = { 0.0f, 0.0f, 0.0f };
		data.ID = 0;
		data.ForwardDirection = { 0.0f, 0.0f, 1.0f };
		data.MaterialIndex = 0;
		data.UpDirection = { 0.0f, 1.0f, 0.0f };
		data.Pad = 0;
		data.WorldMatrix = transform;

		std::vector<InstanceBufferElementData> initialData(6000, data);

		m_instanceBuffer = ResourceManager::Get()->CreateBufferArray("ObjectBuffer",
			BufferLayout({
				{ "Position", ShaderDataType::Float3 },
				{ "ID", ShaderDataType::Float },
				{ "ForwardDirection", ShaderDataType::Float3 },
				{ "MaterialID", ShaderDataType::Float },
				{ "UpDirection", ShaderDataType::Float3 },
				{ "Pad", ShaderDataType::Float },
				{ "WorldMatrix", ShaderDataType::Float4x4 } },
				(UINT)initialData.size()),
			BufferBindType::ShaderResource,
			initialData.data());
	}

	m_materialBuffer = ResourceManager::Get()->CreateBufferArray("MaterialBuffer", 
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
			50),
		BufferBindType::ShaderResource);

	m_frameInfo.FrameDescriptorAllocator.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 32, true);
}

env::Renderer::~Renderer()
{
	//
}

void env::Renderer::Initialize()
{
}

void env::Renderer::BeginFrame(const CameraSettings& cameraSettings, Transform& cameraTransform, ID target)
{
	WindowTarget* targetResource = ResourceManager::Get()->GetTarget(target);
	PipelineState* pipeline = ResourceManager::Get()->GetPipelineState(m_pipelineState);

	float aspectRatio = targetResource->Viewport.Width / targetResource->Viewport.Height;

	m_frameInfo.WindowTarget = targetResource;
	m_frameInfo.FrameDescriptorAllocator.Clear();
	m_frameInfo.InstanceData.clear();
	m_frameInfo.MaterialData.clear();
	m_frameInfo.MaterialIndices.clear();
	m_frameInfo.RenderJobs.clear();
	m_frameInfo.MeshInstances.clear();

	m_directList->Reset();

	Texture2D* depthStencil = ResourceManager::Get()->GetTexture2D(m_depthStencil);

	m_directList->SetTarget(targetResource, depthStencil);
	m_directList->ClearRenderTarget(targetResource->Views.RenderTarget, 0.2f, 0.2f, 0.2f);
	m_directList->ClearDepthStencil(depthStencil->Views.DepthStencil, true, false, 1.0f, 0);

	m_directList->SetPipelineState(pipeline);

	ID3D12DescriptorHeap* descriptorHeap = m_frameInfo.FrameDescriptorAllocator.GetHeap();
	m_directList->SetDescriptorHeaps(1, &descriptorHeap);

	{ // Set up camera
		
		using namespace DirectX;

		Float4x4 cameraView = XMMatrixLookToLH(
			cameraTransform.GetPosition(),
			cameraTransform.GetForward(),
			cameraTransform.GetUp());

		Float4x4 cameraProjection = XMMatrixPerspectiveFovLH(
			cameraSettings.FieldOfView,
			aspectRatio,
			cameraSettings.DistanceNearPlane,
			cameraSettings.DistanceFarPlane);

		Float4x4 cameraViewProjection = cameraView * cameraProjection;
		
		cameraView = cameraView.Transpose();
		cameraProjection = cameraProjection.Transpose();
		cameraViewProjection = cameraViewProjection.Transpose();

		m_frameInfo.CameraBufferInfo.Position = cameraTransform.GetPosition();
		m_frameInfo.CameraBufferInfo.ForwardDirection = cameraTransform.GetForward();
		m_frameInfo.CameraBufferInfo.UpDirection = cameraTransform.GetUp();
		m_frameInfo.CameraBufferInfo.ViewMatrix = cameraView;
		m_frameInfo.CameraBufferInfo.ProjectionMatrix = cameraProjection;
		m_frameInfo.CameraBufferInfo.ViewProjectionMatrix = cameraViewProjection;

		m_frameInfo.CameraBufferInfo.FieldOfView = cameraSettings.FieldOfView;
		m_frameInfo.CameraBufferInfo.DistanceNearPlane = cameraSettings.DistanceNearPlane;
		m_frameInfo.CameraBufferInfo.DistanceFarPlane = cameraSettings.DistanceFarPlane;
		
		Buffer* cameraBuffer = ResourceManager::Get()->GetBuffer(m_cameraBuffer);

		ResourceManager::Get()->UploadBufferData(m_cameraBuffer,
			&m_frameInfo.CameraBufferInfo,
			sizeof(m_frameInfo.CameraBufferInfo));

		//D3D12_CPU_DESCRIPTOR_HANDLE cameraBufferSource = cameraBuffer->Views.Constant;
		//D3D12_CPU_DESCRIPTOR_HANDLE cameraBufferDest = m_frameInfo.FrameDescriptorAllocator.Allocate();
		//GPU::GetDevice()->CopyDescriptorsSimple(1,
		//	cameraBufferDest,
		//	cameraBufferSource,
		//	D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		m_directList->GetNative()->SetGraphicsRootConstantBufferView(ROOT_INDEX_CAMERA_BUFFER,
			cameraBuffer->Native->GetGPUVirtualAddress());
	}

	{ // Set up object
		BufferArray* objectBuffer = ResourceManager::Get()->GetBufferArray(m_instanceBuffer);

		D3D12_CPU_DESCRIPTOR_HANDLE objectBufferSource = objectBuffer->Views.ShaderResource;
		DescriptorAllocation frameAllocation = m_frameInfo.FrameDescriptorAllocator.Allocate();
		GPU::GetDevice()->CopyDescriptorsSimple(1,
			frameAllocation.CPUHandle,
			objectBufferSource,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		m_directList->GetNative()->SetGraphicsRootDescriptorTable(ROOT_INDEX_INSTANCE_TABLE,
			frameAllocation.GPUHandle);
	}

	{
		BufferArray* materialBuffer = ResourceManager::Get()->GetBufferArray(m_materialBuffer);

		D3D12_CPU_DESCRIPTOR_HANDLE objectBufferSource = materialBuffer->Views.ShaderResource;
		DescriptorAllocation frameAllocation = m_frameInfo.FrameDescriptorAllocator.Allocate();
		GPU::GetDevice()->CopyDescriptorsSimple(1,
			frameAllocation.CPUHandle,
			objectBufferSource,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		m_directList->GetNative()->SetGraphicsRootDescriptorTable(ROOT_INDEX_MATERIAL_TABLE,
			frameAllocation.GPUHandle);
	}
}

void env::Renderer::Submit(Transform& transform, ID mesh, ID material)
{
	UINT materialIndex = 0;
	if (m_frameInfo.MaterialIndices.count(material) == 0) {
		Material* materialData = AssetManager::Get()->GetMaterial(material);
		
		MaterialBufferInstanceData bufferData;
		bufferData.AmbientFactor = materialData->AmbientFactor;
		bufferData.AmbientMapIndex = materialData->AmbientMap;
		bufferData.DiffuseFactor = materialData->DiffuseFactor;
		bufferData.DiffuseMapIndex = materialData->DiffuseMap;
		bufferData.SpecularFactor = materialData->SpecularFactor;
		bufferData.SpecularMapIndex = materialData->SpecularMap;
		bufferData.Shininess = materialData->Shininess;
		bufferData.MaterialID = (int)material;
		bufferData.Padding = Float2::Zero;

		materialIndex = (UINT)m_frameInfo.MaterialData.size();
		m_frameInfo.MaterialData.push_back(std::move(bufferData));
		m_frameInfo.MaterialIndices[material] = materialIndex;
	}
	else {
		materialIndex = m_frameInfo.MaterialIndices[material];
	}

	InstanceBufferElementData objectData;
	objectData.Position = transform.GetPosition();
	objectData.ID = mesh;
	objectData.ForwardDirection = transform.GetForward();
	objectData.MaterialIndex = materialIndex;
	objectData.UpDirection = transform.GetUp();
	objectData.Pad = 0;
	objectData.WorldMatrix = transform.GetMatrixTransposed();

	//m_frameInfo.InstanceData.push_back(objectData);
	//m_frameInfo.RenderJobs.push_back(RenderJob({ mesh, materialIndex }));

	//if (MeshInstance)
	if (m_frameInfo.MeshInstances.count(mesh) == 0) {
		std::vector<InstanceBufferElementData>& meshInstances = m_frameInfo.MeshInstances[mesh];
		meshInstances.push_back(objectData);
	}
	else {
		m_frameInfo.MeshInstances[mesh].push_back(objectData);
	}
}

void env::Renderer::EndFrame()
{
	std::vector<MeshInstanceJobData> instanceJobs;
	UINT instanceOffset = 0;
	for (auto& [meshID, meshInstances] : m_frameInfo.MeshInstances) {
		MeshInstanceJobData job;
		job.Mesh = meshID;
		job.InstanceOffset = instanceOffset;
		job.NumInstances = (UINT)meshInstances.size();
		instanceJobs.push_back(job);

		m_frameInfo.InstanceData.insert(m_frameInfo.InstanceData.end(), meshInstances.begin(), meshInstances.end());
		//std::copy(meshInstances.begin(), meshInstances.end(), m_frameInfo.InstanceData.end());
		instanceOffset += job.NumInstances;
	}

	ResourceManager::Get()->UploadBufferData(m_instanceBuffer,
		m_frameInfo.InstanceData.data(),
		m_frameInfo.InstanceData.size() * sizeof(InstanceBufferElementData));

	ResourceManager::Get()->UploadBufferData(m_materialBuffer,
		m_frameInfo.MaterialData.data(),
		m_frameInfo.MaterialData.size() * sizeof(MaterialBufferInstanceData));

	//for (UINT i = 0; i < (UINT)m_frameInfo.RenderJobs.size(); i++) {

	//	const RenderJob& job = m_frameInfo.RenderJobs[i];

	//	const Mesh* meshAsset = AssetManager::Get()->GetMesh(job.MeshID);

	//	Buffer* vertexBuffer = ResourceManager::Get()->GetBuffer(meshAsset->VertexBuffer);
	//	Buffer* indexBuffer = ResourceManager::Get()->GetBuffer(meshAsset->IndexBuffer);

	//	m_directList->SetVertexBuffer(vertexBuffer, 0);
	//	m_directList->SetIndexBuffer(indexBuffer);

	//	m_directList->GetNative()->SetGraphicsRoot32BitConstant(1, i, 0);
	//	m_directList->GetNative()->SetGraphicsRoot32BitConstant(3, job.MaterialBufferIndex, 0);

	//	// Used to be 5609 for the whole city scene
	//	m_directList->DrawIndexed(meshAsset->NumIndices,
	//		meshAsset->OffsetIndices,
	//		meshAsset->OffsetVertices);
	//}

	for (auto& job : instanceJobs) {
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
