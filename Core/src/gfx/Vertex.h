#pragma once
#include <Core/third/glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <utility>
#include <optional>
#include <Core/src/utl/Assert.h>
#include <Core/src/utl/Exception.h>
#include <Core/src/log/Log.h>
#include <assimp/scene.h>
#include <Core/src/gfx/CPUBuffer.h>
#include <concepts>
#include <Core/src/gfx/Animation/Bone.h>

#define DVTX_ELEMENT_AI_EXTRACTOR(member) static void ExtractAndFill( VertexBuffer& buf, const aiMesh& mesh,size_t i, ani::Skeleton const* skeleton = nullptr ) noexcept {buf[i].Attr<type>(0) = *reinterpret_cast<const SysType*>(&mesh.member[i]);}
#define DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION static inline void ExtractAndFill( VertexBuffer& buf, const aiMesh& mesh,size_t i, ani::Skeleton const* skeleton = nullptr ) noexcept
#define DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(vertexElement, member) void inline VertexLayout::VertexElementAttr<VertexLayout::VertexElement::vertexElement>::ExtractAndFill(VertexBuffer& buf, const aiMesh& mesh, size_t i, ani::Skeleton const* skeleton) noexcept { buf[i].Attr<VertexLayout::VertexElement::vertexElement>(0) = *reinterpret_cast<const SysType*>(&mesh.member[i]); }
ZT_EX_DEF(DvtxException);

#define LAYOUT_ELEMENT_TYPES \
		X( Position3D ) \
		X( Position2D ) \
		X( Normal ) \
		X( UV ) \
		X( Float3Color ) \
		X( Float4Color ) \
		X( Char4Color ) \
		X( Tangent ) \
		X( Bitangent ) \
		X( BoneIds ) \
		X( BoneWeights ) \
		X( Unknown )

struct BGRAColor
{
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

namespace tryn::gfx
{
	class VertexBuffer;
	class VertexLayout
	{
	public:
		enum VertexElement
		{
#define X(el) el,
			LAYOUT_ELEMENT_TYPES
#undef X
		};
		enum class Format
		{
			Vec2F,
			Vec3F,
			Vec4F,
			Vec4C_UNorm,
			Vec4UI16,
			Float_Uint,
			Unknown
		};

		template <VertexElement Element>
		struct VertexElementAttrBase
		{
			static constexpr VertexElement type = Element;
		};
#define VERTEX_ELEMENT_ATTR(x) template <> struct VertexElementAttr<x> : public VertexElementAttrBase<x>

		template <VertexElement Element>
		struct VertexElementAttr : public VertexElementAttrBase<Element> {
		};

		VERTEX_ELEMENT_ATTR(Position2D)
		{
			using SysType = glm::vec2;
			static constexpr Format format = Format::Vec2F;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P2";
			
			DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION;
		};
		VERTEX_ELEMENT_ATTR(Position3D)
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P3";
			DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION;
		};
		VERTEX_ELEMENT_ATTR(Normal)
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "N";
			DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION;
		};
		VERTEX_ELEMENT_ATTR(UV)
		{
			using SysType = glm::vec2;
			static constexpr Format format = Format::Vec2F;
			static constexpr const char* semantic = "Texcoord";
			static constexpr const char* code = "UV";
			DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION;
		};
		VERTEX_ELEMENT_ATTR(Float3Color)
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "Cf3";
			DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION;
		};
		VERTEX_ELEMENT_ATTR(Float4Color)
		{
			using SysType = glm::vec4;
			static constexpr Format format = Format::Vec4F;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "Cf4";
			DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION;
		};
		VERTEX_ELEMENT_ATTR(Char4Color)
		{
			using SysType = BGRAColor;
			static constexpr Format format = Format::Vec4C_UNorm;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "Cc4";
			DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION;
		};
		VERTEX_ELEMENT_ATTR(Tangent)
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Tangent";
			static constexpr const char* code = "T";
			DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION;
		};
		VERTEX_ELEMENT_ATTR(Bitangent)
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Bitangent";
			static constexpr const char* code = "Bt";
			DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION;
		};
		VERTEX_ELEMENT_ATTR(BoneIds)
		{
			using SysType = glm::vec<4, uint16_t>;
			static constexpr Format format = Format::Vec4UI16;
			static constexpr const char* semantic = "BoneIds";
			static constexpr const char* code = "BId";
			DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION;
		};
		VERTEX_ELEMENT_ATTR(BoneWeights)
		{
			using SysType = glm::vec4;
			static constexpr Format format = Format::Vec4F;
			static constexpr const char* semantic = "BoneWeights";
			static constexpr const char* code = "BWs";
			DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION;
		};
		VERTEX_ELEMENT_ATTR(Unknown)
		{
			using SysType = int;
			static constexpr Format format = Format::Unknown;
			static constexpr const char* semantic = "Unknown";
			static constexpr const char* code = "?";
			DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION;
		};

		template<template<VertexElement> class F, typename... Args>
		static constexpr auto Bridge(VertexElement type, Args&&... args)
		{
			switch (type)
			{
#define X(el) case VertexLayout::el: return F<VertexLayout::el>::Exec( std::forward<Args>( args )... );
				LAYOUT_ELEMENT_TYPES
#undef X
			}
			throw DvtxException(L"Invalid element type");
			return F<VertexLayout::VertexElement::Unknown>::Exec(std::forward<Args>(args)...);
		}

		class Element
		{
			friend class VertexLayout;
			friend class VertexBuffer;
		private:
			template<VertexLayout::VertexElement type>
			struct VertexSysSizeLookup
			{
				static constexpr auto Exec() noexcept
				{
					return sizeof(VertexLayout::VertexElementAttr<type>::SysType);
				}
			};
			template<VertexLayout::VertexElement type>
			struct VertexNameLookup
			{
				static constexpr auto Exec() noexcept
				{
					return VertexLayout::VertexElementAttr<type>::semantic;
				}
			};
			template<VertexLayout::VertexElement type>
			struct VertexFormatLookup
			{
				static constexpr auto Exec() noexcept
				{
					return VertexLayout::VertexElementAttr<type>::format;
				}
			};
			template<VertexLayout::VertexElement type>
			struct VertexCodeLookup
			{
				static constexpr auto Exec() noexcept
				{
					return VertexLayout::VertexElementAttr<type>::code;
				}
			};
			template<VertexLayout::VertexElement type>
			struct AttributeAiMeshFill
			{
				static constexpr void Exec(VertexBuffer& buf, const aiMesh& mesh, ani::Skeleton const* skeleton = nullptr)
				{
					for (auto end = mesh.mNumVertices, i = 0u; i < end; i++)
					{
						VertexLayout::VertexElementAttr<type>::ExtractAndFill(buf, mesh, i, skeleton);
					}
				}
			};
		public:
			Element(VertexElement type, size_t offset);
			size_t GetOffsetAfter() const;
			size_t GetOffset() const;
			size_t Size() const;
			const char* GetCode() const;
			static constexpr size_t SizeOf(VertexElement type)
			{
				return Bridge<VertexSysSizeLookup>(type);
			}
			static constexpr const char* NameOf(VertexElement type)
			{
				return Bridge<VertexNameLookup>(type);
			}
			static constexpr Format FormatOf(VertexElement type)
			{
				return Bridge<VertexFormatLookup>(type);
			}
			static constexpr const char* CodeOf(VertexElement type)
			{
				return Bridge<VertexCodeLookup>(type);
			}
			Format GetFormat() const;
			const char* GetName() const;
			VertexElement GetType() const;
		private:

			VertexElement type;
			size_t offset;
		};

	public:
		VertexLayout();
		template <typename...Args>
		VertexLayout(Args... args)
		{
			int offset = 0;
			elCounter.resize(static_cast<int>(VertexElement::Unknown));

			AppendElement(offset, args...);
			size = Elements.back().first.GetOffsetAfter();
		}
		// Returns size in bytes
		size_t Size() const;
		template <VertexLayout::VertexElement Type>
		const Element& Resolve(int i = 0) const
		{
			int counter = 0;
			for (auto& element : Elements)
			{
				if (element.first.GetType() == Type)
				{
					if (counter == element.second)
					{
						return element.first;
					}
				}
			}
			throw DvtxException(L"Could not resolve an element");
		}
		const Element& ResolveByIndex(size_t i) const;
		// Returns number of elements
		size_t GetElementCount() const;
		template <typename Element>
		void AppendElement(Element element)
		{
			size_t offset;
			if (Elements.empty())
			{
				offset = 0;
			}
			else
			{
				offset = Elements.back().first.GetOffset() + Elements.back().first.Size();
			}
			Elements.emplace_back(VertexLayout::Element(element, offset), elCounter[static_cast<int>(element)]++);
		}
		std::string GetCode() const;
	private:
		template <typename Element>
		void AppendElement(int& offset, Element element)
		{
			Elements.emplace_back(VertexLayout::Element(element, offset), elCounter[static_cast<int>(element)]++);
			offset += static_cast<int>(VertexLayout::Element::SizeOf(element));
		}
		template <typename First, typename ... Args>
		void AppendElement(int& offset, First first, Args ... rest)
		{
			AppendElement(offset, first);
			AppendElement(offset, rest...);
		}
	public:
		std::vector<std::pair<Element, int>> Elements;
	private:
		std::vector<int> elCounter = {};
		size_t size;
	};


	class Vertex
	{
		friend class VertexBuffer;

	protected:
		Vertex(char* pData, const VertexLayout& layout);

		template<VertexLayout::VertexElement type>
		struct AttributeSetting
		{
			template<typename T>
			static constexpr auto Exec(Vertex* pVertex, char* pAttribute, T&& val)
			{
				return pVertex->SetAttribute<type>(pAttribute, std::forward<T>(val));
			}
		};

	public:
		template<VertexLayout::VertexElement Type>
		auto& Attr(int i = 0)
		{
			auto pAttribute = pData + layout.Resolve<Type>().GetOffset();
			return *reinterpret_cast<typename VertexLayout::VertexElementAttr<Type>::SysType*>(pAttribute);
		}
		template<typename T>
		void SetAttributeByIndex(size_t i, T&& val)
		{
			const auto& element = layout.ResolveByIndex(i);
			auto pAttribute = pData + element.GetOffset();
			VertexLayout::Bridge<AttributeSetting>(
				element.GetType(), this, pAttribute, std::forward<T>(val)
			);
		}
		template <typename ... Args>
		void operator()(Args ... args)
		{
			auto count = sizeof...(args);
			trynass_msg(sizeof...(args) == layout.GetElementCount(), L"Invalid number of elements passed to the Vertex assigment operator");
			SetAttributeByIndex(0u, std::forward<Args>(args)...);
		}

	private:
		template<typename First, typename ...Rest>
		void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest) 
		{
			SetAttributeByIndex(i, std::forward<First>(first));
			SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
		}
		template<VertexLayout::VertexElement DestLayoutType , typename SrcType>
		void SetAttribute(char* pAttribute, SrcType&& val)
		{
			using Dest = typename VertexLayout::VertexElementAttr<DestLayoutType>::SysType;
			if constexpr (std::is_assignable<Dest, SrcType>::value)
			{
				*reinterpret_cast<Dest*>(pAttribute) = val;
			}
			else
			{
				throw DvtxException(L"Parameter attribute type mismatch");
			}
		}
	public:
		char* pData = nullptr;
		const VertexLayout& layout;
	};

	class VertexBuffer : public CPUBuffer
	{
	public:
		VertexBuffer(VertexLayout layout_, size_t size = 0);
		VertexBuffer(VertexLayout layout, const aiMesh& mesh, ani::Skeleton* skeleton = nullptr);
		void Resize(size_t newSize) override;
		constexpr std::size_t ByteSize() const noexcept override;
		constexpr std::size_t Size() const noexcept override;
		Vertex operator[](size_t i);
		Vertex Back();
		constexpr void* Data() const noexcept override;
		std::size_t NumElements() const noexcept;
		std::size_t Stride() const noexcept override;
		const VertexLayout& GetLayout() const;

		template<typename ... Args>
		void EmplaceBack(Args&& ... args)
		{
			dirty = true;
			trynass_msg(sizeof...(args) == layout.GetElementCount(), L"Different number of parameters where passed to the EmplaceBack function for a VertexLayout");
			Resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Args>(args)...);
		}

	protected:       
		VertexLayout layout;
		std::vector<char> buffer;
	};

	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Position2D, mVertices);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Position3D, mVertices);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Normal, mNormals);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(UV, mTextureCoords[0]);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Float3Color, mColors[0]);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Float4Color, mColors[0]);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Char4Color, mColors[0]);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Tangent, mTangents);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Bitangent, mBitangents);

	void inline VertexLayout::VertexElementAttr<VertexLayout::VertexElement::BoneIds>::ExtractAndFill(VertexBuffer& buf, const aiMesh& mesh, size_t i, ani::Skeleton const* skeleton) noexcept
	{
		auto& viewBoneIDs = buf[i].Attr<VertexLayout::VertexElement::BoneIds>(0);
		auto& viewBoneWeights = buf[i].Attr<VertexLayout::VertexElement::BoneWeights>(0);
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

	void inline VertexLayout::VertexElementAttr<VertexLayout::VertexElement::BoneWeights>::ExtractAndFill(VertexBuffer& buf, const aiMesh& mesh, size_t i, ani::Skeleton const* skeleton) noexcept
	{

	}
	void inline VertexLayout::VertexElementAttr<VertexLayout::VertexElement::Unknown>::ExtractAndFill(VertexBuffer& buf, const aiMesh& mesh, size_t i, ani::Skeleton const* skeleton) noexcept
	{

	}
}