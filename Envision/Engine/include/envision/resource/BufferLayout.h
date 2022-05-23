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

		size_t m_byteWidth;
		std::vector<BufferElement> m_elements;

	public:

		BufferLayout() : m_byteWidth(0) {};
		~BufferLayout() = default;

		BufferLayout(std::initializer_list<BufferElement> elements);
		BufferLayout(const std::vector<BufferElement>& elements);

	public:

		void SetElements(const std::vector<BufferElement>& elements);
		void PushElement(const BufferElement&& element);

		inline size_t GetByteWidth() const { return m_byteWidth; }
		inline size_t GetElementCount() const { return m_elements.size(); }

		std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); }

	private:

		void CalculateOffsetsAndStrides();
	};
}