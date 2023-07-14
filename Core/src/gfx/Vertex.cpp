#include "Vertex.h"

namespace tryn::gfx
{
	VertexLayout::Element::Element(VertexElement type, size_t offset)
		:
		type(type),
		offset(offset)
	{}

	size_t VertexLayout::Element::GetOffsetAfter() const
	{
		return offset + Size();
	}

	size_t VertexLayout::Element::GetOffset() const
	{
		return offset;
	}

	size_t VertexLayout::Element::Size() const
	{
		return SizeOf(type);
	}

	const char* VertexLayout::Element::GetCode() const
	{
		return CodeOf(type);
	}

	VertexLayout::Format VertexLayout::Element::GetFormat() const
	{
		return FormatOf(type);
	}
	const char* VertexLayout::Element::GetName() const
	{
		return NameOf(type);
	}
	VertexLayout::VertexElement VertexLayout::Element::GetType() const
	{
		return type;
	}

	std::string VertexLayout::GetCode() const
	{
		std::stringstream ss;
		for (auto& [element,index]:Elements)
		{
			ss << element.GetCode();
		}
		return ss.str();
	}

}