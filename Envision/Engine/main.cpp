#include "envision\envpch.h"
#include "envision\core\Application.h"

class TestLayer : public env::Layer
{
public:

	TestLayer() : env::Layer("TestLayer") {}
	~TestLayer() final = default;

public:

	void OnAttach() final
	{
		std::cout << "Attached" << std::endl;
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
		std::cout << "New event: " << event.GetTypeName() << std::endl;

		//event.CallIf<env::KeyDownEvent>([](auto& e) {
		//	std::cout << "New key down event" << std::endl;
		//	return false;
		//});
	}

};

class TestApplication : public env::Application
{
public:

	TestApplication(int argc, char** argv) :
		env::Application(argc, argv, "TestApplication")
	{
		PushLayer(new TestLayer);
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

	return 0;
}
