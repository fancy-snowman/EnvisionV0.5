#include "envision\resource\ResourceManager.h"

env::ResourceManager::ResourceManager(EventBusObject eventBus) : m_eventBus(eventBus)
{
	//
}

env::Buffer* env::ResourceManager::GetBuffer(ID resourceID)
{
	if (m_buffers.count(resourceID) == 0)
	{
		return nullptr;
	}
	return m_buffers.at(resourceID);
}

env::BufferArray* env::ResourceManager::GetBufferArray(ID resourceID)
{
	if (m_bufferArrays.count(resourceID) == 0)
	{
		return nullptr;
	}
	return m_bufferArrays.at(resourceID);
}

env::Shader* env::ResourceManager::GetShader(ID resourceID)
{
	if (m_shaders.count(resourceID) == 0)
	{
		return nullptr;
	}
	return m_shaders.at(resourceID);
}

env::Texture2D* env::ResourceManager::GetTexture2D(ID resourceID)
{
	if (m_texture2Ds.count(resourceID) == 0)
	{
		return nullptr;
	}
	return m_texture2Ds.at(resourceID);
}

env::Texture2DArray* env::ResourceManager::GetTexture2DArray(ID resourceID)
{
	if (m_texture2DArrays.count(resourceID) == 0)
	{
		return nullptr;
	}
	return m_texture2DArrays.at(resourceID);
}

env::Window* env::ResourceManager::GetWindow(ID resourceID)
{
	return nullptr;
}
