#pragma once
#include <memory>
#include <vector>
#include <concepts>
#include <Core/src/utl/Assert.h>
#include <Core/src/log/Log.h>
#include <Core/src/ecs/EcsClass.h>

#define ZT_DEFINE_SYSTEM(x) class x : public tryn::ecs::SystemImpl<x>
#define ZT_NATIVE_ARRAY(x) private: tryn::utl::MultiSpan<x>

#define TICK_INTERVAL(x) private: static constexpr auto tickInterval = x##u;

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
		requires std::derived_from<T, SystemImpl<T>>;
	};

	class ECS;

	template <ValidSystem S>
	struct SystemState {};

	class SystemGraph
	{
		friend class ECS;
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
		void Finalize2();
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
		virtual int ID() const = 0;
		virtual const std::string_view Name() const = 0;
		template <typename S>
		void AddDependency()
		{
			// assert uniqueness
			auto it = std::ranges::find(dependencyUIDs, S::UID);
			if (it != dependencyUIDs.end())
			{
				trylog.info(utl::ToWide(std::format("The system [{}] already has [{}] as a dependency.", Name(), ZT_TYPE_OF(S))));
				return;
			}
			dependencyUIDs.push_back(S::UID);
		}

		template <typename S>
		void AddPrerequisiteOf()
		{
			// assert uniqueness
			auto it = std::ranges::find(prerequisiteOfUIDs, S::UID);
			if (it != prerequisiteOfUIDs.end())
			{
				trylog.info(utl::ToWide(std::format("The system [{}] already is a prerequisite of [{}].", Name(), ZT_TYPE_OF(S))));
				return;
			}
			prerequisiteOfUIDs.push_back(S::UID);
		}
	protected:
		uint32_t Tick();
		void ResetTickCount();

		uint32_t tickCount = 0u;
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
		std::vector<SystemUID> prerequisiteOfUIDs;
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
		const std::string_view Name() const override
		{
			return name;
		}
		int ID() const override
		{
			return UID.id;
		}
		const static inline auto UID = SystemUID::Resolve();
	protected:
		static constexpr auto tickInvertval = 0u;
	private:
		static constexpr auto name = ZT_TYPE_OF(T);
	};

	class SystemManager
	{
		friend class ECS;
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