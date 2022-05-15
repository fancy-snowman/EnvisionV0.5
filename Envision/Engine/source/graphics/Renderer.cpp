#include "envision/envpch.h"
#include "envision/graphics/Renderer.h"
#include "envision/platform/Windows/WindowsWindow.h"

void env::Renderer::Initialize(env::AssetManager* assetManager, env::ResourceManager* resourceManager)
{
	m_assetManager = assetManager;
	m_resourceManager = resourceManager;
}

env::Renderer::Renderer()
{
	//
}

env::Renderer::~Renderer()
{
}

void env::Renderer::BeginFrame(ID target)
{
}

void env::Renderer::Submit(ID mesh, ID material)
{
}

void env::Renderer::EndFrame()
{
}
