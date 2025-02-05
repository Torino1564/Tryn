#pragma once
#include <string>
#include <memory>
#include <Core/src/utl/Assert.h>
#include <type_traits>
#include <Core/src/mem/ArenaAllocator.h>
#include <Core/src/mem/NativeArray.h>
#include <Core/src/utl/Span.h>
#include <Core/src/utl/Exception.h>
#include <Core/src/ecs/cmp/PrintMember.h>
#include <Core/src/ecs/EntityID.h>
#include <Core/src/ecs/EcsClass.h>
#include <Core/src/ecs/Component.h>
#include <Core/src/utl/TypeName.h>
#include <unordered_map>
#include <Core/src/utl/StringHasher.h>

ZT_EX_DEF(ComponentSMPException);

namespace tryn::ecs
{
	class ECS;

	class ComponentManager
	{
	public:
		ComponentManager(const ECS* pEcs);

		template <typename T>
		void RegisterComponent()
		{
			static constexpr auto name = ZT_TYPE_OF(T);
			static constexpr auto uuid = ZT_TYPE_UUID(T);
			// assert duplicate
			const auto it = componentWrappers.find(uuid);
			trynass(it != componentWrappers.end()).msg(L"The component already exists!");

			componentWrappers.insert({uuid, ComponentWrapper::Make<T>(name, NextFreeAndIncrement())});
		}

		template <typename T>
		requires std::is_default_constructible_v<T>
		void RegisterSingleton()
		{
			RegisterSingleton<T>(std::move(T()));
		}

		template <typename T>
		void RegisterSingleton(T&& singleton)
		{
			static constexpr utl::UUID_t uuid = ZT_TYPE_UUID(T);

			const auto it = singletonWrappers.find(uuid);
			trynass(it != singletonWrappers.end()).msg(L"The component already exists!");

			// component doesn't exist
			singletonWrappers.insert({ uuid, SingletonWrapper::Make(ZT_TYPE_OF(T), NextFreeAndIncrement(), std::move(singleton)) });
		}

		uint16_t ComponentCount() const;

		template<typename T>
		const ComponentWrapper& Wrapper() const
		{
			return componentWrappers.at(ZT_TYPE_UUID(T));
		}

		const ComponentWrapper& Wrapper(const utl::UUID_t componentUUID) const;

		template <typename T>
		const ComponentWrapper& PWrapper()
		{
			if (auto it = componentWrappers.find(ZT_TYPE_UUID(T)); it == componentWrappers.end())
				RegisterComponent<T>();

			return componentWrappers.at(ZT_TYPE_UUID(T));
		}

		template <typename T>
		T& RequestSingleton()
		{
			const auto it = singletonWrappers.find(ZT_TYPE_UUID(T));
			trynass(it != singletonWrappers.end()).msg(L"The requested singleton is not registered!");

			return static_cast<T&>(*it->second.GetData());
		}

		template <typename T>
		const T& RequestSingleton()
		{
			const auto it = singletonWrappers.find(ZT_TYPE_UUID(T));
			trynass(it != singletonWrappers.end()).msg(L"The requested singleton is not registered!");

			return static_cast<const T&>(*it->second.GetData());
		}

	private:
		uint16_t NextFreeAndIncrement();
		const ECS* pEcs = nullptr;
		uint16_t componentCount = 0;
		std::unordered_map<utl::UUID_t, ComponentWrapper> componentWrappers;
		std::unordered_map<utl::UUID_t, SingletonWrapper> singletonWrappers;
	};
}
