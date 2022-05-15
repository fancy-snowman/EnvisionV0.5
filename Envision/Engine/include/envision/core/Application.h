#pragma once
#include "envision/envpch.h"
#include "envision/core/Layer.h"
#include "envision/core/Window.h"
#include "envision/graphics/Renderer.h"
#include "envision/graphics/AssetManager.h"
#include "envision/resource/ResourceManager.h"

int main(int argc, char** argv);

namespace env
{
	class Application
	{
		std::string m_name;
		std::vector<Layer*> m_layerStack;
		std::vector<Window*> m_windows;

		env::IDGenerator m_IDGenerator;

	protected:

		env::Renderer m_renderer;
		env::AssetManager m_assetManager;
		env::ResourceManager* m_resourceManager;

	public:

		Application(int argc, char** argv, const std::string& name);
		virtual ~Application();

	public:

		void PushLayer(Layer* layer);
		void PushWindow(Window* window);

	public:

		void PublishEvent(Event& event);

	private:
		
		void Run();
		friend int ::main(int argc, char** argv);
	};

	// This function has to be defined in by the client
	Application* CreateApplication(int argc, char** argv);
}