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

	template<VertexLayout::VertexElement type>
	struct SysSizeLookup
	{
		static constexpr auto Exec() noexcept
		{
			return sizeof(VertexLayout::VertexElementAttr<type>::SysType);
		}
	};
	constexpr size_t VertexLayout::Element::SizeOf(VertexLayout::VertexElement type)
	{
		return Bridge<SysSizeLookup>(type);
	}
	template<VertexLayout::VertexElement type>
	struct NameLookup
	{
		static constexpr auto Exec() noexcept
		{
			return VertexLayout::VertexElementAttr<type>::semantic;
		}
	};
	template<VertexLayout::VertexElement type>
	struct FormatLookup
	{
		static constexpr auto Exec() noexcept
		{
			return VertexLayout::VertexElementAttr<type>::format;
		}
	};
	constexpr const char * VertexLayout::Element::NameOf(VertexLayout::VertexElement type)
	{
		return Bridge<NameLookup>(type);
	}
	constexpr VertexLayout::Format VertexLayout::Element::FormatOf(VertexElement type)
	{
		return Bridge<FormatLookup>(type);
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

}