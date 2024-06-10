#pragma once
#include <vector>
#include <Core/third/glm/glm.hpp>
#include <string>
#include <optional>
#include <utility>
#include <memory>
#include <Core/src/utl/Exception.h>
#include <Core/src/gfx/CPUBuffer.h>
#include <Core/src/gfx/IBufferFwd.h>
#include <Core/src/utl/Assert.h>

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

		class Node
		{
			friend class ConstantBufferLayout;
			friend class ConstantBuffer;

		public:
			Node() = default;
			Node(Type type, std::string id);
			void Append(Node child);
			void Append(ConstantBufferLayout::Type type, std::string name);
			//bool IsRoot() const;
			bool IsLeaf() const;
			static Node& GetEmpty();
			Node& operator[](std::string_view id);
			Node& operator[](std::size_t index);
			Node& IndexByName(std::string_view id);
			Node& IndexByKey(std::size_t key);
			void Resize(std::size_t newSize, class ConstantBuffer& layout);
			bool Validate() const;
			Type GetType() const;
			size_t GetOffset() const;
			void Set(Node, std::size_t numElements);
			std::size_t Size() const;
		private:
			std::vector<Node> children;
			Type type = Type::Empty;
			//std::optional<Node*> parent = std::nullopt;
			std::string id = "";
			bool solid = false;
			size_t offset = 0;
		};
	public:
		ConstantBufferLayout();
		void Append(Node child);
		void Append(ConstantBufferLayout::Type type, std::string name);
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
		friend class ConstantBufferLayout::Node;
	public:
		ElementView(ConstantBufferLayout::Node& node, char* pBytes, ConstantBuffer* pBuffer);
		ElementView operator[](std::string_view id)
		{
			trynass_msg(node.GetType() == ConstantBufferLayout::Type::Struct, L"Tried to index by name into a non struct ElementView!");
			const auto finalOffset = node[id].GetOffset() - node.GetOffset();
			return ElementView(node[id], pBytes + finalOffset, pBuffer);
		}
		ElementView operator[](std::size_t key)
		{
			trynass_msg(node.GetType() == ConstantBufferLayout::Type::Array, L"Tried to index by key into a non array ElementView!");
			auto finalOffset = node[key].GetOffset() - node.GetOffset();
			return ElementView(node[key], pBytes + finalOffset, pBuffer);
		}
		void Resize(std::size_t newSize)
		{
			node.Resize(newSize, *pBuffer);
		}
		template <typename T>
		T& Get()
		{
			// Validate Type
			trynass_msg(ConstantBufferLayout::TypeAttrLookup<T>::valid == true, L"Get called with an unsupported type");
			static_assert(ConstantBufferLayout::TypeAttrLookup<T>::valid == true);

			return *reinterpret_cast<T*>(pBytes);
		}
		auto& Node()
		{
			return node;
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
		ConstantBuffer* pBuffer;
		char* pBytes;
	};

	class ConstantBuffer : public CPUBuffer
	{
		friend class ConstantBufferLayout::Node;
	public:
		ConstantBuffer(ConstantBufferLayout cbl)
		{
			layout = std::move(cbl);
			Rebase();
		}
		void Rebase()
		{
			buffer.resize(layout.Size(), (std::byte)(0u));
		}
		ElementView operator[](std::string_view id)
		{
			SetDirty(); 
			auto& indexTo = layout.GetRoot().IndexByName(id);
			if (indexTo.GetType() == gfx::ConstantBufferLayout::Type::Empty)
			{
				return ElementView{ indexTo, nullptr, this};
			}
			else
			{
				return ElementView{ indexTo, (char*)(buffer.data() + indexTo.GetOffset()), this};
			}
		}
		constexpr void* Data() const noexcept override
		{
			return (void*)(buffer.data());
		}
		std::size_t Stride() const noexcept override
		{
			return 0u;
		}
		constexpr std::size_t ByteSize() const noexcept override
		{
			return buffer.size();
		}
		std::size_t Size() const noexcept override
		{
			return layout.Size();
		}
		void Resize(const std::size_t newSize) override;
	private:
		ConstantBufferLayout layout;
		std::vector<std::byte> buffer;
	};
}
