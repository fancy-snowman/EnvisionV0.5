#include "systems/GUISystem.h"
#include "envision/graphics/AssetManager.h"

GUISystem::GUISystem(env::Window* mainWindow) : 
	m_mainWindow(mainWindow)
{
	//
}

GUISystem::~GUISystem()
{
	//
}

void GUISystem::OnUpdate(env::Scene& scene, const env::Duration& delta)
{
	env::GUI::Get()->BeginFrame(m_mainWindow->GetCurrentBackbuffer());

	std::vector<env::Material*> materials;
	env::AssetManager::Get()->GetMaterials(materials);

	ImGui::Begin("Assets");
	ImGui::Text("Materials loaded: %i", materials.size());
	
	if (ImGui::TreeNode("Materials"))
	{
		static std::string selectedName = "Select material..";
		static int selectedMaterialID = ID_ERROR;

		if (ImGui::Button(selectedName.c_str()))
			ImGui::OpenPopup("MaterialListPopup");

		if (ImGui::BeginPopup("MaterialListPopup"))
		{
			for (int i = 0; i < materials.size(); ++i)
			{
				env::Material* material = materials[i];
				if (ImGui::Selectable(material->Name.c_str()))
				{
					selectedName = material->Name;
					selectedMaterialID = material->ResourceID;
				}
			}
			ImGui::EndPopup();
		}

		if (selectedMaterialID != ID_ERROR)
		{
			env::Material* material = env::AssetManager::Get()->GetMaterial(selectedMaterialID);

			ImGui::ColorEdit3("Ambient", (float*)&material->AmbientFactor);
			ImGui::ColorEdit3("Diffuse", (float*)&material->DiffuseFactor);
			ImGui::ColorEdit3("Specular", (float*)&material->SpecularFactor);
		}

		ImGui::TreePop();
	}

	ImGui::End();

	env::GUI::Get()->EndFrame();
}
