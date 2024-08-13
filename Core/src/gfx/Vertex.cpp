#include "TrynPCH.h"
#include "Vertex.h"
#include <Core/src/utl/Assert.h>
#include <Core/src/gfx/Animation/Bone.h>

#include "Shape.h"

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

	VertexFormat VertexLayout::Element::GetFormat() const
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
		trynass_msg(layout_.GetElementCount() != 0 && layout_.Size() != 0, L"Attempted to create a VertexBuffer with an empty layout");
		this->layout = std::move(layout_);
		Resize(layout.Size() * size);
		dirty = false;
	}

	VertexBuffer::VertexBuffer(VertexLayout layout, const aiMesh& mesh, ani::Skeleton* skeleton)
	{
		this->layout = std::move(layout);
		Resize(mesh.mNumVertices);

		for (unsigned int i = 0; i < this->layout.GetElementCount(); i++)
		{
			VertexLayout::Bridge<VertexLayout::Element::AttributeAiMeshFill>(this->layout.ResolveByIndex(i).GetType(), *this, mesh, skeleton);
		}
		dirty = false;
	}

	VertexBuffer::VertexBuffer(VertexLayout layout, const Shape3D& shape)
	{
		this->layout = std::move(layout);
		Resize(shape.NumVertices());

		for (unsigned int i = 0; i < this->layout.GetElementCount(); i++)
		{
			VertexLayout::Bridge<VertexLayout::Element::AttributeShapeMeshFill>(this->layout.ResolveByIndex(i).GetType(), *this, shape);
		}
		dirty = false;
	}

	void VertexBuffer::Resize(size_t newSize)
	{
		buffer.resize(newSize * layout.Size());
	}
	constexpr std::size_t VertexBuffer::ByteSize() const noexcept
	{
		return buffer.size();
	}
	constexpr std::size_t VertexBuffer::Size() const noexcept
	{
		return buffer.size();
	}
	Vertex VertexBuffer::operator[](size_t i)
	{
		trynass_msg(i < ByteSize(), L"VertexBuffer indexed out of bounds");
		return Vertex{ buffer.data() + layout.Size() * i, layout };
	}
	Vertex VertexBuffer::Back()
	{
		trynass_msg(buffer.size() != 0u, L"Back called on an empty VertexBuffer");
		return Vertex{ buffer.data() + buffer.size() - layout.Size(),layout };
	}

	constexpr void* VertexBuffer::Data() const noexcept
	{
		return (void*)buffer.data();
	}

	std::size_t VertexBuffer::NumElements() const noexcept
	{
		return buffer.size() / layout.Size();
	}

	std::size_t VertexBuffer::Stride() const noexcept
	{
		return layout.Size();
	}

	const VertexLayout& VertexBuffer::GetLayout() const
	{
		return layout;
	}

	Vertex::Vertex(char* pData, const VertexLayout& layout)
		:
		pData(pData),
		layout(layout)
	{
		trynass_msg(pData != nullptr, L"Vertex constructed from a nullptr!");
	}
	void inline VertexLayout::VertexElementAttr<VertexLayout::VertexElement::BoneIds>::ExtractAndFill(VertexBuffer& buf, const aiMesh& mesh, size_t i, ani::Skeleton const* skeleton) noexcept
	{
		auto& viewBoneIDs = buf[i].Attr<BoneIds>(0);
		auto& viewBoneWeights = buf[i].Attr<BoneWeights>(0);
		for (auto& bone : skeleton->bones)
		{
			for (auto& weight : bone.boneWeights)
			{
				if (weight.vertexID == i)
				{
					for (int j = 0; j < 4; j++)
					{
						if (viewBoneIDs[j] == 0u)
						{
							viewBoneIDs[j] = bone.ID;
							viewBoneWeights[j] = weight.weight;
							break;
						}
					}
				}
			}
		}
	}
}
