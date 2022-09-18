#pragma once
#include "envision/envpch.h"
#include "envision/resource/Resource.h"

namespace env
{
	class GPU;
	class CommandQueue;

	enum class ListState
	{
		Unknown = 0,
		Closed,
		Recording,
		Queued,
	};

	class CommandList
	{
	private:

		ListState m_state;

	private:

		// Queues has the ability to change state when
		// when the list is queued and executed.
		friend class env::CommandQueue;

	protected:

		const D3D12_COMMAND_LIST_TYPE m_type;
		std::string m_name;

		ID3D12GraphicsCommandList* m_list;
		ID3D12CommandAllocator* m_allocator;

		// class GPU is factory for CommandList
		friend class env::GPU;

		CommandList(D3D12_COMMAND_LIST_TYPE type, const std::string& name);
		void Initialize(ID3D12Device* device);

		virtual void ResetInherited() {}

	private:

		CommandList(CommandList&& other) = delete;
		CommandList(const CommandList& other) = delete;
		CommandList& operator=(CommandList&& other) = delete;
		CommandList& operator=(const CommandList& other) = delete;

	public:

		virtual ~CommandList();

		D3D12_COMMAND_LIST_TYPE GetType();

		void Reset();
		void Close();
		ListState GetState();
		const std::string& GetName() const;
		ID3D12GraphicsCommandList* GetNative();

		void TransitionResource(Resource* resource, D3D12_RESOURCE_STATES newState);
		void SetDescriptorHeaps(UINT numHeaps, ID3D12DescriptorHeap* const* heaps);
	};



	// ##################################################################### //
	// ############################# COPY LIST ############################# //
	// ##################################################################### //

	class CopyList : public CommandList
	{
	protected:

		CopyList(D3D12_COMMAND_LIST_TYPE type, const std::string& name);

	private:

		// class GPU is factory for CopyList
		friend class env::GPU;

		CopyList(const std::string& name);

		CopyList(CopyList&& other) = delete;
		CopyList(const CopyList& other) = delete;
		CopyList& operator=(CopyList&& other) = delete;
		CopyList& operator=(const CopyList& other) = delete;

	public:

		~CopyList() override;

		void CopyBufferRegion(Resource* dest, UINT64 destOffset, Resource* src, UINT64 srcOffset, UINT64 numBytes);
		void CopyResource(Resource* dest, Resource* src);

		void UploadBufferData(Buffer* buffer, const void* data, UINT numBytes = 0, UINT destinationOffset = 0);
	};



	// ##################################################################### //
	// ############################ COMPUTE LIST ########################### //
	// ##################################################################### //

	class ComputeList : public CopyList
	{
	protected:

		ComputeList(D3D12_COMMAND_LIST_TYPE type, const std::string& name);

	private:

		// class GPU is factory for ComputeList
		friend class env::GPU;

		ComputeList(const std::string& name);

		ComputeList(ComputeList&& other) = delete;
		ComputeList(const ComputeList& other) = delete;
		ComputeList& operator=(ComputeList&& other) = delete;
		ComputeList& operator=(const ComputeList& other) = delete;

	public:

		~ComputeList() override;

		void Dispatch(UINT numThreadGroupsX, UINT numThreadGroupsY, UINT numThreadGroupsZ);
	};



	// ##################################################################### //
	// ############################ DIRECT LIST ############################ //
	// ##################################################################### //

	class DirectList : public ComputeList
	{
	private:

		// class GPU is factory for DirectList
		friend class env::GPU;

		DirectList(const std::string& name);

		DirectList(DirectList&& other) = delete;
		DirectList(const DirectList& other) = delete;
		DirectList& operator=(DirectList&& other) = delete;
		DirectList& operator=(const DirectList& other) = delete;

		void ResetInherited() final;

		// TODO: Move this to a common GPU state?
		struct {
			ID3D12Resource* IndexBuffer = nullptr;
			ID3D12Resource* VertexBuffers[8] = { nullptr };
		} m_state;

	public:

		~DirectList() final;

		void ClearRenderTarget(Texture2D* target, float red = 0.0f, float green = 0.0f, float blue = 0.0f, float alpha = 0.0f);
		void ClearRenderTarget(Texture2D* target, const Float4& color);
		void ClearDepthStencil(Texture2D* stencil, bool clearDepth, bool clearStencil, FLOAT depthValue, UINT8 stencilValue);

		void Draw(UINT numVertices, UINT vertexOffset);
		void DrawIndexed(UINT numIndices, UINT indexOffset, UINT vertexOffset);
		void DrawInstanced(UINT numVertices, UINT numInstanes, UINT vertexOffset, UINT instanceOffset);
		void DrawIndexedInstanced(UINT numIndices, UINT numInstances, UINT indexOffset, INT vertexOffset, UINT instanceOffset);

		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology);
		void SetPipelineState(PipelineState* state);
		void SetTarget(Texture2D* target, Texture2D* depthStencil = nullptr);
		void SetIndexBuffer(Buffer* buffer);
		void SetVertexBuffer(Buffer* buffer, UINT slot);
	};
}