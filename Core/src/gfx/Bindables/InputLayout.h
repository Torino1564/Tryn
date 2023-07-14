#pragma once
#include <Core/src/gfx/Bindables/Bindable.h>
#include "VertexBuffer.h"
#include "VertexShader.h"
#include "PolyVBuffer.h"
#include <variant>

namespace tryn::gfx
{
	class IInputLayout : public IBindable
	{
	public: 
		static std::string GenerateID(IGraphics& gfx, IVertexBuffer& vb, IVertexShader& vs)
		{
			decltype(auto) typeStr = IGraphics::GetAPIArray()[static_cast<int>(gfx.GetType())];
			std::stringstream ss;
			ss << typeStr << "#InputLayout#FromVB:" << vb.GetTag() << '#';
			ss << "Accepts:" << vs.GetPath();
			const auto& layout = vb.Get().GetLayout();
			ss << layout.GetElementCount() << '#' << layout.GetCode();

			return ss.str();
		}
		static std::string GenerateID(IGraphics& gfx, IPolyVBuffer& pvb, IVertexShader& vs)
		{
			decltype(auto) typeStr = IGraphics::GetAPIArray()[static_cast<int>(gfx.GetType())];
			std::stringstream ss;
			ss << typeStr << "#InputLayout#FromPVB:" << pvb.GetTag() << '#';
			ss << "Accepts:" << vs.GetPath() << '#';
			int slotCount = 0;
			for (auto& buffer : pvb.slots)
			{
				const auto& layout = buffer->Get().GetLayout();
				ss << '$' << slotCount++ << '$' << layout.GetElementCount() << '#' << layout.GetCode();
			}

			return ss.str();
		}
	};
}