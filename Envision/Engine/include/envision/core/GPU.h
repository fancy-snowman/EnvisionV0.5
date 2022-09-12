#pragma once
#include "envision/envpch.h"
#include "envision/core/CommandQueue.h"
#include "envision/core/CommandList.h"

namespace env
{
	// Singleton
	class GPU
	{
	private:

		ID3D12Device* m_device;

		std::string m_name;
		size_t m_maxVideoMemory;

		CommandQueue m_directQueue;
		CommandQueue m_computeQueue;
		CommandQueue m_copyQueue;
		CommandQueue m_presentQueue;

	public:

		static GPU* Initialize();
		static GPU* Get();
		static void Finalize();

	private:

		static GPU* s_instance;

		GPU();
		~GPU();

		GPU(const GPU& other) = delete;
		GPU(const GPU&& other) = delete;
		GPU& operator=(const GPU& other) = delete;
		GPU& operator=(const GPU&& other) = delete;

	public:

		//static CommandList* 

	public:

		static ID3D12Device* GetDevice();
		static CommandQueue& GetDirectQueue();
		static CommandQueue& GetComputeQueue();
		static CommandQueue& GetCopyQueue();
		static CommandQueue& GetPresentQueue();

		static DirectList* CreateDirectCommandList(const std::string& name, bool recordDirectly = false);
		static ComputeList* CreateComputeCommandList(const std::string& name, bool recordDirectly = false);
		static CopyList* CreateCopyCommandList(const std::string& name, bool recordDirectly = false);

	private:

		void InitDevice();
	};
}