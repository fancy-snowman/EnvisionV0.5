#pragma once
#include "envision/envpch.h"
#include "envision/core/CommandList.h"
#include "envision/core/DescriptorAllocator.h"
#include "envision/core/IDGenerator.h"
#include "envision/core/Window.h"

namespace env
{
	// Singleton
	class GUI
	{
	private:

		struct FrameContext
		{
			ID Target = ID_ERROR;
			env::DirectList* DirectList = nullptr;
		};

		FrameContext m_context;
		env::DescriptorAllocator m_heap;

		env::Window* m_mainWindow = nullptr;
		ImGuiContext* m_imguiContext = nullptr;

	public:

		static GUI* Initialize(env::Window* window);
		static GUI* Get();
		static void Finalize();

	private:

		static GUI* s_instance;

		GUI(env::Window* window);
		~GUI();

		GUI(const GUI& other) = delete;
		GUI(const GUI&& other) = delete;
		GUI& operator=(const GUI& other) = delete;
		GUI& operator=(const GUI&& other) = delete;

	public:

		void BeginFrame(ID target);
		void EndFrame();

		inline env::DescriptorAllocator& GetDescriptorAllocator()
		{
			return m_heap;
		}
	};
}