#pragma once
#include "Component.h"
#include <memory>
#include <vector>
#include <optional>
#include <concepts>
#include <Core/src/utl/Exception.h>
#include <tuple>
#include <Core/src/utl/Assert.h>
#include "ComponentCodex.h"

ZT_EX_DEF(ComponentManagerException);

namespace tryn::ent
{
	class ComponentManager
	{
	public:
		using Components_Ty = std::tuple<std::vector<ModelComponent>>;
		template <ComponentWithUID T>
		auto& GetComponentByID(int entityID, std::optional<std::string_view> name = std::nullopt)
		{
			auto& componentVector = std::get<std::vector<T>>(memory);
			trynass_msg(entityID <= componentVector.size(),L"Out of bounds access of the component vector");
			return componentVector[entityID];
		}
		template <ComponentWithUID T>
		void AddComponent(int entityID, T component)
		{
			auto& componentVector = std::get<std::vector<T>>(memory);
			if (componentVector.size() < entityID)
			{
				ResizeArrays((int)(float(entityID) * float(1.3)));
			}
			auto& componentVector2 = std::get<std::vector<T>>(memory);
			componentVector2[entityID] = std::move(component);
		}
		template <std::size_t Index = 0>
		void ResizeArrays(int newSize)
		{
			if constexpr (Index < std::tuple_size_v<Components_Ty>)
			{
				ResizeArray_(std::get<Index>(memory), newSize);
				ResizeArrays<Index + 1>(newSize);
			}
		}
		void ComponentTree() {}

		static ComponentManager& Get()
		{
			ComponentManager singleton;
			return singleton;
		}
	private:
		template <ComponentWithUID T>
		void ResizeArray_(std::vector<T>& componentVector, int newSize)
		{
			componentVector.resize(newSize);
		}
		ComponentManager() = default;
		// Data
		Components_Ty memory;
	};
}
