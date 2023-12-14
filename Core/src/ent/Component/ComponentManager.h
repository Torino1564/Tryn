#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <Core/src/utl/Assert.h>
#include <concepts>

#define ZT_COMPONENT_FIELDS(x) \
	public: struct SubresourceData{ bool active = false; x; }

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
			Resize(componentCounter, 100);
			return componentCounter++;
		}
		template <ValidComponent C>
		C::SubresourceData& AddComponent(std::uint16_t entityID)
		{
			auto componentDataIt = map.find(C::UUID);
			trynass_msg(componentDataIt != map.end(), L"Unknown Component Type!");
			auto& componentData = *componentDataIt;
			std::uint16_t totalOffset = componentData.second * entityID;
			auto& buffer = *(bufferPtrs[C::UUID]);

			std::uint16_t bufferSize = buffer.size();
			
			if (bufferSize < totalOffset + componentData.second)
			{
				while (bufferSize < totalOffset + componentData.second)
				{
					bufferSize = (bufferSize + 5) * 1.3f;
				}
				buffer.resize(bufferSize);
			}

			return reinterpret_cast<C::SubresourceData&>(buffer[totalOffset]);
		}
		void Resize(std::uint16_t index, std::uint16_t newSize)
		{
			bufferPtrs[index]->resize(newSize * map[index]);
			bitsetPtrs[index]->resize(newSize);
		}
		template <ValidComponent C>
		auto& GetComponent(std::uint16_t entityID)
		{
			const auto& componentData = map[C::UUID];
			std::uint16_t totalOffset = componentData * entityID;
			auto& buffer = *(bufferPtrs[C::UUID]);
			trynass_msg(buffer.size() > totalOffset, L"Out of bounds access in the ECS");
			return reinterpret_cast<C::SubresourceData&>(buffer[totalOffset]);
		}
	private:
		ComponentManager() = default;
		std::uint16_t componentCounter = 0;
		std::unordered_map<int, std::size_t> map;
		std::vector<std::unique_ptr<std::vector<std::byte>>> bufferPtrs;
		std::vector<std::unique_ptr<sul::dynamic_bitset<>>> bitsetPtrs;
	};

	template <typename T>
	class Component
	{
		struct SubresourceData
		{
			bool active = false;
		};
	public:
		const static inline int UUID = ComponentManager::Get().RegisterComponent<T>();
	};
}