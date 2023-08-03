#include "Vertex.h"
#include <Core/src/utl/Assert.h>

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

	VertexLayout::VertexLayout()
	{
		size = 0;
		elCounter.resize(static_cast<int>(VertexElement::Unknown));
	}

	size_t VertexLayout::Size() const
	{
		return Elements.back().first.GetOffsetAfter();
	}

	const VertexLayout::Element& VertexLayout::ResolveByIndex(size_t i) const
	{
		trynass_msg(i < GetElementCount(), L"Layout Indexed out of bounds!");
		return Elements[i].first;
	}

	size_t VertexLayout::GetElementCount() const
	{
		return Elements.size();
	}

	std::string VertexLayout::GetCode() const
	{
		std::stringstream ss;
		for (auto& [element, index] : Elements)
		{
			ss << element.GetCode();
		}
		return ss.str();
	}

	VertexBuffer::VertexBuffer(VertexLayout layout_, size_t size)
	{
		trynass_msg(layout_.GetElementCount() != 0, L"Attempted to create a VertexBuffer with an empty layout");
		this->layout = std::move(layout_);
		Resize(layout.Size() * size);
	}

	VertexBuffer::VertexBuffer(VertexLayout layout, const aiMesh& mesh)
	{
		this->layout = std::move(layout);
		Resize(mesh.mNumVertices);

		for (unsigned int i = 0; i < this->layout.GetElementCount(); i++)
		{
			VertexLayout::Bridge<VertexLayout::Element::AttributeAiMeshFill>(this->layout.ResolveByIndex(i).GetType(), *this, mesh);
		}
	}
	void VertexBuffer::Resize(size_t newSize)
	{
		buffer.resize(newSize * layout.Size());
	}
	size_t VertexBuffer::Size() const
	{
		return buffer.size() / layout.Size();
	}
	Vertex VertexBuffer::operator[](int i)
	{
		trynass_msg(i < Size(), L"VertexBuffer indexed out of bounds");
		return Vertex{ buffer.data() + layout.Size() * i, layout };
	}
	Vertex VertexBuffer::Back()
	{
		trynass_msg(buffer.size() != 0u, L"Back called on an empty VertexBuffer");
		return Vertex{ buffer.data() + buffer.size() - layout.Size(),layout };
	}

	char* VertexBuffer::Data()
	{
		return buffer.data();
	}

	size_t VertexBuffer::BufferSize() const
	{
		return buffer.size();
	}

	size_t VertexBuffer::Stride() const
	{
		return layout.Size();
	}

	const VertexLayout& VertexBuffer::GetLayout() const
	{
		return layout;
	}

	bool& VertexBuffer::GetDirty()
	{
		return dirty;
	}

	Vertex::Vertex(char* pData, const VertexLayout& layout)
		:
		pData(pData),
		layout(layout)
	{
		trynass_msg(pData != nullptr, L"Vertex constructed from a nullptr!");
	}
}