#include "envision/envpch.h"
#include "envision/core/GPU.h"

env::GPU* env::GPU::s_instance = nullptr;

env::GPU* env::GPU::Initialize()
{
	if (!s_instance)
		s_instance = new GPU();
	return s_instance;
}

env::GPU* env::GPU::Get()
{
	assert(s_instance);
	return s_instance;
}

void env::GPU::Finalize()
{
	delete s_instance;
	s_instance = nullptr;
}

env::GPU::GPU() : m_device(nullptr), m_name("Not initialized"), m_maxVideoMemory(0)
{
	InitDevice();
	InitQueue(m_direct, D3D12_COMMAND_LIST_TYPE_DIRECT);
	InitQueue(m_compute, D3D12_COMMAND_LIST_TYPE_COMPUTE);
	InitQueue(m_copy, D3D12_COMMAND_LIST_TYPE_COPY);
}

env::GPU::~GPU()
{
}

ID3D12Device* env::GPU::GetDevice()
{
	return Get()->m_device;
}

void env::GPU::InitDevice()
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

		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		std::wstring wname = desc.Description;

		m_name = std::string(wname.begin(), wname.end());
		m_maxVideoMemory = desc.DedicatedVideoMemory;

		adapter->Release();
	}
	factory->Release();
}

void env::GPU::InitQueue(QueueData queue, D3D12_COMMAND_LIST_TYPE type, UINT64 initialFenceValue)
{
	HRESULT hr = S_OK;

	{ // Queue
		D3D12_COMMAND_QUEUE_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Type = type;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		hr = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue.Queue));
		ASSERT_HR(hr, "Could not create command queue");
	}

	{ // Fence
		queue.FenceValue = initialFenceValue;

		hr = m_device->CreateFence(queue.FenceValue,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&queue.Fence));
		ASSERT_HR(hr, "Could not create fence for command queue");

		queue.FenceEvent = CreateEventA(0, FALSE, FALSE, NULL);
	}
}
