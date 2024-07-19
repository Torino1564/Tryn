#pragma once
#include "ComponentManager.h"

namespace tryn::ecs
{
	template <typename C>
	int ComponentManager::RegisterComponent()
	{
		//map.insert({ componentCounter, sizeof(C::SubresourceData)});
		return componentCounter++;
	}

	template <unsigned int N, bool Index>
	const char* ComponentManager::GetComponentName(unsigned int componentUUID)
	{
		static constexpr const char* defaultName = "Default Component Name";

		if constexpr (N < GetComponentCount())
		{
			using CurrentComponent = typename std::tuple_element_t<N, ComponentList<>>;
			if (componentUUID == (Index ? CurrentComponent::index : CurrentComponent::UUID))
			{
				return CurrentComponent::name;
			}
			else
			{
				return GetComponentName<N + 1>(componentUUID);
			}
		}
		return defaultName;
	}

	template <unsigned int N, bool Index>
	std::size_t ComponentManager::GetComponentSize(unsigned int componentUUID)
	{
		if constexpr (N < GetComponentCount())
		{
			using CurrentComponent = typename std::tuple_element_t<N, ComponentList<>>;
			if (componentUUID == (Index ? CurrentComponent::index : CurrentComponent::UUID))
			{
				return sizeof(typename CurrentComponent::SubresourceData);
			}
			else
			{
				return GetComponentSize<N + 1>(componentUUID);
			}
		}
		return 0u;
	}

	template <unsigned int N>
	void ComponentManager::RegisterComponents()
	{
		if constexpr (N >= GetComponentCount())
		{
			return;
		}
		if constexpr (N < GetComponentCount())
		{
			RegisterComponent<std::tuple_element_t<N, ComponentList<>>>();
			RegisterComponents<N + 1>();
		}
	}

	template <template <typename, ValidComponent> class Func, unsigned ComponentN, bool FoundCmp, typename Component,
		unsigned ElementN, typename ... FuncArgs>
	void ComponentManager::IterateComponentMembers(const utl::UUID_t componentUUID, FuncArgs&&... funcArgs)
	{
		if constexpr (!FoundCmp)
		{
			if constexpr (ComponentN >= utl::ctc::element_count<ComponentManager::GetComponentListID()>())
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

	template <unsigned N, auto Tag>
	constexpr unsigned int GetComponentIndex(const utl::UUID_t componentUUID)
	{
		if constexpr (N < ComponentManager::GetComponentCount())
		{
			using CurrentComponent = typename std::tuple_element_t<N, ComponentManager::ComponentList<>>;
			if (componentUUID == CurrentComponent::UUID)
			{
				return CurrentComponent::index;
			}
			else
			{
				return GetComponentIndex<N + 1>(componentUUID);
			}
		}
		return 0u;
	}

	template <typename T>
	void FillData(std::byte* pData)
	{
		auto pDataCasted = reinterpret_cast<typename T::SubresourceData*>(pData);
		if constexpr (std::is_trivially_copyable_v<typename T::SubresourceData>)
		{
			new(pDataCasted) typename T::SubresourceData {};
		}
		else
		{
			std::memset(pData, 0u, sizeof(typename T::SubresourceData));
		}
	}

	template <typename T>
	void DeleteData(std::byte* pData)
	{
		auto pDataCasted = reinterpret_cast<typename T::SubresourceData*>(pData);
		pDataCasted->~SubresourceData();

		std::memset(pDataCasted, 0u, sizeof(typename T::SubresourceData));
	}

	template <Action Action, auto Tag>
	void ComponentData(std::byte* pData, const utl::UUID_t componentUUID)
	{
		// Get Component Index
		const auto componentIndex = GetComponentIndex(componentUUID);

		using Tuple = utl::ctc::get_list<ComponentManager::GetComponentListID()>;
		using DataFiller = void(*)(std::byte*);
		using DataDeleter = void(*)(std::byte*);

		static constexpr auto table = [&]<std::size_t...Is>(std::index_sequence<Is...>)
		{
			return std::array<std::pair<DataFiller, DataDeleter>, sizeof...(Is)>{
				std::pair<DataFiller, DataDeleter>(&FillData<std::tuple_element_t<Is, Tuple>>, &DeleteData<std::tuple_element_t<Is, Tuple>>)...
			};
		}(std::make_index_sequence<std::tuple_size_v<Tuple>>());

		auto [pFiller, pDeleter] = table[componentIndex];
		switch (Action)
		{
		case Action::Fill:
			pFiller(pData);
			break;
		case Action::Delete:
			pDeleter(pData);
			break;
		}
	}

	template <ComponentInfo Info, auto Tag>
	typename ReturnType<Info>::T GetComponentInfo(const utl::UUID_t componentUUID)
	{
		// Get Component Index
		const auto componentIndex = GetComponentIndex(componentUUID);

		using Tuple = utl::ctc::get_list<ComponentManager::GetComponentListID()>;
		using NameInfo = const char*(*)();
		using SizeInfo = std::size_t(*)();

		Tuple t;

		static constexpr auto table = [&]<std::size_t...Is>(std::index_sequence<Is...>)
		{
			return std::array<std::tuple<NameInfo, SizeInfo>, sizeof...(Is)>{
				std::tuple<NameInfo, SizeInfo>(
					&ComponentName_<std::tuple_element_t<Is, Tuple>>,
					&ComponentSize_<std::tuple_element_t<Is, Tuple>>)...
			};
		}(std::make_index_sequence<std::tuple_size_v<Tuple>>());

		auto [pNameInfo, pSizeInfo] = table[componentIndex];

		if constexpr (Info == ComponentInfo::Name)
		{
			return pNameInfo();
		}
		else if constexpr (Info == ComponentInfo::Size)
		{
			return pSizeInfo();
		}
	}

	template <auto Tag>
	constexpr auto ComponentManager::GetComponentCount()
	{
		return utl::ctc::element_count<listID>();
	}
}
