#pragma once
#include <Core/third/glm/glm.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include "Color.h"
#include <utility>

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
			Vec3C,
			Vec4C,
		};

		template <VertexElement>
		struct VertexElementAttr {};
		template <> struct VertexElementAttr<VertexElement::Position3D>
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Position";
		};
		template <> struct VertexElementAttr<VertexElement::Position2D>
		{
			using SysType = glm::vec2;
			static constexpr Format format = Format::Vec2F;
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
			static constexpr Format format = Format::Vec4C;
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
		template <> struct VertexElementAttr<VertexElement::Bitangent>
		{
			using SysType = glm::vec3;
			static constexpr Format format = Format::Vec3F;
			static constexpr const char* semantic = "Bitangent";
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
			assert("Invalid element type" && false);
			return F<VertexLayout::Count>::Exec(std::forward<Args>(args)...);
		}

		template<VertexLayout::VertexElement type>
		struct SysSizeLookup
		{
			static constexpr auto Exec() noexcept
			{
				return sizeof(VertexLayout::VertexElementAttr<type>::SysType);
			}
		};

			

	public:
		VertexLayout( VertexElement alArray[] , size_t elNum)
		{
			int elCounter[static_cast<int>(VertexElement::Unknown)] = {};

			for (int i = 0; i < elNum; i++)
			{
				auto counter = elCounter[static_cast<int>(alArray[i])]++;
			}
		}
	public:
		std::vector<std::pair<VertexElement,std::string>> Elements;
	};

	class VertexBuffer
	{
		VertexLayout layout;
	private:        
		char* buffer;
		int bufferByteSize;
	};
}