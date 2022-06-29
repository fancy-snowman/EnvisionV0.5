#include "envision/envpch.h"
#include "envision/core/Application.h"
#include "envision/core/Scene.h"
#include "envision/core/Component.h"

#include "envision/resource/ResourceManager.h"
#include "envision/graphics/AssetManager.h"
#include "envision/graphics/Renderer.h"
#include "envision/graphics/RendererGUI.h"

class SceneUpdateLayer : public env::System
{
private:

	struct {

		struct {
			float Horizontal = 0.0f;
			float Vertical = 0.0f;
		} Rotation;

		struct {
			float Forward = 0.0f;
			float Up = 0.0f;
			float Right = 0.0f;
		} Movement;

	} m_cameraDelta;

	struct {
		bool W : 1;
		bool A : 1;
		bool S : 1;
		bool D : 1;

		bool Up : 1;
		bool Left : 1;
		bool Down : 1;
		bool Right : 1;
	} m_keyDownStates = { 0 };

public:

	SceneUpdateLayer() : env::System("TestLayer") {}
	~SceneUpdateLayer() final = default;

public:

	void OnAttach(env::Scene& scene) final
	{
		std::cout << "SceneUpdateLayer Attached" << std::endl;
	}

	void OnDetach(env::Scene& scene) final
	{
		std::cout << "SceneUpdateLayer Detached" << std::endl;
	}

	void OnUpdate(env::Scene& scene, const env::Duration& delta) final
	{
		scene.ForEach<env::CameraControllerComponent, env::CameraComponent, env::TransformComponent>(
			[&](env::CameraControllerComponent& controller, env::CameraComponent& camera, env::TransformComponent& transform) {
				if (m_keyDownStates.W)
					m_cameraDelta.Movement.Forward += controller.SpeedForward * delta.InSeconds();
				if (m_keyDownStates.A)
					m_cameraDelta.Movement.Right -= controller.SpeedRight * delta.InSeconds();
				if (m_keyDownStates.S)
					m_cameraDelta.Movement.Forward -= controller.SpeedForward * delta.InSeconds();
				if (m_keyDownStates.D)
					m_cameraDelta.Movement.Right += controller.SpeedRight * delta.InSeconds();

				if (m_keyDownStates.Up)
					m_cameraDelta.Rotation.Vertical -= controller.TurnSpeedVertical * delta.InSeconds();
				if (m_keyDownStates.Left)
					m_cameraDelta.Rotation.Horizontal -= controller.TurnSpeedHorizontal * delta.InSeconds();
				if (m_keyDownStates.Down)
					m_cameraDelta.Rotation.Vertical += controller.TurnSpeedVertical * delta.InSeconds();
				if (m_keyDownStates.Right)
					m_cameraDelta.Rotation.Horizontal += controller.TurnSpeedHorizontal * delta.InSeconds();

				transform.Transformation.RotateAxisY(m_cameraDelta.Rotation.Horizontal);
				transform.Transformation.RotatePitch(m_cameraDelta.Rotation.Vertical);

				if (m_cameraDelta.Movement.Forward != 0.0f)
					transform.Transformation.TranslateForward(m_cameraDelta.Movement.Forward);
				if (m_cameraDelta.Movement.Up != 0.0f)
					transform.Transformation.TranslateUp(m_cameraDelta.Movement.Up);
				if (m_cameraDelta.Movement.Right != 0.0f)
					transform.Transformation.TranslateRight(m_cameraDelta.Movement.Right);
			});

		m_cameraDelta.Rotation.Horizontal = 0.0f;
		m_cameraDelta.Rotation.Vertical = 0.0f;
		m_cameraDelta.Movement.Forward = 0.0f;
		m_cameraDelta.Movement.Up = 0.0f;
		m_cameraDelta.Movement.Right = 0.0f;
	}

	void OnEvent(env::Scene& scene, env::Event& event) final
	{
		event.CallIf<env::KeyDownEvent>([&](env::KeyDownEvent& e) {
			if (e.Code == env::KeyCode::W) m_keyDownStates.W = true;
			else if (e.Code == env::KeyCode::A) m_keyDownStates.A = true;
			else if (e.Code == env::KeyCode::S) m_keyDownStates.S = true;
			else if (e.Code == env::KeyCode::D) m_keyDownStates.D = true;

			else if (e.Code == env::KeyCode::Up) m_keyDownStates.Up = true;
			else if (e.Code == env::KeyCode::Left) m_keyDownStates.Left = true;
			else if (e.Code == env::KeyCode::Down) m_keyDownStates.Down = true;
			else if (e.Code == env::KeyCode::Right) m_keyDownStates.Right = true;
			return false;
		});

		event.CallIf<env::KeyUpEvent>([&](env::KeyUpEvent& e) {
			if (e.Code == env::KeyCode::W) m_keyDownStates.W = false;
			else if (e.Code == env::KeyCode::A) m_keyDownStates.A = false;
			else if (e.Code == env::KeyCode::S) m_keyDownStates.S = false;
			else if (e.Code == env::KeyCode::D) m_keyDownStates.D = false;

			else if (e.Code == env::KeyCode::Up) m_keyDownStates.Up = false;
			else if (e.Code == env::KeyCode::Left) m_keyDownStates.Left = false;
			else if (e.Code == env::KeyCode::Down) m_keyDownStates.Down = false;
			else if (e.Code == env::KeyCode::Right) m_keyDownStates.Right = false;
			return false;
		});

		event.CallIf<env::MouseMoveEvent>([&](env::MouseMoveEvent& e) {
			if (e.Modifiers.RightMouse) {
				m_cameraDelta.Rotation.Horizontal -= e.DeltaX * 0.005f;
				m_cameraDelta.Rotation.Vertical -= e.DeltaY * 0.005f;
			}

			if (e.Modifiers.MiddleMouse) {
				m_cameraDelta.Movement.Right += e.DeltaX;
				m_cameraDelta.Movement.Up -= e.DeltaY;
			}

			return false;
		});

		event.CallIf<env::MouseScrollEvent>([&](env::MouseScrollEvent& e) {
			m_cameraDelta.Movement.Forward += e.Delta * 50.0f;
			return false;
		});
	}

};

class TestApplication : public env::Application
{
	env::Window* m_window;

	ID m_target;
	ID m_mainCamera;

public:

	TestApplication(int argc, char** argv) :
		env::Application(argc, argv, "TestApplication")
	{
	
		
		//GetActiveScene()->LoadScene("Helicopter", "assets/SM_helicopter_01.fbx");
		//m_mesh = env::AssetManager::Get()->LoadMesh("City", "assets/city.fbx");

		GetActiveScene()->LoadScene("City", "assets/Polygon-City Megapolis.fbx");
		
		m_window = new env::Window(1200, 800, "Envision", *this);

		m_target = env::ResourceManager::Get()->CreateWindowTarget("TargetWindow", m_window);

		env::Scene* scene = GetActiveScene();
		
		m_mainCamera = scene->CreateEntity("Camera");

		env::CameraControllerComponent cameraController;
		cameraController.TurnSpeedHorizontal = 3.14f;
		cameraController.TurnSpeedVertical = 3.14f;
		cameraController.SpeedForward = 5000.0f;
		cameraController.SpeedUp = 1000.0f;
		cameraController.SpeedRight = 1000.0f;
		scene->SetComponent<env::CameraControllerComponent>(m_mainCamera, cameraController);

		env::CameraComponent camera;
		camera.Settings.FieldOfView = 3.14f / 2.0f;
		camera.Settings.DistanceNearPlane = 10.0f;
		camera.Settings.DistanceFarPlane = 100000.0f;
		camera.Settings.Orthographic = false;
		scene->SetComponent<env::CameraComponent>(m_mainCamera, camera);

		env::TransformComponent cameraTransform;
		cameraTransform.Transformation.SetPosition({ 5000.0f, 5000.0f, 5000.0f });
		cameraTransform.Transformation.RotateAxisY(-2.0f * 3.14f / 3.0f);
		cameraTransform.Transformation.RotatePitch(2.0f * 3.14f / 12.0f);
		scene->SetComponent<env::TransformComponent>(m_mainCamera, cameraTransform);

		PushSystem(new SceneUpdateLayer());
		PushWindow(m_window);
	}

	void OnUpdate(const env::Duration& delta) override
	{
		static const float TARGET_FRAME_TIME = 1.f / 120.f;
		static float deltaSum = 0.0f;
		deltaSum += delta.InSeconds();

		static float FPS_time = 0.f;
		static int FPS_numFrames = 0;
		static float FPS_frameTime = 0.0f;
		static int FPS_fps = 0;

		FPS_time += delta.InSeconds();

		if (deltaSum >= TARGET_FRAME_TIME) {
			deltaSum -= TARGET_FRAME_TIME;

			env::Scene* scene = GetActiveScene();
			env::CameraSettings& cameraSettings = scene->GetComponent<env::CameraComponent>(m_mainCamera).Settings;
			env::Transform& cameraTransform = scene->GetComponent<env::TransformComponent>(m_mainCamera).Transformation;
			//env::Transform cameraTransform;
			env::Transform objectTransform;

			env::Renderer::Get()->BeginFrame(cameraSettings, cameraTransform, m_target);

			// <MeshID, count>
			std::unordered_map<ID, int> instances;
			scene->ForEach<env::RenderComponent, env::TransformComponent>([&](env::RenderComponent& render, env::TransformComponent& transform) {
				env::Renderer::Get()->Submit(transform.Transformation, render.Mesh, render.Material);
				
				if (instances.count(render.Mesh) == 0)
					instances[render.Mesh] = 1;
				else
					++instances[render.Mesh];
			});

			env::Renderer::Get()->EndFrame();
			
			env::RendererGUI::Get()->BeginFrame(m_target);

			ImGui::Begin("Mesh instances");
			for (auto& instance : instances) {
				env::Mesh* mesh = env::AssetManager::Get()->GetMesh(instance.first);
				ImGui::Text("%i\t%i\t%s", instance.second, instance.first, mesh->Name.c_str());
			}
			ImGui::End();
			
			ImGui::Begin("Camera settings");
			float fovDegress = (cameraSettings.FieldOfView * 180.0f) / 3.14f;
			ImGui::SliderFloat("FOV", &fovDegress, 1.0f, 179.0f, "%.0f deg", 1.0f);
			//ImGui::SliderAngle("FOV", &fovDegress, 30.0f, 180.0f, );
			cameraSettings.FieldOfView = (fovDegress / 180.0f) * 3.14f;
			ImGui::End();

			if (++FPS_numFrames >= 100) {
				FPS_fps = (float)FPS_numFrames / FPS_time;
				FPS_frameTime = FPS_time / (float)FPS_numFrames;
				FPS_frameTime *= 1000.f;

				FPS_time = 0.0f;
				FPS_numFrames = 0;
			}
			ImGui::Begin("Frame statistics (100 frames)");
			ImGui::Text("Frametime: %.0f ms (%i FPS)", FPS_frameTime, FPS_fps);
			ImGui::End();
			
			env::RendererGUI::Get()->EndFrame();

			env::CommandQueue& queue = env::GPU::GetPresentQueue();
			queue.Execute();
			queue.WaitForIdle();

			
		}
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
