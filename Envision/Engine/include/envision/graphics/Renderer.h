#pragma once
#include "envision/envpch.h"
#include "envision/core/Camera.h"
#include "envision/core/DescriptorAllocator.h"
#include "envision/core/GPU.h"
#include "envision/core/IDGenerator.h"
#include "envision/core/Window.h"
#include "envision/graphics/Assets.h"
#include "envision/graphics/CoreShaderDataStructures.h"
#include "envision/graphics/FramePacket.h"
#include "envision/resource/Resource.h"

namespace env
{
	// Singleton
	class Renderer
	{
	private:

		env::IDGenerator& m_commonIDGenerator;

		const UINT ROOT_INDEX_INSTANCE_OFFSET_CONSTANT = 0;
		const UINT ROOT_INDEX_INSTANCE_TABLE = 1;
		const UINT ROOT_INDEX_MATERIAL_TABLE = 2;
		const UINT ROOT_INDEX_CAMERA_TABLE = 3;
		const UINT ROOT_INDEX_SAMPLER_TABLE = 4;
		const UINT ROOT_INDEX_TEXTURE_TABLE = 5;

		ID m_pipelineState;
		ID m_sampler;

		static const int NUM_FRAME_PACKETS = 2;
		int m_currentFramePacketIndex = 0;
		std::array<FramePacket, NUM_FRAME_PACKETS> m_framePackets;
		std::array<DescriptorAllocator, NUM_FRAME_PACKETS> m_descriptorAllocators;
		std::array<DescriptorAllocator, NUM_FRAME_PACKETS> m_samplerAllocators;
		std::array<ID, NUM_FRAME_PACKETS> m_uploadBuffers;

		std::array<CopyList*, NUM_FRAME_PACKETS> m_copyLists;
		std::array<DirectList*, NUM_FRAME_PACKETS> m_directLists;

		char* m_intermediateUploadMemory = nullptr;
		UINT m_intermediateUploadMemoryByteWidth = 0;

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

	private:

		int StepCurrentFramePacketIndex();
		void ClearCurrentFramePacket();
		FramePacket& GetCurrentFramePacket();

	public:

		void Initialize();

		void BeginFrame(const CameraSettings& cameraSettings, Transform& cameraTransform, ID target);
		void BeginFrame(const CameraSettings& cameraSettings, Transform& cameraTransform, Window* target);
		void Submit(Transform& transform, ID mesh, ID material);
		void EndFrame();
	};
}