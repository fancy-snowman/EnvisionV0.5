#pragma once

#include "envision/Envision.h"

class CameraSystem : public env::System
{
private:

	ID m_camera;

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

	CameraSystem(ID cameraEntity);
	~CameraSystem();

public:

	virtual void OnEvent(env::Scene& scene, env::Event& event) final;
	virtual void OnUpdate(env::Scene& scene, const env::Duration& delta) final;
};