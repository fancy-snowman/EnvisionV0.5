#include "envision/envpch.h"
#include "envision/core/Application.h"

#include "envision/resource/ResourceManager.h"
#include "envision/graphics/AssetManager.h"
#include "envision/graphics/Renderer.h"

class RenderLayer : public env::Layer
{
public:

	RenderLayer() : env::Layer("TestLayer") {}
	~RenderLayer() final = default;

public:

	void OnAttach() final
	{
		std::cout << "RenderLayer Attached" << std::endl;
	}

	void OnDetach() final
	{
		std::cout << "RenderLayer Detached" << std::endl;
	}

	void OnUpdate(const env::Duration& delta) final
	{
		//std::cout << "Updated with delta " << delta.InSeconds() << std::endl;

		//env::KeyUpEvent kdevent;
		//env::Event& e = (env::Event&)kdevent;
		//OnEvent(e);
	}

	void OnEvent(env::Event& event)
	{
		//std::cout << "New event: " << event.GetTypeName() << std::endl;

		//event.CallIf<env::KeyDownEvent>([](env::KeyDownEvent& e) {
		//	std::cout << "Key down: " << (int)e.Code << std::endl;
		//	return false;
		//});
	}

};

class TestApplication : public env::Application
{
	env::Window* m_window;

	ID m_target;
	ID m_mesh;
	ID m_material;

public:

	TestApplication(int argc, char** argv) :
		env::Application(argc, argv, "TestApplication")
	{
		m_window = new env::Window(1200, 800, "Envision", *this);

		m_target = env::ResourceManager::Get()->CreateWindowTarget("TargetWindow", m_window);
		m_mesh = env::AssetManager::Get()->CreateMesh("DefaultMesh");
		m_material = env::AssetManager::Get()->CreatePhongMaterial("DefaultMaterial");

		//PushLayer(new RenderLayer);
		PushWindow(m_window);
	}

	void OnUpdate(const env::Duration& delta) override
	{
		env::Renderer::Get()->BeginFrame(m_target);
		env::Renderer::Get()->Submit(m_mesh, m_material);
		env::Renderer::Get()->EndFrame();
	}

	~TestApplication() override = default;
};

env::Application* env::CreateApplication(int argc, char** argv)
{
	return new TestApplication(argc, argv);
}

int main(int argc, char** argv)
{
	env::Application* application = env::CreateApplication(argc, argv);
	application->Run();
	delete application;

	return 0;
}
