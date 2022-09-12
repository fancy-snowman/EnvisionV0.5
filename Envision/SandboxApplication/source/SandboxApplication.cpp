#include "SandboxApplication.h"
#include "systems/CameraSystem.h"
#include "systems/RenderSystem.h"
#include <iostream>

env::Application* env::CreateApplication(int argc, char** argv)
{
	return new SandboxApplication(argc, argv);
}

SandboxApplication::SandboxApplication(int argc, char** argv) :
	env::Application(argc, argv, "SandboxApplication")
{
	// TODO: Force window creation through the engine instead.
	m_mainWindow = new env::Window(1200, 800, "Sandbox", *this);
	PushWindow(m_mainWindow);

	// TODO: This seems unecessary... put this into a window asset or something.
	m_mainTargetResource = env::ResourceManager::Get()->CreateWindowTarget("MainTarget", m_mainWindow);

	// TODO: Force the application to CREATE a scene.
	env::Scene* scene = GetActiveScene();
	//scene->LoadScene("Helicopter", "assets/SM_helicopter_01.fbx");
	scene->LoadScene("City", "assets/Polygon-City Megapolis.fbx");

	m_mainCameraEntity = scene->CreateEntity("Camera");

	env::CameraControllerComponent cameraController;
	cameraController.TurnSpeedHorizontal = 3.14f;
	cameraController.TurnSpeedVertical = 3.14f;
	cameraController.SpeedForward = 5000.0f;
	cameraController.SpeedUp = 1000.0f;
	cameraController.SpeedRight = 1000.0f;
	scene->SetComponent<env::CameraControllerComponent>(m_mainCameraEntity, cameraController);

	env::CameraComponent camera;
	camera.Settings.FieldOfView = 3.14f / 2.0f;
	camera.Settings.DistanceNearPlane = 10.0f;
	camera.Settings.DistanceFarPlane = 100000.0f;
	camera.Settings.Orthographic = false;
	scene->SetComponent<env::CameraComponent>(m_mainCameraEntity, camera);
	
	env::TransformComponent cameraTransform;
	//cameraTransform.Transformation.SetPosition({ 0.0f, 0.0f, -100.0f });
	cameraTransform.Transformation.SetPosition({ 5000.0f, 5000.0f, 5000.0f });
	cameraTransform.Transformation.RotateAxisY(-2.0f * 3.14f / 3.0f);
	cameraTransform.Transformation.RotatePitch(2.0f * 3.14f / 12.0f);
	scene->SetComponent<env::TransformComponent>(m_mainCameraEntity, cameraTransform);

	PushSystem(new CameraSystem(m_mainCameraEntity));
	PushSystem(new RenderLayer(m_mainCameraEntity, m_mainTargetResource));
}

SandboxApplication::~SandboxApplication()
{
}

void SandboxApplication::OnUpdate(const env::Duration& delta)
{
	
}
