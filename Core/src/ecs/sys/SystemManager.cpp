#include "TrynPCH.h"
#include "SystemManager.h"
#include <Core/third/dynamic_bitset.hpp>
#include <Core/src/log/Log.h>
#include <ranges>
#include <Core/src/ecs/cmp/ComponentManager.h>

#include <Core/src/ecs/sys/TransformSystem.h>
#include <Core/src/ecs/sys/RenderSystem.h>
#include <Core/src/ecs/sys/UpdatePositionSystem.h>
#include <Core/src/ecs/sys/UpdateVelocitySystem.h>
#include <Core/src/ecs/sys/AnimationSystem.h>


namespace tryn::ecs
{
	SystemGraph::SystemGraph(SystemManager& manager)
		:
	pManager(&manager)
	{
	}

	void SystemGraph::Finalize2()
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

		// call init method on all systems

		for (auto [levelIndex, level] : std::ranges::views::enumerate(levels))
		{
			for (auto systemIndex : level.systemIndices)
			{
				pSystems[systemIndex]->Init();
			}
		}

		finalized = true;
	}

	void SystemGraph::Finalize()
	{
		/* Create the following data structure
		 * ------------------
		 *	|system	|dep of	|
		 *	-----------------
		 *	|	1	| [...]	|
		 *	|	2	|		|
		 * 	|	3	|		|
		 *	|	4	|		|
		 *	|  ...	|		|
		 *	|   N	|		|
		 *	-----------------
		 *	Each system registers its dependencies and adds itself
		 *	as a dependency for systems it must happen before
		 */

		struct Row
		{
			int systemID;
			std::vector<int> dependencies; // must happen after these systems
		};

		std::vector<Row> table; table.resize(pSystems.size());

		// Fill table
		for (const auto& pSystem : pSystems)
		{
			const auto& system = *pSystem;
			auto& row = table[system.ID()];

			// System ID
			row.systemID = system.ID();

			// Add dependencies
			for (const auto depUID : system.dependencyUIDs)
			{
				row.dependencies.push_back(depUID.id);
			}

			// Add itself as dependency for systems it must happen before of
			for (const auto preReqOfUID : system.prerequisiteOfUIDs)
			{
				table[preReqOfUID.id].dependencies.push_back(system.ID());
			}
		}

		// Once the table is filled, we iterate over all systems and place them on levels accordingly
		sul::dynamic_bitset<> systemsSet; systemsSet.resize(pSystems.size());

		// Loop while there are still systems to set
		while (!systemsSet.all())
		{
			levels.emplace_back();
			auto& currentLevel = levels.back();
			// For each unset system, loop over all dependencies in the table and check if it can be placed in the current level
			for (int systemID = 0; systemID < systemsSet.size(); systemID++)
			{
				// Skip if set
				if (systemsSet[systemID])
				{
					continue;
				}

				// Check dependencies if not set.
				bool dependenciesSet = true;
				for (auto dependencyID : table[systemID].dependencies)
				{
					if (!systemsSet[dependencyID])
					{
						dependenciesSet = false;
						break;
					}
				}

				// Register to this level if its dependencies are set
				if (dependenciesSet)
				{
					currentLevel.systemIndices.push_back(systemID);
					systemsSet[systemID].set();
				}
			}
		}

		// Lastly call init on all systems
		for (auto& pSystem : pSystems)
		{
			pSystem->Init();
		}
	}

	void SystemGraph::Execute() const
	{
		trynass(finalized).msg(L"The system graph must be finalized before it can be executed!").ex();

		for (auto [levelIndex, level] : std::ranges::views::enumerate(levels))
		{
//			trylog.verbose(utl::ToWide(std::format("Executing level {}", levelIndex)));
			// execute async
			for (auto systemIndex : level.systemIndices)
			{
				pSystems[systemIndex]->Execute();
			}
		}
	}

	const gfx::IGraphics& SystemGraph::Gfx() const
	{
		trynass(pManager).msg(L"pManager was nullptr!").ex();
		return pManager->Gfx();
	}

	System::System(const SystemGraph& graph)
		:
		pEcs(graph.GetSystemManager().GetECS()), pGraph(&graph)
	{
	}

	bool System::SystemUID::operator==(const SystemUID& rhs) const
	{
		if (id == rhs.id)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	SystemManager::SystemManager(ECS* pEcs)
		:
	pEcs(pEcs),
	graph(*this)
	{
		// Register default systems
		RegisterSystem<TransformSystem>();
		RegisterSystem<RenderSystem>();
		RegisterSystem<UpdatePositionSystem>();
		RegisterSystem<UpdateVelocitySystem>();
		RegisterSystem<AnimationSystem>();
	}

	void SystemManager::ExecuteSystems() const
	{
		graph.Execute();
	}

	void SystemManager::Finalize()
	{
		graph.Finalize();
	}

	const gfx::IGraphics& SystemManager::Gfx() const
	{
		trynass(pEcs).msg(L"pApp was null!").ex();

		return pEcs->Gfx();
	}
}
