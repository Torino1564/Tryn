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
	class ComponentManager
	{
	public:
		ComponentManager(const ECS* pEcs);

		template <typename T>
		void RegisterComponent()
		{
			static constexpr auto name = ZT_TYPE_OF(T);
			// assert duplicate
			for (auto& wrapper : componentWrappers | std::views::values)
			{
				trynass(wrapper.Name() != name).msg(L"The component already exists!");
			}
			componentWrappers.insert(ComponentWrapper::Make<T>(name, NextFreeAndIncrement()), ZT_STRING_HASH(name));
		}

		uint16_t ComponentCount() const;

		template<typename T>
		const ComponentWrapper& Wrapper() const
		{
			return componentWrappers.at(ZT_TYPE_UUID(T));
		}

		const ComponentWrapper& Wrapper(const utl::UUID_t componentUUID) const
		{
			return componentWrappers.at(componentUUID);
		}

	private:
		uint16_t NextFreeAndIncrement();
		const ECS* pEcs = nullptr;
		uint16_t componentCount = 0;
		std::unordered_map<utl::UUID_t, ComponentWrapper> componentWrappers;
		
	};
}
