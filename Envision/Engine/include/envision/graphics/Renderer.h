#pragma once
#include "envision/envpch.h"
#include "envision/core/GPU.h"
#include "envision/core/DescriptorAllocator.h"
#include "envision/core/IDGenerator.h"
#include "envision/graphics/Assets.h"
#include "envision/resource/Resource.h"

namespace env
{
	struct CameraSettings
	{
		struct {
			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT3 RotationRollPitchYaw;

			void MoveForward(float distance)
			{
				DirectX::XMVECTOR xmPosition = DirectX::XMLoadFloat3(&Position);
				DirectX::XMVECTOR xmForward = { 0.0f, 0.0f, 1.0f, 0.0f };
				DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(
					RotationRollPitchYaw.y,
					RotationRollPitchYaw.z,
					RotationRollPitchYaw.x);
				xmForward = DirectX::XMVector3TransformNormal(xmForward, rotation);
				xmForward = DirectX::XMVectorScale(xmForward, distance);
				xmPosition = DirectX::XMVectorAdd(xmPosition, xmForward);
				DirectX::XMStoreFloat3(&Position, xmPosition);
			}

			void MoveUp(float distance)
			{
				DirectX::XMVECTOR xmPosition = DirectX::XMLoadFloat3(&Position);
				DirectX::XMVECTOR xmUp = { 0.0f, 1.0f, 0.0f, 0.0f };
				DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(
					RotationRollPitchYaw.y,
					RotationRollPitchYaw.z,
					RotationRollPitchYaw.x);
				xmUp = DirectX::XMVector3TransformNormal(xmUp, rotation);
				xmUp = DirectX::XMVectorScale(xmUp, distance);
				xmPosition = DirectX::XMVectorAdd(xmPosition, xmUp);
				DirectX::XMStoreFloat3(&Position, xmPosition);
			}

			void MoveRight(float distance)
			{
				DirectX::XMVECTOR xmPosition = DirectX::XMLoadFloat3(&Position);
				DirectX::XMVECTOR xmRight = { 1.0f, 0.0f, 0.0f, 0.0f };
				DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(
					RotationRollPitchYaw.y,
					RotationRollPitchYaw.z,
					RotationRollPitchYaw.x);
				xmRight = DirectX::XMVector3TransformNormal(xmRight, rotation);
				xmRight = DirectX::XMVectorScale(xmRight, distance);
				xmPosition = DirectX::XMVectorAdd(xmPosition, xmRight);
				DirectX::XMStoreFloat3(&Position, xmPosition);
			}
		} Transform;

		struct {
			float FieldOfView;
			float DistanceNearPlane;
			float DistanceFarPlane;
			bool Orthographic;
		} Projection;

		struct {
			float TurnSpeedHorizontal;
			float TurnSpeedVertical;
			float SpeedForward;
			float SpeedUp;
			float SpeedRight;
		} Movement;
	};

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

		void BeginFrame(const CameraSettings& camera, ID target);
		void Submit(ID mesh, ID material);
		void EndFrame();
	};
}