#pragma once
#include "Bindable.h"
#include <Core/src/gfx/BindablePool.h>


namespace tryn::gfx
{
	enum class SamplerType
	{
		Anisotropic,
		Bilinear,
		Point,
	};

	class ISampler : public IBindable
	{
	public:
	public:
		static std::shared_ptr<ISampler> Resolve(IGraphics& gfx, SamplerType type = SamplerType::Anisotropic, bool reflect = false, int slot = 0u)
		{
			return BindablePool::Resolve<ISampler>(gfx, type, reflect, slot);
		}
		static std::string GenerateID(IGraphics& gfx, SamplerType type, bool reflect, int slot)
		{
			decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
			std::string UID(typeStr);
			UID += "#Sampler#";
			using namespace std::string_literals;
			UID += "#"s + std::to_string((int)type) + (reflect ? "R"s : "W"s) + "@"s + std::to_string(slot);

			return UID;
		}
	protected:
		SamplerType type;
		bool reflect;
		int slot;
	};
}