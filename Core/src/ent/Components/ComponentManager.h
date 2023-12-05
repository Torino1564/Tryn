#pragma once
#include "Component.h"
#include <memory>
#include <vector>
#include <optional>
#include <concepts>
#include <Core/src/utl/Exception.h>

ZT_EX_DEF(ComponentManagerException);

namespace tryn::ent
{
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
		void Update(double dt = 0);
		void AddComponent(std::unique_ptr<Component>&& pNewComponent);
		template <ComponentWithUID T>
		auto& QueryComponent(std::optional<const char*> name = std::nullopt)
		{
			for (auto& component : componentPtrs)
			{
				if (T::GetCUID() == component->GetCUID())
				{
					if (!name || name == component->GetName())
					{
						return *reinterpret_cast<T*>(component.get());
					}
				}
			}
			return std::nullopt;
		}
		void ComponentTree();
	private:
		std::vector<std::unique_ptr<Component>> componentPtrs;
	};
}
