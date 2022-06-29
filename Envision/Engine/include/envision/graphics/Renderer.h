#pragma once
#include "envision/envpch.h"
#include "envision/core/Camera.h"
#include "envision/core/DescriptorAllocator.h"
#include "envision/core/GPU.h"
#include "envision/core/IDGenerator.h"
#include "envision/graphics/Assets.h"
#include "envision/graphics/CoreShaderDataStructures.h"
#include "envision/resource/Resource.h"
namespace env
{
	// Singleton
	class Renderer
	{
	private:

		env::IDGenerator& m_commonIDGenerator;

		DirectList* m_directList;

		const UINT ROOT_INDEX_INSTANCE_OFFSET_CONSTANT = 0;
		const UINT ROOT_INDEX_INSTANCE_TABLE = 1;
		const UINT ROOT_INDEX_MATERIAL_TABLE = 2;
		const UINT ROOT_INDEX_CAMERA_BUFFER = 3;
		ID m_pipelineState;
		
		ID m_intermediateTarget;
		ID m_depthStencil;

		ID m_cameraBuffer;
		ID m_instanceBuffer;
		ID m_materialBuffer;

		struct RenderJob
		{
			ID MeshID = ID_ERROR;
			UINT MaterialBufferIndex = 0;
		};

		struct MeshInstanceJobData
		{
			ID Mesh = ID_ERROR;
			UINT InstanceOffset = 0;
			UINT NumInstances = 0;
		};

		struct FrameInfo
		{

			struct {
				Float3 Position;
				float FieldOfView;
				Float3 ForwardDirection;
				float DistanceNearPlane;
				Float3 UpDirection;
				float DistanceFarPlane;
				Float4x4 ViewMatrix;
				Float4x4 ProjectionMatrix;
				Float4x4 ViewProjectionMatrix;
			} CameraBufferInfo;

			std::vector<InstanceBufferElementData> InstanceData;
			std::vector<MaterialBufferInstanceData> MaterialData;
			std::unordered_map<ID, UINT> MaterialIndices;

			// < MeshID, InstanceData >
			std::unordered_map<ID, std::vector<InstanceBufferElementData>> MeshInstances;

			std::vector<RenderJob> RenderJobs;

			Resource* WindowTarget;
			DescriptorAllocator FrameDescriptorAllocator;
		} m_frameInfo;

	public:

		static Renderer* Initialize(IDGenerator& commonIDGenerator);
		static Renderer* Get();
		static void Finalize();

	private:

		static Renderer* s_instance;

		Renderer(env::IDGenerator& commonIDGenerator);
		~Renderer();

		Renderer(const Renderer& other) = delete;
		Renderer(const Renderer&& other) = delete;
		Renderer& operator=(const Renderer& other) = delete;
		Renderer& operator=(const Renderer&& other) = delete;

	public:

		void Initialize();

		void BeginFrame(const CameraSettings& cameraSettings, Transform& cameraTransform, ID target);
		void Submit(Transform& transform, ID mesh, ID material);
		void EndFrame();
	};
}