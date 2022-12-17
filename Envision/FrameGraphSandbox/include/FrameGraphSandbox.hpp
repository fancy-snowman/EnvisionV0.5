#pragma once
#include "envision/Envision.h"

class FrameGraphSandbox : public env::Application
{
private:

	env::Window* m_mainWindow;

	ID m_vertexBuffer;
	ID m_PSO;

public:

	FrameGraphSandbox(int argc, char** argv);
	~FrameGraphSandbox() final;

public:

	void OnUpdate(const env::Duration& delta) final;
};
