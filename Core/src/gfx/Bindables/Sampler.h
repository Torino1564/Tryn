#pragma once
#include <memory>

#include "Bindable.h"

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
		static std::shared_ptr<ISampler> Resolve(const IGraphics& gfx, SamplerType type = SamplerType::Anisotropic, bool reflect = false, int slot = 0u);

		static std::string GenerateID(const IGraphics& gfx, SamplerType type, bool reflect, int slot);

	protected:
		SamplerType samplerType = SamplerType::Anisotropic;
		bool reflect = true;
		int slot = 0;
	};
}