#pragma once
#include "envision\envpch.h"
#include "envision\core\Layer.h"
#include "envision\resource\ResourceManager.h"

int main(int argc, char** argv);

namespace env
{
	class Application
	{
		std::string m_name;
		std::vector<Layer*> m_layerStack;

	protected:

		ResourceManager* m_resourceManger;

	public:

		Application(int argc, char** argv, const std::string& name);
		virtual ~Application();

	public:

		void PushLayer(Layer* layer);

	public:

		void PublishEvent(Event& event);

	private:
		
		void Run();
		friend int ::main(int argc, char** argv);
	};

	// This function has to be defined in by the client
	Application* CreateApplication(int argc, char** argv);
}