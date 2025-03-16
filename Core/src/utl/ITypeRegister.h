#pragma once
#include <Core/src/utl/StringHasher.h>

#include "Assert.h"

namespace tryn::utl
{
    template <typename Interface>
    struct ITypeRegister
    {
        struct TypeInfo
        {
            const char* name = nullptr;
            void(*createFunc)(Interface**) = nullptr;
            std::size_t size;
        };

        template <typename T>
            requires std::derived_from<T, Interface> || std::same_as<Interface, std::any>
        void RegisterType()
        {
            static constexpr auto uuid = ZT_TYPE_UUID(T);
            const auto it = std::ranges::find_if(infoTable, [&](const auto pair)
                {
                    return pair.first == uuid;
                });

            if (it == infoTable.end())
            {
                infoTable.push_back({ uuid,
                    TypeInfo{
                        .name = ZT_TYPE_OF(T).data(),
						.createFunc =   [](Interface** ppInt)
	                                    {
	                                        if constexpr (std::same_as<Interface, std::any>)
	                                        {
	                                            *ppInt->template emplace<T>();
	                                        }
	                                        else
	                                        {
	                                            *ppInt = new T();
	                                        }
	                                    },
                        .size = sizeof(T)
                        }
                    }
                );
            }
        }

        void ConstructAt(Interface& iRef, UUID_t uuid)
        {
            const auto it = std::ranges::find_if(infoTable, [&](const auto pair)
                {
                    return pair.first == uuid;
                });

            if constexpr (std::same_as<Interface, std::any>)
                iRef.reset();
            auto pRef = &iRef;
            it->second.createFunc(&pRef);
        }

        void ConstructAndFill(Interface** ppInt, UUID_t uuid)
        {
            const auto it = std::ranges::find_if(infoTable, [&](const auto pair)
                {
                    return pair.first == uuid;
                });

            it->second.createFunc(ppInt);
        }

        TypeInfo GetTypeInfo(UUID_t uuid)
        {
            const auto it = std::ranges::find_if(infoTable, [&](const auto pair)
                {
                    return pair.first == uuid;
                });

            if (it != infoTable.end())
                return it->second;
            else
            {
	            trynchk_fail.msg(L"Did not find the specific type info");
                std::unreachable();
            }
        }

        std::vector<std::pair<UUID_t, TypeInfo>> infoTable = {};
    };
}
