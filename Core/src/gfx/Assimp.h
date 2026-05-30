#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace tryn::gfx
{
	class AssimpManager
	{
	public:
		static Assimp::Importer& Get()
		{
			static Assimp::Importer singleton;
			return singleton;
		}
	};
}