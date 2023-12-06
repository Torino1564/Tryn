#pragma once
#include "Component.h"
#include <memory>
#include <vector>
#include <optional>
#include <concepts>
#include <Core/src/utl/Exception.h>
#include <tuple>
#include <Core/src/utl/Assert.h>

ZT_EX_DEF(ComponentManagerException);

namespace tryn::ent
{
	class ModelComponent;
	class PhysicsComponent;

	template <typename T>
	concept ImplementsGetUID = requires (T t)
	{
		{t.GetCUID() } -> std::convertible_to<ComponentType>;
	};
	template <typename T>
	concept IsComponentType = std::is_base_of<Component, T>::value;

	template <typename T>
	concept ComponentWithUID = ImplementsGetUID<T> && IsComponentType<T>;

	class ComponentManager
	{
	public:
		template <ComponentWithUID T>
		auto& GetComponentByID(int entityID, std::optional<std::string_view> name = std::nullopt)
		{
			auto& componentVector = std::get<std::vector<T>>(memory);
			trynass_msg(entityID <= componentVector.size(),L"Out of bounds access of the component vector");
			return componentVector[entityID];
		}
		void ComponentTree();
		static ComponentManager& Get()
		{
			ComponentManager singleton;
			return singleton;
		}
	private:
		ComponentManager() = default;
		// Data
		std::tuple<
			std::vector<ModelComponent>,
			std::vector<PhysicsComponent>,
		> memory;
	};
}
