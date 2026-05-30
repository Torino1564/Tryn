#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <utility>
#include <memory>
#include <Core/src/utl/Exception.h>
#include <Core/src/gfx/CPUBuffer.h>
#include <Core/src/utl/Assert.h>
#include <string_view>

ZT_EX_DEF(DcbException);

#define CONSTANT_BUFFER_ELEMENTS \
		X( Bool ) \
		X( Float ) \
		X( Float2 ) \
		X( Float3 ) \
		X( Float4 ) \
		X( Matrix4 ) \
		X( Matrix3 ) \
		X( UInt32 ) \
		X( UInt64 )

namespace tryn::gfx
{
	class ConstantBuffer;

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
		

		template<typename T>
		struct ReverseTypeAttr
		{
			static constexpr bool valid = false;
		};

		template <typename T>
		struct TypeAttrLookup
		{
			static constexpr bool valid = false;
		};

		template<template<ConstantBufferLayout::Type> class F, typename... Args>
		static constexpr auto Bridge(const ConstantBufferLayout::Type type, Args&&... args);
		static constexpr size_t SizeOf(ConstantBufferLayout::Type type);

		class Node
		{
			friend class ConstantBufferLayout;
			friend class ConstantBuffer;

		public:
			Node() = default;
			Node(Type type, std::string id);
			void Append(Node child);
			void Append(Type type, std::string name);
			//bool IsRoot() const;
			bool IsLeaf() const;
			static Node& GetEmpty();
			Node& operator[](std::string_view id);
			Node& operator[](std::size_t index);
			Node& IndexByName(std::string_view id);
			Node& IndexByKey(std::size_t key);
			void Resize(std::size_t newSize, ConstantBuffer& layout);
			bool Validate() const;
			Type GetType() const;
			size_t GetOffset() const;
			void Set(Node, std::size_t numElements);
			std::size_t Size() const;
		private:
			std::vector<Node> children;
			Type type = Empty;
			std::string id = "";
			bool solid = false;
			size_t offset = 0;
		};
	public:
		ConstantBufferLayout();
		void Append(Node child);
		void Append(Type type, std::string name);
		void Solidify();
		bool IsSolid() const;
		size_t Size() const;
		Node& operator[](const std::string& id);
		Node& GetRoot();
	private:
		std::shared_ptr<Node> root;
		bool solid = false;
		size_t size = 0;
	};

	template <> struct ConstantBufferLayout::TypeAttr<ConstantBufferLayout::Type::Bool>
	{
		using TrueType = bool;
		static constexpr size_t TrueTypeSize = 4;
	};
	template <> struct ConstantBufferLayout::TypeAttr<ConstantBufferLayout::Type::Float>
	{
		using TrueType = float;
		static constexpr size_t TrueTypeSize = 4;
	};
	template <> struct ConstantBufferLayout::TypeAttr<ConstantBufferLayout::Type::Float2>
	{
		using TrueType = glm::vec2;
		static constexpr size_t TrueTypeSize = sizeof(glm::vec2);
	};
	template <> struct ConstantBufferLayout::TypeAttr<ConstantBufferLayout::Type::Float3>
	{
		using TrueType = glm::vec3;
		static constexpr size_t TrueTypeSize = sizeof(glm::vec3);
	};
	template <> struct ConstantBufferLayout::TypeAttr<ConstantBufferLayout::Type::Float4>
	{
		using TrueType = glm::vec4;
		static constexpr size_t TrueTypeSize = sizeof(glm::vec4);
	};
	template <> struct ConstantBufferLayout::TypeAttr<ConstantBufferLayout::Type::Matrix4>
	{
		using TrueType = glm::mat4;
		static constexpr size_t TrueTypeSize = sizeof(glm::mat4);
	};
	template <> struct ConstantBufferLayout::TypeAttr<ConstantBufferLayout::Type::Matrix3>
	{
		using TrueType = glm::mat3;
		static constexpr size_t TrueTypeSize = sizeof(glm::mat3);
	};
	template <> struct ConstantBufferLayout::TypeAttr<ConstantBufferLayout::Type::UInt32>
	{
		using TrueType = uint32_t;
		static constexpr size_t TrueTypeSize = sizeof(TrueType);
	};
	template <> struct ConstantBufferLayout::TypeAttr<ConstantBufferLayout::Type::UInt64>
	{
		using TrueType = uint64_t;
		static constexpr size_t TrueTypeSize = sizeof(TrueType);
	};

#define X(el) \
	template<> struct ConstantBufferLayout::ReverseTypeAttr<typename ConstantBufferLayout::TypeAttr<ConstantBufferLayout::Type::el>::TrueType> \
	{ \
		static constexpr ConstantBufferLayout::Type type = ConstantBufferLayout::Type::el; \
		static constexpr bool valid = true; \
	};
	CONSTANT_BUFFER_ELEMENTS
#undef X

#define X(el) \
	template<> struct ConstantBufferLayout::TypeAttrLookup<typename ConstantBufferLayout::TypeAttr<ConstantBufferLayout::Type::el>::TrueType> \
	{\
		static constexpr ConstantBufferLayout::Type type = ConstantBufferLayout::Type::el; \
		static constexpr bool valid = true; \
	};
	CONSTANT_BUFFER_ELEMENTS
#undef X

	class ElementView
	{
		friend class ConstantBufferLayout::Node;
	public:
		ElementView(ConstantBufferLayout::Node& node, char* pBytes, ConstantBuffer* pBuffer);
		ElementView operator[](std::string_view id);

		ElementView operator[](std::size_t key);

		void Resize(std::size_t newSize);

		template <typename T>
		T& Get()
		{
			// Validate Type
			trynass_msg(ConstantBufferLayout::TypeAttrLookup<T>::valid == true, L"Get called with an unsupported type");
			static_assert(ConstantBufferLayout::TypeAttrLookup<T>::valid == true);

			return *std::bit_cast<T*>(pBytes);
		}
		ConstantBufferLayout::Node& Node();
		bool Exists() const;

		template<typename T>
		T& operator=(const T& rhs) const
		{
			static_assert(ConstantBufferLayout::ReverseTypeAttr<std::remove_const_t<T>>::valid, "Unsupported SysType used in assignment");
			*(std::bit_cast<T*>(pBytes)) = rhs;
			return *(std::bit_cast<T*>(pBytes));
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
		ConstantBuffer(ConstantBufferLayout cbl);

		void Rebase();

		ElementView operator[](std::string_view id);

		constexpr void* Data() const noexcept override;

		std::size_t Stride() const noexcept override;

		constexpr std::size_t ByteSize() const noexcept override;

		std::size_t Size() const noexcept override;
		void Resize(const std::size_t newSize) override;
	private:
		ConstantBufferLayout layout;
		std::vector<std::byte> buffer;
	};

	template<template<ConstantBufferLayout::Type> class F, typename... Args>
	static constexpr auto Bridge(const ConstantBufferLayout::Type type, Args&&... args)
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
}
