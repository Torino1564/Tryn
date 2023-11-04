#pragma once
#include "Bindable.h"
#include <Core/src/gfx/Bindables/ConstantBuffer.h>

namespace tryn::gfx
{
	class Drawable;
	class ITransformCBuf : public IBindable
	{
	public:
		void BindTransformCBuf(const Drawable* parent);
		void BindTransformCBuf(const Drawable* parent, IContext& context);
		void BindParent(const Drawable* parent) const;
		void Bind() override;
		void Bind(IContext& context) override;

		void Update() const;
	protected:
		ITransformCBuf(IGraphics& gfx) :gfx(gfx) {}
		IGraphics& gfx;
		static std::unique_ptr<IVtxConstantBuffer> pVCB;
		mutable const Drawable* parent = nullptr;
	};
}