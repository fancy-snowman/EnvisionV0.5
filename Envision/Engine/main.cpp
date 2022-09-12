#include "envision/envpch.h"
#include "envision/core/Application.h"
#include "envision/core/Scene.h"
#include "envision/core/Component.h"

#include "envision/resource/ResourceManager.h"
#include "envision/graphics/AssetManager.h"
#include "envision/graphics/Renderer.h"
#include "envision/graphics/RendererGUI.h"

//struct ApplicationData
//{
//	ID MainCameraEntity = ID_ERROR;
//	ID MainTargetResource = ID_ERROR;
//};
//
//class RenderLayer : public env::System
//{
//private:
//
//	// Should this be sent to the renderer? Should a "bundle of lists" be created to hide this? Not desided yet..
//	static const int NUM_FRAME_PACKETS = 2;
//	int m_currentFramePacket = 0;
//	std::array<env::DirectList*, 2> m_presentLists;
//
//	bool ValidSanityData(env::Scene& scene, ApplicationData& appData)
//	{
//		bool foundAppData = false;
//		scene.ForEach<ApplicationData>([&](const ApplicationData& data) {
//			foundAppData = true;
//			appData = data;
//		});
//
//		if (!foundAppData) {
//			std::cout << "[RenderLayer] No app data found.\n";
//			return false;
//		}
//
//		if (!env::ResourceManager::Get()->GetTarget(appData.MainTargetResource)) {
//			std::cout << "[RenderLayer] Non-existing main target in app data.\n";
//			return false;
//		}
//
//		if (!scene.IsEntity(appData.MainCameraEntity)) {
//			std::cout << "[RenderLayer] Non-existing main camera in app data.\n";
//			return false;
//		}
//
//		if (!scene.HasComponents<env::CameraComponent, env::TransformComponent>(appData.MainCameraEntity)) {
//			std::cout << "[RenderLayer] Main camera entity is missing some or all of the following components:\n"
//				<< "  - env::CameraComponent\n"
//				<< "  - env::TransformComponent\n";
//			return false;
//		}
//		return true;
//	}
//
//public:
//
//	RenderLayer() : env::System("RenderLayer")
//	{
//		for (int i = 0; i < NUM_FRAME_PACKETS; i++) {
//			m_presentLists[i] = env::GPU::CreateDirectCommandList("RenderLayer::PresentList_" + std::to_string(i));
//		}
//	}
//	~RenderLayer() final = default;
//
//
//public:
//
//	void OnUpdate(env::Scene& scene, const env::Duration& delta) final
//	{
//		static int targetFPS = 120;
//		static float targetFrameTime = 1.f / (float)targetFPS;
//		static float deltaSum = 0.0f;
//		deltaSum += delta.InSeconds();
//
//		if (deltaSum < targetFrameTime)
//			return;
//		deltaSum -= targetFrameTime;
//
//		ApplicationData appData;
//		if (!ValidSanityData(scene, appData))
//			return;
//
//		env::WindowTarget* target =
//			env::ResourceManager::Get()->GetTarget(appData.MainTargetResource);
//		env::CameraSettings& cameraSettings =
//			scene.GetComponent<env::CameraComponent>(appData.MainCameraEntity).Settings;
//		env::Transform& cameraTransform =
//			scene.GetComponent<env::TransformComponent>(appData.MainCameraEntity).Transformation;
//
//		env::DirectList* presentList = m_presentLists[m_currentFramePacket];
//
//		env::CommandQueue& presentQueue = env::GPU::GetPresentQueue();
//		presentQueue.WaitForFenceValueGPUSide(presentQueue.GetFenceValue()); // TODO: Wait for render queue instead when that's implemented
//		presentList->Reset();
//		presentList->TransitionResource(target, D3D12_RESOURCE_STATE_RENDER_TARGET);
//		presentList->Close();
//		presentQueue.QueueList(presentList);
//		presentQueue.Execute();
//		presentQueue.IncrementFence();
//
//		env::Renderer::Get()->BeginFrame(cameraSettings, cameraTransform, appData.MainTargetResource);
//
//		// <MeshID, count>
//		std::unordered_map<ID, int> instances;
//		scene.ForEach<env::RenderComponent, env::TransformComponent>([&](env::RenderComponent& render, env::TransformComponent& transform) {
//			env::Renderer::Get()->Submit(transform.Transformation, render.Mesh, render.Material);
//
//			if (instances.count(render.Mesh) == 0)
//				instances[render.Mesh] = 1;
//			else
//				++instances[render.Mesh];
//		});
//
//		env::Renderer::Get()->EndFrame();
//
//		// Set target to state PRESENT
//		presentQueue.WaitForFenceValueGPUSide(presentQueue.GetFenceValue()); // TODO: Wait for render queue instead when that's implemented
//		presentQueue.WaitForIdle();
//		presentList->Reset();
//		presentList->TransitionResource(target, D3D12_RESOURCE_STATE_PRESENT);
//		presentList->Close();
//		presentQueue.QueueList(presentList);
//		presentQueue.Execute();
//		presentQueue.IncrementFence();
//
//		m_currentFramePacket = (m_currentFramePacket + 1) % NUM_FRAME_PACKETS;
//	}
//};
//
//class SceneUpdateLayer : public env::System
//{
//private:
//
//	struct {
//
//		struct {
//			float Horizontal = 0.0f;
//			float Vertical = 0.0f;
//		} Rotation;
//
//		struct {
//			float Forward = 0.0f;
//			float Up = 0.0f;
//			float Right = 0.0f;
//		} Movement;
//
//	} m_cameraDelta;
//
//	struct {
//		bool W : 1;
//		bool A : 1;
//		bool S : 1;
//		bool D : 1;
//
//		bool Up : 1;
//		bool Left : 1;
//		bool Down : 1;
//		bool Right : 1;
//	} m_keyDownStates = { 0 };
//
//public:
//
//	SceneUpdateLayer() : env::System("TestLayer") {}
//	~SceneUpdateLayer() final = default;
//
//public:
//
//	void OnAttach(env::Scene& scene) final
//	{
//		std::cout << "SceneUpdateLayer Attached" << std::endl;
//	}
//
//	void OnDetach(env::Scene& scene) final
//	{
//		std::cout << "SceneUpdateLayer Detached" << std::endl;
//	}
//
//	void OnUpdate(env::Scene& scene, const env::Duration& delta) final
//	{
//		scene.ForEach<env::CameraControllerComponent, env::CameraComponent, env::TransformComponent>(
//			[&](env::CameraControllerComponent& controller, env::CameraComponent& camera, env::TransformComponent& transform) {
//				if (m_keyDownStates.W)
//					m_cameraDelta.Movement.Forward += controller.SpeedForward * delta.InSeconds();
//				if (m_keyDownStates.A)
//					m_cameraDelta.Movement.Right -= controller.SpeedRight * delta.InSeconds();
//				if (m_keyDownStates.S)
//					m_cameraDelta.Movement.Forward -= controller.SpeedForward * delta.InSeconds();
//				if (m_keyDownStates.D)
//					m_cameraDelta.Movement.Right += controller.SpeedRight * delta.InSeconds();
//
//				if (m_keyDownStates.Up)
//					m_cameraDelta.Rotation.Vertical -= controller.TurnSpeedVertical * delta.InSeconds();
//				if (m_keyDownStates.Left)
//					m_cameraDelta.Rotation.Horizontal -= controller.TurnSpeedHorizontal * delta.InSeconds();
//				if (m_keyDownStates.Down)
//					m_cameraDelta.Rotation.Vertical += controller.TurnSpeedVertical * delta.InSeconds();
//				if (m_keyDownStates.Right)
//					m_cameraDelta.Rotation.Horizontal += controller.TurnSpeedHorizontal * delta.InSeconds();
//
//				transform.Transformation.RotateAxisY(m_cameraDelta.Rotation.Horizontal);
//				transform.Transformation.RotatePitch(m_cameraDelta.Rotation.Vertical);
//
//				if (m_cameraDelta.Movement.Forward != 0.0f)
//					transform.Transformation.TranslateForward(m_cameraDelta.Movement.Forward);
//				if (m_cameraDelta.Movement.Up != 0.0f)
//					transform.Transformation.TranslateUp(m_cameraDelta.Movement.Up);
//				if (m_cameraDelta.Movement.Right != 0.0f)
//					transform.Transformation.TranslateRight(m_cameraDelta.Movement.Right);
//			});
//
//		m_cameraDelta.Rotation.Horizontal = 0.0f;
//		m_cameraDelta.Rotation.Vertical = 0.0f;
//		m_cameraDelta.Movement.Forward = 0.0f;
//		m_cameraDelta.Movement.Up = 0.0f;
//		m_cameraDelta.Movement.Right = 0.0f;
//	}
//
//	void OnEvent(env::Scene& scene, env::Event& event) final
//	{
//		event.CallIf<env::KeyDownEvent>([&](env::KeyDownEvent& e) {
//			if (e.Code == env::KeyCode::W) m_keyDownStates.W = true;
//			else if (e.Code == env::KeyCode::A) m_keyDownStates.A = true;
//			else if (e.Code == env::KeyCode::S) m_keyDownStates.S = true;
//			else if (e.Code == env::KeyCode::D) m_keyDownStates.D = true;
//
//			else if (e.Code == env::KeyCode::Up) m_keyDownStates.Up = true;
//			else if (e.Code == env::KeyCode::Left) m_keyDownStates.Left = true;
//			else if (e.Code == env::KeyCode::Down) m_keyDownStates.Down = true;
//			else if (e.Code == env::KeyCode::Right) m_keyDownStates.Right = true;
//			return false;
//		});
//
//		event.CallIf<env::KeyUpEvent>([&](env::KeyUpEvent& e) {
//			if (e.Code == env::KeyCode::W) m_keyDownStates.W = false;
//			else if (e.Code == env::KeyCode::A) m_keyDownStates.A = false;
//			else if (e.Code == env::KeyCode::S) m_keyDownStates.S = false;
//			else if (e.Code == env::KeyCode::D) m_keyDownStates.D = false;
//
//			else if (e.Code == env::KeyCode::Up) m_keyDownStates.Up = false;
//			else if (e.Code == env::KeyCode::Left) m_keyDownStates.Left = false;
//			else if (e.Code == env::KeyCode::Down) m_keyDownStates.Down = false;
//			else if (e.Code == env::KeyCode::Right) m_keyDownStates.Right = false;
//			return false;
//		});
//
//		event.CallIf<env::MouseMoveEvent>([&](env::MouseMoveEvent& e) {
//			if (e.Modifiers.RightMouse) {
//				m_cameraDelta.Rotation.Horizontal -= e.DeltaX * 0.005f;
//				m_cameraDelta.Rotation.Vertical -= e.DeltaY * 0.005f;
//			}
//
//			if (e.Modifiers.MiddleMouse) {
//				m_cameraDelta.Movement.Right += e.DeltaX;
//				m_cameraDelta.Movement.Up -= e.DeltaY;
//			}
//
//			return false;
//		});
//
//		event.CallIf<env::MouseScrollEvent>([&](env::MouseScrollEvent& e) {
//			m_cameraDelta.Movement.Forward += e.Delta * 50.0f;
//			return false;
//		});
//	}
//
//};
//
//class TestApplication : public env::Application
//{
//	env::Window* m_window;
//
//	ID m_target;
//	ID m_mainCamera;
//
//	// Should this be sent to the renderer? Should a "bundle of lists" be created to hide this? Not desided yet..
//	static const int NUM_FRAME_PACKETS = 2;
//	int m_currentFramePacket = 0;
//	std::array<env::DirectList*, 2> m_presentLists;
//
//public:
//
//	TestApplication(int argc, char** argv) :
//		env::Application(argc, argv, "TestApplication")
//	{	
//		//GetActiveScene()->LoadScene("Helicopter", "assets/SM_helicopter_01.fbx");
//		//m_mesh = env::AssetManager::Get()->LoadMesh("City", "assets/city.fbx");
//
//		GetActiveScene()->LoadScene("City", "assets/Polygon-City Megapolis.fbx");
//		
//		m_window = new env::Window(1200, 800, "Envision", *this);
//
//		m_target = env::ResourceManager::Get()->CreateWindowTarget("TargetWindow", m_window);
//
//		env::Scene* scene = GetActiveScene();
//		
//		m_mainCamera = scene->CreateEntity("Camera");
//
//		env::CameraControllerComponent cameraController;
//		cameraController.TurnSpeedHorizontal = 3.14f;
//		cameraController.TurnSpeedVertical = 3.14f;
//		cameraController.SpeedForward = 5000.0f;
//		cameraController.SpeedUp = 1000.0f;
//		cameraController.SpeedRight = 1000.0f;
//		scene->SetComponent<env::CameraControllerComponent>(m_mainCamera, cameraController);
//
//		env::CameraComponent camera;
//		camera.Settings.FieldOfView = 3.14f / 2.0f;
//		camera.Settings.DistanceNearPlane = 10.0f;
//		camera.Settings.DistanceFarPlane = 100000.0f;
//		camera.Settings.Orthographic = false;
//		scene->SetComponent<env::CameraComponent>(m_mainCamera, camera);
//
//		env::TransformComponent cameraTransform;
//		cameraTransform.Transformation.SetPosition({ 5000.0f, 5000.0f, 5000.0f });
//		cameraTransform.Transformation.RotateAxisY(-2.0f * 3.14f / 3.0f);
//		cameraTransform.Transformation.RotatePitch(2.0f * 3.14f / 12.0f);
//		scene->SetComponent<env::TransformComponent>(m_mainCamera, cameraTransform);
//
//		PushSystem(new SceneUpdateLayer());
//		PushSystem(new RenderLayer());
//		PushWindow(m_window);
//
//		for (int i = 0; i < NUM_FRAME_PACKETS; i++) {
//			m_presentLists[i] = env::GPU::CreateDirectCommandList("TestApplication::PresentList_" + std::to_string(i));
//		}
//
//		ApplicationData appData;
//		appData.MainCameraEntity = m_mainCamera;
//		appData.MainTargetResource = m_target;
//		ID appEntity = scene->CreateEntity("ApplicationData");
//		scene->SetComponent<ApplicationData>(appEntity, appData);
//	}
//
//	void OnUpdate(const env::Duration& delta) override
//	{
//		static float deltaSum = 0.0f;
//		deltaSum += delta.InSeconds();
//
//		static int FPSCap = 120;
//		float targetFrameTime = 1.f / (float)FPSCap;
//
//		// Frame statistics
//		static bool drawWhenReady = false;
//		static int numSampledFrames = 0;
//		static env::Duration durationSampledFrames = 0.0f;
//
//		durationSampledFrames += delta;
//
//		if (deltaSum >= targetFrameTime || drawWhenReady) {
//			deltaSum -= targetFrameTime;
//
//			////ImGui::Begin("Mesh instances");
//			////for (auto& instance : instances) {
//			////	env::Mesh* mesh = env::AssetManager::Get()->GetMesh(instance.first);
//			////	ImGui::Text("%i\t%i\t%s", instance.second, instance.first, mesh->Name.c_str());
//			////}
//			////ImGui::End();
//			////
//
//			//{ // Camera info and settings
//			//	ImGui::Begin("Camera settings");
//
//			//	float fovDegress = (cameraSettings.FieldOfView * 180.0f) / 3.14f;
//			//	ImGui::SliderFloat("FOV", &fovDegress, 1.0f, 179.0f, "%.0f deg", 1.0f);
//			//	cameraSettings.FieldOfView = (fovDegress / 180.0f) * 3.14f;
//
//			//	const Float3& camPos = cameraTransform.GetPosition();
//			//	const Quaternion& camQuat = cameraTransform.GetRotation();
//			//	ImGui::Text("Position\t(%.f, %.f, %.f)", camPos.x, camPos.y, camPos.z);
//			//	ImGui::Text("Rotation\t(%.2f, %.2f, %.2f, %.2f)", camQuat.x, camQuat.y, camQuat.z, camQuat.w);
//
//			//	ImGui::End();
//			//}
//			//
//			//{ // Frame statistics and settings
//			//	static int numFramesToSample = 10;
//			//	static float averageFrameTime = 0.0f;				
//			//	static float averageFPS = 0.0f;
//
//			//	if (++numSampledFrames >= numFramesToSample) {
//			//		averageFPS = (float)numSampledFrames / durationSampledFrames.InSeconds();
//			//		averageFrameTime = durationSampledFrames.InSeconds() / (float)numSampledFrames;
//			//		averageFrameTime *= 1000.f;
//
//			//		durationSampledFrames = 0.0f;
//			//		numSampledFrames = 0;
//			//	}
//
//			//	ImGui::Begin("Frame statistics");
//
//			//	ImGui::Checkbox("Draw when ready?", &drawWhenReady);
//			//	if (drawWhenReady) {
//			//		ImGui::Text("Cap at ");
//			//		ImGui::SameLine();
//			//		ImGui::PushItemWidth(35);
//			//		ImGui::DragInt("FPS", &FPSCap, 1.0f, 1, 240);
//			//	}
//
//			//	ImGui::PushItemWidth(25);
//			//	ImGui::DragInt("sampled frames", &numFramesToSample, 1.0f, 0, 128);
//			//	ImGui::Text("Frametime: %.2f ms (%.0f FPS)", averageFrameTime, averageFPS);
//			//	ImGui::End();
//			//}
//			////
//			//env::RendererGUI::Get()->EndFrame();
//		}
//	}
//
//	~TestApplication() override = default;
//};
//
//env::Application* env::CreateApplication(int argc, char** argv)
//{
//	return new TestApplication(argc, argv);
//}

int main(int argc, char** argv)
{
	env::Application* application = env::CreateApplication(argc, argv);
	application->Run();
	delete application;

	return 0;
}
