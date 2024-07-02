#pragma once
#include <Core/src/gfx/ConstantBuffer.h>
#include <span>

namespace tryn::gfx
{
	class Drawable;
	class Mesh;

	class Step
	{
	public:
		Step(std::string renderQueueID);
		void AddBindable(std::shared_ptr<class IBindable> bindable);
		void Bind() const;
		void Bind(class IContext& context) const;
		void Draw(class IGraphics& gfx, Drawable* parent) const;
		void Submit(class IGraphics& gfx, Drawable* parent);
		void Submit(class IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, class InstancedModelParent& instanceParent);
		void Accept(class TechniqueProbe& probe);
		std::vector<std::shared_ptr<gfx::IBindable>> bindables;
	private:
		std::string renderQueueID;
	};
}