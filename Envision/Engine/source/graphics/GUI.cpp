#include "envision/graphics/GUI.h"
#include "envision/core/GPU.h"
#include "envision/resource/ResourceManager.h"

env::GUI* env::GUI::s_instance = nullptr;

env::GUI* env::GUI::Initialize(env::Window* window)
{
	if (!s_instance)
		s_instance = new GUI(window);
	return s_instance;
}

env::GUI* env::GUI::Get()
{
	return s_instance;
}

void env::GUI::Finalize()
{
	delete s_instance;
	s_instance = nullptr;
}

env::GUI::GUI(env::Window* window) :
	m_mainWindow(window)
{
	m_context.DirectList = GPU::CreateDirectCommandList("RenderGUI::directList");

	IMGUI_CHECKVERSION();
	m_imguiContext = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 3.0f;
		//style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	m_heap.Initialize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 10, true);

	ImGui_ImplWin32_Init(window->GetHandle());
	ImGui_ImplDX12_Init(GPU::GetDevice(),
		2,
		window->GetBackbufferFormat(),
		m_heap.GetHeap(),
		m_heap.GetHeap()->GetCPUDescriptorHandleForHeapStart(),
		m_heap.GetHeap()->GetGPUDescriptorHandleForHeapStart());
}

env::GUI::~GUI()
{
	delete m_context.DirectList;
}

void env::GUI::BeginFrame(ID target)
{
	m_context.Target = target;

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//// TODO: This initialization is temporary
	//if (m_target == ID_ERROR) {
	//	m_target = target;

	//	WindowTarget* targetResource = ResourceManager::Get()->GetTarget(m_target);
	//	Window* window = targetResource->AppWindow;
	//	ImGui_ImplWin32_Init(window->GetHandle());

	//	Texture2D* backbuffer = window->GetCurrentBackbuffer();
	//	ImGui_ImplDX12_Init(GPU::GetDevice(),
	//		1,
	//		backbuffer->Format,
	//		m_imguiDescriptorHeap,
	//		m_imguiDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
	//		m_imguiDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	//	window->InitializeGUI();
	//}
	//m_directList->Reset();

	//ImGui_ImplDX12_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();
}

void env::GUI::EndFrame()
{
	ImGui::EndFrame();
	ImGui::Render();

	env::CommandQueue& queue = GPU::GetPresentQueue();

	queue.WaitForIdle();

	m_context.DirectList->Reset();

	env::Texture2D* target = env::ResourceManager::Get()->GetTexture2D(m_context.Target);

	ID3D12DescriptorHeap* heap = m_heap.GetHeap();
	m_context.DirectList->SetDescriptorHeaps(1, &heap);
	m_context.DirectList->TransitionResource(target, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_context.DirectList->SetTarget(target);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_context.DirectList->GetNative());
	m_context.DirectList->TransitionResource(target, D3D12_RESOURCE_STATE_PRESENT);
	m_context.DirectList->Close();

	queue.QueueList(m_context.DirectList);
	queue.Execute();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault(NULL, (void*)m_context.DirectList);
	}

	//ImGui::EndFrame();
	//ImGui::Render();

	//WindowTarget* target = ResourceManager::Get()->GetTarget(m_target);
	//m_directList->SetTarget(target);
	//m_directList->SetDescriptorHeaps(1, &m_imguiDescriptorHeap);
	//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_directList->GetNative());
	//
	//m_directList->Close();
	//CommandQueue& queue = GPU::GetPresentQueue();
	//queue.QueueList(m_directList);
}
