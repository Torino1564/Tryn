#pragma once
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/Bindables/ConstantBuffer.h>

namespace tryn::gfx
{
	class Drawable;

	class Step
	{
	public:
		void AddBindable(std::shared_ptr<IBindable> bindable);
		void Bind(IGraphics& gfx) const;
		void Draw(IGraphics& gfx, Drawable* parent) const;
		std::vector<std::shared_ptr<gfx::IBindable>> bindables;
	};
}