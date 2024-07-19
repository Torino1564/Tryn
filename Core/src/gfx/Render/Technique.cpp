#include "Technique.h"
#include <Core/src/gfx/Material.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include <Core/src/gfx/IGraphics.h>

namespace tryn::gfx
{
	bool TechniquePool::RegisterTechnique(utl::UUID_t UUID, std::unique_ptr<TechniqueBase>&& pTechnique)
	{
		auto [iterator, result] = techniqueMap.insert({UUID, std::forward<std::unique_ptr<TechniqueBase>>(pTechnique)});

		return result;
	}

	std::shared_ptr<TechniqueBase> TechniquePool::ConstructTechnique(utl::UUID_t techniqueUUID, Material& material,
		aiMaterial& aiMaterial, IGraphics& gfx, const std::string& path, const bool instanced, const bool skeleton) const
	{
		auto it = techniqueMap.find(techniqueUUID);
		trynass(it != techniqueMap.end()).msg(utl::ToWide(std::format("Did not find technique with UUID: {}", techniqueUUID))).lvl(log::Level::Error).ex();
		return it->second->ConstructDerived(material, aiMaterial, gfx, path, instanced, skeleton);
	}

	TechniqueBase::TechniqueBase(const std::string& name)
	{
		this->name = name;
	}
	void TechniqueBase::AddStep(Step step)
	{
		steps.push_back(std::move(step));
	}
	void TechniqueBase::Draw(IGraphics& gfx, Drawable* parent)
	{
		for (auto& step : steps)
		{
			step.Bind();
			step.Draw(gfx, parent);
		}
	}
	void TechniqueBase::Submit(IGraphics& gfx, Drawable* parent)
	{
		for (auto& step : steps)
		{
			step.Submit(gfx, parent);
		}
	}
	void TechniqueBase::Submit(IGraphics& gfx, Drawable* parent, std::span<const glm::mat4> transforms, InstancedModelParent& instancedParent)
	{
		for (auto& step : steps)
		{
			step.Submit(gfx, parent, transforms, instancedParent);
		}
	}
	void TechniqueBase::Accept(TechniqueProbe& probe)
	{
		probe.SetTechnique(this);
		for (auto& step : steps)
		{
			step.Accept(probe);
		}
	}
	VertexLayout& TechniqueBase::ExtractLayoutFromMaterial(Material& mat)
    {
		return mat.vLayout;
    }
}
