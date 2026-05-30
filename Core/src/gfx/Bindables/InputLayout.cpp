
#include "InputLayout.h"
#include <Core/src/gfx/BindablePool.h>
#include "VertexShader.h"

namespace tryn::gfx
{
	std::shared_ptr<IInputLayout> IInputLayout::Resolve(const IGraphics& gfx, IVertexBuffer& vb, IVertexShader& vs)
	{
		return BindablePool::Resolve<IInputLayout>(gfx, vb, vs);
	}

	std::shared_ptr<IInputLayout> IInputLayout::Resolve(const IGraphics& gfx, VertexLayout& vLayout, IVertexShader& vs)
	{
		return BindablePool::Resolve<IInputLayout>(gfx, vLayout, vs);
	}

	std::string IInputLayout::GenerateID(const IGraphics& gfx, const IVertexBuffer& vb, const IVertexShader& vs)
	{
		decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
		std::stringstream ss;
		ss << typeStr << "#InputLayout#FromVB:" << vb.GetTag() << '#';
		ss << "Accepts:" << vs.GetPath();
		const auto& layout = vb.GetLayout();
		ss << layout.GetElementCount() << '#' << layout.GetCode();

		return ss.str();
	}

	std::string IInputLayout::GenerateID(const IGraphics& gfx, const VertexLayout& vLayout, const IVertexShader& vs)
	{
		decltype(auto) typeStr = IGraphics::GetApiArray()[static_cast<int>(gfx.GetType())];
		std::stringstream ss;
		ss << typeStr << "#InputLayout#";
		ss << "Accepts:" << vs.GetPath() << "#";
		ss << vLayout.GetElementCount() << '#' << vLayout.GetCode();

		return ss.str();
	}
}
