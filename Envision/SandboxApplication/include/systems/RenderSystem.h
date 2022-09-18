#pragma once
#include "envision/Envision.h"

class RenderLayer : public env::System
{
private:

	ID m_mainCameraEntity;
	env::Window* m_mainWindow;

	// Should this be sent to the renderer? Should a "bundle of lists" be created to hide this? Not desided yet..
	static const int NUM_FRAME_PACKETS = 2;
	int m_currentFramePacket = 0;
	std::array<env::DirectList*, 2> m_presentLists;

public:

	RenderLayer(ID mainCameraEntity, env::Window* mainWindow);
	~RenderLayer() final;

public:

	void OnUpdate(env::Scene& scene, const env::Duration& delta) final;
};