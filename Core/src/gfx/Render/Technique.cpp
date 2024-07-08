#include "Technique.h"
#include <Core/src/gfx/Material.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include <Core/src/gfx/IGraphics.h>

namespace tryn::gfx
{
	Technique::Technique(std::string name)
	{
		this->name = name;
	}
	void Technique::AddStep(Step step)
	{
		steps.push_back(std::move(step));
	}
	void Technique::Draw(IGraphics& gfx, Drawable* parent)
	{
		for (auto& step : steps)
		{
			step.Bind();
			step.Draw(gfx, parent);
		}
	}
	void Technique::Submit(IGraphics& gfx, Drawable* parent)
	{
		for (auto& step : steps)
		{
			step.Submit(gfx, parent);
		}
	}
	void Technique::Submit(IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, InstancedModelParent& instancedParent)
	{
		for (auto& step : steps)
		{
			step.Submit(gfx, parent, transforms, instancedParent);
		}
	}
	void Technique::Accept(TechniqueProbe& probe)
	{
		probe.SetTechnique(this);
		for (auto& step : steps)
		{
			step.Accept(probe);
		}
	}
	VertexLayout& Technique::ExtractLayoutFromMaterial(Material& mat)
    {
		return mat.vLayout;
    }
}
