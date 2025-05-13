#include "TrynPCH.h"
#include "Technique.h"
#include <Core/src/gfx/Material.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include <Core/src/gfx/IGraphics.h>

#include "Core/src/gfx/Bindables/IBufferBase.h"

namespace tryn::gfx
{
	auto TechniquePool::ConstructTechnique(const IGraphics& gfx, utl::UUID_t techniqueUuid,
	                                       const std::vector<std::shared_ptr<Material>>& materials,
	                                       const Microsoft::glTF::MeshPrimitive& primitive,
	                                       const gfx::WinGLTFLoaderContext& context, const bool instanced,
	                                       const bool skeleton) -> std::shared_ptr<TechniqueBase>
	{
		auto it = Get().techniqueMap.find(techniqueUuid);
		trynass(it != Get().techniqueMap.end()).msg(utl::ToWide(std::format("Technique with UUID: [{}] is not registered", techniqueUuid))).lvl(log::Level::Error).ex();
		auto tech = it->second->ConstructDerived(materials, gfx, instanced, skeleton);
		tech->CreateVertexBuffer(gfx, primitive, context, nullptr);
		return tech;
	}

	auto TechniquePool::ConstructTechnique(const IGraphics& gfx, utl::UUID_t techniqueUuid,
	                                       const std::vector<std::shared_ptr<Material>>& materials, const aiMesh& mesh,
	                                       ani::Skeleton* skeleton,
	                                       const bool instanced) -> std::shared_ptr<TechniqueBase>
	{
		auto it = Get().techniqueMap.find(techniqueUuid);
		trynass(it != Get().techniqueMap.end()).msg(utl::ToWide(std::format("Technique with UUID: [{}] is not registered", techniqueUuid))).lvl(log::Level::Error).ex();
		auto tech = it->second->ConstructDerived(materials, gfx, instanced, skeleton != nullptr);
		tech->CreateVertexBuffer(gfx, mesh, skeleton);
		return tech;
	}

	const std::string& TechniquePool::Name(utl::UUID_t techniqueUuid)
	{
		auto it = Get().techniqueMap.find(techniqueUuid);
		trynass(it != Get().techniqueMap.end()).msg(utl::ToWide(std::format("Technique with UUID: [{}] is not registered", techniqueUuid))).lvl(log::Level::Error).ex();
		return it->second->name;
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
	void TechniqueBase::Draw(const IGraphics& gfx, const Drawable* parent) const
	{
		for (auto& step : steps)
		{
			step.Bind();
			step.Draw(gfx, parent);
		}
	}

	void TechniqueBase::OfferBindable(const std::string& identifier, const std::shared_ptr<IBindable>& pBindable)
	{
		for (auto& step : steps)
		{
			step.OfferBindable(identifier, pBindable);
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

	void TechniqueBase::CreateVertexBuffer(const IGraphics& gfx,
		const Microsoft::glTF::MeshPrimitive& primitive, const gfx::WinGLTFLoaderContext& context,
		ani::Skeleton* pSkeleton)
	{
		auto pCPUBuffer = std::make_shared<VertexBuffer>(*pVertexLayout, primitive, context);
		pVertexBuffer = IVertexBuffer::Resolve(gfx, pCPUBuffer);
	}

	void TechniqueBase::CreateVertexBuffer(const IGraphics& gfx, const aiMesh& mesh, ani::Skeleton* pSkeleton)
	{
		auto pCPUBuffer = std::make_shared<VertexBuffer>(*pVertexLayout, mesh, pSkeleton);
		pVertexBuffer = IVertexBuffer::Resolve(gfx, pCPUBuffer);
	}

	VertexLayout TechniqueBase::GetVertexLayout() const
	{
		return *pVertexLayout;
	}
}
