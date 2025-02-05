#pragma once
#include <memory>

namespace tryn::scr
{
	class ScriptInstance
	{
	private:
		std::shared_ptr<class ScriptGraph> pGraph;
		std::unique_ptr<class VariableBag> instanceVariables;
		VariableBag* componentVariables = nullptr;
	};
}