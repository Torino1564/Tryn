#pragma once
#include <any>
#include <vector>
#include "Core/src/utl/StringHasher.h"

namespace tryn::utl
{
    using UUID_t = uint64_t;
}

namespace tryn::ed
{
    struct TypeRegister
    {
        template <typename T>
        void RegisterType()
        {
            static constexpr auto uuid = ZT_TYPE_UUID(T);
            const auto it = std::ranges::find_if(functionPtrs, [&](const auto pair)
                {
                    return pair.first == uuid;
                });

            if (it == functionPtrs.end())
            {
                functionPtrs.push_back({ uuid, [](std::any& anyRef)
                {
                    anyRef.emplace<T>();
                } });
            }
        }

        void ConstructAny(std::any& anyRef, utl::UUID_t uuid)
        {
            const auto it = std::ranges::find_if(functionPtrs, [&](const auto pair)
                {
                    return pair.first == uuid;
                });

            anyRef.reset();

            it->second(anyRef);
        }

        std::vector<std::pair<utl::UUID_t, void(*)(std::any&)>> functionPtrs = {};
    };
}
