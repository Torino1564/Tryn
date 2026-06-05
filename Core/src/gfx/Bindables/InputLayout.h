#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include "SOAVertexBuffer.h"
#include <Core/src/gfx/Model/StaticMesh.h>

namespace tryn::gfx
{
	class IInputLayout : public IBindable
	{
	public:

		static std::shared_ptr<IInputLayout> Resolve(const IGraphics& gfx, IVertexBuffer& vb, IVertexShader& vs);

		static std::shared_ptr<IInputLayout> Resolve(const IGraphics& gfx, VertexLayout& vLayout, IVertexShader& vs);

		static std::string GenerateID(const IGraphics& gfx, const IVertexBuffer& vb, const IVertexShader& vs);

		static std::string GenerateID(const IGraphics& gfx, const VertexLayout& vLayout, const IVertexShader& vs);
	};
}