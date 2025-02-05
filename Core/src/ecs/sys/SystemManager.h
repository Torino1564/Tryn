#pragma once
#include <memory>
#include <vector>
#include <concepts>
#include <Core/src/utl/Assert.h>
#include <Core/src/log/Log.h>
#include <Core/src/ecs/EcsClass.h>

#define ZT_DEFINE_SYSTEM(x) class x : public tryn::ecs::SystemImpl<x>
#define ZT_NATIVE_ARRAY(x) private: tryn::utl::MultiSpan<x>

namespace tryn::app
{
	class App;
}

namespace tryn::gfx
{
	class IGraphics;
}

namespace tryn::ecs
{
	class SystemManager;

	template <typename T>
	class SystemImpl;
	class System;
	template <typename T>
	concept ValidSystem = requires
	{
		std::derived_from<T, SystemImpl<T>>;
	};

	class ECS;

	class SystemGraph
	{
	public:
		SystemGraph() = default;
		SystemGraph(SystemManager& manager);

		template <ValidSystem S>
		void RegisterSystem()
		{
			// assert not finalized
			trynass_msg(!finalized, L"Cannot register new systems into a finalized system graph!");
			// assert minimum size
			if (pSystems.size() <= S::UID.id)
			{
				pSystems.resize(S::UID.id + 1);
			}
			// assert uniqueness
			if (pSystems[S::UID.id] != nullptr)
			{
				trylog.info(L"The SystemGraph already has that system!");
				return;
			}
			// registers the system
			pSystems[S::UID.id] = std::make_unique<S>(*this);
		}

		void Finalize();
		void Execute() const;
		const gfx::IGraphics& Gfx() const;
		gfx::IGraphics& Gfx();
		auto& GetSystemManager() const
		{
			return *pManager;
		}
	private:

		SystemManager* pManager = nullptr;
		struct Level
		{
			std::vector<std::int16_t> systemIndices;
		};
		// finalized graph flag, levels it has to execute
		bool finalized = false;
		std::vector<Level> levels;
		// Systems owned, index of the array is the systemUID;
		std::vector<std::unique_ptr<System>> pSystems;
	};

	class System
	{
		friend class SystemGraph;
	public:
		System(const SystemGraph& graph);
		virtual ~System() = default;
		virtual void Execute() = 0;
		virtual void Init() = 0;

		template <typename S>
		void AddDependency()
		{
			// assert uniqueness
			for (auto& existingDependencyUID : dependencyUIDs)
			{
				if (existingDependencyUID == S::UID)
				{
					trylog.info(L"The system already has that dependency.");
					return;
				}
			}

			dependencyUIDs.push_back(S::UID);
		}
	protected:
		struct SystemUID
		{
			int id;
			static SystemUID Resolve()
			{
				static int UIDcounter = 0;
				return SystemUID{ UIDcounter++ };
			}
			bool operator==(const SystemUID& rhs) const;
		};
		ECS* pEcs = nullptr;
		const SystemGraph* pGraph = nullptr;

	private:
		std::vector<SystemUID> dependencyUIDs;
	};

	template <typename T>
	class SystemImpl : public System
	{
	public:
		SystemImpl(const SystemGraph& graph) : System(graph)
		{
			T::InitDependencies(this);	
		}
		static void InitDependencies(System* self) {}
		void Execute() override {}
		void Init() override {}

		const static inline auto UID = SystemUID::Resolve();
	};

	class SystemManager
	{
	public:
		SystemManager(ECS* pEcs);
		void ExecuteSystems() const;
		template <ValidSystem S>
		void RegisterSystem()
		{
			graph.RegisterSystem<S>();
		}
		void Finalize();
		const gfx::IGraphics& Gfx() const;
		auto GetECS() const
		{
			return pEcs;
		}
	private:
		ECS* pEcs = nullptr;
		SystemGraph graph;
	};
}