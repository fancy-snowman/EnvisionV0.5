#pragma once
#include "envision/envpch.h"
#include "envision/graphics/AssetManager.h"
#include "envision/resource/ResourceManager.h"

namespace env
{
	class Application;

	class Renderer
	{
		void Initialize(env::AssetManager* assetManager, env::ResourceManager* resourceManager);
		friend class Application;

		env::AssetManager* m_assetManager = nullptr;
		env::ResourceManager* m_resourceManager = nullptr;

		ID m_renderTarget = ID();

	public:

		Renderer();
		~Renderer();

	public:

		void BeginFrame(ID target);
		void Submit(ID mesh, ID material);
		void EndFrame();
	};
}