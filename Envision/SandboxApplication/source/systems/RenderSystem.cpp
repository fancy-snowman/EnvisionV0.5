#include "systems/RenderSystem.h"

RenderSystem::RenderSystem(ID mainCameraEntity, env::Window* mainWindow) :
	m_mainCameraEntity(mainCameraEntity), m_mainWindow(mainWindow)
{
	for (int i = 0; i < NUM_FRAME_PACKETS; i++) {
		m_presentLists[i] =
			env::GPU::CreateDirectCommandList("RenderLayer::PresentList_" + std::to_string(i));
	}
}

RenderSystem::~RenderSystem()
{
	for (int i = 0; i < NUM_FRAME_PACKETS; i++) {
		delete m_presentLists[i];
	}
}

void RenderSystem::OnUpdate(env::Scene& scene, const env::Duration& delta)
{
	static int targetFPS = 120;
	static float targetFrameTime = 1.f / (float)targetFPS;
	static float deltaSum = 0.0f;
	deltaSum += delta.InSeconds();

	if (deltaSum < targetFrameTime)
		return;
	deltaSum -= targetFrameTime;

	env::CameraSettings& cameraSettings =
		scene.GetComponent<env::CameraComponent>(m_mainCameraEntity).Settings;
	env::Transform& cameraTransform =
		scene.GetComponent<env::TransformComponent>(m_mainCameraEntity).Transformation;

	env::DirectList* presentList = m_presentLists[m_currentFramePacket];

	env::Renderer::Get()->BeginFrame(cameraSettings, cameraTransform, m_mainWindow);

	// <MeshID, count>
	std::unordered_map<ID, int> instances;
	scene.ForEach<env::RenderComponent, env::TransformComponent>([&](env::RenderComponent& render, env::TransformComponent& transform) {
		env::Renderer::Get()->Submit(transform.Transformation, render.Mesh, render.Material);

		if (instances.count(render.Mesh) == 0)
			instances[render.Mesh] = 1;
		else
			++instances[render.Mesh];
	});

	env::Texture2D* target = env::ResourceManager::Get()->GetTexture2D(m_mainWindow->GetCurrentBackbuffer());

	env::CommandQueue& presentQueue = env::GPU::GetPresentQueue();
	presentQueue.WaitForFenceValueGPUSide(presentQueue.GetFenceValue()); // TODO: Wait for render queue instead when that's implemented
	presentList->Reset();
	presentList->TransitionResource(target, D3D12_RESOURCE_STATE_RENDER_TARGET);
	presentList->Close();
	presentQueue.QueueList(presentList);
	presentQueue.Execute();
	presentQueue.IncrementFence();

	env::Renderer::Get()->EndFrame();

	// Set target to state PRESENT
	presentQueue.WaitForFenceValueGPUSide(presentQueue.GetFenceValue()); // TODO: Wait for render queue instead when that's implemented
	presentQueue.WaitForIdle();
	presentList->Reset();
	presentList->TransitionResource(target, D3D12_RESOURCE_STATE_PRESENT);
	presentList->Close();
	presentQueue.QueueList(presentList);
	presentQueue.Execute();
	presentQueue.IncrementFence();

	m_currentFramePacket = (m_currentFramePacket + 1) % NUM_FRAME_PACKETS;
}
