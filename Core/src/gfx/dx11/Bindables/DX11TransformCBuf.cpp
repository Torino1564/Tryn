#include "DX11TransformCBuf.h"
#include <Core/src/gfx/dx11/Bindables/DX11ConstantBuffer.h>

namespace tryn::gfx::dx11
{
	DX11TransformCBuf::DX11TransformCBuf(Graphics& gfx)
		:
		ITransformCBuf(gfx)
	{
		if (pVCB == nullptr)
		{
			ConstantBufferLayout cblayout;
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Matrix4, "model"));
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Matrix4, "modelView"));
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Matrix4, "modelViewProj"));
			cblayout.Solidify();
			pVCB = std::make_unique<DX11VtxConstantBuffer>(gfx, std::move(cblayout), 0, "transformCbuf");
		}
		type = GraphicAPI::DX11;
	}
}

std::unique_ptr<tryn::gfx::IVtxConstantBuffer> tryn::gfx::ITransformCBuf::pVCB;
