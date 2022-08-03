#pragma once
#include "envision/envpch.h"
#include "envision/graphics/CoreShaderDataStructures.h"

namespace env
{
	struct GeometryJob
	{
		ID Mesh = ID_ERROR;
		UINT InstanceOffset = 0;
		UINT NumInstances = 0;
	};

	struct FramePacket
	{
		struct {
			ID Intermediate;
			ID Depth;
			ID Result;
		} Targets;

		struct {
			ID Camera;
			ID Instance;
			ID Material;
		} Buffers;

		struct {
			CameraSettings Settings;
			Transform Transform;
		} Camera;

		std::unordered_map<ID, std::vector<InstanceBufferElementData>> MeshOpaqueInstances;

		std::unordered_map<ID, UINT> MaterialInstanceIndices;
		std::vector<MaterialBufferInstanceData> MaterialInstances;

		std::unordered_map<ID, UINT> TextureInstanceIndices;
		std::vector<ID> TextureInstances;

		std::unordered_map<ID, UINT> SamplerInstanceIndices;
		std::vector<ID> SamplerInstances;

		UINT64 FinishedFenceValue = 0;
	};
}