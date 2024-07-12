#pragma once
#include <memory>
#include <vector>
#include <concepts>
#include <Core/src/utl/Assert.h>
#include <Core/src/log/Log.h>
#include <ranges>

#define ZT_DEFINE_SYSTEM(x) class x : public tryn::ecs::sys::SystemImpl<x>
//#define ZT_SYSTEM_UID public: const static inline auto UID = tryn::ecs::sys::System::SystemUID::Resolve()
#define ZT_SYSTEM_UID public: const static inline auto UIDs = 0
#define ZT_NATIVE_ARRAY(x) private: static inline tryn::utl::MultiSpan<cmp::x::SubresourceData>

namespace tryn::ecs::sys
{
	template <typename T>
	class SystemImpl;
	class System;
	template <typename T>
	concept ValidSystem = requires
	{
		std::derived_from<T, SystemImpl<T>>;
	};

	class SystemGraph
	{
	public:
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
			pSystems[S::UID.id] = std::make_unique<S>();
		}
		void Finalize();
		void Execute();
	private:
		struct Level
		{
			std::vector<std::int16_t> systemIndices;
		};
		// finalized graph flag, levels it has to execute
		bool finalized;
		std::vector<Level> levels;
		// Systems owned, index of the array is the systemUID;
		std::vector<std::unique_ptr<System>> pSystems;
	};

	class System
	{
		friend class SystemGraph;
	public:
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
			bool operator==(const SystemUID& rhs) const
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
		};
	private:
		std::vector<SystemUID> dependencyUIDs;
	};

	template <typename T>
	class SystemImpl : public System
	{
	public:	
		virtual ~SystemImpl() = default;
		virtual void Execute() override
		{
			T::Execute();
		}
		virtual void Init() override
		{
			T::OnCreate();
		}
		static void OnCreate() {}
	public:
		const static inline auto UID = SystemUID::Resolve();
	};

	class SystemManager
	{
	public:
		static SystemManager& Get()
		{
			static SystemManager singleton;
			return singleton;
		}
		void ExecuteSystems();
		template <ValidSystem S>
		void RegisterSystem()
		{
			graph.RegisterSystem<S>();
		}
		void Finalize()
		{
			graph.Finalize();
		}
	private:
		SystemManager();
		SystemGraph graph;
	};
}