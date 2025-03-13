#pragma once
#include "Core/src/utl/StringHasher.h"
#include <Core/src/scr/ScriptGraph.h>

namespace tryn::scr
{
	struct NodeRegister
    {
        struct NodeTypeInfo
        {
	        const char* name = nullptr;
            void (*pCreate)(ScriptGraph*, spa::Vec2I, std::function<void()>&) = nullptr;
        };

        // returns false if the type could not be added
        template <typename T>
        bool RegisterNodeType()
        {
        	static constexpr auto uuid = ZT_TYPE_UUID(T);

            if (const auto it = map.find(uuid); it != map.end())
                return false;

            map.insert({uuid, NodeTypeInfo{.name = ZT_TYPE_OF(T).data(), .pCreate = &T::ImGuiCreate}});
            return true;
        }
	    static NodeRegister& Get()
	    {
		    static NodeRegister singleton;
            return singleton;
	    }
        auto& Map()
        {
	        return map;
        }
    private:

        NodeRegister() = default;
        std::unordered_map<utl::UUID_t, NodeTypeInfo> map;
    };

}
