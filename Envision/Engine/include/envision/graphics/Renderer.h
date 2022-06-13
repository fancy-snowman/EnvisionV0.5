#pragma once
#include "envision/envpch.h"
#include "envision/core/Camera.h"
#include "envision/core/DescriptorAllocator.h"
#include "envision/core/GPU.h"
#include "envision/core/IDGenerator.h"
#include "envision/graphics/Assets.h"
#include "envision/resource/Resource.h"

namespace env
{
	// Singleton
	class Renderer
	{
	private:

		env::IDGenerator& m_commonIDGenerator;

		DirectList* m_directList;

		ID m_pipelineState;
		
		ID m_intermediateTarget;
		ID m_depthStencil;

		ID m_cameraBuffer;
		ID m_objectBuffer;
		ID m_phongBuffer;
		
		struct FrameInfo
		{

			struct {
				DirectX::XMFLOAT3 Position;
				float FieldOfView;
				DirectX::XMFLOAT3 ForwardDirection;
				float DistanceNearPlane;
				DirectX::XMFLOAT3 UpDirection;
				float DistanceFarPlane;
				DirectX::XMFLOAT4X4 ViewMatrix;
				DirectX::XMFLOAT4X4 ProjectionMatrix;
				DirectX::XMFLOAT4X4 ViewProjectionMatrix;
			} CameraBufferInfo;

			Resource* WindowTarget;
			DescriptorAllocator FrameDescriptorAllocator;
		} m_frameInfo;

		struct ObjectBufferData
		{
			DirectX::XMFLOAT3 Position;
			float ID;
			DirectX::XMFLOAT3 ForwardDirection;
			float MaterialID;
			DirectX::XMFLOAT3 UpDirection;
			float Pad;
			DirectX::XMFLOAT4X4 WorldMatrix;
		};

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