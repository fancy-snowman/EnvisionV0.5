#pragma once
#include "envision/core/System.h"
#include "envision/graphics/GUI.h"

class GUISystem : public env::System
{
private:

	env::Window* m_mainWindow;
	ID m_currentTarget;

	env::DescriptorAllocation m_ambientMapDescriptor;
	env::DescriptorAllocation m_diffuseMapDescriptor;
	env::DescriptorAllocation m_specularMapDescriptor;

public:

	GUISystem(env::Window* mainWindow);
	~GUISystem() final;

public:

	void OnUpdate(env::Scene& scene, const env::Duration& delta) final;
};