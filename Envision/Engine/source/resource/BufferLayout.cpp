#include "envision/resource/BufferLayout.h"

env::BufferElement::BufferElement(std::string name, ShaderDataType type, int index, int bufferSlot, size_t stride) :
	Name(name),
	Type(type),
	Offset(0),
	Index(index),
	BufferSlot(bufferSlot),
	Stride((stride == 0) ? GetShaderDataTypeSize(type) : stride)
{
	//
}

env::BufferLayout::BufferLayout(std::initializer_list<BufferElement> elements) :
	m_byteWidth(0), m_elements(elements)
{
	CalculateOffsetsAndStrides();
}

env::BufferLayout::BufferLayout(const std::vector<BufferElement>& elements) :
	m_byteWidth(0), m_elements(elements)
{
	CalculateOffsetsAndStrides();
}

void env::BufferLayout::SetElements(const std::vector<BufferElement>& elements)
{
	m_byteWidth = 0;
	m_elements = elements;
	CalculateOffsetsAndStrides();
}

void env::BufferLayout::PushElement(const BufferElement&& element)
{
	m_elements.push_back(element);
}

void env::BufferLayout::CalculateOffsetsAndStrides()
{
	m_byteWidth = 0;
	size_t offset = 0;
	for (auto& e : m_elements)
	{
		e.Offset = offset;
		offset += e.Stride;
		m_byteWidth += e.Stride;
	}
}
