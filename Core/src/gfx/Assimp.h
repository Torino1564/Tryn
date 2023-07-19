#pragma once
#include <Core/third/assimp/Importer.hpp>
#include <Core/third/assimp/scene.h>
#include <Core/third/assimp/postprocess.h>

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