#include "envision/envpch.h"
#include "envision/core/Application.h"
#include "envision/core/GPU.h"
#include "envision/core/Time.h"
#include "envision/graphics/AssetManager.h"
#include "envision/graphics/Renderer.h"
#include "envision/graphics/GUI.h"
#include "envision/resource/ResourceManager.h"

env::Application::Application(int argc, char** argv, const std::string& name) :
	m_name(name)
{
	GPU::Initialize();
	ResourceManager::Initialize(m_IDGenerator);
	AssetManager::Initialize(m_IDGenerator);
	Renderer::Initialize(m_IDGenerator);

	m_activeScene = new Scene();
}

env::Application::~Application()
{
	for (auto& l : m_systemStack)
	{
		l->OnDetach(*m_activeScene);
		delete l;
		l = nullptr;
	}
}

void env::Application::PushSystem(System* layer)
{
	m_systemStack.push_back(layer);
	m_systemStack.back()->OnAttach(*m_activeScene);
}

void env::Application::PushWindow(Window* window)
{
	m_windows.push_back(window);
	m_windows.back()->OnAttach();
}

env::Scene* env::Application::GetActiveScene()
{
	return m_activeScene;
}

void env::Application::PublishEvent(Event& event)
{
	for (auto& l : m_systemStack)
	{
		if (event.Handled)
			break;
		l->OnEvent(*m_activeScene, event);
	}
}

void env::Application::Run()
{
	bool running = true;
	Timepoint past = Time::Now();

	while (running)
	{
		for (auto& w : m_windows)
		{
			w->OnEventUpdate();
			w->OnUpdate();
		}

		MSG msg = { 0 };
		while (PeekMessageA(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		Timepoint now = Time::Now();
		Duration delta = now - past;
		past = now;

		// Update application before its layers
		this->OnUpdate(delta);

		for (auto& l : m_systemStack)
		{
			l->OnUpdate(*m_activeScene, delta);
		}

		for (auto& w : m_windows)
		{
			w->Present();
		}
	}
}
