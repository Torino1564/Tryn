#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <vector>
#include <Core/third/glm/glm.hpp>

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
		enum Types
		{
			#define X(el) el,
			CONSTANT_BUFFER_ELEMENTS
			#undef X
		};

		template <Types>
		struct TypeAttr
		{
			usign SysType = float;
		};
		template <> struct TypeAttr<Float>
		{
			using SysType = float;

		};

	};

	class ConstantBuffer
	{
	private:
		std::vector<char> buffer;
		ConstantBufferLayout layout;
	};
}
