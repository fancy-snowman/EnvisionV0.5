#include "systems/GUISystem.h"
#include "envision/graphics/AssetManager.h"

#include "envision/core/GPU.h"

GUISystem::GUISystem(env::Window* mainWindow) : 
	m_mainWindow(mainWindow)
{
	env::GUI::Get()->GetDescriptorAllocator().Allocate();
	m_ambientMapDescriptor = env::GUI::Get()->GetDescriptorAllocator().Allocate();
	m_diffuseMapDescriptor = env::GUI::Get()->GetDescriptorAllocator().Allocate();
	m_specularMapDescriptor = env::GUI::Get()->GetDescriptorAllocator().Allocate();
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

			env::Texture2D* ambient = env::ResourceManager::Get()->GetTexture2D(material->AmbientMap);
			env::Texture2D* diffuse = env::ResourceManager::Get()->GetTexture2D(material->DiffuseMap);
			env::Texture2D* specular = env::ResourceManager::Get()->GetTexture2D(material->SpecularMap);

			env::GPU::GetDevice()->CopyDescriptorsSimple(1,
				m_ambientMapDescriptor.CPUHandle,
				ambient->Views.ShaderResource,
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			env::GPU::GetDevice()->CopyDescriptorsSimple(1,
				m_diffuseMapDescriptor.CPUHandle,
				diffuse->Views.ShaderResource,
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			env::GPU::GetDevice()->CopyDescriptorsSimple(1,
				m_specularMapDescriptor.CPUHandle,
				specular->Views.ShaderResource,
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

			ImVec2 imageSize{ 50, 50 };
			ImVec2 imageLargeSize{ 200, 200 };

			ImGui::Image((ImTextureID)m_ambientMapDescriptor.GPUHandle.ptr, imageSize);
			ImGui::SameLine();
			if (ImGui::IsItemHovered())
				ImGui::Image((ImTextureID)m_ambientMapDescriptor.GPUHandle.ptr, imageLargeSize);
			else
				ImGui::ColorEdit3("Ambient", (float*)&material->AmbientFactor, ImGuiColorEditFlags_NoInputs);

			ImGui::Image((ImTextureID)m_diffuseMapDescriptor.GPUHandle.ptr, imageSize);
			ImGui::SameLine();
			if (ImGui::IsItemHovered())
				ImGui::Image((ImTextureID)m_diffuseMapDescriptor.GPUHandle.ptr, imageLargeSize);
			else
				ImGui::ColorEdit3("Diffuse", (float*)&material->DiffuseFactor, ImGuiColorEditFlags_NoInputs);

			ImGui::Image((ImTextureID)m_specularMapDescriptor.GPUHandle.ptr, imageSize);
			ImGui::SameLine();
			if (ImGui::IsItemHovered())
				ImGui::Image((ImTextureID)m_specularMapDescriptor.GPUHandle.ptr, imageLargeSize);
			else
				ImGui::ColorEdit3("Specular", (float*)&material->SpecularFactor, ImGuiColorEditFlags_NoInputs);
			

		}

		ImGui::TreePop();
	}

	ImGui::End();

	env::GUI::Get()->EndFrame();
}
