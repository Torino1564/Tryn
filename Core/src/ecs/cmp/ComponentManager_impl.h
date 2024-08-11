#pragma once
#include "ComponentManager_def.h"

namespace tryn::ecs
{
	template <typename T>
	unsigned int ComponentManager::RegisterComponent()
	{
		auto componentIndex = NextFreeAndIncrement();
		const auto success = ComponentMap().insert({T::UUID, std::move(std::make_pair(std::make_unique<T>(), componentIndex))});
		ComponentVector().push_back(T::UUID);
		return componentIndex;
	}

	template <template <typename, ValidComponent> class Func, unsigned ComponentN, bool FoundCmp, typename Component,
	          unsigned ElementN, typename ... FuncArgs, auto Tag>
	void ComponentManager::IterateComponentMembers(const utl::UUID_t componentUUID, FuncArgs&&... funcArgs)
	{
		if constexpr (!FoundCmp)
		{
			if constexpr (ComponentN >= utl::ctc::element_count<GetComponentListID()>())
			{
				return;
			}
			using CurrentComponent = std::tuple_element_t<ComponentN, utl::ctc::get_list<listID>>;
			if (CurrentComponent::UUID == componentUUID)
			{
				IterateComponentMembers<Func, ComponentN, true, CurrentComponent>(componentUUID, funcArgs...);
				return;
			}
			else
			{
				if constexpr (ComponentN < utl::ctc::element_count<listID>() - 1)
					return IterateComponentMembers<Func, ComponentN + 1, false>(componentUUID, funcArgs...);
			}
		}
		else
		{
			using VarMap = utl::CTM::get_list<Component::UUID>;
			if constexpr (ElementN < std::tuple_size_v<VarMap>)
			{
				using MapElement = std::tuple_element_t<ElementN, VarMap>;
				Func<MapElement, Component> func;
				func(funcArgs...);
				IterateComponentMembers<Func, ComponentN, true, Component, ElementN + 1>(componentUUID, funcArgs...);
			}
			else
			{
				return;
			}
		}
	}

	inline void ComponentData(std::byte* pData, const utl::UUID_t componentUUID, const Action action)
	{
		auto& map = ComponentManager::ComponentMap();
		auto& [pComponent, index] = map[componentUUID];

		switch (action)
		{
		case Action::Fill:
			pComponent->SRD().Constructor(pData);
			break;
		case Action::Delete:
			pComponent->SRD().Destructor(pData);
			break;
		}
	}

	template <ComponentInfo Info>
	typename ReturnType<Info>::T GetComponentInfo(const utl::UUID_t componentUUID)
	{
		auto& map = ComponentManager::ComponentMap();
		auto& [pComponent, index] = map[componentUUID];

		if constexpr (Info == ComponentInfo::Name)
		{
			return pComponent->Name();
		}
		else if constexpr (Info == ComponentInfo::Size)
		{
			return pComponent->Size();
		}
		else if constexpr (Info == ComponentInfo::Index)
		{
			return ComponentManager::ComponentMap()[componentUUID].second;
		}
	}
}
