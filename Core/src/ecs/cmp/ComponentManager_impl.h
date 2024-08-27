#pragma once
#include "ComponentManager_def.h"
#include <Core/src/ecs/SerializeLambda.h>
#include <Core/src/ecs/cmp/PrintMemberBase.h>

namespace tryn::ecs
{
	template <typename T>
	unsigned int ComponentManager::RegisterComponent()
	{
		auto componentIndex = NextFreeAndIncrement();
		T test = {};
		const auto [it, success] = ComponentMap().insert({T::UUID, std::move(std::make_pair(std::make_unique<T>(), componentIndex))});
		ComponentVector().push_back(T::UUID);
		ComponentViewVector().push_back(std::move(ComponentView<
			CallbackSignature<VerySimpleCallback>,
			CallbackSignature<SerializeWriteComponentField, const ser::StreamWriter&, const EntityID, const bool, const std::string&>,
			CallbackSignature<SerializeReadComponentField, const ser::StreamReader&, const EntityID, const bool, const ser::ExtraDataPack*>,
			CallbackSignature<cmp::PrintImGuiMemberVariable, EntityID>
		>::Make<T>(*static_cast<T*>(&*it->second.first))  ));
		return componentIndex;
	}

	inline auto& ComponentManager::ComponentViewVector()
	{
		static std::vector<ComponentView<
			CallbackSignature<VerySimpleCallback>,
			CallbackSignature<SerializeWriteComponentField, const ser::StreamWriter&, const EntityID, const bool, const std::string&>,
			CallbackSignature<SerializeReadComponentField, const ser::StreamReader&, const EntityID, const bool, const ser::ExtraDataPack*>,
			CallbackSignature<cmp::PrintImGuiMemberVariable, EntityID>
		>> componentViewVector;
		return componentViewVector;
	}

	//template <template <typename, ValidComponent> class Func, unsigned ComponentN, bool FoundCmp, typename Component,
	//          unsigned ElementN, typename ... FuncArgs, auto Tag>
	//void ComponentManager::IterateComponentMembers(const utl::UUID_t componentUUID, FuncArgs&&... funcArgs)
	//{
	//	if constexpr (!FoundCmp)
	//	{
	//		if constexpr (ComponentN >= utl::ctc::element_count<GetComponentListID()>())
	//		{
	//			return;
	//		}
	//		using CurrentComponent = std::tuple_element_t<ComponentN, utl::ctc::get_list<listID>>;
	//		if (CurrentComponent::UUID == componentUUID)
	//		{
	//			IterateComponentMembers<Func, ComponentN, true, CurrentComponent>(componentUUID, funcArgs...);
	//			return;
	//		}
	//		else
	//		{
	//			if constexpr (ComponentN < utl::ctc::element_count<listID>() - 1)
	//				return IterateComponentMembers<Func, ComponentN + 1, false>(componentUUID, funcArgs...);
	//		}
	//	}
	//	else
	//	{
	//		using VarMap = utl::CTM::get_list<Component::UUID>;
	//		if constexpr (ElementN < std::tuple_size_v<VarMap>)
	//		{
	//			using MapElement = std::tuple_element_t<ElementN, VarMap>;
	//			Func<MapElement, Component> func;
	//			func(funcArgs...);
	//			IterateComponentMembers<Func, ComponentN, true, Component, ElementN + 1>(componentUUID, funcArgs...);
	//		}
	//		else
	//		{
	//			return;
	//		}
	//	}
	//}

	template <typename T>
	template <template <typename, ValidComponent> class Func, unsigned ElementN, auto Tag, typename ... FuncArgs>
	void Component<T>::IterateMembers(FuncArgs&&... funcArgs)
	{
		using VarMap_ = utl::CTM::get_list<UUID>;
		VarMap_ testVarMap;
		if constexpr (ElementN < std::tuple_size_v<VarMap_>)
		{
			using MapElement = std::tuple_element_t<ElementN, VarMap_>;
			Func<MapElement, T> func;
			func(funcArgs...);
			IterateMembers<Func, ElementN + 1>(funcArgs...);
		}
	}
}
