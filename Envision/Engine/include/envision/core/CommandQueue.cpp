#include "envision/envpch.h"
#include "CommandQueue.h"

env::CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type) :
    m_type(type),
    m_queue(nullptr),
    m_fence(nullptr),
    m_fenceValue(0),
    m_fenceEvent(NULL)
{
    //
}

void env::CommandQueue::Initialize(ID3D12Device* device)
{
    HRESULT hr = S_OK;

    { // Queue
        D3D12_COMMAND_QUEUE_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.Type = m_type;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_queue));
        ASSERT_HR(hr, "Could not create command queue");
    }

    { // Fence
        m_fenceValue = 0;

        hr = device->CreateFence(m_fenceValue,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&m_fence));
        ASSERT_HR(hr, "Could not create fence for command queue");

        m_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    }
}

env::CommandQueue::~CommandQueue()
{
    m_fence->Release();
    m_queue->Release();
}

UINT64 env::CommandQueue::IncrementFence()
{
    m_fenceValue++;
    m_queue->Signal(m_fence, m_fenceValue);
    return m_fenceValue;
}

bool env::CommandQueue::IsFenceReached(UINT64 value)
{
    if (value <= m_fenceValue)
        return true;

    UINT64 completedValue = m_fence->GetCompletedValue();
    return value <= completedValue;
}

void env::CommandQueue::WaitForIdle()
{
    IncrementFence();
    m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
    WaitForSingleObject(m_fenceEvent, INFINITE);
}

void env::CommandQueue::WaitForFence(ID3D12Fence* fence, UINT64 value)
{
    m_queue->Wait(fence, value);
}

void env::CommandQueue::WaitForFenceValueCPUSide(UINT64 value)
{
    m_fence->SetEventOnCompletion(value, m_fenceEvent);
    WaitForSingleObject(m_fenceEvent, INFINITE);
}

void env::CommandQueue::WaitForFenceValueGPUSide(UINT64 value)
{
    m_queue->Wait(m_fence, value);
}

void env::CommandQueue::WaitForQueue(CommandQueue* queue, UINT64 value)
{
    m_queue->Wait(queue->m_fence, value);
}

UINT64 env::CommandQueue::GetFenceValue() const
{
    return m_fenceValue;
}

UINT64 env::CommandQueue::GetNextFenceValue() const
{
    return m_fenceValue + 1;
}

ID3D12CommandQueue* env::CommandQueue::GetCommandQueue()
{
    return m_queue;
}

void env::CommandQueue::QueueList(CommandList* list)
{
    assert(list->GetType() >= m_type);
    assert(list->GetState() == ListState::Closed);

    m_queuedLists.push_back(list);
    list->m_state = ListState::Queued;
}

void env::CommandQueue::Execute()
{
    std::vector<ID3D12CommandList*> lists;

    // Copy only the native list pointers
    std::for_each(m_queuedLists.begin(),
        m_queuedLists.end(),
        [&lists](CommandList*& list) { lists.push_back(list->m_list); });

    m_queue->ExecuteCommandLists((UINT)lists.size(), lists.data());

    // Return the state of each list to normal, so that the are
    // not "queued" anymore.
    std::for_each(m_queuedLists.begin(),
        m_queuedLists.end(),
        [](CommandList*& list) { list->m_state = ListState::Closed; });
    m_queuedLists.clear();
}
