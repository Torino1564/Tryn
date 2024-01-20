#pragma once
#include <Core/src/gfx/IGraphics.h>
#include <string>
#include "TechniqueProbe.h"
#include "Step.h"

namespace tryn::gfx
{
	enum class Techniques
	{
		Phong,
		Flat,
		Garaoud,
	};

	class Technique
	{
	public:
		Technique(std::string name)
		{
			this->name = name;
		}
		void AddStep(Step step)
		{
			steps.push_back(std::move(step));
		}
		void Draw(IGraphics& gfx, Drawable* parent)
		{
			for (auto& step : steps)
			{
				step.Bind(gfx);
				step.Draw(gfx, parent);
			}
		}
		void Submit(IGraphics& gfx, Drawable* parent)
		{
			for (auto& step : steps)
			{
				step.Submit(gfx, parent);
			}
		}
		void Submit(IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, InstancedModelParent& instancedParent)
		{
			for (auto& step : steps)
			{
				step.Submit(gfx, parent, transforms, instancedParent);
			}
		}
		void Accept(TechniqueProbe& probe)
		{
			probe.SetTechnique(this);
			for (auto& step : steps)
			{
				step.Accept(probe);
			}
		}

	private:
		std::string name;
		std::vector<Step> steps;
	};
}
