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
	template <int>
	struct empty_t {};
	class ComponentManager
	{
	public:
		using Components_Ty =
			std::tuple<
#define X(el) \
		std::vector<typename ComponentEnumMap<ComponentType::el>::ComponentType::SubresourceData>, \

		COMPONENT_TYPES
#undef X
		empty_t<0>>;

		template <ComponentWithUID T>
		auto& GetComponentByID(int entityID, std::optional<std::string_view> name = std::nullopt)
		{
			auto& componentVector = std::get<std::vector<typename T::SubresourceData>>(memory);
			trynass_msg(entityID <= componentVector.size(),L"Out of bounds access of the component vector");
			return componentVector[entityID];
		}
		template <ComponentWithUID T>
		T::SubresourceData& AddComponent(int entityID, T& component)
		{
			using SubResourceData_t = typename T::SubresourceData;
			auto& componentVector = std::get<std::vector<SubResourceData_t>>(memory);
			if (componentVector.size() <= entityID)
			{
				ResizeArrays((int)(float((entityID) + 1) * float(1.3)));
			}
			auto& componentVector2 = std::get<std::vector<SubResourceData_t>>(memory);
			component.SetEntityID(entityID);
			return componentVector2[entityID];
		}
		template <ComponentType T>
		std::span<typename ComponentEnumMap<T>::ComponentType::SubresourceData> GetComponentData()
		{
			using Type = typename ComponentEnumMap<T>::ComponentType::SubresourceData;
			auto& vector = std::get<T>(memory);
			return std::span<Type>(vector.begin(), vector.size());
		}

		template<std::size_t Index = 0>
		void ExecuteComponents()
		{
			if constexpr (Index < std::tuple_size_v<Components_Ty> -1)
			{
				ExecuteComponent_<typename ComponentEnumMap<static_cast<ComponentType>(Index)>::ComponentType>();
				ExecuteComponents<Index + 1>();
			}
		}

		template <std::size_t Index = 0>
		void ResizeArrays(int newSize)
		{
			if constexpr (Index < std::tuple_size_v<Components_Ty> - 1)
			{
				ResizeArray_(std::get<Index>(memory), newSize);
				ResizeArrays<Index + 1>(newSize);
			}
		}
		void ComponentTree() {}

		static ComponentManager& Get()
		{
			static ComponentManager singleton;
			return singleton;
		}
	private:
		ComponentManager();
		template <typename T>
		void ResizeArray_(std::vector<T>& componentVector, int newSize)
		{
			componentVector.resize(newSize);
		}
		template <ComponentWithUID C>
		void ExecuteComponent_()
		{
			auto& vector = std::get<static_cast<int>(ReverseComponentMap<C>::type)>(memory);
			C::Execute(std::span<typename C::SubresourceData>(vector.begin(), vector.size()));
		}

		// Data
		Components_Ty memory;
	};
}
