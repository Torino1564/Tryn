
#include "Drawable.h"
#include <Core/src/gfx/Bindables/PrimitiveTopology.h>
#include <Core/src/gfx/Material.h>
#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <Core/src/gfx/Bindables/IBufferBase.h>
#include <Core/src/gfx/Model/InstancedModel.h>
#include <Core/src/gfx/Bindables/SOAVertexBuffer.h>
#include <Core/src/gfx/IGraphics.h>

namespace tryn::gfx
{
	template<typename T>
	T wrap_angle(T theta) noexcept
	{
		constexpr T twoPi = T(2) * T(glm::pi<double>());
		const T mod = T(fmod(theta, twoPi));
		if (mod > T(glm::pi<double>()))
		{
			return mod - twoPi;
		}
		else if (mod < -static_cast<T>(glm::pi<double>()))
		{
			return mod + twoPi;
		}
		return mod;
	}

	Drawable::~Drawable() = default;

	void Drawable::Draw(const IGraphics& gfx, const glm::mat4& transform)
	{
		this->transform = transform;

		gfx.Dispatch([&] {
			BindBase();
			for (const auto& [enabled, technique] : pTechniques)
			{
				if (enabled)
					technique->Draw(gfx, this);
			}
			});
	}
	void Drawable::Submit(const IGraphics& gfx, const glm::mat4& transform)
	{
		extraBindPtrs = {};
		ExtraSubmitBehavior();
		this->transform = transform;

		for (const auto& [enabled, technique]: pTechniques)
		{
			if (enabled)
				technique->Submit(gfx, this);
		}
	}
	void Drawable::Submit(const IGraphics& gfx, const std::span<const glm::mat4> transforms, InstancedModelParent& instancedParent)
	{
		extraBindPtrs = {};
		ExtraSubmitBehavior();

		auto& instanceBuffer = instancedParent.RequestInstanceBuffer(ID);
		auto& constantBuffer = instanceBuffer.GetConstantBuffer();
		auto instanceArray = constantBuffer["InstanceArray"];

		if (instanceArray.Node().Size() < transforms.size())
		{
			instanceArray.Resize(transforms.size() + 10);
		}

		auto updatedInstanceArray = constantBuffer["InstanceArray"];

		memset(constantBuffer.Data(), 0, constantBuffer.ByteSize());

		for (auto i = 0; i < transforms.size(); i++)
		{
			updatedInstanceArray[i]["transform"].Get<glm::mat4>() = transpose(transforms[i]);
		}

		AddExtraBind(&instanceBuffer);

		for (auto& [enabled, technique] : pTechniques | std::views::filter([](const auto& pair){ return pair.first; }))
		{
			technique->Submit(gfx, this, transforms, instancedParent);
		}
	}
	void Drawable::BindBase() const
	{
		pIndexBuffer->Bind();
		pTopology->Bind();
	}
	void Drawable::BindBase(const IContext& context) const
	{
		pIndexBuffer->Bind(context);
		pTopology->Bind(context);
	}
	void Drawable::BindExtraBinds()
	{
		for (auto bindPtr : extraBindPtrs)
		{
			if (bindPtr != nullptr)
			{
				bindPtr->Bind();
			}
		}
	}
	void Drawable::BindExtraBinds(const IContext& context)
	{
		for (auto bindPtr : extraBindPtrs)
		{
			if (bindPtr != nullptr)
			{
				bindPtr->Bind(context);
			}
		}
	}
	void Drawable::AddExtraBind(IBindable* pBindable)
	{
		for (auto& bindPtr : extraBindPtrs)
		{
			if (bindPtr == nullptr)
			{
				bindPtr = pBindable;
				return;
			}
		}
		trylog.warn(L"Failed to add extra bind: the 10 slot limit was reached.");
	}

	void Drawable::BindTransformCBuf() const
	{
		pTransformCBuf->BindTransformCBuf(this);
	}
	void Drawable::BindTransformCBuf(const IContext& context) const
	{
		pTransformCBuf->BindTransformCBuf(this, context);
	}
	void Drawable::InitTransformCBuf(const IGraphics& gfx)
	{
		pTransformCBuf = ITransformCBuf::Resolve(gfx);
	}
	ISOAVertexBuffer& Drawable::GetVertexBuffer() const
	{
		return *pSOAVertexBuffer;
	}
	uint32_t Drawable::GetIndexCount() const
	{
		return indexCount;
	}
	glm::mat4 Drawable::GetTransformMatrix() const
	{
		return transform;
	}
	std::uint16_t Drawable::GetID() const
	{
		return ID;
	}

	void Drawable::EnableOrAddTechniqueEx(const IGraphics& gfx, const utl::UUID_t techniqueUUID, const bool skinned,
		const std::span<uint16_t> materialIndex)
	{
		if (const auto it = std::ranges::find_if(pTechniques, [=](const auto& pair) { return (pair.second->UUID() == techniqueUUID); }); it != pTechniques.end())
		{
			// Technique exists, then just enable it
			it->first = true;
		}
		else
		{
			// Technique does not exist, add it and enable it:
			AddTechniqueEx(gfx, techniqueUUID, skinned, materialIndex, true);
		}
	}

	void Drawable::DisableTechnique(const utl::UUID_t techniqueUUID)
	{
		if (const auto it = std::ranges::find_if(pTechniques, [=](const auto& pair) { return (pair.second->UUID() == techniqueUUID); }); it != pTechniques.end())
		{
			// Technique exists, then just disable it
			it->first = false;
		}
		else
		{
			trylog.warn(utl::ToWide(std::format("Attempted to disable a technique that does not exist: [{}]", TechniquePool::Name(techniqueUUID))));
		}
	}

	void Drawable::AddTechniqueEx(const IGraphics& gfx, const utl::UUID_t techniqueUUID, const bool skinned, const std::span<uint16_t> materialIndexes, const bool enabled)
	{
		std::vector<std::shared_ptr<Material>> materials;
		for (auto materialIndex : materialIndexes)
		{
			materials.emplace_back(pMaterials[materialIndex]);
		}

		auto& [_, technique] = pTechniques.emplace_back(enabled, TechniquePool::ConstructTechnique(gfx, techniqueUUID, materials, instanced, skinned));
		technique->FillSOAVertexBuffer(*pSOAVertexBuffer);
	}

	Material& Drawable::GetSelectedMaterial() const
	{
		return *pMaterials[selectedMaterial];
	}
}
