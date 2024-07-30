#pragma once
#include "Bindable.h"
#include <Core/src/gfx/IBufferFwd.h>
#include <Core/src/gfx/IGraphics.h>

namespace tryn::gfx
{
	class Drawable;
	class ITransformCBuf : public IBindable
	{
	public:
		void BindTransformCBuf(const Drawable* parent);
		void BindTransformCBuf(const Drawable* parent, const IContext& context);
		void BindParent(const Drawable* parent) const;
		void Bind() override;
		void Bind(const IContext& context) override;

		void Update() const;
		void Update(const IContext& context) const;
	protected:
		ITransformCBuf(const IGraphics& gfx) :gfx(gfx) {}
		const IGraphics& gfx;
		static std::unique_ptr<IVtxConstantBuffer> pVCB;
		mutable const Drawable* parent = nullptr;
	};
}