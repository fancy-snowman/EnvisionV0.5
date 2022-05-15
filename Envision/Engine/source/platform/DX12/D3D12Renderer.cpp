#include "envision/envpch.h"
#include "envision/platform/DX12/D3D12Renderer.h"
#include "envision/platform/DX12/D3D12ResourceManager.h"

void env::D3D12Renderer::InitPass()
{
	HRESULT hr = S_OK;

	// TODO: Continue here

	//{ // Shaders

	//}

	//{ // Root signature
	//	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	//	ZeroMemory(&rootSignatureDesc, sizeof(rootSignatureDesc));
	//	rootSignatureDesc.NumParameters = 0;
	//	rootSignatureDesc.pParameters = NULL;
	//	rootSignatureDesc.NumStaticSamplers = 0;
	//	rootSignatureDesc.pStaticSamplers = NULL;

	//	ID3DBlob* blob = nullptr;
	//	hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, NULL);
	//	ASSERT_HR(hr, "Could not serialize root signature");

	//	hr = m_device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&m_pass.RootSignature));
	//	ASSERT_HR(hr, "Could not create root signature");
	//}

	//{ // Pipeline state
	//	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
	//	ZeroMemory(&desc, sizeof(desc));
	//	desc.pRootSignature = m_pass.RootSignature;
	//	desc.VS.pShaderBytecode = (void*)pass.VertexShader->GetBufferPointer();
	//	desc.VS.BytecodeLength = pass.VertexShader->GetBufferSize();
	//	desc.PS.pShaderBytecode = (void*)pass.PixelShader->GetBufferPointer();
	//	desc.PS.BytecodeLength = pass.PixelShader->GetBufferSize();
	//	desc.SampleMask = UINT_MAX;
	//	desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//	desc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	//	//desc.DepthStencilState;
	//	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//	desc.NumRenderTargets = 1;
	//	desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	//	//desc.DSVFormat;
	//	desc.SampleDesc.Count = 1;
	//	//desc.Flags;

	//	m_device->CreateGraphicsPipelineState()
	//}
}

env::D3D12Renderer::D3D12Renderer()
{
	IDXGIFactory7* factory = nullptr;
	IDXGIAdapter1* adapter = nullptr;
	ID3D12Debug* debugController = nullptr;

	HRESULT hr = S_OK;

#ifdef _DEBUG 
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		debugController->Release();
	}
	hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
#else
	hr = CreateDXGIFactory(IID_PPV_ARGS(&factory));
#endif

	ASSERT_HR(hr, "Could not create DXGIFactory");


	// Enumerate all available adapters. Find the first one that supports
	// the target feature level and create a device with it.
	for (UINT adapterIndex = 0;; adapterIndex++)
	{
		adapter = nullptr;
		if (factory->EnumAdapters1(adapterIndex, &adapter) == DXGI_ERROR_NOT_FOUND)
		{
			break; // No more adapters to enumerate
		}

		// Check if the adapter supports the target feature level. Don't
		// create the device yet.
		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, __uuidof(m_device), nullptr)))
		{
			break;
		}
		adapter->Release();
		adapter = nullptr;
	}

	if (adapter)
	{
		hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device));
		if (FAILED(hr))
		{
			adapter->Release();
			factory->Release();
		}
		ASSERT_HR(hr, "Could not create device");
		adapter->Release();
	}
	factory->Release();
}

env::D3D12Renderer::~D3D12Renderer()
{
}

void env::D3D12Renderer::Initialize(env::AssetManager*& assetMgrOut, env::ResourceManager*& resourceMgrOut)
{
	m_resourceManager = new D3D12ResourceManager(m_IDGenerator, m_device);
	m_assetManager = new AssetManager(m_IDGenerator, *m_resourceManager);

	assetMgrOut = m_assetManager;
	resourceMgrOut = m_resourceManager;


}

void env::D3D12Renderer::BeginFrame(ID target)
{
}

void env::D3D12Renderer::Submit(ID mesh, ID material)
{
}

void env::D3D12Renderer::EndFrame()
{
}
