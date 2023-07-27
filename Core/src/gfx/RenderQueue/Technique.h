#pragma once
#include <Core/src/gfx/IGraphics.h>
#include <string>
#include "Step.h"

namespace tryn::gfx
{
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
		void Draw(IGraphics& gfx)
		{
			for (auto& step : steps)
			{
				step.Bind(gfx);
				step.Draw(gfx);
			}
		}
		void BindParent(Drawable& parent);
	private:
		std::string name;
		std::vector<Step> steps;
	};

	inline void Technique::BindParent(Drawable& parent)
	{
		for (auto& step : steps)
		{
			step.parent = &parent;
		}
	}
}
