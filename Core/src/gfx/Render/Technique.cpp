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
	const std::string& Technique::GetShaderRootPath() const
	{
		static bool isInitialized = false;
		static std::string shaderRootPath;
		if (!isInitialized)
		{
			shaderRootPath += __FILE__;
			size_t trynPos = shaderRootPath.rfind("Tryn");
			if (trynPos != std::string::npos)
			{
				shaderRootPath.erase(trynPos + 4);
			}

			shaderRootPath += "\\bin\\Shaders\\";
			isInitialized = true;
		}

		return shaderRootPath;
	}
}
