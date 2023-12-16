#pragma once
#include <memory>
#include <vector>
#include <concepts>
#include <Core/src/utl/Assert.h>
#include <Core/src/log/Log.h>
#include <ranges>

#define ZT_DEFINE_SYSTEM(x) class x : public tryn::ent::sys::SystemImpl<x>
#define ZT_SYSTEM_UUID public: const static inline auto UUID = tryn::ent::sys::System::SystemUID::Resolve()

namespace tryn::ent::sys
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
		void RegisterSystem(S& system)
		{
			// assert not finalized
			trynass_msg(!finalized, L"Cannot register new systems into a finalized system graph!");
			// assert minimum size
			if (pSystems.size() <= S::UUID.id)
			{
				pSystems.resize(S::UUID.id + 1);
			}
			// assert uniqueness
			if (pSystems[S::UUID.id] != nullptr)
			{
				trylog.info(L"The SystemGraph already has that system!");
				return;
			}
			// registers the system
			pSystems[S::UUID.id] = std::make_unique<S>(system);
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
		// Systems owned, index of the array is the systemUUID;
		std::vector<std::unique_ptr<System>> pSystems;
	};

	class System
	{
		friend class SystemGraph;
	public:
		virtual ~System() = default;
		virtual void Execute() = 0;
		template <typename S>
		void AddDependency()
		{
			// assert uniqueness
			for (auto& existingDependencyUID : dependencyUIDs)
			{
				if (existingDependencyUID == S::UUID)
				{
					trylog.info(L"The system already has that dependency.");
					return;
				}
			}

			dependencyUIDs.push_back(S::UUID);
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
			bool operator==(SystemUID& rhs) const
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
		virtual void Execute()
		{
			T::Execute();
		}
	};
}