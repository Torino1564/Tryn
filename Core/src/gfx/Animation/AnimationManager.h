#pragma once
#include "Animation.h"
#include <unordered_map>
#include <Core/third/dynamic_bitset.hpp>

namespace tryn::gfx::ani
{
	class AnimationManager
	{
	public:
		static AnimationManager& Get()
		{
			static AnimationManager singleton;
			return singleton;
		}
		uint32_t New(const std::string& path, struct aiAnimation& anim);
		uint32_t New(const std::string& path);
		std::shared_ptr<Animation> Resolve(const uint32_t key);
		std::shared_ptr<Animation> Resolve(const std::string& key);
	private:
		AnimationManager();
		uint32_t ResolveID();
		void Resize(const size_t newSize);
		// key is animation ID
		std::vector<std::shared_ptr<Animation>> animationPtrArray = {};
		// key management;
		uint32_t upperLimit = 1;
		uint32_t idPointer = 1;
		sul::dynamic_bitset<> IDbooker;
		std::unordered_map<std::string, uint32_t> nameMapper = {};
	};
}