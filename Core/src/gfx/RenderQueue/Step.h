#pragma once
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/Bindables/ConstantBuffer.h>

namespace tryn::gfx
{
	class Drawable;

	class Step
	{
	public:
		Step(std::string renderQueueID);
		void AddBindable(std::shared_ptr<IBindable> bindable);
		void Bind(IGraphics& gfx) const;
		void Bind(IGraphics& gfx, IContext& context) const;
		void Draw(IGraphics& gfx, Drawable* parent) const;
		void Submit(IGraphics& gfx, Drawable* parent);
		std::vector<std::shared_ptr<gfx::IBindable>> bindables;
	private:
		std::string renderQueueID;
	};
}