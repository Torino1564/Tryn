#pragma once
#include <memory>
#include "Bindable.h"

namespace tryn::gfx
{
	class IRasterizer : public IBindable
	{
	public:
		static std::shared_ptr<IRasterizer> Resolve(const IGraphics& gfx, bool twoSided = true);

		static std::string GenerateID(const IGraphics& gfx, bool twoSided = true);

	protected:
		bool twoSided = false;
	};
}