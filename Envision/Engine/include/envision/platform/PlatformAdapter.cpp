#include "envision\envpch.h"

#ifdef PLATFORM_DIRECT3D_12
#include "envision\platform\DX12\DX12ResourceManager.h"
env::ResourceManager* env::CreateResourceManager(env::EventBusObject eventBus)
{
	return new env::DX12ResourceManager(eventBus);
}
#endif