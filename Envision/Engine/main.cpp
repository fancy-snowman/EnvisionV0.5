#include "envision/envpch.h"
#include "envision/core/Application.h"
#include "envision/core/Scene.h"
#include "envision/core/Component.h"

#include "envision/resource/ResourceManager.h"
#include "envision/graphics/AssetManager.h"
#include "envision/graphics/Renderer.h"
#include "envision/graphics/GUI.h"

int main(int argc, char** argv)
{
	env::Application* application = env::CreateApplication(argc, argv);
	application->Run();
	delete application;

	return 0;
}
