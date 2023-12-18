#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <Core/src/utl/Assert.h>
#include <concepts>
#include <span>
#include <array>
#include <Core/third/dynamic_bitset.hpp>
#include <type_traits>
#include <optional>

#define ZT_COMPONENT_FIELDS(x) \
	public: struct SubresourceData{ bool active = false; std::uint16_t entityID = 0;  x }

#define ZT_DEFINE_COMPONENT(x) class x : public tryn::ent::Component<x>

namespace tryn::ent
{
	class ComponentManager;

	template <typename T>
	class Component;

	template <typename T>
	concept ImplementsUUID = requires
	{
		std::convertible_to<decltype(T::UUID), std::string>;
	};

	template <typename T>
	concept ImplementsSRD = requires
	{
		typename T::SubresourceData;
	};

	template <typename T>
	concept ValidComponent = ImplementsSRD<T> && ImplementsUUID<T>;

	class ComponentManager;
	class ArchetypeManager;

	using ComponentIndex = typename int;
	using ComponentSize = typename std::size_t;

	class ECS
	{
	public:
		static ECS& Get()
		{
			static ECS ecs;
			return ecs;
		}
		ComponentManager& componentManager;
		ArchetypeManager& archetypeManager;
	private:
		ECS();
	};

	class ComponentManager
	{
	public:
		static ComponentManager& Get()
		{
			static ComponentManager singleton;
			return singleton;
		}

		template <typename C>
		int RegisterComponent()
		{
			map.insert({ componentCounter, sizeof(C::SubresourceData)});
			return componentCounter++;
		}

		void ActivateComponent(std::uint16_t componentUUID, std::uint16_t componentIndex);
	private:
		ComponentManager() = default;
		std::uint16_t componentCounter = 0;
		std::unordered_map<ComponentIndex, ComponentSize> map;
	};

	template <typename T>
	class Component
	{
	public:
		ZT_COMPONENT_FIELDS();
	public:
		const static inline int UUID = ComponentManager::Get().RegisterComponent<T>();
	};

	ZT_DEFINE_COMPONENT(ActivationComponent)
	{
		ZT_COMPONENT_FIELDS();
	};
	
	class Archetype
	{
		friend class Entity;
		friend class ArchetypeManager;
		template <ValidComponent... Cs>
		Archetype Make()
		{
			Archetype archetype;
			archetype.InitializeUUID();
			archetype.AppendComponents<Cs...>();
			return archetype;
		}
		template <ValidComponent First, ValidComponent Second, ValidComponent... Rest>
		void AppendComponents()
		{
			AppendComponents<First>();
			AppendComponents<Second, Rest...>();
		}
		template <ValidComponent C>
		void AppendComponents()
		{
			components.push_back(C::UUID);
		}
		const int GetUUID() const
		{
			return this->UUID;
		}
		const int ComponentCount() const
		{
			return (int)(components.size());
		}
	private:
		void InitializeUUID();
		int UUID = -1;
		std::vector<int> components;
		sul::dynamic_bitset<> booker;
		std::vector<std::unique_ptr<std::vector<std::byte>>> bufferPtrs;
	};

	class ArchetypeManager
	{
	public:
		friend class Archetype;

		std::array<Archetype*, 100> QueryArchetype(std::span<ComponentIndex> componentIDs)
		{
			std::array<std::pair<Archetype*, int>, 1000 > archetypeMap = {};

			std::array<Archetype*, 100> result = {};

			for (auto componentID : componentIDs)
			{
				for (auto& archetype : archetypeTable[componentID])
				{
					auto& ref = archetypeMap[archetype->GetUUID()];
					ref.first = archetype;
					ref.second++;
				}
			}
			int resultCounter = 0;
			for (auto& [pType, counter] : archetypeMap)
			{
				if (counter == componentIDs.size())
				{
					result[resultCounter++] = pType;
				}
			}

			return result;
		}

		template <ValidComponent... Cs>
		std::array<Archetype*, 100> QueryArchetype()
		{
			std::array<int, sizeof...(Cs)> componentIDs;
			ExtractComponentIDs<sizeof...(Cs), Cs...>(componentIDs);

			return QueryArchetype(std::span<int>(componentIDs.begin(), componentIDs.size()));
		}

		Archetype* GetArchetype(std::span<ComponentIndex> components)
		{
			auto queriedArchetypes = QueryArchetype(components);

		}

		template <ValidComponent... Cs>
		Archetype* GetArchetype()
		{
			//std::array<int, sizeof...(Cs)> componentIDs;
			//ExtractComponentIDs<sizeof...(Cs), Cs...>(componentIDs);

			//for (auto pArchetype : queriedArchetypes)
			//{
			//	if (pArchetype->ComponentCount() == sizeof...(Cs))
			//	{
			//		return pArchetype;
			//	}
			//}

			//// No existing archetype was found, adding a new one
			//return AddArchetype<Cs...>();
		}

		static ArchetypeManager& Get()
		{
			static ArchetypeManager singleton;
			return singleton;
		}
		template <ValidComponent... Cs>
		Archetype* AddArchetype()
		{
			archetypeBuffer[archetypeCounter++] = Archetype::Make<Cs...>();
			auto& newlyAddedArchetype = archetypeBuffer[archetypeCounter - 1];

			for (auto componentIndex : newlyAddedArchetype.components)
			{
				archetypeTable[componentIndex].push_back(&newlyAddedArchetype);
			}
			return &newlyAddedArchetype;
		}
	private:
		template <int arraySize, ValidComponent First, ValidComponent Second, ValidComponent... Rest>
		void ExtractComponentIDs(std::array<int, sizeof(arraySize)>& componentIDs, int index = 0)
		{
			ExtractComponentIDs<arraySize, First>(index++);
			ExtractComponentIDs<arraySize, Second, Rest...>(index);
		}
		template <int arraySize, ValidComponent C>
		void ExtractComponentIDs(std::array<int, sizeof(arraySize)>& componentIDs, int index)
		{
			componentIDs[index] = C::UUID;
		}
		ArchetypeManager() = default;
		int ResolveUUID()
		{
			return archetypeCounter++;
		}
		int archetypeCounter = 0;
		// Indexed by componentUUID
		std::vector<std::vector<Archetype*>> archetypeTable;
		std::array<Archetype, 1000> archetypeBuffer = {};
	};
}