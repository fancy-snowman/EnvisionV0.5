#include "envision/graphics/RendererGUI.h"
#include "envision/core/GPU.h"
#include "envision/core/Window.h"
#include "envision/resource/ResourceManager.h"

env::RendererGUI* env::RendererGUI::s_instance = nullptr;

env::RendererGUI* env::RendererGUI::Initialize(IDGenerator& commonIDGenerator)
{
	if (!s_instance)
		s_instance = new RendererGUI(commonIDGenerator);
	return s_instance;
}

env::RendererGUI* env::RendererGUI::Get()
{
	return s_instance;
}

void env::RendererGUI::Finalize()
{
	delete s_instance;
	s_instance = nullptr;
}

env::RendererGUI::RendererGUI(env::IDGenerator& commonIDGenerator) :
	m_commonIDGenerator(commonIDGenerator)
{
	{ // Init Dear ImGui
		IMGUI_CHECKVERSION();
		m_imguiContext = ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_
	}

	{ // Init rendering API for ImGui
		m_directList = GPU::CreateDirectCommandList();
		
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 1;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		HRESULT hr = GPU::GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_imguiDescriptorHeap));
		ASSERT_HR(hr, "Could not create descriptor heap for ImGui");
	}
}

env::RendererGUI::~RendererGUI()
{
	delete m_directList;
}

void env::RendererGUI::BeginFrame(ID target)
{
	// TODO: This initialization is temporary
	if (m_target == ID_ERROR) {
		m_target = target;

		WindowTarget* targetResource = ResourceManager::Get()->GetTarget(m_target);
		Window* window = targetResource->AppWindow;
		ImGui_ImplWin32_Init(window->GetHandle());

		Texture2D* backbuffer = window->GetCurrentBackbuffer();
		ImGui_ImplDX12_Init(GPU::GetDevice(),
			1,
			backbuffer->Format,
			m_imguiDescriptorHeap,
			m_imguiDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			m_imguiDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

		window->InitializeGUI();
	}
	m_directList->Reset();

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void env::RendererGUI::EndFrame()
{
	ImGui::EndFrame();
	ImGui::Render();

	WindowTarget* target = ResourceManager::Get()->GetTarget(m_target);
	m_directList->SetTarget(target);
	m_directList->SetDescriptorHeaps(1, &m_imguiDescriptorHeap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_directList->GetNative());
	
	m_directList->Close();
	CommandQueue& queue = GPU::GetPresentQueue();
	queue.QueueList(m_directList);
}
