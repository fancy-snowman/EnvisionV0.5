#pragma once
#include "envision/envpch.h"
#include "envision/resource/ShaderDataType.h"

namespace env
{
	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		size_t Offset;
		size_t Stride;
		int Index;
		int BufferSlot; // I.e. multiple vertex buffers

		BufferElement(std::string name, ShaderDataType type, int index = 0, int bufferSlot = 0, size_t stride = 0);
	};

	class BufferLayout
	{
	private:

		UINT m_numRepetitions;
		UINT m_repetitionStride;
		std::vector<BufferElement> m_elements;

	public:

		BufferLayout() : m_numRepetitions(0), m_repetitionStride(0) {};
		~BufferLayout() = default;

		BufferLayout(std::initializer_list<BufferElement> elements, UINT repetitions = 1);
		BufferLayout(const std::vector<BufferElement>& elements, UINT repetitions = 1);

	public:

		void SetElements(const std::vector<BufferElement>& elements, UINT repetitions = 1);
		void PushElement(const BufferElement&& element);
		void SetRepetitions(UINT repetitions);

		inline UINT GetByteWidth() const { return m_repetitionStride; }
		inline UINT GetNumElements() const { return (UINT)m_elements.size(); }
		inline UINT GetNumRepetitions() const { return m_numRepetitions; }

		DXGI_FORMAT GetDXGIFormat() const;

		std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }

	private:

		void CalculateOffsetsAndStrides();
	};
}