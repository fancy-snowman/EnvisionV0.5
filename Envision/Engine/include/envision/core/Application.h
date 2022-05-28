#pragma once
#include "envision/envpch.h"
#include "envision/core/IDGenerator.h"
#include "envision/core/Layer.h"
#include "envision/core/Time.h"
#include "envision/core/Window.h"

int main(int argc, char** argv);

namespace env
{
	class Application
	{
		std::string m_name;
		std::vector<Layer*> m_layerStack;
		std::vector<Window*> m_windows;

		env::IDGenerator m_IDGenerator;

	public:

		Application(int argc, char** argv, const std::string& name);
		virtual ~Application();

	public:

		virtual void OnUpdate(const Duration& delta) {};

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