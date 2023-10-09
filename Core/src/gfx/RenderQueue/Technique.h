#pragma once
#include <Core/src/gfx/IGraphics.h>
#include <string>
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
	private:
		std::string name;
		std::vector<Step> steps;
	};
}
