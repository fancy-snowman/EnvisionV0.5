#pragma once
#include "envision/envpch.h"

namespace env
{
	// Singleton
	class GPU
	{
	private:

		ID3D12Device* m_device;

		std::string m_name;
		size_t m_maxVideoMemory;

		struct QueueData
		{
			ID3D12CommandQueue* Queue;
			ID3D12Fence* Fence;
			UINT64 FenceValue;
			HANDLE FenceEvent;
		};

		QueueData m_direct;
		QueueData m_compute;
		QueueData m_copy;

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

		static ID3D12Device* GetDevice();

	private:

		void InitDevice();
		void InitQueue(QueueData queue, D3D12_COMMAND_LIST_TYPE type, UINT64 initialFenceValue = 0);
	};
}