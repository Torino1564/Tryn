#pragma once
#include <Core/src/scr/Variable.h>
#include <Core/src/gph/TGraph.h>
#include <Core/src/gph/TNodeRegister.h>

namespace tryn
{
    namespace scr
	{
		class ScriptGraph : public gph::TGraph
		{
		public:
			ScriptGraph(std::string_view name);

			std::vector<Variable> variables;
		};
	}
}
