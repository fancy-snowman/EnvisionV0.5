#pragma once
#include "envision/envpch.h"
#include "envision/graphics/AssetManager.h"
#include "envision/resource/ResourceManager.h"

namespace env
{
	class Renderer
	{
	protected:

		env::IDGenerator& m_commonIDGenerator;

	public:

		Renderer(env::IDGenerator& commonIDGenerator);
		virtual ~Renderer();

	public:

		virtual void Initialize() = 0;

		virtual void BeginFrame(ID target) = 0;
		virtual void Submit(ID mesh, ID material) = 0;
		virtual void EndFrame() = 0;
	};

	// Defined in PlatformAdapter.cpp
	Renderer* CreateRenderer();
}