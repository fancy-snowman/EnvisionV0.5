#include "envision/envpch.h"
#include "envision/graphics/Renderer.h"
#include "envision/resource/ResourceManager.h"

env::Renderer* env::Renderer::s_instance = nullptr;

env::Renderer* env::Renderer::Initialize(IDGenerator& commonIDGenerator)
{
	if (!s_instance)
		s_instance = new Renderer(commonIDGenerator);
	return s_instance;
}

env::Renderer* env::Renderer::Get()
{
	assert(s_instance);
	return s_instance;
}

void env::Renderer::Finalize()
{
	delete s_instance;
	s_instance = nullptr;
}

env::Renderer::Renderer(env::IDGenerator& commonIDGenerator) :
	m_commonIDGenerator(commonIDGenerator)
{
	m_intermediateTarget = ResourceManager::Get()->CreateTexture2D("IntermediateTarget",
		1200,
		800,
		TextureLayout::RGBA_FLOAT,
		BindType::RenderTarget);

	m_phongBuffer = ResourceManager::Get()->CreateConstantBuffer("PhongBuffer", {
			{ "DiffuseFactor", ShaderDataType::Float3 },
			{ "Padding1", ShaderDataType::Float },
			{ "AmbientFactor", ShaderDataType::Float3 },
			{ "Padding2", ShaderDataType::Float },
			{ "SpecularFactor", ShaderDataType::Float3 },
			{ "SpecularExponent", ShaderDataType::Float } });
}

env::Renderer::~Renderer()
{
	//
}

void env::Renderer::Initialize()
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
