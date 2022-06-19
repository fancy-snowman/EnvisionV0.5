#pragma once
#include "envision/envpch.h"
#include "envision/core/Camera.h"
#include "envision/core/Transform.h"

namespace env
{
	struct CameraComponent
	{
		CameraSettings Settings;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) = default;
	};

	struct CameraControllerComponent
	{
		float TurnSpeedHorizontal;
		float TurnSpeedVertical;
		float SpeedForward;
		float SpeedUp;
		float SpeedRight;
	};

	struct DebugInfoComponent
	{
		static const int MAX_NAME_SIZE = 32;
		char Name[MAX_NAME_SIZE] = { '\0' };

		DebugInfoComponent() = default;
		DebugInfoComponent(const std::string& name) {
			memset(Name, '\0', MAX_NAME_SIZE);
			int size = std::min((int)name.size(), MAX_NAME_SIZE);
			memcpy_s(Name, size, name.c_str(), size);
		};
		DebugInfoComponent(const DebugInfoComponent& other) {
			memcpy_s(Name, MAX_NAME_SIZE, other.Name, MAX_NAME_SIZE);
		}
	};

	struct RenderComponent
	{
		ID Mesh = ID_ERROR;
		ID Material = ID_ERROR;

		RenderComponent() = default;
		RenderComponent(const RenderComponent& other) = default;
	};

	struct TransformComponent
	{
		Transform Transformation;

		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
	};
}