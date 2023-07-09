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
		template <> struct VertexElementAttr<VertexElement::Position2D>
		{
			using SysType = glm::vec2;
			static constexpr Format format = Format::Vec2F;
			static constexpr const char* semantic = "Position";
		};
		template <> struct VertexElementAttr<VertexElement::Position3D>
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Position";
		};
		template <> struct VertexElementAttr<VertexElement::Normal>
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Normal";
		};
		template <> struct VertexElementAttr<VertexElement::UV>
		{
			using SysType = glm::vec2;
			static constexpr Format format = Format::Vec2F;
			static constexpr const char* semantic = "Texcoord";
		};
		template <> struct VertexElementAttr<VertexElement::Float3Color>
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Color";
		};
		template <> struct VertexElementAttr<VertexElement::Float4Color>
		{
			using SysType = glm::vec4;
			static constexpr Format format = Format::Vec4F;
			static constexpr const char* semantic = "Color";
		};
		template <> struct VertexElementAttr<VertexElement::Char4Color>
		{
			using SysType = BGRAColor;
			static constexpr Format format = Format::Vec4C_UNorm;
			static constexpr const char* semantic = "Color";
		};
		template <> struct VertexElementAttr<VertexElement::Tangent>
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Tangent";
		};
		template <> struct VertexElementAttr<VertexElement::Bitangent>
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Bitangent";
		};
		template <> struct VertexElementAttr<VertexElement::Unknown>
		{
			using SysType = int;
			static constexpr Format format = Format::Unknown;
			static constexpr const char* semantic = "Unknown";
		};
		
		template<template<VertexLayout::VertexElement> class F, typename... Args>
		static constexpr auto Bridge(VertexLayout::VertexElement type, Args&&... args)
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
		public:
			Element(VertexElement type, size_t offset);
			size_t GetOffsetAfter() const;
			size_t GetOffset() const;
			size_t Size() const;
			static constexpr size_t SizeOf(VertexElement type);
			static constexpr const char* NameOf(VertexElement type);
			static constexpr Format FormatOf(VertexElement type);
			Format GetFormat() const;
			const char* GetName() const;
			VertexElement GetType() const;
		private:
			VertexElement type;
			size_t offset;
		};

	public:
		VertexLayout()
		{
			size = 0;
			elCounter.resize(static_cast<int>(VertexElement::Unknown));
		}
		template <typename...Args>
		VertexLayout( Args... args )
		{
			int offset = 0;
			elCounter.resize(static_cast<int>(VertexElement::Unknown));

			AppendElement( offset , args...);
			size = Elements.back().first.GetOffsetAfter();
		}
		// Returns size in bytes
		size_t Size() const
		{
			return Elements.back().first.GetOffsetAfter();
		}
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
		const Element& ResolveByIndex(size_t i) const
		{
			trynass_msg(i < Size(), L"Layout Indexed out of bounds!");
			return Elements[i].first;
		}
		// Returns number of elements
		size_t GetElementCount() const
		{
			return Elements.size();
		}
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
		Vertex(char* pData, const VertexLayout& layout)
			:
			pData(pData),
			layout(layout)
		{
			trynass_msg(pData != nullptr, L"Vertex constructed from a nullptr!");
		}

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

	class VertexBuffer : public IBindable
	{
	public:
		VertexBuffer(VertexLayout layout_, size_t size)
		{
			trynass_msg(layout_.GetElementCount() != 0, L"Attempted to create a VertexBuffer with an empty layout");
			this->layout = std::move(layout_);
			Resize(layout.Size() * size);
		}
		void Resize(size_t newSize)
		{
			buffer.resize(newSize);
		}
		size_t Size() const
		{
			return buffer.size() / layout.Size();
		}
		Vertex operator[](int i)
		{
			trynass_msg(i < Size() , L"VertexBuffer indexed out of bounds");
			return Vertex{ buffer.data() + layout.Size() * i, layout };
		}
		Vertex Back()
		{
			trynass_msg(buffer.size() != 0u , L"Back called on an empty VertexBuffer");
			return Vertex{ buffer.data() + buffer.size() - layout.Size(),layout };
		}

		template<typename ... Args>
		void EmplaceBack(Args&& ... args)
		{
			dirty = true;
			trynass_msg(sizeof...(args) == layout.GetElementCount(), L"Different number of parameters where passed to the EmplaceBack function for a VertexLayout");
			Resize(buffer.size() + layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Args>(args)...);
		}
		char* Data() 
		{
			return buffer.data();
		}
		size_t BufferSize() const
		{
			return buffer.size();
		}
		size_t Stride() const
		{
			return layout.Size();
		}
		const VertexLayout& GetLayout() const
		{
			return layout;
		}
		bool& GetDirty()
		{
			return dirty;
		}

		virtual std::vector<char> GetLayoutFromVB() const
		{
			trylog.error(L"GetLayoutFromVB member on the VertexBuffer virtual class was called");
			return {};
		}

		void Bind() override
		{
			trylog.error(L"Bind member on the VertexBuffer virtual class was called");
		}

	protected:       
		bool dirty = true;
		VertexLayout layout;
		std::vector<char> buffer;
	};
}