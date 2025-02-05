#pragma once
#include <Core/src/scr/ScriptNode.h>
#include <concepts>

namespace tryn::scr
{
	class StateNode : public ScriptNode
	{
	public:
		template <typename T>
			requires std::is_integral_v<T>
		static StateNode Make()
	};
}
