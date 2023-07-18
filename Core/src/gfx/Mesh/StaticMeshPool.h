#pragma once
#include "StaticMesh.h"
#include <unordered_map>

namespace tryn::gfx
{
	class StaticMeshPool
	{
	public:
		std::shared_ptr<StaticMesh> Resolve(std::string path)
		{
			const auto it = Get().pool.find(path);
			if (it == Get().pool.end())
			{
				auto ptr = std::make_shared<StaticMesh>(path);
				std::weak_ptr<StaticMesh> weakPtr = ptr;
				Get().pool[path] = weakPtr;
				return ptr;
			}
			else
			{
				return Get().pool[path].lock();
			}
		}
		StaticMeshPool& Get()
		{
			static StaticMeshPool singleton;
			return singleton;
		}
	private:
		StaticMeshPool();
		std::unordered_map<std::string, std::weak_ptr<StaticMesh>> pool;
	};
}
