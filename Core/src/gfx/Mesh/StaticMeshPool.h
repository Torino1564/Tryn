#pragma once
#include "StaticMesh.h"
#include <unordered_map>

namespace tryn::gfx
{
	class StaticMeshPool
	{
	public:
		static std::shared_ptr<StaticMesh> Resolve(std::string path , glm::vec3 scale = {1.0f,1.0f,1.0f})
		{
			const auto it = Get().pool.find(path);
			if (it == Get().pool.end())
			{
				auto ptr = std::make_shared<StaticMesh>(path,scale);
				std::weak_ptr<StaticMesh> weakPtr = ptr;
				Get().pool[path] = weakPtr;
				return ptr;
			}
			else
			{
				return Get().pool[path].lock();
			}
		}
		static StaticMeshPool& Get()
		{
			static StaticMeshPool singleton;
			return singleton;
		}
	private:
		StaticMeshPool() {}
		std::unordered_map<std::string, std::weak_ptr<StaticMesh>> pool;
	};
}
