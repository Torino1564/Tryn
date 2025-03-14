#pragma once
#include "Core/src/utl/StringHasher.h"
#include "TNode.h"

namespace tryn::gph
{
    class TGraph;
    template <typename GraphType>
	struct TNodeRegister
    {
        struct TNodeTypeInfo
        {
	        const char* name = nullptr;
            void (*pCreate)(GraphType*, spa::Vec2I, std::function<void()>&) = nullptr;
        };

        // returns false if the type could not be added
        template <typename T>
        bool RegisterNodeType()
        {
        	static constexpr auto uuid = ZT_TYPE_UUID(T);

            if (const auto it = map.find(uuid); it != map.end())
                return false;

            map.insert({uuid, TNodeTypeInfo{.name = ZT_TYPE_OF(T).data(), .pCreate = &T::ImGuiCreate}});
            return true;
        }
        auto& Map()
        {
	        return map;
        }
    private:

        std::unordered_map<utl::UUID_t, TNodeTypeInfo> map;
    };

}
