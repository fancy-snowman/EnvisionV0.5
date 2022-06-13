#pragma once
#include "envision/envpch.h"
#include "envision/core/Transform.h"

namespace env
{
	struct CameraSettings
	{
		float FieldOfView;
		float DistanceNearPlane;
		float DistanceFarPlane;
		bool Orthographic;
	};
}
