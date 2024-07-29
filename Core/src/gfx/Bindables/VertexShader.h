#pragma once
#include <memory>
#include <Core/src/gfx/Bindables/Bindable.h>
#include <string>

namespace tryn::gfx
{
	class IVertexShader : public IBindable
	{
	public:
		static std::shared_ptr<IVertexShader> Resolve(const IGraphics& gfx, const std::string& path);

		static std::string GenerateID(const IGraphics& gfx, const std::string& path);

		std::string_view GetPath() const;

	protected:
		std::string path = {};
	};
}