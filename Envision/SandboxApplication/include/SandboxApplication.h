#pragma once
#include "envision/Envision.h"

#ifdef _DEBUG
#pragma comment(lib, "Engine_d")
#else
#pragma comment(lib, "Engine")
#endif

class SandboxApplication : public env::Application
{
private:

	env::Window* m_mainWindow;

	ID m_mainCameraEntity;
	ID m_mainTargetResource;

public:

	SandboxApplication(int argc, char** argv);
	~SandboxApplication() final;

public:

	void OnUpdate(const env::Duration& delta) final;
};