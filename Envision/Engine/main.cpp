#include "envision/envpch.h"
#include "envision/core/Application.h"

#include "envision/resource/ResourceManager.h"
#include "envision/graphics/AssetManager.h"

class TestLayer : public env::Layer
{
public:

	TestLayer() : env::Layer("TestLayer") {}
	~TestLayer() final = default;

public:

	void OnAttach() final
	{
		std::cout << "Attached" << std::endl;

		struct BufferDataStruct
		{
			float a = 5;
			float b = 2;
			bool c = true;
			int d = 42;
		} data;

		env::BufferLayout layout = {
			env::BufferElement("a", env::ShaderDataType::Float),
			env::BufferElement("b", env::ShaderDataType::Float),
			env::BufferElement("c", env::ShaderDataType::Bool),
			env::BufferElement("d", env::ShaderDataType::Int),
		};

		ID buffer = env::ResourceManager::Get()->CreateConstantBuffer("CBuffer", layout, &data);
	}

	void OnDetach() final
	{
		std::cout << "Detached" << std::endl;
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

		event.CallIf<env::KeyDownEvent>([](env::KeyDownEvent& e) {
			std::cout << "Key down: " << (int)e.Code << std::endl;
			return false;
		});
	}

};

class TestApplication : public env::Application
{
	env::Window* m_window;

public:

	TestApplication(int argc, char** argv) :
		env::Application(argc, argv, "TestApplication")
	{
		m_window = new env::Window(1200, 800, "Envision", *this);

		PushLayer(new TestLayer);
		PushWindow(m_window);

		ID target = env::ResourceManager::Get()->CreateWindowTarget("TargetWindow", m_window);
		ID mesh = env::AssetManager::Get()->CreateMesh("DefaultMesh");


		//ID material = m_assetManager->CreatePhongMaterial("DefaultMaterial");

		//m_renderer->BeginFrame(target);
		//m_renderer->Submit(mesh, material);
		//m_renderer->EndFrame();
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
