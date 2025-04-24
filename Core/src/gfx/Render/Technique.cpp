#include "TrynPCH.h"
#include "Technique.h"
#include <Core/src/gfx/Material.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include <Core/src/gfx/IGraphics.h>

namespace tryn::gfx
{
	std::shared_ptr<TechniqueBase> TechniquePool::ConstructTechnique(utl::UUID_t techniqueUUID, const Material& material, const IGraphics& gfx, const std::string& path, const bool instanced,
		const bool skeleton)
	{
		auto it = Get().techniqueMap.find(techniqueUUID);
		trynass(it != Get().techniqueMap.end()).msg(utl::ToWide(std::format("Did not find technique with UUID: {}", techniqueUUID))).lvl(log::Level::Error).ex();
		return it->second->ConstructDerived(material, gfx, path, instanced, skeleton);
	}

	TechniquePool& TechniquePool::Get()
	{
		static TechniquePool singleton;
		return singleton;
	}

	TechniqueBase::TechniqueBase(const std::string& name)
	{
		this->name = name;
		pVertexLayout = std::make_unique<VertexLayout>();
	}
	void TechniqueBase::AddStep(Step step)
	{
		steps.push_back(std::move(step));
	}
	void TechniqueBase::Draw(const IGraphics& gfx, Drawable* parent) const
	{
		for (auto& step : steps)
		{
			step.Bind();
			step.Draw(gfx, parent);
		}
	}
	void TechniqueBase::Submit(const IGraphics& gfx, Drawable* parent)
	{
		for (auto& step : steps)
		{
			step.Submit(gfx, parent);
		}
	}
	void TechniqueBase::Submit(const IGraphics& gfx, Drawable* parent, const std::span<const glm::mat4> transforms, InstancedModelParent& instancedParent)
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

	VertexLayout TechniqueBase::GetVertexLayout() const
	{
		return *pVertexLayout;
	}
}
