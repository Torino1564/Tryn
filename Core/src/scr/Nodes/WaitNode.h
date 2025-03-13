#pragma once
#include "Core/src/scr/ScriptNode.h"

namespace tryn::scr
{
	struct WaitNode : public ScriptNode
    {
        WaitNode() = default;
	    WaitNode(ScriptGraph* pGraph, const std::string& name, const float timeInSeconds)
		    :
        ScriptNode(pGraph, name)
	    {
		    // Implement
	    }
    };

}
