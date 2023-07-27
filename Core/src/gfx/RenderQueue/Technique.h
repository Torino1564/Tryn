#pragma once
#include <memory>
#include <Core/src/gfx/IGraphics.h>
#include <string>
#include <Core/src/gfx/Bindables/VertexShader.h>
#include <Core/src/gfx/Bindables/PixelShader.h>
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
		void Draw()
		{
			for (auto& step : steps)
			{

			}
		}
	private:
		std::string name;
		std::vector<Step> steps;
	};
}