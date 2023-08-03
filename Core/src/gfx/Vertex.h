#pragma once
#include <Core/third/glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <utility>
#include <Core/src/utl/Assert.h>
#include <Core/src/utl/Exception.h>
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/log/Log.h>
#include <assimp/scene.h>

#define DVTX_ELEMENT_AI_EXTRACTOR(member) static SysType Extract( const aiMesh& mesh,size_t i ) noexcept {return *reinterpret_cast<const SysType*>(&mesh.member[i]);}

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
			Unknown
		};

		template <VertexElement>
		struct VertexElementAttr {};
		template <> struct VertexElementAttr<Position2D>
		{
			using SysType = glm::vec2;
			static constexpr Format format = Format::Vec2F;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P2";
			DVTX_ELEMENT_AI_EXTRACTOR(mVertices)
		};
		template <> struct VertexElementAttr<Position3D>
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Position";
			static constexpr const char* code = "P3";
			DVTX_ELEMENT_AI_EXTRACTOR(mVertices)
		};
		template <> struct VertexElementAttr<Normal>
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Normal";
			static constexpr const char* code = "N";
			DVTX_ELEMENT_AI_EXTRACTOR(mNormals)
		};
		template <> struct VertexElementAttr<UV>
		{
			using SysType = glm::vec2;
			static constexpr Format format = Format::Vec2F;
			static constexpr const char* semantic = "Texcoord";
			static constexpr const char* code = "UV";
			DVTX_ELEMENT_AI_EXTRACTOR(mTextureCoords[0])
		};
		template <> struct VertexElementAttr<Float3Color>
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "Cf3";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0])
		};
		template <> struct VertexElementAttr<Float4Color>
		{
			using SysType = glm::vec4;
			static constexpr Format format = Format::Vec4F;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "Cf4";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0])
		};
		template <> struct VertexElementAttr<Char4Color>
		{
			using SysType = BGRAColor;
			static constexpr Format format = Format::Vec4C_UNorm;
			static constexpr const char* semantic = "Color";
			static constexpr const char* code = "Cc4";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0])
		};
		template <> struct VertexElementAttr<Tangent>
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Tangent";
			static constexpr const char* code = "T";
			DVTX_ELEMENT_AI_EXTRACTOR(mTangents)
		};
		template <> struct VertexElementAttr<Bitangent>
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Bitangent";
			static constexpr const char* code = "Bt";
			DVTX_ELEMENT_AI_EXTRACTOR(mBitangents)
		};
		template <> struct VertexElementAttr<Unknown>
		{
			using SysType = int;
			static constexpr Format format = Format::Unknown;
			static constexpr const char* semantic = "Unknown";
			static constexpr const char* code = "?";
			DVTX_ELEMENT_AI_EXTRACTOR(mFaces)
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
			throw DvtxException("Invalid element type");
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
				static constexpr void Exec(VertexBuffer& pBuf, const aiMesh& mesh)
				{
					for (auto end = mesh.mNumVertices, i = 0u; i < end; i++)
					{
						pBuf[i].Attr<type>(0) = VertexLayout::VertexElementAttr<type>::Extract(mesh, i);
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
		VertexLayout( Args... args )
		{
			int offset = 0;
			elCounter.resize(static_cast<int>(VertexElement::Unknown));

			AppendElement( offset , args...);
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
			throw DvtxException("Could not resolve an element");
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
		void AppendElement( int& offset, Element element)
		{
			Elements.emplace_back(VertexLayout::Element(element, offset), elCounter[static_cast<int>(element)]++);
			offset += static_cast<int>(VertexLayout::Element::SizeOf(element));
		}
		template <typename First , typename ... Args>
		void AppendElement( int& offset , First first, Args ... rest)
		{
			AppendElement( offset , first);
			AppendElement( offset , rest...);
		}
	public:
		std::vector<std::pair<Element,int>> Elements;
	private:
		std::vector<int> elCounter = {};
		size_t size;
	};

	class Vertex
	{
		friend class VertexBuffer;

		template<VertexLayout::VertexElement type>
		struct AttributeSetting
		{
			template<typename T>
			static constexpr auto Exec(Vertex* pVertex, char* pAttribute, T&& val)
			{
				return pVertex->SetAttribute<type>(pAttribute, std::forward<T>(val));
			}
		};
	protected:
		Vertex(char* pData, const VertexLayout& layout);

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
				throw DvtxException("Parameter attribute type mismatch");
			}
		}
	public:
		char* pData = nullptr;
		const VertexLayout& layout;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(VertexLayout layout_, size_t size = 0);
		VertexBuffer(VertexLayout layout, const aiMesh& mesh);
		void Resize(size_t newSize);
		size_t Size() const;
		Vertex operator[](int i);
		Vertex Back();
		char* Data();
		size_t BufferSize() const;
		size_t Stride() const;
		const VertexLayout& GetLayout() const;
		bool& GetDirty();

		template<typename ... Args>
		void EmplaceBack(Args&& ... args)
		{
			dirty = true;
			trynass_msg(sizeof...(args) == layout.GetElementCount(), L"Different number of parameters where passed to the EmplaceBack function for a VertexLayout");
			Resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Args>(args)...);
		}

	protected:       
		bool dirty = true;
		VertexLayout layout;
		std::vector<char> buffer;
	};
}