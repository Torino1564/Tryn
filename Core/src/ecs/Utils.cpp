
#include "Utils.h"
#include <Core/src/utl/StringHasher.h>
#include "Entity.h"
#include "cmp/ModelComponent.h"
#include "cmp/UpdateJITBufferComponent.h"
#include "Core/src/gfx/Render/Techniques/EntityIDTechnique.h"

namespace tryn::ecs
{
	void AddEntityIDJITBuffer(Entity& entity, ECS& ecs)
	{
		const auto& pModel = entity.GetComponent<ModelComponent>().pModel;

		if (!pModel->HasTechnique(ZT_TYPE_UUID(gfx::EntityIDTechnique)))
		{
			// adds the technique:
			pModel->AddOrEnableTechniques(std::array{ ZT_TYPE_UUID(gfx::EntityIDTechnique) });
		}

		if (!entity.HasComponent<UpdateJITBufferComponent>())
		{
			// Add component if it doesnt already have it:
			entity.AddComponent<UpdateJITBufferComponent>(ecs);
		}
		auto& jitBufferComponent = entity.GetComponent<UpdateJITBufferComponent>();

		gfx::ConstantBufferLayout cblayout;
		cblayout.Append(gfx::ConstantBufferLayout::Type::UInt32, "entityID");
		cblayout.Append(gfx::ConstantBufferLayout::Type::UInt32, "archetypeID");
		cblayout.Append(gfx::ConstantBufferLayout::Type::UInt32, "padding");
		cblayout.Append(gfx::ConstantBufferLayout::Type::UInt32, "empty");
		cblayout.Solidify();

		auto buffer = gfx::IPxConstantBuffer::Resolve(ecs.Gfx(), std::move(cblayout));

		auto pJITBuffer = std::make_shared<gfx::JITUpdateBuffer>(gfx::JITUpdateBuffer::Make(buffer));

		pModel->AddPerTechniqueBindable(pJITBuffer, "entityIDBuffer");

		auto pFun = [](const std::shared_ptr<gfx::JITUpdateBuffer>& pBuffer, uint32_t entityID, const ecs::Archetype* pArchetype)
			{
				const auto& data = pArchetype->Manager().GetArenaAllocator().Emplace(std::tuple{ entityID, pArchetype->GetUUID(), 0u, 0u });
				pBuffer->Set(&data, sizeof(decltype(data)));
			};
		jitBufferComponent.jitCombinations.emplace_back(pJITBuffer, pFun);
	}
}
