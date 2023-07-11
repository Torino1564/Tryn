#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include <string>

namespace tryn::gfx
{
	class IVertexShader : public IBindable
	{
	protected:
		std::string path;
	};
}