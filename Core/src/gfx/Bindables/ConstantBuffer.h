#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <vector>
#include <Core/third/glm/glm.hpp>
#include <string>
#include <optional>

#define CONSTANT_BUFFER_ELEMENTS \
		X( Float ) \
		X( Float2 ) \
		X( Float3 ) \
		X( Float4 ) \
		X( Matrix4 ) \
		X( Matrix3 ) \
		X( Empty )

namespace tryn::gfx
{
	class ConstantBufferLayout
	{
		enum Type
		{
			#define X(el) el,
			CONSTANT_BUFFER_ELEMENTS
			#undef X
			Struct,
			Array,
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

		struct Node
		{
			Node(Type type, std::wstring id);
			void Append(Node child);
			bool IsRoot() const;
			bool IsLeaf() const;
			Node& GetEmpty() const;
			Node& operator[](std::wstring id);
			bool Validate() const;

			Type type = Type::Empty;
			std::vector<Node> children;
			std::optional<Node&> parent;
			std::wstring id;
			size_t offset;
		};
	private:


	};

	class ConstantBuffer
	{
	private:
		std::vector<char> buffer;
		ConstantBufferLayout layout;
	};
}
