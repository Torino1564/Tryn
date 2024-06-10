#pragma once
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/ConstantBuffer.h>

namespace tryn::gfx
{
	class Drawable;
	class Mesh;

	class Step
	{
	public:
		Step(std::string renderQueueID);
		void AddBindable(std::shared_ptr<IBindable> bindable);
		void Bind() const;
		void Bind(IContext& context) const;
		void Draw(IGraphics& gfx, Drawable* parent) const;
		void Submit(IGraphics& gfx, Drawable* parent);
		void Submit(IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, class InstancedModelParent& instanceParent);
		void Accept(class TechniqueProbe& probe);
		std::vector<std::shared_ptr<gfx::IBindable>> bindables;
	private:
		std::string renderQueueID;
	};
}