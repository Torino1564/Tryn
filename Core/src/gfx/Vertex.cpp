#include "TrynPCH.h"
#include "Vertex.h"
#include <Core/src/utl/Assert.h>
#include <Core/src/gfx/Animation/Bone.h>
#include <GLTFSDK/Document.h>
#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLTFResourceReader.h>
#include <GLTFSDK/MeshPrimitiveUtils.h>
#include "Shape.h"
#include "Core/src/utl/StatefulMeta/TemplateData.h"
#include "win/gltfSDK.h"

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
		for (const auto& element : Elements | std::views::keys)
		{
			ss << element.GetCode();
		}
		return ss.str();
	}

	VertexBuffer::VertexBuffer(VertexLayout layout_, const size_t size)
	{
		trynass_msg(layout_.GetElementCount() != 0 && layout_.Size() != 0, L"Attempted to create a VertexBuffer with an empty layout");
		this->layout = std::move(layout_);
		VertexBuffer::Resize(layout.Size() * size);
		dirty = false;
	}

	VertexBuffer::VertexBuffer(VertexLayout layout, const aiMesh& mesh, ani::Skeleton* skeleton)
	{
		this->layout = std::move(layout);
		VertexBuffer::Resize(mesh.mNumVertices);

		for (unsigned int i = 0; i < this->layout.GetElementCount(); i++)
		{
			VertexLayout::Bridge<VertexLayout::Element::AttributeAiMeshFill>(this->layout.ResolveByIndex(i).GetType(), *this, mesh, skeleton);
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
	   void LoadBufferData(const gfx::WinGLTFLoaderContext& context, VertexBuffer& buffer, const Microsoft::glTF::MeshPrimitive& primitive, const std::string& accessorString, const int sameTypeIndex = 0)
		{
			using SysType = std::conditional_t<std::same_as<SysTypeOverride, void>, typename VertexLayout::VertexElementAttr<ElementType>::SysType, SysTypeOverride>;
			using namespace Microsoft::glTF;
			const auto& doc = *context.pDocument;
			const auto& reader = *context.pReader;

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
				buffer[i].Attr<ElementType>(sameTypeIndex) = element;
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

		uint8_t uvCounter = 0;
		bool computedBitangents = false;
		const auto elCount = layout.GetElementCount();
		for (unsigned int i = 0; i < elCount; i++)
		{
			switch (auto type = layout.ResolveByIndex(i).GetType())
			{
			case VertexLayout::Position3D:
				{
					LoadBufferData<AccessorType::TYPE_VEC3, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Position3D>(context, *this, primitive, ACCESSOR_POSITION);
					break;
				}
			case VertexLayout::Position2D:
				{
					LoadBufferData<AccessorType::TYPE_VEC2, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Position2D>(context, *this, primitive, ACCESSOR_POSITION);
					break;
				}
			case VertexLayout::Tangent:
				{
					try
					{
						LoadBufferData<AccessorType::TYPE_VEC4, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Tangent, glm::vec4, ComputeBitangentBehaviour>(context, *this, primitive, ACCESSOR_TANGENT);
						computedBitangents = true;
					}
					catch (GLTFException&)
					{
						LoadBufferData<AccessorType::TYPE_VEC3, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Tangent>(context, *this, primitive, ACCESSOR_TANGENT);
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
					LoadBufferData<AccessorType::TYPE_VEC3, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Normal>(context, *this, primitive, ACCESSOR_NORMAL);
					break;
				}
			case VertexLayout::UV:
				{
					std::string accessorString;
					if (uvCounter == 0)
					{
						accessorString = ACCESSOR_TEXCOORD_0;
					}
					else
					{
						accessorString = ACCESSOR_TEXCOORD_1;
					}
					LoadBufferData<AccessorType::TYPE_VEC2, ComponentType::COMPONENT_FLOAT, float, VertexLayout::UV>(context, *this, primitive, accessorString, uvCounter++);
					break;
				}
			case VertexLayout::Char4Color:
				{
					LoadBufferData<AccessorType::TYPE_UNKNOWN, ComponentType::COMPONENT_UNSIGNED_SHORT, char, VertexLayout::Char4Color>(context, *this, primitive, ACCESSOR_COLOR_0);
					break;
				}
			case VertexLayout::Float3Color:
				{
					LoadBufferData<AccessorType::TYPE_VEC3, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Float3Color>(context, *this, primitive, ACCESSOR_COLOR_0);
					break;
				}
			case VertexLayout::Float4Color:
			{
				LoadBufferData<AccessorType::TYPE_VEC4, ComponentType::COMPONENT_FLOAT, float, VertexLayout::Float3Color>(context, *this, primitive, ACCESSOR_COLOR_0);
				break;
			}
			case VertexLayout::BoneWeights:
				{
					// TODO
					LoadBufferData<AccessorType::TYPE_VEC4, ComponentType::COMPONENT_FLOAT, float, VertexLayout::BoneWeights>(context, *this, primitive, ACCESSOR_WEIGHTS_0);
					break;
				}
			case VertexLayout::BoneIds:
			{
				LoadBufferData<AccessorType::TYPE_UNKNOWN, ComponentType::COMPONENT_UNSIGNED_INT, unsigned int, VertexLayout::BoneIds>(context, *this, primitive, ACCESSOR_JOINTS_0);
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
