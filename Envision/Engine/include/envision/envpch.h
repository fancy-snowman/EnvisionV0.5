#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <array>
#include <chrono>
#include <assert.h>
#include <algorithm>

typedef long long ID;
#define ID_ERROR 0;

#define PLATFORM_DIRECT3D_12

#ifdef PLATFORM_DIRECT3D_12
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d12")
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3dcompiler")

//#define ASSERT_HR(hr) assert(SUCCEEDED(hr))

#define ASSERT_HR(hr, caption) if (FAILED(hr))\
	MessageBoxA(NULL,\
	("Failure in file " + std::string(__FILE__) + ", line" +\
	std::to_string(__LINE__) + "\n\n" + std::string(caption)).c_str(),\
	"HRESULT failure",\
	MB_OK)

#define ASSERT(condition, caption) if (!condition)\
	MessageBoxA(NULL,\
	("Failure in file " + std::string(__FILE__) + ", line" +\
	std::to_string(__LINE__) + "\n\n" + std::string(caption)).c_str(),\
	"Failure",\
	MB_OK)

#include <DirectXMath.h>
#include <DirectXTK/SimpleMath.h>
using Float2 = DirectX::SimpleMath::Vector2;
using Float3 = DirectX::SimpleMath::Vector3;
using Float4 = DirectX::SimpleMath::Vector4;
using Float4x4 = DirectX::SimpleMath::Matrix;
using Quaternion = DirectX::SimpleMath::Quaternion;

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <entt/entt.hpp>

#endif