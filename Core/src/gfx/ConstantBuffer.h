#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <vector>
#include <Core/third/glm/glm.hpp>
#include <string>
#include <optional>
#include <utility>
#include <memory>
#include <Core/src/utl/Exception.h>
#include <unordered_map>
#include <Core/src/gfx/BindablePool.h>
#include <Core/src/gfx/CPUBuffer.h>
#include <Core/src/gfx/IBufferFwd.h>

ZT_EX_DEF(DcbException);

#define CONSTANT_BUFFER_ELEMENTS \
		X( Bool ) \
		X( Float ) \
		X( Float2 ) \
		X( Float3 ) \
		X( Float4 ) \
		X( Matrix4 ) \
		X( Matrix3 )

namespace tryn::gfx
{
	class ConstantBufferLayout
	{
	public:
		enum Type
		{
#define X(el) el,
			CONSTANT_BUFFER_ELEMENTS
#undef X
			Struct,
			Array,
			Empty
		};

		template <Type>
		struct TypeAttr
		{
			using TrueType = float;
			static constexpr size_t TrueTypeSize = 4;
		};
		template <> struct TypeAttr<Bool>
		{
			using TrueType = bool;
			static constexpr size_t TrueTypeSize = 4;
		};
		template <> struct TypeAttr<Float>
		{
			using TrueType = float;
			static constexpr size_t TrueTypeSize = 4;
		};
		template <> struct TypeAttr<Float2>
		{
			using TrueType = glm::vec2;
			static constexpr size_t TrueTypeSize = sizeof(glm::vec2);
		};
		template <> struct TypeAttr<Float3>
		{
			using TrueType = glm::vec3;
			static constexpr size_t TrueTypeSize = sizeof(glm::vec3);
		};
		template <> struct TypeAttr<Float4>
		{
			using TrueType = glm::vec4;
			static constexpr size_t TrueTypeSize = sizeof(glm::vec4);
		};
		template <> struct TypeAttr<Matrix4>
		{
			using TrueType = glm::mat4;
			static constexpr size_t TrueTypeSize = sizeof(glm::mat4);
		};
		template <> struct TypeAttr<Matrix3>
		{
			using TrueType = glm::mat3;
			static constexpr size_t TrueTypeSize = sizeof(glm::mat3);
		};

		template<typename T>
		struct ReverseTypeAttr
		{
			static constexpr bool valid = false;
		};
#define X(el) \
	template<> struct ReverseTypeAttr<typename TypeAttr<el>::TrueType> \
	{ \
		static constexpr Type type = el; \
		static constexpr bool valid = true; \
	};
		CONSTANT_BUFFER_ELEMENTS
#undef X

			template <typename T>
		struct TypeAttrLookup
		{
			static constexpr bool valid = false;
		};

#define X(el) \
		template<> struct TypeAttrLookup<typename TypeAttr<el>::TrueType> \
		{\
			static constexpr Type type = el; \
			static constexpr bool valid = true; \
		};
		CONSTANT_BUFFER_ELEMENTS
#undef X

			template<template<ConstantBufferLayout::Type> class F, typename... Args>
		static constexpr auto Bridge(ConstantBufferLayout::Type type, Args&&... args)
		{
			switch (type)
			{
#define X(el) case ConstantBufferLayout::Type::el: return F<ConstantBufferLayout::Type::el>::Exec( std::forward<Args>( args )... );
				CONSTANT_BUFFER_ELEMENTS
#undef X
			}
			throw DcbException("Invalid element type");
			return F<ConstantBufferLayout::Type::Empty>::Exec(std::forward<Args>(args)...);
		}

		static constexpr size_t SizeOf(ConstantBufferLayout::Type type);

		struct Node
		{
			friend class ConstantBufferLayout;

			Node(Type type, std::string id);
			void Append(Node child);
			bool IsRoot() const;
			bool IsLeaf() const;
			static Node& GetEmpty();
			Node& operator[](std::string id);
			Node& IndexByName(std::string id);
			bool Validate() const;
			Type GetType() const;
			size_t GetOffset() const;

		private:
			std::vector<Node> children;
			Type type = Type::Empty;
			std::optional<Node*> parent;
			std::string id;
			bool solid = false;
			size_t offset = 0;
		};
	public:
		ConstantBufferLayout();
		void Append(Node child);
		void Solidify();
		bool IsSolid() const;
		size_t Size() const;
		Node& operator[](const std::string& id);
		Node& GetRoot();
	private:
		std::unique_ptr<Node> root;
		bool solid = false;
		size_t size = 0;
	};

	class ElementView
	{
		friend struct ConstantBufferLayout::Node;
	public:
		ElementView(ConstantBufferLayout::Node& node, char* pBytes);
		ElementView operator[](std::string id)
		{
			trynass_msg(node.GetType() == ConstantBufferLayout::Type::Struct, L"Tried to index to a non struct ElementView!");

			return ElementView(node[id], pBytes - (node[id].GetOffset() - node.GetOffset()));
		}

		template <typename T>
		T& Get()
		{
			// Validate Type
			trynass_msg(ConstantBufferLayout::TypeAttrLookup<T>::valid == true, L"Get called with an unsupported type");
			static_assert(ConstantBufferLayout::TypeAttrLookup<T>::valid == true);

			return *reinterpret_cast<T*>(pBytes);
		}

		bool Exists() const;

		template<typename T>
		T& operator=(const T& rhs) const
		{
			static_assert(ConstantBufferLayout::ReverseTypeAttr<std::remove_const_t<T>>::valid, "Unsupported SysType used in assignment");
			*(reinterpret_cast<T*>(pBytes)) = rhs;
			return *(reinterpret_cast<T*>(pBytes));
		}
	private:
		ConstantBufferLayout::Node& node;
		char* pBytes;
	};

	class ConstantBuffer : public CPUBuffer
	{
	public:
		ConstantBuffer(ConstantBufferLayout cbl)
		{
			layout = std::move(cbl);
			buffer.Resize(layout.Size());
		}
		ElementView operator[](std::string id)
		{
			buffer.SetDirty();
			auto& indexTo = layout.GetRoot().IndexByName(id);
			if (indexTo.GetType() == gfx::ConstantBufferLayout::Type::Empty)
			{
				return ElementView{ indexTo, nullptr };
			}
			else
			{
				return ElementView{ indexTo, (char*)(buffer.Data()) + indexTo.GetOffset() };
			}
		}
		constexpr void* Data() const noexcept override
		{
			return buffer.Data();
		}
		std::size_t Stride() const noexcept override
		{
			return buffer.Stride();
		}
		constexpr std::size_t ByteSize() const noexcept override
		{
			return buffer.ByteSize();
		}
		std::size_t Size() const noexcept override
		{
			return layout.Size();
		}
		void Resize(const std::size_t newSize) override
		{
			throw CPUBufferException("Cannot Resize a Constant Buffer!");
		}
	private:
		ConstantBufferLayout layout;
		FlatBuffer buffer;
	};
}
