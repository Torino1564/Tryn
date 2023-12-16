#include "SystemManager.h"
#include <Core/third/dynamic_bitset.hpp>
#include <Core/src/log/Log.h>
#include <ranges>
#include <Core/src/utl/String.h>

namespace tryn::ent::sys
{
	void SystemGraph::Finalize()
	{
		sul::dynamic_bitset<> systemsSet;
		sul::dynamic_bitset<> perLevelSystemsSet;
		systemsSet.resize(pSystems.size());
		perLevelSystemsSet.resize(pSystems.size());

		// set unused systems
		for (auto [systemIndex, pSystem] : std::views::enumerate(pSystems))
		{
			if (pSystem == nullptr)
			{
				systemsSet[systemIndex].set();
			}
		}

		// loop while there are systems not set to their appropiate level
		while (!systemsSet.all())
		{
			levels.emplace_back();
			perLevelSystemsSet = systemsSet;
			// pointer used to sweep across all systems
			auto systemPointer = 0;
			while (systemPointer != -1)
			{
				//set the value of systemPointer to the next non set system
				for (int i = systemPointer; i < pSystems.size(); i++)
				{
					if (systemsSet[i] == false)
					{
						systemPointer = i;
						break;
					}
				}
				if (systemPointer == pSystems.size())
					break;
				// Get the system that systemPointer points to
				auto& pSystem = pSystems[systemPointer];
				bool dependenciesSet = true;
				for (auto& dependencyIndex : pSystem->dependencyUIDs)
				{
					// Check if all dependencies have been set
					if (!perLevelSystemsSet[dependencyIndex.id])
					{
						// if a single dependency has not been set, then it cant add this system to the current level
						dependenciesSet = false;
						break;
					}
				}
				if (dependenciesSet)
				{
					// all dependencies are set
					levels.back().systemIndices.push_back(systemPointer);
					systemsSet[systemPointer].set();
				}
				// increments system pointer
				systemPointer++;
			}
		}

		finalized = true;
	}
	void SystemGraph::Execute()
	{
		trynass_msg(finalized, L"Can only execute a finalized System Graph!");

		for (auto [levelIndex, level] : std::ranges::views::enumerate(levels))
		{
			trylog.info(utl::ToWide(std::format("Executing level {}", levelIndex)));
			// execute async
			for (auto systemIndex : level.systemIndices)
			{
				pSystems[systemIndex]->Execute();
			}
		}
	}
}