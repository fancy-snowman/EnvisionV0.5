#include "envision/envpch.h"
#include "envision/core/Application.h"

#include "envision/resource/ResourceManager.h"
#include "envision/graphics/AssetManager.h"
#include "envision/graphics/Renderer.h"

#include <assimp/Importer.hpp>

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
		Assimp::Importer importer;
		
		m_window = new env::Window(1200, 800, "Envision", *this);

		m_target = env::ResourceManager::Get()->CreateWindowTarget("TargetWindow", m_window);

		struct Vertex {
			struct {
				float x, y, z;
			} Position;

			struct {
				float r, g, b;
			} Color;

			struct {
				float x, y, z;
			} Normal;
		};

		std::vector<Vertex> vertices = {
			// Front
			{ {  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  1.0f } },
			{ {  1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  1.0f } },
			{ { -1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  1.0f } },
			{ {  1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  1.0f } },
			{ { -1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  1.0f } },
			{ { -1.0f, -1.0f,  1.0f }, { 1.0f, 0.0f, 0.0f }, {  0.0f,  0.0f,  1.0f } },

			// Back
			{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, {  0.0f,  0.0f, -1.0f } },
			{ { -1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, {  0.0f,  0.0f, -1.0f } },
			{ {  1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, {  0.0f,  0.0f, -1.0f } },
			{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, {  0.0f,  0.0f, -1.0f } },
			{ {  1.0f,  1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, {  0.0f,  0.0f, -1.0f } },
			{ {  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, {  0.0f,  0.0f, -1.0f } },

			// Left
			{ { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f,  0.0f,  0.0f } },
			{ { -1.0f,  1.0f,  1.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f,  0.0f,  0.0f } },
			{ { -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f,  0.0f,  0.0f } },
			{ { -1.0f, -1.0f,  1.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f,  0.0f,  0.0f } },
			{ { -1.0f,  1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f,  0.0f,  0.0f } },
			{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }, { -1.0f,  0.0f,  0.0f } },

			// Right
			{ {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f }, {  1.0f,  0.0f,  0.0f } },
			{ {  1.0f,  1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f }, {  1.0f,  0.0f,  0.0f } },
			{ {  1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f }, {  1.0f,  0.0f,  0.0f } },
			{ {  1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 0.0f }, {  1.0f,  0.0f,  0.0f } },
			{ {  1.0f,  1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f }, {  1.0f,  0.0f,  0.0f } },
			{ {  1.0f, -1.0f,  1.0f }, { 1.0f, 1.0f, 0.0f }, {  1.0f,  0.0f,  0.0f } },

			// Up
			{ { -1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 1.0f }, {  0.0f,  1.0f,  0.0f } },
			{ { -1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 1.0f }, {  0.0f,  1.0f,  0.0f } },
			{ {  1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 1.0f }, {  0.0f,  1.0f,  0.0f } },
			{ { -1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 1.0f }, {  0.0f,  1.0f,  0.0f } },
			{ {  1.0f,  1.0f,  1.0f }, { 1.0f, 0.0f, 1.0f }, {  0.0f,  1.0f,  0.0f } },
			{ {  1.0f,  1.0f, -1.0f }, { 1.0f, 0.0f, 1.0f }, {  0.0f,  1.0f,  0.0f } },

			// Down
			{ { -1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 1.0f }, {  0.0f, -1.0f,  0.0f } },
			{ { -1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 1.0f }, {  0.0f, -1.0f,  0.0f } },
			{ {  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 1.0f }, {  0.0f, -1.0f,  0.0f } },
			{ { -1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 1.0f }, {  0.0f, -1.0f,  0.0f } },
			{ {  1.0f, -1.0f, -1.0f }, { 0.0f, 1.0f, 1.0f }, {  0.0f, -1.0f,  0.0f } },
			{ {  1.0f, -1.0f,  1.0f }, { 0.0f, 1.0f, 1.0f }, {  0.0f, -1.0f,  0.0f } },
		};

		std::vector<UINT> indices;
		for (UINT i = 0; i < (UINT)vertices.size(); i++)
			indices.push_back(i);

		env::BufferLayout vertexLayout({
			{ "position", env::ShaderDataType::Float3 },
			{ "color", env::ShaderDataType::Float3 },
			{ "normal", env::ShaderDataType::Float3 } },
			vertices.size());

		m_mesh = env::AssetManager::Get()->CreateMesh("Box",
			vertices.data(),
			vertexLayout,
			indices.data(),
			indices.size());

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
