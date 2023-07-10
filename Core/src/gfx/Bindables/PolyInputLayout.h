#pragma once
#include "InputLayout.h"
#include "Bindable.h"
#include "PolyVBuffer.h"
#include "VertexShader.h"
#include <memory>
#include <vector>

namespace tryn::gfx
{
	class PolyInputLayout : public IBindable
	{
	protected:
		std::vector<std::shared_ptr<IInputLayout>> slots;
	};
}