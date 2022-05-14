#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <queue>
#include <chrono>


#define PLATFORM_DIRECT3D_12

#ifdef PLATFORM_DIRECT3D_12
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dcompiler")
typedef ID3D12Device IDX12Device;
typedef ID3D12CommandAllocator IDX12CommandAllocator;
typedef ID3D12CommandList IDX12CommandList;
typedef ID3D12CommandQueue IDX12CommandQueue;
typedef ID3D12Fence IDX12Fence;
typedef ID3D12GraphicsCommandList IDX12GraphicsCommandList;
typedef ID3D12Heap IDX12Heap;
typedef ID3D12PipelineState IDX12PipelineState;
typedef ID3D12Resource IDX12Resource;
typedef ID3D12RootSignature IDX12RootSignature;
typedef IDXGIAdapter IDXAdapter;
typedef IDXGIDevice IDXDevice;
typedef IDXGIFactory IDXFactory;
typedef IDXGIResource IDXResource;
typedef IDXGISwapChain IDXSwapChain;
#endif

typedef long long ID;
#define ID_ERROR 0;

#include <assert.h>
#define ASSERT_HR(hr) assert(SUCCEEDED(hr))
