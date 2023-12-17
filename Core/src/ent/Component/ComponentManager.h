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
#define ZT_COMPONENT_CONSTRUCTOR public: static SubresourceData&& Construct

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
			bufferPtrs.push_back(std::move(std::make_unique<std::vector<std::byte>>()));
			bitsetPtrs.push_back(std::move(std::make_unique<sul::dynamic_bitset<>>()));
			Resize(componentCounter, 1000);
			return componentCounter++;
		}

		template <ValidComponent C>
		[[nodiscard("The returned integer is a handle to a component")]] C::SubresourceData* AddComponent(std::uint16_t entityID)
		{
			// Finds empty slot
			auto ID = bitsetPtrs[C::UUID]->find_first();
			if (ID == sul::dynamic_bitset<>::npos)
			{
				Grow(C::UUID);
				ID = bitsetPtrs[C::UUID]->find_first();
			}
			bitsetPtrs[C::UUID]->flip(ID);
			auto& ref = reinterpret_cast<C::SubresourceData&>((*(bufferPtrs[C::UUID]))[ID * map[C::UUID]]);
			ref.entityID = entityID;
			return &ref;
		}
		template <ValidComponent C>
		auto GetComponent(std::uint16_t componentID)
		{
			const auto& componentData = map[C::UUID];
			std::uint16_t totalOffset = componentData * componentID;
			auto& buffer = *(bufferPtrs[C::UUID]);
			trynass_msg(buffer.size() > totalOffset, L"Out of bounds access in the ECS");
			return reinterpret_cast<C::SubresourceData*>(&(buffer[totalOffset]));
		}

		template <ValidComponent C>
		std::span<typename C::SubresourceData> GetData()
		{
			auto pStart = reinterpret_cast<typename C::SubresourceData*>(bufferPtrs[C::UUID]->data());
			return std::span<typename C::SubresourceData>(pStart, bufferPtrs[C::UUID]->size() / map[C::UUID]);
		}

		void ActivateComponent(std::uint16_t componentUUID, std::uint16_t componentIndex);
	private:
		void Resize(std::uint16_t index, std::uint16_t newSize)
		{
			bufferPtrs[index]->resize(newSize * map[index]);
			bitsetPtrs[index]->resize(newSize, true);
		}
		void Grow(std::uint16_t index, float scale = 1.3f)
		{
			bufferPtrs[index]->resize(bufferPtrs[index]->size() * scale);
			bitsetPtrs[index]->resize(bitsetPtrs[index]->size() * scale, true);
		}
		ComponentManager() = default;
		std::uint16_t componentCounter = 0;
		std::unordered_map<int, std::size_t> map;
		std::vector<std::unique_ptr<std::vector<std::byte>>> bufferPtrs;
		std::vector<std::unique_ptr<sul::dynamic_bitset<>>> bitsetPtrs;
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

	class Archetype;

	class ArchetypeManager
	{
		template <ValidComponent... Cs>
		std::array<Archetype*, 100> QueryArchetype()
		{
			std::array<int, sizeof...(Cs)> componentIDs;
			ExtractComponentIDs<sizeof...(Cs), Cs...>(componentIDs);

			static std::array<Archetype*, 1000> = {};

			for (auto componentID : componentIDs)
			{

			}
		}
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
	private:
		int archetypeCounter = 0;
		// Indexed by componentUUID
		std::vector<std::vector<Archetype>> archetypeTable;
	};

	class Archetype
	{

	private:
		std::vector<int> components;
		int UID;
		sul::dynamic_bitset<> booker;
		std::vector<std::unique_ptr<std::vector<std::byte>>> bufferPtrs;
	};
}