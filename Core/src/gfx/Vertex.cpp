#include "TrynPCH.h"
#include "Vertex.h"
#include <Core/src/utl/Assert.h>
#include <Core/src/gfx/Animation/Bone.h>
#undef max
#undef min
#include <GLTFSDK/Document.h>
#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLTFResourceReader.h>
#include <GLTFSDK/MeshPrimitiveUtils.h>
#include "Shape.h"
#include "win/gltfSDK.h"

namespace tryn::gfx
{
	VertexLayout::Element::Element(const VertexElement type, const size_t offset, const uint16_t semanticIndex, const std::string& id)
		:
		id(id),
		semanticIndex(semanticIndex),
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

	uint16_t VertexLayout::Element::Index() const
	{
		return semanticIndex;
	}

	const std::string& VertexLayout::Element::Id() const
	{
		return id;
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
		elCounter.resize(std::to_underlying(VertexElement::Unknown));
	}

	size_t VertexLayout::Size() const
	{
		return Elements.back().GetOffsetAfter();
	}

	const VertexLayout::Element& VertexLayout::Resolve(const VertexElement type, const std::string& id) const
	{
		for (auto& element : Elements)
		{
			if (element.GetType() == type)
			{
				if (id == element.Id())
				{
					return element;
				}
			}
		}
		throw DvtxException(L"Could not resolve an element");
	}

	const VertexLayout::Element& VertexLayout::ResolveByIndex(const size_t i) const
	{
		trynass_msg(i < GetElementCount(), L"Layout Indexed out of bounds!");
		return Elements[i];
	}

	size_t VertexLayout::GetElementCount() const
	{
		return Elements.size();
	}

	void VertexLayout::AppendElement(const VertexElement element, const std::string& id)
	{
		size_t offset;
		if (Elements.empty())
		{
			offset = 0;
		}
		else
		{
			offset = Elements.back().GetOffset() + Elements.back().Size();
		}
		Elements.emplace_back(VertexLayout::Element(element, offset, elCounter[std::to_underlying(element)], id));
		size = Size();
	}

	std::string VertexLayout::GetCode() const
	{
		std::stringstream ss;
		for (const auto& element : Elements)
		{
			ss << element.GetCode();
		}
		return ss.str();
	}

	VertexBuffer::VertexBuffer(VertexLayout layout_, const size_t size)
	{
		trynass_msg(layout_.GetElementCount() != 0 && layout_.Size() != 0, L"Attempted to create a VertexBuffer with an empty layout");
		layout = std::move(layout_);
		VertexBuffer::Resize(layout.Size() * size);
		dirty = false;
	}

	VertexBuffer::VertexBuffer(VertexLayout layout, const aiMesh& mesh, ani::Skeleton* skeleton)
	{
		this->layout = std::move(layout);
		VertexBuffer::Resize(mesh.mNumVertices);

		for (unsigned int i = 0; i < this->layout.GetElementCount(); i++)
		{
			auto element = this->layout.ResolveByIndex(i);
			VertexLayout::Bridge<VertexLayout::Element::AttributeAiMeshFill>(element.GetType(), *this, element.Id(), mesh, skeleton);
		}
		dirty = false;
	}

	struct NothingBehaviour
	{
		template <typename T>
		void operator()(T&& element, VertexBuffer&, int) const
		{
		}
	};

	namespace {
		template <Microsoft::glTF::AccessorType AccessorType, Microsoft::glTF::ComponentType ComponentType, typename ComponentTypeT, VertexLayout::VertexElement ElementType, typename SysTypeOverride = void, typename ExtraBehaviour = NothingBehaviour>
	   void LoadBufferData(const gfx::WinGLTFLoaderContext& context, VertexBuffer& buffer, const Microsoft::glTF::MeshPrimitive& primitive, const std::string& id)
		{
			using SysType = std::conditional_t<std::same_as<SysTypeOverride, void>, typename VertexLayout::VertexElementAttr<ElementType>::SysType, SysTypeOverride>;
			using namespace Microsoft::glTF;
			const auto& doc = *context.pDocument;
			const auto& reader = *context.pReader;

			std::string accessorString = VertexLayout::VertexElementAttr<ElementType>::semantic;

			if constexpr (VertexLayout::VertexElementAttr<ElementType>::semantic == "COLOR" || VertexLayout::VertexElementAttr<ElementType>::semantic == "TEXCOORD")
			{
				accessorString += "_" + std::to_string(buffer.GetLayout().Resolve(ElementType, id).Index());
			}

			trynass(primitive.HasAttribute(accessorString)).msg(utl::ToWide(std::format("The primitive does not have the requested attribute: [{}]", accessorString)));

			const Accessor accessor = doc.accessors[primitive.GetAttributeAccessorId(accessorString)];

			if (accessor.componentType != ComponentType)
			{
				throw GLTFException("Invalid component type for " + accessorString);
			}

			if (accessor.type != AccessorType)
			{
				throw GLTFException("Invalid accessor type for " + accessorString);
			}

			auto data = reader.ReadBinaryData<ComponentTypeT>(doc, accessor);
			static constexpr uint8_t componentPerSysType = sizeof(SysType) / sizeof(ComponentTypeT);
			buffer.Resize(data.size()/componentPerSysType);

			for (unsigned int i = 0; i < data.size()/componentPerSysType; i++)
			{
				const auto& element = *reinterpret_cast<SysType*>(data.data() + i*componentPerSysType);
				buffer[i].Attr<ElementType>(id) = element;
				static constexpr auto eb = ExtraBehaviour();
				eb(element, buffer, i);
			}
		}
	}

	struct ComputeBitangentBehaviour
	{
		template <typename T>
		void operator()(T&& tangent, VertexBuffer& buffer, const int i) const
		{
			const auto normal = buffer[i].Attr<VertexLayout::Normal>();
			const auto bitangent = glm::cross(normal, glm::vec3{ tangent.x, tangent.y, tangent.z }) * tangent.w;
			buffer[i].Attr<VertexLayout::Bitangent>() = bitangent;
		}
	};

	VertexBuffer::VertexBuffer(VertexLayout layout_, const Microsoft::glTF::MeshPrimitive& primitive, const gfx::WinGLTFLoaderContext& context, std::optional<ani::Skeleton> skeleton)
	{
		using namespace Microsoft::glTF;
		this->layout = std::move(layout_);
		auto& doc = *context.pDocument;
		auto& reader = *context.pReader;

		bool computedBitangents = false;
		const auto elCount = layout.GetElementCount();
		for (unsigned int i = 0; i < elCount; i++)
		{
			auto element = layout.ResolveByIndex(i);
			switch (auto type = element.GetType())
			{
			case VertexLayout::Position3D:
				{
					LoadBufferData<AccessorType::TYPE_VEC3, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Position3D>(context, *this, primitive, element.Id());
					break;
				}
			case VertexLayout::Position2D:
				{
					LoadBufferData<AccessorType::TYPE_VEC2, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Position2D>(context, *this, primitive, element.Id());
					break;
				}
			case VertexLayout::Tangent:
				{
					try
					{
						LoadBufferData<AccessorType::TYPE_VEC4, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Tangent, glm::vec4, ComputeBitangentBehaviour>(context, *this, primitive, element.Id());
						computedBitangents = true;
					}
					catch (GLTFException&)
					{
						LoadBufferData<AccessorType::TYPE_VEC3, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Tangent>(context, *this, primitive, element.Id());
					}
					break;
				}
			case VertexLayout::Bitangent:
				{
					trynass(computedBitangents == true);
					break;
				}
			case VertexLayout::Normal:
				{
					LoadBufferData<AccessorType::TYPE_VEC3, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Normal>(context, *this, primitive, element.Id());
					break;
				}
			case VertexLayout::UV:
				{

					LoadBufferData<TYPE_VEC2, COMPONENT_FLOAT, float, VertexLayout::UV>(context, *this, primitive, element.Id());
					break;
				}
			case VertexLayout::Char4Color:
				{
					LoadBufferData<AccessorType::TYPE_UNKNOWN, ComponentType::COMPONENT_UNSIGNED_SHORT, char, VertexLayout::Char4Color>(context, *this, primitive, element.Id());
					break;
				}
			case VertexLayout::Float3Color:
				{
					LoadBufferData<AccessorType::TYPE_VEC3, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Float3Color>(context, *this, primitive, element.Id());
					break;
				}
			case VertexLayout::Float4Color:
			{
				LoadBufferData<AccessorType::TYPE_VEC4, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Float3Color>(context, *this, primitive, element.Id());
				break;
			}
			case VertexLayout::BoneWeights:
				{
					// TODO
					LoadBufferData<AccessorType::TYPE_VEC4, ComponentType::COMPONENT_FLOAT, float, VertexLayout::BoneWeights>(context, *this, primitive, element.Id());
					break;
				}
			case VertexLayout::BoneIds:
			{
				LoadBufferData<AccessorType::TYPE_UNKNOWN, ComponentType::COMPONENT_UNSIGNED_INT, unsigned int, VertexLayout::BoneIds>(context, *this, primitive, element.Id());
				break;
			}
			case VertexLayout::Unknown:
			{
				throw GLTFException("Unknown Vertex Element!");
				break;
			}
			}
		}
		dirty = false;
	}

	VertexBuffer::VertexBuffer(VertexLayout layout, const Shape3D& shape)
	{
		this->layout = std::move(layout);
		Resize(shape.NumVertices());

		for (unsigned int i = 0; i < this->layout.GetElementCount(); i++)
		{
			auto& element = this->layout.ResolveByIndex(i);
			VertexLayout::Bridge<VertexLayout::Element::AttributeShapeMeshFill>(element.GetType(), *this, element.Id(), shape);
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
	void inline VertexLayout::VertexElementAttr<VertexLayout::VertexElement::BoneIds>::ExtractAndFill(VertexBuffer& buf, const std::string& id, const aiMesh& mesh, size_t i, ani::Skeleton const* skeleton) noexcept
	{
		auto& viewBoneIDs = buf[i].Attr<BoneIds>(id);
		auto& viewBoneWeights = buf[i].Attr<BoneWeights>(id);
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
