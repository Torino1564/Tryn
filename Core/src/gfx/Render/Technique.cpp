#include "Technique.h"
#include <Core/src/gfx/Material.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include <ranges>
#include <Core/src/utl/String.h>

namespace tryn::gfx
{
	Technique::Technique(std::string name)
	{
		this->name = name;
	}
	void Technique::AddStep(Step step)
	{
		steps.push_back(std::move(step));
		stepActive.push_back(true);
	}
	void Technique::Draw(IGraphics& gfx, Drawable* parent)
	{
		for (auto [index, step] :std::ranges::enumerate_view(steps))
		{
			if (stepActive[index] == true)
			{
				step.Bind();
				step.Draw(gfx, parent);
			}
		}
	}
	void Technique::Submit(IGraphics& gfx, Drawable* parent)
	{
		for (auto [index, step] : std::ranges::enumerate_view(steps))
		{
			if (stepActive[index] == true)
			{
				step.Submit(gfx, parent);
			}
		}
	}
	void Technique::Submit(IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, InstancedModelParent& instancedParent)
	{
		for (auto [index, step] : std::ranges::enumerate_view(steps))
		{
			if (stepActive[index] == true)
			{
				step.Submit(gfx, parent, transforms, instancedParent);
			}
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
	void Technique::SetStepState(const std::string& name, bool state)
	{
		for (auto [index, step] : std::ranges::enumerate_view(steps))
		{
			if (step.GetName() == name)
			{
				stepActive[index] = state;
				return;
			}
		}
		trylog.warn(utl::ToWide(std::format("Could not find step [{}] when setting its state in techinque [{}]", name, this->name)));
	}
	bool Technique::GetStepState(const std::string& name) const
	{
		for (auto [index, step] : std::ranges::enumerate_view(steps))
		{
			if (step.GetName() == name)
			{
				return stepActive[index];
			}
		}
		trylog.warn(utl::ToWide(std::format("Could not find step [{}] when getting state information of techinque [{}]", name, this->name)));
		return false;
	}
	bool Technique::IsSkinned() const
	{
		return skinned;
	}
	bool Technique::IsInstanced() const
	{
		return instanced;
	}
	const std::string& Technique::GetName() const
	{
		return name;
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
