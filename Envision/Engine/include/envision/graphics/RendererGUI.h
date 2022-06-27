#pragma once
#include "envision/envpch.h"
#include "envision/core/IDGenerator.h"
#include "envision/core/CommandList.h"

namespace env
{
	// Singleton
	class RendererGUI
	{
	private:

		env::IDGenerator& m_commonIDGenerator;

		DirectList* m_directList = nullptr;
		ID3D12DescriptorHeap* m_imguiDescriptorHeap = nullptr;
		ImGuiContext* m_imguiContext = nullptr;

		ID m_target = ID_ERROR;

	public:

		static RendererGUI* Initialize(IDGenerator& commonIDGenerator);
		static RendererGUI* Get();
		static void Finalize();

	private:

		static RendererGUI* s_instance;

		RendererGUI(env::IDGenerator& commonIDGenerator);
		~RendererGUI();

		RendererGUI(const RendererGUI& other) = delete;
		RendererGUI(const RendererGUI&& other) = delete;
		RendererGUI& operator=(const RendererGUI& other) = delete;
		RendererGUI& operator=(const RendererGUI&& other) = delete;

	public:

		void BeginFrame(ID target);
		void EndFrame();
	};
}