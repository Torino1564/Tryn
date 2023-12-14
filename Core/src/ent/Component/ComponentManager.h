#pragma once
#include "Component.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <Core/src/utl/Assert.h>

namespace tryn::ent
{
	class ComponentManager
	{
	public:
		static ComponentManager& Get()
		{
			static ComponentManager singleton;
			return singleton;
		}

		template <Component C> 
		void RegisterComponent()
		{
			map.insert({ C::UUID,{ sizeof(C::SubresourceData), componentCounter++}});
			bufferPtrs[componentCounter]->resize(10 * sizeof(C::SubresourceData));
		}
		template <Component C>
		C::SubresourceData& AddComponent(std::uint16_t entityID)
		{
			auto componentDataIt = map.find(C::UUID);
			trynass_msg(componentDataIt != map.end(), L"Unknown Component Type!");
			auto& componentData = *componentDataIt;
			std::uint16_t totalOffset = componentData.first * entityID;
			auto& buffer = *(bufferPtrs[componentData.second]);

			std::uint16_t bufferSize = buffer.size();
			
			if (bufferSize < totalOffset + componentData.first)
			{
				while (bufferSize < totalOffset + componentData.first)
				{
					bufferSize = (bufferSize + 5) * 1.3f;
				}
				buffer.resize(bufferSize);
			}

			return reinterpret_cast<C::SubresourceData&>(buffer[totalOffset]);
		}
		template <Component C>
		auto& GetComponent(std::uint16_t entityID)
		{
			const auto& componentData = map[C::UUID];
			std::uint16_t totalOffset = componentData.first * entityID;
			auto& buffer = *(bufferPtrs[componentData.second]);
			trynass_msg(buffer.size() > totalOffset, L"Out of bounds access in the ECS");
			return reinterpret_cast<C::SubresourceData&>(buffer[totalOffset]);
		}
	private:
		ComponentManager() = default;
		std::uint16_t componentCounter = 0;
		std::unordered_map<std::string, std::pair<std::size_t, int>> map;
		std::vector<std::unique_ptr<std::vector<std::byte>>> bufferPtrs;
	};
}