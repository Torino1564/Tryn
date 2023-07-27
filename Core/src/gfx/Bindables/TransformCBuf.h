#pragma once
#include "Bindable.h"
#include <Core/src/gfx/Bindables/ConstantBuffer.h>

namespace tryn::gfx
{
	class Drawable;
	class ITransformCBuf : public IBindable
	{
	public:
		void BindParent(Drawable& parent);
		void Bind() override;
		void Update() const;
	protected:
		ITransformCBuf(IGraphics& gfx) :gfx(gfx){}
		IGraphics& gfx;
		static std::unique_ptr<IVtxConstantBuffer> pVCB;
		Drawable* parent = nullptr;
	};
}