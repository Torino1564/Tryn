#include "Step.h"
#include <Core/src/gfx/Drawable.h>

namespace tryn::gfx
{
	void Step::AddBindable(std::shared_ptr<IBindable> bindable)
	{
		bindables.push_back(std::move(bindable));
	}
	void Step::Bind(IGraphics& gfx) const
	{
		for (auto& bind : bindables)
		{
			bind->Bind();
		}
	}
	void Step::Draw(IGraphics& gfx, Drawable* parent) const
	{
		gfx.DrawIndexed(parent->GetIndexCount());
	}
}