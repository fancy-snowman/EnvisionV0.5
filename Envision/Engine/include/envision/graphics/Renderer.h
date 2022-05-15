#pragma once
#include "envision/envpch.h"
#include "envision/graphics/AssetManager.h"
#include "envision/resource/ResourceManager.h"

namespace env
{
	class Renderer
	{
	protected:

		env::IDGenerator m_IDGenerator;

		// Renderer is responsible for creating and freeing these
		env::AssetManager* m_assetManager = nullptr;
		env::ResourceManager* m_resourceManager = nullptr;

	public:

		Renderer();
		virtual ~Renderer();

	public:

		virtual void Initialize(env::AssetManager*& assetMgrOut, env::ResourceManager*& resourceMgrOut) = 0;

		virtual void BeginFrame(ID target) = 0;
		virtual void Submit(ID mesh, ID material) = 0;
		virtual void EndFrame() = 0;
	};

	// Defined in PlatformAdapter.cpp
	Renderer* CreateRenderer();
}