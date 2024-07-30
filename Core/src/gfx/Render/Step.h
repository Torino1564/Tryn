#pragma once
#include <Core/src/gfx/ConstantBuffer.h>
#include <span>

namespace tryn::gfx
{
	class Drawable;
	class Mesh;
	class IGraphics;
	class IContext;

	class Step
	{
	public:
		Step(std::string renderQueueID);
		void AddBindable(std::shared_ptr<class IBindable> bindable);
		void Bind() const;
		void Bind(const IContext& context) const;
		void Draw(const IGraphics& gfx, Drawable* parent) const;
		void Submit(const IGraphics& gfx, Drawable* parent);
		void Submit(const IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, class InstancedModelParent& instanceParent);
		void Accept(class TechniqueProbe& probe);
		std::vector<std::shared_ptr<gfx::IBindable>> bindables;
	private:
		std::string renderQueueID;
	};
}