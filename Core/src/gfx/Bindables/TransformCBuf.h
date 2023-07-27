#pragma once
#include "Bindable.h"
#include <Core/src/gfx/IGraphics.h>
#include <Core/third/glm/glm.hpp>
#include <Core/src/ent/Entity.h>
#include <Core/src/gfx/Bindables/ConstantBuffer.h>

namespace tryn::gfx
{
	class ITransformCBuf : public IBindable
	{
	public:
		void BindParent(Drawable& parent)
		{
			this->parent = &parent;
		}
	protected:
		static std::unique_ptr<IVtxConstantBuffer> pVCB;
		Drawable* parent = nullptr;
	};
}