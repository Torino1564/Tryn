#include "TrynPCH.h"
#include "Utils.h"
#include <Core/src/utl/StringHasher.h>
#include "Entity.h"
#include "cmp/ModelComponent.h"
#include "cmp/UpdateJITBufferComponent.h"

namespace tryn::ecs
{
	void AddEntityIDJITBuffer(Entity& entity, ECS& ecs)
	{
		const auto& pModel = entity.GetComponent<ModelComponent>().pModel;

		if (!entity.HasComponent<UpdateJITBufferComponent>())
		{
			// Add component if it doesnt already have it:
			entity.AddComponent<UpdateJITBufferComponent>(ecs);
		}
		auto& jitBufferComponent = entity.GetComponent<UpdateJITBufferComponent>();

		gfx::ConstantBufferLayout cblayout;
		cblayout.Append(gfx::ConstantBufferLayout::Type::Int32, "entityID");
		cblayout.Solidify();

		auto buffer = gfx::IPxConstantBuffer::Resolve(ecs.Gfx(), std::move(cblayout));

		auto pJITBuffer = std::make_shared<gfx::JITUpdateBuffer>(gfx::JITUpdateBuffer::Make(buffer));

		pModel->AddPerTechniqueBindable(pJITBuffer, "entityIDBuffer");

		auto pFun = [](const std::shared_ptr<gfx::JITUpdateBuffer>& pBuffer, uint32_t entityID, const ecs::Archetype* pArchetype)
			{
				uint32_t* pEntityID = &pArchetype->Manager().GetArenaAllocator().Emplace(entityID);

				pBuffer->Set(pEntityID, sizeof(decltype(entityID)));
			};
		jitBufferComponent.jitCombinations.emplace_back(pJITBuffer, pFun);
	}
}
