#pragma once
#include <glm/glm.hpp>
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
#include <Core/src/gfx/VertexFormat.h>
#include <Core/src/gfx/Shape.h>
#include <string_view>
#include "Animation/BonedMesh.h"

namespace Microsoft::glTF
{
	struct MeshPrimitive;
	class Document;
}

namespace tryn::gfx::ani
{
	class Skeleton;
}

#define DVTX_ELEMENT_EXTRACTOR DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION;\
		DVTX_ELEMENT_SHAPE_EXTRACTOR_DECLARATION

#define DVTX_ELEMENT_AI_EXTRACTOR_DECLARATION static inline void ExtractAndFill( VertexBuffer& buf, const std::string& id, const aiMesh& mesh,size_t i, ani::Skeleton const* skeleton = nullptr ) noexcept

#define DVTX_ELEMENT_SHAPE_EXTRACTOR_DECLARATION static inline void ExtractAndFill( VertexBuffer& buf, const std::string& id, const gfx::Shape3D&, size_t i) noexcept

#define DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(vertexElement, member) void inline VertexLayout::VertexElementAttr<VertexLayout::VertexElement::vertexElement>::ExtractAndFill(VertexBuffer& buf, const std::string& id, const aiMesh& mesh, size_t i, ani::Skeleton const* skeleton) noexcept { buf[i].Attr<VertexLayout::VertexElement::vertexElement>(id) = *reinterpret_cast<const SysType*>(&mesh.member[i]); }

#define DVTX_ELEMENT_SHAPE_EXTRACTOR_DEFINITION(vertexElement, memberFunc) void inline VertexLayout::VertexElementAttr<VertexLayout::VertexElement::vertexElement>::ExtractAndFill(VertexBuffer& buf, const std::string& id, const Shape3D& shape, size_t i) noexcept { buf[i].Attr<VertexLayout::VertexElement::vertexElement>(id) = *reinterpret_cast<const SysType*>(&shape.memberFunc()[i]); }

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
	class WinGLTFLoaderContext;
	class VertexBuffer;

	class VertexLayout
	{
	public:
		enum VertexElement : uint8_t
		{
#define X(el) el,
			LAYOUT_ELEMENT_TYPES
#undef X
		};

		template <VertexElement Element>
		struct VertexElementAttrBase
		{
			static constexpr VertexElement type = Element;
			static constexpr std::string_view semantic = "UNKNOWN";
			using SysType = void;
			static constexpr VertexFormat format = VertexFormat::Unknown;
			static constexpr const char* code = "";

			DVTX_ELEMENT_EXTRACTOR;
		};
#define VERTEX_ELEMENT_ATTR(x) template <> struct VertexLayout::VertexElementAttr<VertexLayout::VertexElement::x> : public VertexLayout::VertexElementAttrBase<VertexLayout::VertexElement::x>

		template <VertexLayout::VertexElement Element>
		struct VertexElementAttr : public VertexElementAttrBase<Element> {
		};

		template<template<VertexElement> class F, typename... Args>
		static constexpr auto Bridge(VertexLayout::VertexElement type, Args&&... args) -> decltype(F<VertexLayout::Unknown>::Exec(std::forward<Args>(args)...));

		class Element
		{
			friend class VertexLayout;
			friend class VertexBuffer;
		private:
			template<VertexLayout::VertexElement type>
			struct VertexSysSizeLookup
			{
				static constexpr auto Exec() noexcept;
			};
			template<VertexLayout::VertexElement type>
			struct VertexNameLookup
			{
				static constexpr auto Exec() noexcept;
			};
			template<VertexLayout::VertexElement type>
			struct VertexFormatLookup
			{
				static constexpr auto Exec() noexcept;
			};
			template<VertexLayout::VertexElement type>
			struct VertexCodeLookup
			{
				static constexpr auto Exec() noexcept;
			};
			template<VertexLayout::VertexElement type>
			struct AttributeAiMeshFill
			{
				static constexpr void Exec(VertexBuffer& buf, const std::string& id, const aiMesh& mesh, ani::Skeleton const* skeleton = nullptr);
			};
			template<VertexLayout::VertexElement type>
			struct AttributeShapeMeshFill
			{
				static constexpr void Exec(VertexBuffer& buf, const std::string& id, const gfx::Shape3D& shape);
			};
		public:
			Element(VertexElement type, size_t offset, uint16_t semanticIndex = 0, const std::string& id = "");
			size_t GetOffsetAfter() const;
			size_t GetOffset() const;
			size_t Size() const;
			uint16_t Index() const;
			const std::string& Id() const;
			std::string_view GetCode() const;
			static constexpr size_t SizeOf(VertexElement type);
			static constexpr std::string_view NameOf(VertexElement type);
			static constexpr VertexFormat FormatOf(VertexElement type);
			static constexpr std::string_view CodeOf(VertexElement type);
			VertexFormat GetFormat() const;
			std::string_view GetName() const;
			VertexElement GetType() const;
		private:
			std::string id = "";
			uint16_t semanticIndex = 0;
			VertexElement type;
			size_t offset;
		};

	public:
		VertexLayout();
		// Returns size in bytes
		size_t Size() const;
		const Element& Resolve(VertexElement type, const std::string& id = "") const;
		const Element& ResolveByIndex(size_t i) const;
		// Returns number of elements
		size_t GetElementCount() const;

		void AppendElement(VertexElement element, const std::string& id = "");
		std::string GetCode() const;
		std::vector<Element> Elements;
		std::vector<uint16_t> elCounter;
		size_t size;
	};

	template<VertexLayout::VertexElement Type>
	constexpr auto VertexLayout::Element::VertexSysSizeLookup<Type>::Exec() noexcept
	{
		return sizeof(
			typename VertexLayout::template VertexElementAttr<Type>::SysType
			);
	}

	template<VertexLayout::VertexElement Type>
	constexpr auto VertexLayout::Element::VertexNameLookup<Type>::Exec() noexcept
	{
		return VertexLayout:: template VertexElementAttr<Type>::semantic;
	}

	template<VertexLayout::VertexElement Type>
	constexpr auto VertexLayout::Element::VertexFormatLookup<Type>::Exec() noexcept
	{
		return VertexLayout:: template VertexElementAttr<Type>::format;
	}

	template<VertexLayout::VertexElement Type>
	constexpr auto VertexLayout::Element::VertexCodeLookup<Type>::Exec() noexcept
	{
		return VertexLayout:: template VertexElementAttr<Type>::code;
	}

	template<VertexLayout::VertexElement Type>
	constexpr void VertexLayout::Element::AttributeAiMeshFill<Type>::Exec(VertexBuffer& buf, const std::string& id, const aiMesh& mesh, ani::Skeleton const* skeleton)
	{
		for (auto end = mesh.mNumVertices, i = 0u; i < end; i++)
		{
			VertexLayout:: template VertexElementAttr<Type>::ExtractAndFill(buf, id, mesh, i, skeleton);
		}
	}

	template<VertexLayout::VertexElement Type>
	constexpr void VertexLayout::Element::AttributeShapeMeshFill<Type>::Exec(VertexBuffer& buf, const std::string& id, const Shape3D& shape)
	{
		for (auto i = 0u; i < shape.NumVertices(); i++)
		{
			VertexLayout:: template VertexElementAttr<Type>::ExtractAndFill(buf, id, shape, i);
		}
	}

	VERTEX_ELEMENT_ATTR(Position2D)
	{
		using SysType = glm::vec2;
		static constexpr VertexFormat format = VertexFormat::Vec2F;
		static constexpr std::string_view semantic = "POSITION";
		static constexpr const char* code = "P2";

		DVTX_ELEMENT_EXTRACTOR;
	};
	VERTEX_ELEMENT_ATTR(Position3D)
	{
		using SysType = glm::vec3;
		static constexpr VertexFormat format = VertexFormat::Vec3F;
		static constexpr std::string_view semantic = "POSITION";
		static constexpr const char* code = "P3";
		DVTX_ELEMENT_EXTRACTOR;
	};
	VERTEX_ELEMENT_ATTR(Normal)
	{
		using SysType = glm::vec3;
		static constexpr VertexFormat format = VertexFormat::Vec3F;
		static constexpr std::string_view semantic = "NORMAL";
		static constexpr const char* code = "N";
		DVTX_ELEMENT_EXTRACTOR;
	};
	VERTEX_ELEMENT_ATTR(UV)
	{
		using SysType = glm::vec2;
		static constexpr VertexFormat format = VertexFormat::Vec2F;
		static constexpr std::string_view semantic = "TEXCOORD";
		static constexpr const char* code = "UV";
		DVTX_ELEMENT_EXTRACTOR;
	};
	VERTEX_ELEMENT_ATTR(Float3Color)
	{
		using SysType = glm::vec3;
		static constexpr VertexFormat format = VertexFormat::Vec3F;
		static constexpr std::string_view semantic = "COLOR";
		static constexpr const char* code = "Cf3";
		DVTX_ELEMENT_EXTRACTOR;
	};
	VERTEX_ELEMENT_ATTR(Float4Color)
	{
		using SysType = glm::vec4;
		static constexpr VertexFormat format = VertexFormat::Vec4F;
		static constexpr std::string_view semantic = "COLOR";
		static constexpr const char* code = "Cf4";
		DVTX_ELEMENT_EXTRACTOR;
	};
	VERTEX_ELEMENT_ATTR(Char4Color)
	{
		using SysType = BGRAColor;
		static constexpr VertexFormat format = VertexFormat::Vec4C_UNorm;
		static constexpr std::string_view semantic = "COLOR";
		static constexpr const char* code = "Cc4";
		DVTX_ELEMENT_EXTRACTOR;
	};
	VERTEX_ELEMENT_ATTR(Tangent)
	{
		using SysType = glm::vec3;
		static constexpr VertexFormat format = VertexFormat::Vec3F;
		static constexpr std::string_view semantic = "TANGENT";
		static constexpr const char* code = "T";
		DVTX_ELEMENT_EXTRACTOR;
	};
	VERTEX_ELEMENT_ATTR(Bitangent)
	{
		using SysType = glm::vec3;
		static constexpr VertexFormat format = VertexFormat::Vec3F;
		static constexpr std::string_view semantic = "BITANGENT";
		static constexpr const char* code = "Bt";
		DVTX_ELEMENT_EXTRACTOR;
	};
	VERTEX_ELEMENT_ATTR(BoneIds)
	{
		using SysType = glm::vec<4, uint16_t>;
		static constexpr VertexFormat format = VertexFormat::Vec4UI16;
		static constexpr std::string_view semantic = "JOINTS";
		static constexpr const char* code = "BId";
		DVTX_ELEMENT_EXTRACTOR;
	};
	VERTEX_ELEMENT_ATTR(BoneWeights)
	{
		using SysType = glm::vec4;
		static constexpr VertexFormat format = VertexFormat::Vec4F;
		static constexpr std::string_view semantic = "WEIGHTS";
		static constexpr const char* code = "BWs";
		DVTX_ELEMENT_EXTRACTOR;
	};
	VERTEX_ELEMENT_ATTR(Unknown)
	{
		using SysType = int;
		static constexpr VertexFormat format = VertexFormat::Unknown;
		static constexpr std::string_view semantic = "UNKNOWN";
		static constexpr const char* code = "?";
		DVTX_ELEMENT_EXTRACTOR;
	};

	template<template<VertexLayout::VertexElement> class F, typename... Args>
	constexpr auto VertexLayout::Bridge(VertexLayout::VertexElement type, Args&&... args) -> decltype(F<VertexLayout::Unknown>::Exec(std::forward<Args>(args)...))
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

	constexpr size_t VertexLayout::Element::SizeOf(VertexElement type)
	{
		return Bridge<VertexSysSizeLookup>(type);
	}
	constexpr std::string_view VertexLayout::Element::NameOf(VertexLayout::VertexElement type)
	{
		return Bridge<VertexNameLookup>(type);
	}
	constexpr VertexFormat VertexLayout::Element::FormatOf(VertexLayout::VertexElement type)
	{
		return Bridge<VertexFormatLookup>(type);
	}
	constexpr std::string_view VertexLayout::Element::CodeOf(VertexLayout::VertexElement type)
	{
		return Bridge<VertexCodeLookup>(type);
	}

	class Vertex
	{
		friend class VertexBuffer;

	protected:
		Vertex(char* pData, const VertexLayout& layout);

		template<VertexLayout::VertexElement type>
		struct AttributeSetting
		{
			template<typename T>
			static constexpr void Exec(Vertex* pVertex, char* pAttribute, T&& val)
			{
				pVertex->SetAttribute<type>(pAttribute, std::forward<T>(val));
			}
		};

	public:
		template<VertexLayout::VertexElement Type>
		auto& Attr(const std::string& id = "") const
		{
			auto pAttribute = pData + layout.Resolve(Type, id).GetOffset();
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
		//VertexBuffer(VertexLayout layout, const Microsoft::glTF::MeshPrimitive& primitive, const gfx::WinGLTFLoaderContext& context, std::optional<ani::Skeleton> skeleton = std::nullopt);
		VertexBuffer(VertexLayout layout, const Shape3D& mesh);
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
			//dirty = true;
			trynass_msg(sizeof...(args) == layout.GetElementCount(), L"Different number of parameters where passed to the EmplaceBack function for a VertexLayout");
			Resize(NumElements() + 1);
			Back().SetAttributeByIndex(0u, std::forward<Args>(args)...);
		}

	protected:       
		VertexLayout layout;
		std::vector<char> buffer;
	};

	void inline VertexLayout::VertexElementAttr<VertexLayout::VertexElement::Position2D>::ExtractAndFill(VertexBuffer& buf, const std::string& id, const aiMesh& mesh, size_t i, ani::Skeleton const* skeleton) noexcept
	{
		buf[i].Attr<VertexLayout::VertexElement::Position2D>(id) = *reinterpret_cast<const SysType*>(&mesh.mVertices[i]);
	};
	DVTX_ELEMENT_SHAPE_EXTRACTOR_DEFINITION(Position2D, Vertices);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Position3D, mVertices);
	DVTX_ELEMENT_SHAPE_EXTRACTOR_DEFINITION(Position3D, Vertices);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Normal, mNormals);
	DVTX_ELEMENT_SHAPE_EXTRACTOR_DEFINITION(Normal, Normals);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(UV, mTextureCoords[0]);
	DVTX_ELEMENT_SHAPE_EXTRACTOR_DEFINITION(UV, TexCoords);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Float3Color, mColors[0]);
	DVTX_ELEMENT_SHAPE_EXTRACTOR_DEFINITION(Float3Color, Vertices);
	void inline VertexLayout::VertexElementAttr<VertexLayout::VertexElement::Float4Color>::ExtractAndFill(VertexBuffer& buf, const std::string& id, const aiMesh& mesh, size_t i, ani::Skeleton const* skeleton) noexcept
	{
		buf[i].Attr<VertexLayout::VertexElement::Float4Color>(id) = *reinterpret_cast<const SysType*>(&mesh.mColors[0][i]);
	};
	DVTX_ELEMENT_SHAPE_EXTRACTOR_DEFINITION(Float4Color, Vertices);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Char4Color, mColors[0]);
	DVTX_ELEMENT_SHAPE_EXTRACTOR_DEFINITION(Char4Color, Vertices);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Tangent, mTangents);
	DVTX_ELEMENT_SHAPE_EXTRACTOR_DEFINITION(Tangent, Tangents);
	DVTX_ELEMENT_AI_EXTRACTOR_DEFINITION(Bitangent, mBitangents);
	DVTX_ELEMENT_SHAPE_EXTRACTOR_DEFINITION(Bitangent, Bitangents);
	DVTX_ELEMENT_SHAPE_EXTRACTOR_DEFINITION(BoneWeights, Vertices);
	DVTX_ELEMENT_SHAPE_EXTRACTOR_DEFINITION(BoneIds, Vertices);
	DVTX_ELEMENT_SHAPE_EXTRACTOR_DEFINITION(Unknown, Vertices);

	void inline VertexLayout::VertexElementAttr<VertexLayout::VertexElement::BoneWeights>::ExtractAndFill(VertexBuffer& buf, const std::string& id, const aiMesh& mesh, size_t i, ani::Skeleton const* skeleton) noexcept
	{

	}

	void inline VertexLayout::VertexElementAttr<VertexLayout::VertexElement::Unknown>::ExtractAndFill(VertexBuffer& buf, const std::string& id, const aiMesh& mesh, size_t i, ani::Skeleton const* skeleton) noexcept
	{

	}
}