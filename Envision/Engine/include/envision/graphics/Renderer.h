#pragma once
#include "envision/envpch.h"
#include "envision/core/GPU.h"
#include "envision/core/DescriptorAllocator.h"
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

		ID m_cameraBuffer;
		ID m_objectBuffer;
		ID m_phongBuffer;
		
		struct {
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

		void BeginFrame(ID target);
		void Submit(ID mesh, ID material);
		void EndFrame();
	};
}