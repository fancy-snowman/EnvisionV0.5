#include "envision/envpch.h"
#include "envision/core/Application.h"

#include "envision/resource/ResourceManager.h"
#include "envision/graphics/AssetManager.h"
#include "envision/graphics/Renderer.h"

class SceneUpdateLayer : public env::Layer
{
private:

	env::CameraSettings& m_commonCamera;

	struct {
		float Roll = 0.0f;
		float Pitch = 0.0f;
		float Yaw = 0.0f;
		float Forward = 0.0f;
		float Up = 0.0f;
		float Right = 0.0f;
	} m_cameraDelta;

	struct {
		bool W : 1;
		bool A : 1;
		bool S : 1;
		bool D : 1;
	} m_keyDownStates = { 0 };

public:

	SceneUpdateLayer(env::CameraSettings& camera) : m_commonCamera(camera), env::Layer("TestLayer") {}
	~SceneUpdateLayer() final = default;

public:

	void OnAttach() final
	{
		std::cout << "SceneUpdateLayer Attached" << std::endl;
	}

	void OnDetach() final
	{
		std::cout << "SceneUpdateLayer Detached" << std::endl;
	}

	void OnUpdate(const env::Duration& delta) final
	{
		if (m_keyDownStates.W)
			m_cameraDelta.Forward += m_commonCamera.Movement.SpeedForward * delta.InSeconds();
		if (m_keyDownStates.A)
			m_cameraDelta.Right -= m_commonCamera.Movement.SpeedRight * delta.InSeconds();
		if (m_keyDownStates.S)
			m_cameraDelta.Forward -= m_commonCamera.Movement.SpeedForward * delta.InSeconds();
		if (m_keyDownStates.D)
			m_cameraDelta.Right += m_commonCamera.Movement.SpeedRight * delta.InSeconds();


		m_commonCamera.Transform.RotationRollPitchYaw.z +=
			m_cameraDelta.Yaw * m_commonCamera.Movement.TurnSpeedHorizontal * delta.InSeconds();
		m_commonCamera.Transform.RotationRollPitchYaw.y +=
			m_cameraDelta.Pitch * m_commonCamera.Movement.TurnSpeedVertical * delta.InSeconds();

		if (m_cameraDelta.Forward != 0.0f)
			m_commonCamera.Transform.MoveForward(m_cameraDelta.Forward
				* m_commonCamera.Movement.SpeedForward
				* delta.InSeconds());
		if (m_cameraDelta.Up != 0.0f)
			m_commonCamera.Transform.MoveUp(m_cameraDelta.Up
				* m_commonCamera.Movement.SpeedUp
				* delta.InSeconds());
		if (m_cameraDelta.Right != 0.0f)
			m_commonCamera.Transform.MoveRight(m_cameraDelta.Right
				* m_commonCamera.Movement.SpeedRight
				* delta.InSeconds());

		m_cameraDelta.Yaw = 0.0f;
		m_cameraDelta.Pitch = 0.0f;
		m_cameraDelta.Forward = 0.0f;
		m_cameraDelta.Up = 0.0f;
		m_cameraDelta.Right = 0.0f;
	}

	void OnEvent(env::Event& event)
	{
		event.CallIf<env::KeyDownEvent>([&](env::KeyDownEvent& e) {
			if (e.Code == env::KeyCode::W) m_keyDownStates.W = true;
			else if (e.Code == env::KeyCode::A) m_keyDownStates.A = true;
			else if (e.Code == env::KeyCode::S) m_keyDownStates.S = true;
			else if (e.Code == env::KeyCode::D) m_keyDownStates.D = true;
			return false;
		});

		event.CallIf<env::KeyUpEvent>([&](env::KeyUpEvent& e) {
			if (e.Code == env::KeyCode::W) m_keyDownStates.W = false;
			else if (e.Code == env::KeyCode::A) m_keyDownStates.A = false;
			else if (e.Code == env::KeyCode::S) m_keyDownStates.S = false;
			else if (e.Code == env::KeyCode::D) m_keyDownStates.D = false;
			return false;
		});

		event.CallIf<env::MouseMoveEvent>([&](env::MouseMoveEvent& e) {
			if (e.Modifiers.RightMouse) {
				m_cameraDelta.Yaw -= e.DeltaX;
				m_cameraDelta.Pitch -= e.DeltaY;
			}

			if (e.Modifiers.MiddleMouse) {
				m_cameraDelta.Right += e.DeltaX;
				m_cameraDelta.Up -= e.DeltaY;
			}

			return false;
		});

		event.CallIf<env::MouseScrollEvent>([&](env::MouseScrollEvent& e) {
			m_cameraDelta.Forward += e.Delta * 20.0f;
			return false;
		});
	}

};

class TestApplication : public env::Application
{
	env::Window* m_window;

	ID m_target;
	ID m_mesh;
	ID m_material;

	env::CameraSettings m_camera;

public:

	TestApplication(int argc, char** argv) :
		env::Application(argc, argv, "TestApplication")
	{
		
		m_mesh = env::AssetManager::Get()->LoadMesh("Helicopter", "assets/SM_helicopter_01.fbx");
		
		m_window = new env::Window(1200, 800, "Envision", *this);

		m_target = env::ResourceManager::Get()->CreateWindowTarget("TargetWindow", m_window);

		m_material = env::AssetManager::Get()->CreatePhongMaterial("DefaultMaterial");

		m_camera.Transform.Position = { 0.0f, 250.f, -500.0 };
		m_camera.Transform.RotationRollPitchYaw = { 0.0f, 0.0f, 0.0f };
		m_camera.Projection.FieldOfView = 3.14f / 2.0f;
		m_camera.Projection.DistanceNearPlane = 10.0f;
		m_camera.Projection.DistanceFarPlane = 1000.0f;
		m_camera.Projection.Orthographic = false;
		m_camera.Movement.TurnSpeedHorizontal = 0.002f;
		m_camera.Movement.TurnSpeedVertical = 0.002f;
		m_camera.Movement.SpeedRight = 0.8f;
		m_camera.Movement.SpeedUp = 0.8f;
		m_camera.Movement.SpeedForward = 0.8f;

		PushLayer(new SceneUpdateLayer(m_camera));
		PushWindow(m_window);
	}

	void OnUpdate(const env::Duration& delta) override
	{
		//m_camera.Transform.RotationRollPitchYaw.z += delta.InSeconds() * 0.001f;

		env::Renderer::Get()->BeginFrame(m_camera, m_target);
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
