#pragma once
#include <Core/src/gfx/IGraphics.h>
#include <Core/src/gfx/Drawable.h>
#include <Core/src/gfx/Bindables/ConstantBuffer.h>

namespace tryn::gfx
{
	class Step
	{
	public:
		void Bind(IGraphics& gfx)
		{
			for (auto& [name,vtxCbuff] : vtxCbuffers)
			{
				vtxCbuff->Bind();
			}
			for (auto& [name, pxCbuffers] : pxCbuffers)
			{
				pxCbuffers->Bind();
			}
			pVertexShader->Bind();
			pPixelShader->Bind();
		}
		void Draw(IGraphics& gfx)
		{
			gfx.DrawIndexed(parent->GetIndexCount());
		}
		std::vector<std::pair<std::string, std::shared_ptr<gfx::IVtxConstantBuffer>>> vtxCbuffers;
		std::vector<std::pair<std::string, std::shared_ptr<gfx::IPxConstantBuffer>>> pxCbuffers;
		std::shared_ptr<IVertexShader> pVertexShader;
		std::shared_ptr<IPixelShader> pPixelShader;
		Drawable* parent = nullptr;
	};
}