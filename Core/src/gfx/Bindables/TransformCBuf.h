#pragma once
#include "Bindable.h"

namespace tryn::gfx
{
	class IGraphics;
	class Drawable;
	class ITransformCBuf : public IBindable
	{
	public:
		static std::shared_ptr<ITransformCBuf> Resolve(const IGraphics& gfx);
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
		static std::unique_ptr<class IConstantBufferResource> pVCB;
		mutable const Drawable* parent = nullptr;
	};
}