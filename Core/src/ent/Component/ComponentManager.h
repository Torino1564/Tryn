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
#include <Core/src/mem/ArenaAllocator.h>
#include <ranges>
#include <Core/src/mem/NativeArray.h>
#include <Core/src/utl/Span.h>

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
		mem::ArenaAllocator<> allocator;
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

		auto& GetComponentMap()
		{
			return map;
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
	
	template <ValidComponent... Cs>
	using HeterogeneusComponentDataPointerContainer = typename std::tuple<utl::Span<typename Cs::SubresourceData>...>;

	class Archetype
	{
		friend class Entity;
		friend class ArchetypeManager;

	public:
		template <ValidComponent... Cs>
		void FillComponentPointerTuple(std::tuple<utl::Span<typename Cs::SubresourceData>...>& container)
		{
			FillComponentPointerTupleImpl<0,Cs...>(container);
		}

		template <int N, typename... Cs> using NthTypeOf = 
			typename std::tuple_element<N, std::tuple<Cs...>>::type;

		template <int N = 0, ValidComponent... Cs>
		void FillComponentPointerTupleImpl(std::tuple<utl::Span<typename Cs::SubresourceData>...>& container)
		{
			std::get<N>(container) = GetComponentData<NthTypeOf<N, Cs...>>();
			if constexpr (N < sizeof...(Cs) - 1)
			{
				FillComponentPointerTupleImpl<N + 1, Cs...>(container);
			}
		}

		template <ValidComponent C>
		utl::Span<typename C::SubresourceData> GetComponentData()
		{
			for (auto [index, componentUUID] : std::ranges::views::enumerate(components))
			{
				if (componentUUID == C::UUID)
				{
					return utl::Span<typename C::SubresourceData>(
						reinterpret_cast<typename C::SubresourceData*>(bufferPtrs[index]->data()),
						bufferPtrs[index]->size() / sizeof(typename C::SubresourceData));
				}
			}
			return {};
		}

		template <ValidComponent... Cs>
		static Archetype Make()
		{
			Archetype archetype;
			archetype.InitializeUUID();
			archetype.Resize(100);
			archetype.AppendComponents<Cs...>();
			return archetype;
		}
		static Archetype Make(utl::Span<int> componentIDs)
		{
			Archetype archetype;
			archetype.InitializeUUID();
			archetype.Resize(100);
			archetype.components.reserve(componentIDs.Size());
			for (auto i = 0 ; i < componentIDs.Size() ; i++)
			{
				archetype.components.push_back(componentIDs[i]);
				archetype.bufferPtrs.push_back(std::make_unique<std::vector<std::byte>>());
			}
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
			bufferPtrs.push_back(std::make_unique<std::vector<std::byte>>());
		}
		const int GetUUID() const
		{
			return this->UUID;
		}
		const int ComponentCount() const
		{
			return (int)(components.size());
		}
		class EntityID ResolveEntityUUID();
		void Free(class EntityID);
		void Grow()
		{
			Resize(booker.size() * 3);
		}
		void Resize(std::uint32_t newSize)
		{
			booker.resize(newSize, true);
			for (auto [index, pBuffer]: std::ranges::views::enumerate(bufferPtrs) )
			{
				pBuffer->resize(newSize * ComponentManager::Get().GetComponentMap().at(components[index]));
			}
		}
	private:
		void InitializeUUID();
		std::uint16_t UUID = 0;
		std::vector<int> components;
		std::uint32_t bookerPointer = 0;
		sul::dynamic_bitset<> booker;
		std::vector<std::unique_ptr<std::vector<std::byte>>> bufferPtrs;
	};

	class ArchetypeManager
	{
	public:
		friend class Archetype;

	public:
		template <ValidComponent... Cs>
		utl::Span<std::tuple<utl::Span<typename Cs::SubresourceData>...>> GetComponentGroup()
		{
			auto archetypeQuery = QueryArchetype<Cs...>();
			mem::NativeArray<std::tuple<utl::Span<typename Cs::SubresourceData>...>> heterogeneusComponentPointerArray(archetypeQuery.Size(), ECS::Get().allocator);
			
			std::array<std::tuple<utl::Span<typename Cs::SubresourceData>...>, 100> testArr = {};

			for (auto i = 0 ; i < archetypeQuery.Size() ; i++)
			{
				archetypeQuery[i]->FillComponentPointerTuple<Cs...>(testArr[i]);
			}
			
			return utl::Span<std::tuple<utl::Span<typename Cs::SubresourceData>...>>(testArr.data(), archetypeQuery.Size());
		}

		utl::Span<Archetype*> QueryArchetype(utl::Span<ComponentIndex> componentIDs)
		{
			static std::vector<std::pair<Archetype*, int> > archetypeMap;
			static bool initialized = false;
			if (!initialized)
			{
				initialized = true;
				archetypeMap.resize(1000);
			}

			std::fill(archetypeMap.begin(), archetypeMap.end(), std::pair<Archetype*, int>{nullptr, 0});

			auto result = *ECS::Get().allocator.MakeNew<std::array<Archetype*, 100>>();

			for (int i = 0 ; i < componentIDs.Size() ; i++)
			{
				if (componentIDs[i] < archetypeTable.size())
				{
					for (auto& archetype : archetypeTable[componentIDs[i]])
					{
						auto& ref = archetypeMap[archetype->GetUUID()];
						ref.first = archetype;
						ref.second++;
					}
				}
			}

			int resultCounter = 0;
			for (auto& [pType, counter] : archetypeMap)
			{
				if (counter == componentIDs.Size())
				{
					result[resultCounter++] = pType;
				}
			}

			return utl::Span<Archetype*>(result.data(), resultCounter);
		}

		template <ValidComponent... Cs>
		utl::Span<Archetype*> QueryArchetype()
		{
			auto pComponentIDs = ECS::Get().allocator.MakeNew<std::array<int, sizeof...(Cs)>>();
			auto& componentIDs = *pComponentIDs;
			ExtractComponentIDs<sizeof...(Cs), Cs...>(componentIDs);

			return QueryArchetype(utl::Span<int>(componentIDs.data(), componentIDs.size()));
		}

		Archetype* GetArchetype(utl::Span<ComponentIndex> components)
		{
			auto queriedArchetypes = QueryArchetype(components);

			for (auto i = 0 ; i < queriedArchetypes.Size() ; i++)
			{
				auto pArchetype = queriedArchetypes[i];
				if (pArchetype != nullptr && pArchetype->ComponentCount() == components.Size())
				{
					return pArchetype;
				}
			}

			// No existing archetype was found, adding a new one
			return AddArchetype(components);
		}

		template <ValidComponent... Cs>
		Archetype* GetArchetype()
		{
			auto pComponentIDs = ECS::Get().allocator.MakeNew<std::array<int, sizeof...(Cs)>>();
			auto& componentIDs = *pComponentIDs;
			ExtractComponentIDs<sizeof...(Cs), Cs...>(componentIDs);

			return GetArchetype(utl::Span<int>(componentIDs.data(), componentIDs.size()));
		}

		static ArchetypeManager& Get()
		{
			static ArchetypeManager singleton;
			return singleton;
		}
		template <ValidComponent... Cs>
		Archetype* AddArchetype()
		{
			archetypeBuffer[archetypeCounter] = Archetype::Make<Cs...>();
			auto& newlyAddedArchetype = archetypeBuffer[archetypeCounter - 1];

			for (auto componentIndex : newlyAddedArchetype.components)
			{
				archetypeTable[componentIndex].push_back(&newlyAddedArchetype);
			}
			return &newlyAddedArchetype;
		}
		Archetype* AddArchetype(utl::Span<int> componentIDs)
		{
			archetypeBuffer[archetypeCounter] = Archetype::Make(componentIDs);
			auto& newlyAddedArchetype = archetypeBuffer[archetypeCounter];

			for (auto componentIndex : newlyAddedArchetype.components)
			{
				if (archetypeTable.size() < componentIndex + 1)
				{
					archetypeTable.resize(componentIndex + 1);
				}
				archetypeTable[componentIndex].push_back(&newlyAddedArchetype);
			}
			return &newlyAddedArchetype;
		}
	private:
		template <int arraySize, ValidComponent First, ValidComponent Second, ValidComponent... Rest>
		void ExtractComponentIDs(std::array<int, arraySize>& componentIDs, int index = 0)
		{
			ExtractComponentIDs<arraySize, First>(componentIDs, index++);
			ExtractComponentIDs<arraySize, Second, Rest...>(componentIDs, index);
		}
		template <int arraySize, ValidComponent C>
		void ExtractComponentIDs(std::array<int, arraySize>& componentIDs, int index = 0)
		{
			componentIDs[index] = C::UUID;
		}
		ArchetypeManager()
		{
			archetypeBuffer.resize(1000);
		}
		int ResolveUUID()
		{
			return archetypeCounter++;
		}
		int archetypeCounter = 0;
		// Indexed by componentUUID
		std::vector<std::vector<Archetype*>> archetypeTable;
		std::vector<Archetype> archetypeBuffer = {};
	};
}