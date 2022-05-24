#pragma once
#include "envision/envpch.h"
#include "envision/core/CommandQueue.h"

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
		static CommandQueue& GetDirectQueue();
		static CommandQueue& GetComputeQueue();
		static CommandQueue& GetCopyQueue();

	private:

		void InitDevice();
	};
}