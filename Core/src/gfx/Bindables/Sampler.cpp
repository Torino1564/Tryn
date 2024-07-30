#include "TrynPCH.h"
#include "Sampler.h"
#include <Core/src/gfx/BindablePool.h>

namespace tryn::gfx
{
	std::shared_ptr<ISampler> ISampler::Resolve(const IGraphics& gfx, SamplerType type, bool reflect, int slot)
	{
		return BindablePool::Resolve<ISampler>(gfx, type, reflect, slot);
	}

	std::string ISampler::GenerateID(const IGraphics& gfx, SamplerType type, bool reflect, int slot)
	{
		decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
		std::string UID(typeStr);
		UID += "#Sampler#";
		using namespace std::string_literals;
		UID += "#"s + std::to_string((int)type) + (reflect ? "R"s : "W"s) + "@"s + std::to_string(slot);

		return UID;
	}
}
