#include "envision\envpch.h"
#include "envision\core\Application.h"
#include "envision\core\Time.h"

env::Application::Application(int argc, char** argv, const std::string& name) :
	m_name(name)
{
	//
}

env::Application::~Application()
{
	for (auto& l : m_layerStack)
	{
		l->OnDetach();
		delete l;
		l = nullptr;
	}
}

void env::Application::PushLayer(Layer* layer)
{
	m_layerStack.push_back(layer);
	m_layerStack.back()->OnAttach();
}

void env::Application::Run()
{
	bool running = true;
	Timepoint past = Time::Now();

	while (running)
	{
		Timepoint now = Time::Now();
		Duration delta = now - past;
		past = now;

		for (auto& l : m_layerStack)
		{
			l->OnUpdate(delta);
		}
	}
}
