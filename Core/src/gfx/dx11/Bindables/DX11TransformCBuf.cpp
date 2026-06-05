
#include "DX11TransformCBuf.h"
#include <Core/src/gfx/Bindables/ConstantBufferResource.h>
#include <Core/src/gfx/dx11/Bindables/DX11ConstantBufferResource.h>

namespace tryn::gfx::dx11
{
	DX11TransformCBuf::DX11TransformCBuf(const Graphics& gfx)
		:
		ITransformCBuf(gfx)
	{
		if (pVCB == nullptr)
		{
			ConstantBufferLayout cblayout;
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Matrix4, "model"));
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Matrix4, "view"));
			cblayout.Append(ConstantBufferLayout::Node(ConstantBufferLayout::Type::Matrix4, "viewProjection"));
			cblayout.Solidify();
			pVCB = std::make_unique<DX11ConstantBufferResource>(gfx, std::move(cblayout), IConstantBufferResource::Type::Vertex, 0, "transformCbuf");
		}
		type = GraphicAPI::DX11;
	}
}

std::unique_ptr<tryn::gfx::IConstantBufferResource> tryn::gfx::ITransformCBuf::pVCB;