#pragma once
#include "envision/envpch.h"

namespace env
{
	class GPU;

	class CommandQueue
	{
	private:

		const D3D12_COMMAND_LIST_TYPE m_type;

		ID3D12CommandQueue* m_queue;

		ID3D12Fence* m_fence;
		UINT64 m_fenceValue;
		HANDLE m_fenceEvent;

	private:

		// class GPU is factory for CommandQueue
		friend class env::GPU;

		CommandQueue(D3D12_COMMAND_LIST_TYPE type);

		void Initialize(ID3D12Device* device);

		CommandQueue(CommandQueue&& other) = delete;
		CommandQueue(const CommandQueue& other) = delete;
		CommandQueue& operator=(CommandQueue&& other) = delete;
		CommandQueue& operator=(const CommandQueue& other) = delete;

	public:

		~CommandQueue();

		UINT64 IncrementFence();
		bool IsFenceReached(UINT64 value);

		void WaitForIdle();
		void WaitForFence(ID3D12Fence* fence, UINT64 value);
		void WaitForQueue(CommandQueue* queue, UINT64 value);

		UINT64 GetFenceValue() const;
		UINT64 GetNextFenceValue() const;
		ID3D12CommandQueue* GetCommandQueue();
	};
}