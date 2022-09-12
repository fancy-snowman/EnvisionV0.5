#include "systems/CameraSystem.h"

CameraSystem::CameraSystem(ID cameraEntity) :
	m_camera(cameraEntity)
{
	//
}

CameraSystem::~CameraSystem()
{
	//
}

void CameraSystem::OnEvent(env::Scene& scene, env::Event& event)
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

void CameraSystem::OnUpdate(env::Scene& scene, const env::Duration& delta)
{
	if (!scene.IsEntity(m_camera))
		return;
	if (!scene.HasComponent<env::CameraControllerComponent>(m_camera))
		return;
	if (!scene.HasComponent<env::TransformComponent>(m_camera))
		return;

	env::CameraControllerComponent& controller = scene.GetComponent<env::CameraControllerComponent>(m_camera);
	env::TransformComponent& transform = scene.GetComponent<env::TransformComponent>(m_camera);

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

	m_cameraDelta.Rotation.Horizontal = 0.0f;
	m_cameraDelta.Rotation.Vertical = 0.0f;
	m_cameraDelta.Movement.Forward = 0.0f;
	m_cameraDelta.Movement.Up = 0.0f;
	m_cameraDelta.Movement.Right = 0.0f;
}
