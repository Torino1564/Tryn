#pragma once
#include <vector>
#include <Core/third/glm/gtx/euler_angles.hpp>
#include <Core/src/gfx/Bindables/Bindable.h>
#include <Core/src/gfx/Vertex.h>
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/Bindables/PolyVBuffer.h>
#include <Core/src/gfx/Bindables/IndexBuffer.h>
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>
#include <Core/src/gfx/Drawable.h>

namespace tryn::gfx
{
	class Mesh : public Drawable
	{
	public:
		virtual ~Mesh() = default;

		[[nodiscard]] std::string GetTag() const
		{
			return tag;
		}

		[[nodiscard]] virtual bool IsStatic() const = 0;
	protected:
		std::string tag;
	};
}
