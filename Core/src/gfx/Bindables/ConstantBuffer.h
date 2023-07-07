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

ZT_EX_DEF(DcbException);

#define CONSTANT_BUFFER_ELEMENTS \
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
			using SysType = float;
			static constexpr size_t SysTypeSize = 4;
		};
		template <> struct TypeAttr<Float>
		{
			using SysType = float;
			static constexpr size_t SysTypeSize = 4;
		};
		template <> struct TypeAttr<Float2>
		{
			using SysType = glm::vec2;
			static constexpr size_t SysTypeSize = sizeof(glm::vec2);
		};
		template <> struct TypeAttr<Float3>
		{
			using SysType = glm::vec3;
			static constexpr size_t SysTypeSize = sizeof(glm::vec3);
		};
		template <> struct TypeAttr<Float4>
		{
			using SysType = glm::vec4;
			static constexpr size_t SysTypeSize = sizeof(glm::vec4);
		};
		template <> struct TypeAttr<Matrix4>
		{
			using SysType = glm::mat4;
			static constexpr size_t SysTypeSize = sizeof(glm::mat4);
		};
		template <> struct TypeAttr<Matrix3>
		{
			using SysType = glm::mat3;
			static constexpr size_t SysTypeSize = sizeof(glm::mat3);
		};

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
			Node& GetEmpty() const;
			Node& operator[](std::string id);
			bool Validate() const;
			Type GetType() const;
			size_t GetOffset() const;
			auto& Ref()
			{
				return reinterpret_cast<TypeAttr<type>::SysType*>()
			}
			// returns size in byes

			std::vector<Node> children;
		private:
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
		Node& operator[](std::string id);
	private:
		std::unique_ptr<Node> root;
		bool solid = false;
		size_t size = 0;
	};

	class ConstantBuffer
	{
	public:
		ConstantBuffer(ConstantBufferLayout cbl);

	private:
		ConstantBufferLayout layout;
		std::vector<char> buffer;
	};
}
